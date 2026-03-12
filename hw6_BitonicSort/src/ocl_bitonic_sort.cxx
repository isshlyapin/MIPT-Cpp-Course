//-----------------------------------------------------------------------------
//
//                    OCLBitonicSorter methods
//
//-----------------------------------------------------------------------------

#include "mydef.hxx"
#include "config.hxx"
#include "ocl_bitonic_sort.hxx"

#include "CL/opencl.hpp"

#include <stdexcept>

namespace iss::ocl {

void dump_bitonic_env(const IOCLBitonicEnv& env) {
  const cl::string plt_name    = env.get_platform().getInfo<CL_PLATFORM_NAME>();
  const cl::string plt_vendor  = env.get_platform().getInfo<CL_PLATFORM_VENDOR>();
  const cl::string plt_version = env.get_platform().getInfo<CL_PLATFORM_VERSION>();
  const cl::string plt_profile = env.get_platform().getInfo<CL_PLATFORM_PROFILE>();

  const cl::vector<cl::Device> devices = env.get_context().getInfo<CL_CONTEXT_DEVICES>();

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