#include <string>
#include <iostream>

#include "utils.hpp"
#include "lirs_cache.hpp"
#include "belady_cache.hpp"

#include "CLI/CLI.hpp"

int main(int argc, char** argv) {
    CLI::App app{"Simulator caches"};
    argv = app.ensure_utf8(argv);

    std::string cache_type;
    app.add_option("-t,--type", cache_type, "lirs/belady")
        ->required()
        ->check(CLI::IsMember({"lirs", "belady"}));

    CLI11_PARSE(app, argc, argv);

    InputCacheData data;
    process_input(data);

    size_t n_hits = 0;
    if (cache_type == "lirs") {
        LirsCache<double> cache(data.size_cache);
        n_hits = count_hits(cache, data.requests, slow_get_page);
    } else if (cache_type == "belady") {
        BeladyCache<double> cache(data.size_cache, data.requests);
        n_hits = count_hits(cache, data.requests, slow_get_page);        
    } else {
        std::cerr << "Unknown type cache" << std::endl;
        return 1;
    }

    std::cout << n_hits << std::endl;
    return 0;
}
