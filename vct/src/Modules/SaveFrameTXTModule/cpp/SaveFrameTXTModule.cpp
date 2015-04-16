#include "../SaveFrameTXTModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

SaveFrameTXTModule::SaveFrameTXTModule()
:m_filename(""),
 m_component_to_write(0),
 m_frame_to_write(-1),
 m_channel_to_write(-1)
{
}	

void SaveFrameTXTModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameters to lower cased strings for easier handling.
	string sName  = boost::to_lower_copy(name);

	if(sName == "filename")
	{
		m_filename = value;
	}
	else if(sName == "componenttowrite")
	{
		try
		{
			m_component_to_write = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameTXTModule::SetParameter: the parameter provided in \"ComponentToWrite\" cannot be casted to int");
		}
	}
	else if(sName == "frametowrite")
	{
		try
		{
			m_frame_to_write = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameTXTModule::SetParameter: the parameter provided in \"FrameToWrite\" cannot be casted to int");
		}
	}
	else if(sName == "channeltowrite")
	{
		try
		{
			m_channel_to_write = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameTXTModule::SetParameter: the parameter provided in \"ChannelToWrite\" cannot be casted to int");
		}
	}
	else
	{
		m_pMevicLogger->logError("SaveFrameTXTModule::SetParameter: Invalid parameter name");
	}
}

void SaveFrameTXTModule::Simulate( vector<DSContainer*>& list)
{
	DSContainer* container = list.back();

	// test to know if the component is valid or not
	if (container->GetComponentValid(m_component_to_write)==-1) 
	{
		m_pMevicLogger->logError("Component non valid");
	}
	
	//files write
	if (!container->SaveFrameTXT(m_component_to_write, m_frame_to_write, m_filename.c_str(), m_channel_to_write))
	{
		m_pMevicLogger->logError("SaveFrameTXT call failed");
	}
	int test = 0;
}

SaveFrameTXTModule::~SaveFrameTXTModule()
{
}

