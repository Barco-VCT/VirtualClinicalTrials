//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamIt.h
 * \brief Implementation of the class ParamIt
 * \date 01-April-2008
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

#ifndef PARAMIT_H
#define PARAMIT_H

#include "AbstractParameter.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>


/*!
 * \class ParamIt
 * \brief initializing an iterating parameter
 *		  Syntax: [min:step:max]
 *		  Example: [0:1:10]
 *		  Incremented of step at each iteration. If > max, the returned value is "".
 *		  Between the colons, the values could be integers, float; but it should not contain letters. 
 *		  A float must have the syntax 0.4 for example and not 0,4
 */
class ParamIt:public AbstractParameter{
public:
	/*!
	 * Constructor
	 * \brief it does not initialize the parameter. For the initialization, use ::CreateParameter
	*/
	ParamIt();

	/*!
	 * \fn CreateParameter
	 * \brief Initialization of the variables of the parameters
	 * \param parameterValue: character string containing the different features of the parameter to initialize
	 * \return bool: true if the initialization has been performed correctly; false otherwise
	 */
	bool CreateParameter(std::string parameterValue);

	/*!
	 * \fn GetCurrentPosition
	 * \brief accessor for getting the current parameter value
	 * \return string: Current value of the parameter. Return "" if the value is out of range.
	 */
	std::string GetCurrentPosition();

	/*!
	 * \fn GetNextIterationValue
	 * \brief Increments the value of the parameter and return the new value. Returns "" if the value is out of range.
	 * \return string: the incremented value, "" if out of range.
	 */
	std::string GetNextIterationValue();

	/*!
	 * \fn CheckConsistency
	 * \brief Checks the consistency of the parameter following an expected number of iterations given as input parameter
	 * \param nbExpected: value number the parameter should have
	 * \return int: "0" if the parameter is cosistent; "1" is a warning: the simulation can run but all the possible values of the parameter will not be took in account; 2: error: too many iterations in comparison to the number of possible values of the parameter
	 */
	int CheckConsistency (int nbExpected);

	/*!
	 * \fn GetCurrentPositionAndIterate
	 * \brief accessor for getting current parameter value and then increments it
	 * \return string: current value of the parameter. "" if a problem occured
	 */
	std::string GetCurrentPositionAndIterate();

	/*!
	 * \fn Iterate
	 * \brief increment the current value of the parameter
	 * \return	bool: true if the iteration has been performed, false if the value (before the incrementation) is out of range
	 */
	bool Iterate();

	/*!
	 * \fn match
	 * \brief pattern for identifying the parameter
	 * \param p: parameter value with which the pattern must be checked
	 * \return static bool
	 */
	static bool match(std::string & p);

private:
	/*!
	 * \fn getSubstr
	 * \brief returns a substring from a string, its first character being firstChar, and its last being lastChar
	 * \param str: character string from which the substring is extracted
	 * \param firstChar: first character of the researched substring
	 * \param lastChar: last character of the researched substring
	 * \return string: substring which first character is firstChar and last character is lastChar
	 */
	std::string getSubstr(std::string str, std::string firstChar, std::string lastChar){return str.substr(str.find_first_of(firstChar)+1,str.find(lastChar,str.find_first_of(firstChar)+1)-(str.find_first_of(firstChar)+1));};

	/*!
	 * \fn hash
	 * \brief method used to extract the 3 numbers from the parameter value
	 * \param str: value of the string containing the 3 numbers to extract. The first character must be [, the last ], and the numbers must be separated by a colon. The numbers could be integer, float or double
	 * \return double: array of double. Normally 3: min, step, max. Returns NULL if a problem occured.
	 */
	double* hash(std::string str);

	double	_min,		//!< Minimum value of the parameter
			_max,		//!< Maximum value of the parameter
			_step,		//!< Value used to increment the value of the parameter
			_current;	//!< Current value of the parameter
};

#endif // PARAMIT_H

