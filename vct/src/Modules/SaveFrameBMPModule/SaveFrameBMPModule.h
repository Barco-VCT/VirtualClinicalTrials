//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file SaveFrameBMPModule.h
 * \brief Implementation of class SaveFrameBMPModule
 * \date 1-Feb-2007
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

#ifndef SaveFrameBMPModule_H
#define SaveFrameBMPModule_H

#include <Module.h>

/**
 * This class allows to save
 * image in BMP format (input RGB 8 bits per channel)
 */
class SaveFrameBMPModule : public Module
{

public:

	/** 
	 * Constructor of the class. 
	 */ 
	SaveFrameBMPModule();

	/** 
	 * Method Simulate. 
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

	/** 
	 * Destructor of the class. 
	 */ 
	~SaveFrameBMPModule();

private:
	std::string m_unit;
	std::string m_illum;
	std::string m_obs;

	float m_white_point[3];
	unsigned int m_number_of_bits;
	unsigned int m_input_number_of_frames;
	unsigned long m_width,
	              m_height;
	int m_input_component_number;

	std::string m_filename;
	int m_component_number,
	    m_frame_number,
	    m_number_of_channels;
	bool m_normalization;
};

#endif // SaveFrameBMPModule_H

