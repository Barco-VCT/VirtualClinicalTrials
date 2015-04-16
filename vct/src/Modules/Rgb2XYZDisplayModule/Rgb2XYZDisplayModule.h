//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Rgb2XYZDisplayModule.h
 * \brief Implementation of class Rgb2XYZDisplayModule
 * \date 29-Sept-2014
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

#ifndef Rgb2XYZDisplayModule_H
#define Rgb2XYZDisplayModule_H

#include <Module.h>
#include <utilities/UtilitiesDisplay.h>

/*!
 * \class Rgb2XYZDisplayModule
 * \brief this class allow to convert data from RGB to XYZ values
 */ 
class Rgb2XYZDisplayModule : public Module
{

public:

	/*!
	 *  Default constructor of the class Rgb2XYZDisplayModule
	 */
	Rgb2XYZDisplayModule();

	/*!
	 * \fn Simulate. 
	 * \brief main function for the simulation
	 * \param container: list of containers to process
	 * \return void
	 */
	void Simulate( std::vector<DSContainer*>& container);

	/*!
	 * \fn SetParameter
	 * \brief Method allowing to set the value of a class parameter
	 * \param name:  name of the parameter
	 * \param value: value of the parameter, string type
	 * \return void
	 */
	void SetParameter(const std::string & name, const std::string & value );

	/**
	 *  Default destructor of the class ConversionRGBtoXYZModule
	 */
	~Rgb2XYZDisplayModule();

private:
	UtilitiesDisplay* m_ud;

};

#endif // Rgb2XYZDisplayModule_H
