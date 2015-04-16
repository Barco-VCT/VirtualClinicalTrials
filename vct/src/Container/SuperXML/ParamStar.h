//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file ParamStart.h
 * \brief Implementation of the class ParamStart
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

#ifndef PARAMSTAR_H
#define PARAMSTAR_H

#include "AbstractParameter.h"
#include "../FileFinder/FileFinder.h"

/*!
 * \class ParamStar
 * \brief Type of parameter allowing to browse in the folder
 *		  This parameter should contain a path to the a folder, and [*] as identifier.
 *		  The files of the folders are read as Windows read them; normally by filename.
 *		  Example of this parameter: "F:\data\XMLParser\images\study\ref\[*].raw"
 */
class ParamStar: public AbstractParameter
{
public:
	/*!
	 * Constructor
	 * Initialisation of the class variables
	 */
	ParamStar();

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
	std::string	GetCurrentPosition();

	/*!
	 * \fn Iterate
	 * \brief increment the current value of the parameter
	 * \return	bool: true if the iteration has been performed, false if the value (before the incrementation) is out of range
	 */
	bool Iterate();

	/*!
	 * \fn GetNextIterationValue
	 * \brief Increments the value of the parameter and return the new value. Returns "" if the value is out of range.
	 * \return string: the incremented value, "" if out of range.
	 */
	std::string	GetNextIterationValue();

	/*!
	 * \fn GetCurrentPositionAndIterate
	 * \brief accessor for getting current parameter value and then increments it
	 * \return string: current value of the parameter. "" if a problem occured
	 */
	std::string	GetCurrentPositionAndIterate();

	/*!
	 * \fn CheckConsistency
	 * \brief Checks the consistency of the parameter following an expected number of iterations given as input parameter
	 * \param nbExpected: value number the parameter should have
	 * \return int: "0" if the parameter is cosistent; "1" is a warning: the simulation can run but all the possible values of the parameter will not be took in account; 2: error: too many iterations in comparison to the number of possible values of the parameter
	 */
	int	CheckConsistency (int nbExpected);

	/*!
	 * \fn match
	 * \brief pattern for identifying the parameter
	 * \param p: parameter value with which the pattern must be checked
	 * \return static bool
	 */
	static bool match(std::string & p);

private:
	FileFinder   m_fileFinder;   //!< FileFinder object providing OS specific implementation of File find functions
	std::string  m_pathToDir,    //!< Target directory full path name
				 m_targetSearch, //!< Target search criterium (wildcard)
	             m_fileName,     //!< Found file name
	             m_pathToFile;   //!< Found file full path name

	bool m_createState, //!< State of the initialization (create parameter); true if OK (done and successfully initialized), false if not OK (not yet done or error)
	     m_searchState, //!< State of the files search; true if OK (first search done and files found), false if not OK (not yet done or error) 
	     m_foundState;  //!< State of the files browsing; true if OK (next file found), false if not OK (no more files found)
};

#endif // PARAMSTAR_H

