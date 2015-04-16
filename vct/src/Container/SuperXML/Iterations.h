//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Iterations.h
 * \brief Implementation of the class Iterations
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

#include "AbstractSimulation.h"
#include <iostream>
#include <map>
#include <vector>
#include "../iConvert/iConvert.h"
#include <string>
#include "FactoryParameter.h"
#include "AbstractParameter.h"

#ifndef Iterations_h
#define Iterations_h

/*!
 * \class Iterations
 * \brief Class excecuting the different simulations using the iteration mode
 *		  In the iteration mode, the simulation \e x is excecuted \e n times, the simulation <em> x+1 </em> is executed \e m times, ...
 *		  GlobalParameters are not taken into account for this case; as well as the input / output temporary files management (see InOutManagement)
 */
class Iterations:public AbstractSimulation{
public:
	/*!
	 * \fn CheckConsistency
	 * \brief Check the conistency of the XML file
	 * \param root: reference to the first node of the XML file
	 * \param option: optional parameter; 0: default value, normal excecution; 1: high level excecution, the warnings becomes errors, see AbstractParameter for more detail about warnings
	 * \return	bool: true (for this version)
	 */
	bool CheckConsistency(XMLNode root, int option=0);

	/*!
	 * \fn StartSimulation
	 * \brief Start the execution of the different simulations of the XML file
	 * \param	PathToXML	Path to the XML file containing the description of the different simulations
	 * \param args: arguments from prompt command
	 * \return	true if the simulation run correctly, false otherwise
	 */
	bool StartSimulation (std::string& PathToXML, std::vector<std::string>& args);

private:
	/*!
	 * \struct simu
	 * brief Structure used to put in relation the identifier of the simluation and how many times it has to be excecuted.
	 */
	struct simu {
		std::string idSimu;				//!< Identifier of the simulation
		int numberOfIterations;		//!< Number of iterations the corresponding simulation have to be excecuted
	};

	std::vector<simu> _listOfIterations;	//!< Vector contianing the different simulation identifiers described in the XML file. The order the simulation are described in the \e ListOfIterations part of the XML file will describe the order they will be excecuted

	/*!
	 * \brief Map containing the different dynamic parameters
	 *		  syntax:			<simulation name, <module name, <parameter name, pointer to the corresponding parameter>>>
	 *		  simulation name:	name of the simulation the parameter refers to
	 *		  module name:		name of the module inside the simulation the parameter refers to
	 *		  parameter name:	name of the parameter
	 *		  pointer:			pointer to the parameter thus idenfitied (to do this, use FactoryParameter);
	 */
	std::map<std::string, std::map<std::string, std::map<std::string, AbstractParameter*> > > _localParameters; 

	/*!
	 * \fn initListOfIterations
	 * \brief Initialization of the ::_listOfIterations variable
	 * \param node: reference to the node of the XML file containing the description of the number of iterations of the different described simulations
	 * \return bool: true of theinitialization has been performed correctly; false otherwise
	 */
	bool initListOfIterations(XMLNode node);

	/*!
	 * \fn initListOfParameters
	 * \brief Initialization of the ::_localParameters variable
	 * \param node: reference to the node of the XML file containing the description of the local parameters of the different described simulations
	 * \return bool: true of theinitialization has been performed correctly; false otherwise
	 */
	bool initListOfParameters(XMLNode node);

	/*!
	 * \fn runSimulation
	 * \brief Excecution of the different simulations described in the XML file
	 *		  the simulation described in the _listOfIterations variables must be the same present in this part. Otherwise the program will crash
	 * \param node: reference to the node of the XML file containing the description of the different simulations to run
	 * \return bool: true if the simulations have been performed correctly; false if a problem occured
	 */
	bool runSimulation(XMLNode node);
};

#endif
