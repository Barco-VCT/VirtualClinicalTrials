//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file AbstractSimulation.h
 * \brief Implementation of the class AbstractSimulation
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

#include <iostream>
#include <string>
#include <vector>
#include <DSContainer.h>
#include <FactoryModule.h>
#include <xml/xmlparser.h>
#include <Module.h>
#include "../iConvert/iConvert.h"
#include <ErrorManagement/MevicLogger.h>
#include <map>

#ifndef AbstractSimulation_H
#define AbstractSimulation_H

/*!
 * \class AbstractSimulation
 * \brief Virtual class used for the simulations, A simulation describes a way to excecute the different simulations
 */
class AbstractSimulation
{
public:

	/*!
	 * \fn SetMevicLogger
	 * \param pMevicLogger: Pointer to thread logger object
	 * \return void
	 */
	void SetMevicLogger(MevicLogger * pMevicLogger);

	/*!
	 * \fn CheckConsistency
	 * \brief Check the consistency of the XML file. A consistent file means the number of iterations is coherent with the number of possible values of the dynamic parameters. It cheks also the number of "$" present in the file is coherent with the descripted dynamic parameters.
	 * \param root: reference to the first node of the XML
	 * \param option: optional parameter; 0: default value, normal excecution; 1: high level excecution, the warnings becomes errors, see AbstractParameter for more detail about warnings
	 * \return bool: true if the file is consistent; false otherwise
	 */
	virtual bool CheckConsistency(XMLNode root, int option = 0)=0;

	/*!
	 * \fn StartSimulation
	 * \brief Starts the excecution of the different simulations
	 * \param PathToXML: path to the XML file containing the simulations to excecute.
	 * \param args: arguments
	 * \return bool: true is the simulation performed correctly; false otherwise
	 */
	virtual bool StartSimulation (std::string& PathToXML, std::vector<std::string>& args) = 0;

	/*!
	 * \fn getName
	 * \brief Operation returning the name of the node given in input parameter
	 * \param node: to get the name
	 * \return name: of the node if "module" tag not present. If "module" tag present, returning of the value of this tag
	 */
	std::string getName (XMLNode node);

	/*!
	 * \fn instantiateArgs 
	 * \brief Instentiation of the arguments of the XML file given as parameters
	 *		  The arguments given by this way are not analysed to be processed as dynamic of global parameters
	 * \param args:	2D array of char containing the arguments
	 * \return bool: true if the initialization has been performed correctly; false otherwise
	*/
	bool instantiateArgs(std::vector<std::string>& args);

	/*!
	 * \fn getParamValue
	 * \brief Returns the correct value of the parameter in the case of a # is found inside it, 
	 *		  meaning it has to be replaced by a command line argument, if # is not found, the same value is returned
	 * \param paramValue: string to process
	 * \return string: the same string is # is not found; the string with the corresponding command line argument otherwise
	 */
	std::string getParamValue(std::string paramValue);

	/*!
	 * Destructor
	 */
	virtual ~AbstractSimulation();

private:
	std::map<int, std::string> _args; //!< map containing the arguments given by the command line

protected:
	MevicLogger * m_pMevicLogger; //!< (Pointer to) MevicLogger object
};

#endif

