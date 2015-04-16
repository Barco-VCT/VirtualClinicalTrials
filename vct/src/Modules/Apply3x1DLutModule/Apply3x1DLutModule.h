//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Apply3x1DLutModule.h
 * \brief Implementation of class Apply3x1DLutModule
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

#ifndef APPLY3X1DMODULE_H
#define APPLY3X1DMODULE_H

#include <Module.h>
#include <lut/LookUpTable.h>

#include <string>
#include <vector>

/*!
 * \class Apply3x1DLutModule
 * \brief implementation of a static Channelized Hotelling Observer with LG channels
 */
class Apply3x1DLutModule : public Module
{
public:

	/*!
	 * Constructor 
	 */
	Apply3x1DLutModule( );

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
	~Apply3x1DLutModule( );

	private:

		LookUpTable *m_lutR, *m_lutG, *m_lutB;
		std::string m_pathR, m_pathG, m_pathB;
		unsigned m_nbElemLut;
		unsigned m_nbBits;
		unsigned m_max;

		float getLutMax(LookUpTable *lut);

};

#endif // APPLY3X1DMODULE_H

