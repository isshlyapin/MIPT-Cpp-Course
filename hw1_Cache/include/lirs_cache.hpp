#pragma once

#include <list>
#include <utility>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace caches {

enum class LirsType : std::uint8_t {
    LIR = 0,
    HIR = 1
};

}

namespace detail {

using caches::LirsType;

template <typename KeyT>
class LirsStack {
public:
    using Entry       = typename std::pair<KeyT, LirsType>; 
    using StackList   = typename std::list<Entry>;
    using StackListIt = typename StackList::iterator;
    using StackUMap   = typename std::unordered_map<KeyT, StackListIt>;

    explicit LirsStack(size_t sz): sz_(sz) {}

    void push(KeyT key, LirsType type = LirsType::HIR) {
        if (size() == sz_) { handle_overflow(); }
        push_entry(key, type);
    }

    void pop() {
        assert(size());
        erase_entry(stack_.back().first);
        pruning();
    }

    size_t size() const {
        return stack_.size();
    }
    
    bool contains(KeyT key) const {
        return stackHash_.contains(key);
    }
    
    Entry bottom() const {
        assert(size());
        return stack_.back();
    }
    
private:
    void pruning() {
        assert(size() && "Prune empty stack");
        auto lastElement = bottom();
        while (lastElement.second == LirsType::HIR) {
            erase_entry(stack_.back().first);

            assert(size() && "Absence of LIR entry in the stack");
            
            lastElement = bottom();
        }
    }
    
    void push_entry(KeyT key, LirsType type) {
        auto hash_it = stackHash_.find(key);
        if (hash_it != stackHash_.end()) {
            stack_.erase(hash_it->second);
            type = LirsType::LIR;
        }
        stack_.emplace_front(key, type);
        stackHash_[key] = stack_.begin();
        pruning();
    }

    void handle_overflow() {
        auto it = std::ranges::find_if(stack_.rbegin(), stack_.rend(), 
            [](auto x) { return x.second == LirsType::HIR; });

        assert((it != stack_.rend()) && "The stack size is less than the LIRS part in the cache");

        stackHash_.erase(it->first);
        stack_.erase(std::next(it).base());
    }

    void erase_entry(KeyT key) {
        auto hash_it = stackHash_.find(key);

        assert((hash_it != stackHash_.end()) && "Key not found in stack during erase");

        stack_.erase(hash_it->second);
        stackHash_.erase(hash_it);
    }
    
    size_t sz_;
    StackList stack_;
    StackUMap stackHash_;
};

} //namespace detail

namespace caches {

template <typename PageT, typename KeyT = int>
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
            lirsStack_.push(key, LirsType::LIR);
            return true;
        }
        
        if (is_hit_cold(key)) {
            if (lirsStack_.contains(key)) {
                lirsStack_.push(key, LirsType::LIR);
                promote_to_hot(key);
            } else {
                lirsStack_.push(key);
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
            lirsStack_.push(key, LirsType::LIR);
            add_to_cache(hotCache_, hotHash_, key, page);
            return;
        }

        if (coldCache_.size() < sz_cold_) {
            lirsStack_.push(key);
            add_to_cache(coldCache_, coldHash_, key, page);
            return;
        }

        evict_cold();
        
        if (lirsStack_.contains(key)) {
            lirsStack_.push(key, LirsType::LIR);
            add_to_cache(coldCache_, coldHash_, key, page);
            promote_to_hot(key);
        } else {
            lirsStack_.push(key);
            add_to_cache(coldCache_, coldHash_, key, page);
        }
    }

    void move_to_front(CacheList& cache, CacheUMap& hash_map, KeyT key) {
        auto hash_it = hash_map.find(key);
        assert(hash_it != hash_map.end());

        cache.splice(cache.begin(), cache, hash_it->second);
    }

    void promote_to_hot(KeyT key) {
        KeyT victim_key = lirsStack_.bottom().first;
        
        lirsStack_.pop();

        swap_cold_and_hot(key, victim_key);
    }

    void add_to_cache(CacheList& cache, CacheUMap& hash_map, KeyT key, PageT page) {
        cache.emplace_front(key, page);

        auto [it, ok] = hash_map.emplace(key, cache.begin());
        assert(ok);
    }

    void evict_cold() {
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
        assert(hash_it != from_hash.end());

        to_cache.splice(to_cache.begin(), from_cache, hash_it->second);
        auto [it, ok] = to_hash.emplace(key, to_cache.begin());
        assert(ok);
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

    detail::LirsStack<KeyT> lirsStack_;

    CacheList hotCache_;
    CacheUMap hotHash_;

    CacheList coldCache_;
    CacheUMap coldHash_;
};

}  // namespace caches
