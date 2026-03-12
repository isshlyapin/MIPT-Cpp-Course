#pragma once

#include "types.hxx"
#include "mydef.hxx"
#include "config.hxx"
#include "helpers.hxx"
#include "kernel_source.hxx"
#include "ocl_bitonic_env.hxx"

#include "CL/opencl.hpp"

#include <memory>
#include <limits>
#include <cstddef>
#include <concepts>
#include <iterator>
#include <stdexcept>
#include <string_view>

namespace iss::ocl {

template<typename T, BitonicEnv Env>
class OCLBitonicSorter {
public:
  OCLBitonicSorter(const Env& env, size_t lsz)
    : env_(env), lsz_(validate_lsz(lsz)),
      program_(buildProgram()),
      gsort_kernel_(program_, "global_bitonic_sort"), 
      lsort_kernel_(program_, "local_bitonic_sort") {
    dbgs << "Create OCLBitonicSorter:" << "\n";
    dbgs << "\tSelected type: "        << ocl_type_name<T>::value << "\n";
    dbgs << "\tSelected local size: "  << lsz_                    << "\n";
  }

  template<std::contiguous_iterator It>
  requires std::same_as<std::iter_value_t<It>, T>
  void sort(It start, It end) {
    const size_t size = end - start;
    if (size < 2) { return; }

    const auto& context = env_->get_context();
    cl::CommandQueue queue(context);

    const size_t source_bytes = size * sizeof(T);

    auto [buffer, aligned_size] = 
      create_aligned_buffer(size, context);

    const size_t local_size = 
      largest_divisor_leq_limit(aligned_size, lsz_);

    dbgs << "\nSorting sequence\n"
          << "\tsize: "                 << size         << "\n"
          << "\taligned size: "         << aligned_size << "\n"
          << "\tglobal local size: "    << lsz_         << "\n"
          << "\teffective local size: " << local_size   << "\n\n";

    fill_buffer(queue, buffer, size, std::to_address(start));
    pad_buffer(queue, buffer, size, aligned_size);
    
    sort_pow2(queue, buffer, aligned_size, local_size);

    read_buffer(queue, buffer, size, std::to_address(start));
  }

private:
  size_t validate_lsz(size_t lsz) const {
    if (lsz <= 1 || (lsz * sizeof(T)) > env_.get_local_mem_size()) {
      throw std::runtime_error("Invalid local size: " + std::to_string(lsz));
    }
    return lsz;
  }

  cl::Program buildProgram() const {
    std::string opts = 
      std::string("-DTYPE=") + std::string(ocl_type_name<T>::value) + " " +
      std::string("-DLSZ=")  + std::to_string(lsz_);
    
    cl::Program program(env_.get_context(), ocl_kernels::BITONIC_SORT_CL);
    program.build(opts.c_str());
    
    return program;
  }

  std::tuple<cl::Buffer, size_t> create_aligned_buffer(size_t sz, cl::Context& context) const {
    const size_t aligned_sz = next_power_of_2(sz);
    const auto bytes = aligned_sz * sizeof(T);
    return {cl::Buffer(context, CL_MEM_READ_WRITE, bytes), aligned_sz};
  }

  void fill_buffer(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, const T* source) const {
    queue.enqueueWriteBuffer(buf, CL_FALSE, 0, sz * sizeof(T), source);
  }

  void read_buffer(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, T* dest) const {
    queue.enqueueReadBuffer(buf, CL_TRUE, 0, sz * sizeof(T), dest);
  }

  void pad_buffer(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, size_t aligned_sz) const {
    if (aligned_sz > sz) {
      const std::vector<T> padding(aligned_sz - sz, std::numeric_limits<T>::max());
      const auto padding_bytes = (aligned_sz - sz) * sizeof(T);
      queue.enqueueWriteBuffer(buf, CL_FALSE, sz * sizeof(T), padding_bytes, padding.data());
    }
  }

  void sort_pow2(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, size_t lsz) const {
    for (size_t merge_sz = 2; merge_sz <= sz; merge_sz <<= 1) {
      for (size_t sort_sz = merge_sz; sort_sz > lsz; sort_sz >>= 1) {
        sort_global(queue, buf, sz, lsz, merge_sz, sort_sz);
      }
      sort_local(queue, buf, sz, lsz, merge_sz, lsz);
    }
  }

  void sort_local(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, size_t lsz, size_t merge_sz, size_t sort_sz) {
    lsort_kernel_(
      cl::EnqueueArgs(queue, cl::NDRange(sz), cl::NDRange(lsz)),
      buf, sz, merge_sz, sort_sz
    );
  }

  void sort_global(cl::CommandQueue& queue, cl::Buffer& buf, size_t sz, size_t lsz, size_t merge_sz, size_t sort_sz) {
    gsort_kernel_(
      cl::EnqueueArgs(queue, cl::NDRange(sz), cl::NDRange(lsz)),
      buf, sz, merge_sz, sort_sz
    );
  }
 
  Env env_;
  size_t lsz_;

  cl::Program program_;
  cl::KernelFunctor<cl::Buffer, int, int, int> gsort_kernel_;
  cl::KernelFunctor<cl::Buffer, int, int, int> lsort_kernel_;
};

void dump_bitonic_env(const IOCLBitonicEnv& env);

} // namespace iss::ocl