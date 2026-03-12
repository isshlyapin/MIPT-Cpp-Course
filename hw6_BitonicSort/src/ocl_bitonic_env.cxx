#include "CL/opencl.hpp"

#include "ocl_bitonic_env.hxx"

namespace iss::ocl {

GpuBitonicEnv::GpuBitonicEnv() 
  : platform_(find_gpu_platform()), 
    context_(create_context_for_gpu_device(platform_)) {}

cl::Platform GpuBitonicEnv::find_gpu_platform() {
  cl::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (auto p : platforms) {
    std::vector<cl::Device> devices;
    p.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    
    if (!devices.empty()) { return p; }
  }
  throw std::runtime_error("No platform selected");
}

cl::Context GpuBitonicEnv::create_context_for_gpu_device(const cl::Platform& plt) {
  cl::vector<cl::Device> devices;
  plt.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  return cl::Context{devices.front()};
}

cl_ulong GpuBitonicEnv::get_local_mem_size() const {
  cl::vector<cl::Device> devices = context_.getInfo<CL_CONTEXT_DEVICES>();
  return devices.front().getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
}

} // namespace iss::ocl

