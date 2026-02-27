#include "config.hxx"

#include "CLI11.hpp"

#include <iostream>

inline constexpr int CFG_DEFAULT_LOCAL_SIZE = 256;
inline constexpr int CFG_DEFAULT_ARRAY_SIZE = 1024;

// dump config to stream
void Config::dump(std::ostream& os) const {
  os << "size = [" << sz << "]\n";
  os << "local size = [" << lsz << "]\n";
}

// read config from command line
Config Config::read(int argc, char **argv) {
  CLI::App app{"Bitonic Sort"};

  int sz{0};
  int lsz{0};

  app.add_option("--size", sz, "number of elements in the array")
    ->check(CLI::PositiveNumber)->default_val(CFG_DEFAULT_ARRAY_SIZE);

  app.add_option("--lsize", lsz, "local work-group size (LSZ)")
    ->check(CLI::PositiveNumber)->default_val(CFG_DEFAULT_LOCAL_SIZE);

  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp& e) {
    std::cout << app.help() << std::endl;
    throw;
  } catch (const CLI::CallForAllHelp& e) {
    std::cout << app.help("", CLI::AppFormatMode::All) << std::endl;
    throw;
  }

  return {.sz = sz,.lsz = lsz};
}