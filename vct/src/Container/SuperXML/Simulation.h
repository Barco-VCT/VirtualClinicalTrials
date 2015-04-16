//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Simulation.h
 * \brief Implementation of the class Simulation
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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "AbstractSimulation.h"
#include "../Dependencies/xml/xmlParser.h"
#include "AbstractParameter.h"
#include "InOutManagement.h"
#include "../DataExchanger/UtilitiesExchanger.h"
#include "../DataExchanger/ContainerExchanger.h"
#include "../Container/DSContainer.h"
#include "../ErrorManagement/MevicLogger.h"

#include "Chain.h"

#include <map>
#include <vector>
#include <queue>

/*!
 * \class Simulation
 * \brief Simulation class
 */
class Simulation {
public:
	/*!
	 * Constructor
	 */
	Simulation();
	
	/*!
	 * Constructor
	 * \param pMevicLogger: Pointer to thread logger object
	 * \param simulation: XML node of the XML file that describes this simulation
	 */
	Simulation(MevicLogger * pMevicLogger, XMLNode simulation);

	/*!
	 * \fn SetMevicLogger
	 * \brief method for setting the mevic logger
	 * \param pMevicLogger: Pointer to thread logger object
	 * \return void
	 */
	void SetMevicLogger(MevicLogger * pMevicLogger);

	/*!
	 * \fn SetXMLNode
	 * \param simulation: XML node of the XML file that describes this simulation
	 * \return void
	 */
	void SetXMLNode(XMLNode simulation);

	/*!
	 * \fn Initialize
	 * \param pAbstractSimulation: pointer to AbstractSimulation (derived) object
	 * \param localParameters: Reference to local parameters
	 * \param globalParameters: Reference to global parameters
	 * \return bool: true if success; else if not
	 */
	bool Initialize(AbstractSimulation * pAbstractSimulation,
	                std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter *> > > &localParameters,
	                std::map<std::string, AbstractParameter *> &globalParameters);

	/*!
	 * \fn Simulate
	 * \brief run simulation
	 * \return bool: true if success; else if not
	 */
	bool Simulate();

	/*!
	 * Destructor
	 */
	~Simulation();

private:

	static int numberOfSimulations; //<! Number of Simulation objects in use

	std::queue<Chain *> m_listOfChains; //<! List of chains

	InOutManagement * m_pInOutManagement; //<! (Pointer to) Temporary input/output manager

	ContainerExchanger * m_pContainerExchanger; //<! (Pointer to) Container exchanger

	MevicLogger * m_pMevicLogger; //<! (Pointer to) MevicLogger object

	XMLNode m_simulation; //<! XML node of the XML file that describes this simulation
	
	std::string m_simulationName; //<! Simulation name
};

#endif // SIMULATION_H