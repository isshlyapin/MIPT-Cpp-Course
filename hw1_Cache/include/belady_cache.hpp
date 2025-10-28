#pragma once

#include <map>
#include <vector>
#include <cstddef>
#include <utility>
#include <cassert>
#include <iterator>
#include <unordered_map>

namespace detail {
struct NextAccess {
    bool exists;
    size_t index;

    bool operator<(const NextAccess& other) const {
        if (!exists && !other.exists) { return exists; }
        if (exists != other.exists)   { return exists; }
        return index < other.index;
    }
};
}  // namespace detail

namespace caches {

template <typename PageT, typename KeyT = int>
class BeladyCache {
public:
    using Entry      = typename std::pair<KeyT, PageT>;
    using CacheMap   = typename std::multimap<detail::NextAccess, Entry>; 
    using CacheMapIt = typename CacheMap::iterator;
    using CacheUMap  = typename std::unordered_map<KeyT, CacheMapIt>;

    BeladyCache(size_t sz, const std::vector<KeyT>& future_requests) : sz_(sz) {
        if (sz_ <= 0) {
          throw std::invalid_argument("Cache size must be greater than 0");
        }
        
        for (ssize_t i = future_requests.size() - 1; i >= 0; --i) {
            requests_data_[future_requests[i]].push_back(i);
        }
    }

    template<typename F>
    bool lookup_update(KeyT key, F get_page) {
        auto hash_it = hash_.find(key);
        if (hash_it != hash_.end()) {
            requests_data_[key].pop_back();
            update_relevance(hash_it->first, hash_it->second);
            return true;
        }
        handle_miss(key, get_page);
        return false;
    }

private:
    template<typename F>
    void handle_miss(KeyT key, F get_page) {
        requests_data_[key].pop_back();
        if (cache_.size() < sz_) {
            add_page(key, get_page(key));
        } else {
            detail::NextAccess request_na{.exists=false, .index=0};
            if (!requests_data_[key].empty()) {
                request_na.exists = true;
                request_na.index  = requests_data_[key].back();
            }

            KeyT excess_key = choose_excess_page(request_na, key);
            if (key != excess_key) {
                remove_page(excess_key);
                add_page(key, get_page(key));
            }
        }
    }

    void update_key_in_cache(CacheMapIt map_it, detail::NextAccess next_access) {
        auto extract_node = cache_.extract(map_it);
        extract_node.key() = next_access;
        cache_.insert(std::move(extract_node));
    }

    void update_relevance(KeyT key, CacheMapIt map_it) {
        if (requests_data_[key].empty()) {
            update_key_in_cache(map_it, detail::NextAccess{.exists=false, .index=0});
        } else {
            update_key_in_cache(map_it, detail::NextAccess{true, requests_data_[key].back()});
        }
    }

    void add_page(KeyT key, PageT page) {
        detail::NextAccess na{.exists=false, .index=0};
        if (!requests_data_[key].empty()) {
            na.index  = requests_data_[key].back();
            na.exists = true;
        }

        CacheMapIt cache_it = cache_.insert({na, Entry{key, page}});
        auto [it, ok] = hash_.emplace(key, cache_it);

        if (!ok) { throw std::logic_error("duplicate in hash_"); }
    }

    KeyT choose_excess_page(detail::NextAccess request, KeyT current_key) {
        if (cache_.empty()) { throw std::logic_error("Choose excess page in empty cache"); }

        auto cache_it = std::prev(cache_.end());        
        if (cache_it->first < request) {
            return current_key;
        } 
        
        return cache_it->second.first;
    }

    void remove_page(KeyT key) {
        if (!hash_.contains(key)) { throw std::logic_error("Attempt to remove non-existing key"); }
        auto hash_it = hash_.find(key);
        cache_.erase(hash_it->second);
        hash_.erase(key);
    }

    size_t sz_;
    CacheMap cache_;
    CacheUMap hash_;
    std::unordered_map<KeyT, std::vector<size_t>> requests_data_;
};

} // namespace caches