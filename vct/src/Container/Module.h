//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Module.h
 * \brief Implementation of the class Module
 * \date 1-Dec-2006
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

#ifndef MODULE_H
#define MODULE_H

#include <vector>

#include "DSContainer.h"
#include <DataExchanger/UtilitiesExchanger.h>
#include <DataExchanger/ContainerExchanger.h>
#include <ErrorManagement/MevicLogger.h>

const float pi = 3.14159265358979323846f;

/*!
 * \class Module
 * \brief This class is the core of the modules chain 
 *		  This class can not be instanciated because it is an abstract class
 */ 
class Module
{
public:
	
	/*!
	*  Default constructor of the class Module
	*/
	Module();
	
	/*!
	 * \fn SetUtilitiesExchanger
	 * \brief add by EMPEC for data exchange management
	 * \param pUtilitiesExchanger: The UtilitiesExchanger (pointer) to set in the module
	 * \return void
	 */
	void SetUtilitiesExchanger(UtilitiesExchanger * pUtilitiesExchanger);

	/*!
	 * \fn SetContainerExchanger
	 * \brief assign a ContainerExchanger with this module
	 * \param pContainerExchanger: The ContainerExchanger (pointer) to set in the module
	 * \return void
	 */
	void SetContainerExchanger(ContainerExchanger * pContainerExchanger);

	/*!
	 * \fn SetMevicLogger
	 * \brief Assign a MevicLogger with this module
	 * \param mevicLogger*: pointer to the mevic logger object
	 */
	void SetMevicLogger(MevicLogger * mevicLogger);

	//virtual functions
	/*!
	 * \fn SetParameter
	 * \brief virtual function for the inherited classes for setting the parameter values 
	 *		  from the xml	
	 * \param name: name of the pointer (string)
	 * \param  value: value of the parameter (string)
	 */
	virtual void SetParameter(const std::string & name, const std::string & value )=0;

	/*!
	 * \fn Simulate
	 * \brief virtual function for the inherited classes for running the simulation of one module
	 * \param container: vector containing the containers of the pipeline simulation
	 */
	virtual void Simulate(std::vector<DSContainer*>& container )=0;
	
	/**
	 *  Default destructor of the class Module
	 */
	virtual ~Module();					

private:

	// Copy Constructor (used to create an object from an existing one)
	Module(const Module& oneModule);

protected:

	bool m_debug; //!< for debug mode

	UtilitiesExchanger * m_pUtilitiesExchanger; //!< utility exchanger

	ContainerExchanger * m_pContainerExchanger; //!< container exchanger

	MevicLogger * m_pMevicLogger; //!< mevic logger for the error management
};

#endif // MODULE_H
