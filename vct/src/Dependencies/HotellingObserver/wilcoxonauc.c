#include <Meschach/matrix.h>
#include "iq.h"

/*!
 * \fn sum_thresh
 * \brief compute sum(x > thresh), where x has size 1x(dim_n)
 * \param[in] x: pointer to input vector data
 * \param[in] thresh: the threshold value
 * \return int: the the sum up to the threshold
 */
static int sum_thresh(VEC * x, double thresh)
{
	int n,dim_n=x->dim,sum=0;
	for (n=0;n<dim_n;n++)
		sum += (x->ve[n] > thresh);
	return sum;
}

/*!
 * \fn trapz
 * \brief Trapezoidal numerical integration
 *		  n (from matlab)
 *		  z = trapz(x,y)
 *	      z = diff(x).' * (y(1:m-1,:) + y(2:m,:))/2;
 * \param[in] x: pointer to vector data
 * \param[in] y: pointer to vector data
 * \return double: difference (approximate derivative)
 */
static double trapz(VEC * x, VEC * y) //x:false		// y:true
{
	double dx, z = 0;	
	int dim_m, m;
	dim_m = x->dim;

	for (m=0;m<dim_m-1;m++)
	{
		dx = x->ve[m+1]-x->ve[m];
		z += dx * (y->ve[m]+y->ve[m+1]);
	}
	return 0.5 * z;
}

void WilcoxonAUC(VEC * Sout, VEC * Nout, double * AUC, VEC **ptpf, VEC **pfpf)
{
	VEC * xs, * tpf, * fpf;
	int m,i;
	int unique_cnt, cnt;
	double last_val, thresh;

	xs = v_get(Sout->dim + Nout->dim);
	//mv_move(Sout, 0, 0, Sout->m, Sout->n, xs, 0);
	//mv_move(Nout, 0, 0, Nout->m, Nout->n, xs, Sout->n);
	v_move(Sout, 0, Sout->dim, xs, 0);
	v_move(Nout, 0, Nout->dim, xs, Sout->dim);
	v_sort(xs, PNULL);

	/* get the unique elements */
	unique_cnt=0;
	last_val=-1e10;
	for (m=0; m<xs->dim; m++)
	{
		xs->ve[unique_cnt] = xs->ve[m];
		if (last_val!=xs->ve[m])
		{
			last_val=xs->ve[m];
			unique_cnt++;
		}
	}
	
	tpf = v_get(unique_cnt+1);
	fpf = v_get(unique_cnt+1);
	cnt = 0;

	/* go backwards so that the ROC curve starts at (0,0). */
	for (m=unique_cnt-1; m>=0; m--, cnt++)
	{
		thresh = xs->ve[m];
		tpf->ve[cnt] = sum_thresh(Sout, thresh)*1.0/Sout->dim;
		fpf->ve[cnt] = sum_thresh(Nout, thresh)*1.0/Nout->dim;
	}
	tpf->ve[cnt] = 1.0;
	fpf->ve[cnt] = 1.0;
	tpf->ve[0] = 0.0;
	fpf->ve[0] = 0.0;


	*ptpf = tpf;
	*pfpf = fpf;
	*AUC = trapz(fpf,tpf);	

	v_free(xs);
}
