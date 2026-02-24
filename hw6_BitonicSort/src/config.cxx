#include "config.hxx"

#include "CLI11.hpp"

#include <iostream>
#include <string_view>

inline constexpr int CFG_DEFAULT_LOCAL_SIZE = 256;
inline constexpr int CFG_DEFAULT_ARRAY_SIZE = 1024;
inline constexpr std::string_view CFG_DEFAULT_KERNEL_PATH = "src/fast_bitonic_sort.cl";

// dump config to stream
void Config::dump(std::ostream& os) const {
  os << "size = [" << sz << "]\n";
  os << "local size = [" << lsz << "]\n";
}

// read config from command line
Config Config::read(int argc, char **argv) {
  CLI::App app{"Bitonic Sort"};

  int sz{CFG_DEFAULT_ARRAY_SIZE};
  int lsz{CFG_DEFAULT_LOCAL_SIZE};

  app.add_option("--size", sz, "number of elements in the array");
  app.add_option("--lsize", lsz, "the desired size of local memory GPU");

  app.parse(argc, argv);

  return {.sz = sz,.lsz = lsz};
}