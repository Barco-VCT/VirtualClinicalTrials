//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file OpenCLManager.h
 * \brief Implementation of the class OpenCLManager
 * \date 29-April-2011
 * (C) Copyright 2014 Barco NV and others.
 *
 * This software code is licensed to you under the Barco VCT Research License. The text of this license
 * is found in the file License.TXT which is available in the same repository, in absence of which you need
 * to contact Barco first before you are allowed to engage in any act of familiarization or use of whatever
 * kind. Any familiarization or use of whatever kind of this software implies acceptance of the pre-mentioned
 * license. You warrant that the modifications you make are “original” and are able to be subject to the provisions
 * of the pre-mentioned license. Any contribution containing “non-original” modification require the prior written
 * approval from Barco.
 *
 */

#ifndef OpenCLManager_H
#define OpenCLManager_H
#include <string>
#include <vector>
#include <map>
#include <utility> // pair(), make_pair()

#define __CL_ENABLE_EXCEPTIONS
#if defined WIN32 || defined _WIN32 || defined __WIN32__ || defined WIN64
#pragma warning(disable: 4290)	// MSVC doesn't support exc spec; warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
// and link to a libOpenCL.a/OpenCL.lib (or dyn) from any vendor

#include "OpenCLErrorStrings.h"
#include "OpenCLHostTypes.h"

#include <DSContainer.h> // enum ComponentType (better split off a few defs into a separate .h to include here)
#include <ErrorManagement/MevicLogger.h>


class OpenCLManager {
	friend class SuperMEVICXML; // To instantiate and clean up, both once.

	static OpenCLManager* m_instance;

	MevicLogger* m_pMevicLogger;

	// Compiled kernel programs. The map key is the concatenation of the __kernel function names.
	std::map<std::string, cl::Program> m_programs;

	// keep it simple for now: 1 context over all selected devices with their own, single command queue.
	cl::Context m_ctx;
	std::vector<cl::CommandQueue> m_cqueues; // atm, only m_cqueues[0] is used

	// Location of kernels at runtime relative to current working dir when started.
	std::string m_openclKernelPathName;

	// OpenCL kernel debugging: pass -g (compile in DEBUG mode), then break after compiled (e.g. when creating a Kernel obj),
	// Type into DDD/GDB: 'info func __OpenCL_<kernel-func-name>_kernel' and/or 'break __OpenCL_<kernel-func-name>_kernel' to set a breakpoint.
	// Pragma to use printf with AMD APP: #pragma OPENCL EXTENSION cl_amd_printf : enable . For Intel, use cl_intel_printf
	std::string m_build_flags;


	OpenCLManager();
	OpenCLManager(MevicLogger* pMevicLogger, std::string& execPath, std::vector<std::string>& args);
	OpenCLManager(OpenCLManager const& rhs) { }				// private, don't use, not even by friend or itself
	OpenCLManager& operator=(OpenCLManager const& rhs) { return *this; }	// idem; stmt avoids GCC warning
	~OpenCLManager();

public:
	/*!
	 * Retrieve the same, single reference to the OpenCLManager.
	 */
	static OpenCLManager* getInstance();

	/*!
	 * Run a kernel and wait for it to finish. Set an unused dim to 1 (e.g. {64, 64, 1}).
	 * If threadDims doesn't divide groupDims, we extend threadDims and your kernel must be able to deal with it!
	 * The dimensions of threadDims, groupDims and offset must be the same (1, 2 or 3 dims) (unless default para).
	 */
	void runKernel(const cl::Kernel& kernel, const uint3& threadDims, const uint3& groupDims, const cl::NDRange& offset = cl::NullRange);

	/*!
 	 * Create buffer/image[2|3]D allocation in OpenCL device memory. Optionally, transfer data from host_ptr there.
	 * Override access_flags for a read-only or write-only buffer.
	 * To create a component in OpenCL device memory (new or as a copy from an existing component),
	 * use one of the DSContainer::createContainer() functions that accepts an 'enum AllocatorType'.
	 */
	cl::Buffer setupBuffer(size_t size, cl_mem_flags access_flags = CL_MEM_READ_WRITE, const void* host_ptr = 0);
	cl::Image2D setupImage2D(enum ComponentType elType, size_t dims[2], cl_mem_flags access_flags = CL_MEM_READ_WRITE, /*const */void* host_ptr = 0);
	cl::Image3D setupImage3D(enum ComponentType elType, size_t dims[3], cl_mem_flags access_flags = CL_MEM_READ_WRITE, /*const */void* host_ptr = 0);

	/*!
	 * Load and compile kernel objects from source filenames for every passed kernel function name. Returns a vector of Kernel objects.
	 * Pass cl_filenames in reverse dep order (if x.cl depends on y.cl: {y.cl, x.cl}).
	 * Note that a lot of system headers cannot be included in OpenCL kernels.
	 */
	std::vector<cl::Kernel> createKernels(const std::vector<std::string>& cl_filenames, const std::vector<std::string>& kernel_func_names);

	// This is a wrapper which doesn't add anything, but there are reasons to do arg setting different (we did), so I'll leave the interface for the mo.
	// To set a container as a kernel argument, use DSContainer::SetOpenCLKernelArg().
	template <typename T>
	void setKernelArg(cl::Kernel& kernel, unsigned int arg_index, T arg_value) {
		kernel.setArg(arg_index, arg_value);
	}

	// These are not so nice, but the class will not be feature complete for a while, so allow access, also for experimental / debug code. (Likely, need to drop the const&)
	const cl::Context& getContextReference() const;
	unsigned int getNumberOfSelectedDevices() const;
	const cl::CommandQueue& getCommandQueueReference(unsigned int idx = 0) const;

private:
	const std::string readFile(const std::string& filename) const;
	cl::Program buildProgramFromSourceFiles(const std::vector<std::string>& cl_filenames, const std::vector<std::string>& kernel_func_names) const;
	void printDevices(std::vector<cl::Device>& devs, unsigned int platf_id) const;
	void printPlatformsAndDevices(std::vector<cl::Platform>& platfs) const;
	void addDevicesToSelection(std::vector<cl::Device>& selectedDevices, const std::vector<cl::Platform>& platforms, cl_device_type deviceTypeMask,
								unsigned int plat_i = 0, unsigned int dev_i = 0) const;
	void setKernelBuildFlags();
	void initOpenCL(const std::string& deviceSelectStr = std::string());

};


#endif // OpenCLManager_H

