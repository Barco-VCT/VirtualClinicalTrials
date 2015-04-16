#include "channel_laguerre.h"
#include <math.h>

static double prod(int a)
{
	double p = 1;
	for (;a>1;a--) p *= a;
	return p;
}

MAT * Laguerre1D(int j, MAT * x)
{
	int jp, m, n, im, in;
	MAT * val;
	double a;

	m = x->m;
	n = x->n;
	val = m_get(m, n);

	for (jp=0;jp<=j;jp++)
	{
		a = ((jp & 1) ? -1 : 1) * prod(j) / (prod(jp)*prod(j-jp)*prod(jp));
		for (im=0;im<m;im++)
			for (in=0;in<n;in++)
			{
				val->me[im][in] += a * pow(x->me[im][in], jp);
			}
	}
	return val;
}