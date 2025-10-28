#pragma once

#include <cmath>
#include <vector>
#include <cstddef>
#include <iostream>

namespace utils {

struct InputCacheData {
    size_t size_cache{0};
    size_t n_requests{0};
    std::vector<int> requests;
};

static double slow_get_page(int key);
static void process_input(InputCacheData& ref_data);

static double slow_get_page(int key) {
    return std::sin(key);
}

static void process_input(InputCacheData& ref_data) {    
    if (!(std::cin >> ref_data.size_cache)) {
        throw std::invalid_argument("Incorrect cache size");
    }
    if (!(std::cin >> ref_data.n_requests)) {
        throw std::invalid_argument("Incorrect number of requests");
    }

    int x = 0;  
    for(size_t i = 0; i < ref_data.n_requests; ++i) {
        if (!(std::cin >> x)) {
            throw std::invalid_argument("Invalid request value");
        }
        ref_data.requests.push_back(x);
    }
}

template<typename Cache, typename F>
static size_t count_hits(Cache& cache, const std::vector<int>& requests, F get_page) {
    size_t n_hits = 0;
    for (auto key : requests) {
        if (cache.lookup_update(key, get_page)) {
            n_hits++;
        }    
    }    
    return n_hits;
}

} // namespace utils
