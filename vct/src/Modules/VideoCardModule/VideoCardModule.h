//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file VideoCardModule.h
 * \brief Implementation of class VideoCardModule
 * \date 11-May-2009
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

#ifndef __VideoCardModule_H
#define __VideoCardModule_H

#include <Module.h>

/** 
 * This module simulate the video card.
 * There are a dithering, and a possible 
 * interpolation depending on nb bits required
 * as output.
 */ 
class VideoCardModule : public Module 
{ 
public:
	/** 
	 * Constructor of the class. 
	 */ 
	VideoCardModule();

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

	~VideoCardModule();

private:
	//private attributes
	std::string m_unit,
                m_illum,
                m_obs;
	unsigned long m_width,
	              m_height;
	unsigned int m_number_of_bits_in,
	             m_number_of_bits_out;
	int m_input_component_number,
        m_output_component_number;
    enum ComponentType m_element_type;
	float m_white_point[3];
	float m_frame_rate;
	unsigned int m_output_number_of_frames,
	             m_input_number_of_frames;

	unsigned char m_tab_0_50_frame_1[2][2],
	              m_tab_0_50_frame_2[2][2],
	              m_tab_0_25_frame_1[3][3],
	              m_tab_0_25_frame_2[3][3],
	              m_tab_0_25_frame_3[3][3],
	              m_tab_0_75_frame_1[3][3],
	              m_tab_0_75_frame_2[3][3],
	              m_tab_0_75_frame_3[3][3];

	//private functions
	/** 
	 * Method InitializeDitheringTables. 
	 * Method to initialize the dithering tables
	 * @return void
	 */
	void InitializeDitheringTables();

	/** 
	 * Method Dithering_GRAY. 
	 * Method to process the Dithering on grayscale image
	 * @param container: the container to process
	 * @return void
	 */
	void Dithering_GRAY(DSContainer *container);

	/** 
	 * Method Dithering_RGB. 
	 * Method to process the Dithering on RGB image
	 * @param container: the container to process
	 * @return void
	 */
	void Dithering_RGB(DSContainer *container);

	/** 
	 * Method GetDitheredValue. 
	 * Method to get the dithered value for a pixel
	 * @param frame: the number of the frame
	 * @param row_position: the position in the row
	 * @param column_position: the position in the column
	 * @param value_int: the integer value of the pixel
	 * @return int: the dithered value
	 */
	int GetDitheredValue(int k, int row_position, int column_position, int value_int);
};

#endif // __VideoCardModule_H

