#include <cmath>
#include <vector>
#include <cstddef>
#include <iostream>

#include "utils.hpp"

double slow_get_page(int key) {
    return std::sin(key);
}

void process_input(InputCacheData& ref_data) {
    std::cin >> ref_data.size_cache;
    std::cin >> ref_data.n_requests;
    int x = 0;

    for(size_t i = 0; i < ref_data.n_requests; ++i) {
        std::cin >> x;
        ref_data.requests.push_back(x);
    }
}
