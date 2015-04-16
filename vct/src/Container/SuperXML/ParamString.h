//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamString.h
 * \brief Implementation of the class ParamString
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

#ifndef PARAMSTRING_H
#define PARAMSTRING_H

#include "ParamIt.h"
#include "ParamItFormatted.h"
#include "ParamStep.h"
#include "AbstractParameter.h"
#include <iostream>

/*!
 * \class ParamString
 * \brief Parameter composed of a string and a ParamIt, ParamItFormatted or a ParamStep 
 *		  (see these classes for more information about the syntax).
 *		  Syntax: \n 
 *		  blahblahblah[a,b,c,d]blahblahblah \n 
 *		  blahblahblah[a:b:c]blahblahblah \n 
 *		  blahblahblah[a:b:c][number:nbOf0]blahblahblah \n 
 *		  At each iteration the next value of the list is selected, and is returned the value inside the string.
 *		  The string must contain only one inside parameter
 */
class ParamString : public AbstractParameter {
public:
	/*!
	 * Constructor
	 */
	ParamString();

	/*!
	 * \fn CreateParameter
	 * \brief Initialization of the parameter
	 * \param parameterValue: Value of the string containing the parameters to initialize
	 * \return bool: true if the initialization was correct, false is a problem occured.
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
	 * \fn GetCurrentPositionAndIterate
	 * \brief accessor for getting current parameter value and then increments it
	 * \return string: current value of the parameter. "" if a problem occured
	 */
	std::string GetCurrentPositionAndIterate();

	/*!
	 * \fn CheckConsistency
	 * \brief Checks the consistency of the parameter following an expected number of iterations given as input parameter
	 * \param nbExpected: value number the parameter should have
	 * \return int: "0" if the parameter is cosistent; "1" is a warning: the simulation can run but all the possible values of the parameter will not be took in account; 2: error: too many iterations in comparison to the number of possible values of the parameter
	 */
	int CheckConsistency(int nbExpected);

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

	/*!
	 * Destructor
	 */
	~ParamString();

private:
	std::string	_part1,				//!< Part of the string before the inside parameter (i.e. before the [)
				_part2;				//!< Part of the string after the inside paramete (i.e. after the ])

	AbstractParameter * _ap;	//!< Pointer to the AbstractParameter object, function of the type of the inside parameter
};

#endif // PARAMSTRING_H