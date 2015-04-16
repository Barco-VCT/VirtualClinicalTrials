#ifndef MTGP32_FAST_H
#define MTGP32_FAST_H
/**
 * @file mtgp32-fast.h
 *
 * @brief Mersenne Twister for Graphic Processors (mtgp32), which
 * generates 32-bit unsigned integers and single precision floating
 * point numbers based on IEEE 754 format.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2009 Mutsuo Saito, Makoto Matsumoto and
 * Hiroshima University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "mtgp32dc-param-11213.h"

/**
 * \struct MTGP32_STATUS_FAST_T
 * MTGP32 internal state array.
 * In this structure, we use \b larger_size and \b larger_mask to
 * avoid slow operation of remainder (%).
 *
 * \b size is the minimum needed size to represent linear space of
 * two element filed as 32-bit array.
 *
 * \b large_size is a minimum power of 2 which is larger than \b size.
 * \b array has this size.
 *
 * \b large_mask is a bit mask to update \b idx in a fast way.
 * \b idx is updated by `logical and' \b & with large_mask.
 */
typedef struct MTGP32_STATUS_FAST_T {
    int idx;			/**< index */
    int size;			/**< minimum needed size */
    int large_size;		/**< real size of array */
    int large_mask;		/**< bit mask to update \b idx */
    uint32_t array[];		/**< internal state array */
} mtgp32_status_fast_t;

/**
 * \struct MTGP32_STATUS_FAST_T
 * MTGP32 interface structure
 */
typedef struct MTGP32_FAST_T {
    mtgp32_params_fast_t params; /**< parameters */
    mtgp32_status_fast_t *status; /**< internal state */
} mtgp32_fast_t;

/**
 * parameter constants tables for MEXP=11213.
 * \b size - \b pos > 256.
 */
extern mtgp32_params_fast_t mtgp32_params_fast_11213[128];
/**
 * parameter constants tables for MEXP=23209.
 * \b size - \b pos > 512.
 */
extern mtgp32_params_fast_t mtgp32_params_fast_23209[128];
/**
 * parameter constants tables for MEXP=44497.
 * \b size - \b pos > 1024.
 */
extern mtgp32_params_fast_t mtgp32_params_fast_44497[128];

int mtgp32_init(mtgp32_fast_t *mtgp32,
		const mtgp32_params_fast_t *para, uint32_t seed);
void mtgp32_init_state(uint32_t array[],
		      const mtgp32_params_fast_t *para, uint32_t seed);
int mtgp32_init_by_array(mtgp32_fast_t *mtgp32,
			 const mtgp32_params_fast_t *para,
			uint32_t *array, int length);
int mtgp32_init_by_str(mtgp32_fast_t *mtgp32,
		       const mtgp32_params_fast_t *para,
		       char *str);
void mtgp32_free(mtgp32_fast_t *mtgp32);
void mtgp32_print_idstring(const mtgp32_fast_t *mtgp32, FILE *fp);

inline static void mtgp32_do_recursion(uint32_t *r, uint32_t x1,
				       uint32_t x2, uint32_t y,
				       int sh1, int sh2,
				       uint32_t mask, uint32_t tbl[16]);
inline static void mtgp32_next_state(mtgp32_fast_t *mtgp32);
inline static uint32_t mtgp32_genrand_uint32(mtgp32_fast_t *mtgp32);
inline static float mtgp32_genrand_close1_open2(mtgp32_fast_t *mtgp32);
inline static float mtgp32_genrand_close_open(mtgp32_fast_t *mtgp32);
inline static float mtgp32_genrand_open_close(mtgp32_fast_t *mtgp32);
inline static float mtgp32_genrand_open_open(mtgp32_fast_t *mtgp32);

#if 0 // alx: this doesn't belong in this header and we don't want it included along. Disable for now, move later.
/*
 * PRIVATE INLINE FUNCTIONS
 */
/**
 * This is a recursion formula of the generator.
 * MTGP32 is a 32-bit generator, but using 32-bit operations to fit to
 * graphic processors.

 * @param[out] r output
 * @param[in] x1 the farthest part of state array.
 * @param[in] x2 the second farthest part of state array.
 * @param[in] y a part of state array.
 * @param[in] sh1 the shift parameter 1.
 * @param[in] sh2 the shift parameter 2.
 * @param[in] mask the bit mask parameter.
 * @param[in] tbl the matrix parameter.
 */
inline static void mtgp32_do_recursion(uint32_t *r, uint32_t x1,
				       uint32_t x2, uint32_t y,
				       int sh1, int sh2,
				       uint32_t mask, uint32_t tbl[16]) {
    uint32_t x;

    x = (x1 & mask) ^ x2;
    x ^= x << sh1;
    y = x ^ (y >> sh2);
    *r = y ^ tbl[y & 0x0f];
}

/**
 * The state transition function.
 * @param[in,out] mtgp32 the all in one structure
 */
inline static void mtgp32_next_state(mtgp32_fast_t *mtgp32) {
    uint32_t *array = mtgp32->status->array;
    int idx;
    int pos = mtgp32->params.pos;
    int large_size = mtgp32->status->large_size;
    uint32_t large_mask = mtgp32->status->large_mask;
    int size = mtgp32->status->size;

    mtgp32->status->idx = (mtgp32->status->idx + 1) & large_mask;
    idx = mtgp32->status->idx;
    mtgp32_do_recursion(&(array[idx]),
			array[(idx - size + large_size) & large_mask],
			array[(idx - size + large_size + 1) & large_mask],
			array[(idx + pos - size + large_size) & large_mask],
			mtgp32->params.sh1,
			mtgp32->params.sh2,
			mtgp32->params.mask,
			mtgp32->params.tbl);
}

/**
 * The tempering function.
 * @param[in] tmp_tbl the pre-computed tempering table.
 * @param[in] r the value to be tempered.
 * @param[in] t the tempering helper value.
 * @return the tempered value.
 */
inline static uint32_t mtgp32_temper(const uint32_t tmp_tbl[16],
				     uint32_t r, uint32_t t) {
    t ^= t >> 16;
    t ^= t >> 8;
    r ^= tmp_tbl[t & 0x0f];
    return r;
}

/**
 * The tempering and converting function.
 * @param[in] flt_tmp_tbl the pre-computed tempering table.
 * @param[in] r the value to be tempered.
 * @param[in] t the tempering helper value.
 * @return the tempered value.
 */
inline static float mtgp32_temper_float(const uint32_t flt_tmp_tbl[16],
					  uint32_t r, uint32_t t) {
    union {
	uint32_t u;
	float f;
    } x;
    t ^= t >> 16;
    t ^= t >> 8;
    r = r >> 9;
    x.u = r ^ flt_tmp_tbl[t & 0x0f];
    return x.f;
}

/**
 * The tempering and converting function for generating floating point
 * number \b f (0 < \b f < 1).
 * @param[in] flt_tmp_tbl the pre-computed tempering table.
 * @param[in] r the value to be tempered.
 * @param[in] t the tempering helper value.
 * @return the tempered value.
 */
inline static float mtgp32_temper_float_open(const uint32_t flt_tmp_tbl[16],
					       uint32_t r, uint32_t t) {
    union {
	uint32_t u;
	float f;
    } x;
    t ^= t >> 16;
    t ^= t >> 8;
    r = r >> 9;
    x.u = (r ^ flt_tmp_tbl[t & 0x0f]) | 1;
    return x.f;
}

/*
 * PUBLIC INLINE FUNCTIONS
 */
/**
 * This function generates and returns 32-bit unsigned integer.
 * mtgp32_init(), mtgp32_init_by_array() or mtgp32_init_by_str() must
 * be called before this function.
 *
 * @param[in,out] mtgp32 MTGP all in one structure.
 * @return 32-bit unsigned integer.
 */
inline static uint32_t mtgp32_genrand_uint32(mtgp32_fast_t *mtgp32) {
    int idx;
    uint32_t *tmp_tbl = mtgp32->params.tmp_tbl;
    uint32_t *array = mtgp32->status->array;
    int pos = mtgp32->params.pos;
    int large_size = mtgp32->status->large_size;
    int size = mtgp32->status->size;
    uint32_t large_mask = mtgp32->status->large_mask;

    mtgp32_next_state(mtgp32);
    idx = mtgp32->status->idx;
    return mtgp32_temper(tmp_tbl,
			 array[idx],
			 array[(idx + pos - 1 - size + large_size)
			       & large_mask]);
}

/**
 * This function generates and returns single precision pseudorandom
 * number which distributes uniformly in the range [1, 2).
 * mtgp32_init(), mtgp32_init_by_array() or mtgp32_init_by_str() must
 * be called before this function.
 *
 * @param[in,out] mtgp32 MTGP all in one structure.
 * @return single precision floating point pseudorandom number
 */
inline static float mtgp32_genrand_close1_open2(mtgp32_fast_t *mtgp32) {
    int idx;
    uint32_t *flt_tmp_tbl = mtgp32->params.flt_tmp_tbl;
    uint32_t *array = mtgp32->status->array;
    int pos = mtgp32->params.pos;
    int large_size = mtgp32->status->large_size;
    int size = mtgp32->status->size;
    uint32_t large_mask = mtgp32->status->large_mask;

    mtgp32_next_state(mtgp32);
    idx = mtgp32->status->idx;
    return mtgp32_temper_float(flt_tmp_tbl,
				array[idx],
				array[(idx + pos - 1 - size + large_size)
				      & large_mask]);
}

/**
 * This function generates and returns single precision pseudorandom
 * number which distributes uniformly in the range [0, 1).
 * mtgp32_init(), mtgp32_init_by_array() or mtgp32_init_by_str() must
 * be called before this function.
 *
 * @param[in,out] mtgp32 MTGP all in one structure.
 * @return single precision floating point pseudorandom number
 */
inline static float mtgp32_genrand_close_open(mtgp32_fast_t *mtgp32) {
    return mtgp32_genrand_close1_open2(mtgp32) - 1.0F;
}

/**
 * This function generates and returns single precision pseudorandom
 * number which distributes uniformly in the range (0, 1].
 * mtgp32_init(), mtgp32_init_by_array() or mtgp32_init_by_str() must
 * be called before this function.
 *
 * @param[in,out] mtgp32 MTGP all in one structure.
 * @return single precision floating point pseudorandom number
 */
inline static float mtgp32_genrand_open_close(mtgp32_fast_t *mtgp32) {
    return 2.0F - mtgp32_genrand_close1_open2(mtgp32);
}

/**
 * This function generates and returns single precision pseudorandom
 * number which distributes uniformly in the range (0, 1).
 * mtgp32_init(), mtgp32_init_by_array() or mtgp32_init_by_str() must
 * be called before this function.
 *
 * @param[in,out] mtgp32 MTGP all in one structure.
 * @return single precision floating point pseudorandom number
 */
inline static float mtgp32_genrand_open_open(mtgp32_fast_t *mtgp32) {
    float r;
    int idx;
    uint32_t *flt_tmp_tbl = mtgp32->params.flt_tmp_tbl;
    uint32_t *array = mtgp32->status->array;
    int pos = mtgp32->params.pos;
    int large_size = mtgp32->status->large_size;
    int size = mtgp32->status->size;
    uint32_t large_mask = mtgp32->status->large_mask;
    mtgp32_next_state(mtgp32);
    idx = mtgp32->status->idx;
    r = mtgp32_temper_float_open(flt_tmp_tbl,
				  array[idx],
				  array[(idx + pos - 1 - size + large_size)
					& large_mask]);
    return r - 1.0F;
}
#endif // alx (next one is include guard)

#endif

