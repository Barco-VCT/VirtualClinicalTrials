
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
	Header for sparse matrix stuff.
	Basic sparse routines to be held in sparse.c
*/

/* RCS id: $Id: sparse.h,v 1.2 1994/01/13 05:33:36 des Exp $ */

#ifndef SPARSEH

#define SPARSEH 


#include        "matrix.h"


#ifdef __cplusplus
#define EXTERN_C		extern "C"
#else
#define EXTERN_C		extern
#endif

/* basic sparse types */

typedef struct row_elt	{
	int	col, nxt_row, nxt_idx;
	Real	val;
		} row_elt;

typedef struct SPROW {
	int	len, maxlen, diag;
	row_elt	*elt;		/* elt[maxlen] */
		} SPROW;

typedef struct SPMAT {
	int	m, n, max_m, max_n;
	char	flag_col, flag_diag;
	SPROW	*row;		/* row[max_m] */
	int	*start_row;	/* start_row[max_n] */
	int	*start_idx;	/* start_idx[max_n] */
	      } SPMAT;

/* Note that the first allocated entry in column j is start_row[j];
	This starts the chain down the columns using the nxt_row and nxt_idx
	fields of each entry in each row. */

typedef struct pair { int pos;	Real val; } pair;

typedef struct SPVEC {
	int	dim, max_dim;
	pair	*elt;		/* elt[max_dim] */
	       } SPVEC;

#define	SMNULL	((SPMAT*)NULL)
#define	SVNULL	((SPVEC*)NULL)

/* Macro for speedup */
#define	sprow_idx2(r,c,hint)	\
	( ( (hint) >= 0 && (hint) < (r)->len && \
	   (r)->elt[hint].col == (c)) ? (hint) : sprow_idx((r),(c)) )



/* memory functions */

#ifdef ANSI_C
EXTERN_C int sp_get_vars(int m,int n,int deg,...);
EXTERN_C int sp_resize_vars(int m,int n,...);
EXTERN_C int sp_free_vars(SPMAT **,...);
#elif VARARGS
EXTERN_C int sp_get_vars();
EXTERN_C int sp_resize_vars();
EXTERN_C int sp_free_vars();

#endif /* ANSI_C */

/* Sparse Matrix Operations and Utilities */
#ifndef ANSI_C
EXTERN_C	SPMAT	*sp_get(), *sp_copy(), *sp_copy2(),
			*sp_zero(), *sp_resize(), *sp_compact();
EXTERN_C	double	sp_get_val(), sp_set_val();
EXTERN_C	VEC	*sp_mv_mlt(), *sp_vm_mlt();
EXTERN_C	int	sp_free();

/* Access path operations */
EXTERN_C	SPMAT	*sp_col_access();
EXTERN_C	SPMAT	*sp_diag_access();
EXTERN_C  int     chk_col_access();

/* Input/output operations */
EXTERN_C	SPMAT	*sp_finput();
EXTERN_C	void sp_foutput(), sp_foutput2();

/* algebraic operations */
EXTERN_C SPMAT *sp_smlt(), *sp_add(), *sp_sub(), *sp_mltadd();


/* sparse row operations */
EXTERN_C	SPROW	*sprow_get(), *sprow_xpd(), *sprow_merge(), *sprow_mltadd(),
  *sprow_resize(), *sprow_copy();
EXTERN_C SPROW *sprow_add(), *sprow_sub(), *sprow_smlt();
EXTERN_C	double	sprow_set_val();
EXTERN_C	void	sprow_foutput();
EXTERN_C	int	sprow_idx(), sprow_free();

/* dump */
EXTERN_C  void   sp_dump(), sprow_dump();
EXTERN_C  MAT  *sp_m2dense();

#else
EXTERN_C SPMAT	*sp_get(int,int,int), *sp_copy(const SPMAT *),
	*sp_copy2(const SPMAT *,SPMAT *),
	*sp_zero(SPMAT *), *sp_resize(SPMAT *,int,int),
	*sp_compact(SPMAT *,double);
EXTERN_C double	sp_get_val(const SPMAT *,int,int), sp_set_val(SPMAT *,int,int,double);
EXTERN_C VEC	*sp_mv_mlt(const SPMAT *, const VEC *, VEC *), 
        *sp_vm_mlt(const SPMAT *, const VEC *, VEC *);
EXTERN_C int	sp_free(SPMAT *);

/* Access path operations */
EXTERN_C SPMAT	*sp_col_access(SPMAT *);
EXTERN_C SPMAT	*sp_diag_access(SPMAT *);
EXTERN_C int     chk_col_access(const SPMAT *);

/* Input/output operations */
EXTERN_C SPMAT	*sp_finput(FILE *);
EXTERN_C void	sp_foutput(FILE *, const SPMAT *);

/* algebraic operations */
EXTERN_C SPMAT *sp_smlt(const SPMAT *A,double alpha,SPMAT *B),
      *sp_add(const SPMAT *A,const SPMAT *B,SPMAT *C),
      *sp_sub(const SPMAT *A,const SPMAT *B,SPMAT *C),
      *sp_mltadd(const SPMAT *A,const SPMAT *B,double alpha,SPMAT *C);

/* sparse row operations */
EXTERN_C SPROW	*sprow_get(int), *sprow_xpd(SPROW *r,int n,int type),
        *sprow_resize(SPROW *r,int n,int type),
	*sprow_merge(const SPROW *,const SPROW *,SPROW *,int type),
        *sprow_copy(const SPROW *,const SPROW *,SPROW *,int type),
	*sprow_mltadd(const SPROW *r1,const SPROW *r2, double alpha,
		      int j0, SPROW *r_out, int type);
EXTERN_C SPROW *sprow_add(const SPROW *r1,const SPROW *r2, int j0,SPROW *r_out, int type), 
        *sprow_sub(const SPROW *r1,const SPROW *r2, int j0,SPROW *r_out, int type), 
        *sprow_smlt(const SPROW *r1,double alpha, int j0,SPROW *r_out, int type);
EXTERN_C double	sprow_set_val(SPROW *,int,double);
EXTERN_C int      sprow_free(SPROW *);
EXTERN_C int	sprow_idx(const SPROW *,int);
EXTERN_C void	sprow_foutput(FILE *,const SPROW *);

/* dump */
EXTERN_C void    sp_dump(FILE *fp, const SPMAT *A);
EXTERN_C void    sprow_dump(FILE *fp, const SPROW *r);
EXTERN_C MAT	*sp_m2dense(const SPMAT *A,MAT *out);

#endif /* ANSI_C */

/* MACROS */

#define	sp_input()	sp_finput(stdin)
#define	sp_output(A)	sp_foutput(stdout,(A))
#define	sp_output2(A)	sp_foutput2(stdout,(A))
#define	row_mltadd(r1,r2,alpha,out)	sprow_mltadd(r1,r2,alpha,0,out)
#define	out_row(r)	sprow_foutput(stdout,(r))

#define SP_FREE(A)    ( sp_free((A)),  (A)=(SPMAT *)NULL) 

/* utility for index computations -- ensures index returned >= 0 */
#define	fixindex(idx)	((idx) == -1 ? (error(E_BOUNDS,"fixindex"),0) : \
			 (idx) < 0 ? -((idx)+2) : (idx))


/*  NOT USED */

/* loop over the columns in a row */
/*
#define	loop_cols(r,e,code) \
    do { int _r_idx; row_elt *e; SPROW *_t_row;			\
	  _t_row = (r); e = &(_t_row->elt);				\
	  for ( _r_idx = 0; _r_idx < _t_row->len; _r_idx++, e++ )	\
	  {  code;  }  }  while ( 0 )
*/
/* loop over the rows in a column */
/*
#define	loop_cols(A,col,e,code) \
    do { int _r_num, _r_idx, _c; SPROW *_r; row_elt *e;		\
	  if ( ! (A)->flag_col )	sp_col_access((A));		\
	  col_num = (col);						\
	  if ( col_num < 0 || col_num >= A->n )				\
	      error(E_BOUNDS,"loop_cols");				\
          _r_num = (A)->start_row[_c]; _r_idx = (A)->start_idx[_c];	\
	  while ( _r_num >= 0 )  {					\
	      _r = &((A)->row[_r_num]);					\
              _r_idx = sprow_idx2(_r,_c,_r_idx);			\
              if ( _r_idx < 0 )  continue;				\
	      e = &(_r->elt[_r_idx]);	code;				\
	      _r_num = e->nxt_row;	_r_idx = e->nxt_idx;		\
	      } } while ( 0 )

*/

#endif /* SPARSEH */

