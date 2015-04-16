//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ConversionDDL2CDModule.h
 * \brief Implementation of class ConversionDDL2CDModule
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

#ifndef ConversionDDL2CDModule_H
#define ConversionDDL2CDModule_H

#include <Module.h>

#include <utilities/UtilitiesDisplay.h>

/** 
 * This module allows to manage the conversion
 * between Direct Driving Level and Candela/m2
 * (DDL->CD/m2 or CD/m2->DDL)
 */ 
class ConversionDDL2CDModule : public Module
{

public:
	/** 
	* Constructor of the class. 
	*/ 
	ConversionDDL2CDModule();

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
	~ConversionDDL2CDModule();

private:

	std::string m_unit,
	     m_illum,
	     m_obs;
	float m_white_point[3];
	unsigned int m_number_of_bits;
	enum ComponentType m_element_type;
	unsigned int m_input_number_of_frames,
	             m_output_number_of_frames;
	float m_frame_rate;
	int m_last_component_number;
	unsigned long m_width,
	              m_height;
	float m_max,
	      m_min;
	int m_input_component_number,
	    m_output_component_number;
	
	unsigned int m_output_number_of_bits,
	             m_1DDL2CD_0CD2DDL;

	UtilitiesDisplay * m_ud;
};

#endif // ConversionDDL2CDModule_H
