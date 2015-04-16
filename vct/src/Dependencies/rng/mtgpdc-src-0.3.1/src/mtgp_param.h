#ifndef MTGP_PARAM_H
#define MTGP_PARAM_H
/**
 * @file mtgp_param.h
 *
 * @brief a class for the parameter of mtgps.
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

#include <stdint.h>
#include <string>
#include <openssl/sha.h>

namespace mtgp {
    template<typename T> class mtgp_param {
    public:
	void set_sha1(std::string& src) {
	    unsigned int sha1_length = SHA_DIGEST_LENGTH * 2 + 1;
	    for (unsigned int i = 0; i < sha1_length && i < src.size(); i++) {
		sha1[i] = src[i];
	    }
	    sha1[sha1_length - 1] = 0;
	};
	int mexp;
	int pos;
	int sh1;
	int sh2;
	uint32_t id;
	T mask;
	T tbl[4];
	T p[16];
	T tmp_tbl[4];
	T tp[16];
	char sha1[SHA_DIGEST_LENGTH * 2 + 1];
    };
}
#endif
