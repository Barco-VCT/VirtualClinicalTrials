#ifndef SEARCH_TEMPER_HPP
#define SEARCH_TEMPER_HPP
/**
 * @file search_temper.hpp
 *
 * @brief search tempering parameters so that the random number
 * generator has a good equidistribution property.
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
#include <unistd.h>
#include <tr1/memory>
#include <linear_generator.hpp>
#include <shortest_basis.hpp>

namespace MTToolBox {
    /**
     * @class search_temper
     *
     * @brief search tempering parameters so that the random number
     * generator has a good equidistribution property.
     *
     * This class is expected to be a strategy pattern.
     *
     * @tparam G generator class.
     * \b G must have get_mexp() method,
     * \b G must have set_temper_bit(int pos, int param_pos) and
     * reset_temper_bit(int pos, int param_pos).
     * \b G must have setup_temper().
     * and \b G must be acceptable by shortest_basis class.
     * @tparam T type of the output of \b G.
     * @tparam bit_len bit length of a tempering parameter.
     * @tparam param_num number of tempering parameters.
     * @tparam try_bit_len bit length of the output of \b G to
     * be tempered. Only \b try_bit_len MSBs of the output are tempered.
     * @tparam step Equidistribution property of v-bit accuracy is calculated
     * each \b step bits pattern of tempering parameter. Large \b step will
     * cause low performance.
     * @tparam lsb if lsb is true then the equidistribution property is
     * calculated from LSB bit and tempering parameters are set from LSB.
     */
    template<typename G, typename T,
	     int bit_len, int param_num, int try_bit_len, int step,
	     bool lsb = false>
    class search_temper {
    public:
	/**
	 * search tempering parameters.
	 * @returns 0
	 */
	int operator()(G& rand, bool verbose = false) {
	    using namespace std;
	    if (verbose) {
		cout << "searching..." << endl;
	    }
	    if (lsb) {
		rand.set_reverse_bit();
		if (verbose) {
		    cout << "searching from LSB" << endl;
		}
	    } else {
		rand.reset_reverse_bit();
		if (verbose) {
		    cout << "searching from MSB" << endl;
		}
	    }
	    int delta = 0;
	    int start_pos = 1;
	    for (int p = start_pos; p < try_bit_len; p += step) {
		int max_depth = p + step;
		if (max_depth > try_bit_len) {
		    max_depth = try_bit_len;
		}
		for (int i = 0; i < param_num; i++) {
		    delta = search_best_temper(rand, p, i, max_depth, verbose);
		    if (delta == 0) {
			break;
		    }
		}
		if (delta == 0) {
		    break;
		}
	    }
	    if (verbose) {
		cout << "delta = " << delta << endl;
	    }
	    return delta;
	};
    private:
	/**
	 * search for one tempering parameter. generate all bit pattern
	 * for v-bit to max_v_bit of the tempering parameter, and calculate
	 * equidistribution property, then select the best bit pattern.
	 * This function calls itself recursively.
	 *
	 * @param rand linear generator
	 * @param v_bit the bit of tempering parameter is searched
	 * @param param_pos position of current tempering parameter.
	 * @param max_v_bit search stops at this bit.
	 * @param verbose if true output searching process
	 * @return delta an integer which shows equidistribution property.
	 */
	int search_best_temper(G& rand, int v_bit,
			       int param_pos, int max_v_bit, bool verbose) {
	    using namespace std;
	    using namespace MTToolBox;
	    int mexp = rand.get_mexp();
	    int delta;
	    int min_delta = mexp * bit_len;
	    int max_delta = 0;
	    T min_pattern = 0;
	    int size = max_v_bit - v_bit;
	    T pattern;
	    T mask = make_mask(v_bit, size);
	    for (int i = (1 << size) -1; i >= 0; i--) {
		if (lsb) {
		    pattern = static_cast<T>(i) << v_bit;
		} else {
		    pattern = static_cast<T>(i) << (bit_len - v_bit - size);
		}
		make_temper_bit(rand, mask, pattern, param_pos);
		delta = get_equidist(rand, bit_len, mexp);
		if (delta < min_delta) {
		    min_delta = delta;
		    min_pattern = pattern;
		    if (min_delta == 0) {
			break;
		    }
		}
		if (delta > max_delta) {
		    max_delta = delta;
		}
	    }
	    make_temper_bit(rand, mask, min_pattern, param_pos);
	    if (verbose) {
		cout << dec << min_delta << "-" << max_delta
		     << ":" << hex << min_pattern << endl;
	    }
	    return min_delta;
	};

	/**
	 * wrapper of shortest_basis#get_equidist()
	 *
	 * @param rand linear generator
	 * @param bit_len_ \b bit_len_ MSBs of equidistribution
	 * property of the generator is calculated.
	 * @param sum summation of equidistribution property
	 * from 0 to \b bit_len_ -1 MSBs.
	 * This summation has weight for each bit to consider MSBs
	 * are important.
	 * @returns equidistribution property at \b bit_len_-th
	 * MSB.
	 */
	int get_equidist(G& rand, int bit_len_, int mexp) {

	    G r(rand);
	    shortest_basis<G, T> sb(r, bit_len_);
	    int veq[bit_len_];
	    if (lsb) {
		r.set_reverse_bit();
	    } else {
		r.reset_reverse_bit();
	    }
	    sb.get_all_equidist(veq);
	    int sum = 0;
	    for (int i = 0; i < bit_len_; i++) {
		sum += (mexp / (i + 1) - veq[i]) * (bit_len_ - i);
	    }
	    return sum;
	};

	/**
	 * set bit patten of \b rand's tempering parameters.
	 * @rand linear generator whose tempering parameters are set.
	 * @mask shows which bit position of tempering parameters are set.
	 * @pattern a pattern to be set.
	 * @param_pos shows index of tempering parameter array.
	 */
	void make_temper_bit(G& rand, T mask,T pattern, int param_pos) {
	    rand.set_temper_pattern(mask, pattern, param_pos);
	    rand.setup_temper();
	};

	/**
	 * make mask which has \b size bits 1s form start.<br/>
	 * ex.<br/>
	 * when lsb is false, start = 3, size = 5 then
	 * return 0x1f000000 (0001 1111 0000 0000 0000 0000 0000 0000)<br/>
	 * ex2.<br/>
	 * when lsb is true, start = 2, size = 12 then
	 * return 0x3ffc (0000 0000 0000 0000 0011 1111 1111 1100)<br/>
	 * @param start start bit
	 * @param size size of 1s
	 * @return bit mask
	 */
	T make_mask(int start, int size) {
	    T mask = 0;
	    mask = ~mask;
	    if (start + size > bit_len) {
		size = bit_len - start;
	    }
	    if (lsb) {
		mask >>= start;
		mask <<= bit_len - size;
		mask >>= bit_len - start - size;
	    } else {
		mask <<= start;
		mask >>= bit_len - size;
		mask <<= bit_len - start - size;
	    }
	    return mask;
	};
    };
}

#endif
