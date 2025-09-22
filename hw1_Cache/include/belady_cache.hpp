#pragma once

#include <cassert>
#include <map>
#include <vector>
#include <cstddef>
#include <utility>
#include <iterator>
#include <unordered_map>

namespace {
struct NextAccess {
    size_t index;
    bool exists;

    bool operator<(const NextAccess& other) const {
        if (exists != other.exists) {
            return exists; // "не существует" считается больше
        }
        return index < other.index;
    }
};
}

template <typename PageT, typename KeyT = int>
class BeladyCache {
public:
    using Entry      = typename std::pair<KeyT, PageT>;
    using CacheMap   = typename std::multimap<NextAccess, Entry>; 
    using CacheMapIt = typename CacheMap::iterator;
    using CacheUMap  = typename std::unordered_map<KeyT, CacheMapIt>;

    BeladyCache(size_t sz, const std::vector<KeyT>& future_requests) : sz_(sz) {
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
            NextAccess request_na{.index=0, .exists=false};
            if (!requests_data_[key].empty()) {
                request_na.index = requests_data_[key].back();
                request_na.exists = true;
            }

            KeyT excess_key = choose_excess_page(request_na, key);
            if (key != excess_key) {
                remove_page(excess_key);
                add_page(key, get_page(key));
            }
        }
    }

    void update_key_in_cache(CacheMapIt map_it, NextAccess next_access) {
        auto extract_node = cache_.extract(map_it);
        extract_node.key() = next_access;
        cache_.insert(std::move(extract_node));
    }

    void update_relevance(KeyT key, CacheMapIt map_it) {
        if (requests_data_[key].empty()) {
            update_key_in_cache(map_it, NextAccess{.index=0, .exists=false});
        } else {
            update_key_in_cache(map_it, NextAccess{requests_data_[key].back(), true});
        }
    }

    void add_page(KeyT key, PageT page) {
        NextAccess na{.index=0, .exists=false};
        if (!requests_data_[key].empty()) {
            na.index  = requests_data_[key].back();
            na.exists = true;
        }

        CacheMapIt cache_it = cache_.insert({na, Entry{key, page}});
        auto [it, ok] = hash_.emplace(key, cache_it);
        assert(ok && "duplicate in hash_");
    }

    KeyT choose_excess_page(NextAccess request, KeyT current_key) {
        assert(!cache_.empty());
        auto cache_it = std::prev(cache_.end());
        
        if (cache_it->first < request) {
            return current_key;
        } 
        
        return cache_it->second.first;
    }

    void remove_page(KeyT key) {
        auto hash_it = hash_.find(key);
        assert(hash_it != hash_.end());
        cache_.erase(hash_it->second);
        hash_.erase(key);
    }

    size_t sz_;
    CacheMap cache_;
    CacheUMap hash_;
    std::unordered_map<KeyT, std::vector<size_t>> requests_data_;
};