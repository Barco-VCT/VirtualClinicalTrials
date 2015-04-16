/* [tS,tN] = CHotelling(IS,IN,Temps)

 Compute the channelized Hotelling observer outputs for
 the signal-images given by IS, the noise images given by
 IN, and the templates given by Temps.

 Inputs: 
       IS, IN -- [NPixels X NImages] The images (in vector format)
       Temps  -- [NPixels X NTemplates] The templates

 Outputs: tS,tN -- [NImages X 1] -- the decision variable outputs
                   can use WilcoxanAUC or dprime on this output.

 See Also: WilcoxanAUC
           dprime
           RunExperiment
*/
#include <Meschach/matrix.h>
#include <Meschach/matrix2.h>
#include "iq.h"

#include <stdio.h>

extern 
VEC * symmeig2(MAT* A, MAT* eigvec, VEC* eigval);

/* Compute the average over the columns */
static VEC * mean(MAT * x)
{
	int dim_m = x->m;
	int dim_n = x->n;
	int m,n;
	VEC * val = v_get(dim_m);
	double sum;

	for (m=0;m<dim_m;m++)
	{
		sum = 0;
		for (n=0;n<dim_n;n++)
			sum += x->me[m][n];
		val->ve[m] = sum / dim_n;
	}
	return val;
}

/* Compute the covariance matrix (each column is an observation) */
MAT * cov(MAT * x)
{
	int dim_m, dim_n, m, n, k;
	double sum;
	VEC * mu;
	MAT * val;

	dim_m = x->m;
	dim_n = x->n;
	mu = v_get(dim_m);
	val = m_get(dim_m,dim_m);

	/* Compute the means */
	for (m=0;m<dim_m;m++)
	{
		sum = 0;
		for (n=0;n<dim_n;n++)
			sum += x->me[m][n];
		mu->ve[m] = sum / dim_n;
	}

	/* Average the sample covariances */
	for (n=0;n<dim_n;n++)
	{
		for (m=0;m<dim_m;m++)
			for (k=m;k<dim_m;k++)
				val->me[m][k] += (x->me[m][n] - mu->ve[m]) * (x->me[k][n] - mu->ve[m]);
	}

	for (m=0;m<dim_m;m++)
		for (k=m;k<dim_m;k++)
		{
			val->me[m][k] /= (dim_n-1);
			val->me[k][m] = val->me[m][k];
		}

	v_free(mu);
	return val;
}

/* Computes the inverse of the positive definite matrix (src). 
   If the inverse does not exist, the pseudoinverse is computed by only 
   using eigenvectors with eigenvalues > (constant) x largest eigenvalue */
void m_pseudoinverse(MAT * src, MAT * dst)
{
	int d,i,j;
	MAT * U, * tmp, * D2;
	VEC * D;
	float largest,thresh;

	// singular value decomposition
	d = src->m;
	U = m_get(d, d);
	D = v_get(d);	
	symmeig2(src,U,D);

	// compute the largest eigenvalue
	largest = D->ve[0];
	for (i=1; i<d; i++)
		if (D->ve[i]>largest) largest = D->ve[i];
	thresh = largest * 1e-8;

	D2 = m_get(d, d);
	for (i=0; i<d; i++)
	{
		if (D->ve[i]<thresh)
		{
			// replace the corresponding eigenvector with 0 
			// (to avoid numerical problems)
			for (j=0; j<d; j++) U->me[i][j] = 0;
			D2->me[i][i] = 0.0f;
		}
		else
		{
			// inverse of this diagonal element
			D2->me[i][i] = 1/D->ve[i];
		}
	}

	// reconstruct the inverse matrix, using
	// dst = U*(D^(-1))*U^T
	tmp = m_mlt(U, D2, MNULL);
	mmtr_mlt(tmp, U, dst);
	m_free(tmp);
	m_free(U);
	v_free(D);
}

/* compute the channel responses for a given slice */
void chotelling_chresponse(MAT * IS, MAT *IN, MAT * Temps, MAT * vS, MAT * vN, VEC * sbar, int image_id)
{
	MAT *tvS, *tvN;
	int i;

	tvS = mtrm_mlt(Temps, IS, MNULL);
	tvN = mtrm_mlt(Temps, IN, MNULL);

	for (i=0;i<tvS->m;i++)
	{
		vS->me[i][image_id] = tvS->me[i][0];
		sbar->ve[i] = sbar->ve[i] + tvS->me[i][0];
	}
	for (i=0;i<tvN->m;i++)
	{
		vN->me[i][image_id] = tvN->me[i][0];
		sbar->ve[i] = sbar->ve[i] - tvN->me[i][0];
	}

	m_free(tvS);
	m_free(tvN);
}

/* compute the channel template (training step) */
void chotelling_chtemplate(MAT * IS, MAT *IN, MAT * Temps, VEC ** wCh)
{
	MAT * S;
	MAT * vS;
	MAT * vN;
	VEC * sbar, * tmp;

	/* Compute channel outputs */
	vS = mtrm_mlt(Temps, IS, MNULL);  /* vS = Temps'*IS;*/
	vN = mtrm_mlt(Temps, IN, MNULL);  /* vN = Temps'*IN;*/

	/* compute mean difference image -- signal (average over the columns) */	
	sbar = mean(vS);
	tmp = mean(vN);
	v_sub(sbar,tmp,sbar);

	chotelling_chtemplate_from_outputs(vS, vN, sbar, wCh);

	v_free(tmp); 
	v_free(sbar);
	m_free(vS); 
	m_free(vN); 
}

/* compute the channel template based on the channel outputs */
void chotelling_chtemplate_from_outputs(MAT *vS, MAT *vN, VEC * sbar, VEC ** wCh)
{
	MAT * S, * tmp;

	/* Intra-class scatter matrix */
	S = cov(vN);
	tmp = cov(vS);
	m_add(S,tmp,S);
	sm_mlt(0.5,S,S);	

	/* channel template */
	/* wCh = inv(S) * (Temps' * sbar'); */
	/* Note: the associativity plays a role here; this way we only use
	   small matrices */

  //printm(S, "S");

	m_pseudoinverse(S, tmp);

	//printm(tmp, "S^(-1)");

	*wCh = mv_mlt(tmp, sbar, VNULL);

	//printv(*wCh, "wCh");

	m_free(S); 
	m_free(tmp); 
}

/* compute the decision variables (testing step) */
void chotelling(MAT * vS, MAT *vN, VEC * wCh, VEC ** tS, VEC ** tN)
{
	/* apply the channel template to produce outputs */
	*tS = vm_mlt(vS, wCh, VNULL);
	*tN = vm_mlt(vN, wCh, VNULL);
}
