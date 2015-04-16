//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ChannelFilter.h
 * \brief Implementation of class ChannelFilter
 * \date 1-Dec-2009
 * (C) Copyright 2014 Barco NV and others.
 *
  * This software code is licensed to you under the Barco VCT Research License. The text of this license
 * is found in the file License.TXT which is available in the same repository, in absence of which you need
 * to contact Barco first before you are allowed to engage in any act of familiarization or use of whatever
 * kind. Any familiarization or use of whatever kind of this software implies acceptance of the pre-mentioned
 * license. You warrant that the modifications you make are “original” and are able to be subject to the provisions
 * of the pre-mentioned license. Any contribution containing “non-original” modification require the prior written
 * approval from Barco.
 *
 */

#ifndef CHANNELFILTER_H
#define CHANNELFILTER_H

#include <ErrorManagement/MevicLogger.h>
#include "channel_laguerre.h"

/*!
 * \class ChannelFilter
 * \brief 2d filter channels for the CHO
 */
class ChannelFilter
{
public:

	/*!
	 * \enum ChannelType
	 * \brief Enumeration used to manage type of the channels 
	 */
	enum ChannelType
	{
		LAGUERRE_GAUSS = 1,
	};

	/*! 
	 * Default constructor 
	 */
	ChannelFilter(MevicLogger * pLogger=NULL);

	/*! Constructor 
     * \param[in] ch_width:		width of the channels [pixels] 
     * \param[in] ch_height:	height of the channels [pixels] 
     * \param[in] ch_depth:		depth of the channels [pixels]
     * \param[in] ch_type:		type of the channels: LG, DOG
     * \param[in] ch_num:		number of the channels
     * \param[in] ch_spread:	spread of the LG channels, 0 otherwise 
	 */
	ChannelFilter( 
		int ch_width, int ch_height, int ch_depth, 
		int ch_type, int ch_num, float ch_spread, float ch_Tspread);

	/*! 
	 * Destructor
	 */
	~ChannelFilter( );

 	/*!
	 * \fn
	 * \brief creates the matrix of the channels
     *		  Each channel corresponds to a column vector of the matrix
	 * \return void
	 */
	void create_channel_matrix( );
 	
	/*!
	 * \fn 
	 * \brief accessor for getting the matrix of the channels
	 * \return MAT*: pointer to the matrix of the channels 
	 */
	MAT* get_channel_matrix( ) const;

private:
	int m_width; //!< width of the channels
	int m_height;	//!< height of the channels
	int m_depth; //!< depth of the channels
	int m_type; //!< type of the channels
	int m_number; //!< number of the channels
	float m_lg_spread; //!< spread of the LG the channels, a

	float m_lg_Tspread; //!< allows different spread in time (i.e., depth)

	MAT* m_channel_matrix; //!< matrix which stores the channels
};


#endif // CHANNELFILTER_H

