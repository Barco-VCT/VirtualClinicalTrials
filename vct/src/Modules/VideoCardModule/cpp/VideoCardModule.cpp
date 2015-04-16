#include "../VideoCardModule.h"

#include <cmath>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

/** 
 * Constructor of the class. 
 */ 
VideoCardModule::VideoCardModule()
:m_unit(""),
 m_illum(""),
 m_obs(""),
 m_width(0),
 m_height(0),
 m_number_of_bits_in(0),
 m_number_of_bits_out(0),
 m_input_component_number(0),
 m_output_component_number(0),
 m_element_type(TYPE_BYTE),
 m_frame_rate(0),
 m_output_number_of_frames(0),
 m_input_number_of_frames(0)
{
	m_white_point[0] = m_white_point[1] = m_white_point[2] = 0;
}

void VideoCardModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameters to lower cased strings for easier handling.
	string sName  = boost::to_lower_copy(name);

	if(sName == "number_of_bits_out")
	{
		try
		{
			m_number_of_bits_out = boost::lexical_cast<unsigned int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("VideoCardModule::SetParameter: the parameter provided in \"number_of_bits_out\" cannot be casted to unsigned int");
		}
	}
	else m_pMevicLogger->logError("VideoCardModule::SetParameter: invalid parameter name");
}

void VideoCardModule::Simulate( vector<DSContainer*>& list)
{
	DSContainer* container = list.back();
	
	// extract or play with the last component, --> extract number of last component.
	m_input_component_number=container->GetLastComponentNumber();
	m_output_component_number=m_input_component_number+1;

	// test to know if last component is valid or not
	if (container->GetComponentValid(m_input_component_number)==-1) 
	{
		m_pMevicLogger->logError("component is not valid");
	}

	// test to know if last component is gray or rgb
	// if not stop program, input for VideoCardModule is Gray or RGB
	container->GetComponentElementType(m_input_component_number, &m_element_type);
	if (m_element_type!=TYPE_IMAGE_GRAY && m_element_type!=TYPE_IMAGE_RGB) 
	{
		m_pMevicLogger->logError("m_element_type non valid");
	}
	
	// m_width and m_height for the output component
	container->GetComponentNumberOfColumnElementsPerFrame(m_input_component_number,&m_width);
	container->GetComponentNumberOfRowElementsPerFrame(m_input_component_number, &m_height);
	// ask for number of frames
	// if m_input_number_of_frames==1 -> m_output_number_of_frames=10
	// else m_output_number_of_frames=m_input_number_of_frames
	container->GetComponentNumberOfFrames(m_input_component_number, &m_input_number_of_frames);
	if (m_input_number_of_frames==1)
		m_output_number_of_frames=1;
	else
		m_output_number_of_frames=m_input_number_of_frames;

	// create new component to save output of VideCardModule
	container->GetComponentUnit(m_input_component_number, m_unit);
	container->GetComponentIlluminant(m_input_component_number, m_illum);
	container->GetComponentNumberOfBits(m_input_component_number, &m_number_of_bits_in);
	container->GetComponentObserver(m_input_component_number, m_obs);
	container->GetComponentWhitePoint(m_input_component_number, m_white_point);

	if (m_number_of_bits_in<m_number_of_bits_out) 
	{
		m_pMevicLogger->logError("input number_of_bits_input < number_of_bits_output");
	}
	container->CreateComponent(m_height, m_width, 0, 0, 
		m_unit,
		m_number_of_bits_out,
		m_white_point,
		m_illum,
		m_obs,
		TYPE_IMAGE_RGB,
		m_output_number_of_frames,
		m_frame_rate);

	// Dithering, second stage simulation in VideoCardModule
	InitializeDitheringTables();

	if (m_element_type==TYPE_IMAGE_GRAY)
	{ 
		Dithering_GRAY(container);  // Gray LUT
	}
	else 
	{
		Dithering_RGB(container);	// RGB LUT
	}
}

void VideoCardModule::InitializeDitheringTables()
{
	m_tab_0_50_frame_1[0][0]=1;	m_tab_0_50_frame_1[0][1]=0;
	m_tab_0_50_frame_1[1][0]=0;	m_tab_0_50_frame_1[1][1]=1;

	m_tab_0_50_frame_2[0][0]=0;	m_tab_0_50_frame_2[0][1]=1;
	m_tab_0_50_frame_2[1][0]=1;	m_tab_0_50_frame_2[1][1]=0;

	m_tab_0_25_frame_1[0][0]=1;	m_tab_0_25_frame_1[0][1]=1;	m_tab_0_25_frame_1[0][2]=0;
	m_tab_0_25_frame_1[1][0]=1;	m_tab_0_25_frame_1[1][1]=0;	m_tab_0_25_frame_1[1][2]=1;
	m_tab_0_25_frame_1[2][0]=0;	m_tab_0_25_frame_1[2][1]=1;	m_tab_0_25_frame_1[2][2]=1;

	m_tab_0_25_frame_2[0][0]=1;	m_tab_0_25_frame_2[0][1]=0;	m_tab_0_25_frame_2[0][2]=1;
	m_tab_0_25_frame_2[1][0]=0;	m_tab_0_25_frame_2[1][1]=1;	m_tab_0_25_frame_2[1][2]=1;
	m_tab_0_25_frame_2[2][0]=1;	m_tab_0_25_frame_2[2][1]=1;	m_tab_0_25_frame_2[2][2]=0;

	m_tab_0_25_frame_3[0][0]=0;	m_tab_0_25_frame_3[0][1]=1;	m_tab_0_25_frame_3[0][2]=1;
	m_tab_0_25_frame_3[1][0]=1;	m_tab_0_25_frame_3[1][1]=1;	m_tab_0_25_frame_3[1][2]=0;
	m_tab_0_25_frame_3[2][0]=1;	m_tab_0_25_frame_3[2][1]=0;	m_tab_0_25_frame_3[2][2]=1;

	m_tab_0_75_frame_1[0][0]=0;	m_tab_0_75_frame_1[0][1]=0;	m_tab_0_75_frame_1[0][2]=1;
	m_tab_0_75_frame_1[1][0]=0;	m_tab_0_75_frame_1[1][1]=1;	m_tab_0_75_frame_1[1][2]=0;
	m_tab_0_75_frame_1[2][0]=1;	m_tab_0_75_frame_1[2][1]=0;	m_tab_0_75_frame_1[2][2]=0;

	m_tab_0_75_frame_2[0][0]=0;	m_tab_0_75_frame_2[0][1]=1;	m_tab_0_75_frame_2[0][2]=0;
	m_tab_0_75_frame_2[1][0]=1;	m_tab_0_75_frame_2[1][1]=0;	m_tab_0_75_frame_2[1][2]=0;
	m_tab_0_75_frame_2[2][0]=0;	m_tab_0_75_frame_2[2][1]=0;	m_tab_0_75_frame_2[2][2]=1;

	m_tab_0_75_frame_3[0][0]=1;	m_tab_0_75_frame_3[0][1]=0;	m_tab_0_75_frame_3[0][2]=0;
	m_tab_0_75_frame_3[1][0]=0;	m_tab_0_75_frame_3[1][1]=0;	m_tab_0_75_frame_3[1][2]=1;
	m_tab_0_75_frame_3[2][0]=0;	m_tab_0_75_frame_3[2][1]=1;	m_tab_0_75_frame_3[2][2]=0;
}

void VideoCardModule::Dithering_GRAY(DSContainer *container)
{
	int gray=0;
	int final_value[3]={0};

	for (int i=0; i<int (m_height); i++) 
	{
		for (int j=0; j<int (m_width); j++) 
		{
			for (int k=0; k<int (m_output_number_of_frames);k++) 
			{
				container->GetComponentElement(m_input_component_number, i*m_width+j, k, &gray);

				final_value[0]=final_value[1]=final_value[2]=GetDitheredValue(k, i, j, gray);

				container->SetComponentElement(m_output_component_number, i*m_width+j, k, (void*) &final_value);
			}
		}
	}

}

void VideoCardModule::Dithering_RGB(DSContainer *container)
{
	int rgb[3]={0};
	int final_value[3]={0};
	int temp[3]={0};

	for (int i=0; i<int (m_height); i++) 
	{
		for (int j=0; j<int (m_width); j++) 
		{
			for (int k=0; k<int (m_output_number_of_frames);k++) 
			{
				container->GetComponentElement(m_input_component_number, i*m_width+j, k, &rgb);

				final_value[0]=GetDitheredValue(k, i, j, rgb[0]);
				final_value[1]=GetDitheredValue(k, i, j, rgb[1]);
				final_value[2]=GetDitheredValue(k, i, j, rgb[2]);

				container->SetComponentElement(m_output_component_number, i*int (m_width)+j, k, &final_value);
			}
		}
	}
}

int VideoCardModule::GetDitheredValue(int frame, int row_position, int column_position, int value_int)
{
	int test_frame_number;

	float factor=pow(float(2),float(m_number_of_bits_in))/pow(float(2),float(m_number_of_bits_out));

	float value_float = float(value_int)/factor; //interpolation if required

	value_int = int(value_int/factor); //interpolation if required

	float difference = value_float-value_int;

	float final_value;

	// special dithering with tab_0.50
	if (difference==0.50) 
	{
		test_frame_number=frame%2;
		if (test_frame_number==0) 
			final_value = float (value_int) + m_tab_0_50_frame_1[row_position%2][column_position%2];
		else
			final_value = float (value_int) + m_tab_0_50_frame_2[row_position%2][column_position%2];
	}
	else
		if (difference == 0.25) { // special dithering with tab_0.25
			test_frame_number = frame%3;
			if (test_frame_number == 0) 
				final_value = float (value_int) + m_tab_0_25_frame_1[row_position%3][column_position%3];
			else
				if (test_frame_number == 1)
					final_value = float (value_int) + m_tab_0_25_frame_2[row_position%3][column_position%3];
				else // test_frame_number == 2
					final_value = float (value_int) + m_tab_0_25_frame_3[row_position%3][column_position%3];
		}
		else
			if (difference == 0.75) 
			{ // special dithering with tab_0.75
				test_frame_number = frame%3;
				if (test_frame_number == 0) 
					final_value = float (value_int) + m_tab_0_75_frame_1[row_position%3][column_position%3];
				else
					if (test_frame_number == 1)
						final_value = float (value_int) + m_tab_0_75_frame_2[row_position%3][column_position%3];
					else // test_frame_number == 2
						final_value = float (value_int) + m_tab_0_75_frame_3[row_position%3][column_position%3];
			}
			else
				final_value = value_float;

		if (final_value==pow(float(2),float(m_number_of_bits_out)))
			final_value=pow(float(2),float(m_number_of_bits_out))-1;

	return int (final_value);
}

VideoCardModule::~VideoCardModule()
{
}

