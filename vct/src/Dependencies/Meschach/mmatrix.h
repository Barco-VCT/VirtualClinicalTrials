
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
		Type definitions for general purpose maths package
*/

#ifndef	MATRIXH

/* RCS id: $Id: matrix.h,v 1.18 1994/04/16 00:33:37 des Exp $ */

#define	MATRIXH	

#ifndef ANSI_C
#define ANSI_C
#endif

#include	"machine.h"
#include        "err.h"
#include 	"meminfo.h"

#ifdef __cplusplus
#define EXTERN_C		extern "C"
#else
#define EXTERN_C		extern
#endif

/* unsigned integer type */
/************************************************************
#ifndef U_INT_DEF
typedef	unsigned int	u_int;
#define U_INT_DEF
#endif
************************************************************/

/* vector definition */
typedef	struct	{
		unsigned int	dim, max_dim;
		Real	*ve;
		} VEC;

/* matrix definition */
typedef	struct	{
		unsigned int	m, n;
		unsigned int	max_m, max_n, max_size;
		Real	**me,*base;	/* base is base of alloc'd mem */
		} MAT;

/* band matrix definition */
typedef struct {
               MAT   *mat;       /* matrix */
               int   lb,ub;    /* lower and upper bandwidth */
               } BAND;


/* permutation definition */
typedef	struct	{
		unsigned int	size, max_size, *pe;
		} PERM;

/* integer vector definition */
typedef struct	{
		unsigned int	dim, max_dim;
		int	*ive;
	        } IVEC;


#ifndef MALLOCDECL
#ifndef ANSI_C
extern	char	*malloc(), *calloc(), *realloc();
#else
#include <stdlib.h>
#endif
#endif /* MALLOCDECL */

/* For creating MEX files (for use with Matlab) using Meschach
   See also: mexmesch.h */
#ifdef MEX
#include	"mex.h"
#define	malloc(len)		mxMalloc(len)
#define	calloc(n,len)		mxCalloc(n,len)
#define	realloc(ptr,len)	mxRealloc(ptr,len)
#define	free(ptr)		mxFree(ptr)
#define	printf			mexPrintf
#ifndef THREADSAFE	/* for use as a shared library */
#define	THREADSAFE 1
#endif
#endif /* MEX */

#ifdef THREADSAFE
#define	STATIC
#else
#define	STATIC	static
#endif /* THREADSAFE */

#ifndef ANSI_C
extern void m_version();
#else
void	m_version( void );
#endif

#ifndef ANSI_C
/* allocate one object of given type */
#define	NEW(type)	((type *)calloc((size_t)1,sizeof(type)))

/* allocate num objects of given type */
#define	NEW_A(num,type)	((type *)calloc((size_t)(num),sizeof(type)))

 /* re-allocate arry to have num objects of the given type */
#define	RENEW(var,num,type) \
    ((var)=(type *)((var) ? \
		    realloc((char *)(var),(size_t)(num)*sizeof(type)) : \
		    calloc((size_t)(num),sizeof(type))))

#define	MEMCOPY(from,to,n_items,type) \
    MEM_COPY((char *)(from),(char *)(to),(size_t)(n_items)*sizeof(type))

#else
/* allocate one object of given type */
#define	NEW(type)	((type *)calloc((size_t)1,(size_t)sizeof(type)))

/* allocate num objects of given type */
#define	NEW_A(num,type)	((type *)calloc((size_t)(num),(size_t)sizeof(type)))

 /* re-allocate arry to have num objects of the given type */
#define	RENEW(var,num,type) \
    ((var)=(type *)((var) ? \
		    realloc((char *)(var),(size_t)((num)*sizeof(type))) : \
		    calloc((size_t)(num),(size_t)sizeof(type))))

#define	MEMCOPY(from,to,n_items,type) \
 MEM_COPY((char *)(from),(char *)(to),(unsigned)(n_items)*sizeof(type))

#endif /* ANSI_C */

/* type independent min and max operations */
#ifndef max
#define	max(a,b)	((a) > (b) ? (a) : (b))
#endif /* max */
#ifndef min
#define	min(a,b)	((a) > (b) ? (b) : (a))
#endif /* min */


#undef TRUE
#define	TRUE	1
#undef FALSE
#define	FALSE	0


/* for input routines */
#define MAXLINE 81


/* Dynamic memory allocation */

/* Should use M_FREE/V_FREE/PX_FREE in programs instead of m/v/px_free()
   as this is considerably safer -- also provides a simple type check ! */

#ifndef ANSI_C

EXTERN_C	VEC *v_get(), *v_resize();
EXTERN_C	MAT *m_get(), *m_resize();
EXTERN_C	PERM *px_get(), *px_resize();
EXTERN_C	IVEC *iv_get(), *iv_resize();
EXTERN_C	int m_free(),v_free();
EXTERN_C  int px_free();
EXTERN_C  int iv_free();
EXTERN_C  BAND *bd_get(), *bd_resize();
EXTERN_C  int bd_free();

#else

/* get/resize vector to given dimension */
EXTERN_C	VEC *v_get(int), *v_resize(VEC *,int);
/* get/resize matrix to be m x n */
EXTERN_C	MAT *m_get(int,int), *m_resize(MAT *,int,int);
/* get/resize permutation to have the given size */
EXTERN_C	PERM *px_get(int), *px_resize(PERM *,int);
/* get/resize an integer vector to given dimension */
EXTERN_C	IVEC *iv_get(int), *iv_resize(IVEC *,int);
/* get/resize a band matrix to given dimension */
EXTERN_C  BAND *bd_get(int,int,int), *bd_resize(BAND *,int,int,int);

/* free (de-allocate) (band) matrices, vectors, permutations and 
   integer vectors */
EXTERN_C  int iv_free(IVEC *);
EXTERN_C  void m_free(MAT *), v_free(VEC *), px_free(PERM *);
EXTERN_C   int bd_free(BAND *);

#endif /* ANSI_C */


/* MACROS */

/* macros that also check types and sets pointers to NULL */
#define	M_FREE(mat)	( m_free(mat),	(mat)=(MAT *)NULL )
#define V_FREE(vec)	( v_free(vec),	(vec)=(VEC *)NULL )
#define	PX_FREE(px)	( px_free(px),	(px)=(PERM *)NULL )
#define	IV_FREE(iv)	( iv_free(iv),	(iv)=(IVEC *)NULL )

#define MAXDIM  	10000001


/* Entry level access to data structures */
/* routines to check indexes */
#define	m_chk_idx(A,i,j)	((i)>=0 && (i)<(A)->m && (j)>=0 && (j)<=(A)->n)
#define	v_chk_idx(x,i)		((i)>=0 && (i)<(x)->dim)
#define	bd_chk_idx(A,i,j)	((i)>=max(0,(j)-(A)->ub) && \
		(j)>=max(0,(i)-(A)->lb) && (i)<(A)->mat->n && (j)<(A)->mat->n)

#define	m_entry(A,i,j)		m_get_val(A,i,j)
#define	v_entry(x,i)		v_get_val(x,i)
#define	bd_entry(A,i,j)		bd_get_val(A,i,j)
#ifdef DEBUG
#define	m_set_val(A,i,j,val)	( m_chk_idx(A,i,j) ? \
	(A)->me[(i)][(j)] = (val) : (error(E_BOUNDS,"m_set_val"), 0.0))
#define	m_add_val(A,i,j,val)	( m_chk_idx(A,i,j) ? \
	(A)->me[(i)][(j)] += (val) : (error(E_BOUNDS,"m_add_val"), 0.0))
#define	m_sub_val(A,i,j,val)	( m_chk_idx(A,i,j) ? \
	(A)->me[(i)][(j)] -= (val) : (error(E_BOUNDS,"m_sub_val"), 0.0))
#define	m_get_val(A,i,j)	( m_chk_idx(A,i,j) ? \
	(A)->me[(i)][(j)] : (error(E_BOUNDS,"m_get_val"), 0.0))
#define	v_set_val(x,i,val)	( v_chk_idx(x,i) ? (x)->ve[(i)] = (val) : \
	(error(E_BOUNDS,"v_set_val"), 0.0))
#define	v_add_val(x,i,val)	( v_chk_idx(x,i) ? (x)->ve[(i)] += (val) : \
	(error(E_BOUNDS,"v_set_val"), 0.0))
#define	v_sub_val(x,i,val)	( v_chk_idx(x,i) ? (x)->ve[(i)] -= (val) : \
	(error(E_BOUNDS,"v_set_val"), 0.0))
#define	v_get_val(x,i)	( v_chk_idx(x,i) ? (x)->ve[(i)] : \
	(error(E_BOUNDS,"v_get_val"), 0.0))
#define	bd_set_val(A,i,j,val)	( bd_chk_idx(A,i,j) ? \
	(A)->mat->me[(A)->lb+(j)-(i)][(j)] = (val) : \
	(error(E_BOUNDS,"bd_set_val"), 0.0))
#define	bd_add_val(A,i,j,val)	( bd_chk_idx(A,i,j) ? \
	(A)->mat->me[(A)->lb+(j)-(i)][(j)] += (val) : \
	(error(E_BOUNDS,"bd_set_val"), 0.0))
#define	bd_get_val(A,i,j)	( bd_chk_idx(A,i,j) ? \
	(A)->mat->me[(A)->lb+(j)-(i)][(j)] : \
	(error(E_BOUNDS,"bd_get_val"), 0.0))
#else /* no DEBUG */
#define	m_set_val(A,i,j,val)	((A)->me[(i)][(j)] = (val))
#define	m_add_val(A,i,j,val)	((A)->me[(i)][(j)] += (val))
#define	m_sub_val(A,i,j,val)	((A)->me[(i)][(j)] -= (val))
#define	m_get_val(A,i,j)	((A)->me[(i)][(j)])
#define	v_set_val(x,i,val)	((x)->ve[(i)] = (val))
#define	v_add_val(x,i,val)	((x)->ve[(i)] += (val))
#define	v_sub_val(x,i,val)	((x)->ve[(i)] -= (val))
#define	v_get_val(x,i)		((x)->ve[(i)])
#define	bd_set_val(A,i,j,val)	((A)->mat->me[(A)->lb+(j)-(i)][(j)] = (val))
#define	bd_add_val(A,i,j,val)	((A)->mat->me[(A)->lb+(j)-(i)][(j)] += (val))
#define	bd_get_val(A,i,j)	((A)->mat->me[(A)->lb+(j)-(i)][(j)])
#endif /* DEBUG */


/* I/O routines */
#ifndef ANSI_C

EXTERN_C	void v_foutput(),m_foutput(),px_foutput();
EXTERN_C  void iv_foutput();
EXTERN_C	VEC *v_finput();
EXTERN_C	MAT *m_finput();
EXTERN_C	PERM *px_finput();
EXTERN_C	IVEC *iv_finput();
EXTERN_C	int fy_or_n(), fin_int(), yn_dflt(), skipjunk();
EXTERN_C	double fin_double();

#else

/* print x on file fp */
EXTERN_C void v_foutput(FILE *fp,const VEC *x),
       /* print A on file fp */
	m_foutput(FILE *fp,const MAT *A),
       /* print px on file fp */
	px_foutput(FILE *fp,const PERM *px);
/* print ix on file fp */
EXTERN_C void iv_foutput(FILE *fp,const IVEC *ix);

/* Note: if out is NULL, then returned object is newly allocated;
        Also: if out is not NULL, then that size is assumed */

/* read in vector from fp */
EXTERN_C VEC *v_finput(FILE *fp,VEC *out);
/* read in matrix from fp */
EXTERN_C MAT *m_finput(FILE *fp,MAT *out);
/* read in permutation from fp */
EXTERN_C PERM *px_finput(FILE *fp,PERM *out);
/* read in int vector from fp */
EXTERN_C IVEC *iv_finput(FILE *fp,IVEC *out);

/* fy_or_n -- yes-or-no to question in string s
        -- question written to stderr, input from fp 
        -- if fp is NOT a tty then return y_n_dflt */
EXTERN_C int fy_or_n(FILE *fp, const char *s);

/* yn_dflt -- sets the value of y_n_dflt to val */
EXTERN_C int yn_dflt(int val);

/* fin_int -- return integer read from file/stream fp
        -- prompt s on stderr if fp is a tty
        -- check that x lies between low and high: re-prompt if
                fp is a tty, error exit otherwise
        -- ignore check if low > high           */
EXTERN_C int fin_int(FILE *fp,const char *s,int low,int high);

/* fin_double -- return double read from file/stream fp
        -- prompt s on stderr if fp is a tty
        -- check that x lies between low and high: re-prompt if
                fp is a tty, error exit otherwise
        -- ignore check if low > high           */
EXTERN_C double fin_double(FILE *fp,const char *s,double low,double high);

/* it skips white spaces and strings of the form #....\n
   Here .... is a comment string */
EXTERN_C int skipjunk(FILE *fp);

#endif /* ANSI_C */


/* MACROS */

/* macros to use stdout and stdin instead of explicit fp */
#define	v_output(vec)	v_foutput(stdout,vec)
#define	v_input(vec)	v_finput(stdin,vec)
#define	m_output(mat)	m_foutput(stdout,mat)
#define	m_input(mat)	m_finput(stdin,mat)
#define	px_output(px)	px_foutput(stdout,px)
#define	px_input(px)	px_finput(stdin,px)
#define	iv_output(iv)	iv_foutput(stdout,iv)
#define	iv_input(iv)	iv_finput(stdin,iv)

/* general purpose input routine; skips comments # ... \n */
#define	finput(fp,prompt,fmt,var) \
	( ( isatty(fileno(fp)) ? fprintf(stderr,prompt) : skipjunk(fp) ), \
							fscanf(fp,fmt,var) )
#define	input(prompt,fmt,var)	finput(stdin,prompt,fmt,var)
#define	fprompter(fp,prompt) \
	( isatty(fileno(fp)) ? fprintf(stderr,prompt) : skipjunk(fp) )
#define	prompter(prompt)	fprompter(stdin,prompt)
#define	y_or_n(s)	fy_or_n(stdin,s)
#define	in_int(s,lo,hi)	fin_int(stdin,s,lo,hi)
#define	in_double(s,lo,hi)	fin_double(stdin,s,lo,hi)


/* special purpose access routines */

/* Copying routines */
#ifndef ANSI_C
EXTERN_C	MAT	*_m_copy(), *m_move(), *vm_move();
EXTERN_C	VEC	*_v_copy(), *v_move(), *mv_move();
EXTERN_C	PERM	*px_copy();
EXTERN_C	IVEC	*iv_copy(), *iv_move();
EXTERN_C  BAND    *bd_copy();

#else

/* copy in to out starting at out[i0][j0] */
EXTERN_C	MAT	*_m_copy(const MAT *in,MAT *out,unsigned int i0,unsigned int j0),
		* m_move(const MAT *in, int, int, int, int, MAT *out, int, int),
		*vm_move(const VEC *in, int, MAT *out, int, int, int, int);
/* copy in to out starting at out[i0] */
EXTERN_C	VEC	*_v_copy(const VEC *in,VEC *out,unsigned int i0),
		* v_move(const VEC *in, int, int, VEC *out, int),
		*mv_move(const MAT *in, int, int, int, int, VEC *out, int);
EXTERN_C	PERM	*px_copy(const PERM *in,PERM *out);
EXTERN_C	IVEC	*iv_copy(const IVEC *in,IVEC *out),
		*iv_move(const IVEC *in, int, int, IVEC *out, int);
EXTERN_C  BAND    *bd_copy(const BAND *in,BAND *out);

#endif /* ANSI_C */


/* MACROS */
#define	m_copy(in,out)	_m_copy(in,out,0,0)
#define	v_copy(in,out)	_v_copy(in,out,0)


/* Initialisation routines -- to be zero, ones, random or identity */
#ifndef ANSI_C
EXTERN_C	VEC     *v_zero(), *v_rand(), *v_ones();
EXTERN_C	MAT     *m_zero(), *m_ident(), *m_rand(), *m_ones();
EXTERN_C	PERM    *px_ident();
EXTERN_C  IVEC    *iv_zero();
#else
EXTERN_C	VEC     *v_zero(VEC *), *v_rand(VEC *), *v_ones(VEC *);
EXTERN_C	MAT     *m_zero(MAT *), *m_ident(MAT *), *m_rand(MAT *),
						*m_ones(MAT *);
EXTERN_C	PERM    *px_ident(PERM *);
EXTERN_C  IVEC    *iv_zero(IVEC *);
#endif /* ANSI_C */

/* Basic vector operations */
#ifndef ANSI_C
EXTERN_C	VEC *sv_mlt(), *mv_mlt(), *vm_mlt(), *v_add(), *v_sub(),
		*px_vec(), *pxinv_vec(), *v_mltadd(), *v_map(), *_v_map(),
		*v_lincomb(), *v_linlist();
EXTERN_C	double	v_min(), v_max(), v_sum();
EXTERN_C	VEC	*v_star(), *v_slash(), *v_sort();
EXTERN_C	double _in_prod(), __ip__();
EXTERN_C	void	__mltadd__(), __add__(), __sub__(), 
                __smlt__(), __zero__();
#else

EXTERN_C	VEC	*sv_mlt(double s,const VEC *x,VEC *out),	/* out <- s.x */
		*mv_mlt(const MAT *A,const VEC *s,VEC *out),	/* out <- A.x */
		*vm_mlt(const MAT *A,const VEC *x,VEC *out),	/* out^T <- x^T.A */
		*v_add(const VEC *x,const VEC *y,VEC *out), 	/* out <- x + y */
                *v_sub(const VEC *x,const VEC *y,VEC *out),	/* out <- x - y */
		*px_vec(PERM *px,const VEC *x,VEC *out),	/* out <- P.x */
		*pxinv_vec(PERM *px,const VEC *x,VEC *out),	/* out <- P^{-1}.x */
		*v_mltadd(const VEC *x,const VEC *y,double s,VEC *out),   /* out <- x + s.y */
#ifdef PROTOTYPES_IN_STRUCT
		*v_map(double (*f)(double),const VEC *x,VEC *y),  
                                                 /* out[i] <- f(x[i]) */
		*_v_map(double (*f)(void *,double),void *p,const VEC *x,VEC *y),
#else
		*v_map(double (*f)(),const VEC *,VEC *), /* out[i] <- f(x[i]) */
		*_v_map(double (*f)(),void *,const VEC *,VEC *),
#endif /* PROTOTYPES_IN_STRUCT */
		*v_lincomb(int,const VEC **,const Real *,VEC *),   
                                                 /* out <- sum_i s[i].x[i] */
                *v_linlist(VEC *out,VEC *v1,double a1,...);
                                              /* out <- s1.x1 + s2.x2 + ... */

/* returns min_j x[j] (== x[i]) */
EXTERN_C	double	v_min(const VEC *, int *), 
     /* returns max_j x[j] (== x[i]) */		
        v_max(const VEC *, int *), 
        /* returns sum_i x[i] */
        v_sum(const VEC *);

/* Hadamard product: out[i] <- x[i].y[i] */
EXTERN_C	VEC	*v_star(const VEC *, const VEC *, VEC *),
                 /* out[i] <- x[i] / y[i] */
		*v_slash(const VEC *, const VEC *, VEC *),
               /* sorts x, and sets order so that sorted x[i] = x[order[i]] */ 
		*v_sort(VEC *, PERM *);

/* returns inner product starting at component i0 */
EXTERN_C	double	_in_prod(const VEC *x, const VEC *y,unsigned int i0),
                /* returns sum_{i=0}^{len-1} x[i].y[i] */
                __ip__(const Real *,const Real *,int);

/* see v_mltadd(), v_add(), v_sub() and v_zero() */
EXTERN_C	void	__mltadd__(Real *,const Real *,double,int),
		__add__(const Real *,const Real *,Real *,int),
		__sub__(const Real *,const Real *,Real *,int),
                __smlt__(const Real *,double,Real *,int),
		__zero__(Real *,int);

#endif /* ANSI_C */


/* MACRO */
/* usual way of computing the inner product */
#define	in_prod(a,b)	_in_prod(a,b,0)

/* Norms */
/* scaled vector norms -- scale == NULL implies unscaled */
#ifndef ANSI_C

EXTERN_C	double	_v_norm1(), _v_norm2(), _v_norm_inf(),
		m_norm1(), m_norm_inf(), m_norm_frob();

#else
               /* returns sum_i |x[i]/scale[i]| */
EXTERN_C	double	_v_norm1(const VEC *x,const VEC *scale),   
               /* returns (scaled) Euclidean norm */
                _v_norm2(const VEC *x,const VEC *scale),
               /* returns max_i |x[i]/scale[i]| */
		_v_norm_inf(const VEC *x,const VEC *scale);

/* unscaled matrix norms */
EXTERN_C double m_norm1(const MAT *A), 
	m_norm_inf(const MAT *A), 
	m_norm_frob(const MAT *A);

#endif /* ANSI_C */


/* MACROS */
/* unscaled vector norms */
#define	v_norm1(x)	_v_norm1(x,VNULL)
#define	v_norm2(x)	_v_norm2(x,VNULL)
#define	v_norm_inf(x)	_v_norm_inf(x,VNULL)

/* Basic matrix operations */
#ifndef ANSI_C

EXTERN_C	MAT *sm_mlt(), *m_mlt(), *mmtr_mlt(), *mtrm_mlt(), *m_add(), *m_sub(),
		*sub_mat(), *m_transp(), *ms_mltadd();

EXTERN_C  BAND *bd_transp(), *sbd_mlt(), *bds_mltadd(), *bd_zero();
EXTERN_C	MAT *px_rows(), *px_cols(), *swap_rows(), *swap_cols(),
             *_set_row(), *_set_col();
EXTERN_C	VEC *get_row(), *get_col(), *sub_vec(),
		*mv_mltadd(), *vm_mltadd(), *bdv_mltadd();

#else

EXTERN_C	MAT	*sm_mlt(double s, const MAT *A,MAT *out), 	/* out <- s.A */
		*m_mlt(const MAT *A,const MAT *B,MAT *out),	/* out <- A.B */
		*mmtr_mlt(const MAT *A,const MAT *B,MAT *out),	/* out <- A.B^T */
		*mtrm_mlt(const MAT *A,const MAT *B,MAT *out),	/* out <- A^T.B */
		*m_add(const MAT *A,const MAT *B,MAT *out),	/* out <- A + B */
		*m_sub(const MAT *A,const MAT *B,MAT *out),	/* out <- A - B */
		*sub_mat(const MAT *A,unsigned int,unsigned int,unsigned int,
			 unsigned int,MAT *out),
		*m_transp(const MAT *A,MAT *out),		/* out <- A^T */
                /* out <- A + s.B */ 
		*ms_mltadd(const MAT *A,const MAT *B,double s,MAT *out);   


EXTERN_C  BAND    *bd_transp(const BAND *in, BAND *out),	/* out <- A^T */
  *sbd_mlt(Real s, const BAND *A, BAND *OUT),		/* OUT <- s.A */
  *bds_mltadd(const BAND *A, const BAND *B,double alpha, BAND *OUT),
  /* OUT <- A+alpha.B */
  *bd_zero(BAND *A);					/* A <- 0 */

EXTERN_C	MAT	*px_rows(const PERM *px,const MAT *A,MAT *out),	/* out <- P.A */
		*px_cols(const PERM *px,const MAT *A,MAT *out),	/* out <- A.P^T */
		*swap_rows(MAT *,int,int,int,int),
		*swap_cols(MAT *,int,int,int,int),
                 /* A[i][j] <- out[j], j >= j0 */
		*_set_col(MAT *A,unsigned int i,const VEC *col,unsigned int j0),
                 /* A[i][j] <- out[i], i >= i0 */
		*_set_row(MAT *A,unsigned int j,const VEC *row,unsigned int i0);

EXTERN_C	VEC	*get_row(const MAT *,unsigned int,VEC *),
		*get_col(const MAT *,unsigned int,VEC *),
		*sub_vec(const VEC *,int,int,VEC *),
                   /* mv_mltadd: out <- x + s.A.y */
		*mv_mltadd(const VEC *x,const VEC *y,const MAT *A,
			   double s,VEC *out),
                  /* vm_mltadd: out^T <- x^T + s.y^T.A */
		*vm_mltadd(const VEC *x,const VEC *y,const MAT *A,
			   double s,VEC *out),
                  /* bdv_mltadd: out <- x + s.A.y */
                *bdv_mltadd(const VEC *x,const VEC *y,const BAND *A,
			    double s,VEC *out);
#endif /* ANSI_C */


/* MACROS */
/* row i of A <- vec */
#define	set_row(mat,row,vec)	_set_row(mat,row,vec,0) 
/* col j of A <- vec */
#define	set_col(mat,col,vec)	_set_col(mat,col,vec,0)


/* Basic permutation operations */
#ifndef ANSI_C

EXTERN_C	PERM *px_mlt(), *px_inv(), *px_transp();
EXTERN_C	int  px_sign();

#else

EXTERN_C	PERM	*px_mlt(const PERM *px1,const PERM *px2,PERM *out),	/* out <- px1.px2 */
		*px_inv(const PERM *px,PERM *out),	/* out <- px^{-1} */
                 /* swap px[i] and px[j] */
		*px_transp(PERM *px,unsigned int i,unsigned int j);

     /* returns sign(px) = +1 if px product of even # transpositions
                           -1 if ps product of odd  # transpositions */
EXTERN_C	int	px_sign(const PERM *);

#endif /* ANSI_C */


/* Basic integer vector operations */
#ifndef ANSI_C

EXTERN_C	IVEC	*iv_add(), *iv_sub(), *iv_sort();

#else

EXTERN_C	IVEC	*iv_add(const IVEC *ix,const IVEC *iy,IVEC *out),  
  /* out <- ix + iy */
		*iv_sub(const IVEC *ix,const IVEC *iy,IVEC *out),  
  /* out <- ix - iy */
  /* sorts ix & sets order so that sorted ix[i] = old ix[order[i]] */
		*iv_sort(IVEC *ix, PERM *order);

#endif /* ANSI_C */


/* miscellaneous functions */

#ifndef ANSI_C

EXTERN_C	double	square(), cube(), mrand();
EXTERN_C	void	smrand(), mrandlist();
EXTERN_C  void    m_dump(), px_dump(), v_dump(), iv_dump();
EXTERN_C MAT *band2mat();
EXTERN_C BAND *mat2band();

#else

EXTERN_C double	square(double x), 	/* returns x^2 */
  cube(double x), 		/* returns x^3 */
  mrand(void);                  /* returns random # in [0,1) */

EXTERN_C void	smrand(int seed),            /* seeds mrand() */
  mrandlist(Real *x, int len);       /* generates len random numbers */

EXTERN_C void    m_dump(FILE *fp,const MAT *a), px_dump(FILE *fp, const PERM *px),
        v_dump(FILE *fp,const VEC *x), iv_dump(FILE *fp, const IVEC *ix);

EXTERN_C MAT *band2mat(const BAND *bA, MAT *A);
EXTERN_C BAND *mat2band(const MAT *A, int lb,int ub, BAND *bA);

#endif /* ANSI_C */


/* miscellaneous constants */
#define	VNULL	((VEC *)NULL)
#define	MNULL	((MAT *)NULL)
#define	PNULL	((PERM *)NULL)
#define	IVNULL	((IVEC *)NULL)
#define BDNULL  ((BAND *)NULL)



/* varying number of arguments */

#ifdef ANSI_C
#include <stdarg.h>

/* prototypes */

EXTERN_C int v_get_vars(int dim,...);
EXTERN_C int iv_get_vars(int dim,...);
EXTERN_C int m_get_vars(int m,int n,...);
EXTERN_C int px_get_vars(int dim,...);

EXTERN_C int v_resize_vars(int new_dim,...);
EXTERN_C int iv_resize_vars(int new_dim,...);
EXTERN_C int m_resize_vars(int m,int n,...);
EXTERN_C int px_resize_vars(int new_dim,...);

EXTERN_C int v_free_vars(VEC **,...);
EXTERN_C int iv_free_vars(IVEC **,...);
EXTERN_C int px_free_vars(PERM **,...);
EXTERN_C int m_free_vars(MAT **,...);

#elif VARARGS
/* old varargs is used */

#include  <varargs.h>

/* prototypes */

EXTERN_C int v_get_vars();
EXTERN_C int iv_get_vars();
EXTERN_C int m_get_vars();
EXTERN_C int px_get_vars();

EXTERN_C int v_resize_vars();
EXTERN_C int iv_resize_vars();
EXTERN_C int m_resize_vars();
EXTERN_C int px_resize_vars();

EXTERN_C int v_free_vars();
EXTERN_C int iv_free_vars();
EXTERN_C int px_free_vars();
EXTERN_C int m_free_vars();

#endif /* ANSI_C */

#endif /* MATRIXH */


