//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamBasic.h
 * \brief Implementation of the class ParamBasic
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

#ifndef PARAMBASIC_H
#define PARAMBASIC_H

#include "AbstractParameter.h"
#include <iostream>


/*!
 * \class ParamBasic
 * \brief Class managing a "basic" parameter, i.e. a parameter which does not require any specific process
 *		  A parameter will be classified as "basic" if it does not contain any square brackets. Example: "512"
 *		  The parameter could have any value, the class manages it as a string.
 */
class ParamBasic:public AbstractParameter {
public:
	/*!
	 * Constructor
	 */
	ParamBasic();

	/*!
	 * \fn CreateParameter
	 * \brief Initialization of the different parameters
	 * \param parameterValue: Value of the parameter to initialize
	 * \return bool: true if the initialization has been performed correctly; false otherwise
	 */
	bool CreateParameter(std::string parameterValue);

	/*!
	 * \fn GetCurrentPosition
	 * \brief accessor for getting the parameter value
	 * \return string: parameter value
	 */
	std::string GetCurrentPosition();

	/*!
	 * \fn GetNextIterationValue
	 * \brief Return the value of the parameter (no incrementation for a basic parameter)
	 * For this special case, equivalent to ParamBasic::GetCurrentPosition().
	 *
	 * @return value of the parameter
	 */
	/*!
	 * \fn GetNextIterationValue
	 * \brief accessor for getting the parameter value (no incrementation for a basic parameter)
	 *		  For this special case, equivalent to ParamBasic::GetCurrentPosition().
	 * \return string: parameter value
	 */
	std::string GetNextIterationValue();

	/*!
	 * \fn CheckConsistency
	 * \brief Nothing has to be checked for the consistency of this parameter. 
	 *		  Whatever is the value passed as input prameter, the returned value is true
	 * \param nbExpected: Any value
	 * \return	int: 0
	 */
	int CheckConsistency (int nbExpected);

	/*!
	 * \fn GetCurrentPositionAndIterate
	 * \brief accessor for getting the parameter value (no incrementation for a basic parameter)
	 *		  For this special case, equivalent to ParamBasic::GetCurrentPosition().
	 * \return string: value of the parameter
	 */
	std::string GetCurrentPositionAndIterate();

	/*!
	 * \fn Iterate
	 * \brief The value is static so this function does not do anything.
	 * \return bool: true
	 */
	bool Iterate();

	/*!
	 * \fn match
	 * \brief Pattern idenfiying a basic parameter: value which does not contain square brackets
	 * \return bool: true is the pattern is checked, false otherwise
	 */
	static bool match(std::string & p);

private:
	std::string _value; //!< Value of the parameter
};

#endif // PARAMBASIC_H