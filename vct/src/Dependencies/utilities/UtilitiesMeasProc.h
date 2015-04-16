//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file UtilitiesMeasProc.h
 * \brief Implementation of the class UtilitiesMeasProc
 * \date 09-July-10
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

#ifndef UTILITIESMEASPROC_H
#define UTILITIESMEASPROC_H

#include <DataExchanger/UtilitiesInterface.h>

#include "DataStore.h"


/*!
 * \class UtilitiesMeasProc
 * \brief Manage data exchange between modules.
 *		  It allows to exchange data for modules of the measurements part.
 */ 
class UtilitiesMeasProc:public UtilitiesInterface
{
	public:
	/*!
	 * Default constructor of the class UtilitiesMeasProc
	 */
	UtilitiesMeasProc();//--> more efficient if empty 
	

	/*!
	 * \fn getDataStorePreProc
	 * \brief accessor for getting the DataStore PreProc step
	 * \return DataStore*: pointer to the DataStore PreProc coordinates of the source
	 */
	DataStore* getDataStorePreProc() {return &m_dataStPreProc;}
	
	/*! 
	 * \fn getDataStorePostProc
	 * \brief accessor for getting DataStore PostProc step
	 * \return tDataStore*: pointer to the DataStore PostProc coordinates of the source
	 */
	DataStore* getDataStorePostProc() {return &m_dataStPostProc;}

	/*!
	 *  Default destructor of the class UtilitiesMeasProc
	 */
	~UtilitiesMeasProc(); 

	private:
	DataStore m_dataStPreProc;
	DataStore m_dataStPostProc;
};

#endif // UTILITIESMEASPROC_H
