#include "FileFinder.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <list>

#include <iConvert/iConvert.h>

using namespace iConvert;

bool FileFinderImpl::sortFindFileData(FindFileData_t first, FindFileData_t second)
{
	return (first < second);
}

FileFinderImpl::FileFinderImpl()
:m_targetDir(""),
 m_targetSearch("")
{
	m_findFileData.clear() ;
}

bool FileFinderImpl::initSearch(std::string targetDir, std::string targetSearch)
{
	//! Initialize search
	m_targetDir    = targetDir;
	m_targetSearch = targetSearch;

	//! Return
	return true;
}

bool FileFinderImpl::findFirst(std::string &fileNameFound)
{
	//! List all files in the target directory matching the search criterium
	bool searchState;
	m_findFileData.clear();
	// Boost's target directory path
	const boost::filesystem::path targetDirPath(m_targetDir.c_str());

	if (!boost::filesystem::exists(targetDirPath) || !boost::filesystem::is_directory(targetDirPath))
	{
		searchState = false;
		std::cerr << "[FileFinderImpl::findFirst] Error: unable to initialize search for:" << std::endl << m_targetDir.c_str() << std::endl;
		fileNameFound = "";
	}
	else
	{
		searchState = true;

		// Boost's target search criterium regular expression
		const boost::regex targetSearchRegex(m_targetSearch.c_str());

		// List all files in target directory matching target search criterium
		boost::filesystem::directory_iterator endItr; // default construction yields past-the-end
		for(boost::filesystem::directory_iterator i(targetDirPath); i != endItr; ++i )
		{
			// Skip if not a file
			if(!(boost::filesystem::is_regular_file(i->status()) || boost::filesystem::is_directory(i->status()))) continue;
			boost::smatch what;
			// Skip if no match
			if(!boost::regex_match(i->leaf(), what, targetSearchRegex)) continue;
			std::cout << i->leaf() << std::endl;
			// File matches, store it
			m_findFileData.push_back(i->leaf());
		}

		//! Get the first file if any
		if (m_findFileData.empty())
		{
			searchState = false;
			std::cerr << "[FileFinderImpl::findFirst] Error: no files found for search criterium:" << std::endl << m_targetSearch.c_str() << std::endl << m_targetDir.c_str();
			fileNameFound = "";
		}
		else
		{
			searchState = true;

			//! Sort the list of files alphabetically
			m_findFileData.sort(FileFinderImpl::sortFindFileData);

			//! Get the first file
			FindFileData_t firstFile = m_findFileData.front();
			fileNameFound = firstFile.c_str();
			while(fileNameFound[0] == '.')
			{
				m_findFileData.pop_front();
				firstFile = m_findFileData.front();
				fileNameFound = firstFile.c_str();
			}
			m_findFileData.pop_front();
		}
	}
	return searchState;
}

bool FileFinderImpl::findNext(std::string &fileNameFound)
{
	//! Get the next file if any
	bool fileFound = !m_findFileData.empty();
	if (!fileFound)
	{
		fileNameFound = "";
	}
	else
	{
		FindFileData_t nextFile = m_findFileData.front();
		fileNameFound = nextFile.c_str();
		while(fileNameFound[0] == '.')
		{
			m_findFileData.pop_front();
			nextFile = m_findFileData.front();
			fileNameFound = nextFile.c_str();
			
		}
		m_findFileData.pop_front();
	}

	//! Return
	return fileFound;
}

FileFinderImpl::~FileFinderImpl()
{
}

FileFinder::FileFinder()
:m_pFileFinder(0)
{
	m_pFileFinder = new FileFinderImpl();
}

bool FileFinder::initSearch(std::string targetDir, std::string targetSearch)
{
	return m_pFileFinder->initSearch(targetDir, targetSearch);
}

bool FileFinder::findFirst(std::string &fileNameFound)
{
	return m_pFileFinder->findFirst(fileNameFound);
}

bool FileFinder::findNext(std::string &fileNameFound)
{
	return m_pFileFinder->findNext(fileNameFound);
}

FileFinder::~FileFinder()
{
	if (m_pFileFinder != 0)
	{
		delete m_pFileFinder;
		m_pFileFinder = 0;
	}
}
