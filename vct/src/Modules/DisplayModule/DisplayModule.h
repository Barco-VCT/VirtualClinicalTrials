//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file DisplayModule.h
 * \brief Implementation of class DisplayModule
 * \date 16-Dec-2009
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

#ifndef DisplayModule_H
#define DisplayModule_H

#include <DSContainer.h>	// from VCT
#include <Module.h>
#include <utilities/UtilitiesDisplay.h>

#include <vector>	// from the standard library

#include <boost/algorithm/string.hpp> // to use Boost's string manipluation operations
#include <boost/lexical_cast.hpp> // to use Boost's string casting operations

/**
 * This class allows to instantiate 
 * a display (native curve and m_frequency)
 */
class DisplayModule : public Module
{

public:
	/*
	 * Constructor of the class DisplayModule
	 */
	DisplayModule();
	
	/** 
	 * Method Simulate. 
	 * In this module, nothing is done in the container!!
	 * Main function for the simulation
	 * @param list: list of containers to process
	 * @return void
	 */
	void Simulate( std::vector<DSContainer*>& container);

	/**
	 * method SetParameter
	 * Method allowing to set the value of a class parameter
	 * @param name:  name of the parameter
	 * @param value: value of the parameter, string type
	 * @return void
	 */
	void SetParameter(const std::string & name, const std::string & value );

	/*
	 * Destructor of the class DisplayModule
	 */
	~DisplayModule();

private:
	UtilitiesDisplay* m_ud;
	int m_nbBits;
	int m_frequency;
	std::string m_filenameNativeCurveGray;
	std::string m_filenameNativeCurveRed;
	std::string m_filenameNativeCurveGreen;
	std::string m_filenameNativeCurveBlue;
	std::string m_filenameNativeCurveCyan;
	std::string m_filenameNativeCurveMagenta;
	std::string m_filenameNativeCurveYellow;
	int m_1Color_0Monochrome;
	int m_screenWidth, m_screenHeight, m_imageTopColumnPosition, m_imageTopRowPosition;
	double m_pitchPixelColumnDirection, m_pitchPixelRowDirection;
};

#endif
