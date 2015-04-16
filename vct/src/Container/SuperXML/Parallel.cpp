#include "Parallel.h"
#include "FactoryParameter.h"

#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/ref.hpp>

#include <iConvert/iConvert.h>
#include <DataExchanger/UtilitiesExchanger.h>

#ifdef USE_OPENCL
#include "../OpenCLManager.h"
#endif

using namespace std;
using namespace iConvert;

Parallel::Parallel()
:m_numberOfProcessors(getNumberOfProcessors()),
 m_numberOfSimulations(0)
{
}

bool Parallel::CheckConsistency(XMLNode root, int option)
{
	//! (1st part) Check consistency of parents of XML file
	m_pMevicLogger->logMessage("[Parallel::CheckConsistency] Part 1 of 2 ...");

	// Check for list of iterations
	if (root.getChildNode(_T("LIST_OF_ITERATIONS")).isEmpty())
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not set");
	}
	// Check for list of parameters
	if (root.getChildNode(_T("LIST_OF_PARAMETERS")).isEmpty())
	{
		m_pMevicLogger->logError("LIST_OF_PARAMETERS not set");
	}
	// Check for template MEVIC simulation
	if (root.getChildNode(_T("TEMPLATE_MEVIC_SIMULATION")).isEmpty())
	{
		m_pMevicLogger->logError("TEMPLATE_MEVIC_SIMULATION not set");
	}

	XMLNode loi = root.getChildNode(_T("LIST_OF_ITERATIONS")),         // list of iterations XML node
	        lop = root.getChildNode(_T("LIST_OF_PARAMETERS")),         // list of parameters XML node
	        tms = root.getChildNode(_T("TEMPLATE_MEVIC_SIMULATION"));  // template mevic simulation XML node

	// Check if list of iterations is defined correctly
	if(   2 != loi.nAttribute()                                          // 2 attributes?
	   || 0 == loi.isAttributeSet(_T("name"))                            // attribute name set?
	   || 0 == loi.isAttributeSet(_T("value"))                           // attribute value set?
	   || 0 != wcToMb(loi.getAttribute(_T("name"))).compare("parallel")) // name = "parallel"?
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}
	int nbIternations;
	try
	{
		// Get number of times the pipeline of simulations will be executeed
		nbIternations = boost::lexical_cast<int>(getParamValue(wcToMb(loi.getAttribute(_T("value")))).c_str());
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}

	m_pMevicLogger->logMessage("... OK");

	//! (2nd part) Check consistency of children of XML file: simulation chains, modules, parameters
	m_pMevicLogger->logMessage("[Parallel::CheckConsistency] Part 2 of 2 ...");

	XMLNode chain, // Current Chain
	        mod,   // Current Module in the current Chain
	        param; // Current Parameter in the current Module of the current Chain

	string chainName,     // Respective chain of modules name
	       modFormalName, // Module formal name (type)
	       modActualName, // Module actual name (label); indicated by keyword "module" or "name"
	       paramName,     // Parameter name
	       paramValue;    // Parameter value

	XMLNode localParam,  // Current local parameter
	        globalParam; // Current global parameter

	AbstractParameter * pAbstractParam; // Pointer to object derived from AbstractParameter class

	// For every Chain
	for(int i = 0; i < tms.nChildNode(); i++)
	{
		chain = tms.getChildNode(i);
		chainName = wcToMb(chain.getName());
		m_pMevicLogger->logMessage("\tChain: " + chainName);

		// For every Module in the current Chain
		for(int j = 0; j < chain.nChildNode(); j++)
		{
			mod = chain.getChildNode(j);
			modFormalName = wcToMb(chain.getName());
			modActualName = getName(mod);
			m_pMevicLogger->logMessage("\t\tModule: " + modActualName);

			// For every Parameter in the current Module of the current Chain
			for(int k = 0; k < mod.nChildNode(); k++)
			{
				param = mod.getChildNode(k);
				paramName  = wcToMb(param.getName());
				m_pMevicLogger->logMessage("\t\t\tParameter: " + paramName);

				// Check if parameter value is defined
				if(0 == param.isAttributeSet(_T("value")))
				{
					m_pMevicLogger->logError("No value defined");
				}
				paramValue = wcToMb(param.getAttribute(_T("value")));

				// Local parameters, value "$"
				if(0 == paramValue.compare("$"))
				{
					localParam = lop.getChildNodeWithAttribute(_T("LocalParameter"), _T("name"), chain.getName());

					// Check if parameter has the respective local parameter defined
					if(localParam.isEmpty())
					{
						m_pMevicLogger->logError("No respective local parameter defined");
					}

					// Check if local parameter is defined correctly
					if(0 == localParam.getChildNodeWithAttribute(param.getParentNode().getName(), _T("name"), param.getName()).isAttributeSet(_T("value")))
					{
						m_pMevicLogger->logError("Respective local parameter incorrectly instantiated (no value)");
					}

					// Update parameter value
					paramValue = wcToMb(localParam.getChildNodeWithAttribute(param.getParentNode().getName(), _T("name"), param.getName()).getAttribute(_T("value")));
					//! Replace backslashes by forward-slashes if any
					while(paramValue.find_last_of('\\') != std::string::npos)
					{
						paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
					}

					// Configure the appropriate SuperXML parameter parser; ParamBasic/ParamtIt/ParamStar/ParamStep/ParamString
					pAbstractParam = FactoryParameter::ConfigureParameter(paramValue.c_str());
					if(NULL == pAbstractParam)
					{
						m_pMevicLogger->logError("Respective local parameter incorrectly instantiated (incorrect value)");
					}

					// Create the correct parameter
					pAbstractParam->CreateParameter(paramValue.c_str());
					
					// Check consistency of the parameter (> 0: inconsistency failure)
					switch(pAbstractParam->CheckConsistency(nbIternations))
					{
					// Success
					case 0:
						break;
					// Warning
					case 1:
						// Treat as error
						if(1 == option)
						{
							m_pMevicLogger->logError("Local parameter correctly instantiated, but the value is not consistent with the number of iterations\n\tThe process could be performed, but not all possible values would be taken into account");
						}
						// Treat as warning
						else
						{
							m_pMevicLogger->logWarning("Local parameter correctly instantiated, but the value is not consistent with the number of iterations\n\tThe process will be performed, but not all possible values will be taken into account");
						}
						break;
					// Error
					case 2:
						m_pMevicLogger->logError("Local parameter correctly instantiated, but the value is not consistent with the number of iterations");
						break;
					// Unkown
					default:
						m_pMevicLogger->logError("Unkown inconsistency");
						break;
					}
				}

				// Global parameters, value "$A", "$1", "$test", ...
				else if(   2 <= paramValue.size()
				        && 0 == paramValue.compare(0, 1, "$"))
				{
					globalParam = lop.getChildNodeWithAttribute(_T("GlobalParameter"), _T("name"), mbToWc(paramValue.substr(1)));

					// Check if parameter has the respective global parameter defined
					if(globalParam.isEmpty())
					{
						m_pMevicLogger->logError("No respective global parameter defined");
					}

					// Update parameter value
					paramValue = wcToMb(globalParam.getAttribute(_T("value")));
					//! Replace backslashes by forward-slashes if any
					while(paramValue.find_last_of('\\') != std::string::npos)
					{
						paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
					}

					// Configure the appropriate SuperXML parameter parser; ParamBasic/ParamtIt/ParamStar/ParamStep/ParamString
					pAbstractParam = FactoryParameter::ConfigureParameter(paramValue.c_str());
					if(NULL == pAbstractParam)
					{
						m_pMevicLogger->logError("Respective global parameter incorrectly instantiated (incorrect value)");
					}

					// Create the correct parameter
					pAbstractParam->CreateParameter(paramValue.c_str());
					
					// Check consistency of the parameter (> 0: inconsistency failure)
					switch(pAbstractParam->CheckConsistency(nbIternations))
					{
					// Success
					case 0:
						break;
					// Warning
					case 1:
						// Treat as error
						if(1 == option)
						{
							m_pMevicLogger->logError("Global parameter correctly instantiated, but the value is not consistent with the number of iterations\n\tThe process could be performed, but not all possible values would be taken into account");
						}
						// Treat as warning
						else
						{
							m_pMevicLogger->logWarning("Global parameter correctly instantiated, but the value is not consistent with the number of iterations\n\tThe process will be performed, but not all possible values will be taken into account");
						}
						break;
					// Error
					case 2:
						m_pMevicLogger->logError("Global parameter correctly instantiated, but the value is not consistent with the number of iterations");
						break;
					// Unkown
					default:
						m_pMevicLogger->logError("Unkown inconsistency");
						break;
					}
				}
			}
		}
	}

	m_pMevicLogger->logMessage("... OK");

	//! Return
	return true;
}

bool Parallel::StartSimulation(std::string& PathToXML, std::vector<string>& args)
{
	m_pMevicLogger->logMessage("[Parallel::StartSimulation]");

	//! Open and parse the XML file
	XMLNode root, // root XML node
	        loi,  // list of iterations XML node
	        lop,  // list of parameters XML node
	        tms;  // template mevic simulation XML node

	root = XMLNode::openFileHelper(mbToWc(PathToXML.c_str()), _T("SuperXML")); // root XML node
 
	loi  = root.getChildNode(_T("LIST_OF_ITERATIONS")); // list of iterations XML node
	lop  = root.getChildNode(_T("LIST_OF_PARAMETERS")); // list of parameters XML node
	tms  = root.getChildNode(_T("TEMPLATE_MEVIC_SIMULATION")); // template mevic simulation XML node

	// Double-check if list of iterations is defined correctly
	if(   2 != loi.nAttribute()                                          // 2 attributes?
	   || 0 == loi.isAttributeSet(_T("name"))                            // attribute name set?
	   || 0 == loi.isAttributeSet(_T("value"))                           // attribute value set?
	   || 0 != wcToMb(loi.getAttribute(_T("name"))).compare("parallel")) // name = "parallel"?
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}

	//! Instantiate command-line arguments
	if(false == instantiateArgs(args))
	{
		m_pMevicLogger->logError("Command-line arguments instantiation failed");
	}

	try
	{
		//! Get number of times the pipeline of simulations will be executeed
		m_numberOfSimulations = boost::lexical_cast<int>(getParamValue(wcToMb(loi.getAttribute(_T("value")))).c_str());
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}

	//! Instantiate local parameters
	if (false == instantiateLocalParameters(lop))
	{
		m_pMevicLogger->logError("Local parameters instantiation failed");
	}

	//! Instantiate global parameters
	if (false == instantiateGlobalParameters(lop))
	{
		m_pMevicLogger->logError("Global parameters instantiation failed");
	}

	try
	{
		//! Initialize
		if (false == Initialize(tms))
		{
			m_pMevicLogger->logError("Initialization failed");
		}
	}
	catch(...)
	{
		m_pMevicLogger->logError("Exception caught: Parallel initialization failed");
	}
	try
	{
		//! Simulate
		if (false == Simulate())
		{
			m_pMevicLogger->logError("Simulation failed");
		}
	}
	catch(...)
	{
		m_pMevicLogger->logError("Exception caught: Parallel simulation failed");
	}

	//! Return
	return true;
}

bool Parallel::instantiateLocalParameters(XMLNode lop)
{
	m_pMevicLogger->logMessage("[Parallel::instantiateLocalParameters]");

	//! Instantiate local parameters
	XMLNode localParam,      // Local parameter element
	        localParamChild; // Actual dynamic module parameter element

	string chainName,     // Respective chain of modules name
	       modFormalName, // Module formal name (type)
	       modActualName, // Module actual name (label); indicated by keyword "module" or "name"
	       paramName,     // Parameter name
	       paramValue;    // Parameter value

	AbstractParameter * pAbstractParam; // Pointer to object derived from AbstractParameter class

	// For all local parameters
	for(int i = 0; i < lop.nChildNode(_T("LocalParameter")); i++)
	{
		localParam = lop.getChildNode(_T("LocalParameter"), i);

		// Respecive chain of modules name
		chainName = wcToMb(localParam.getAttribute(_T("name")));

		// For all local parameter children (modules from chain)
		for(int j = 0; j < localParam.nChildNode(); j++)
		{
			// Actual dynamic module parameter element
			localParamChild = localParam.getChildNode(j);

			// Extract parameter information
			modFormalName = wcToMb(localParamChild.getName());
			modActualName = getName(localParamChild);
			paramName  = wcToMb(localParamChild.getAttribute(_T("name")));
			paramValue = wcToMb(localParamChild.getAttribute(_T("value")));

			// If the parameter value contains one (or more) '#', it refers to a command-line argument
			paramValue = getParamValue(paramValue);

			// Replace backslashes by forward-slashes if any
			while(paramValue.find_last_of('\\') != std::string::npos)
			{
				paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
			}

			// Configure the appropriate SuperXML parameter parser; ParamBasic/ParamtIt/ParamStar/ParamStep/ParamString
			pAbstractParam = FactoryParameter::ConfigureParameter(paramValue);
			if(NULL == pAbstractParam)
			{
				m_pMevicLogger->logError("Initialization of local parameter "  + paramName + " failed");
			}

			// Create the correct parameter
			pAbstractParam->CreateParameter(paramValue);

			// Check consistency of the parameter (2: inconsistency error)
			if(2 == pAbstractParam->CheckConsistency(m_numberOfSimulations))
			{
				m_pMevicLogger->logError("Local parameter " + paramName + " is not consistent with the number of iterations");
			}

			// Add the parameter
			m_localParameters[chainName][modActualName].insert(pair<std::string, AbstractParameter *>(paramName, pAbstractParam));
			m_pMevicLogger->logMessage("\t" + chainName + "." + modActualName + "." + paramName);
		}
	}

	//! Return
	return true;
}

bool Parallel::instantiateGlobalParameters(XMLNode lop)
{
	m_pMevicLogger->logMessage("[Parallel::instantiateGlobalParameters]");

	//! Instantiate global parameters
	XMLNode globalParam; // Global parameter element

	string paramName,  // Parameter name
	       paramValue; // Parameter value

	AbstractParameter * pAbstractParam; // Pointer to object derived from AbstractParameter class

	// For all global parameters
	for(int i = 0; i < lop.nChildNode(_T("GlobalParameter")); i++)
	{
		globalParam = lop.getChildNode(_T("GlobalParameter"), i);

		// Extract parameter information
		paramName  = wcToMb(globalParam.getAttribute(_T("name")));
		paramValue = wcToMb(globalParam.getAttribute(_T("value")));

		// If the parameter value contains one (or more) '#', it refers to a command-line argument
		paramValue = getParamValue(paramValue);

		// Replace backslashes by forward-slashes if any
		while(paramValue.find_last_of('\\') != std::string::npos)
		{
			paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
		}

		// Configure the appropriate SuperXML parameter parser; ParamBasic/ParamtIt/ParamStar/ParamStep/ParamString
		pAbstractParam = FactoryParameter::ConfigureParameter(paramValue.c_str());
		if(NULL == pAbstractParam)
		{
			m_pMevicLogger->logError("Initialization of global parameter " + paramName + "failed");
		}

		// Create the correct parameter
		pAbstractParam->CreateParameter(paramValue.c_str());

		// Check consistency of the parameter (2: inconsistency error)
		if(2 == pAbstractParam->CheckConsistency(m_numberOfSimulations))
		{
			m_pMevicLogger->logError("Global parameter " + paramName + " is not consistent with the number of iterations");
		}

		// Add the parameter
		m_globalParameters[paramName] = pAbstractParam;
		m_pMevicLogger->logMessage("\t" + paramName);
	}

	//! Return
	return true;
}

bool Parallel::Initialize(XMLNode tms)
{
	m_pMevicLogger->logMessage("[Parallel::Initialize]");

	//! For every Simulation
	for(int simulation = 0; simulation < m_numberOfSimulations; simulation++)
	{
		//! Create new MevicLogger for this simulation
		MevicLogger * pMevicLogger = new MevicLogger(m_pMevicLogger->getDirName(), "simulation_" + boost::lexical_cast<string>(simulation), false); // Log in the same directory, no console feedback

		//! Create new Simulation & add to list
		Simulation * pSimulation = new Simulation(pMevicLogger, tms);
		m_listOfSimulations.push(pSimulation);

		try
		{
			//! Initialize Simulation
			pSimulation->Initialize(this, m_localParameters, m_globalParameters);
		}
		catch(...)
		{
			m_pMevicLogger->logWarning("Exception caught: Simulation initialization failed");
		}

		pSimulation = 0;
		pMevicLogger = 0;
	}

	//! Return
	return true;
}

bool Parallel::Simulate()
{
	m_pMevicLogger->logMessage("[Parallel::Simulate]");

	queue<boost::thread *> listOfThreads; //<! List of active thread object pointers

	/*!
	 * Launch/schedule Simulations (threads) in parallel
	 * Optimal number of Simulations to launch is depending on:
	 * - Total number of Simulations to run
	 * - Number of processors available in the system
	 */
	while(!m_listOfSimulations.empty() && (listOfThreads.size() <= m_numberOfProcessors - 1))
	{
		Simulation * pSimulation = m_listOfSimulations.front();
		m_listOfSimulations.pop();

		try
		{
			//! Start new thread to simulate Simulation
			boost::thread * pThread = new boost::thread(&Simulation::Simulate, pSimulation); // pSimulation->Simulate()
			listOfThreads.push(pThread);

			m_pMevicLogger->logMessage(boost::lexical_cast<string>(listOfThreads.size()) + " thread(s) running on " + boost::lexical_cast<string>(m_numberOfProcessors) + " processor(s) available in the system");
			stringstream nextThreadID;
			nextThreadID << pThread->get_id();
			m_pMevicLogger->logMessage("\tThread ID " + nextThreadID.str() + " running");
			pThread = 0;
		}
		catch(...)
		{
			m_pMevicLogger->logWarning("Exception caught: Simulation simulation failed");
		}
	}

	/*!
	 * Schedule next Simulation (thread)
	 * Wait for oldest Simulation (thread) to finish
	 * Schedule next Simulation (thread) if any
	 */
	while(!m_listOfSimulations.empty())
	{
		//! Wait for oldest Simulation (thread) to finish

		boost::thread * pThread = listOfThreads.front();
		listOfThreads.pop();

		stringstream oldThreadID;
		oldThreadID << pThread->get_id();

		//! Wait for the thread to finish
		pThread->join();

		m_pMevicLogger->logMessage("\tThread ID " + oldThreadID.str() + " finished");
		m_pMevicLogger->logMessage(boost::lexical_cast<string>(listOfThreads.size()) + " thread(s) running on " + boost::lexical_cast<string>(m_numberOfProcessors) + " processor(s) available in the system");

		delete pThread;
		pThread = 0;

		//! Schedule next Simulation (thread) if any

		Simulation * pSimulation = m_listOfSimulations.front();
		m_listOfSimulations.pop();

		try
		{
			//! Start new thread to simulate Simulation
			pThread = new boost::thread(&Simulation::Simulate, pSimulation); // pSimulation->Simulate()
			listOfThreads.push(pThread);

			m_pMevicLogger->logMessage(boost::lexical_cast<string>(listOfThreads.size()) + " thread(s) running on " + boost::lexical_cast<string>(m_numberOfProcessors) + " processor(s) available in the system");
			stringstream nextThreadID;
			nextThreadID << pThread->get_id();
			m_pMevicLogger->logMessage("\tThread ID " + nextThreadID.str() + " running");

			pThread = 0;
		}
		catch(...)
		{
			m_pMevicLogger->logWarning("Exception caught: Simulation simulation failed");
		}
	}

	/*!
	 * Finalize Simulations (threads)
	 * Wait for all running Simulations (threads) to finish
	 */
	while(!listOfThreads.empty())
	{
		boost::thread * pThread = listOfThreads.front();
		listOfThreads.pop();

		stringstream oldThreadID;
		oldThreadID << pThread->get_id();

		//! Wait for the thread to finish
		pThread->join();

		m_pMevicLogger->logMessage("\tThread ID " + oldThreadID.str() + " finished");
		m_pMevicLogger->logMessage(boost::lexical_cast<string>(listOfThreads.size()) + " thread(s) running on " + boost::lexical_cast<string>(m_numberOfProcessors) + " processor(s) available in the system");

		delete pThread;
		pThread = 0;
	}

	//! Return
	return true;
}

unsigned short getNumberOfProcessors()
{
#ifdef _WIN32 // Windows
	char * pNumberOfProcessors = getenv("NUMBER_OF_PROCESSORS");
	if(NULL != pNumberOfProcessors)
	{
		try
		{
			//! Return
			return boost::lexical_cast<unsigned short>(pNumberOfProcessors);
		}
		catch(boost::bad_lexical_cast e)
		{
			//! Return
			return 1;
		}
	}
#elif __GNUC__ // Linux
	FILE * pF = popen("grep -c '^processor' /proc/cpuinfo", "r");
	if(NULL != pF)
	{
		char numberOfProcessors[1];
		fread(numberOfProcessors, sizeof(char), sizeof(numberOfProcessors), pF);
		try
		{
			//! Return
			return boost::lexical_cast<unsigned short>(numberOfProcessors[0]);
		}
		catch(boost::bad_lexical_cast e)
		{
			//! Return
			return 1;
		}
	}
#endif

	//! Return
	return 1;
}

Parallel::~Parallel()
{
}
