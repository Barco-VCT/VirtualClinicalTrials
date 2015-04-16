//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file FileFinder.h
 * \brief Implementation of the class FileFinder
 * \date 20-Oct-2009
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

#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <string>
#include <list>


typedef std::string FindFileData_t;

class FileFinderImpl;

/*!
 * \class FileFinder
 * \brief Generic file finder interface
 */
class FileFinder
{
private:
	FileFinderImpl *m_pFileFinder;

public:
	/*!
	 * Constructor
	 */
	FileFinder();

	/*!
	 * \fn initSearch
	 * \brief Initialize search
	 * \param targetDir Target directory
	 * \param targetSearch Target search criterium
	 * \return true if success, false if not
	 */
	bool initSearch(std::string targetDir, std::string targetSearch);
	
	/*!
	 * \fn findFirst
	 * \brief find first file
	 * \param fileNameFound: first found file path name
	 * \return bool: true if file found, false if not
	 */
	bool findFirst(std::string &fileNameFound);
	
	/*!
	 * \fn findNext
	 * \brief Find next file
	 * \param fileNameFound: next found file path name
	 * \return bool: true if file found, false if not
	 */
	bool findNext(std::string &fileNameFound);
	
	/*!
	 * Destructor
	 */
	~FileFinder();
};

/*!
 * \class FileFinderImpl
 */
class FileFinderImpl
{
private:
	std::string    m_targetDir,    //!< Target directory
				   m_targetSearch; //!< Target search criterium

	std::list<FindFileData_t> m_findFileData; //!< Found file data

	static bool sortFindFileData(FindFileData_t first, FindFileData_t second); //! FindFileData_t sorting algorithm

public:
	/*!
	 * Constructor
	 */
	FileFinderImpl();
	
	/*!
	 * \fn initSearch
	 * \brief Initialize search
	 * \param targetDir: Target directory
	 * \param targetSearch: Target search criterium
	 * \return bool: true if success, false if not
	 */
	bool initSearch(std::string targetDir, std::string targetSearch);
	
	/*!
	 * \fn findFirst
	 * \brief Find first file
	 * \param fileNameFound first found file path name
	 * \return bool: true if file found, false if not
	 */
	bool findFirst(std::string &fileNameFound);
	
	/*!
	 * \fn findNext
	 * \brief Find next file
	 * \param fileNameFound: next found file path name
	 * \return bool: true if file found, false if not
	 */
	bool findNext(std::string &fileNameFound);
	
	/*!
	 * Destructor
	 */
	~FileFinderImpl();
};

#endif // FILEFINDER_H
