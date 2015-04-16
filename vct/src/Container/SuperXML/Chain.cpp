#include "Chain.h"

#include <iConvert/iConvert.h>

#ifdef USE_OPENCL
#include "../OpenCLManager.h"
#endif

using namespace std;
using namespace iConvert;

Chain::Chain()
:m_pMevicLogger(0),
 m_chain(),
 m_chainName("")
{
	m_pUtilitiesExchanger = new UtilitiesExchanger();
}

Chain::Chain(MevicLogger * pMevicLogger, XMLNode chain)
:m_pMevicLogger(pMevicLogger),
 m_chain(chain)
{
	m_pUtilitiesExchanger = new UtilitiesExchanger();

	m_chainName = wcToMb(chain.getName());
}

void Chain::SetMevicLogger(MevicLogger * pMevicLogger)
{
	this->m_pMevicLogger = pMevicLogger;
}

void Chain::SetXMLNode(XMLNode chain)
{
	this->m_chain = chain;

	m_chainName = wcToMb(chain.getName());
}

bool Chain::Initialize(AbstractSimulation * pAbstractSimulation,
                std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter *> > > &localParameters,
                std::map<std::string, AbstractParameter *> &globalParameters,
                InOutManagement * pInOutManagement,
                ContainerExchanger * pContainerExchanger)
{
	XMLNode mod,         // Current Module in this Chain
	        param;       // Current Parameter in the current Module of this Chain

	string modFormalName, // Module formal name (type)
	       modActualName, // Module actual name (label); indicated by keyword "module" or "name"
	       paramName,     // Parameter name
	       paramValue;    // Parameter value

	m_pMevicLogger->logMessage("\tChain: " + m_chainName);

	//! For every Module in this Chain
	for(int i = 0; i < m_chain.nChildNode(); i++)
	{
		XMLNode mod = m_chain.getChildNode(i);
		string modFormalName = wcToMb(mod.getName());
		string modActualName = pAbstractSimulation->getName(mod);
		m_listOfModuleNames.push(modActualName);
		m_pMevicLogger->logMessage("\t\tModule: " + modActualName);

		try
		{
			//! Generate new Module and add to list of modules
			FactoryModule * pFactoryModule = new FactoryModule();
			Module * pModule = pFactoryModule->construct(modFormalName.c_str(), m_pMevicLogger);
			m_listOfModules.push(pModule);
			delete pFactoryModule;
			pFactoryModule = 0;

			//! For every Parameter in the current Module
			for(int j = 0; j < mod.nChildNode(); j++)
			{
				XMLNode param = mod.getChildNode(j);
				string paramName  = wcToMb(param.getName());
				string paramValue = wcToMb(param.getAttribute(_T("value")));

				//! Get respective local parameter if any
				if(0 == paramValue.compare("$"))
				{
					paramValue = localParameters[m_chainName][modActualName][paramName]->GetCurrentPositionAndIterate();
				}
				//! Get respective global parameter if any
				else if(   2 <= paramValue.size()
						&& 0 == paramValue.compare(0, 1, "$"))
				{
					paramValue = globalParameters[paramValue.substr(1)]->GetCurrentPosition();
				}
				//! Provide in-out management if any
				else if(InOutManagement::check(paramValue))
				{
					paramValue = pInOutManagement->manage(paramValue);
				}
				//! Replace command-line arguments if any
				paramValue = pAbstractSimulation->getParamValue(paramValue);
				//! Replace backslashes by forward-slashes if any
				while(paramValue.find_last_of('\\') != std::string::npos)
				{
					paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
				}

				m_pMevicLogger->logMessage("\t\t\tParameter: " + paramName + " = " + paramValue);;

				//! Set the Module's Parameter
				pModule->SetParameter(paramName, paramValue);
			}

			//! Set the Module's container exchanger
			pModule->SetContainerExchanger(pContainerExchanger);

			//! Set the Module's utilities exchanger
			pModule->SetUtilitiesExchanger(m_pUtilitiesExchanger);

			pModule = 0;
		}
#ifdef USE_OPENCL
		catch(cl::Error& e)
		{
			m_pMevicLogger->logError(string("Exception caught: Module initialization failed ")
				+modFormalName+string("; ")+e.what()+"(): "+openclStrError(e.err()));
		}
#endif
		catch(exception& e)
		{
			m_pMevicLogger->logError(string("Exception caught: Module initialization failed ")
				+modFormalName+string("; ")+e.what());
		}
	}

	//! Return
	return true;
}

bool Chain::Simulate(ContainerExchanger * pContainerExchanger)
{
	m_pMevicLogger->logMessage("\tChain: " + m_chainName);

	//! If the vector of DSContainer objects is empty, add a dummy test object
	if(m_containers.empty())
	{
		DSContainer * pContainer = new DSContainer();
		m_containers.push_back(pContainer);
		pContainer = 0;
	}

	//! For every Module in this Chain
	while(!(m_listOfModules.empty() || m_listOfModuleNames.empty()))
	{
		string modActualName = m_listOfModuleNames.front();
		m_listOfModuleNames.pop();
		m_pMevicLogger->logMessage("\t\tModule: " + modActualName);

		Module * pModule = m_listOfModules.front();
		m_listOfModules.pop();

		try
		{
			//! Simulate Module
			pModule->Simulate(m_containers);
		}
#ifdef USE_OPENCL
		catch(cl::Error& e)
		{
			m_pMevicLogger->logError(string("Exception caught: Module simulation failed; ")
				+e.what()+string("(): ")+openclStrError(e.err()));
		}
#endif
		catch(exception& e)
		{
			m_pMevicLogger->logError(string("Exception caught: Module simulation failed; ")
				+e.what());
		}

		delete pModule;
		pModule = 0;
	}

	//! Clean-up non-used (not present in the ContainerExchanger) DSContainer objects in the vector
	for (unsigned i = 0; i < m_containers.size(); i++)
	{
		DSContainer * pContainer = m_containers[i];
		// Check if the Container is not present in the ContainerExchanger
		if ((pContainerExchanger->lookUpContainer(pContainer)).empty())
		{
			delete pContainer;
		}
	}
	m_containers.clear();

	//! Return
	return true;
}

Chain::~Chain()
{
	if (m_pUtilitiesExchanger != 0)
	{
		delete m_pUtilitiesExchanger;
		m_pUtilitiesExchanger = 0;
	}

	m_pMevicLogger = 0;
}

