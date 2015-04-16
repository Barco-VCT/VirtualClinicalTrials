#include "SuperMEVICXML.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ErrorManagement/MevicLogger.h>
#ifdef USE_OPENCL
#include "../Container/OpenCLManager.h"
#endif

using namespace std;
using namespace iConvert;

int SuperMEVICXML::StartSimulation(string& progName, vector<string>& progArgs, string& xmlPath, vector<string>& simArgs, int option) {
	//! Instantiation of the different variables
	AbstractSimulation *as;

	//! MevicLogger object
	MevicLogger * pMevicLogger = new MevicLogger();

#ifdef USE_OPENCL
	stripExecName(progName);
	OpenCLManager oclMgr(pMevicLogger, progName, progArgs); // We're a friend class, modules must use OpenCLManager::getInstance() if a ref is needed at all.
#endif

	//! Getting of the root node of the XML file
	XMLNode root = XMLNode::openFileHelper(iConvert::mbToWc(xmlPath.c_str()), _T("SuperXML"));

	//! Identification of the way to read the XML file
	if      (wcToMb(root.getChildNode(_T("LIST_OF_ITERATIONS")).getAttribute(_T("name"))).compare("parallel") == 0)   as = new Parallel();   // Parallel mode
	else if (wcToMb(root.getChildNode(_T("LIST_OF_ITERATIONS")).getAttribute(_T("name"))).compare("pipeline") == 0)   as = new Pipeline();   // Pipeline mode
	else if (wcToMb(root.getChildNode(_T("LIST_OF_ITERATIONS")).getAttribute(_T("name"))).compare("iterations") == 0) as = new Iterations(); // Iterations mode
	else return 1;

	//! Assign MevicLogger
	as->SetMevicLogger(pMevicLogger);

	//! Check of the consistency of the XML file
	if (option == 0 || option == 1 || option == 2)
	{ 
		int opt = option;
		if (option == 2) opt = 0;
		try{
			if(false == as->CheckConsistency(root, opt))
			{
				pMevicLogger->logError("Error during file consistency checkout");
			}
		}
		catch(exception & e)
		{
			pMevicLogger->logError(e.what());
		}
	}

	boost::posix_time::ptime         startTime,          // Simulation start time stamp
	                                 endTime;            // Simulation end time stamp
	boost::posix_time::time_duration simulationDuration; // Simulation duration

	startTime = boost::posix_time::microsec_clock::local_time();
	pMevicLogger->logMessage("Simulations started @ " + boost::posix_time::to_simple_string(startTime));

	//! Starting of the excecution of the simulations
	if (option == 0 || option == 1 || option == 3)
	{
		try{
			if(true == as->StartSimulation(xmlPath, simArgs))
			{
				pMevicLogger->logMessage("*** ok ***");
			}
			else
			{
				pMevicLogger->logMessage("*** bad ***");
			}
		}
		catch (exception & e)
		{
			pMevicLogger->logError(e.what());
		}
	}

	endTime = boost::posix_time::microsec_clock::local_time();
	pMevicLogger->logMessage("Simulations ended @ " + boost::posix_time::to_simple_string(endTime));

	simulationDuration = endTime - startTime;
	pMevicLogger->logMessage("Total duration: " + boost::posix_time::to_simple_string(simulationDuration));

	delete as;
	delete pMevicLogger;

	return 0;
}

/**
 * Turn execPath into the path to the executable (usually MEVIC.exe) including a trailing path separator if there was any.
 * Input type examples: "MEVIC.exe", "C:\dir\dir 2\MEVIC.exe", "/usr/local/mevic/mevic3-xxx.exe", "/MEVIC.exe"
 */
void SuperMEVICXML::stripExecName(string& execPath) const {
#if defined WIN32 || defined _WIN32 || defined __WIN32__ || defined WIN64
	const char pathSep = '\\';
#else
	const char pathSep = '/';
#endif

	string::size_type sepPos = execPath.find_last_of(pathSep);
	if (sepPos == string::npos) {
		// Path separator not found. Must be just the file name, so path to it is empty (or .\ or ./).
		execPath.clear();
	} else {
		// Strip the executable name; leave the path separator.
		execPath.resize(sepPos + 1);
	}
}
