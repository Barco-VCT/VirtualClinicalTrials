#include "ParamStar.h"

using namespace std;

ParamStar::ParamStar()
:m_pathToDir(""),
 m_fileName(""),
 m_pathToFile(""),
 m_createState(false),
 m_searchState(false), 
 m_foundState(false)
{
}

bool ParamStar::CreateParameter(std::string parameterValue)
{
	//! Parse the parameter and extract information
	std::string targetFile,      // Target file name
	            targetExtension, // Target file extension
	            targetWildcard;  // Target search wildcard

	// Get the target directory path
	m_pathToDir = parameterValue.substr(0, parameterValue.find_last_of('/') + 1); // up to last forward-slash (incl.)

	// Compose target search criterium
	targetFile      = parameterValue.substr(parameterValue.find_last_of('/') + 1, parameterValue.find('[') - (parameterValue.find_last_of('/') + 1)); // from (excl.) last forward-slash up to wild card delimiter "[]" (excl.)
	targetExtension = parameterValue.substr(parameterValue.find_last_of('.') + 1, parameterValue.length() - (parameterValue.find(']') + 1));          // from (excl.) last period
	
	// QUICK FIX
	if ( targetExtension == "" )
	{
		targetWildcard  = ".*";
	}
	else
	{
		targetWildcard  = ".*\\.";                                                                                                                        // Regular expression wildcard ".*" including period "\." (file name - extension delimiter)
	}
	m_targetSearch = targetFile + targetWildcard + targetExtension;

	//! Initialize parameter
	m_createState = m_fileFinder.initSearch(m_pathToDir, m_targetSearch);
	m_searchState = m_foundState = false;

	//! Return
	return m_createState;
}

bool ParamStar::Iterate()
{
	//! Iterate
	// Files search not yet initialized
	if (!m_searchState)
	{
		// Find first file
		m_foundState = m_searchState = m_fileFinder.findFirst(m_fileName);
	}
	else
	{
		// Find next file
		m_foundState = m_fileFinder.findNext(m_fileName);
	}
	
	//! Return
	return m_foundState;
}

std::string ParamStar::GetCurrentPosition()
{
	//! Get current position
	// Files search not yet initialized
	if (!m_searchState)
	{
		// Find first file
		m_foundState = m_searchState = m_fileFinder.findFirst(m_fileName);
	}

	// Beyond last file
	if (!m_foundState)
	{
		m_pathToFile = "";
	}
	else
	{
		m_pathToFile = m_pathToDir + m_fileName;
	}

	//! Return
	return m_pathToFile;
}

std::string ParamStar::GetCurrentPositionAndIterate()
{
	string currentPosition;

	//! Get current position
	currentPosition = GetCurrentPosition();
	//! Iterate
	Iterate();

	//! Return
	return currentPosition;
}

std::string ParamStar::GetNextIterationValue()
{
	string currentPosition;

	//! Iterate
	Iterate();
	//! Get current position
	currentPosition = GetCurrentPosition();

	//! Return
	return currentPosition;
}

int ParamStar::CheckConsistency(int nbExpected)
{
	//! Return
	return 0;
}

bool ParamStar::match(string & p)
{
	// Strip off Windows Drive letters in path, e.g. D:/... if any
	string pv;
	if (p.find("/") != string::npos)
		pv = p.substr(p.find_first_of("/"));
	else
		pv = p;

	if
	(
		// not allowed: , :
		   pv.find(",") != string::npos
		|| pv.find(":") != string::npos
		// syntax: foo[*]bar
		|| pv.find("[*]") == string::npos
		|| pv.find_first_of("[") != pv.find_last_of("[")
		|| pv.find_first_of("*") != pv.find_last_of("*")
		|| pv.find_first_of("]") != pv.find_last_of("]")
	) return false;
	return true;
}

