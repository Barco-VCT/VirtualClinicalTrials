//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file SequenceRawGeneratorModule.h
 * \brief Implementation of class SequenceRawGeneratorModule
 * \date 1-Sept-2008
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

#ifndef SequenceRawGeneratorModule_H
#define SequenceRawGeneratorModule_H

#include <Module.h>

#include <iConvert/iConvert.h>


/** 
 * class SequenceRawGeneratorModule
 * This class allows to load a sequence of raw from a folder
 */ 
class SequenceRawGeneratorModule : public Module
{

public:
	/** 
	* Constructor of the class. 
	*/ 
	SequenceRawGeneratorModule();

	/** 
	 * Method Simulate. 
	 * Main function for the simulation
	 * @param list: list of containers to process
	 * @return void
	 */
	void Simulate( std::vector<DSContainer*>& list);

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
	~SequenceRawGeneratorModule();

private:
	std::string m_directory;
	unsigned int m_number_of_slices; // number of slices to load in the frames.
	unsigned int m_number_of_frames; // number of frames to copy in the sequence of the component.

	int	m_width,			// Width of the image
		m_height,			// Height of the image
		m_nbChannels,		// Number of channels of the image (3: RGB, 1: Grayscale)
		m_nbBitsRange,		// Number of bits of the image (recommended: 8 bits)
		m_littleEndian,
		m_crop,
		m_widthCrop,
		m_heightCrop,
		m_ptOriTopLeftX,
		m_ptOriTopLeftY,
		m_normalization,
		m_whiteIs0,
		m_1RGB_0GRAY,
		m_outputXYZ, // if output is XYZ mevic format
		m_nbBitsPrecision,
		m_nbBitsOutput;

	float m_frame_repeat; //allowing non-integer frame repeat
	
	int m_start,
	    m_end,
	    m_nbBytesToRead;
};

#endif // SequenceRawGeneratorModule_H
