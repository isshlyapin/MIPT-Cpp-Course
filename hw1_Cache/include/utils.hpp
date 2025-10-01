#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

enum class CacheType : std::uint8_t {
    LIRS = 0,
    BELADY = 1
};

struct InputCacheData {
    size_t size_cache{0};
    size_t n_requests{0};
    std::vector<int> requests;
};

double slow_get_page(int key);
void process_input(InputCacheData& ref_data);

template<typename Cache, typename F>
size_t count_hits(Cache& cache, const std::vector<int>& requests, F get_page) {
    size_t n_hits = 0;
    for (auto key : requests) {
        if (cache.lookup_update(key, get_page)) {
            n_hits++;
        }
    }
    return n_hits;
}
