//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file UtilitiesInterface.h
 * \brief Implementation of the class UtilitiesInterface
 * \date 4-May-2009
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
#ifndef UTILITIESINTERFACE_H
#define UTILITIESINTERFACE_H

/*!
 * \class UtilitiesInterface
 * \brief virtual interface to manage data exchange between modules.
 */ 
class UtilitiesInterface
{
public:
	/*!
	*  Default constructor of the class UtilitiesInterface
	*/
	UtilitiesInterface();

	/*!
	 *  Default destructor of the class UtilitiesInterface
	 */
	virtual ~UtilitiesInterface();		
};

#endif // UTILITIESINTERFACE_H