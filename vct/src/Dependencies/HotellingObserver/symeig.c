#include <math.h>
#include <Meschach/matrix.h> /* meschach */

#define ROTATE(a,i,j,k,l) g=a[i-1][j-1];h=a[k-1][l-1];a[i-1][j-1]=g-s*(h+g*tau);\
    a[k-1][l-1]=h+s*(g-h*tau);
    
/*!
 * \fn jacobi
 * \brief Computes all eigenvalues and eigenvectors of a real symmetric matrix a[1..n][1..n]. On
 *		  output, elements of a above the diagonal are destroyed. d[1..n] returns the eigenvalues of a.
 *		  v[1..n][1..n] is a matrix whose columns contain, on output, the normalized eigenvectors of
 *		  a. nrot returns the number of Jacobi rotations that were required.
 * \param[in] a: real symmetric matrix a[1..n][1..n]
 * \param[in] n: size of the matrix n
 * \param[out] d: eigen vectors of matrix a
 * \param [out] v: the normalized eigenvectors of a
 * \param[out] nrot: the number of Jacobi rotations that were required
 * \return void
 */
void jacobi(double **a, int n, double d[], double **v, int *nrot)
{
    int j,iq,ip,i;
    float tresh,theta,tau,t,sm,s,h,g,c;
    VEC *b, *z;
    b=v_get(n);
    z=v_get(n);
                                                        /*  Initialize to the identity matrix. */
    for (ip=1;ip<=n;ip++) {
         for (iq=1;iq<=n;iq++) v[ip-1][iq-1]=0.0;
         v[ip-1][ip-1]=1.0;
    }
                                                        /* Initialize b and d to the diagonal*/
    for (ip=1;ip<=n;ip++) {				/* of a.*/
         b->ve[ip-1]=d[ip-1]=a[ip-1][ip-1];
         /* This vector will accumulate terms of the form tapq as in equation (11.1.14).*/
         z->ve[ip-1]=0.0;
    }
    *nrot=0;
    for (i=1;i<=50;i++) {
         sm=0.0;
	 /* Sum oﬀ-diagonal elements.*/
         for (ip=1;ip<=n-1;ip++) {
             for (iq=ip+1;iq<=n;iq++)
                 sm += fabs(a[ip-1][iq-1]);
         }
         /* The normal return, which relies on quadratic convergence to machine underﬂow.*/
         if (sm == 0.0) {
             v_free(z);
             v_free(b);
             return;
         }
         if (i < 4)
             /*...on the ﬁrst three sweeps.*/
             tresh=0.2*sm/(n*n);
         else
             /*...thereafter.*/
             tresh=0.0;
         for (ip=1;ip<=n-1;ip++) {
             for (iq=ip+1;iq<=n;iq++) {
                 g=100.0*fabs(a[ip-1][iq-1]);
                 /*After four sweeps, skip the rotation if the oﬀ-diagonal element is small.*/
                 if (i > 4 && (float)(fabs(d[ip-1])+g) == (float)fabs(d[ip-1])
                      && (float)(fabs(d[iq-1])+g) == (float)fabs(d[iq-1]))
                      a[ip-1][iq-1]=0.0;

              else if (fabs(a[ip-1][iq-1]) > tresh) {
                  h=d[iq-1]-d[ip-1];
                  if ((float)(fabs(h)+g) == (float)fabs(h))
                      /*t = 1/(2θ)*/
                      t=(a[ip-1][iq-1])/h;
                  else {
                  	/*Equation (11.1.10).*/
                      theta=0.5*h/(a[ip-1][iq-1]);
                      t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                      if (theta < 0.0) t = -t;
                  }
                  c=1.0/sqrt(1+t*t);
                  s=t*c;
                  tau=s/(1.0+c);
                  h=t*a[ip-1][iq-1];
                  z->ve[ip-1] -= h;
                  z->ve[iq-1] += h;
                  d[ip-1] -= h;
                  d[iq-1] += h;
                  a[ip-1][iq-1]=0.0;
                  /* Case of rotations 1 ≤ j < p.*/
                  for (j=1;j<=ip-1;j++) {
                      ROTATE(a,j,ip,j,iq)
                  }	
                  /* Case of rotations p < j < q.*/
                  for (j=ip+1;j<=iq-1;j++) {
                      ROTATE(a,ip,j,j,iq)
                  }
                  /* Case of rotations q < j ≤ n.*/
                  for (j=iq+1;j<=n;j++) {
                      ROTATE(a,ip,j,iq,j)
                  }
                  for (j=1;j<=n;j++) {
                      ROTATE(v,j,ip,j,iq)
                  }
                  ++(*nrot);
              }
          }
      }
      for (ip=1;ip<=n;ip++) {
          b->ve[ip-1] += z->ve[ip-1];
           /*Update d with the sum of tapq ,*/
          d[ip-1]=b->ve[ip-1];
          /* and reinitialize z.*/
          z->ve[ip-1]=0.0;
      }
  }
  printf("Too many iterations in routine jacobi\n");
}

/*!
 * \brief Householder reduction of a real, symmetric matrix a[1..n][1..n]. 
 *		  On output, a is replaced by the orthogonal matrix Q effecting the transformation. d[1..n] returns the diagonal elements of the tridiagonal matrix, 
 *		  and e[1..n] the oﬀ-diagonal elements, with e[1]=0. Several statements, as noted in comments, can be omitted if only eigenvalues are to be found,
 *		  in which case a contains no useful information on output. Otherwise they are to be included.
 * \param[in] a: real symmetric matrix a[1..n][1..n]
 * \param[in] n: size of the matrix n
 * \param[out] d: diagonal elements of the tridiagonal matrix
 * \param[out] e: the oﬀ-diagonal elements
 * \return void
 */
void tred2(float **a, int n, float d[], float e[])
{
	int l,k,j,i;
        float scale,hh,h,g,f;
        for (i=n;i>=2;i--) {
		l=i-1;
		h=scale=0.0;
		if (l > 1) {
			for (k=1;k<=l;k++)
				scale += fabs(a[i][k]);		/*Skip transformation.*/
			if (scale == 0.0)
				e[i]=a[i][l];
			else {
				for (k=1;k<=l;k++) {		/*Use scaled a’s for transformation.*/
					a[i][k] /= scale;	/*Form σ in h.*/
					h += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g=(f >= 0.0 ? -sqrt(h) : sqrt(h));
				e[i]=scale*g;			/* Now h is equation (11.2.4).*/
				h -= f*g;			/* Store u in the ith row of a.*/
				a[i][l]=f-g;
				f=0.0;
				for (j=1;j<=l;j++) {
					/* Next statement can be omitted if eigenvectors not wanted */
					/*Store u/H in ith column of a.*/
					a[j][i]=a[i][j]/h;
					/*Form an element of A · u in g.*/
					g=0.0;
					for (k=1;k<=j;k++)
						g += a[j][k]*a[i][k];
					for (k=j+1;k<=l;k++)
						g += a[k][j]*a[i][k];
					/* Form element of p in temporarily unused*/
					e[j]=g/h;
					/*element of e.*/
					f += e[j]*a[i][j];
				}
				/*Form K, equation (11.2.11).*/
				hh=f/(h+h);
				/*Form q and store in e overwriting p.*/
				for (j=1;j<=l;j++) {
					f=a[i][j];
					e[j]=g=e[j]-hh*f;	/*Reduce a, equation (11.2.13).*/
					for (k=1;k<=j;k++)
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		} else
			e[i]=a[i][l];
		d[i]=h;
	}
	/* Next statement can be omitted if eigenvectors not wanted */
	d[1]=0.0;
	e[1]=0.0;
	/* Contents of this loop can be omitted if eigenvectors not
		wanted except for statement d[i]=a[i][i]; */
        /* Begin accumulation of transformation matrices. */
	for (i=1;i<=n;i++) {
		l=i-1;
						/* This block skipped when i=1. */
		if (d[i]) {
			for (j=1;j<=l;j++) {
				g=0.0;		/* Use u and u/H stored in a to form P·Q.*/
			for (k=1;k<=l;k++)
				g += a[i][k]*a[k][j];
			for (k=1;k<=l;k++)
				a[k][j] -= g*a[k][i];
			}
		}
		/* This statement remains.*/
		d[i]=a[i][i];
		/*Reset row and column of a to identity*/
		a[i][i]=1.0;
		/*matrix for next iteration.*/
		for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
	}
}

VEC* symmeig2(MAT* A, MAT* eigvec, VEC* eigval)
{
	int d = A->m;	
	int destroy_eigvec = 0;
	int nrot = 0;
	/* create a copy because jacobi does some overwriting */
	MAT * tmp = m_copy(A, MNULL);
	
	if (eigval == VNULL)
		eigval = v_get(d);
	if (eigvec == MNULL)
	{
		eigvec = m_get(d,d);
		destroy_eigvec = 1;
	}
		
	jacobi(tmp->me, d, eigval->ve, eigvec->me, &nrot);
	
	if (destroy_eigvec)
		m_free(eigvec);
	m_free(tmp);
	return eigval;
}
