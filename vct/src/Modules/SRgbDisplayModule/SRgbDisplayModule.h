//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file SRgbDisplayModule.h
 * \brief Implementation of class SRgbDisplayModule
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

#ifndef SRGBDISPLAYMODULE_H
#define SRGBDISPLAYMODULE_H

#include <Module.h>
#include <string>
#include <vector>

/*!
 * \class SRgbDisplayModule
 * \brief implementation of a static Channelized Hotelling Observer with LG channels
 */
class SRgbDisplayModule : public Module
{
public:

	/*!
	 * Constructor 
	 */
	SRgbDisplayModule( );

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
	~SRgbDisplayModule( );

	private:
		float m_gamma;
		unsigned m_contrast;
		float m_lumMax;
		float m_lumMin;
		std::string m_illum;

		std::vector<float> SRgbDisplayModule::sRGB(float R, float G, float B);


};

#endif // SRGBDISPLAYMODULE_H

