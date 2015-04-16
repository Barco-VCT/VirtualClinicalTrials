//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamItFormatted.h
 * \brief Implementation of the class ParamItFormatted
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

#ifndef PARAMITFORMATTED_H
#define PARAMITFORMATTED_H

#include "AbstractParameter.h"
#include "ParamIt.h"

/*!
 * \class ParamItFormatted
 * \brief initializing an iterating parameter (see ParamIt), but the ouput is formatted
 *		  Syntax: [min:step:max][number:nbOf0]
 *		  Example: [9:1:11][number:000] --> The returned value are: 009, 010, 011; whereas for the normal iterating parameter, the returned value were: 9, 10, 11
 *		  See ParamIt for more information
 */
class ParamItFormatted : public ParamIt {
public:
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
	 * \brief accessor for getting the incremented parameter value. Returns "" if the value is out of range.
	 * \return string: the increment value. "" if out of range.
	 */
	std::string GetNextIterationValue();

	/*!
	 * \fn GetCurrentPositionAndIterate
	 * \brief Returns the current value of the parameter and then increments it
	 * \return string: current value of the parameter. "" if a problem occured
	 */
	std::string GetCurrentPositionAndIterate();

	/*!
	 * \fn match
	 * \brief pattern for identifying the parameter
	 * \param p: value of the parameter with which the pattern must be checked
	 * \return bool
	 */
	static bool match(std::string & p);

private:

	/*!
	 * \enum option 
	 * \brief Informs about the type of the option to format the ouput (in the example: number)
	 *		  Present to add easily a new option in the object
	 */
	enum option {
		NUMBER,			//!< The option is number: formatting the ouput with a given number of digit.
		UNDEFINED		//!< Undefined option
	};

	enum option _opt;	//!< Nature of the current option

	std::string	_pattern,	//!< Value of the parameter containing the variables to initialize the parameter
			_option;	//!< Option used to format the ouput
};

#endif // PARAMITFORMATTED_H

