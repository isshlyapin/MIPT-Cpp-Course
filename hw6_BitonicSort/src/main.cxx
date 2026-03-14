#include "mydef.hxx"
#include "config.hxx"
#include "bitonic_env.hxx"
#include "bitonic_sort.hxx"

#include "CLI11.hpp"
#include "CL/opencl.hpp"

#include <chrono>
#include <vector>
#include <iostream>
#include <stdexcept>

using namespace iss::ocl;

int main(int argc, char **argv) try {
  const Config cfg = Config::read(argc, argv);
  dbgs << "Hello from bitonic sort. Config:\n" << cfg << std::endl;

  auto env = GpuEnvironment();
  dump_bitonic_env(env); 
  dbgs << std::endl;

  BitonicSorter<TYPE, GpuEnvironment> sorter(env, cfg.lsz);

  std::vector<TYPE> v;
#ifdef RANDOM_INPUT
  v.resize(cfg.sz);
  rand_init(v.begin(), v.end(), -1000, 1000);
#else
  TYPE x;
  while (std::cin >> x) { v.push_back(x); }
#endif

  const auto TimeStart = std::chrono::high_resolution_clock::now();
#if defined(GPU_SORT)
  sorter.sort(v.begin(), v.end());
#elif defined(CPU_SORT)
  std::ranges::sort(v);
#else
  static_assert(false, "Sort type should be defined");
#endif
  const auto TimeFin = std::chrono::high_resolution_clock::now();
    
#ifdef VERIFY
  std::ranges::for_each(v, [](auto x){ std::cout << x << ' '; });
  std::cout << std::endl; 
#endif

#ifdef BENCHMARK
  const auto Dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
      TimeFin - TimeStart
    ).count();
  std::cout << Dur << std::endl;
#endif
} catch (const cl::BuildError& err) {
  std::cerr << "OCL BUILD ERROR: " << err.err() << ":" << err.what() << std::endl;
  std::cerr << "-- Log --\n";
  for (const auto& e : err.getBuildLog()) { std::cerr << e.second; }
  std::cerr << "-- End log --\n";
  return -1;
} catch (const cl::Error& err) {
  std::cerr << "OCL ERROR: " << err.err() << ":" << err.what() << std::endl;
  return -1;
} catch (const CLI::CallForHelp& err) {
  return -1;
} catch (const CLI::CallForAllHelp& err) {
  return -1;
} catch (const CLI::ParseError& err) {
  std::cerr << "INVALID OPTION: " << err.what() << std::endl;
  return -1;
} catch (const std::overflow_error& err) {
  std::cerr << "OVERFLOW ERROR: " << err.what() << std::endl;
  return -1;
} catch (const std::runtime_error& err) {
  std::cerr << "RUNTIME ERROR: " << err.what() << std::endl;
  return -1;
} catch (...) {
  std::cerr << "UNKNOWN ERROR\n";
  return -1;
}