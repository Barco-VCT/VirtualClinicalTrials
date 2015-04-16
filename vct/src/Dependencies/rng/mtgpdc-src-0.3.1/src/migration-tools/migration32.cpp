/**
 * @file migration32.cpp
 *
 * @brief a tool to chagne the output of old version.
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
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include "random_util.hpp"
#include "shortest_basis.hpp"
#include "mtgp32.hpp"
#include "output.hpp"

using namespace std;
using namespace MTToolBox;
using namespace mtgp;

typedef linear_generator<uint32_t, mtgp32> lg32;
typedef search_temper<lg32, uint32_t, 32, 4, 9, 5, true> stlsb32;

int search_lsb(mtgp32 &mtgp);
int read_line(mtgp_param<uint32_t>& param, istream& is,
	      int *weight, int *delta);
int set_param(mtgp_param<uint32_t>& param, char buffer[],
	       int *weight, int *delta);
void get_sha1(char sha1[], char line[]);
int get_equidist(mtgp32& mtgp, bool lsb);

int main(int argc, char** argv) {
    if (argc < 2) {
	cout << "usage:" << endl;
	cout << argv[0] << " infile" << endl;
	return 1;
    }
    ifstream ifs(argv[1]);
    if (! ifs) {
	cout << "can't open file:" << argv[1] << endl;
	return 1;
    }
    mtgp_param<uint32_t> param;
    bool first = true;
    int weight;
    int delta;
    int delta2;
    int rc;
    cout << "#This file is generated by migration32" << endl;
    while (true) {
	rc = read_line(param, ifs, &weight, &delta);
	if (rc < 0) {
	    break;
	}
	mtgp32 mtgp(param.mexp, param.id);
	mtgp.fill_table(param.p, param.tbl, 16);
	mtgp.fill_table(param.tp, param.tmp_tbl, 16);
	mtgp.set_param(param);
#if defined(DEBUG)
	//output_params_stream(weight, delta, param, false, cout);
	mtgp.out_param(cout);
#endif
	search_lsb(mtgp);
	delta2 = get_equidist(mtgp, false);
#if defined(DEBUG)
	if (delta < delta2) {
	    cout << "delta got worse" << endl;
	    cout << "old:" << dec << delta << endl;
	    cout << "new:" << dec << delta2 << endl;
	}
#endif
	param = mtgp.get_param();
#if defined(DEBUG)
	mtgp.out_param(cout);
#endif
	output_params_stream(weight, delta2, param, first, cout);
	first = false;
	if (ifs.eof()) {
	    break;
	}
	if (ifs.bad()) {
	    cerr << "Error occured at input file." << endl;
	    break;
	}
    }
    ifs.close();
    return 0;
}

int read_line(mtgp_param<uint32_t>& param, istream& is,
	      int *weight, int *delta) {
    const int size = 2000;
    char line[size + 1];
    int rc = 0;

    while (true) {
	is.getline(line, size);
	if (line[0] == '#') {
	    continue;
	}
	rc = set_param(param, line, weight, delta);
	break;
    }
    return rc;
}

int set_param(mtgp_param<uint32_t>& param, char buffer[],
	       int *weight, int *delta) {
    char * p;
    memset(param.sha1, 0, sizeof(param.sha1));
    p = strtok(buffer, ",");
    if (p == NULL) {
	return -1;
    }
    strncpy(param.sha1, p+1, sizeof(param.sha1)-1);
    param.sha1[sizeof(param.sha1)-1] = '\0';
    p = strtok(NULL, ",");
    param.mexp = strtol(p, NULL, 10);
    p = strtok(NULL, ","); // uint32_t
    p = strtok(NULL, ",");
    param.id = strtol(p, NULL, 10);
    p = strtok(NULL, ",");
    param.pos = strtol(p, NULL, 10);
    p = strtok(NULL, ",");
    param.sh1 = strtol(p, NULL, 10);
    p = strtok(NULL, ",");
    param.sh2 = strtol(p, NULL, 10);
    for (int i = 0; i < 4; i++) {
	p = strtok(NULL, ",");
	param.tbl[i] = strtoul(p, NULL, 16);
    }
    for (int i = 0; i < 4; i++) {
	p = strtok(NULL, ",");
	param.tmp_tbl[i] = strtoul(p, NULL, 16);
    }
    p = strtok(NULL, ",");
    param.mask = strtoul(p, NULL, 16);
    p = strtok(NULL, ",");
    *weight = strtol(p, NULL, 10);
    p = strtok(NULL, ",");
    *delta = strtol(p, NULL, 10);
    return 0;
}

int search_lsb(mtgp32 &mtgp) {
    shared_ptr<lg32> g(new lg32(mtgp));
    stlsb32 st;
    g->seeding(1);
#if defined(DEBUG)
    (*g).debug_print();
    st(*g, true);
#else
    st(*g, false);
#endif
    shared_ptr<mtgp32> m = g->get_random();
    mtgp_param<uint32_t> p = m->get_param();
    mtgp.set_param(p);
    return 0;
}

int get_equidist(mtgp32& mtgp, bool lsb) {
    const int bit_len = 32;
    lg32 r(mtgp);
    shortest_basis<lg32, uint32_t> sb(r, bit_len);
    int veq[bit_len];
    if (lsb) {
	r.set_reverse_bit();
    } else {
	r.reset_reverse_bit();
    }
    int sum = sb.get_all_equidist(veq);
#if defined(DEBUG)
    int mexp = mtgp.get_mexp();
    for (int i = 0; i < bit_len; i++) {
	int delta = (mexp / (i + 1) - veq[i]);
	cout << dec << i << ":" << veq[i] << "(" << delta << ")" << endl;
    }
#endif
    return sum;
}