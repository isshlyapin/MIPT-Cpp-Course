#pragma once

#include <list>
#include <utility>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace caches {

template<typename KeyT>
class LirsStack;

template <typename PageT, typename KeyT = int>
class LirsCache;

enum class LirsType : std::uint8_t {
    LIR = 0,
    HIR = 1
};

template <typename PageT, typename KeyT>
class LirsCache {
public:
    using Entry         = typename std::pair<KeyT, PageT>; 
    using CacheList     = typename std::list<Entry>;
    using CacheListIt   = typename CacheList::iterator;
    using CacheUMap     = typename std::unordered_map<KeyT, CacheListIt>;

    explicit LirsCache(size_t sz) : lirsStack_(sz * stack_coeff_) {
        if (sz <= 1) {
            throw std::invalid_argument("Cache size must be greater than 1");
        }
        sz_hot_ = static_cast<size_t>(sz * hot_part_);
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
                move_to_front(coldCache_, coldHash_, key);
            }
            return true;
        }

        handle_miss(key, get_page(key));
        return false;
    }

private:
    void handle_miss(KeyT key, PageT page) {
        if (hotCache_.size() < sz_hot_) {
            add_to_cache(hotCache_, hotHash_, key, page, LirsType::LIR);
            return;
        }

        if (coldCache_.size() < sz_cold_) {
            add_to_cache(coldCache_, coldHash_, key, page, LirsType::HIR);
            return;
        }

        evict_cold();
        if (lirsStack_.contains(key)) {
            add_to_cache(coldCache_, coldHash_, key, page, LirsType::HIR);
            promote_to_hot(key);
        } else {
            add_to_cache(coldCache_, coldHash_, key, page, LirsType::HIR);
        }
    }

    void move_to_front(CacheList& cache, CacheUMap& hash_map, KeyT key) {
        auto hash_it = hash_map.find(key);
        if (hash_it == hash_map.end()) {
            throw std::invalid_argument("Key not found in cache");
        }
        auto cache_it = hash_it->second;
        cache.splice(cache.begin(), cache, cache_it);
    }

    void promote_to_hot(KeyT key) {
        if (!lirsStack_.contains(key)) {
            throw std::logic_error("Key not found in LIRS stack during promotion");
        }
        lirsStack_.move_to_front(key);
        lirsStack_.update_type(key, LirsType::LIR);

        auto victim_hot = lirsStack_.bottom();
        if (victim_hot.second != LirsType::LIR) {
            throw std::logic_error("Bottom of stack is not LIR during promotion");
        }
        KeyT victim_hot_key = victim_hot.first;
        lirsStack_.update_type(victim_hot_key, LirsType::HIR);
        
        lirsStack_.pruning();

        swap_cold_and_hot(key, victim_hot_key);
    }

    void add_to_cache(CacheList& cache, CacheUMap& hash_map, KeyT key, PageT page, LirsType type) {
        if (lirsStack_.contains(key)) {
            lirsStack_.move_to_front(key);
        } else {
            lirsStack_.push_front(key, type);
        }
        cache.push_front({key, page});
        auto [it, ok] = hash_map.emplace(key, cache.begin());
        if (!ok) { throw std::logic_error("Duplicate key in cache hash_map"); }
    }

    void evict_cold() {
        if (coldCache_.empty()) {
            throw std::runtime_error("Cannot evict from empty cold cache");
        }
        coldHash_.erase(coldCache_.back().first);
        coldCache_.pop_back();
    }

    bool is_hit_cold(KeyT key) const {
        return coldHash_.contains(key);
    }

    bool is_hit_hot(KeyT key) const {
        return hotHash_.contains(key);
    }

    void move_from_to(CacheList& from_cache, CacheUMap& from_hash, CacheList& to_cache, CacheUMap& to_hash, KeyT key) {
        auto hash_it = from_hash.find(key);
        if (hash_it == from_hash.end()) {
            throw std::invalid_argument("Key not found in source cache");
        }
        to_cache.push_front(*(hash_it->second));
        auto [it, ok] = to_hash.emplace(key, to_cache.begin());
        if (!ok) { throw std::logic_error("Duplicate key in destination cache"); }
        from_cache.erase(hash_it->second);
        from_hash.erase(key);
    }

    void swap_cold_and_hot(KeyT key_cold, KeyT key_hot) {
        move_from_to(coldCache_, coldHash_, hotCache_, hotHash_, key_cold);
        move_from_to(hotCache_, hotHash_, coldCache_, coldHash_, key_hot);
    }

    double hot_part_{0.9};
    size_t stack_coeff_{3};

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

    explicit LirsStack(size_t sz): sz_(sz) {}

    void pop_back() {
        if (size() == 0) {
            throw std::runtime_error("Cannot pop from empty stack");
        }
        KeyT key = stack_.back().first;
        erase_entry(key);
    }

    size_t size() const {
        return stack_.size();
    }

    void push_front(KeyT key, LirsType type) {
        if (size() == sz_) { handle_overflow(); }
        push_front_entry(key, type);
    }

    
    bool contains(KeyT key) const {
        return stackHash_.contains(key);
    }
    
    void remove(KeyT key) {
        if (!contains(key)) {
            throw std::invalid_argument("Key not found in stack");
        }
        erase_entry(key);
    }
    
    void update_type(KeyT key, LirsType newType) {
        if (!contains(key)) {
            throw std::invalid_argument("Key not found in stack");
        }
        auto it = stackHash_.find(key);
        it->second->second = newType;
    }
    
    void move_to_front(KeyT key) {
        if (!contains(key)) {
            throw std::invalid_argument("Key not found in stack");
        }
        auto it = stackHash_.find(key)->second;
        stack_.splice(stack_.begin(), stack_, it);
    }
    
    Entry top() const {
        if (size() == 0) {
            throw std::runtime_error("Cannot get top of empty stack");
        }
        return stack_.front();
    }
    
    Entry bottom() const {
        if (size() == 0) {
            throw std::runtime_error("Cannot get bottom of empty stack");
        }
        return stack_.back();
    }
    
    void pruning() {
        if (size() == 0) {
            throw std::runtime_error("Cannot prune empty stack");
        }
        auto lastElement = bottom();
        while (lastElement.second == LirsType::HIR) {
            pop_back();        
            if (size() == 0) {
                throw std::runtime_error("Stack became empty during pruning - invalid state");
            }
            lastElement = bottom();
        }
    }
    
    private:
    void erase_entry(KeyT key) {
        auto hash_it = stackHash_.find(key);
        if (hash_it == stackHash_.end()) {
            throw std::invalid_argument("Key not found in stack during erase");
        }
        stack_.erase(hash_it->second);
        stackHash_.erase(hash_it);
    }
    
    void push_front_entry(KeyT key, LirsType type) {
        stack_.push_front({key, type});
        auto [it, ok] = stackHash_.emplace(key, stack_.begin());
        if (!ok) { throw std::logic_error("Duplicate key in stack"); }
    }

    void handle_overflow() {        
        auto it = std::ranges::find_if(stack_.rbegin(), stack_.rend(), 
            [](auto x) { return x.second == LirsType::HIR; });
        stackHash_.erase(it->first);
        stack_.erase(std::next(it).base());
    }

    size_t sz_;
    StackList stack_;
    StackUMap stackHash_;
};

}  // namespace caches
