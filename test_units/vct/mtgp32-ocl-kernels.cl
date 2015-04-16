/**
 * Sample Program for OpenCL (was: CUDA 2.3)
 * written by M.Saito (saito@math.sci.hiroshima-u.ac.jp)
 * ported to OpenCL from mtgp32-cuda-tex.cu by Alexander S. van Amesfoort
 *
 * This sample uses OpenCL images (was: texture reference).
 * The CUDA generation speed of PRNG using texture is faster than using
 * constant tabel on Geforce GTX 260.
 *
 * MTGP32-11213
 * This program generates 32-bit unsigned integers.
 * The period of generated integers is 2<sup>11213</sup>-1.
 * This also generates single precision floating point numbers.
 */

#include "mtgp32-ocl-kernels.h" // Can include with OpenCL, but better pass all sources/headers to the compiler at once.

__constant sampler_t tex_param_ref  = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
__constant sampler_t tex_temper_ref = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
__constant sampler_t tex_single_ref = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
/*
 * Generator Parameters.
 */
// OpenCL has a different way to use constant mem, so disable here and simply create/pass Buffer objects.
//__constant__ uint32_t pos_tbl[BLOCK_NUM_MAX];
//__constant__ uint32_t sh1_tbl[BLOCK_NUM_MAX];
//__constant__ uint32_t sh2_tbl[BLOCK_NUM_MAX];
/* high_mask and low_mask should be set by make_constant(), but
 * did not work.
 */
//const uint32_t mask = 0xff800000;
#define MASK 0xff800000

/**
 * The function of the recursion formula calculation.
 *
 * @param[in] X1 the farthest part of state array.
 * @param[in] X2 the second farthest part of state array.
 * @param[in] Y a part of state array.
 * @param[in] bid block id.
 * @return output
 */
uint32_t para_rec(__constant uint32_t* sh1_tbl, __constant uint32_t* sh2_tbl,
		__read_only image2d_t img_param_ref, uint32_t X1, uint32_t X2, uint32_t Y, int bid) {
	uint32_t X = (X1 & MASK) ^ X2;
	uint32_t MAT;

	X ^= X << sh1_tbl[bid];
	Y = X ^ (Y >> sh2_tbl[bid]);
	MAT = read_imageui(img_param_ref, tex_param_ref, (int2)(bid * 16 + (Y & 0x0f), 0)).x;
	return Y ^ MAT;
}

/**
 * The tempering function.
 *
 * @param[in] V the output value should be tempered.
 * @param[in] T the tempering helper value.
 * @param[in] bid block id.
 * @return the tempered value.
 */
uint32_t temper(__read_only image2d_t img_temper_ref, uint32_t V, uint32_t T, int bid) {
	uint32_t MAT;

	T ^= T >> 16;
	T ^= T >> 8;
	MAT = read_imageui(img_temper_ref, tex_temper_ref, (int2)(bid * 16 + (T & 0x0f), 0)).x;
	return V ^ MAT;
}

/**
 * The tempering and converting function.
 * By using the preset-ted table, converting to IEEE format
 * and tempering are done simultaneously.
 * Resulted outputs are distributed in the range [1, 2).
 *
 * @param[in] V the output value should be tempered.
 * @param[in] T the tempering helper value.
 * @param[in] bid block id.
 * @return the tempered and converted value.
 */
float temper_single(__read_only image2d_t img_single_ref, uint32_t V, uint32_t T, int bid) {
	uint32_t MAT;
	uint32_t r;

	T ^= T >> 16;
	T ^= T >> 8;
	MAT = read_imageui(img_single_ref, tex_single_ref, (int2)(bid * 16 + (T & 0x0f), 0)).x;
	r = (V >> 9) ^ MAT;
	return as_float(r);
}

/**
 * The tempering and converting function.
 * By using the preset-ted table, converting to IEEE format
 * and tempering are done simultaneously.
 * Resulted outputs are distributed in the range [0, 1).
 *
 * @param[in] V the output value should be tempered.
 * @param[in] T the tempering helper value.
 * @param[in] bid block id.
 * @return the tempered and converted value.
 */
float temper_single01(__read_only image2d_t img_single_ref, uint32_t V, uint32_t T, int bid) {
	uint32_t MAT;
	uint32_t r;

	T ^= T >> 16;
	T ^= T >> 8;
	MAT = read_imageui(img_single_ref, tex_single_ref, (int2)(bid * 16 + (T & 0x0f), 0)).x;
	r = (V >> 9) ^ MAT;
	return as_float(r) - 1.0f;
}

/**
 * Read the internal state vector from kernel I/O data, and
 * put them into shared memory.
 *
 * @param[out] status shared memory.
 * @param[in] d_status kernel I/O data
 * @param[in] bid block id
 * @param[in] tid thread id
 */
void status_read(__local uint32_t status[LARGE_SIZE],
		__global const mtgp32_kernel_status_t *d_status,
		int bid, int tid) {
	status[LARGE_SIZE - N + tid] = d_status[bid].status[tid];
	if (tid < N - MTGP_THREAD_NUM) {
		status[LARGE_SIZE - N + MTGP_THREAD_NUM + tid] = d_status[bid].status[MTGP_THREAD_NUM + tid];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

/**
 * Read the internal state vector from shared memory, and
 * write them into kernel I/O data.
 *
 * @param[out] d_status kernel I/O data
 * @param[in] status shared memory.
 * @param[in] bid block id
 * @param[in] tid thread id
 */
void status_write(__global mtgp32_kernel_status_t *d_status,
		__local const uint32_t status[LARGE_SIZE],
		int bid, int tid) {
	d_status[bid].status[tid] = status[LARGE_SIZE - N + tid];
	if (tid < N - MTGP_THREAD_NUM) {
	d_status[bid].status[MTGP_THREAD_NUM + tid] = status[4 * MTGP_THREAD_NUM - N + tid];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
}

/**
 * kernel function.
 * This function generates 32-bit unsigned integers in d_data
 *
 * @params[in,out] d_status kernel I/O data
 * @params[out] d_data output
 * @params[in] size number of output data requested.
 */
__kernel void mtgp32_uint32_kernel(__constant uint32_t* pos_tbl, __constant uint32_t* sh1_tbl, __constant uint32_t* sh2_tbl,
				__read_only image2d_t img_param_ref, __read_only image2d_t img_temper_ref,
				__global mtgp32_kernel_status_t* d_status,
				__global __write_only uint32_t* d_data, int size) {
	const int bid = get_group_id(0);
	const int tid = get_local_id(0);
	int pos = pos_tbl[bid];
	uint32_t r;
	uint32_t o;

	/**
	 * Shared memory
	 * The generator's internal status vector.
	 */
	__local uint32_t status[LARGE_SIZE];

	// copy status data from global memory to shared memory.
	status_read(status, d_status, bid, tid);

	// main loop
	for (int i = 0; i < size; i += LARGE_SIZE) {

#if defined(DEBUG)
		if ((i == 0) && (bid == 0) && (tid <= 1)) {
			printf("status[LARGE_SIZE - N + tid]:%08x\n",
				status[LARGE_SIZE - N + tid]);
			printf("status[LARGE_SIZE - N + tid + 1]:%08x\n",
				status[LARGE_SIZE - N + tid + 1]);
			printf("status[LARGE_SIZE - N + tid + pos]:%08x\n",
				status[LARGE_SIZE - N + tid + pos]);
			printf("sh1:%d\n", sh1_tbl[bid]);
			printf("sh2:%d\n", sh2_tbl[bid]);
			printf("mask:%08x\n", MASK);
			for (int j = 0; j < 16; j++) {
				printf("tbl[%d]:%08x\n", j, param_tbl[0][j]);
			}
		}
#endif
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[LARGE_SIZE - N + tid],
				status[LARGE_SIZE - N + tid + 1],
				status[LARGE_SIZE - N + tid + pos], bid);
		status[tid] = r;
#if defined(DEBUG)
		if ((i == 0) && (bid == 0) && (tid <= 1)) {
			printf("status[tid]:%08x\n", status[tid]);
		}
#endif
		o = temper(img_temper_ref, r, status[LARGE_SIZE - N + tid + pos - 1], bid);
#if defined(DEBUG)
		if ((i == 0) && (bid == 0) && (tid <= 1)) {
			printf("r:%08" PRIx32 "\n", r);
		}
#endif
		d_data[size * bid + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[(4 * MTGP_THREAD_NUM - N + tid) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + 1) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + pos) % LARGE_SIZE], bid);
		status[tid + MTGP_THREAD_NUM] = r;
		o = temper(img_temper_ref, r, status[(4 * MTGP_THREAD_NUM - N + tid + pos - 1) % LARGE_SIZE], bid);
		d_data[size * bid + MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[2 * MTGP_THREAD_NUM - N + tid],
				status[2 * MTGP_THREAD_NUM - N + tid + 1],
				status[2 * MTGP_THREAD_NUM - N + tid + pos], bid);
		status[tid + 2 * MTGP_THREAD_NUM] = r;
		o = temper(img_temper_ref, r, status[tid + pos - 1 + 2 * MTGP_THREAD_NUM - N], bid);
		d_data[size * bid + 2 * MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
    	}
	// write back status for next call
	status_write(d_status, status, bid, tid);
}

/**
 * kernel function.
 * This function generates single precision floating point numbers in d_data.
 *
 * @params[in,out] d_status kernel I/O data
 * @params[out] d_data output. IEEE single precision format.
 * @params[in] size number of output data requested.
 */
__kernel void mtgp32_single_kernel(__constant uint32_t* pos_tbl, __constant uint32_t* sh1_tbl, __constant uint32_t* sh2_tbl,
				__read_only image2d_t img_param_ref, __read_only image2d_t img_single_ref,
				__global mtgp32_kernel_status_t* d_status,
				__global __write_only float* d_data, int size) {
	const int bid = get_group_id(0);
	const int tid = get_local_id(0);
	int pos = pos_tbl[bid];
	uint32_t r;
	float o;

	/**
	 * Shared memory
	 * The generator's internal status vector.
	 */
	__local uint32_t status[LARGE_SIZE];

	// copy status data from global memory to shared memory.
	status_read(status, d_status, bid, tid);

	// main loop
	for (int i = 0; i < size; i += LARGE_SIZE) {
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[LARGE_SIZE - N + tid],
				status[LARGE_SIZE - N + tid + 1],
				status[LARGE_SIZE - N + tid + pos], bid);
		status[tid] = r;
		o = temper_single(img_single_ref, r, status[LARGE_SIZE - N + tid + pos - 1], bid);
		d_data[size * bid + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[(4 * MTGP_THREAD_NUM - N + tid) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + 1) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + pos) % LARGE_SIZE], bid);
		status[tid + MTGP_THREAD_NUM] = r;
		o = temper_single(img_single_ref, r, status[(4 * MTGP_THREAD_NUM - N + tid + pos - 1) % LARGE_SIZE], bid);
		d_data[size * bid + MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[2 * MTGP_THREAD_NUM - N + tid],
				status[2 * MTGP_THREAD_NUM - N + tid + 1],
				status[2 * MTGP_THREAD_NUM - N + tid + pos], bid);
		status[tid + 2 * MTGP_THREAD_NUM] = r;
		o = temper_single(img_single_ref, r, status[tid + pos - 1 + 2 * MTGP_THREAD_NUM - N], bid);
		d_data[size * bid + 2 * MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	// write back status for next call
	status_write(d_status, status, bid, tid);
}

/**
 * kernel function.
 * This function generates single precision floating point numbers in d_data.
 *
 * @params[in,out] d_status kernel I/O data
 * @params[out] d_data output. IEEE single precision format.
 * @params[in] size number of output data requested.
 */
__kernel void mtgp32_single01_kernel(__constant uint32_t* pos_tbl, __constant uint32_t* sh1_tbl, __constant uint32_t* sh2_tbl,
				__read_only image2d_t img_param_ref, __read_only image2d_t img_single_ref,
				__global mtgp32_kernel_status_t* d_status,
				__global __write_only float* d_data, int size) {
	const int bid = get_group_id(0);
	const int tid = get_local_id(0);
	int pos = pos_tbl[bid];
	uint32_t r;
	float o;

	/**
	 * Shared memory
	 * The generator's internal status vector.
	 */
	__local uint32_t status[LARGE_SIZE];

	// copy status data from global memory to shared memory.
	status_read(status, d_status, bid, tid);

	// main loop
	for (int i = 0; i < size; i += LARGE_SIZE) {
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[LARGE_SIZE - N + tid],
				status[LARGE_SIZE - N + tid + 1],
				status[LARGE_SIZE - N + tid + pos], bid);
		status[tid] = r;
		o = temper_single01(img_single_ref, r, status[LARGE_SIZE - N + tid + pos - 1], bid);
		d_data[size * bid + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[(4 * MTGP_THREAD_NUM - N + tid) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + 1) % LARGE_SIZE],
				status[(4 * MTGP_THREAD_NUM - N + tid + pos) % LARGE_SIZE], bid);
		status[tid + MTGP_THREAD_NUM] = r;
		o = temper_single01(img_single_ref, r, status[(4 * MTGP_THREAD_NUM - N + tid + pos - 1) % LARGE_SIZE], bid);
		d_data[size * bid + MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
		r = para_rec(sh1_tbl, sh2_tbl, img_param_ref, status[2 * MTGP_THREAD_NUM - N + tid],
				status[2 * MTGP_THREAD_NUM - N + tid + 1],
				status[2 * MTGP_THREAD_NUM - N + tid + pos], bid);
		status[tid + 2 * MTGP_THREAD_NUM] = r;
		o = temper_single01(img_single_ref, r, status[tid + pos - 1 + 2 * MTGP_THREAD_NUM - N], bid);
		d_data[size * bid + 2 * MTGP_THREAD_NUM + i + tid] = o;
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	// write back status for next call
	status_write(d_status, status, bid, tid);
}

