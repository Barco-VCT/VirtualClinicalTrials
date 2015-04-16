#include "channel_laguerre.h"
#include <math.h>

// r^2 is used (never r only)

MAT * Laguerre2D(int dim_m, int dim_n, int j, double a, MAT * r2)
{
	char free_r = 0;
	double f;
	int m, n;
	MAT * val, * tmp;

	if (r2 == NULL)
	{
		double pos_m = 0.5*(dim_m-1);
		double pos_n = 0.5*(dim_n-1);

		free_r = 1;
		r2 = m_get(dim_m, dim_n);
		for (m=0; m<dim_m; m++)
			for (n=0; n<dim_n; n++)
				r2->me[m][n] = (m-pos_m)*(m-pos_m)+(n-pos_n)*(n-pos_n);
	}

	tmp = m_get(dim_m, dim_n);
	f = 2*PI/(a*a);
	for (m=0; m<dim_m; m++)
		for (n=0; n<dim_n; n++)	
			tmp->me[m][n] = f*r2->me[m][n];

	val = Laguerre1D(j, tmp);
	sm_mlt(sqrt(2)/a, val, val); 

	f = -PI/(a*a);
	for (m=0; m<dim_m; m++)
		for (n=0; n<dim_n; n++)	
			val->me[m][n] *= exp(f*r2->me[m][n]);

	if (free_r) m_free(r2);
	m_free(tmp);
	return val;
}
