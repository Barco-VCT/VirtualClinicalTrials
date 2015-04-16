//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file MevicLogger.h
 * \brief Implementation of the class MevicLogger
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

#ifndef MEVICLOGGER_H
#define MEVICLOGGER_H

#include <fstream>
#include <string>

/*!
 * \class MevicLogger
 * @brief MevicLogger class for the error management of the simulation
 */
class MevicLogger
{

public:

	/*!
	 * (Default) Constructor
	 */
	MevicLogger();

	/*!
	 * Constructor
 	 * \param dirName: Directory Name of the log file location
 	 * \param fileName: File name of the log file
 	 * \param consoleFeedback: Indicator whether or not to repeate message on console for user feedback
	 */
	MevicLogger(std::string dirName, std::string fileName, bool consoleFeedback);

	/*!
	 * \fn getDirName
	 * \brief accessor for getting the Directory Name
	 * \return string: directory Name of the log file location
	 */
	std::string getDirName()
	{
		return m_dirName;
	}

	/*!
	 * \fn getFileName
	 * \brief accessor for getting the file name
	 * return string: Log file name
	 */
	std::string getFileName()
	{
		return m_fileName;
	}

	/*!
	 * \fn logProgress
	 * \brief log progress status in the log file
	 * \param message: progress message to log
	 * \return void
	 */
	void logProgress(std::string message);

	/*!
	 * \fn logMessage
	 * \brief log message status in the log file 
	 * \param message: Info message to log
	 * \return: void
	 */
	void logMessage(std::string message);

	/*!
	 * \fn logWarning
	 * \brief log warning meassage in the the log file
	 * \param message: Warning message to log
	 * \return void
	 */
	void logWarning(std::string message);

	/*!
	 * \fn logError
	 * \brief log error meassage in the the log file
	 *		  the simulation is stopped
	 * \param message: Error message to log
	 * \return void
	 */
	void logError(std::string message);

	/*!
	 * \fn logDebug
	 * \brief log debug meassage in the the log file
	 * \param message: Debugging message to log
	 * \return void
	 */
	void logDebug(std::string message);

	/*!
	 * Destructor
	 */
	~MevicLogger();

private:

	static int numberOfMevicLoggers; //<! Number of MevicLogger objects in use

	bool m_consoleFeedback; //<! Indicator whether or not to repeate message on console for user feedback
	std::string m_dirName,  //<! Directory Name of the log file location
	            m_fileName; //<! Log file name

	std::ofstream logFile; //<! Log file
};

#endif // MEVICLOGGER_H
