#pragma once

#include <iostream>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_CL_1_2_DEFAULT_BUILD

#ifndef COMPARE_CPU
#define COMPARE_CPU 1
#endif

// Config for program: we can also read it from options
struct Config {
  int sz;
  int lsz;
  
  void dump(std::ostream& os) const;
  static Config read(int argc, char **argv);
};

static std::ostream& operator<<(std::ostream& os, const Config& cfg) {
  cfg.dump(os);
  return os;
}
