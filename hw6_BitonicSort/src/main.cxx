#include "mydef.hxx"
#include "config.hxx"
#include "ocl_bitonic_sort.hxx"

#include "CLI11.hpp"
#include "CL/opencl.hpp"

#include <memory>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace iss::ocl;

int main(int argc, char **argv) try {
  const Config cfg = Config::read(argc, argv);
  dbgs << "Hello from bitonic sort. Config:\n" << cfg << std::endl;

  auto env = 
    std::make_shared<OCLSimpleBitonicEnv>();

  dump_bitonic_env(*env); 
  dbgs << std::endl;

  OCLBitonicSorter<TYPE> sorter(env, cfg.lsz);

  std::vector<TYPE> v;
#ifdef RANDOM_INPUT
  v.resize(cfg.sz);
  rand_init(v.begin(), v.end(), -1000, 1000);
#else
  TYPE x;
  while (std::cin >> x) { v.push_back(x); }
#endif

  std::chrono::high_resolution_clock::time_point TimeStart;
  std::chrono::high_resolution_clock::time_point TimeFin;

  TimeStart = std::chrono::high_resolution_clock::now();
#if defined(GPU_SORT)
  sorter.sort(v.begin(), v.end());
#elif defined(CPU_SORT)
  std::ranges::sort(v);
#else
  static_assert(false, "Sort type should be defined");
#endif
  TimeFin = std::chrono::high_resolution_clock::now();
    
#ifdef VERIFY
  for (auto x : v) { std::cout << x << " "; }
  std::cout << std::endl; 
#endif

#ifdef BENCHMARK
  const auto Dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
      TimeFin - TimeStart
    ).count();
  std::cout << Dur << std::endl;
#endif
} catch (const cl::BuildError &err) {
  std::cerr << "OCL BUILD ERROR: " << err.err() << ":" << err.what() << std::endl;
  std::cerr << "-- Log --\n";
  for (const auto& e : err.getBuildLog()) { std::cerr << e.second; }
  std::cerr << "-- End log --\n";
  return -1;
} catch (const cl::Error &err) {
  std::cerr << "OCL ERROR: " << err.err() << ":" << err.what() << std::endl;
  return -1;
} catch (const CLI::ParseError &err) {
  std::cerr << "INVALID OPTION: " << err.what() << std::endl;
  return -1;
} catch (const std::runtime_error &err) {
  std::cerr << "RUNTIME ERROR: " << err.what() << std::endl;
  return -1;
} catch (...) {
  std::cerr << "UNKNOWN ERROR\n";
  return -1;
}