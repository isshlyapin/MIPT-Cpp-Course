#include "config.hxx"
#include "bitonic_env.hxx"

#include "CL/opencl.hpp"

namespace iss::ocl {

GpuEnvironment::GpuEnvironment() 
  : platform_(find_gpu_platform()), device_(find_gpu_device(platform_)), 
    context_(device_) {}

cl::Platform GpuEnvironment::find_gpu_platform() {
  cl::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (auto p : platforms) {
    std::vector<cl::Device> devices;
    p.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    
    if (!devices.empty()) { return p; }
  }
  throw std::runtime_error("No platform selected");
}

cl::Device GpuEnvironment::find_gpu_device(const cl::Platform& plt) {
  cl::vector<cl::Device> devices;
  plt.getDevices(CL_DEVICE_TYPE_GPU, &devices);
  if (devices.empty()) {
    throw std::runtime_error("GPU devices disappeared after platform selection");
  }
  return devices.front();  
}

cl_ulong GpuEnvironment::get_local_mem_size() const {
  return device_.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
}

} // namespace iss::ocl

