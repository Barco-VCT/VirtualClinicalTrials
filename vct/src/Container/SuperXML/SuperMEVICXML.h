//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file superMEVICXML.h
 * \brief Implementation of the class superMEVICXML
 * \date 1-April-2008
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

#ifndef SUPERMEVICXML_H
#define SUPERMEVICXML_H

#include <iostream>
#include "Parallel.h"
#include "Pipeline.h"
#include "Iterations.h"
#include "../iConvert/iConvert.h"
#include <vector>
#include <string>

/*!
 * \class SuperMEVICXML
 * \brief Class allowing to start a new simulation
 *		  Research the correct way to read the XML file, and start the simulation. 
 *		  The way to read the XML file must be described in the LIST_OF_ITERATIONS element of the XML, attribute name
 */
class SuperMEVICXML {
public:
	/*!
	 * \fn StartSimulation
	 * \brief Start a simulation using an XML file which path is given as input parameter
	 * \param progName: path and name of the executed binary relative to the current working directory
	 * \param progArgs: arguments starting with "--"
	 * \param xmlPath: path to the .xml file describing the simulation to execute
	 * \param simArgs: argument strings to replace .xml module parameters having placeholders like #0, #1, ...
	 * \param option: optional parameter used for debugging: 0: default, all is executed; 1: check the consistency only; 2: no consistency check; 3: high check consistency, warning becomes errors, see AbstractParameter for detail of the warnings
	 * \return	0 if the simulation has run correctly; 1 if a problem occured
	 */
	int StartSimulation(std::string& progName, std::vector<std::string>& progArgs, std::string& xmlPath, std::vector<std::string>& simArgs, int option);

private:
	/*!
	 * \fn stripExecName
	 * \brief Turn execPath into the path with the executable (usually MEVIC.exe) including a trailing path separator if there was any.
	 *		  Input type examples: "MEVIC.exe", "C:\dir\dir 2\MEVIC.exe", "/usr/local/mevic/mevic3-xxx.exe", "/MEVIC.exe"
	 */
	void stripExecName(std::string& execPath) const;
};

#endif // SUPERMEVICXML_H
