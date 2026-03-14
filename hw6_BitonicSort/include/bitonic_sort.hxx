#pragma once

#include "types.hxx"
#include "mydef.hxx"
#include "config.hxx"
#include "helpers.hxx"
#include "kernel_source.hxx"
#include "bitonic_env.hxx"

#include "CL/opencl.hpp"

#include <memory>
#include <limits>
#include <cstddef>
#include <concepts>
#include <iterator>
#include <stdexcept>

namespace iss::ocl {

template<typename T, BitonicEnv Env>
class BitonicSorter {
private:
  struct SortSession {
    cl::CommandQueue queue;
    cl::Buffer buffer;
    size_t input_size;
    size_t aligned_size;
    size_t local_size;
    cl::EnqueueArgs enqueue_args;
  };

public:
  BitonicSorter(const Env& env, size_t lsz)
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
    validate_range(start, end);

    const auto input_size = end - start;
    if (input_size < 2) { return; }

    auto session = make_sort_session(input_size);

    upload_input(session, std::to_address(start));
    pad_buffer_with_max_values(session);

    sort(session);

    download_sort_result(session, std::to_address(start));
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

  template<std::contiguous_iterator It>
  void validate_range(It start, It end) {
    const auto size = end - start;
    if (size < 0) {
      throw std::invalid_argument("Invalid range: start > end");
    }
    if (size > (1 << 30)) {
      throw std::invalid_argument("Invalid range: size must be <= 2^30");
    }
    validate_range_device_limit(size);
  }

  void validate_range_device_limit(size_t size) {
    const auto max_global_size = env_.get_device(). template getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
    const auto max_alloc_size = env_.get_device(). template getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

    const auto device_limit = 
      prev_power_of_2(std::min(max_global_size, max_alloc_size));

    if (size > (device_limit / sizeof(T))) {
      throw std::invalid_argument(
        "Invalid range: size exceeds device limit (" + std::to_string(max_global_size) + ")"
      );
    }
  }

  SortSession make_sort_session(const size_t input_size) {
    const auto aligned_size = compute_aligned_size(input_size);
    const auto local_size = compute_local_size(aligned_size);
    auto queue = make_queue();
    return SortSession {
      .queue        = queue,
      .buffer       = make_buffer(aligned_size), 
      .input_size   = input_size, 
      .aligned_size = aligned_size, 
      .local_size   = local_size,
      .enqueue_args = {queue, cl::NDRange(aligned_size), cl::NDRange(local_size)}
    };
  }

  size_t compute_aligned_size(const size_t size) const {
    return next_power_of_2(size);
  }

  size_t compute_local_size(const size_t size) {
    const auto local_kernel = lsort_kernel_.getKernel();
    const auto global_kernel = gsort_kernel_.getKernel();
    const auto min_wgs = std::min(
      local_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(env_.get_device()),
      global_kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(env_.get_device())
    );
    return largest_divisor_leq_limit(size, std::min(lsz_, min_wgs));
  }
  
  cl::CommandQueue make_queue() const {
    return cl::CommandQueue{env_.get_context(), env_.get_device()};
  }

  cl::Buffer make_buffer(const size_t size) const {
    return cl::Buffer{env_.get_context(), CL_MEM_READ_WRITE, bytes_size(size)};    
  }

  void upload_input(SortSession& session, const T* data) const {
    session.queue.enqueueWriteBuffer(
      session.buffer, CL_TRUE, 0, bytes_size(session.input_size), data
    );
  }
  
  size_t bytes_size(const size_t n) const {
    return n * sizeof(T);
  }

  void pad_buffer_with_max_values(SortSession& session) const {
    const auto pad_size = session.aligned_size - session.input_size;
    if (pad_size > 0) {
      session.queue.enqueueFillBuffer(
        session.buffer, std::numeric_limits<T>::max(),
        bytes_size(session.input_size), bytes_size(pad_size)
      );
    }
  }

  void sort(SortSession& session) {
    const auto size = session.aligned_size;
    const auto local_size = session.local_size;
    for (size_t merge_sz = 2; merge_sz <= size; merge_sz <<= 1) {
      for (size_t sort_sz = merge_sz; sort_sz > local_size; sort_sz >>= 1) {
        sort_global(session, merge_sz, sort_sz);
      }
      sort_local(session, merge_sz, local_size);
    }
  }

  void sort_global(SortSession& session, size_t merge_sz, size_t sort_sz) {
    gsort_kernel_(
      session.enqueue_args,
      session.buffer, 
      static_cast<int>(session.aligned_size), 
      static_cast<int>(merge_sz), 
      static_cast<int>(sort_sz)
    );
  }

  void sort_local(SortSession& session, size_t merge_sz, size_t sort_sz) {
    lsort_kernel_(
      session.enqueue_args,
      session.buffer, 
      static_cast<int>(session.aligned_size), 
      static_cast<int>(merge_sz), 
      static_cast<int>(sort_sz)
    );
  }

  void download_sort_result(SortSession& session, T* data) const {
    session.queue.enqueueReadBuffer(
      session.buffer, CL_TRUE, 0, bytes_size(session.input_size), data
    );
  }
 
  Env env_;
  size_t lsz_;

  cl::Program program_;
  cl::KernelFunctor<cl::Buffer, int, int, int> gsort_kernel_;
  cl::KernelFunctor<cl::Buffer, int, int, int> lsort_kernel_;
};

// void dump_bitonic_env(const IOCLBitonicEnv& env);

} // namespace iss::ocl