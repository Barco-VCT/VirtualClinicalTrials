// mtgp32-ocl-kernels.h
// Note: this file is also compiled before (or included by) MTGP OpenCL kernels at runtime, so keep it dual-compilable.
// That also means to not include any system headers disallowed for OpenCL kernels.
typedef unsigned int uint32_t;

#define MEXP 11213
#define N MTGPDC_N						// .cl
#define MTGP_THREAD_NUM MTGPDC_FLOOR_2P		// .cl
#define LARGE_SIZE (MTGP_THREAD_NUM * 3)		// .cl // each thread generates 3 numbers per round
//#define BLOCK_NUM 32
#define BLOCK_NUM_MAX 200
#define TBL_SIZE 16

// definitions from mtgp32dc-param-11213.c which can now be compiled instead of included
#define MTGPDC_MEXP 11213
#define MTGPDC_N 351					// .cl
#define MTGPDC_FLOOR_2P 256				// .cl
#define MTGPDC_CEIL_2P 512

/**
 * kernel I/O
 * This structure must be initialized before first use.
 */
struct mtgp32_kernel_status_t {			// .cl
    uint32_t status[N];
};
typedef struct mtgp32_kernel_status_t mtgp32_kernel_status_t; // announce for OpenCL what C++ already knows

