#include "mydef.hxx"
#include "config.hxx"
#include "ocl_bitonic_sort.hxx"

#include "CLI/CLI.hpp"
#include "CL/opencl.hpp"

#include <memory>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <algorithm>

int main(int argc, char **argv) try {
  std::chrono::high_resolution_clock::time_point TimeStart;
  std::chrono::high_resolution_clock::time_point TimeFin;
  cl_ulong GPUTimeStart = 0;
  cl_ulong GPUTimeFin   = 0;
  int64_t  Dur  = 0;
  uint64_t GDur = 0;

  const Config cfg = Config::read(argc, argv);
  dbgs << "Hello from bitonic sort. Config:\n" << cfg << std::endl;

  auto env = 
    std::make_shared<OCLSimpleBitonicEnv>();

  dump_bitonic_env(*env); dbgs << std::endl;

  OCLBitonicSorter<TYPE> sorter(env, cfg.lsz);

  std::vector<TYPE> v(cfg.sz);
  rand_init(v.begin(), v.end(), -1000, 1000);

  // do the sort and get events for profiling
  TimeStart = std::chrono::high_resolution_clock::now();

  const SortProfile prof = sorter.sort(v.begin(), v.end(), cl::QueueProperties::Profiling);

  TimeFin = std::chrono::high_resolution_clock::now();

  Dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
    TimeFin - TimeStart
  ).count();

  std::cout << "GPU wall time measured: " << Dur << " ns" << std::endl;

  GPUTimeStart = prof.first_ev.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  GPUTimeFin   = prof.last_ev.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  GDur = (GPUTimeFin - GPUTimeStart); // ns

  std::cout << "GPU pure time measured: " << GDur << " ns" << std::endl;

  if (!std::ranges::is_sorted(v)) {
    throw std::runtime_error("Sorting failed");
  }

#if COMPARE_CPU
  std::vector<TYPE> v2(cfg.sz);

  TimeStart = std::chrono::high_resolution_clock::now();
 
  std::ranges::sort(v2);
 
  TimeFin = std::chrono::high_resolution_clock::now();
 
  Dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
    TimeFin - TimeStart
  ).count();

  std::cout << "CPU time measured: " << Dur << " ns" << std::endl;
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