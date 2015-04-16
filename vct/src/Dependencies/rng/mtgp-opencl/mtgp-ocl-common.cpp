/**
 * Sample Program for OpenCL (was: CUDA 2.3)
 * written by M.Saito (saito@math.sci.hiroshima-u.ac.jp)
 * ported to OpenCL by Alexander S. van Amesfoort
 *
 * This sample uses OpenCL images (was: texture reference).
 * The generation speed of PRNG using texture is faster than using
 * constant tabel on Geforce GTX 260. (CUDA)
 *
 * MTGP32-11213
 * This program generates 32-bit unsigned integers.
 * The period of generated integers is 2<sup>11213</sup>-1.
 * This also generates single precision floating point numbers.
 */
#include <iostream>
#include <fstream>
#include <string>

#include "mtgp-ocl-common.h"

int get_suitable_block_num(int word_size, int thread_num, int large_size) {
	return 512; // OpenCL: take the easy way out for now
}

/*
 * Read file into returned, newly allocated buffer and set the output para ksize.
 */
static char* read_file(size_t& ksize, const char* filename) {
	std::ifstream kstream(filename);
	if (!kstream.good()) {
		fprintf(stderr, "Failed to open OpenCL kernel source file."); exit(1);
	}
	kstream.seekg(0, std::ios::end);
	ksize = kstream.tellg();
	kstream.seekg(0, std::ios::beg);

	char* ksrcText = new char[ksize+1];
	if (ksrcText == 0) {
		fprintf(stderr, "Failed to allocate memory to load and compile OpenCL kernel source file."); exit(1);
	}
	kstream.read(ksrcText, ksize);
	ksrcText[ksize] = '\0'; // not really necessary for OpenCL, but now we can check/print it.
	if (!kstream.good()) {
		fprintf(stderr, "Warning: failed to read file\n");
	}
	kstream.close();
	return ksrcText;
}

/*
 * Initialize OpenCL, print platforms and devices, select 1st dev from 1st platf,
 * load and compile filename from source, create a context and command queue into context and cmdQueue.
 */
void initOpenCL(cl::Context& context, cl::CommandQueue& cmdQueue, cl::Program& program, const char* filename) {
	cl_int status;

	std::vector<cl::Platform> platforms;
	std::vector<cl::Platform>::iterator pl_i;

	status = cl::Platform::get(&platforms);
	CL_CHECK(status, "retrieve list of OpenCL platforms");

	printf("Detected %zu OpenCL platforms\n", platforms.size());
	if (platforms.size() == 0) {
		fprintf(stderr, "OpenCL error: need at least 1 platform\n"); exit(1);
	}
	// print all OpenCL platforms (that properly registered their ICD)
	for (pl_i = platforms.begin(); pl_i != platforms.end(); ++pl_i) {
		cl::Platform pl = *pl_i;
		printf("Profile:    %s\n", pl.getInfo<CL_PLATFORM_PROFILE>().c_str() );
		printf("Version:    %s\n", pl.getInfo<CL_PLATFORM_VERSION>().c_str() );
		printf("Name:       %s\n", pl.getInfo<CL_PLATFORM_NAME>().c_str() );
		printf("Vendor:     %s\n", pl.getInfo<CL_PLATFORM_VENDOR>().c_str() );
		printf("Extensions: %s\n", pl.getInfo<CL_PLATFORM_EXTENSIONS>().c_str() );
		printf("\n");
	}
	// and pick the first platform for now
	pl_i = platforms.begin();

	cl_context_properties cps[3] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(*pl_i)(), 0
	};
	cl_device_type deviceType = CL_DEVICE_TYPE_ALL;
	context = cl::Context(deviceType, cps, NULL, NULL, &status);
	CL_CHECK(status, "create OpenCL context");

	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>(&status);
	CL_CHECK(status, "retrieve list of devices from context\n");
	
	printf("Detected %zu OpenCL devices in selected context\n", devices.size());
	if (devices.size() == 0) {
		fprintf(stderr, "OpenCL error: need at least 1 device\n"); exit(1);
	}
	// print all devices
	std::vector<cl::Device>::iterator dev_i;
	for (dev_i = devices.begin(); dev_i != devices.end(); ++dev_i) {
		cl::Device dev = *dev_i;
		cl_device_type type = dev.getInfo<CL_DEVICE_TYPE>();
		printf("Type:              ");
		if (type & CL_DEVICE_TYPE_CPU) printf("CPU ");
		if (type & CL_DEVICE_TYPE_GPU) printf("GPU ");
		if (type & CL_DEVICE_TYPE_ACCELERATOR) printf("Accelerator ");
		if (type & CL_DEVICE_TYPE_DEFAULT) printf("Default ");
		printf("\n");
		printf("Name:              %s\n", dev.getInfo<CL_DEVICE_NAME>().c_str() );
		printf("Vendor:            %s\n", dev.getInfo<CL_DEVICE_VENDOR>().c_str() );
		printf("Version:           %s\n", dev.getInfo<CL_DEVICE_VERSION>().c_str() );
		printf("Local Memory Size: %lu\n", (unsigned long)dev.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() );
		cl_bool img_supp = dev.getInfo<CL_DEVICE_IMAGE_SUPPORT>();
		printf("Image Support:     %s\n", img_supp ? "yes" : "no");
		if (dev_i == devices.begin() && !img_supp) { // going to select the first device, so check its image support
			printf("Warning: No OpenCL image support. If using AMD Stream, you may want to set the env var CPU_IMAGE_SUPPORT=1\n");
		}
	}
	// pick the first device for now
	dev_i = devices.begin();

	cmdQueue = cl::CommandQueue(context, *dev_i, CL_QUEUE_PROFILING_ENABLE, &status); // enable profiling to do timing and N RNG/s printing
	CL_CHECK(status, "create command queue");

	// load and compile kernel from source file, no support for loading a kernel binary file for now
	size_t ksize;
	char* ksrcText;
	ksrcText = read_file(ksize, filename); // sets ksize and returns ptr to new file data buffer
	// create and build program
	cl::Program::Sources programSources; // sources is a vector, but we only need to build 1 program file
	programSources.push_back(std::make_pair(ksrcText, ksize));
	program = cl::Program(context, programSources, &status);
	CL_CHECK(status, "create OpenCL kernel program");

	std::vector<cl::Device> targetDevices;
	targetDevices.push_back(*dev_i);
	// Debugging: pass -g, then break after compiled (e.g. when creating a Kernel obj),
	// Type into GDB: 'info func __OpenCL_<kernel-func-name>_kernel' and/or 'break __OpenCL_<kernel-func-name>_kernel' to set a breakpoint.
	const char* build_flags_str = "-g"; // this opt crashes the NV OpenCL compiler in driver 260.19.36
//	const char* build_flags_str = "-cl-fast-relaxed-math -cl-single-precision-constant"; // fast but sloppy math
	status = program.build(targetDevices, build_flags_str);
	if (status == CL_BUILD_PROGRAM_FAILURE) {
		// print build messages
		cl_int status2;
		std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*dev_i, &status2);
		CL_CHECK(status2, "build program for selected device and retrieve the build log");
		std::cerr << str << std::endl << std::endl;
	}
	CL_CHECK(status, "build program for selected device");	
	
	delete[] ksrcText; // too late if error and already bailed out
}

