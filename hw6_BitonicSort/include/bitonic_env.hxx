#pragma once

#include "mydef.hxx"
#include "config.hxx"

#include "CL/opencl.hpp"

#include <concepts>

namespace iss::ocl {

template <class Env>
concept BitonicEnv = requires (const Env& e) {
  { e.get_platform()        } -> std::same_as<const cl::Platform&>;
  { e.get_device()          } -> std::same_as<const cl::Device&>;
  { e.get_context()         } -> std::same_as<const cl::Context&>;
  { e.get_local_mem_size()  } -> std::convertible_to<cl_ulong>;
};

//---------------------------------------------------------------------------//
//          An environment that implements the choice of the                 //
//                 first platform with a gpu device                          //
//---------------------------------------------------------------------------//
class GpuEnvironment {
public:
  GpuEnvironment();

  [[nodiscard]] const cl::Platform& get_platform() const { return platform_;  }
  [[nodiscard]] const cl::Context&  get_context()  const { return context_;   }
  [[nodiscard]] const cl::Device&   get_device()   const { return device_;    }

  [[nodiscard]] cl_ulong get_local_mem_size() const;

private:
  static cl::Platform find_gpu_platform();
  static cl::Device find_gpu_device(const cl::Platform& plt);

  cl::Platform platform_;
  cl::Device   device_;
  cl::Context  context_;
};

template <BitonicEnv Env>
void dump_bitonic_env(const Env& env) {
  const cl::string plt_name    = env.get_platform(). template getInfo<CL_PLATFORM_NAME>();
  const cl::string plt_vendor  = env.get_platform(). template getInfo<CL_PLATFORM_VENDOR>();
  const cl::string plt_version = env.get_platform(). template getInfo<CL_PLATFORM_VERSION>();
  const cl::string plt_profile = env.get_platform(). template getInfo<CL_PLATFORM_PROFILE>();

  const cl::vector<cl::Device> devices = env.get_context(). template getInfo<CL_CONTEXT_DEVICES>();

  dbgs << "CL_PLATFORM_NAME = "    << plt_name    << "\n";
  dbgs << "CL_PLATFORM_VENDOR = "  << plt_vendor  << "\n";
  dbgs << "CL_PLATFORM_VERSION = " << plt_version << "\n";
  dbgs << "CL_PLATFORM_PROFILE = " << plt_profile << "\n";
  dbgs << "Devices:\n";

  for (const auto& d : devices) {
    const cl::string d_name    = d.getInfo<CL_DEVICE_NAME>();
    const cl::string d_vendor  = d.getInfo<CL_DEVICE_VENDOR>();
    const cl::string d_version = d.getInfo<CL_DEVICE_VERSION>();
    
    const cl_ulong d_local_mem     = d.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
    const cl_ulong d_global_mem    = d.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
    const cl_uint  d_compute_units = d.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();

    dbgs << "\t" << d_name << " by " << d_vendor << ": " << d_version << "\n";
    dbgs << "\t\tLocal mem: "     << d_local_mem     << "\n";
    dbgs << "\t\tGlobal mem: "    << d_global_mem    << "\n";
    dbgs << "\t\tCompute units: " << d_compute_units << "\n";
  }
}

} // namespace iss::ocl