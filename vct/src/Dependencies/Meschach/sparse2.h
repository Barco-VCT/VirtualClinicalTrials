
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


/* Sparse matrix factorise/solve header */
/* RCS id: $Id: sparse2.h,v 1.4 1994/01/13 05:33:46 des Exp $ */



#ifndef SPARSE2H

#define SPARSE2H

#include "sparse.h"


#ifdef __cplusplus
#define EXTERN_C		extern "C"
#else
#define EXTERN_C		extern
#endif


#ifdef ANSI_C
EXTERN_C SPMAT	*spCHfactor(SPMAT *A), *spICHfactor(SPMAT *A), *spCHsymb(SPMAT *A);
EXTERN_C VEC	*spCHsolve(SPMAT *CH, const VEC *b, VEC *x);

EXTERN_C SPMAT	*spLUfactor(SPMAT *A,PERM *pivot,double threshold);
EXTERN_C SPMAT	*spILUfactor(SPMAT *A,double theshold);
EXTERN_C VEC	*spLUsolve(const SPMAT *LU,PERM *pivot, const VEC *b,VEC *x),
	*spLUTsolve(SPMAT *LU,PERM *pivot, const VEC *b,VEC *x);

EXTERN_C SPMAT	*spBKPfactor(SPMAT *, PERM *, PERM *, double);
EXTERN_C VEC	*spBKPsolve(SPMAT *, PERM *, PERM *, const VEC *, VEC *);

EXTERN_C VEC	*pccg(VEC *(*A)(),void *A_par,VEC *(*M_inv)(),void *M_par,VEC *b,
						double tol,VEC *x);
EXTERN_C VEC	*sp_pccg(SPMAT *,SPMAT *,VEC *,double,VEC *);
EXTERN_C VEC	*cgs(VEC *(*A)(),void *A_par,VEC *b,VEC *r0,double tol,VEC *x);
EXTERN_C VEC	*sp_cgs(SPMAT *,VEC *,VEC *,double,VEC *);
EXTERN_C VEC	*lsqr(VEC *(*A)(),VEC *(*AT)(),void *A_par,VEC *b,double tol,VEC *x);
EXTERN_C VEC	*sp_lsqr(SPMAT *,VEC *,double,VEC *);
EXTERN_C int	cg_set_maxiter(int);

EXTERN_C void	lanczos(VEC *(*A)(),void *A_par,int m,VEC *x0,VEC *a,VEC *b,
						Real *beta_m1,MAT *Q);
EXTERN_C void	sp_lanczos(SPMAT *,int,VEC *,VEC *,VEC *,Real *,MAT *);
EXTERN_C VEC	*lanczos2(VEC *(*A)(),void *A_par,int m,VEC *x0,VEC *evals,
						VEC *err_est);
EXTERN_C VEC	*sp_lanczos2(SPMAT *,int,VEC *,VEC *,VEC *);
EXTERN_C  void    scan_to(SPMAT *,IVEC *,IVEC *,IVEC *,int);
EXTERN_C  row_elt  *chase_col(const SPMAT *,int,int *,int *,int);
EXTERN_C  row_elt  *chase_past(const SPMAT *,int,int *,int *,int);
EXTERN_C  row_elt  *bump_col(const SPMAT *,int,int *,int *);

#else
EXTERN_C SPMAT	*spCHfactor(), *spICHfactor(), *spCHsymb();
EXTERN_C VEC	*spCHsolve();

EXTERN_C SPMAT	*spLUfactor();
EXTERN_C SPMAT	*spILUfactor();
EXTERN_C VEC	*spLUsolve(), *spLUTsolve();

EXTERN_C SPMAT	*spBKPfactor();
EXTERN_C VEC	*spBKPsolve();

EXTERN_C VEC	*pccg(), *sp_pccg(), *cgs(), *sp_cgs(), *lsqr(), *sp_lsqr();
EXTERN_C int	cg_set_maxiter();

EXTERN_C void	lanczos(), sp_lanczos();
EXTERN_C VEC	*lanczos2(), *sp_lanczos2();
EXTERN_C void    scan_to();
EXTERN_C row_elt  *chase_col();
EXTERN_C row_elt  *chase_past();
EXTERN_C row_elt  *bump_col();

#endif


#endif
