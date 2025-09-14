#include<iostream>
#include <string_view>

#include "utils.hpp"
#include "lirs_cache.hpp"
#include "belady_cache.hpp"

int main(int argc, char** argv) {
    assert(argc > 1 && "Proper launch ./cache belady/lirs");

    const std::string_view cache_type(argv[1]);

    InputCacheData data1;
    process_input(data1);

    size_t n_hits = 0;

    if (cache_type == "lirs") {
        LirsCache<double> cache(data1.size_cache);

        for (auto key : data1.requests) {
            if (cache.lookup_update(key, slow_get_page)) {
                n_hits++;
            }
        }

    } else if (cache_type == "belady") {
        BeladyCache<double> cache(data1.size_cache, data1.requests);

        for (auto key : data1.requests) {
            if (cache.lookup_update(key, slow_get_page)) {
                n_hits++;
            }
        }

    } else {
        std::cerr << "Unknown cache type: " << cache_type << "\n";
        return 1;
    }

    std::cout << n_hits << std::endl;
}
