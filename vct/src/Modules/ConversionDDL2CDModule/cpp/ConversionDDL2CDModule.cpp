#include "../ConversionDDL2CDModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;

ConversionDDL2CDModule::ConversionDDL2CDModule()
:m_unit(""),
 m_illum(""),
 m_obs(""),
 m_number_of_bits(0),
 m_element_type(TYPE_BYTE),
 m_input_number_of_frames(0),
 m_output_number_of_frames(0),
 m_frame_rate(0),
 m_last_component_number(0),
 m_width(0),
 m_height(0),
 m_max(0),
 m_min(0),
 m_input_component_number(0),
 m_output_component_number(0),
 m_output_number_of_bits(0),
 m_1DDL2CD_0CD2DDL(0),
 m_ud(0)
{
	m_white_point[0] = m_white_point[1] = m_white_point[2] = 0;

}	

void ConversionDDL2CDModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameters to lower cased strings for easier handling.
	string sName  = boost::to_lower_copy(name);

	if(sName == "_1ddl2cd_0cd2ddl")
	{
		try
		{
			m_1DDL2CD_0CD2DDL = boost::lexical_cast<unsigned int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("ConversionDDL2CDModule::SetParameter: the parameter provided in \"_1DDL2CD_0CD2DDL\" cannot be casted to unsigned int");
		}
	}
	else
	{
		m_pMevicLogger->logError("ConversionDDL2CDModule::SetParameter: incorrect parameter");
	}
}

void ConversionDDL2CDModule::Simulate( std::vector<DSContainer*>& list)
{
	DSContainer* container = list.back();

	// extract or play with the last component, --> extract number of last component.
	m_input_component_number = container->GetLastComponentNumber();
	m_output_component_number = m_input_component_number+1;

	try
	{
		//get utilitiesDisplay
		m_ud = dynamic_cast<UtilitiesDisplay*>(m_pUtilitiesExchanger->requestUtilityObject("UtilitiesDisplay"));
	}
	catch(...)
	{
		m_pMevicLogger->logError("Utilities display request failed");
	}
	if (m_ud==0)
		m_pMevicLogger->logError("Utilities display request failed");

	//check bits consistency
	container->GetComponentNumberOfBits(m_input_component_number, &m_output_number_of_bits);
	if(m_output_number_of_bits!=m_ud->getNbBits())
	{
		m_pMevicLogger->logError("display and component have not the same number of bits");
	}
	
	
	// test to know if last component is valid or not
	if (container->GetComponentValid(m_input_component_number)==-1) 
	{
		m_pMevicLogger->logError("Component non valid");
	}

	enum ComponentType eType;

	container->GetComponentElementType(m_input_component_number, &eType);
	if(eType!=TYPE_IMAGE_RGB && eType!=TYPE_IMAGE_XYZ)
	{
		m_pMevicLogger->logError("module not ready for entry != TYPE_IMAGE_RGB");
	}

	int nbChannel =container->GetNbChannel(eType);
	
	// width and height for the output component
	container->GetComponentNumberOfColumnElementsPerFrame(m_input_component_number,&m_width);
	container->GetComponentNumberOfRowElementsPerFrame(m_input_component_number, &m_height);
	container->GetComponentNumberOfFrames(m_input_component_number, &m_input_number_of_frames);
	
	m_output_number_of_frames=m_input_number_of_frames;

	// create new component to save output of ConversionDDL2CDModule
	container->GetComponentUnit(m_input_component_number, m_unit);
	container->GetComponentIlluminant(m_input_component_number, m_illum);
	container->GetComponentNumberOfBits(m_input_component_number, &m_output_number_of_bits);
	container->GetComponentObserver(m_input_component_number, m_obs);
	container->GetComponentWhitePoint(m_input_component_number, m_white_point);
	m_number_of_bits=m_output_number_of_bits;

	if (m_1DDL2CD_0CD2DDL==1)
	{
		if (nbChannel==1)
		{
			container->CreateComponent(m_height, m_width, 0, 0, 
				m_unit,
				m_number_of_bits,
				m_white_point,
				m_illum,
				m_obs,
				TYPE_FLOAT,
				m_output_number_of_frames,
				m_frame_rate);
		}
		else
		{
			container->CreateComponent(m_height, m_width, 0, 0, 
				m_unit,
				m_number_of_bits,
				m_white_point,
				m_illum,
				m_obs,
				TYPE_IMAGE_XYZ,
				m_output_number_of_frames,
				m_frame_rate);
		}
	}
	else if (m_1DDL2CD_0CD2DDL==0)
	{	
		if (nbChannel==1)
		{
			container->CreateComponent(m_height, m_width, 0, 0, 
				m_unit,
				m_number_of_bits,
				m_white_point,
				m_illum,
				m_obs,
				TYPE_IMAGE_GRAY,
				m_output_number_of_frames,
				m_frame_rate);
		}
		else
		{
			container->CreateComponent(m_height, m_width, 0, 0, 
				m_unit,
				m_number_of_bits,
				m_white_point,
				m_illum,
				m_obs,
				TYPE_IMAGE_RGB,
				m_output_number_of_frames,
				m_frame_rate);
		}
	}
	else m_pMevicLogger->logError("Parameter _1DDL2CD_0CD2DDL should be either 0 or 1");

	int *rgb= new int[nbChannel];
	float *xyz= new float[nbChannel];
	// Set the new component with modified target curve & LUT
	for (int i=0; i<int (m_height); i++)
	{
		for (int j=0; j<int (m_width); j++)
		{
			for (int k=0; k<int (m_output_number_of_frames);k++) 
			{
				if (m_1DDL2CD_0CD2DDL==1)
				{
					container->GetComponentElement(this->m_input_component_number,i*m_width+j,k,rgb);
					for (int c=0; c<nbChannel; c++) //for each channel
					{
						xyz[c]=m_ud->getNativeCurve(GRAY)[rgb[c]][1];						
					} 
					container->SetComponentElement(this->m_output_component_number,i*m_width+j,k,xyz);
				}
				else if (m_1DDL2CD_0CD2DDL==0)
				{
					container->GetComponentElement(this->m_input_component_number,i*m_width+j,k,xyz);
					for (int c=0; c<nbChannel; c++) //for each channel
					{
						rgb[c]=m_ud->return_ddl_value_BinarySearch(xyz[c],1, GRAY);
					} 					
					container->SetComponentElement(this->m_output_component_number,i*m_width+j,k,rgb);
				}
			}
		}
	}
	if(m_1DDL2CD_0CD2DDL==1)
	{
		float white[3];
		white[0] = m_ud->getNativeCurve(GRAY)[(int)pow(2.0f,m_ud->getNbBits())-1][0];
		white[1] = m_ud->getNativeCurve(GRAY)[(int)pow(2.0f,m_ud->getNbBits())-1][1];
		white[2] = m_ud->getNativeCurve(GRAY)[(int)pow(2.0f,m_ud->getNbBits())-1][2];
		container->SetComponentWhitePoint(this->m_output_component_number,white);
	}

	if(0 != rgb)
	{
		delete[] rgb;
		rgb = 0;
	}
	if(0 != xyz)
	{
		delete[] xyz;
		xyz = 0;
	}
}

ConversionDDL2CDModule::~ConversionDDL2CDModule()
{	
}
