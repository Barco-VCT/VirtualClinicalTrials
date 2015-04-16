// mtgp32-ocl-common.h
#include "mtgp-ocl-common.h"
#include "mtgp32-fast.h"
#include "mtgp32-ocl-kernels.h"

void make_kernel_data(cl::CommandQueue cmdQueue, cl::Buffer d_status, mtgp32_params_fast_t params[], int block_num);
void print_float_array(const float array[], int size, int block);
void print_uint32_array(uint32_t array[], int size, int block);
