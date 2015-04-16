#ifndef ALL_IN_ONE_HPP
#define ALL_IN_ONE_HPP
/**
 * @file all_in_one.hpp
 *
 * @brief search irreducible polynomial and temper the output.
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
#include <unistd.h>
#include <time.h>
#include "search.hpp"
#include "linear_generator.hpp"
#include "random_util.hpp"
#include "shortest_basis.hpp"
#include "search_temper.hpp"

/**
 * @namespace MTToolBox
 * name space for random number generator development
 */
namespace MTToolBox {
    /**
     * @class all_in_one
     * 1) search parameters of random number generator whose state transition
     * function has an irreducible characteristic polynomial.
     *
     * 2) search tempering parameters for the generator.
     *
     * @tparam T type of generator's output, uint32_t or uint64_t.
     * @tparam G class of generator, always linear_generator with template.
     * @tparam ST tempering parameter search strategy class.
     */
    template<typename T, typename G, typename ST, typename ST2>
    class all_in_one {
    public:
	/**
	 * search and temper
	 *
	 * @param lg linear generator class
	 * @param st tempering parameter search strategy
	 * @param seed seed for MT, which generate random parameters
	 * @param verbose verbose mode, output information about search process.
	 */
	bool search(G& lg, ST *st, ST2 *st2, const std::string& seed,
		    bool verbose = true) {
	    using namespace MTToolBox;
	    using namespace NTL;
	    using namespace std;
	    using namespace std::tr1;

	    int veq[bit_size(T)];
	    Search<G> s(lg, seed);
	    int mexp = lg.get_mexp();
	    bool found = false;
	    for (int i = 0;; i++) {
		if (s.start(10000)) {
		    found = true;
		    break;
		}
		if (verbose) {
		    cout << "not found in " << (i + 1) * 10000 << endl;
		}
	    }
	    if (!found) {
		return false;
	    }
	    if (verbose) {
		time_t t = time(NULL);
		cout << "irreducible parameter is found at " << ctime(&t);
	    }
	    rand = s.get_random();
	    if (verbose) {
		cout << "count = " << s.get_count() << endl;
		rand->out_param(cout);
	    }
	    shared_ptr<GF2X> minpoly = s.get_minpoly();
	    weight = NTL::weight(*minpoly);
	    poly_sha1(sha1, *minpoly);
	    if (verbose) {
		shortest_basis<G, T> sb(*rand, bit_size(T));
		print_binary(cout, *minpoly);
		int delta = sb.get_all_equidist(veq);
		for (int i = 0; i < bit_size(T); i++) {
		    cout << dec << i + 1 << ":" << veq[i]
			 << "(" << mexp / (i + 1) - veq[i] << ")"
			 << endl;
		}
		cout << "delta = " << dec << delta << endl;
		cout << "sha1 = " << sha1 << endl;
	    }
	    (*st)(*rand, false);
	    if (st2 != NULL) {
		(*st2)(*rand, false);
	    }
	    shortest_basis<G, T> sc(*rand, bit_size(T));
	    delta = sc.get_all_equidist(veq);
	    if (verbose) {
		time_t t = time(NULL);
		cout << "tempering parameters are found at " << ctime(&t)
		     << endl;
		rand->out_param(cout);
		for (int i = 0; i < bit_size(T); i++) {
		    cout << dec << i + 1 << ":" << veq[i]
			 << "(" << mexp / (i + 1) - veq[i] << ")"
			 << endl;
		}
		cout << "delta = " << dec << delta << endl;
	    }
	    return true;
	};
	/** getter of rand */
	std::tr1::shared_ptr<G> get_rand() {
	    return rand;
	};
	/** getter of weight */
	int get_weight() {
	    return weight;
	};
	/* getter of delta */
	int get_delta() {
	    return delta;
	};
	/* getter of sha1 */
	std::string get_sha1() {
	    return sha1;
	};
    private:
	/**
	 * the humming weight of the charateristic polynomial.
	 * e.g the number of terms of characteristic polynomial.
	 */
	int weight;
	/**
	 * sum of d(v).
	 */
	int delta;
	/**
	 * searched generator with its parameters.
	 */
	std::tr1::shared_ptr<G> rand;
	/**
	 * @em important.
	 * SHA1 hash of the characteristic polynomial.
	 * In MTGP, the diffrent id does not assure the diffrence of
	 * characteristic polynomials. Instead, the diffrent hash means
	 * diffrent polynomials and diffrent sequences.
	 */
	std::string sha1;
    };

}
#endif
