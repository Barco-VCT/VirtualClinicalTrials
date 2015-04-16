#include <math.h>
#include <boost/math/special_functions/erf.hpp>
#include <stdio.h>
#include <Meschach/matrix.h>
#include "iq.h"


double mean( VEC * score )
{
	int n, dim_n=score->dim;
  double sum=0.0;

	for ( n=0; n<dim_n; n++ )
		sum += score->ve[n];

  sum /= dim_n;

	return sum;
}


double var( VEC * score )
{
	int n, dim_n=score->dim;
  double sum=0.0;
  double avg=mean(score);

	for ( n=0; n<dim_n; n++ )
		sum += (avg - score->ve[n])*(avg - score->ve[n]);

  sum /= (dim_n-1);
	
  return sum;
}


double snr_t( VEC * Sout, VEC * Nout )
{
  return ( mean(Sout) - mean(Nout) ) / sqrt( ( 0.5*var(Sout) + 0.5*var(Nout) ) ); 
}


double snr_auc( double auc )
{
  return ( 2*boost::math::erf_inv( 2*auc - 1.0 ) ); 
}
