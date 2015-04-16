//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Pipeline.h
 * \brief Implementation of the class Pipeline
 * \date 01-April-2008
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

#ifndef PIPELINE_H
#define PIPELINE_H

#include <vector>
#include <map>

#include "AbstractSimulation.h"

#include "../Dependencies/xml/xmlParser.h"
#include "../Container/DSContainer.h"
#include "AbstractParameter.h"
#include "InOutManagement.h"

/*!
 * \class Pipeline
 * \brief read and interpret xml file using the "pipeline" way
 *		  The diferent simulations are excecutes using the following way: \n
 *		  The first iteration excecutes the different simulations in the order describes in the XML file \n
 *		  The second one do the same thing, after incremeted the different parameters of one unit, ... \n
 *		  For this file reading is allowed to use the local parameters, the global parameters , ... \n
 *		  as well as the input and output temporary files management (see InOutManagement for more information)
 */
class Pipeline : public AbstractSimulation
{
public:

	/*!
	 * Constructor
	 */
	Pipeline();

	/*!
	 * \fn CheckConsistency
	 * \brief Check XML file consistency
	 * \param root: Reference to the first node in the XML file
	 * \param option: Optional parameters; 0 (default): normal excecution; 1: high-level excecution, warnings ar treated as errors (see AbstractParameter for more detail about warnings)
	 * \return bool: true if success; false if not
	 */
	bool CheckConsistency(XMLNode root, int option = 0);

	/*!
	 * \fn StartSimulation
	 * \brief Start the execution of the different simulations, as described in the XML file
	 * \param PathToXML: Path to the XML file that describes the different simulations
	 * \return bool: true if simulations ran correctly; false if not
	 */
	bool StartSimulation(std::string& PathToXML, std::vector<std::string>& args);

	/*!
	 * Destructor
	 */
	~Pipeline();

private:

	int m_numberOfSimulations; //!< Number of times the pipeline of simulations will be executed

	/*!
	 * Local parameters
	 *
	 * Syntax: <Simulation name, <Module name, <Parameter name, Pointer to the corresponding parameter value> > >\n
	 *     Simulation name: Name of the simulation the parameter(s) refer(s) to\n
	 *     Module name:     Name of the module in the simulation the parameter(s) refer(s) to\n
	 *     Parameter name:  Name of the parameter\n
	 *     Pointer:         Pointer to the parameter value thus identified (use FactoryParameter)
	 */
	std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter *> > > m_localParameters;

	/*!
	 * Global parameters
	 *
	 * Syntax: <Parameter name, Pointer to the corresponding parameter value>\n
	 *     Parameter name:  Name of the parameter (will be a capital character: A, B, ...)\n
	 *     Pointer:         Pointer to the parameter value thus identified (use FactoryParameter)
	 */
	std::map<std::string, AbstractParameter *> m_globalParameters;

	/*!
	 * \fn instantiateLocalParameters
	 * \brief Instantiate local parameters
	 * \param lop: List of local parameters to instantiate
	 * \return bool: true if the instantiation was succesful; false if not
	 */
	bool instantiateLocalParameters(XMLNode lop);
	
	/*!
	 * \fn instantiateGlobalParameters
	 * \brief Instantiate global paramters
	 * \param lop: List of global parameters to instantiate
	 * \return bool: true if the instantiation was succesful; false if not
	 */
	bool instantiateGlobalParameters(XMLNode lop);

	/*!
	 * \fn runSimulation
	 * \brief Execute the different simulations, as described in the XML file
	 * \param tms: XML node "TEMPLATE_MEVIC_SIMULATION" of the XML file that describes the different simulations
	 * \return bool: true if simulations ran correctly; false if not
	 */
	bool runSimulation(XMLNode tms);
};

#endif // PIPELINE_H