#include "channel_laguerre.h"
#include <math.h>
#include "iq.h"


// allow different spread in time (i.e., depth)
void Laguerre3D( int dim_y, int dim_x, int dim_z, int j, float a, float aT, float * r2, float *output)
{
	int free_r = 0;
	int kx, ky, kz, i;
	float f1, f2, f3;
	float *val, *tmp;
	MAT *lg1, *ch1;
	float pos_x, pos_y, pos_z;

	if ( r2 == NULL )
	{
		free_r = 1;
		
		pos_x = 0.5f*(dim_x-1);
		pos_y = 0.5f*(dim_y-1);
		pos_z = 0.5f*(dim_z-1);

		// make a temporary r defining coordinate system
		r2 = new float[dim_x*dim_y*dim_z];

		tmp = r2;
		aT*=aT;
		a*=a;

		for ( kz=0; kz<dim_z; kz++ ) 
		{
			for ( ky=0; ky<dim_y; ky++) 
			{
				for ( kx=0; kx<dim_x; kx++) 
				{
			  		*tmp = (kx-pos_x)*(kx-pos_x)/a+(ky-pos_y)*(ky-pos_y)/a+(kz-pos_z)*(kz-pos_z)/aT;
					tmp++;
				}
			}
		}
	}

	/* val = (1/sqrt(2*pi))*2*sqrt(pi)/a * exp(-(pi*r.^2)/(a^2)).*Laguerre1D(j,2*pi*r.^2/(a^2)); */
	/*     = sqrt(2)/a * exp(-(pi*r.^2)/(a^2)) .* Laguerre1D(j,2*pi*r.^2/(a^2)); */
	f1 = 2.0f*float(PI);
	f2 = -float(PI);

	val = output;
	tmp = r2;

	lg1 = m_get( dim_x*dim_y*dim_z, 1 );
	for ( i=0; i<dim_x*dim_y*dim_z; i++ ) 
	{
		lg1->me[i][0] = f1*(*tmp++);
	}
	ch1 = Laguerre1D( j, lg1 );

	tmp = r2;

	for ( i=0; i<dim_x*dim_y*dim_z; i++ ) 
	{
		f3 = f2*(*tmp);
		*val = (float)(exp(f3) * ch1->me[i][0]);
		tmp++;
		val++;
	}

	if ( free_r ) 
	{
		delete [] r2;
	}

 	m_free(lg1);
	m_free(ch1);
}

