#include "MevicLogger.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <direct.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/exception/diagnostic_information.hpp>

using namespace std;

/*!
 * Static members initialization
 */
int MevicLogger::numberOfMevicLoggers = 0;

MevicLogger::MevicLogger()
:m_consoleFeedback(true)
{
	// Increment number of MevicLogger objects in use
	numberOfMevicLoggers++;

	// Set members
	m_dirName  = "./" + boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()) + "/";
	m_fileName = "mevic_" + boost::lexical_cast<string>(numberOfMevicLoggers - 1) + "_log.txt";

	// Create log directory
	
	//boost::filesystem::create_directories(m_dirName.c_str());
	_mkdir( m_dirName.c_str() );
	
	// Open log file
	string logFilePath = m_dirName + m_fileName;
	logFile.open(logFilePath.c_str(), ios::trunc);
}

MevicLogger::MevicLogger(std::string dirName, std::string fileName, bool consoleFeedback)
{
	// Increment number of MevicLogger objects in use
	numberOfMevicLoggers++;

	// Set members
	m_dirName  = "./" + dirName + "/";
	m_fileName = fileName + "_log.txt";
	m_consoleFeedback = consoleFeedback;

	// Create log directory
	boost::filesystem::create_directory(m_dirName);

	// Open log file
	string logFilePath = m_dirName + m_fileName;
	logFile.open(logFilePath.c_str(), ios::trunc);
}

void MevicLogger::logProgress(std::string message)
{
	logFile << message;
	logFile.flush();
	if(m_consoleFeedback)
	{
		cout << message;
		cout.flush();
	}
}

void MevicLogger::logMessage(std::string message)
{
	message = boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()) + ": [INFO]  " + message;
	logFile << endl << message;
	logFile.flush();
	if(m_consoleFeedback)
	{
		cout << endl << message;
		cout.flush();
	}
}

void MevicLogger::logWarning(std::string message)
{
	message = boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()) + ": [WARN]  " + message;
	logFile << endl << message;
	logFile.flush();
	if(m_consoleFeedback)
	{
		cerr << endl << message;
		cerr.flush();
	}
}

void MevicLogger::logError(std::string message)
{
	message = boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()) + ": [ERROR] " + message;
	logFile << endl << message;
	logFile.flush();
	cerr << endl << message;
	cerr.flush();

	if(m_consoleFeedback)
	{
		cerr << endl << endl << "***ERROR** MEVIC is going to abort!" << endl;
		cerr.flush();
	}
	exit(1);
}

void MevicLogger::logDebug(std::string message)
{
	message = boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time()) + ": [DEBUG] " + message;
	logFile << endl << message;
	logFile.flush();
	if(m_consoleFeedback)
	{
		cout << endl << message;;
		cout.flush();
	}
	
}

MevicLogger::~MevicLogger()
{
	// Close log file
	logFile.close();

	// Decrement number of MevicLogger objects in use
	numberOfMevicLoggers--;
}
