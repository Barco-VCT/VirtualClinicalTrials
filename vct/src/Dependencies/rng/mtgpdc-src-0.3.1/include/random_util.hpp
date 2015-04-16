#ifndef RANDOM_UTIL_HPP
#define RANDOM_UTIL_HPP
/**
 * @file random_util.hpp
 *
 * @brief some functions commonly used in MTToolBox.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (c) 2010 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 * Copyright (c) 2011 Mutsuo Saito, Makoto Matsumoto, Hiroshima
 * University and University of Tokyo. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdint.h>
#include <openssl/sha.h>

#include <NTL/GF2X.h>

namespace MTToolBox {
    /**
     * calculate the largest number which is 2^m and does not exceed n.
     * @tparam T type of integer
     * @param n number
     * @returns the largest number which is 2^m and does not exceed n.
     */
    template<typename T>
    T floor2p(T n) {
	if (n == 1) {
	    return 1;
	} else {
	    return 2 * floor2p<T>(n / 2);
	}
    }

    /**
     * print polynomial in binary form. The coefficient of the smallest degree
     * is printed first.
     * @param os output-stream
     * @param poly polynomial to be printed
     * @param breakline if true, break line every 32 outputs.
     */
    inline static void print_binary(std::ostream& os,
				    NTL::GF2X& poly,
				    bool breakline = true) {
	using namespace NTL;
	if (deg(poly) < 0) {
	    os << "0deg=-1" << std::endl;
	    return;
	}
	for(int i = 0; i <= deg(poly); i++) {
	    if(rep(coeff(poly, i)) == 1) {
		os << '1';
	    } else {
		os << '0';
	    }
	    if (breakline && ((i % 32) == 31)) {
		os << std::endl;
	    }
	}
	os << "deg=" << deg(poly) << std::endl;
    }

    /**
     * change \b input to the number between \b start and \b end
     * @param input input number
     * @param start start of the range
     * @param end end of the range
     * @return the number \b r such that \b start <= \b r <= \b end.
     */
    inline static uint32_t get_range(uint32_t input, int start, int end) {
	if (end < start) {
	    printf("get_range:%d, %d\n", start, end);
	    exit(0);
	}
	return input % (end - start + 1) + start;
    }

    /**
     * change \b input to the number between \b start and \b end
     * @param input input number
     * @param start start of the range
     * @param end end of the range
     * @return the number \b r such that \b start <= \b r <= \b end.
     */
    inline static uint64_t get_range(uint64_t input, int start, int end) {
	if (end < start) {
	    printf("get_range:%u, %u\n", start, end);
	    exit(0);
	}
	return input % (end - start + 1) + start;
    }

    /**
     * change the small F2 table to the fast and redundant table.
     * @tparam T type of table member.
     * @param dist_tbl new redundant table
     * @param src_tbl source table
     * @param size size of \b dist_table
     */
    template<typename T>
    void fill_table(T dist_tbl[], T src_tbl[], int size) {
    	for(int i = 1; i < size; i++) {
	    for(int j = 1, k = 0; j <= i; j <<= 1, k++) {
		if (i & j) {
		    dist_tbl[i] ^= src_tbl[k];
		}
	    }
	}
    }

    /**
     * calculate the SHA1 digest of F2 polynomial. The coefficients of
     * the polynomial are changed to the string of "0" and "1", which
     * starts with the coefficient of the lowest degree, and the SHA1
     * hash of the string is calculated. The result hash is returned
     * by hexadecimal string.
     *
     * @param str output string
     * @param poly F2 polynomial
     */
    inline static void poly_sha1(std::string& str, const NTL::GF2X& poly) {
	using namespace NTL;
	using namespace std;
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	if (deg(poly) < 0) {
	    SHA1_Update(&ctx, "-1", 2);
	}
	for(int i = 0; i <= deg(poly); i++) {
	    if(rep(coeff(poly, i)) == 1) {
		SHA1_Update(&ctx, "1", 1);
	    } else {
		SHA1_Update(&ctx, "0", 1);
	    }
	}
	unsigned char md[SHA_DIGEST_LENGTH];
	SHA1_Final(md, &ctx);
	stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
	    ss << setfill('0') << setw(2) << hex
	       << static_cast<int>(md[i]);
	}
	ss >> str;
    }

    /**
     * look-up table for calc_1pos().
     */
    static const uint8_t calc_1pos_tbl[256] = {
	0, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	2, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	1, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	2, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
	4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    };

    /**
     * calculate the position of most right 1, or
     * least significant 1. The position of the MSB is
     * 0. returns -1 when \b v is zero.
     *
     * @param v input
     * @return the position of most right 1.
     */
    static inline int calc_1pos(uint32_t v) {
	int p1, p2, p3, p4;

	p1 = calc_1pos_tbl[v & 0xff];
	if (p1) {
	    return p1 + 24 - 1;
	}
	p2 = calc_1pos_tbl[(v >> 8) & 0xff];
	if (p2) {
	    return p2 + 16 - 1;
	}
	p3 = calc_1pos_tbl[(v >> 16) & 0xff];
	if (p3) {
	    return p3 + 8 - 1;
	}
	p4 = calc_1pos_tbl[(v >> 24) & 0xff];
	if (p4) {
	    return p4 - 1;
	} else {
	    return -1;
	}
    }

    /**
     * calculate the position of most right 1, or
     * least significant 1. The position of the MSB is
     * 0. returns -1 when \b v is zero.
     *
     * @param v input
     * @return the position of most right 1.
     */
    static inline int calc_1pos(uint64_t v) {
	int p1, p2, p3, p4, p5, p6, p7, p8;

	p1 = calc_1pos_tbl[v & 0xff];
	if (p1) {
	    return p1 + 56 - 1;
	}
	p2 = calc_1pos_tbl[(v >> 8) & 0xff];
	if (p2) {
	    return p2 + 48 - 1;
	}
	p3 = calc_1pos_tbl[(v >> 16) & 0xff];
	if (p3) {
	    return p3 + 40 - 1;
	}
	p4 = calc_1pos_tbl[(v >> 24) & 0xff];
	if (p4) {
	    return p4 + 32 - 1;
	}
	p5 = calc_1pos_tbl[(v >> 32) & 0xff];
	if (p5) {
	    return p5 + 24 - 1;
	}
	p6 = calc_1pos_tbl[(v >> 40) & 0xff];
	if (p6) {
	    return p6 + 16 - 1;
	}
	p7 = calc_1pos_tbl[(v >> 48) & 0xff];
	if (p7) {
	    return p7 + 8 - 1;
	}
	p8 = calc_1pos_tbl[(v >> 56) & 0xff];
	if (p8) {
	    return p8 - 1;
	} else {
	    return -1;
	}
    }

    /**
     * check if \b array is all zero or not.
     * @tparam type of members of \b array
     * @param array checked array
     * @param size size of \b array
     * @return true if all elements of \b array are zero.
     */
    template<typename T>
    bool is_zero_array(T *array, int size) {
	if (array[0] != 0) {
	    return false;
	} else {
	    return (memcmp(array, array + 1, sizeof(T) * (size - 1)) == 0);
	}
    }

#define bit_size(tp) (static_cast<int>(sizeof(tp) * 8))

}
#endif
