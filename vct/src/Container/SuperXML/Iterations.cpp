#include "Iterations.h"
#include <boost/lexical_cast.hpp>

#ifdef USE_OPENCL
#include "../OpenCLManager.h"
#endif

using namespace std;
using namespace iConvert;

bool Iterations::CheckConsistency(XMLNode root, int option)
{
	//! (1st part) Check consistency of parents of XML file
	m_pMevicLogger->logMessage("[Iterations::CheckConsistency] Part 1 of 2 ...");

	// Check for list of iterations
	if (root.nChildNode(_T("LIST_OF_ITERATIONS")) == root.getChildNode(_T("TEMPLATE_MEVIC_SIMULATION")).nChildNode())
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

	XMLNode loi = root.getChildNode(_T("LIST_OF_ITERATIONS")),
	        lop = root.getChildNode(_T("LIST_OF_PARAMETERS")),
	        tms = root.getChildNode(_T("TEMPLATE_MEVIC_SIMULATION"));

	// Check if list of iterations is defined correctly
	if(   1 != loi.nAttribute()                                            // 1 attribute?
	   || 0 == loi.isAttributeSet(_T("name"))                              // attribute name set?
	   || 0 != wcToMb(loi.getAttribute(_T("name"))).compare("iterations")) // name = "iterations"?
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}

	m_pMevicLogger->logMessage("... OK");

	//! (2nd part) Check consistency of children of XML file: simulation chains, modules, parameters
	m_pMevicLogger->logMessage("[Iterations::CheckConsistency] Part 2 of 2 ...");

	XMLNode child;
	AbstractParameter *as;
	string val;
	int nbit;

	for (int i = 0; i < tms.nChildNode(); i ++) {
		m_pMevicLogger->logMessage("\t" + wcToMb(tms.getChildNode(i).getName()));
		for (int j = 0; j < tms.getChildNode(i).nChildNode(); j ++) {
			m_pMevicLogger->logMessage("\t\t" + wcToMb(tms.getChildNode(i).getChildNode(j).getName()));
			for (int k = 0; k < tms.getChildNode(i).getChildNode(j).nChildNode(); k ++) {
				child = tms.getChildNode(i).getChildNode(j).getChildNode(k);
				m_pMevicLogger->logMessage("\t\t\t" + wcToMb(child.getName()));
				if (child.isAttributeSet(_T("value")) == 0) {
					m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") does not contain the value attribute");
				}
				if (wcToMb(child.getAttribute(_T("value"))).compare("$") == 0) {
					if (lop.getChildNode(tms.getChildNode(i).getName()).isEmpty()) {
						m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") does not have an associated local parameter");
					}
					if (lop.getChildNode(tms.getChildNode(i).getName()).getChildNodeWithAttribute(child.getParentNode().getName(), _T("name"), child.getName()).isAttributeSet(_T("value")) == 0) {
						m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") has a local parameter incorrectly instanciated");
					}
					val = wcToMb( lop.getChildNode(tms.getChildNode(i).getName()).getChildNodeWithAttribute(child.getParentNode().getName(), _T("name"), child.getName()).getAttribute(_T("value")));
					as = FactoryParameter::ConfigureParameter(val.c_str());
					if (as == NULL) {
						m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") has a local parameter incorrectly instanciated (value present but not correct)");
					}
					as->CreateParameter(val.c_str());
					nbit = atoi(wcToMb(loi.getChildNode(tms.getChildNode(i).getName()).getAttribute(_T("value"))).c_str());
					if (as->CheckConsistency(nbit) == 2) {
						m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") has a local parameter correctly instanciated, but not consistent with the number of iterations");
					}
					else if (as->CheckConsistency(nbit) == 1) {
						if (option == 1) {
							m_pMevicLogger->logError("An element (" + wcToMb(child.getName()) + ") has a local parameter correctly instanciated, the process can be performed, but all the possible values of the parameter will not be taken into account");
						}
						else m_pMevicLogger->logWarning("Warning: An element (" + wcToMb(child.getName()) + ") has a local parameter correctly instanciated, the process can be performed, but all the possible values of the parameter will not be taken into account");
					}
				}
			}
		}
	}

	m_pMevicLogger->logMessage("... OK");

	//! Return
	return true;
}

bool Iterations::StartSimulation(string& PathToXML, vector<string>& args)
{
	m_pMevicLogger->logMessage("[Iterations::StartSimulation]");

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
	if(   1 != loi.nAttribute()                                            // 2 attributes?
	   || 0 == loi.isAttributeSet(_T("name"))                              // attribute name set?
	   || 0 != wcToMb(loi.getAttribute(_T("name"))).compare("iterations")) // name = "iterations"?
	{
		m_pMevicLogger->logError("LIST_OF_ITERATIONS not defined correctly");
	}

	//! Instantiate command-line arguments
	if(false == instantiateArgs(args))
	{
		m_pMevicLogger->logError("Command-line arguments instantiation failed");
	}

	//! List of iterations
	if (false == initListOfIterations(loi))
	{
		m_pMevicLogger->logError("List of iterations initialization failed");
	}

	//! Local parameters
	if (false == initListOfParameters(lop))
	{
		m_pMevicLogger->logError("Local parameters initialization failed");
	}

	try
	{
		//! Simulate
		runSimulation(tms);
	}
	catch(...)
	{
		m_pMevicLogger->logError("Exception caught: Iterations simulation failed");
	}

	//! Return
	return true;
}

bool Iterations::initListOfIterations(XMLNode node){
	//! Process
	simu *test;

	try {
		for (int i = 0; i < node.nChildNode();i ++) {
			test = new simu();
			test->idSimu = wcToMb(node.getChildNode(i).getName());
			test->numberOfIterations = atoi(getParamValue(wcToMb(node.getChildNode(i).getAttribute(_T("value")))).c_str());
			_listOfIterations.push_back(*test); 
			delete test;
		}
	}
	catch (...){
		return false;
	}

	//! Return
	return true;
}

bool Iterations::initListOfParameters(XMLNode node){
	//! Instanciation of the variables
	XMLNode element,
		parameter;
	string elementId,
		elementName,
		elementValue;
	AbstractParameter * as;

	//! Process
	//! For each simulation
	try {
		for (int i = 0; i < node.nChildNode(); i ++) {
			//! For each element
			element = node.getChildNode(i);
			for (int j = 0; j < element.nChildNode(); j ++) {
				parameter = element.getChildNode(j);
				elementName = wcToMb( parameter.getAttribute(_T("name")));
				elementValue = wcToMb( parameter.getAttribute(_T("value")));

				//! If the param value contains one (or more) #, means it contains a command line argument
				elementValue = getParamValue(elementValue);

				//! Replace backslashes by forward-slashes if any
				while(elementValue.find_last_of('\\') != std::string::npos)
				{
					elementValue = elementValue.replace(elementValue.find_last_of('\\'), 1, "/");
				}

				elementId = getName(parameter);
				as = FactoryParameter::ConfigureParameter(elementValue);
				as->CreateParameter(elementValue);
	
				if (as->CheckConsistency(atoi(wcToMb(node.getParentNode().getChildNode(_T("LIST_OF_ITERATIONS")).getChildNode(element.getName()).getAttribute(_T("value"))).c_str())) == 2) return false;

				_localParameters[wcToMb(element.getName())][elementId][elementName] = as;
			}
		}
	}
	catch (...) {
		m_pMevicLogger->logError("Instantiation of List of Local parameters failed");
	}

	//! Return
	return true;
}

bool Iterations::runSimulation(XMLNode node){
	//! Instanciation of the variables
	string simulation;
	string paramName, paramValue;
	XMLNode m, p, s; // respectively for module and parameter
	FactoryModule fact_module;
	Module *mod;

	// For each simulation
	for (unsigned i = 0; i < _listOfIterations.size(); i ++)
	{
		simulation = _listOfIterations[i].idSimu;
		s = node.getChildNode(mbToWc(simulation));

		//! Create new MevicLogger for this simulation
		MevicLogger * pMevicLogger = new MevicLogger(m_pMevicLogger->getDirName(), "simulation_" + boost::lexical_cast<string>(i), false); // Log in the same directory, no console feedback

		//! For each iteration of the current simulation
		for (int h = 0; h < _listOfIterations[i].numberOfIterations; h ++)
		{
			UtilitiesExchanger * pUtilitiesExchanger = new UtilitiesExchanger(); // (Pointer to) Utilities exchanger

			vector<DSContainer *> containers; // List of (pointers to) DSContainers

			//! If the vector of DSContainer objects is empty, add a dummy test object
			if(containers.empty())
			{
				DSContainer * pContainer = new DSContainer();
				containers.push_back(pContainer);
				pContainer = 0;
			}

			//! For each module
			for (int j = 0; j < s.nChildNode(); j ++)
			{
				m = s.getChildNode(j);
				//lom = new ListOfModules(m);

				try
				{
					// Generate new Module
					mod = fact_module.construct(wcToMb(m.getName()).c_str(), pMevicLogger);

					// For each parameter
					for (int k = 0; k < m.nChildNode(); k ++)
					{
						p = m.getChildNode(k);

						paramName="";
						paramValue="";
						paramName = wcToMb(p.getName());
						paramValue = wcToMb(p.getAttribute(_T("value")));
						
						//! Get respective local parameter if any
						if (paramValue.compare("$") == 0) {
							paramValue = _localParameters[wcToMb(s.getName())][getName(m)][paramName]->GetCurrentPositionAndIterate();
						}
					
						//! Replace command-line arguments if any
						paramValue = getParamValue(paramValue);
						//! Replace backslashes by forward-slashes if any
						while(paramValue.find_last_of('\\') != std::string::npos)
						{
							paramValue = paramValue.replace(paramValue.find_last_of('\\'), 1, "/");
						}

						//! Set the Module's Parameter
						mod->SetParameter(paramName, paramValue);
					}
					//! Set the Module's utilities exchanger
					mod->SetUtilitiesExchanger(pUtilitiesExchanger);

					//! Simulate Module
					mod->Simulate(containers);

					delete mod;
					mod = 0;
				}
#ifdef USE_OPENCL
				catch(cl::Error& e)
				{
					pMevicLogger->logError(string("Exception caught: Module initialization/simulation failed; ")
						+e.what()+string("(): ")+openclStrError(e.err()));
				}
#endif
				catch(exception& e)
				{
					pMevicLogger->logError(string("Exception caught: Module initialization/simulation failed; ")
						+e.what());
				}
			}

			//! If the vector still contains DSContainer objects, delete them and clear it
			while(!containers.empty())
			{
				DSContainer * pContainer = containers.back();
				containers.pop_back();
				delete pContainer;
				pContainer = 0;
			}
			containers.clear();

			delete pUtilitiesExchanger;
			pUtilitiesExchanger = 0;
		}
		delete pMevicLogger;
	}

	//! Return
	return true;
}
