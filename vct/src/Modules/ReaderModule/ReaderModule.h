//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ReaderModule.h
 * \brief Implementation of class ReaderModule
 * \date 1-Feb-2008
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

#include <Module.h>

#ifndef READERMODULE_H_
#define READERMODULE_H_

/*! 
 * \class class ReaderModule
 * \brief This class allows to read a complete container 
 *		  in a binary file, and to store it in the chain
 */ 
class ReaderModule : public Module
{

public:

	/*! 
	 * Constructor of the class. 
	 */ 
	ReaderModule();

	/*!
	 * \fn Simulate
	 * \brief Opens a container file and adds it to containers 
	 *		  Main function for the simulation
 	 * \param list: list of containers to process
	 * \return void
	 */	
	void Simulate(std::vector<DSContainer*>& containers);

	/*!
	 * \fn SetParameter
	 * \brief allowing to set the value of a class parameter
	 * \param name:  name of the parameter
	 * \param value: value of the parameter, string type
	 * \return void
	 */
	void SetParameter(const std::string & name, const std::string & value );

	/*! 
	 * Destructor of the class. 
	 */ 
	~ReaderModule();

private:
	std::string m_filename; //!< filename of the container file to read
	bool m_compression; //!< condition: 1 if the file to read is compressed, 0 otherwise
};

#endif /* READERMODULE_H_ */
