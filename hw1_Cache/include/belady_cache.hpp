#include <map>
#include <list>
#include <sys/types.h>
#include <vector>
#include <cstddef>
#include <utility>
#include <iterator>
#include <unordered_map>

template <typename PageT, typename KeyT = int>
class BeladyCache {
public:
    using CacheList = typename std::list<std::pair<KeyT, PageT>>;
    using CacheIt   = typename CacheList::iterator;
    using CacheMap  = typename std::map<size_t, CacheIt>; 
    using CacheMapIt = typename CacheMap::iterator;

    BeladyCache(size_t sz, const std::vector<KeyT>& future_requests) : sz_(sz) {
        for (ssize_t i = future_requests.size() - 1; i >= 0; --i) {
            requests_data_[future_requests[i]].push_back(i);
        }
    }

    void update_relevance(CacheMapIt map_it) {
        auto cache_it = map_it->second;
        KeyT key = cache_it->first;

        relevance_map_.erase(map_it);

        requests_data_[key].pop_back();
        if (requests_data_[key].empty()) {
            dead_keys_.push_back(key);
            availability_map_[key] = {relevance_map_.end(), cache_it};
        } else {
            auto res_ins = relevance_map_.insert({requests_data_[key].back(), cache_it});
            assert(res_ins.second);
            availability_map_[key] = {res_ins.first, cache_it};
        }
    }

    template<typename F>
    void add_page(KeyT key, F get_page) {
        requests_data_[key].pop_back();
        cache_.push_front({key, get_page(key)});
        auto relevance_it = relevance_map_.end();
        if (requests_data_[key].empty()) {
            dead_keys_.push_back(key);
        } else {
            auto res_ins = relevance_map_.insert({requests_data_[key].back(), cache_.begin()});
            assert(res_ins.second);
            relevance_it = res_ins.first;
        }
        availability_map_[key] = {relevance_it, cache_.begin()};
    }

    KeyT find_excess_page() {
        KeyT excess_key;
        auto rel_it = std::prev(relevance_map_.end());
        excess_key = rel_it->second->first;

        return excess_key;
    }

    void remove_excess_page() {
        KeyT excess_key;
        if (dead_keys_.empty()) {
            excess_key = find_excess_page();
        } else {
            excess_key = dead_keys_.back();
            dead_keys_.pop_back();
        }
        auto aval_it = availability_map_.find(excess_key);
        if (aval_it->second.first != relevance_map_.end()) {
            relevance_map_.erase(aval_it->second.first);
        }
        cache_.erase(aval_it->second.second);
        availability_map_.erase(aval_it);        
    }

    template<typename F>
    bool lookup_update(KeyT key, F get_page) {
        bool result = false;
        auto aval_it = availability_map_.find(key);
        if (aval_it != availability_map_.end()) {
            result = true;
            update_relevance(aval_it->second.first);
        } else {
            result = false;
            if (cache_.size() == sz_) {
                remove_excess_page();
            }
            add_page(key, get_page);
        }
        return result;
    }

private:
    size_t sz_;
    CacheMap relevance_map_;
    CacheList cache_;
    std::vector<KeyT> dead_keys_;
    std::unordered_map<KeyT, std::pair<CacheMapIt, CacheIt>> availability_map_;
    std::unordered_map<KeyT, std::vector<size_t>> requests_data_;
};