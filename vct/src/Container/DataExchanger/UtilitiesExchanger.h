//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file UtilitiesExchanger.h
 * \brief Implementation of the class UtilitiesExchanger
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


#ifndef UTILITIESEXCHANGER_H
#define UTILITIESEXCHANGER_H

#include  "UtilitiesInterface.h"
#include <map>
#include <string>

/*!
 * \class Generic data exchanger between modules
 */ 
class UtilitiesExchanger
{
public:
	/*!
 	 *  Default constructor of the class UtilitiesExchanger
	 */
	UtilitiesExchanger();

	/*!
	 * \fn addUtilityObject
	 * \brief function to add a utility object in the map
	 * \param exchangerName: the name of the interace (key)
	 * \param ui: the interface to add
	 * \return bool: true if no error detected
	 */
	bool addUtilityObject(const std::string& exchangerName, UtilitiesInterface* ui);

	/*!
	 * \fn requestUtilityObject
	 * \brief Function to get a utility object in the map
	 * \param exchangerName: the name of the interface (key)
	 * \return UtilitiesInterface*: the requested object
	 */
	UtilitiesInterface* requestUtilityObject(const std::string& exchangerName);

	/*!
 	 *  Default destructor of the class UtilitiesExchanger
	 */
	~UtilitiesExchanger();

private:
	std::map<std::string,UtilitiesInterface*> utilitiesMap; //!< map module name->interface
};

#endif // UTILITIESEXCHANGER_H