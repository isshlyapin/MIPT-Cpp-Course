#include <string>
#include <iostream>

#include "CLI/CLI.hpp"

#include "utils.hpp"
#include "lirs_cache.hpp"
#include "belady_cache.hpp"

int main(int argc, char** argv) {
    CLI::App app{"Simulator caches"};
    argv = app.ensure_utf8(argv);

    std::string cache_type;
    app.add_option("-t,--type", cache_type, "lirs/belady")
        ->required()
        ->check(CLI::IsMember({"lirs", "belady"}));

    CLI11_PARSE(app, argc, argv);

    utils::InputCacheData data;
    try {
        utils::process_input(data);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Input error: " << e.what() << std::endl;
        return 1;
    }

    try {
        size_t n_hits = 0;
        if (cache_type == "lirs") {
            caches::LirsCache<double> cache(data.size_cache);
            n_hits = utils::count_hits(cache, data.requests, utils::slow_get_page);
        } else if (cache_type == "belady") {
            caches::BeladyCache<double> cache(data.size_cache, data.requests);
            n_hits = utils::count_hits(cache, data.requests, utils::slow_get_page);        
        }
        std::cout << n_hits << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Cache error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
