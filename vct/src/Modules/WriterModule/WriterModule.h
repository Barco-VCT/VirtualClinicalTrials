//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file WriterModule.h
 * \brief Implementation of class WriterModule
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

#ifndef WRITERMODULE_H_
#define WRITERMODULE_H_

#include <Module.h>

/*! 
 * \class WriterModule
 * \brief This class allows to write a complete container 
 *		  in a binary file
 */ 
class WriterModule : public Module
{

public:

	/*! 
	 * Constructor of the class. 
	 */ 
	WriterModule();

	/*! 
 	 * \fn Simulate
	 * \brief Main function for the simulation
 	 * \param list: list of containers to process
	 * \return void
	 */	
	
	void Simulate(std::vector<DSContainer*>& containers);
	
	/*!
	 * \fn SetParameter
	 * \brief Method allowing to set the value of a class parameter
	 * \param name:  name of the parameter
	 * \param value: value of the parameter, string type
	 * \return void
	 */
	void SetParameter(const std::string & name, const std::string & value );

	/*! 
	 * Destructor of the class. 
	 */ 
	~WriterModule();

private:
	std::string m_filename; //!< filename of the container file to read
	bool m_compression; //!< condition: 1 if the file to save is compressed, 0 otherwise
	bool m_savelastcomponent; //!< condition: 1 for saving only the last component of the container, 0 otherwise
	int m_deletecontainer; //!< condition: 1 for deleting the container, 0 otherwise

};

#endif /* WRITERMODULE_H_ */
