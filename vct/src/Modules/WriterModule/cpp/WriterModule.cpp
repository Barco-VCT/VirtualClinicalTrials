#include "../WriterModule.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

WriterModule::WriterModule()
: m_filename(""),
m_compression(false),
m_savelastcomponent(false),
m_deletecontainer(1)
{
}

void WriterModule::SetParameter(const std::string & name, const std::string & value )
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
			m_pMevicLogger->logError("WriterModule::SetParameter: the parameter provided in \"Compression\" cannot be casted to bool");
		}
	}
	else if(sName == "savelastcomponent")
	{
		try
		{
			m_savelastcomponent = boost::lexical_cast<bool>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("WriterModule::SetParameter: the parameter provided in \"SaveLastComponent\" cannot be casted to bool");
		}
	}
	else if(sName == "deletecontainer")
	{
		try
		{
			m_deletecontainer = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("WriterModule::SetParameter: the parameter provided in \"DeleteContainer\" cannot be casted to int");
		}
	}
	else
	{
		m_pMevicLogger->logError("WriterModule::SetParameter: Invalid parameter name");
	}

}
	
void WriterModule::Simulate( vector<DSContainer*>& containers )
{
	/* Any container? */
	if( !containers.empty() )
	{
		/* Save the last element of the vector to a file */
		DSContainer* container = containers.back();

		/* Correct filename ? */
		if( m_filename.size() > 0 )
		{
			if( m_compression == true )
			{
				if( m_savelastcomponent == true )
					container->SaveToFileWithCompressionLastComponent( (char*) m_filename.c_str() );
				else
					container->SaveToFileWithCompression( (char*) m_filename.c_str() );
			}
			else
				container->SaveToFile( (char*) m_filename.c_str() );	

			if(m_debug)
				m_pMevicLogger->logMessage("written file " + m_filename);

			/* Remove it from the vector */
			if (m_deletecontainer == 1 ) 
			{
				delete container;
				containers.pop_back();
			}
		}
	}
}
WriterModule::~WriterModule()
{
}
