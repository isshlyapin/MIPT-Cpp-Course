#pragma once

#include "config.hxx"

#include "CL/opencl.hpp"

namespace iss::ocl {

template <class Env>
concept BitonicEnv = requires (const Env& e) {
  { e.get_context()        } -> std::same_as<const cl::Context&>;
  { e.get_platform()       } -> std::same_as<const cl::Platform&>;
  { e.get_local_mem_size() } -> std::convertible_to<cl_ulong>;
};

//---------------------------------------------------------------------------//
//          An environment that implements the choice of the                 //
//                 first platform with a gpu device                          //
//---------------------------------------------------------------------------//
class GpuBitonicEnv {
public:
  GpuBitonicEnv();

  [[nodiscard]] const cl::Platform& get_platform() const { return platform_;  }
  [[nodiscard]] const cl::Context&  get_context()  const { return context_;   }

  [[nodiscard]] cl_ulong get_local_mem_size() const;

private:
  static cl::Platform find_gpu_platform();
  static cl::Context  create_context_for_gpu_device(const cl::Platform& plt);

  cl::Platform platform_;
  cl::Context  context_;
};

} // namespace iss::ocl