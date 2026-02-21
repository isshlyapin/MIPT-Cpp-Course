#pragma once

#include "config.hxx"
#include "mydef.hxx"
#include "helpers.hxx"
#include "kernel_source.hxx"

#include "CL/opencl.hpp"

#include <memory>
#include <cstddef>
#include <concepts>
#include <iterator>
#include <stdexcept>
#include <string_view>

template <typename T>
struct ocl_type_name;

template<>
struct ocl_type_name<float> {
  static constexpr const char* value = "float";
};

template<>
struct ocl_type_name<int> {
  static constexpr const char* value = "int";
};

struct SortProfile {
  cl::Event first_ev;
  cl::Event last_ev;
};

class IOCLBitonicEnv {
public:
  virtual ~IOCLBitonicEnv() = default;

  [[nodiscard]] virtual const cl::Platform&     get_platform()  const = 0;
  [[nodiscard]] virtual const cl::Context&      get_context()   const = 0;

  [[nodiscard]] virtual cl_ulong get_local_mem_size() const = 0;

protected:
  IOCLBitonicEnv() = default;

  IOCLBitonicEnv(const IOCLBitonicEnv&) = default;
  IOCLBitonicEnv& operator=(const IOCLBitonicEnv&) = default;

  IOCLBitonicEnv(IOCLBitonicEnv&&) = default;
  IOCLBitonicEnv& operator=(IOCLBitonicEnv&&) = default; 
};

// Select the first platform with a GPU devices
class OCLSimpleBitonicEnv : public IOCLBitonicEnv {
public:
  OCLSimpleBitonicEnv() 
    : platform_(select_platform()), context_(get_context(platform_)) {}

  [[nodiscard]] const cl::Platform& get_platform() const override { return platform_;  }
  [[nodiscard]] const cl::Context&  get_context()  const override { return context_;   }

  [[nodiscard]] cl_ulong get_local_mem_size() const override {
    cl::vector<cl::Device> devices = context_.getInfo<CL_CONTEXT_DEVICES>();
    return devices.front().getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
  }
private:
  static cl::Platform select_platform();
  static cl::Context  get_context(const cl::Platform& plt);

  cl::Platform platform_;
  cl::Context  context_;
};

template<typename T>
class OCLBitonicSorter {
public:
  OCLBitonicSorter(std::shared_ptr<const IOCLBitonicEnv> env, size_t lsz) 
    : lsz_(lsz), env_(std::move(env)), 
      program_(buildProgram(env_->get_context(), ocl_type_name<T>::value, lsz)),
      gsort_kernel_(program_, "global_bitonic_sort"), lsort_kernel_(program_, "local_bitonic_sort") {
      if (!is_correct_lsz(lsz)) {
        throw std::runtime_error("Invalid local size");
      }

      dbgs << "Create OCLBitonicSorter:" << "\n";
      dbgs << "\tSelected type: "        << ocl_type_name<T>::value << "\n";
      dbgs << "\tSelected local size: "  << lsz_                    << "\n";
    }

  template<std::contiguous_iterator It>
  requires std::same_as<std::iter_value_t<It>, T>
  void sort(It start, It end) {
    sort(start, end, cl::QueueProperties::None);
  }

  template<std::contiguous_iterator It>
  requires std::same_as<std::iter_value_t<It>, T>
  SortProfile sort(It start, It end, cl::QueueProperties qprops) {
    const size_t sz = std::distance(start, end);
    if (!is_power_of_2(sz)) {
      throw std::runtime_error("Size must be a power of 2");
    }
    const size_t bytes = sz * sizeof(T);

    const size_t right_lsz = largest_divisor_leq_limit(sz, lsz_);
    dbgs << "\nSorting sequence of size: " << sz        << "\n";
    dbgs << "Global local size: "          << lsz_      << "\n";
    dbgs << "Right local size: "           << right_lsz << "\n\n";

    cl::CommandQueue queue(env_->get_context(), qprops);
    
    cl::Buffer buf(env_->get_context(), CL_MEM_READ_WRITE, bytes);

    cl::Event first_event;
    queue.enqueueWriteBuffer(buf, CL_FALSE, 0, bytes, std::to_address(start), nullptr, &first_event);

    size_t k   = 2;  
    size_t cnt = 2;
    for (; cnt <= sz && cnt <= right_lsz; cnt <<= 1, k <<= 1) {
      lsort_kernel_(
        cl::EnqueueArgs(queue, cl::NDRange(sz), cl::NDRange(right_lsz)),
        buf, sz, cnt, k
      );
    }

    for (; cnt <= sz; cnt <<= 1) {
      for (size_t j = cnt; j > right_lsz; j >>= 1) {
        gsort_kernel_(
          cl::EnqueueArgs(queue, cl::NDRange(sz), cl::NDRange(right_lsz)),
          buf, sz, cnt, j
        );
      }
      lsort_kernel_(
        cl::EnqueueArgs(queue, cl::NDRange(sz), cl::NDRange(right_lsz)),
        buf, sz, cnt, right_lsz
      );
    }

    cl::Event last_event;
    queue.enqueueReadBuffer(buf, CL_FALSE, 0, bytes, std::to_address(start), nullptr, &last_event);
    
    last_event.wait();

    return SortProfile{.first_ev=first_event, .last_ev=last_event};
  }

private:
  [[nodiscard]] bool is_correct_lsz(size_t lsz) const {    
    return lsz > 1 && (lsz * sizeof(T)) <= env_->get_local_mem_size();
  }

  cl::Program buildProgram(const cl::Context& context, std::string_view type_name, size_t lsz)  {
    std::string opts = 
      std::string("-DTYPE=") + std::string(type_name) + " " +
      std::string("-DLSZ=")  + std::to_string(lsz);
    
    cl::Program program(context, ocl_kernels::BITONIC_SORT_CL);
    program.build(opts.c_str());
    
    return program;
  }
 
  size_t lsz_;
  std::shared_ptr<const IOCLBitonicEnv> env_;

  cl::Program program_;
  cl::KernelFunctor<cl::Buffer, int, int, int> gsort_kernel_;
  cl::KernelFunctor<cl::Buffer, int, int, int> lsort_kernel_;
};

void dump_bitonic_env(const IOCLBitonicEnv& env);