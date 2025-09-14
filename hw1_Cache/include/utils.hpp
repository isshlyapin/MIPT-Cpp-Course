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
