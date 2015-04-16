//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file FactoryParameter.h
 * \brief Implementation of the class FactoryParameter
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

#ifndef FACTORYPARAMETER_H
#define FACTORYPARAMETER_H

#include "AbstractParameter.h"
#include "ParamIt.h"
#include "ParamBasic.h"
#include "ParamStep.h"
#include "ParamItFormatted.h"
#include "ParamStar.h"
#include "ParamString.h"
#include "../../Container/ErrorManagement/ErrorLogger.h"

/*!
 * \class FactoryParameter
 * \brief From a value passed in parameter, allows to find the coresponding parameter type
 */
class FactoryParameter {
public:
	/*!
	 * \fn ConfigureParameter
	 * \brief From the value passed in input parameter, return the corresponding parameter type.
	 *		  if a new type of parameter is created, its identifier pattern must be added in this method
	 * \param parameterValue: value of the parameter to idenfity
	 * \return AbstractParameter*: Pointer to the identified object; NULL if the type has not be identified
	*/
	static AbstractParameter* ConfigureParameter(std::string parameterValue);
	
};

#endif //FACTORYPARAMETER_H
