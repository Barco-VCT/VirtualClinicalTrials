//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Chain.h
 * \brief Implementation of the class Chain
 * \date 13-Nov-2009
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

#ifndef CHAIN_H
#define CHAIN_H

#include <map>
#include <vector>
#include <queue>

#include "AbstractSimulation.h"
#include <xml/xmlParser.h>
#include "AbstractParameter.h"
#include "InOutManagement.h"
#include <DataExchanger/UtilitiesExchanger.h>
#include <DataExchanger/ContainerExchanger.h>
#include <DSContainer.h>
#include <ErrorManagement/MevicLogger.h>

#include <Module.h>



/*!
 * \class Chain
 */
class Chain
{

public:

	/*!
	 * Constructor
	 */
	Chain();
	/*!
	 * Constructor
	 * \param pMevicLogger: pointer to thread logger object
	 * \param chain: XML node of the XML file that describes this chain
	 */
	Chain(MevicLogger* pMevicLogger, XMLNode chain);

	/*!
	 * \fn SetMevicLogger
	 * \param pMevicLogger: pointer to thread logger object
	 * \return void
	 */
	void SetMevicLogger(MevicLogger* pMevicLogger);

	/*!
	 * \fn SetXMLNode
	 * \param chain: chain XML node of the XML file that describes this chain
	 * \return void
	 */
	void SetXMLNode(XMLNode chain);

	/*!
	 * Initialize
	 *
	 * \param AbstractSimulation*: pAbstractSimulation, pointer to AbstractSimulation (derived) object
	 * \param localParameters: reference to local parameters
	 * \param globalParameters: reference to global parameters
	 * \param pInOutManagement: pointer to input/output management object
	 * \param pContainerExchanger: pointer to container exchanger
	 * \return bool: true if success; else if not
	 */
	bool Initialize(AbstractSimulation* pAbstractSimulation,
	                std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter*> > > &localParameters,
	                std::map<std::string, AbstractParameter*> &globalParameters,
	                InOutManagement* pInOutManagement,
	                ContainerExchanger* pContainerExchanger);

	/*!
	 * \fn Simulate
	 * \param pContainerExchanger, pointer to container exchanger
	 * \return bool: true if success; else if not
	 */
	bool Simulate(ContainerExchanger* pContainerExchanger);

	/*!
	 * Destructor
	 */
	~Chain();

private:
	std::queue<Module *> m_listOfModules; //!< List of modules
	
	std::queue<std::string> m_listOfModuleNames; //!< List of module names
	
	std::vector<DSContainer *> m_containers; //!< List of (pointers to) DSContainers

	UtilitiesExchanger * m_pUtilitiesExchanger; //!< (Pointer to) Utilities exchanger

	MevicLogger * m_pMevicLogger; //!< (Pointer to) MevicLogger object

	XMLNode m_chain; //!< XML node of the XML file that describes this chain

	std::string m_chainName; //!< Chain name
};

#endif // CHAIN_H

