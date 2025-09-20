#include <list>
#include <utility>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <iterator>
#include<unordered_map>

template<typename KeyT>
class LirsStack;

enum class LirsType : std::uint8_t {
    LIR = 0,
    HIR = 1
};

const class LirsConfig {
public:
    double get_hot_part() const {
        return hot_part_;
    }

    size_t get_stack_part() const {
        return stack_part_;
    }
private:
    double hot_part_{0.9};
    size_t stack_part_{2};
} LIRS_CONFIG1;

template <typename PageT, typename KeyT = int>
class LirsCache {
public:
    using Entry         = typename std::pair<KeyT, PageT>; 
    using CacheList     = typename std::list<Entry>;
    using CacheListIt   = typename CacheList::iterator;
    using CacheUMap     = typename std::unordered_map<KeyT, CacheListIt>;

    LirsCache(size_t sz, double part_lir=LIRS_CONFIG1.get_hot_part()) 
      : lirsStack_(sz * LIRS_CONFIG1.get_stack_part()) {
        assert(sz > 1);
        sz_hot_ = static_cast<size_t>(sz * part_lir);
        sz_cold_ = sz - sz_hot_;

        if (sz_cold_ == 0) {
            ++sz_cold_;
            --sz_hot_;
        }

        if (sz_hot_ == 0) {
            ++sz_hot_;
            --sz_cold_;
        }
    };

    template <typename F>
    bool lookup_update(KeyT key, F get_page) {
        if (is_hit_hot(key)) {
            lirsStack_.move_to_front(key);
            lirsStack_.pruning();
            return true;
        }
        
        if (is_hit_cold(key)) {
            if (lirsStack_.contains(key)) {
                promote_to_hot(key);
            } else {
                lirsStack_.push_front(key, LirsType::HIR);
                cold_move_to_front(key);
            }
            return true;
        }

        handle_miss(key, get_page(key));
        return false;
    }

private:
    void handle_miss(KeyT key, PageT page) {
        if (hotCache_.size() < sz_hot_) {
            add_to_hot(key, page);
            return;
        }

        if (coldCache_.size() < sz_cold_) {
            add_to_cold(key, page);
            return;
        }

        evict_cold();
        if (lirsStack_.contains(key)) {
            add_to_cold(key, page);
            promote_to_hot(key);
        } else {
            add_to_cold(key, page);
        }
    }

    void hot_move_to_front(KeyT key) {
        auto hash_it = hotHash_.find(key);
        assert(hash_it != hotHash_.end());
        auto cache_it = hash_it->second;
        hotCache_.splice(hotCache_.begin(), hotCache_, cache_it);
    }

    void cold_move_to_front(KeyT key) {
        auto hash_it = coldHash_.find(key);
        assert(hash_it != coldHash_.end());
        auto cache_it = hash_it->second;
        coldCache_.splice(coldCache_.begin(), coldCache_, cache_it);
    }

    void promote_to_hot(KeyT key) {
        assert(lirsStack_.contains(key));
        lirsStack_.move_to_front(key);
        lirsStack_.update_type(key, LirsType::LIR);

        auto victim_hot = lirsStack_.bottom();
        assert(victim_hot.second == LirsType::LIR);
        KeyT victim_hot_key = victim_hot.first;
        lirsStack_.update_type(victim_hot_key, LirsType::HIR);
        
        lirsStack_.pruning();

        swap_cold_and_hot(key, victim_hot_key);
    }

    void add_to_hot(KeyT key, PageT page) {
        if (lirsStack_.contains(key)) {
            lirsStack_.move_to_front(key);
        } else {
            lirsStack_.push_front(key, LirsType::LIR);
        }
        hotCache_.push_front({key, page});
        auto [it, ok] = hotHash_.emplace(key, hotCache_.begin());
        assert(ok && "duplicate key in hotHash_");
    }

    void add_to_cold(KeyT key, PageT page) {
        if (lirsStack_.contains(key)) {
            lirsStack_.move_to_front(key);
        } else {
            lirsStack_.push_front(key, LirsType::HIR);
        }
        coldCache_.push_front({key, page});
        auto [it, ok] = coldHash_.emplace(key, coldCache_.begin());
        assert(ok && "duplicate key in coldHash_");
    }

    void evict_cold() {
        assert(!coldCache_.empty());
        coldHash_.erase(coldCache_.back().first);
        coldCache_.pop_back();
    }

    bool is_hit_cold(KeyT key) const {
        return coldHash_.contains(key);
    }

    bool is_hit_hot(KeyT key) const {
        return hotHash_.contains(key);
    }

    void swap_cold_and_hot(KeyT key_cold, KeyT key_hot) {
        auto hash_it_cold = coldHash_.find(key_cold);
        auto hash_it_hot  = hotHash_.find(key_hot);
        assert(hash_it_cold != coldHash_.end() && hash_it_hot  != hotHash_.end());
        
        // Add new entries
        hotCache_.push_front(*(hash_it_cold->second));
        {
            auto [it, ok] = hotHash_.emplace(key_cold, hotCache_.begin());
            assert(ok && "duplicate key in hotHash_");
        }

        coldCache_.push_front(*(hash_it_hot->second));
        {
            auto [it, ok] = coldHash_.emplace(key_hot, coldCache_.begin());
            assert(ok && "duplicate key in coldHash_");
        }

        // Remove old entries
        hotCache_.erase(hash_it_hot->second);
        hotHash_.erase(key_hot);

        coldCache_.erase(hash_it_cold->second);
        coldHash_.erase(key_cold);
    }

    size_t sz_hot_;
    size_t sz_cold_;

    LirsStack<KeyT> lirsStack_;

    CacheList hotCache_;
    CacheUMap hotHash_;

    CacheList coldCache_;
    CacheUMap coldHash_;
};

template <typename KeyT>
class LirsStack {
public:
    using Entry       = typename std::pair<KeyT, LirsType>; 
    using StackList   = typename std::list<Entry>;
    using StackListIt = typename StackList::iterator;
    using StackUMap   = typename std::unordered_map<KeyT, StackListIt>;

    LirsStack(size_t sz): sz_(sz) {}

    void pop_back() {
        assert(size() != 0);
        KeyT key = stack_.back().first;
        stackHash_.erase(key);
        stack_.pop_back();
    }

    size_t size() const {
        return stack_.size();
    }

    void push_front(KeyT key, LirsType type) {
        if (size() == sz_) { handle_overflow(); }

        stack_.push_front({key, type});
        auto [it, ok] = stackHash_.emplace(key, stack_.begin());
        assert(ok && "duplicate key in stack");
    }

    void handle_overflow() {
        if (bottom().second == LirsType::HIR) {
            pop_back();
        } else {
            for (auto it = stack_.rbegin(); it != stack_.rend(); ++it) {
                if (it->second == LirsType::HIR) {
                    stackHash_.erase(it->first);
                    stack_.erase(std::next(it).base());
                    break;
                }
            }
        }
    }

    bool contains(KeyT key) const {
        return stackHash_.contains(key);
    }

    void remove(KeyT key) {
        assert(contains(key));
        stack_.erase(stackHash_.find(key)->second);
        stackHash_.erase(key);
    }

    void update_type(KeyT key, LirsType newType) {
        assert(contains(key));
        auto it = stackHash_.find(key);
        it->second->second = newType;
    }

    void move_to_front(KeyT key) {
        assert(contains(key));
        auto it = stackHash_.find(key)->second;
        stack_.splice(stack_.begin(), stack_, it);
    }

    Entry top() const {
        assert(size() != 0);
        return stack_.front();
    }

    Entry bottom() const {
        assert(size() != 0);
        return stack_.back();
    }

    void pruning() {
        assert(size() != 0);
        auto lastElement = bottom();
        while (lastElement.second == LirsType::HIR) {
            pop_back();        
            assert(size() != 0);
            lastElement = bottom();
        }
    }

private:
    size_t sz_;
    StackList stack_;
    StackUMap stackHash_;
};