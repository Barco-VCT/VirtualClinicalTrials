#include "OpenCLErrorStrings.h"

static const char* openclErrorStrings[] = {
	"No error (or error code lost)",					// 0
	"Device not found",									// -1
	"Device not available",
	"Compiler not available",
	"Memory object allocation failure",
	"Out of resources or access outside of allocation",	// CL_OUT_OF_RESOURCES
	"Out of host memory",
	"Profiling info not available",
	"Memory copy overlap",
	"Image format mismatch",
	"Image format not supported",						// -10
	"Build program failure",
	"Map failure",
	"Misaligned sub-buffer offset",
	"Execution status error for events in wait list",
	"Unknown error (-15)",
	"Unknown error (-16)",
	"Unknown error (-17)",
	"Unknown error (-18)",
	"Unknown error (-19)",
	"Unknown error (-20)",								// -20
	"Unknown error (-21)",
	"Unknown error (-22)",
	"Unknown error (-23)",
	"Unknown error (-24)",
	"Unknown error (-25)",
	"Unknown error (-26)",
	"Unknown error (-27)",
	"Unknown error (-28)",
	"Unknown error (-29)",
	"Invalid value",									// -30
	"Invalid device type",
	"Invalid platform",
	"Invalid device",
	"Invalid context",
	"Invalid queue properties",
	"Invalid command queue",
	"Invalid host pointer",
	"Invalid memory object",
	"Invalid image format descriptor",
	"Invalid image size or pitch",						// CL_INVALID_IMAGE_SIZE -40
	"Invalid sampler",
	"Invalid binary",
	"Invalid build options",
	"Invalid program",
	"Invalid program executable",
	"Invalid kernel name or compilation failed",		// CL_INVALID_KERNEL_NAME
	"Invalid kernel definition",
	"Invalid kernel",
	"Invalid argument index",
	"Invalid argument value",							// -50
	"Invalid argument size or number of arguments",		// CL_INVALID_ARG_SIZE
	"Invalid kernel arguments",
	"Invalid work dimension",
	"Invalid work group size",
	"Invalid work item size",
	"Invalid global offset",
	"Invalid event wait list",
	"Invalid event",
	"Invalid operation",
	"Invalid GL object",								// -60
	"Invalid buffer size",
	"Invalid mip level",
	"Invalid global work size",
	"Invalid property"
};

const char* openclStrError(int clErrno) {
	int errIdx = -clErrno;
	if ((unsigned int)errIdx >= sizeof(openclErrorStrings) / sizeof(openclErrorStrings[0])) {
		return "Unknown OpenCL error number";
	} else if (errIdx == 1001) {
		/* AMD APP gives this error when querying platforms if it cannot load any OpenCL ICD (installable client driver) under Linux.
		 * Put the proper vendor supplied .icd file(s) in /etc/OpenCL/vendors/, or set OPENCL_VENDOR_PATH to the right dir, and in any case make sure the dyn linker can find the ICD(s).
		 */
		return "Failed to find .icd file(s) or load the therein specified dynamic library";
	}
	return openclErrorStrings[errIdx];
}

