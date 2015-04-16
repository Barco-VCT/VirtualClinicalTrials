#include "Simulation.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iConvert/iConvert.h>

using namespace std;
using namespace iConvert;

/*!
 * Static members initialization
 */
int Simulation::numberOfSimulations = 0;

Simulation::Simulation()
:m_pMevicLogger(0),
 m_simulation(),
 m_simulationName("")
{
	numberOfSimulations++;

	m_pInOutManagement    = new InOutManagement();
	m_pContainerExchanger = new ContainerExchanger();
}

Simulation::Simulation(MevicLogger * pMevicLogger, XMLNode simulation)
:m_pMevicLogger(pMevicLogger),
 m_simulation(simulation)
{
	numberOfSimulations++;

	m_pInOutManagement    = new InOutManagement();
	m_pContainerExchanger = new ContainerExchanger();

	m_simulationName = boost::lexical_cast<std::string>(numberOfSimulations - 1);
}

void Simulation::SetMevicLogger(MevicLogger * pMevicLogger)
{
	this->m_pMevicLogger = pMevicLogger;
}

void Simulation::SetXMLNode(XMLNode simulation)
{
	this->m_simulation = simulation;
}

bool Simulation::Initialize(AbstractSimulation * pAbstractSimulation,
                            std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter *> > > &localParameters,
                            std::map<std::string, AbstractParameter *> &globalParameters)
{
	m_pMevicLogger->logMessage("Initialize Simulation: " + boost::lexical_cast<string>(m_simulationName));

	//! For every Chain in this Simulation
	for(int i = 0; i < m_simulation.nChildNode(); i++)
	{
		XMLNode chain = m_simulation.getChildNode(i);

		//! Create new Chain & add to list
		Chain * pChain = new Chain(m_pMevicLogger, chain);
		m_listOfChains.push(pChain);

		try
		{
			//! Initialize Chain
			pChain->Initialize(pAbstractSimulation, localParameters, globalParameters, m_pInOutManagement, m_pContainerExchanger);
		}
		catch(...)
		{
			m_pMevicLogger->logError("Exception caught: Chain initialization failed");
		}

		pChain = 0;
	}

	//! Return
	return true;
}

bool Simulation::Simulate()
{
	boost::posix_time::ptime         startTime,          // Simulation start time stamp
	                                 endTime;            // Simulation end time stamp
	boost::posix_time::time_duration simulationDuration; // Simulation duration

	startTime = boost::posix_time::microsec_clock::local_time();

	m_pMevicLogger->logMessage("Simulate Simulation: " + boost::lexical_cast<string>(m_simulationName));

	//! For every Chain in this Simulation
	while(!m_listOfChains.empty())
	{
		Chain * pChain = m_listOfChains.front();
		m_listOfChains.pop();

		try
		{
			//! Simulate Chain
			pChain->Simulate(m_pContainerExchanger);
		}
		catch(...)
		{
			m_pMevicLogger->logWarning("Exception caught: Chain simulation failed");
		}

		delete pChain;
		pChain = 0;
	}

	endTime = boost::posix_time::microsec_clock::local_time();
	simulationDuration = endTime - startTime;
	m_pMevicLogger->logMessage("Duration: " + boost::posix_time::to_simple_string(simulationDuration));

	//! Return
	return true;
}

Simulation::~Simulation()
{
	if (m_pContainerExchanger != 0)
	{
		delete m_pContainerExchanger;
		m_pContainerExchanger = 0;
	}

	if (m_pInOutManagement != 0)
	{
		delete m_pInOutManagement;
		m_pInOutManagement = 0;
	}

	m_pMevicLogger = 0;

	numberOfSimulations--;
}
