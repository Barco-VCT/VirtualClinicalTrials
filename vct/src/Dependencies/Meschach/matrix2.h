
/**************************************************************************
**
** Copyright (C) 1993 David E. Steward & Zbigniew Leyk, all rights reserved.
**
**			     Meschach Library
** 
** This Meschach Library is provided "as is" without any express 
** or implied warranty of any kind with respect to this software. 
** In particular the authors shall not be liable for any direct, 
** indirect, special, incidental or consequential damages arising 
** in any way from use of the software.
** 
** Everyone is granted permission to copy, modify and redistribute this
** Meschach Library, provided:
**  1.  All copies contain this copyright notice.
**  2.  All modified copies shall carry a notice stating who
**      made the last modification and the date of such modification.
**  3.  No charge is made for this software or works derived from it.  
**      This clause shall not be construed as constraining other software
**      distributed on the same medium as this software, nor is a
**      distribution fee considered a charge.
**
***************************************************************************/


/*
	Header file for ``matrix2.a'' library file
*/


#ifndef MATRIX2H
#define MATRIX2H

#include "matrix.h"

/* Unless otherwise specified, factorisation routines overwrite the
   matrix that is being factorised */

#ifndef ANSI_C

EXTERN_C	MAT	*BKPfactor(), *CHfactor(), *LUfactor(), *QRfactor(),
		*QRCPfactor(), *LDLfactor(), *Hfactor(), *MCHfactor(),
		*m_inverse();
EXTERN_C	double	LUcondest(), QRcondest();
EXTERN_C	MAT	*makeQ(), *makeR(), *makeHQ(), *makeH();
EXTERN_C	MAT	*LDLupdate(), *QRupdate();

EXTERN_C	VEC	*BKPsolve(), *CHsolve(), *LUsolve(), *_Qsolve(), *QRsolve(),
		*LDLsolve(), *Usolve(), *Lsolve(), *Dsolve(), *LTsolve(),
		*UTsolve(), *LUTsolve(), *QRCPsolve();

EXTERN_C  BAND    *bdLUfactor(), *bdLDLfactor();
EXTERN_C  VEC     *bdLUsolve(), *bdLDLsolve();

EXTERN_C	VEC	*hhvec();
EXTERN_C	VEC	*hhtrvec();
EXTERN_C	MAT	*hhtrrows();
EXTERN_C	MAT	*hhtrcols(), *_hhtrcols();

EXTERN_C	void	givens();
EXTERN_C	VEC	*rot_vec();	/* in situ */
EXTERN_C	MAT	*rot_rows();	/* in situ */
EXTERN_C	MAT	*rot_cols();	/* in situ */


/* eigenvalue routines */
EXTERN_C	VEC	*trieig(), *symmeig();
EXTERN_C	MAT	*schur();
EXTERN_C	void	schur_evals();
EXTERN_C	MAT	*schur_vecs();

/* singular value decomposition */
EXTERN_C	VEC	*bisvd(), *svd();

/* matrix powers and exponent */
EXTERN_C MAT  *_m_pow();
EXTERN_C MAT  *m_pow();
EXTERN_C MAT  *m_exp(), *_m_exp();
EXTERN_C MAT  *m_poly();

/* FFT */
EXTERN_C void fft();
EXTERN_C void ifft();


#else

                 /* forms Bunch-Kaufman-Parlett factorisation for
                        symmetric indefinite matrices */
EXTERN_C	MAT	*BKPfactor(MAT *A,PERM *pivot,PERM *blocks),
                 /* Cholesky factorisation of A
                        (symmetric, positive definite) */
		*CHfactor(MAT *A),
                /* LU factorisation of A (with partial pivoting) */ 
                *LUfactor(MAT *A,PERM *pivot),
                /* QR factorisation of A; need dim(diag) >= # rows of A */
		*QRfactor(MAT *A,VEC *diag),
                /* QR factorisation of A with column pivoting */
		*QRCPfactor(MAT *A,VEC *diag,PERM *pivot),
                /* L.D.L^T factorisation of A */
		*LDLfactor(MAT *A), 
                /* Hessenberg factorisation of A -- for schur() */
                *Hfactor(MAT *A,VEC *diag1,VEC *diag2),
                /* modified Cholesky factorisation of A;
                        actually factors A+D, D diagonal with no
                        diagonal entry in the factor < sqrt(tol) */
                *MCHfactor(MAT *A,double tol),
		*m_inverse(const MAT *A,MAT *out);

                /* returns condition estimate for A after LUfactor() */
EXTERN_C	double	LUcondest(const MAT *A, PERM *pivot),
                /* returns condition estimate for Q after QRfactor() */
                QRcondest(const MAT *A);

/* Note: The make..() and ..update() routines assume that the factorisation
        has already been carried out */

     /* Qout is the "Q" (orthongonal) matrix from QR factorisation */
EXTERN_C	MAT	*makeQ(const MAT *QR,const VEC *diag,MAT *Qout),
                /* Rout is the "R" (upper triangular) matrix
                        from QR factorisation */
		*makeR(const MAT *A,MAT *Rout),
                /* Qout is orthogonal matrix in Hessenberg factorisation */
		*makeHQ(MAT *A,VEC *diag1,VEC *diag2,MAT *Qout),
                /* Hout is the Hessenberg matrix in Hessenberg factorisation */
		*makeH(const MAT *A,MAT *Hout);

                /* updates L.D.L^T factorisation for A <- A + alpha.u.u^T */
EXTERN_C	MAT	*LDLupdate(MAT *A,VEC *u,double alpha),
                /* updates QR factorisation for QR <- Q.(R+u.v^T)
		   Note: we need explicit Q & R matrices,
                        from makeQ() and makeR() */
		*QRupdate(MAT *Q,MAT *R,VEC *u,VEC *v);

/* Solve routines assume that the corresponding factorisation routine
        has already been applied to the matrix along with auxiliary
        objects (such as pivot permutations)

        These solve the system A.x = b,
        except for LUTsolve and QRTsolve which solve the transposed system
                                A^T.x. = b.
        If x is NULL on entry, then it is created.
*/

EXTERN_C	VEC	*BKPsolve(const MAT *A,PERM *pivot,const PERM *blocks,
			  const VEC *b,VEC *x),
		*CHsolve(const MAT *A,const VEC *b,VEC *x),
		*LDLsolve(const MAT *A,const VEC *b,VEC *x),
		*LUsolve(const MAT *A, PERM *pivot, const VEC *b,VEC *x),
		*_Qsolve(const MAT *A, const VEC *diag, const VEC *b, 
			 VEC *x, VEC *tmp),
		*QRsolve(const MAT *A, const VEC *diag, const VEC *b,VEC *x),
    		*QRTsolve(const MAT *A,const VEC *,const VEC *b,VEC *x),


     /* Triangular equations solve routines;
        U for upper triangular, L for lower traingular, D for diagonal
        if diag_val == 0.0 use that values in the matrix */

		*Usolve(const MAT *A,const VEC *b,VEC *x,double diag_val),
		*Lsolve(const MAT *A,const VEC *b,VEC *x,double diag_val),
		*Dsolve(const MAT *A,const VEC *b,VEC *x),
		*LTsolve(const MAT *A,const VEC *b,VEC *x,double diag_val),
		*UTsolve(const MAT *A,const VEC *b,VEC *x,double diag_val),
                *LUTsolve(const MAT *A,PERM *pivot,const VEC *b, VEC *x),
                *QRCPsolve(const MAT *QR,const VEC *diag,PERM *pivot,
			   const VEC *b,VEC *x);

EXTERN_C  BAND    *bdLUfactor(BAND *A,PERM *pivot),
                *bdLDLfactor(BAND *A);
EXTERN_C  VEC     *bdLUsolve(const BAND *A,PERM *pivot,const VEC *b,VEC *x),
                *bdLDLsolve(const BAND *A,const VEC *b,VEC *x);



EXTERN_C	VEC	*hhvec(const VEC *,unsigned int,Real *,VEC *,Real *);
EXTERN_C	VEC	*hhtrvec(const VEC *,double,unsigned int,const VEC *,VEC *);
EXTERN_C	MAT	*hhtrrows(MAT *,unsigned int,unsigned int,const VEC *,double);
EXTERN_C	MAT	*hhtrcols(MAT *,unsigned int,unsigned int,const VEC *,double);
EXTERN_C	MAT	*_hhtrcols(MAT *,unsigned int,unsigned int,const VEC *,double,VEC *);

EXTERN_C	void	givens(double,double,Real *,Real *);
EXTERN_C	VEC	*rot_vec(const VEC *,unsigned int,unsigned int,
			 double,double,VEC *); /* in situ */
EXTERN_C	MAT	*rot_rows(const MAT *,unsigned int,unsigned int,
			  double,double,MAT *); /* in situ */
EXTERN_C	MAT	*rot_cols(const MAT *,unsigned int,unsigned int,
			  double,double,MAT *); /* in situ */


/* eigenvalue routines */

               /* compute eigenvalues of tridiagonal matrix
                  with diagonal entries a[i], super & sub diagonal entries
                  b[i]; eigenvectors stored in Q (if not NULL) */
EXTERN_C	VEC	*trieig(VEC *a,VEC *b,MAT *Q),
                 /* sets out to be vector of eigenvectors; eigenvectors
                   stored in Q (if not NULL). A is unchanged */
		*symmeig(const MAT *A,MAT *Q,VEC *out);

               /* computes real Schur form = Q^T.A.Q */
EXTERN_C	MAT	*schur(MAT *A,MAT *Q);
         /* computes real and imaginary parts of the eigenvalues
                        of A after schur() */
EXTERN_C	void	schur_evals(MAT *A,VEC *re_part,VEC *im_part);
          /* computes real and imaginary parts of the eigenvectors
                        of A after schur() */
EXTERN_C	MAT	*schur_vecs(MAT *T,MAT *Q,MAT *X_re,MAT *X_im);


/* singular value decomposition */

        /* computes singular values of bi-diagonal matrix with
                   diagonal entries a[i] and superdiagonal entries b[i];
                   singular vectors stored in U and V (if not NULL) */
EXTERN_C VEC	*bisvd(VEC *a,VEC *b,MAT *U,MAT *V),
               /* sets "out" to be vector of singular values;
                   singular vectors stored in U and V */
	*svd(MAT *A,MAT *U,MAT *V,VEC *out);

/* matrix powers and exponent */
EXTERN_C MAT  *_m_pow(const MAT *A, int p, MAT *tmp,MAT *out);
EXTERN_C MAT  *m_pow(const MAT *A, int p, MAT *out);
EXTERN_C MAT  *m_exp(MAT *,double,MAT *);
EXTERN_C MAT  *_m_exp(MAT *A, double eps, MAT *out, int *q_out, int *j_out);
EXTERN_C MAT  *m_poly(const MAT *,const VEC *,MAT *);

/* FFT */
EXTERN_C void fft(VEC *,VEC *);
EXTERN_C void ifft(VEC *,VEC *);

#endif


#endif
