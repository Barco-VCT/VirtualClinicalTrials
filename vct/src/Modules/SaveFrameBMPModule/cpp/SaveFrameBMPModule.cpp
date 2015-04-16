#include "../SaveFrameBMPModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

SaveFrameBMPModule::SaveFrameBMPModule()
:m_unit(""),
 m_illum(""),
 m_obs(""),
 m_number_of_bits(0),
 m_input_number_of_frames(0),
 m_width(0),
 m_height(0),
 m_input_component_number(0),
 m_filename(""),
 m_component_number(-1),
 m_frame_number(-1),
 m_number_of_channels(-1),
 m_normalization(false)
{
	m_white_point[0] = m_white_point[1] = m_white_point[2] = 0;
}

void SaveFrameBMPModule::SetParameter(const string & name, const string & value)
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
	else if(sName == "component_number")
	{
		try
		{
			m_component_number = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameBMPModule::SetParameter: the parameter provided in \"component_number\" cannot be casted to int");
		}
	}
	else if(sName == "frame_number")
	{
		try
		{
			m_frame_number = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameBMPModule::SetParameter: the parameter provided in \"frame_number\" cannot be casted to int");
		}
	}
	else if(sName == "normalization")
	{
		if (value == "true")
		{
			m_normalization = true;
		}
		else if (value == "false")
		{
			m_normalization = false;
		}
		else 
		{
			m_normalization = false;
			m_pMevicLogger->logError("SaveFrameBMPModule::SetParameter: incorrect value for parameter \"normalization\"");
		}
	}
	else if(sName == "number_of_channels")
	{
		try
		{
			m_number_of_channels = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SaveFrameBMPModule::SetParameter: the parameter provided in \"number_of_channels\" cannot be casted to int");
		}
	}
	else
	{
		m_pMevicLogger->logError("SaveFrameBMPModule::SetParameter: Invalid parameter name");
	}
}

void SaveFrameBMPModule::Simulate( vector<DSContainer*>& list)
{
	DSContainer* container = list.back();
	
	// extract or play with the last component, --> extract number of last component.
	m_input_component_number=container->GetLastComponentNumber();


	// test to know if last component is valid or not
	if (container->GetComponentValid(m_input_component_number)==-1) 
	{
		m_pMevicLogger->logError( "Component non valid in SaveFrameBMPModule");
	}
	
	// m_width and m_height for the output component
	container->GetComponentNumberOfColumnElementsPerFrame(m_input_component_number,&m_width);
	container->GetComponentNumberOfRowElementsPerFrame(m_input_component_number, &m_height);
	container->GetComponentNumberOfFrames(m_input_component_number, &m_input_number_of_frames);


	// create new component to save output of SaveFrameBMPModule
	container->GetComponentUnit(m_input_component_number, m_unit);
	container->GetComponentIlluminant(m_input_component_number, m_illum);
	container->GetComponentNumberOfBits(m_input_component_number, &m_number_of_bits);
	container->GetComponentObserver(m_input_component_number, m_obs);
	container->GetComponentWhitePoint(m_input_component_number, m_white_point);

	container->SaveFrameBMP(m_component_number, m_frame_number, m_filename, m_normalization, m_number_of_channels);
}

SaveFrameBMPModule::~SaveFrameBMPModule()
{
}

