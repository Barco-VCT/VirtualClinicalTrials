/**
 * Sample Program for OpenCL (was: CUDA 2.3)
 * written by M.Saito (saito@math.sci.hiroshima-u.ac.jp)
 * ported to OpenCL from mtgp32-cuda-tex.cu by Alexander S. van Amesfoort
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
#define __STDC_FORMAT_MACROS 1
#define __STDC_CONSTANT_MACROS 1
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>

#include "mtgp32-ocl-common.h"

/**
 * This function sets constants in device memory.
 * @param params input, MTGP32 parameters.
 */
void make_constant_param(cl::Context context, cl::CommandQueue cmdQueue,
			cl::Buffer& pos_tbl, cl::Buffer& sh1_tbl, cl::Buffer& sh2_tbl,
			const mtgp32_params_fast_t params[], int block_num) {
	const int size1 = sizeof(uint32_t) * block_num;
	uint32_t *h_pos_tbl;
	uint32_t *h_sh1_tbl;
	uint32_t *h_sh2_tbl;
#if 0
	uint32_t *h_mask;
#endif
	h_pos_tbl = (uint32_t *)malloc(size1);
	h_sh1_tbl = (uint32_t *)malloc(size1);
	h_sh2_tbl = (uint32_t *)malloc(size1);
#if 0
	h_mask = (uint32_t *)malloc(sizeof(uint32_t));
#endif
	if (h_pos_tbl == NULL || h_sh1_tbl == NULL || h_sh2_tbl == NULL
#if 0
	|| h_mask == NULL
#endif
	) {
		printf("failure in allocating host memory for constant table.\n");
		exit(1);
	}
#if 0
	h_mask = params[0].mask;
#endif
	for (int i = 0; i < block_num; i++) {
		h_pos_tbl[i] = params[i].pos;
		h_sh1_tbl[i] = params[i].sh1;
		h_sh2_tbl[i] = params[i].sh2;
	}

	cl_int status;
	pos_tbl = cl::Buffer(context, CL_MEM_READ_ONLY, size1, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create pos_tbl constant device buffer");
	sh1_tbl = cl::Buffer(context, CL_MEM_READ_ONLY, size1, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create sh1_tbl constant device buffer");
	sh2_tbl = cl::Buffer(context, CL_MEM_READ_ONLY, size1, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create sh2_tbl constant device buffer");

	// copy from malloc area only
	status = CL_SUCCESS; // assume 0
	status |= cmdQueue.enqueueWriteBuffer(pos_tbl, CL_TRUE/*blocking*/, 0/*offset*/, size1, h_pos_tbl, 0/*event_wait_list*/, 0/*event*/);
	status |= cmdQueue.enqueueWriteBuffer(sh1_tbl, CL_TRUE/*blocking*/, 0/*offset*/, size1, h_sh1_tbl, 0/*event_wait_list*/, 0/*event*/);
	status |= cmdQueue.enqueueWriteBuffer(sh2_tbl, CL_TRUE/*blocking*/, 0/*offset*/, size1, h_sh2_tbl, 0/*event_wait_list*/, 0/*event*/);
	CL_CHECK(status, "copy image data to the device");
#if 0
	// port to OpenCL: skip adding disabled code to setup mask
#endif
	free(h_pos_tbl);
	free(h_sh1_tbl);
	free(h_sh2_tbl);
#if 0
	free(h_mask);
#endif
}

/**
 * This function sets texture lookup table.
 * @param params input, MTGP32 parameters.
 * @param block_num block number used for kernel call
 */
void make_texture(cl::Context context, cl::CommandQueue cmdQueue,
		cl::Image2D& img_param_ref, cl::Image2D& img_temper_ref, cl::Image2D& img_single_ref,
		const mtgp32_params_fast_t params[],
		int block_num) {
	const int count = block_num * TBL_SIZE;
	const int size = sizeof(uint32_t) * count;
	uint32_t *h_texture_tbl[3];
	cl_int status;
	
	int i, j;
	for (i = 0; i < 3; i++) {
		h_texture_tbl[i] = (uint32_t *)malloc(size);
		if (h_texture_tbl[i] == NULL) {
			for (j = 0; j < i; j++) {
				free(h_texture_tbl[i]);
			}
			printf("failure in allocating host memory for constant table.\n");
			exit(1);
		}
    }
	for (int i = 0; i < block_num; i++) {
		for (int j = 0; j < TBL_SIZE; j++) {
			h_texture_tbl[0][i * TBL_SIZE + j] = params[i].tbl[j];
			h_texture_tbl[1][i * TBL_SIZE + j] = params[i].tmp_tbl[j];
			h_texture_tbl[2][i * TBL_SIZE + j] = params[i].flt_tmp_tbl[j];
		}
	}

	img_param_ref = cl::Image2D(context, CL_MEM_READ_ONLY,
		cl::ImageFormat(CL_R, CL_UNSIGNED_INT32), count, 1/*height*/, /*row_pitch*/0, /*host_ptr*/0, &status);
	CL_CHECK(status, "create 2D param_ref image");
	img_temper_ref = cl::Image2D(context, CL_MEM_READ_ONLY,
		cl::ImageFormat(CL_R, CL_UNSIGNED_INT32), count, 1/*height*/, /*row_pitch*/0, /*host_ptr*/0, &status);
	CL_CHECK(status, "create 2D temper_ref image");
	img_single_ref = cl::Image2D(context, CL_MEM_READ_ONLY,
		cl::ImageFormat(CL_R, CL_UNSIGNED_INT32), count, 1/*height*/, /*row_pitch*/0, /*host_ptr*/0, &status);
	CL_CHECK(status, "create 2D single_ref image");

	cl::size_t<3> origin;
	origin[0] = origin[1] = origin[2] = 0;
	cl::size_t<3> region;
	region[0] = count; // width, 
	region[1] = region[2] = 1; // height, depth (1 for 2D images)
	status = CL_SUCCESS; // assume 0
	status |= cmdQueue.enqueueWriteImage(img_param_ref, CL_TRUE/*blocking*/, origin, region,
			0/*row_pitch*/, 0/*slice_pitch*/, h_texture_tbl[0], 0/*event_wait_list*/, 0/*event*/);
	status |= cmdQueue.enqueueWriteImage(img_temper_ref, CL_TRUE/*blocking*/, origin, region,
			0/*row_pitch*/, 0/*slice_pitch*/, h_texture_tbl[1], 0/*event_wait_list*/, 0/*event*/);
	status |= cmdQueue.enqueueWriteImage(img_single_ref, CL_TRUE/*blocking*/, origin, region,
			0/*row_pitch*/, 0/*slice_pitch*/, h_texture_tbl[2], 0/*event_wait_list*/, 0/*event*/);
	CL_CHECK(status, "copy image data to the device");

	free(h_texture_tbl[0]);
	free(h_texture_tbl[1]);
	free(h_texture_tbl[2]);
}

/**
 * host function.
 * This function calls corresponding kernel function.
 *
 * @param d_status kernel I/O data.
 * @param num_data number of data to be generated.
 */
void make_uint32_random(cl::Context context, cl::CommandQueue cmdQueue, cl::Program program,
			cl::Buffer pos_tbl, cl::Buffer sh1_tbl, cl::Buffer sh2_tbl,
			cl::Image2D img_param_ref, cl::Image2D img_temper_ref,
			cl::Buffer d_status,
			int num_data,
			int block_num) {
	uint32_t* h_data;
	float gputime;
	cl_int status;

	cl::Kernel kernel = cl::Kernel(program, "mtgp32_uint32_kernel",  &status);
	CL_CHECK(status, "create kernel from program");

	printf("generating 32-bit unsigned random numbers.\n");
	cl::Buffer d_data(context, CL_MEM_WRITE_ONLY, sizeof(uint32_t) * num_data, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create device buffer for output data");
	h_data = (uint32_t *) malloc(sizeof(uint32_t) * num_data);
	if (h_data == NULL) {
		printf("failure in allocating host memory for output data.\n");
		exit(1);
	}

	/* parameter setup and kernel queue/call */
	status = CL_SUCCESS; // assume is 0
	status |= kernel.setArg(0, pos_tbl);
	status |= kernel.setArg(1, sh1_tbl);
	status |= kernel.setArg(2, sh2_tbl);
	status |= kernel.setArg(3, img_param_ref);
	status |= kernel.setArg(4, img_temper_ref);
	status |= kernel.setArg(5, d_status);
	status |= kernel.setArg(6, d_data);
	status |= kernel.setArg(7, num_data / block_num);
	CL_CHECK(status, "set kernel arguments");
	cl::Event event_kdone;
	cl::NDRange globalThreads(block_num * THREAD_NUM); // or num_data/3; not num_data, as each thread generates 3 nrs
	cl::NDRange localThreads(THREAD_NUM);
	status = cmdQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalThreads, localThreads, 0, &event_kdone);
	CL_CHECK(status, "enqueue kernel");

	status = event_kdone.wait();
	CL_CHECK(status, "wait for kernel");

	cl_ulong kstart, kstop;
	status = CL_SUCCESS; // assume is 0
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_START, &kstart);
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_END, &kstop);
	CL_CHECK(status, "read kernel profiling timer");

	status = cmdQueue.enqueueReadBuffer(d_data, CL_TRUE/*blocking*/, 0/*offset*/, sizeof(uint32_t) * num_data, h_data, 0, 0);
	CL_CHECK(status, "retrieve generated data");

	gputime = (float)((kstop - kstart) / 1000000.0); // ns to ms
	print_uint32_array(h_data, num_data, block_num);
	printf("generated numbers: %d\n", num_data);
	printf("Processing time: %f (ms)\n", gputime);
	printf("Samples per second: %E \n", num_data / (gputime * 0.001f));

	// free memories
	free(h_data); //reen
}

/**
 * host function.
 * This function calls corresponding kernel function.
 *
 * @param d_status kernel I/O data.
 * @param num_data number of data to be generated.
 */
void make_single_random(cl::Context context, cl::CommandQueue cmdQueue, cl::Program program,
			cl::Buffer pos_tbl, cl::Buffer sh1_tbl, cl::Buffer sh2_tbl,
			cl::Image2D img_param_ref, cl::Image2D img_single_ref,
			cl::Buffer d_status,
			int num_data,
			int block_num) {
	float* h_data;
	float gputime;
	cl_int status;

	cl::Kernel kernel = cl::Kernel(program, "mtgp32_single_kernel",  &status);
	CL_CHECK(status, "create kernel from program");

	printf("generating single precision floating point random numbers.\n");
	cl::Buffer d_data(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_data, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create device buffer for output data");
	h_data = (float *) malloc(sizeof(float) * num_data);
	if (h_data == NULL) {
		printf("failure in allocating host memory for output data.\n");
		exit(1);
	}

	/* parameter setup and kernel queue/call */
	status = CL_SUCCESS; // assume is 0
	status |= kernel.setArg(0, pos_tbl);
	status |= kernel.setArg(1, sh1_tbl);
	status |= kernel.setArg(2, sh2_tbl);
	status |= kernel.setArg(3, img_param_ref);
	status |= kernel.setArg(4, img_single_ref);
	status |= kernel.setArg(5, d_status);
	status |= kernel.setArg(6, d_data);
	status |= kernel.setArg(7, num_data / block_num);
	CL_CHECK(status, "set kernel arguments");

	cl::Event event_kdone;
	cl::NDRange globalThreads(block_num * THREAD_NUM); // or num_data/3; not num_data, as each thread generates 3 nrs
	cl::NDRange localThreads(THREAD_NUM);
	status = cmdQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalThreads, localThreads, 0, &event_kdone);
	CL_CHECK(status, "enqueue kernel");
	
	status = event_kdone.wait();
	CL_CHECK(status, "wait for kernel");

	cl_ulong kstart, kstop;
	status = CL_SUCCESS; // assume is 0
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_START, &kstart);
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_END, &kstop);
	CL_CHECK(status, "read kernel profiling timer");

	status = cmdQueue.enqueueReadBuffer(d_data, CL_TRUE/*blocking*/, 0/*offset*/, sizeof(float) * num_data, h_data, 0, 0);
	CL_CHECK(status, "retrieve generated data");

	gputime = (float)((kstop - kstart) / 1000000.0); // ns to ms
	print_float_array(h_data, num_data, block_num);
	printf("generated numbers: %d\n", num_data);
	printf("Processing time: %f (ms)\n", gputime);
	printf("Samples per second: %E \n", num_data / (gputime * 0.001f));

	// free memories
	free(h_data);
}

/**
 * host function.
 * This function calls corresponding kernel function.
 *
 * @param d_status kernel I/O data.
 * @param num_data number of data to be generated.
 */
void make_single01_random(cl::Context context, cl::CommandQueue cmdQueue, cl::Program program,
			cl::Buffer pos_tbl, cl::Buffer sh1_tbl, cl::Buffer sh2_tbl,
			cl::Image2D img_param_ref, cl::Image2D img_single_ref,
			cl::Buffer d_status,
			int num_data,
			int block_num) {
	float* h_data;
	float gputime;
	cl_int status;

	cl::Kernel kernel = cl::Kernel(program, "mtgp32_single01_kernel",  &status);
	CL_CHECK(status, "create kernel from program");

	printf("generating single precision floating point random numbers.\n");
	cl::Buffer d_data(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_data, 0/*host_ptr*/, &status);
	CL_CHECK(status, "create device buffer for output data");
	h_data = (float *) malloc(sizeof(float) * num_data);
	if (h_data == NULL) {
		printf("failure in allocating host memory for output data.\n");
		exit(1);
	}

	/* parameter setup and kernel queue/call */
	status = CL_SUCCESS; // assume is 0
	status |= kernel.setArg(0, pos_tbl);
	status |= kernel.setArg(1, sh1_tbl);
	status |= kernel.setArg(2, sh2_tbl);
	status |= kernel.setArg(3, img_param_ref);
	status |= kernel.setArg(4, img_single_ref);
	status |= kernel.setArg(5, d_status);
	status |= kernel.setArg(6, d_data);
	status |= kernel.setArg(7, num_data / block_num);
	CL_CHECK(status, "set kernel arguments");

	cl::Event event_kdone;
	cl::NDRange globalThreads(block_num * THREAD_NUM); // or num_data/3; not num_data, as each thread generates 3 nrs
	cl::NDRange localThreads(THREAD_NUM);
	status = cmdQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalThreads, localThreads, 0, &event_kdone);
	CL_CHECK(status, "enqueue kernel");

	status = event_kdone.wait();
	CL_CHECK(status, "wait for kernel");

	cl_ulong kstart, kstop;
	status = CL_SUCCESS; // assume is 0
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_START, &kstart);
	status |= event_kdone.getProfilingInfo(CL_PROFILING_COMMAND_END, &kstop);
	CL_CHECK(status, "read kernel profiling timer");

	status = cmdQueue.enqueueReadBuffer(d_data, CL_TRUE/*blocking*/, 0/*offset*/, sizeof(float) * num_data, h_data, 0, 0);
	CL_CHECK(status, "retrieve generated data");
	gputime = (float)((kstop - kstart) / 1000000.0); // ns to ms
	print_float_array(h_data, num_data, block_num);
	printf("generated numbers: %d\n", num_data);
	printf("Processing time: %f (ms)\n", gputime);
	printf("Samples per second: %E \n", num_data / (gputime * 0.001f));

	// free memories
	free(h_data);
}

// ./prog <nblocks> <noutputvals>, e.g. ./prog 1 768 or ./prog 64 49152
int main(int argc, char *argv[]) {
	// LARGE_SIZE is a multiple of 16
	int num_data = 10000000;
	int block_num;
	int num_unit;
	int r;

	cl::Context context;
	cl::CommandQueue cmdQueue;
	cl::Program program;
	cl::Image2D img_param_ref, img_temper_ref, img_single_ref;
	cl::Buffer pos_tbl, sh1_tbl, sh2_tbl; // __constant buffers
	cl_int status;

	if (argc >= 2) {
		errno = 0;
		block_num = strtol(argv[1], NULL, 10);
		if (errno) {
			printf("%s number_of_block number_of_output\n", argv[0]);
			return 1;
		}
		if (block_num < 1 || block_num > BLOCK_NUM_MAX) {
			printf("%s block_num should be between 1 and %d\n",
					argv[0], BLOCK_NUM_MAX);
			return 1;
		}
		errno = 0;
		num_data = strtol(argv[2], NULL, 10);
		if (errno) {
			printf("%s number_of_block number_of_output\n", argv[0]);
			return 1;
		}
		argc -= 2;
		argv += 2;
	} else {
		printf("%s number_of_block number_of_output\n", argv[0]);
		block_num = get_suitable_block_num(sizeof(uint32_t),
						   THREAD_NUM, LARGE_SIZE);
		if (block_num <= 0) {
			printf("can't calculate sutable number of blocks.\n");
			return 1;
		}
		printf("the suitable number of blocks for device 0 will be a multiple of %d\n", block_num);
		return 1;
	}

	initOpenCL(context, cmdQueue, program, "mtgp32-ocl-kernels.cl");
	num_unit = LARGE_SIZE * block_num;
	cl::Buffer d_status(context, CL_MEM_READ_WRITE, sizeof(mtgp32_kernel_status_t) * block_num, 0/*host_ptr*/, &status);
	// d_status buffer is populated by make_kernel_data()
	CL_CHECK(status, "create status buffer");

	r = num_data % num_unit;
	if (r != 0) {
		num_data = num_data + num_unit - r;
	}

	make_constant_param(context, cmdQueue, pos_tbl, sh1_tbl, sh2_tbl, MTGPDC_PARAM_TABLE, block_num);
	make_texture(context, cmdQueue, img_param_ref, img_temper_ref, img_single_ref, MTGPDC_PARAM_TABLE, block_num);
	make_kernel_data(cmdQueue, d_status, MTGPDC_PARAM_TABLE, block_num);
	make_uint32_random(context, cmdQueue, program, pos_tbl, sh1_tbl, sh2_tbl, img_param_ref, img_temper_ref, d_status, num_data, block_num);
	make_single_random(context, cmdQueue, program, pos_tbl, sh1_tbl, sh2_tbl, img_param_ref, img_single_ref, d_status, num_data, block_num);
	make_single01_random(context, cmdQueue, program, pos_tbl, sh1_tbl, sh2_tbl, img_param_ref, img_single_ref, d_status, num_data, block_num);

	// finalize
	// OpenCL/C++ cleans up itself
}

