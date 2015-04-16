#ifndef LINEAR_GENERATOR_HPP
#define LINEAR_GENERATOR_HPP
/**
 * @file linear_generator.hpp
 *
 * @brief generic class for linear_generator
 * This class is a wrapper class of user-defined generator.
 * This class has generator's internal state, and some handling
 * functions of the state.
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
#include <tr1/memory>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <typeinfo>
#include <stdexcept>
#include <NTL/vec_GF2.h>
#include <NTL/GF2X.h>
#include "mt19937.hpp"
#include "random_util.hpp"

namespace MTToolBox {
    /**
     * @class linear_generator
     * @brief a wrapper class for F2-linear generator.
     *
     * @tparam T type of output of \b GF
     * @tparam GF F2-linear generator
     */
    template<typename T, typename GF> class linear_generator {
    public:
	/**
	 * returns a copy using shared_ptr.
	 *
	 * @return copy of me
	 */
	std::tr1::shared_ptr< linear_generator<T, GF> >
	clone() const {
	    std::tr1::shared_ptr< linear_generator<T, GF> >
		copy(new linear_generator<T, GF>(*this));
	    return copy;
	};
	/**
	 * make a copy of me and initialize its internal state
	 * with zero, then return it using shared_ptr.
	 *
	 */
	std::tr1::shared_ptr< linear_generator<T, GF> >
	zero() const {
	    std::tr1::shared_ptr< linear_generator<T, GF> >
		copy(new linear_generator<T, GF>(*this));
	    memset(copy->status, 0, sizeof(T) * copy->status_size);
	    return copy;
	};

	/**
	 * returns Mersenne exponent of user-defined generator.
	 *
	 * @return Mersenne exponent.
	 */
	int get_mexp() const {
	    return mexp;
	};

	/**
	 * returns word size counted by bit.
	 * @return word size
	 */
	int get_word_size() const {
	    return sizeof(T) * 8;
	};

	/**
	 * returns the element number of internal status array
	 * @returns status_size
	 */
	int get_status_size() const {
	    return status_size;
	};

	/**
	 * test if the internal state array is all zero.
	 * @returns true if internal state array is all zero.
	 */
	bool is_zero() const {
	    return is_zero_array<T>(status, status_size);
	};

	/**
	 * calculate the minimal polynomial of the generated sequence.
	 * @returns the minimal polynomial
	 */
	std::tr1::shared_ptr<NTL::GF2X> get_minpoly() {
	    using namespace std;
	    using namespace NTL;
	    using namespace std::tr1;

	    vec_GF2 vec;
	    vec.SetLength(mexp * 2);
	    T array[mexp * 2];
	    gf.block_generate_without_tempering(array,
						mexp * 2,
						status,
						status_size);
	    for (int i = 0; i < mexp * 2; i++) {
		vec[i] = array[i] & 1;
	    }
	    shared_ptr<GF2X> minpoly(new GF2X());
	    MinPolySeq(*minpoly, vec, mexp);
	    return minpoly;
	};

	/**
	 * initialize the internal state array
	 * @param seed seed of initialization
	 */
	void seeding(uint32_t seed) {
	    status[0] = seed;
	    for (int i = 1; i < status_size; i++) {
		status[i] = 1812433253UL
		    * (status[i - 1] ^ (status[i - 1] >> 30))
		    + i;
	    }
	    idx = status_size - 1;
	};

	/**
	 * make the internal state has many zeros.
	 */
	void init_zero_ex() {
	    memset(status, 0, sizeof(T) * status_size);
	    status[status_size - 1] = static_cast<T>(1) << (bit_size(T) - 1);
	};

	/**
	 * transfer the internal state to the next state
	 */
	void next_state() {
	    idx = (idx + 1) % status_size;
	    gf.next_state(status, status_size, idx);
	};

	/**
	 * transfer the internal state to the next state and
	 * returns the generated value.
	 * @return genetated value.
	 */
	T generate() {
	    next_state();
	    return gf.temper(status[idx], status, status_size, idx);
	};

	T generate(int bit_size) {
	    if (reverse_bit) {
		return generate_lsb(bit_size);
	    } else {
		return generate_msb(bit_size);
	    }
	};

	/**
	 * generate only upper \b bit_size (MSB) bit
	 * @return generated value
	 */
	T generate_msb(int bit_size) {
	    T u = generate();
	    T mask = (~static_cast<T>(0)) << (bit_size(T) - bit_size);
	    return u & mask;
	};

	/**
	 * generate only lower \b bit_size (LSB) bit
	 * @return generated value
	 */
	T generate_lsb(int bit_size) {
	    T u = generate();
	    T w = 0;
	    T mask = static_cast<T>(1) << (bit_size(T) - 1);
	    for (int i = 0; i < bit_size; i++) {
		if (u & 1) {
		    w = w | mask;
		}
		u >>= 1;
		mask >>= 1;
	    }
	    return w;
	};

	/**
	 * set up parameter using another random number generator.
	 * @param mt Mersenne Twister used for pramater generation.
	 */
	void setup_param(MersenneTwister& mt) {
	    gf.setup_param(mt, status_size);
	};

	/**
	 * set 1 to \b target_bit of \b temper_index of tempering table.
	 * @param target_bit bit position count from MSB
	 * @param temper_index index of tempering table
	 */
	void set_temper_bit(int target_bit, int temper_index) {
	    gf.set_temper_bit(target_bit, temper_index);
	};

	/**
	 * set 0 to \b target_bit of \b temper_index of tempering table.
	 * @param target_bit bit position count from MSB
	 * @param temper_index index of tempering table
	 */
	void reset_temper_bit(int target_bit, int temper_index) {
	    gf.reset_temper_bit(target_bit, temper_index);
	};

	/**
	 * set tempering pattern
	 * @param mask mask of tempering place
	 * @param pattern pattern to be set
	 * @param temper_index index of tempering table
	 */
        void set_temper_pattern(T mask, T pattern, int temper_index) {
	    gf.set_temper_pattern(mask, pattern, temper_index);
        };

	/**
	 * if user-defined generator need to do something after
	 * set_temper_bit() and before generation, then do it when
	 * this funciton is called
	 */
	void setup_temper() {
	    gf.setup_temper();
	};

	/**
	 * return size of tempering table
	 * @return size of tempering table
	 */
	int get_temper_param_num() {
	    return gf.get_temper_param_num();
	};

	/**
	 * output user-defined generator's parameter to \b o stream.
	 * @param o output stream where the parameter is printed.
	 */
	void out_param(std::ostream& o) {
	    gf.out_param(o);
	};

	/**
	 * returns copy of user-defined random number generator using
	 * shared pointer.
	 * @return user-defined random number generator.
	 */
	std::tr1::shared_ptr<GF> get_random() {
	    std::tr1::shared_ptr<GF> sp(new GF(gf));
	    return sp;
	}

	/**
	 * add internal state array of \b src to that of this object
	 * as if they are F2 vectors.
	 * @param src linear generator to be added to this.
	 */
	void add(const linear_generator<T, GF>& src) {
	    using namespace std;

	    if (this == &src) {
		throw new logic_error("add to myself");
	    }
	    int i;
	    int diff = (src.idx - idx + status_size) % status_size;
	    for (i = 0; i < status_size - diff; i++) {
		status[i] ^= src.status[i + diff];
	    }
	    diff = diff - status_size;
	    for (; i < status_size; i++) {
		status[i] ^= src.status[i + diff];
	    }
	};

	/**
	 * print some information to cout.
	 */
	void debug_print() const {
	    using namespace std;
	    int w = sizeof(T) * 2;
	    cout << "== debug ==" << endl;
	    cout << "mexp = " << mexp << endl;
	    cout << "word_size = " << get_word_size() << endl;
	    cout << "status_size = " << status_size << endl;
	    cout << dec << "idx:" << idx << endl;
	    for (int i = 0; i < status_size; i++) {
		cout <<  hex << setw(w) << setfill('0') << status[i] << endl;
	    }
	};

	void set_reverse_bit() {
	    reverse_bit = true;
	};

	void reset_reverse_bit() {
	    reverse_bit = false;
	};
	bool get_reverse_bit() {
	    return reverse_bit;
	};
	/**
	 * construct from user-defined generator.
	 * @param src_gf user-defined generator.
	 */
	linear_generator<T, GF> (GF& src_gf) : gf(src_gf) {
	    mexp = gf.get_mexp();
	    status_size = gf.get_status_size();
	    status = new T[status_size];
	    idx = 0;
	    reverse_bit = false;
	};

	/**
	 * copy constructor
	 * @param src copy source
	 */
	linear_generator<T, GF>
	(const linear_generator<T, GF>& src) : gf(src.gf) {
	    status_size = src.status_size;
	    status = new T[status_size];
	    memcpy(status, src.status, sizeof(T) * src.status_size);
	    idx = src.idx;
	    mexp = gf.get_mexp();
	    reverse_bit = src.reverse_bit;
	};

	/**
	 * destructor
	 */
	~linear_generator<T, GF>() {
	    delete[] status;
	}
    private:
	/** Mersenne exponent */
	int mexp;
	/** number of elements in status array */
	int status_size;
	/** index of status for round robbin */
	int idx;
	/** user defined random number generator algorithm */
	GF gf;
	/** internal state */
	T *status;
	bool reverse_bit;
    };
}
#endif
