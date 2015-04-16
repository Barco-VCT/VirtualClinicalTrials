#include "OpenCLManager.h"

#include <cstdlib>	// std::atoi()
#include <cctype>	// std::isdigit(), std::islower()
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "DSContainer.h" // we just need CreateOpenCLImageFormat(), but it needs cl types and enum ComponentType (put that in a separate .h?)

using std::string;
using std::vector;

#ifdef OPENCL_KERNEL_SUBDIR
// Indirection to force eval of OPENCL_KERNEL_SUBDIR before #x. Otherwise, the dir will be "OPENCL_KERNEL_SUBDIR/".
#define STRINGIZE(x)	STRINGIZE2(x)
#define STRINGIZE2(x)	#x
static const char oclKernelSubDir[] = STRINGIZE(OPENCL_KERNEL_SUBDIR) "/"; // OPENCL_KERNEL_SUBDIR is defined in mevic/CMakeLists.txt under BUILD.
#else
#warning OPENCL_KERNEL_SUBDIR not defined, using default "opencl-kernels/".
static const char oclKernelSubDir[] = "opencl-kernels/";
#endif

OpenCLManager* OpenCLManager::m_instance = 0;

OpenCLManager::OpenCLManager()
: m_pMevicLogger(new MevicLogger()/*(unused constr; leaks, but a logger should be Singleton or properly do ref counting*/), m_openclKernelPathName(oclKernelSubDir)
{
	try {
		initOpenCL();
		m_instance = this;
	} catch (cl::Error& err) {
		m_pMevicLogger->logWarning("Failed to initialize OpenCL sub-system: " + string(openclStrError(err.err())) + 
								". This is only a problem if your simulation uses OCL modules.");
	}
}

OpenCLManager::OpenCLManager(MevicLogger* pMevicLogger, string& execPath, vector<string>& args)
: m_pMevicLogger(pMevicLogger), m_openclKernelPathName(execPath.append(oclKernelSubDir))
{
	const string optName = "--opencl-devices=";
	string optVal;
	vector<string>::iterator it;
	for (it = args.begin(); it != args.end(); ++it) {
		if (it->substr(0, optName.size()) == optName) {
			optVal = it->substr(optName.size());
			args.erase(it); // arg consumed
			break;
		}
	}

	try {
		initOpenCL(optVal);
		m_instance = this;
	} catch (cl::Error& err) {
		m_pMevicLogger->logWarning("Failed to initialize OpenCL sub-system: " + string(openclStrError(err.err())) + 
								". This is only a problem if your simulation uses OCL modules.");
	}
}

OpenCLManager::~OpenCLManager() {
	m_instance = 0;
}

OpenCLManager* OpenCLManager::getInstance() {
	if (m_instance == 0) {
		throw std::runtime_error("Cannot use OpenCL modules if the OpenCL sub-system was not successfully initialized.");
	}
	return m_instance;
}

void OpenCLManager::runKernel(const cl::Kernel& kernel, const uint3& threadDims, const uint3& groupDims, const cl::NDRange& offset) {
	// Correct threadDims if it doesn't divide groupDims.
	cl::NDRange thrDims;
	uint3 threadDimsMult;
	unsigned int rem;

	threadDimsMult.x = threadDims.x;
	rem = threadDimsMult.x % groupDims.x;
	if (rem != 0) {
		threadDimsMult.x += groupDims.x - rem;
	}
	if (threadDims.y > 1) {
		threadDimsMult.y = threadDims.y;
		rem = threadDimsMult.y % groupDims.y;
		if (rem != 0) {
			threadDimsMult.y += groupDims.y - rem;
		}
		if (threadDims.z > 1) {
			threadDimsMult.z = threadDims.z;
			rem = threadDimsMult.z % groupDims.z;
			if (rem != 0) {
				threadDimsMult.z += groupDims.z - rem;
			}
			// 3D
			thrDims = cl::NDRange(threadDimsMult.x, threadDimsMult.y, threadDimsMult.z);
		} else { // 2D
			thrDims = cl::NDRange(threadDimsMult.x, threadDimsMult.y);
		}
	} else { // 1D
		thrDims = cl::NDRange(threadDimsMult.x);
	}
	cl::NDRange grpDims(groupDims.x, groupDims.y, groupDims.z);

	cl::Event kRunEvent;
	m_cqueues[0].enqueueNDRangeKernel(kernel, offset, thrDims, grpDims, 0, &kRunEvent);
	kRunEvent.wait();
}

cl::Buffer OpenCLManager::setupBuffer(size_t size, cl_mem_flags access_flags, const void* host_ptr) {
	// Create buffer, but don't pass host_ptr, because it'll either map (=> short, inefficient transfers) or copy to _and_ back. Back is not requested and may be a waste. 
	// CL_MEM_READ_ONLY doesn't avoid copying back in general, because with multiple kernels we need a RW buffer and only copy back after the last kernel.
	// CL_MEM_ALLOC_HOST_PTR _is_ useful (and allows pinned memory if supp), but is needed earlier, not here.
	cl::Buffer buf(m_ctx, access_flags, size, 0, 0);
	if (host_ptr != 0) {
		m_cqueues[0].enqueueWriteBuffer(buf, CL_TRUE/*blocking*/, 0, size, host_ptr, 0, 0);
	}
	return buf;
}

cl::Image2D OpenCLManager::setupImage2D(enum ComponentType elType, size_t dims[2], cl_mem_flags access_flags, /*const */void* host_ptr) {
	cl::Image2D img2D(m_ctx, access_flags, DSContainer::CreateOpenCLImageFormat(elType), dims[0], dims[1], 0, 0, 0);
	if (host_ptr != 0) {
		cl::size_t<3> origin;
		origin[0] = origin[1] = origin[2] = 0;
		cl::size_t<3> region;
		region[0] = dims[0];
		region[1] = dims[1];
		region[2] = 1;

		m_cqueues[0].enqueueWriteImage(img2D, CL_TRUE/*blocking*/, origin, region, 0, 0, host_ptr, 0, 0);
	}
	return img2D;
}

cl::Image3D OpenCLManager::setupImage3D(enum ComponentType elType, size_t dims[3], cl_mem_flags access_flags, /*const */void* host_ptr) {
	cl::Image3D img3D(m_ctx, access_flags, DSContainer::CreateOpenCLImageFormat(elType), dims[0], dims[1], dims[2], 0, 0, 0, 0);
	if (host_ptr != 0) {
		cl::size_t<3> origin;
		origin[0] = origin[1] = origin[2] = 0;
		cl::size_t<3> region;
		region[0] = dims[0];
		region[1] = dims[1];
		region[2] = dims[2];

		m_cqueues[0].enqueueWriteImage(img3D, CL_TRUE/*blocking*/, origin, region, 0, 0, host_ptr, 0, 0);
	}
	return img3D;
}

cl::Program OpenCLManager::buildProgramFromSourceFiles(const vector<string>& cl_filenames, const vector<string>& kernel_func_names) const {
	// Read the .cl (and maybe .h) source files and lengths.
	cl::Program::Sources programSources; // vector to build multiple sources at once
	vector<string> sourceTexts;
	for (unsigned int i = 0; i < cl_filenames.size(); ++i) {
		string ksrcStr(readFile(string(m_openclKernelPathName).append(cl_filenames[i])));
		sourceTexts.push_back(ksrcStr);
		programSources.push_back(std::make_pair(sourceTexts[i].c_str(), ksrcStr.size()));
	}

	cl::Program program(m_ctx, programSources, 0);

	// Build sources for all selected target devices. The limitation is that all targets use the same compilation options.
	vector<cl::Device> targetDevices(m_ctx.getInfo<CL_CONTEXT_DEVICES>());
	try {
		program.build(targetDevices, m_build_flags.c_str());
	} catch (cl::Error& err) {
#if !defined _DEBUG		// Only print warnings in debug mode, and always print errors.
		if (err.err() == CL_BUILD_PROGRAM_FAILURE)
#endif
		{
			// Retrieve and print build errors/warnings.
			for (unsigned int i = 0; i < targetDevices.size(); ++i) {
				try {
					string buildLog(program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(targetDevices[i], 0));
					if (buildLog.size() > 0) {
						string devName(targetDevices[i].getInfo<CL_DEVICE_NAME>());
						m_pMevicLogger->logMessage("=== OpenCL build messages for device " + devName + buildLog +
												"=====================================");
					}
				} catch (cl::Error& err) {
					m_pMevicLogger->logWarning("Failed to retrieve OpenCL kernel build messages: " + string(openclStrError(err.err())));
				}
			}
		}

		if (err.err() != CL_BUILD_PROGRAM_FAILURE) {
			throw;
		}
	}

	return program;
}

vector<cl::Kernel> OpenCLManager::createKernels(const vector<string>& cl_filenames, const vector<string>& kernel_func_names) {
	// See if we already have this combination of kernels compiled. If so, re-use, otherwise build.
	string progMapKey;
	for (unsigned int i = 0; i < kernel_func_names.size(); ++i) {
		progMapKey.append(kernel_func_names[i]);
	}
	cl::Program program;
	if (m_programs.count(progMapKey) > 0) { // use count(), because if not exists, operator[] will add it
		program = m_programs[progMapKey];
	} else { // build and remember
		program = buildProgramFromSourceFiles(cl_filenames, kernel_func_names);
		m_programs.insert(std::make_pair(progMapKey, program));
	}

	// Create kernel objects. Each requester must have its private Kernel objects, because the setArg() mechanism is not thread-safe.
	vector<cl::Kernel> kernels(kernel_func_names.size());
	for (unsigned int i = 0; i < kernel_func_names.size(); ++i) {
		kernels[i] = cl::Kernel(program, kernel_func_names[i].c_str(), 0);
	}

	return kernels;
}


const cl::Context& OpenCLManager::getContextReference() const {
	return m_ctx;
}
unsigned int OpenCLManager::getNumberOfSelectedDevices() const {
	return m_cqueues.size();
}
const cl::CommandQueue& OpenCLManager::getCommandQueueReference(unsigned int idx) const {
	return m_cqueues.at(idx); // bounds checked, even in release mode
}

/*
 * Read file into returned, newly allocated buffer and set the output para ksize.
 */
const string OpenCLManager::readFile(const string& filename) const {
	// Open in binary, otherwise requested file length and nread will not correspond on Windows due to CR NL. Seek to end ('ate').
	std::ifstream kstream(filename.c_str(), std::ios_base::binary | std::ios_base::ate);
	if (!kstream) {
		m_pMevicLogger->logError("Failed to open OpenCL kernel source file " + filename);
	}
	size_t ksize = (size_t)kstream.tellg();
	kstream.seekg(0, std::ios::beg);

	char* ksrc = new char[ksize+1];
	if (ksrc == 0) {
		m_pMevicLogger->logError("Failed to allocate memory to load and compile OpenCL kernel source file " + filename);
	}
	size_t nread = 0;
	while (nread < ksize && kstream.good()) {
		kstream.read(ksrc + nread, ksize - nread);
		nread += (size_t)kstream.gcount();
	}
	ksrc[nread] = '\0'; // just good practice
	kstream.close();

	// Pity we cannot read a file into an std::string in one blow (I think it can only be done with C++0x).
	// This copies, but at least we keep the delete[] here and don't need another para to return len.
	string ksrcStr(ksrc);
	delete[] ksrc;
	return ksrcStr;
}

// Print some info on OpenCL devices.
void OpenCLManager::printDevices(vector<cl::Device>& devs, unsigned int platf_id) const {
	for (unsigned int i = 0; i < devs.size(); ++i) {
		cl::Device dev(devs[i]);

		std::stringstream devSStr;
		devSStr << "Device " << platf_id << ":" << i;
		m_pMevicLogger->logMessage(devSStr.str());
		
		m_pMevicLogger->logMessage("Name:              " + dev.getInfo<CL_DEVICE_NAME>());
		m_pMevicLogger->logMessage("Vendor:            " + dev.getInfo<CL_DEVICE_VENDOR>());
		m_pMevicLogger->logMessage("Version:           " + dev.getInfo<CL_DEVICE_VERSION>());

		std::stringstream typeSStr;
		typeSStr << "Type:              ";
		cl_device_type type = dev.getInfo<CL_DEVICE_TYPE>();
		if (type & CL_DEVICE_TYPE_CPU) typeSStr << "CPU ";
		if (type & CL_DEVICE_TYPE_GPU) typeSStr << "GPU ";
		if (type & CL_DEVICE_TYPE_ACCELERATOR) typeSStr << "Accelerator ";
		if (type & CL_DEVICE_TYPE_DEFAULT) typeSStr << "Default ";
		m_pMevicLogger->logMessage(typeSStr.str());
		
		std::stringstream lmemSStr;
		lmemSStr << "Local Memory Size: " << (unsigned long)dev.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
		m_pMevicLogger->logMessage(lmemSStr.str());

		std::stringstream imgsuppSStr;
		imgsuppSStr << "Image Support:     ";
		if (dev.getInfo<CL_DEVICE_IMAGE_SUPPORT>()) imgsuppSStr << "yes";
		else imgsuppSStr << "no";
		m_pMevicLogger->logMessage(imgsuppSStr.str());
		
		m_pMevicLogger->logMessage("");
	}
}

// Print some info on OpenCL platforms and devices.
void OpenCLManager::printPlatformsAndDevices(vector<cl::Platform>& platfs) const {
	vector<cl::Device> devices;

	for (unsigned int i = 0; i < platfs.size(); ++i) {
		cl::Platform pl(platfs[i]);
		std::stringstream sstr;

		sstr << "Platform " << i;
		m_pMevicLogger->logMessage(sstr.str());
		m_pMevicLogger->logMessage("Name:       " + pl.getInfo<CL_PLATFORM_NAME>());
		m_pMevicLogger->logMessage("Vendor:     " + pl.getInfo<CL_PLATFORM_VENDOR>());
		m_pMevicLogger->logMessage("Version:    " + pl.getInfo<CL_PLATFORM_VERSION>());
		m_pMevicLogger->logMessage("Extensions: " + pl.getInfo<CL_PLATFORM_EXTENSIONS>());

		m_pMevicLogger->logMessage("");

		try {
			pl.getDevices(CL_DEVICE_TYPE_ALL, &devices); // may throw if 0 devices
			printDevices(devices, i);
		} catch (cl::Error& ) {
			// getDevices() may throw if 0 devices (irrel) or if type is invalid (imposs), so don't warn/print.
		}
	}
}

// Add devices with OpenCL image support from platforms to selectedDevices, based on deviceTypeMask. Only if deviceTypeMask is 0, then device plat_i:dev_i is selected instead.
// If the device requested using platf_id and dev_id doesn't exist or has no OpenCL image support, don't add it and print a warning.
void OpenCLManager::addDevicesToSelection(vector<cl::Device>& selectedDevices, const vector<cl::Platform>& platforms,
											cl_device_type deviceTypeMask, unsigned int plat_i, unsigned int dev_i) const {
	vector<cl::Device> devices;
	cl_device_type mask = deviceTypeMask;
	bool cpuAdded = false;

	unsigned int psize = platforms.size();
	if (deviceTypeMask != 0) {
		plat_i = 0;
	} else { // use plat_i and dev_i parameters
		mask = CL_DEVICE_TYPE_ALL; // do our own platf_i:dev_i selection, so ask OpenCL for all devices
		if (plat_i >= psize) {
			std::stringstream sstr;
			sstr << "Skipping OpenCL device '" << plat_i << ":" << dev_i << "': platform does not exist.";
			m_pMevicLogger->logWarning(sstr.str());
			return;
		}
		psize = plat_i + 1;
	}

	for (; plat_i < psize; ++plat_i) {
		try {
			platforms[plat_i].getDevices(mask, &devices);
		} catch (cl::Error& ) {
			// getDevices() may throw if 0 devices (irrel) or if type is invalid (bad caller), so don't warn/print.
			continue;
		}

		unsigned int dsize = devices.size();
		if (deviceTypeMask != 0) {
			dev_i = 0;
		} else { // use plat_i and dev_i parameters
			if (dev_i >= dsize) {
				std::stringstream sstr;
				sstr << "Skipping OpenCL device '" << plat_i << ":" << dev_i << "': device does not exist.";
				m_pMevicLogger->logWarning(sstr.str());
				continue;
			}
			dsize = dev_i + 1;
		}
		for (; dev_i < dsize; ++dev_i) {
			if (!devices[dev_i].getInfo<CL_DEVICE_IMAGE_SUPPORT>()) {
				std::stringstream sstr;
				sstr << "Skipping OpenCL device '" << plat_i << ":" << dev_i << "': no OpenCL image support.";
				m_pMevicLogger->logWarning(sstr.str());
				continue;
			}

			// Try to avoid selecting the same device multiple times (in one function call).
			// This is not easy in general, but let's tackle only the most common, easy to detect variant:
			// >1 platforms with a CPU device. (This can be overridden using the ids.)
			// Context creation already filters duplicate devices, but this warning for CPUs can still make sense.
			if (devices[dev_i].getInfo<CL_DEVICE_TYPE>() & CL_DEVICE_TYPE_CPU) {
				if (!cpuAdded) {
					cpuAdded = true;
				} else {
					std::stringstream sstr;
					sstr << "Skipping OpenCL device '" << plat_i << ":" << dev_i << "': already selected a CPU device.";
					m_pMevicLogger->logWarning(sstr.str());
					continue;
				}
			}

			selectedDevices.push_back(devices[dev_i]);
		}
	}
}

void OpenCLManager::setKernelBuildFlags() {
	m_build_flags = "-I ";
	m_build_flags.append(m_openclKernelPathName);
#if defined _DEBUG
	m_build_flags.append(" -D _DEBUG=1");
	m_build_flags.append(" -cl-opt-disable"); // OpenCL opts are enabled by default; disable in debug mode.
//	m_build_flags.append(" -g"); // On an AMD OpenCL platform, you can use the -g compiler option.
#else
	m_build_flags.append(" -D NDEBUG=1"); // allows to disable assertion-like and other debugging code in kernels
	// In Release mode, compile with the fp32 and fast, non-compliant math options.
	m_build_flags.append(" -cl-single-precision-constant"); // MEVIC uses single precision atm
//	m_build_flags.append(" -cl-fast-relaxed-math"); // includes the next 2. Disabled, see below.
	m_build_flags.append(" -cl-finite-math-only");
//	m_build_flags.append(" -cl-unsafe-math-optimizations"); // may enable low precision math instr on GPUs; includes the next 2. NOTE: this makes our normalization_clb kernel error (-36) on NV drv 260.19.36 / CUDA 3.2
	m_build_flags.append(" -cl-no-signed-zeros");
	m_build_flags.append(" -cl-mad-enable");
#endif
}

/*
 * Initialize OpenCL, select an OpenCL platform and device, and create a context and command queue.
 * Select a device based on deviceSelectStr. If "", choose a DEFAULT device if avail, otherwise prefer GPU or ACCELERATOR over CPU.
 * Avoid devices that lack support for OpenCL images. If you're using ATI Stream 2.3 on the CPU, set the env var CPU_IMAGE_SUPPORT=1 or upgrade to AMD APP 2.4 or later.
 */
void OpenCLManager::initOpenCL(const string& deviceSelectStr) {
	vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	// Device(s) selection
	vector<cl::Device> selectedDevices;
	string devStr(boost::to_lower_copy(string(deviceSelectStr)));
	cl_device_type devType;

	if (devStr == "") {
		// If no device selection was specified, select whatever we can.
		addDevicesToSelection(selectedDevices, platforms, CL_DEVICE_TYPE_ALL);
	} else if (std::islower(devStr[0])) { // generic device category selection
		if (devStr == "cpu") {
			devType = CL_DEVICE_TYPE_CPU;
		} else if (devStr == "gpu") {
			devType = CL_DEVICE_TYPE_GPU;
		} else if (devStr == "accelerator") {
			devType = CL_DEVICE_TYPE_ACCELERATOR;
		} else if (devStr == "default") {
			devType = CL_DEVICE_TYPE_DEFAULT; // Can have 0 (even if CPU type has 1) or > 1 default platforms...
		} else if (devStr == "all") {
			devType = CL_DEVICE_TYPE_ALL;
		} else {
			devType = CL_DEVICE_TYPE_ALL; // avoid compiler warning
			m_pMevicLogger->logError("Incorrect OpenCL devices argument value: valid is one of: cpu gpu accelerator default all.");
		}
		addDevicesToSelection(selectedDevices, platforms, devType);
	} else if (std::isdigit(devStr[0])) { // try to read specific selection format: <platform_idx>:<device_idx>[, ...]
		char* chp = &devStr[0];

		do {
			unsigned int plat_id, dev_id;
			if (std::isdigit(*chp)) { // need to check, because atoi() return 0 on error, while '0' is valid input.
				plat_id = std::atoi(chp);
				chp += plat_id / 10 + 1; // skip digits read
				if (*chp++ != ':' || !std::isdigit(*chp)) {
					m_pMevicLogger->logError("Incorrect OpenCL devices argument value: valid syntax is <platform index>:<device index>, e.g. 0:0 or 0:1,1:0");
				}
				dev_id = std::atoi(chp);
				chp += dev_id / 10 + 1;
				addDevicesToSelection(selectedDevices, platforms, 0, plat_id, dev_id);
			}
		} while (*chp++ == ',');
		if (*--chp != '\0') { // no trailing garbage, please
			m_pMevicLogger->logError("Unrecognized character(s) found in or after valid OpenCL devices argument value.");
		}
	} else {
		m_pMevicLogger->logWarning("Unrecognized OpenCL devices argument value. Listing platforms and devices.");
		printPlatformsAndDevices(platforms);
		m_pMevicLogger->logError("Please retry with a proper device selection or leave the argument out.");
	}

#ifdef _DEBUG
	printPlatformsAndDevices(platforms);
#endif

	if (selectedDevices.size() == 0) {
		m_pMevicLogger->logWarning("Could not select an OpenCL device."); // not fatal, maybe the sim doesn't use OpenCL modules
	}

	// Force first device only, because using a context that spans mult devices is unstable atm, and we don't have a lot of multiple-devices-on-one-platform systems.
	selectedDevices.resize(1);

	// Create context across all selected devices and pass the platform to use for this context. A context can only have devices from the same platform.

	cl_context_properties cps[1] = {0}; // ... let the runtime figure out CL_CONTEXT_PLATFORM
	m_ctx = cl::Context(selectedDevices, cps, 0, 0, 0);

	// Create a command queue for and print the name of each selected device.
#if defined _DEBUG
	// Enable profiling to do timing/profiling.
	cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
#else
	cl_command_queue_properties cq_props = 0;
#endif
	for (unsigned int i = 0; i < selectedDevices.size(); ++i) {
		cl::CommandQueue cq(m_ctx, selectedDevices[i], cq_props, 0);
		m_cqueues.push_back(cq);

		m_pMevicLogger->logMessage("Selected OpenCL device " + selectedDevices[i].getInfo<CL_DEVICE_NAME>());
	}

	setKernelBuildFlags();
}

