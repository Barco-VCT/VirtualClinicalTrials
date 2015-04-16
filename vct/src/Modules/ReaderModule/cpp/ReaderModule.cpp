#include "../ReaderModule.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

ReaderModule::ReaderModule()
: m_filename(""),
m_compression(false)
{

}

void ReaderModule::SetParameter(const std::string & name, const std::string & value )
{
	string sName  = boost::to_lower_copy(name);
	if(sName == "filename")
	{
		m_filename = value;
	}
	else if(sName == "compression")
	{
		try
		{
			m_compression = boost::lexical_cast<bool>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("ReaderModule::SetParameter: the parameter provided in \"Compression\" cannot be casted to bool");
		}
	}
	else
	{
		m_pMevicLogger->logError("ReaderModule::SetParameter: Invalid parameter name");
	}
}

void ReaderModule::Simulate( vector<DSContainer*>& containers )
{
	/* Verify filename */
	if( m_filename.size() == 0 )
		m_pMevicLogger->logError("no filename specified");

	/* Make a new container */
	DSContainer* container = new DSContainer();
		
	/* Load from file */
	if( m_compression == true )
		container->LoadFromFileWithCompression( (char*) m_filename.c_str() );
	else
		container->LoadFromFile( (char*) m_filename.c_str() );

	if(m_debug)
		m_pMevicLogger->logMessage("read file " + m_filename);

	/* Add to vector */
	containers.push_back( container );
}
ReaderModule::~ReaderModule()
{

}
