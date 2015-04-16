#ifndef MTGP64_HPP
#define MTGP64_HPP
/**
 * @file mtgp64.hpp
 *
 * @brief 64bit mtgp generator used by mtgpdc.
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
#include <cstdlib>
#include <cerrno>
#include <sstream>
#include <unistd.h>
#include "mtgp_param.h"

namespace mtgp {
    using namespace MTToolBox;
    using namespace NTL;
    using namespace std;
    using namespace std::tr1;

    class mtgp64 {
    public:
	mtgp64(int mexp_, uint32_t id_) {
	    memset(&param, 0, sizeof(param));
	    param.mexp = mexp_;
	    status_size = mexp_ / (sizeof(uint64_t) * 8) + 1;
	    param.mask = (~static_cast<uint64_t>(0))
		<< (sizeof(uint64_t) * 8 * status_size - param.mexp);
	    param.id = id_;
	};
	mtgp64(const mtgp64& src) {
	    param = src.param;
	    status_size = src.status_size;
	};
	int get_mexp() {
	    return param.mexp;
	};
	int get_status_size() {
	    return status_size;
	};
	uint64_t rec(uint64_t p1, uint64_t p2, uint64_t p3) {
	    uint64_t x = (p1 & param.mask) ^ p2;
	    uint64_t y = p3;
	    uint64_t r;
	    uint32_t xh = x >> 32;
	    uint32_t xl = x & 0xffffffff;
	    uint32_t yh = y >> 32;
	    uint32_t yl = y & 0xffffffff;
	    xh ^= xh << param.sh1;
	    xl ^= xl << param.sh1;
	    yh = xl ^ (yh >> param.sh2);
	    yl = xh ^ (yl >> param.sh2);
	    r = ((uint64_t)yh << 32) | yl;
	    r ^= param.p[yl & 0x0f];
	    return r;
	};
	void next_state(uint64_t status[], int status_size,
			int idx) {
	    status[idx] = rec(status[idx],
			      status[(idx + 1) % status_size],
			      status[(idx + param.pos) % status_size]);
	};
	void block_generate_without_tempering(uint64_t array[],
					      int size,
					      uint64_t status[],
					      int status_size) {
	    int i;
	    for (i = 0; i < status_size - param.pos; i++) {
		array[i] = rec(status[i], status[i + 1], status[i + param.pos]);
	    }
	    for (; i < status_size - 1; i++) {
		array[i] = rec(status[i],
			       status[i + 1],
			       array[i + param.pos - status_size]);
	    }
	    array[status_size - 1] = rec(status[status_size - 1],
					 array[0],
					 array[param.pos - 1]);
	    for (i = 0; i < size - status_size; i++) {
		array[i + status_size] = rec(array[i],
					     array[i + 1],
					     array[i + param.pos]);
	    }
	};
	void fill_table(uint64_t dist_tbl[], uint64_t src_tbl[], int size) {
	    for(int i = 1; i < size; i++) {
		for(int j = 1, k = 0; j <= i; j <<= 1, k++) {
		    if (i & j) {
			dist_tbl[i] ^= src_tbl[k];
		    }
		}
	    }
	};
	void setup_param(MersenneTwister& mt, int status_size){
	    if (status_size > 50) {
		param.pos
		    = get_range(mt.next(), 3,
				status_size - floor2p<int>(status_size) - 1);
	    } else {
		param.pos = get_range(mt.next(), 3, status_size / 2 - 1);
	    }
	    //param.sh1 = get_range(mt.next(), 1, 31);
	    //param.sh2 = get_range(mt.next(), 1, 31);
	    param.sh1 = 11;
	    param.sh2 = 4;
	    for (int i = 0; i < 4; i++) {
		param.tbl[i] = (uint64_t)mt.next() << 32;
	    }
	    uint64_t id = (uint64_t)param.id << 32;
	    param.tbl[2] = (param.tbl[2] & UINT64_C(0xffff000000000000))
		^ (id & UINT64_C(0x000fffff00000000));
	    param.tbl[3] = (param.tbl[3] & UINT64_C(0x0000ffff00000000))
		^ (id & UINT64_C(0xffff000000000000));
	    memset(param.p, 0, sizeof(param.p));
	    fill_table(param.p, param.tbl, 16);
	};
	mtgp_param<uint64_t> get_param() {
	    return param;
	}
	int get_temper_param_num() {
	    return 4;
	};
	uint64_t temper(uint64_t v, uint64_t status[],
			int status_size, int idx) {
	    uint64_t t = status[(idx + param.pos - 1) % status_size];
	    t = t ^ (t >> 16);
	    t = t ^ (t >> 8);
	    return v ^ param.tp[t & 0x0f];
	};
	void set_temper_bit(int target_bit, int src_bit) {
	    param.tmp_tbl[src_bit] |= static_cast<uint64_t>(1) << target_bit;
	};
	void reset_temper_bit(int target_bit, int src_bit) {
	    param.tmp_tbl[src_bit] &= ~(static_cast<uint64_t>(1) << target_bit);
	};
        void set_temper_pattern(uint64_t mask, uint64_t pattern, int src_bit) {
            param.tmp_tbl[src_bit] &= ~mask;
            param.tmp_tbl[src_bit] |= pattern & mask;
        };
	void setup_temper() {
	    memset(param.tp, 0, sizeof(param.tp));
	    fill_table(param.tp, param.tmp_tbl, 16);
	};
	void set_param(mtgp_param<uint64_t> src) {
	    param = src;
	};

	void out_param(ostream& out) {
	    out << "pos:" << param.pos << endl;
	    out << "sh1:" << param.sh1 << endl;
	    out << "sh2:" << param.sh2 << endl;
	    for (int i = 0; i < 4; i++) {
		out << "tbl[" << dec << i << "]:0x" << hex
		    << setw(sizeof(uint64_t) * 2) << setfill('0')
		    << param.tbl[i]
		    << endl;
	    }
	    for (int i = 0; i < 4; i++) {
		out << "tmp_tbl[" << dec << i << "]:0x"
		    << hex << setw(sizeof(uint64_t) * 2) << setfill('0')
		    << param.tmp_tbl[i] << endl;
	    }
	    out << "id:" << "0x" << hex << setw(sizeof(uint32_t) * 2)
		<< setfill('0') << param.id << dec << endl;
	};

    private:
	int status_size;
	mtgp_param<uint64_t> param;
    };
}

#endif
