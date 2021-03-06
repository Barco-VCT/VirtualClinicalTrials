//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file SaveFrameRAWModule.h
 * \brief Implementation of class SaveFrameRAWModule
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

#ifndef SaveFrameRAWModule_H
#define SaveFrameRAWModule_H

#include <vector>

#include <Module.h>

/** 
 * class SaveFrameRAWModule
 * This class allow to write any channel of any component of any frame
 * in a binary raw file
 */ 
class SaveFrameRAWModule : public Module
{

public:
	/** 
	 * Constructor of the class. 
	 */ 
	SaveFrameRAWModule();

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
	~SaveFrameRAWModule();

private:
	std::string m_filename;
	int m_component_to_write;
	int m_frame_to_write;
	int m_channel_to_write;
};

#endif // SaveFrameRAWModule_H
