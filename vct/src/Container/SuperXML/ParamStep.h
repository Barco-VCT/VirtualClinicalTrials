//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamStep.h
 * \brief Implementation of the class ParamStep
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

#ifndef PARAMSTEP_H
#define PARAMSTEP_H

#include "AbstractParameter.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "../iConvert/iConvert.h"

/*!
 * \class ParamStep
 * \brief Parameter containing a list a values
 *		  Syntax: [a,b,c,d]. Not limits on the numbers of values. Can contain anything exceped comma: the comma is used by the program to identify two different values. For the float / double, instead of write 4,5 write 4.5
 *		  Example: [0,6.5,78,24] \n
 *		  At each iteration the next value of the list is selected. There is no sorting  performed on these values, i.e. the first value of the list will be the first returned, the second of the list the second returned, ...
 */
class ParamStep : public AbstractParameter {
public:
	/*!
	 * Constructor
	 */
	ParamStep();

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
	std::vector<std::string> _values;	//!< List containing the different values of the 
	int _current;						//!< Current value of the list selected
};

#endif // PARAMSTEP_H