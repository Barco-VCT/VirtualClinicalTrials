// g++ -o test-box-muller test-box-muller.cpp ../../software/repos/branches/mevic_platform/vs2005/dev_aleva/mevic/src/Dependencies/rng/mersenne.cpp ../../software/repos/branches/mevic_platform/vs2005/dev_aleva/mevic/src/Dependencies/rng/sfmt.cpp ../../software/repos/branches/mevic_platform/vs2005/dev_aleva/mevic/src/Dependencies/rng/stoc1.cpp ../../software/repos/branches/mevic_platform/vs2005/dev_aleva/mevic/src/Dependencies/rng/userintf.cpp -I ../../software/repos/branches/mevic_platform/vs2005/dev_aleva/mevic/src/Dependencies/rng
#include <math.h>
#include <stdio.h>

#include "randomc.h"
#include "stocc.h"

#include "test-box-muller.h"

typedef double val_t; // select float, double or long double
//#define sqrt sqrtf
//#define log logf
//#define sincos sincosf

typedef struct {
	val_t x;
	val_t y;
} val2_t;

typedef struct {
	unsigned long x;
	unsigned long y;
} ui2_t;

//using namespace std;

// The para 'u' must contain two uniform distr rand nrs in the range (0.0, 1.0].
static val2_t normal_distr_v2(val2_t u, val_t mean, val_t stddev) {
	// Use the box-muller transformation, the one without rejection sampling.
	val_t r   = sqrt(-2.0 * log(u.x)) * stddev;
	val_t phi = 2.0 * M_PI * u.y;
//	u.y = sincos(phi, &u.x); // Note: this is the OpenCL sincos: s = sincos(phi, &c); GNU: sincos(phi, &s, &c);
	sincos(phi, &u.y, &u.x);
	u.x = u.x * r + mean;
	u.y = u.y * r + mean;
	return u;
}
#if NEEDED
static val2_t BoxMuller(val2_t u, val_t mean, val_t stddev){
    val_t   r = sqrt(-2.0f * log(u.x));
    val_t phi = 2.0 * M_PI * u.y;
    u.x = r * cos(phi) * stddev + mean;
    u.y = r * sin(phi) * stddev + mean;

    return u;
}
#endif
#if NOTDEF
#define PI 3.14159265358979f
__device__ inline void BoxMuller(float& u1, float& u2){
    float   r = sqrtf(-2.0f * logf(u1));
    float phi = 2 * PI * u2;
    u1 = r * __cosf(phi);
    u2 = r * __sinf(phi);
}
#endif
int main() {
    CRandomMersenne rng(12345U);
    int nnumbers = 1000000; // must be even
    int buckets[40] = {0};
    int nbuckets = sizeof(buckets)/sizeof(*buckets);
    const val_t mean = nbuckets/2.0;
    const val_t stddev = 4.0;
    int nstar_cols = 80;
    val_t scale = 1.0/1000.0;//10 * (val_t)nstar_cols / nnumbers;
    int i, j;

#if RANDONLYTEST
// SFMT (SIMD) stoc1 Normal() seems about 40% slower, single core, TIG Linux server, than default non-SIMD
StochasticLib1 sto(12345U);
val_t x = 0.0;
for (i = 0; i < 100000000; i++) {
	x += sto.Normal(mean, stddev);
}
printf("%f\n", x);
return 0;
#endif


val_t min = 1000000.0, max = -1000000.0;
    for(i=0; i<nnumbers/2; i++) {
	//ui2_t ui2; ui2.x = rng.BRandom(); ui2.y = rng.BRandom(); // [0,4294967295]
//	val2_t u2 = {rand_nrs[i], rand_nrs[i+1]};
	val2_t u2;
	u2.x = (val_t)rng.Random(); u2.y = (val_t)rng.Random(); // [0.0, 1.0)
	//u2.x = ui2.x * (1.0/4294967296.0); u2.y = ui2.y * (1.0/4294967296.0);
	//u2.x = ui2.x * (1./((double)(unsigned long)(-1L)+1.)); u2.y = ui2.y * (1./((double)(unsigned long)(-1L)+1.));

	u2.x = 1.0 - u2.x;
	u2.y = 1.0 - u2.y;
	u2 = normal_distr_v2(u2, mean, stddev);
if (u2.x < min) min = u2.x;
if (u2.y < min) min = u2.y;
if (u2.x > max) max = u2.x;
if (u2.y > max) max = u2.y;

	int x = (int)(u2.x/* * nbuckets*/);
        if (x >= 0 && x < nbuckets) {
		buckets[x]++;
	}// else printf("out of histo: %d\n", x);

	int y = (int)(u2.y/* * nbuckets*/);
        if (y >= 0 && y < nbuckets) {
		buckets[y]++;
	}// else printf("out of histo: %d\n", y);
    }
printf("nrs: min=%.20f max=%.20f\n", min, max);

    int minb = 1000000, maxb = -1000000;
    for(i=0; i<nbuckets; i++) {
	int b = buckets[i];
	if (b < minb) minb = b;
	if (b > maxb) maxb = b;
    }
    printf("buckets: minb=%d maxb=%d\n", minb, maxb);

    for(i=0; i<nbuckets; i++) {
	printf("%06d ", buckets[i]);
	buckets[i] = (int)(buckets[i] * scale + 0.5);
        for(j=0; j<buckets[i]; j++) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}

