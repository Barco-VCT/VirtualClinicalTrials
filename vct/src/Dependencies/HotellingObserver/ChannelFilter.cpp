#include "ChannelFilter.h"
#include <misc/misc.h>

#include <iostream> // std::cerr, should use MevicLogger instead

using namespace std;

ChannelFilter::ChannelFilter(MevicLogger * pLogger) 
{ };

ChannelFilter::ChannelFilter( 
  int ch_width, int ch_height, int ch_depth, 
  int ch_type, int ch_num, float ch_spread, float ch_Tspread) 
{
	m_width = ch_width;
	m_height = ch_height;
	m_depth = ch_depth;
	m_type = ch_type;
	m_number = ch_num;
	m_lg_spread = ch_spread;
	m_lg_Tspread = ch_Tspread;
	m_channel_matrix = m_get(m_height*m_width*m_depth,m_number);
};

ChannelFilter::~ChannelFilter( )
{ };

void ChannelFilter::create_channel_matrix( )
{
	switch( m_type ) 
	{
		case LAGUERRE_GAUSS:
			m_channel_matrix = m_get(m_height*m_width*m_depth,m_number);
			if (m_depth==1) // 2D channels
			{
				for (int ch=0; ch<m_number; ch++) 
				{
					MAT *U = Laguerre2D(m_height, m_width, ch, m_lg_spread, NULL);
					for ( int k=0; k<m_height; k++) 
					{
						for ( int l=0; l<m_width; l++) 
						{
							m_channel_matrix->me[k*m_width+l][ch] = U->me[k][l];
						}
					}
					m_free(U);
				} 
			}
			else // 3D channels
			{
				float *U = new float[m_height*m_width*m_depth];
				float *U0=U;
				for (int ch=0; ch<m_number; ch++) 
				{
					Laguerre3D(m_height, m_width, m_depth, ch, m_lg_spread, m_lg_Tspread, NULL, U0);
					U=U0;
					for (int i=0; i<m_height*m_width*m_depth; i++)
					{
						m_channel_matrix->me[i][ch] = (float)*U++;
					}
				}
				delete [] U0; 
			}
			break;
		default:
			std::cerr << "ChannelFilter::create_channel_matrix: Error: Channel type not recognized.";
	}
};

MAT* ChannelFilter::get_channel_matrix( ) const
{
  return m_channel_matrix;
};

