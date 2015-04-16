//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file DeltaE2000Module.h
 * \brief Implementation of class DeltaE2000Module
 * \date 1-June-2007
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

#ifndef DELTAE2000MODULE_H
#define DELTAE2000MODULE_H

#include <Module.h>
#include <string>
#include <vector>

/*!
 * \class DeltaE2000Module
 * \brief implementation of a module for computing DeltaE2000
 */
class DeltaE2000Module : public Module
{
public:

	/*!
	 * Constructor 
	 */
	DeltaE2000Module( );

	/*!
	 * \fn Simulate
	 * \brief for running the simulation
	 * \param  [in/out] container: vector of the containers 
	 * \return void
	 */
	void Simulate( std::vector<DSContainer*>& container );

	/*!
	 * \fn SetParameter
	 * \brief Method assigns the given value to the specified parameter.
	 * \param  [in] name: name of the parameter
	 * \param  [in] value: value of the parameter
	 * \return void
	 */
	void SetParameter( const std::string & name, const std::string & value );

	/*!
	 * Destructor
	 */
	~DeltaE2000Module( );

	private:
		DSContainer *m_refDSCont, *m_testDSCont;
		int m_index;

};

#endif // DELTAE2000MODULE_H
