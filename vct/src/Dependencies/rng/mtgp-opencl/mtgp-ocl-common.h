// mtgp-ocl-common.h

#include <stdlib.h> // exit()
//#define CL_VERSION_1_1	1
#include <CL/cl.hpp>

#define CL_CHECK(status, msg) { \
	if (status != CL_SUCCESS) { \
		fprintf(stderr, "OpenCL error (%d): failed to " msg "\n", status); exit(1); \
	} \
}

int get_suitable_block_num(int word_size, int thread_num, int large_size);
void initOpenCL(cl::Context& context, cl::CommandQueue& cmdQueue, cl::Program& program, const char* filename);

