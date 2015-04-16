//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ErrorLogger.h
 * \brief Implementation of the class ErrorLogger
 * \date 01-May-2009
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

#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include <string>

#undef warning

/*!
 * \namespace ErrorLogger
 * \brief This namespace allows the management of
 *		  the errors and display message in MEVIC
 */
namespace ErrorLogger
{
	/*!
 	 * \fn message
	 * \brief method to display a message in the simulation
	 * \param message: the message to display
	 * \return void
	 */ 
	void message(const std::string message);

	/*!
 	 * \fn warning
	 * \brief method to display a warning message in the simulation
	 * \param: message the message to display
	 * \return void
	 */ 
	void warning(const std::string message);

	/*!
 	 * \fn logError
	 * \brief method to display a message in the simulation, and quit it after the user enters a character
	 * \param message: the message to display
	 * \return void
	 */ 
	void logError(const std::string message);

	/*!
 	 * \fndebug
	 * \brief method to display a debug message in the simulation; only shown when Debug build
	 * \param message the message to display
	 * \return void
	 */ 
	void debug(const std::string message);

	/*!
	 * \fn assertion 
	 * \brief checks whether the given condition is true or not, throw an error if false
	 * \param condition: the condition to check
	 * \param message: the message associated to the error in case of failure
	 * \return void
	 */
	void assertion(bool condition, const std::string message);
}

#endif //ERRORLOGGER_H
