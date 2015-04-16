#ifndef SEARCH_HPP
#define SEARCH_HPP
/**
 * @file search.hpp
 *
 * @brief search parameters so that the random number generator's state
 * transition function has an irreducible characteristic polynomial.
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
#include <tr1/memory>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include "mt19937.hpp"

namespace MTToolBox {
    /**
     * @class Search
     * search parameters so that the generator's state transition function
     * has an irreducible characteristic polynomial.
     * 1) call start() function.
     * 2) if start() returns true, then call get_random(), get_minpoly(),
     * or get_count().
     *
     * @tparam T generators class
     */
    template<class T> class Search {
    public:

	/**
	 * generate random parameters and check if the generator's
	 * state transition function has an irreducible characteristic
	 * polynomial. If found in \b try_count times, return true, else
	 * return false.
	 *
	 * @param try_count
	 */
	bool start(int try_count) {
	    long mexp = rand->get_mexp();
	    long degree;
	    for (int i = 0; i < try_count; i++) {
		rand->setup_param(*mt);
		rand->seeding(1);
		minpoly = rand->get_minpoly();
		count++;
		degree = deg(*minpoly);
		if (degree != mexp) {
		    continue;
		}
		if (IterIrredTest(*minpoly)) {
		    return true;
		}
	    }
	    return false;
	};

	/**
	 * call this function after start() has returned true.
	 * @return random number generator class with parameters.
	 */
	std::tr1::shared_ptr<T> get_random() const {
	    return rand;
	};

	/**
	 * call this function after start() has returned true.
	 * In this program, if minimal polynomial is irreducible,
	 * then the polynomial is characteristic polynomial of
	 * generator's state transition function.
	 *
	 * @return minimal polynomial of generated sequence.
	 */
	std::tr1::shared_ptr<NTL::GF2X> get_minpoly() const {
	    return minpoly;
	};

	/**
	 * @return tried count after this class has created.
	 */
	int get_count() const {
	    return count;
	};

	/**
	 * @param rand_ random number generator whose parameters are
	 * searched.
	 * @param seed this seed is given to Mersenne Twister, not to
	 * \b rand_ generator.
	 */
	Search(const T& rand_, int seed) {
	    rand = rand_.clone();
	    std::tr1::shared_ptr<MersenneTwister> m(new MersenneTwister(seed));
	    mt = m;
	    count = 0;
	};

	/**
	 * @param rand_ random number generator whose parameters are
	 * searched.
	 * @param seed this seed is given to Mersenne Twister, not to
	 * \b rand_ generator. \b seed is string.
	 */
	Search(const T& rand_, const std::string& seed) {
	    rand = rand_.clone();
	    std::tr1::shared_ptr<MersenneTwister> m(new MersenneTwister(seed));
	    mt = m;
	    count = 0;
	};
    private:
	std::tr1::shared_ptr<T> rand;
	std::tr1::shared_ptr<NTL::GF2X> minpoly;
	std::tr1::shared_ptr<MersenneTwister> mt;
	int count;
    };
}
#endif
