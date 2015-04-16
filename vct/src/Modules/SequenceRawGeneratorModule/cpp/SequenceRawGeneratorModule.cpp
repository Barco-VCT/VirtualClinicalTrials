#include "../SequenceRawGeneratorModule.h"

#include <fstream>
#include <cmath>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <FileFinder/FileFinder.h>

using namespace std;

SequenceRawGeneratorModule::SequenceRawGeneratorModule()
:m_directory(""),
 m_number_of_slices(0),
 m_number_of_frames(0),
 m_width(-1),
 m_height(-1),
 m_nbChannels(-1),
 m_nbBitsRange(-1),
 m_littleEndian(-1),
 m_crop(-1),
 m_widthCrop(-1),
 m_heightCrop(-1),
 m_ptOriTopLeftX(-1),
 m_ptOriTopLeftY(-1),
 m_normalization(-1),
 m_whiteIs0(-1),
 m_frame_repeat(-1),
 m_1RGB_0GRAY(-1),
 m_outputXYZ(-1),
 m_nbBitsPrecision(-1),
 m_nbBitsOutput(-1),
 m_start(-1),
 m_end(-1),
 m_nbBytesToRead(-1)
{
}

void SequenceRawGeneratorModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameters to lower cased strings for easier handling.
	string sName  = boost::to_lower_copy(name);

	if(sName == "directory")
	{
		m_directory = value;
	}
	else if(sName == "number_of_slices")
	{
		try
		{
			m_number_of_slices = boost::lexical_cast<unsigned int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"number_of_slices\" cannot be casted to unsigned int");
		}
	}
	else if(sName == "width")
	{
		try
		{
			m_width = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"width\" cannot be casted to int");
		}
	}
	else if(sName == "height")
	{
		try
		{
			m_height = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"height\" cannot be casted to int");
		}
	}
	else if(sName == "nbbitsrange")
	{
		try
		{
			m_nbBitsRange = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_nbBitsRange\" cannot be casted to int");
		}
	}
	else if(sName == "_0bigendian_1littleendian")
	{
		try
		{
			m_littleEndian = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"_0bigEndian_1littleEndian\" cannot be casted to int");
		}
	}
	else if(sName == "nbbitsoutput")
	{
		try
		{
			m_nbBitsOutput = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_nbBitsOutput\" cannot be casted to int");
		}
	}
	else if(sName == "nbbitsprecision")
	{
		try
		{
			m_nbBitsPrecision = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_nbBitsPrecision\" cannot be casted to int");
		}
	}
	else if(sName == "frame_repeat")
	{
		try
		{
			// allowing fractional frame repeat
			m_frame_repeat = boost::lexical_cast<float>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_frame_repeat\" cannot be casted to float");
		}
	}
	else if(sName == "_1whiteis0_0otherwise")
	{
		try
		{
			m_whiteIs0 = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"_1WhiteIs0_0Otherwise\" cannot be casted to int");
		}
	}
	else if(sName == "_1rgb_0gray")
	{
		try
		{
			m_1RGB_0GRAY = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_1RGB_0GRAY\" cannot be casted to int");
		}
	}
	else if(sName == "output_xyz")
	{
		try
		{
			m_outputXYZ = boost::lexical_cast<int>(value);
		}
		catch(boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: the parameter provided in \"m_output_XYZ\" cannot be casted to int");
		}
	}
	else m_pMevicLogger->logError("SequenceRawGeneratorModule::SetParameter: incorrect parameter");
}

template<typename T>
T getRawValue(ifstream & rawFile, int littleEndian)
{
	T value;
	rawFile.read((char *)&value, sizeof(value));

	if(1 != littleEndian)
	{
		T swapValue = value;
		for(int i = 0; i < sizeof(value); i++)
		{
			((char *)(&value))[i] = ((char *)(&swapValue))[sizeof(value)- 1 - i ];
		}
	}

	// Return
	return value;
}

void SequenceRawGeneratorModule::Simulate(std:: vector<DSContainer*>& list)
{
	DSContainer *container = list.back();
	int maxInput = (int)((pow((float) 2, m_nbBitsRange))-(float)1.0);
	float factor=(float)1.0/(float)maxInput*((pow((float) 2, m_nbBitsOutput))-(float)1.0);//bits output

	int nbChannel=0;

	// do i need to round the mult result below?
	// It is floored now.
	m_number_of_frames=unsigned int(m_frame_repeat*m_number_of_slices);


	int current_frame_number;

	//consistency checks
	if (m_frame_repeat < 1) 
	{
		m_pMevicLogger->logError("Value of m_frame_repeat cannot be less than 1");
	}

	if (m_whiteIs0 != 0 && m_whiteIs0 != 1) 
	{
		m_pMevicLogger->logError("Value of m_whiteIs0 has to be either 0 == white is 0 or 1 == white is not 0");
	}

	if (m_nbBitsRange != 8 && m_nbBitsRange != 10)
	{
		m_pMevicLogger->logWarning("Value of m_nbBitsRange has to be either 8 or 10 bits");
	}
		
	if (m_nbBitsPrecision !=8 && m_nbBitsPrecision != 16 && m_nbBitsPrecision != 24 && m_nbBitsPrecision != 32 &&  m_nbBitsPrecision != 64) 
	{
		m_pMevicLogger->logError("Value of m_nbBitsPrecision has to be 8, 16, 24, 32 or 64 bits");
	}

	if (m_nbBitsRange == 10 && m_nbBitsPrecision==8)
	{
		m_pMevicLogger->logError("if m_nbBitsRange=10, input can not be 8 or RGB 24 bits");
	}

	if (m_nbBitsRange == 10 && m_nbBitsPrecision==24)
	{
		m_pMevicLogger->logError("if m_nbBitsRange=10, input can not be 8 or RGB 24 bits");
	}

	if (container->GetNumberOfComponents() > 0) 
	{
		m_pMevicLogger->logError("The container has to be empty !");
	}
	
	float white[3]={0};

	if (m_1RGB_0GRAY==0)
	{
		nbChannel=1;
		container->CreateComponent(m_height, m_width, 
									  0, 0, no_unit,
									  m_nbBitsOutput, white, no_illuminant, no_observer, TYPE_IMAGE_GRAY,
									  m_number_of_frames, 0);
	}
	else if (m_1RGB_0GRAY==1)
	{
		nbChannel=3;
		container->CreateComponent(m_height, m_width, 
									  0, 0, no_unit,
									  m_nbBitsOutput, white, no_illuminant, no_observer, TYPE_IMAGE_RGB, 
									  m_number_of_frames, 0);
	}
	else if (m_outputXYZ==1)
	{
		nbChannel=3;
		container->CreateComponent(m_height, m_width, 
									  0, 0, no_unit,
									  m_nbBitsOutput, white, no_illuminant, no_observer, TYPE_IMAGE_XYZ, 
									  m_number_of_frames, 0);
	}
	else
	{
		m_pMevicLogger->logError("m_1RGB_0GRAY has to be 0 or 1: error in SequenceRawGeneatorModule:Simulate");
	}

	unsigned int* value= new unsigned int[nbChannel];

	///////////////////////////////////////////////////////////////////

	FileFinder fileFinder; // FileFinder object, providing directory browsing functionality
	std::string pathToDir = m_directory, // Target directory
	            fileName,               // Found file name
	            pathToFile;             // Found file full path name

	bool createState, // State of the initialization (create parameter); true if OK (done and successfully initialized), false if not OK (not yet done or error)
	     foundState;  // State of the files browsing; true if OK (file found), false if not OK (no more files found)

	ifstream rawFile; // Found file

	// Target directory
	while(pathToDir.find_last_of('\\') != std::string::npos) // Replace backslashes by forward-slashes if any
	{
		pathToDir = pathToDir.replace(pathToDir.find_last_of('\\'), 1, "/");
	}
	pathToDir += "/"; // Append path delimiter
	
	// Initialize search; Target search criterium (wildcard): all files matching *.raw
	createState = fileFinder.initSearch(pathToDir, ".*\\.raw");
	if (!createState)
	{
		m_pMevicLogger->logError("Unable to initialize search");
	}

	// Find first file
	foundState = fileFinder.findFirst(fileName);
	if (!foundState)
	{	
		m_pMevicLogger->logError("Invalid file (file not found)");
	}

	m_pMevicLogger->logMessage("...sequence generation");

	// Slices
	current_frame_number=0;
	double resErr = 0.0; // residual error is
	// the fraction of FR (frame repeat) that is not rendered as each iteration
	// can only add one (integer) frame. resErr is kept to be considered in the next
	// slice.
	for(int current_slice_number=0; current_slice_number<(signed int)m_number_of_slices; current_slice_number++) //for each slice
	{							
		m_pMevicLogger->logProgress(".");
		
		if (current_slice_number!=0) //if we didn't work on the 1st slice
		{
			// Find next file
			foundState = fileFinder.findNext(fileName);
			if (!foundState)
			{	
				m_pMevicLogger->logError("Invalid file (file not found)");
			}
		}

		// Building of the complete path of the fileName
		pathToFile = pathToDir + fileName;

		// Opening of the file

		rawFile.open(pathToFile.c_str(), ios::binary);
		if (!rawFile.good())
		{
			m_pMevicLogger->logError("File can not be opened: " + pathToFile);
		}

		// Frames
		// We save nbFramePerSlice time in the container
		int pos=current_frame_number;
		// consider resErr
		while(current_frame_number< floor(pos+m_frame_repeat+resErr) ) 
		{
			rawFile.seekg(0);
			// Lines
			for (int i = 0; i < m_height; i ++) 
			{
				// Pixels
				for (int j = 0; j < m_width; j ++) 
				{ 
					// Gray-scale image
					if (m_1RGB_0GRAY==0)
					{
						// 32-bit
						if (m_nbBitsPrecision==32)
						{
							//to get value either littleEndian or bigEndian
							float res=getRawValue<float>(rawFile, m_littleEndian);

							//interpolation and rounding to int
							value[0]=(unsigned int)(int(res+0.5)*factor);
							//if (m_nbBitsRange==8 && res==255.0)value[0]=1023;//m_end interpolation management

							if (m_whiteIs0 == 1)
								value[0]=maxInput-value[0];

							container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, &value[0]);
						}
						// 64-bit
						else if (m_nbBitsPrecision==64)
						{
							//to get value either littleEndian or bigEndian
							double res=getRawValue<double>(rawFile, m_littleEndian);

							//interpolation and rounding to int
							value[0]=(unsigned int)(int(res+0.5)*factor);

							if (m_whiteIs0 == 1)
								value[0]=maxInput-value[0];

							container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, &value[0]);
						}
						// 16-bit
						else if (m_nbBitsPrecision==16)
						{
							//to get value either littleEndian or bigEndian
							int res=getRawValue<unsigned short>(rawFile, m_littleEndian);

							//interpolation and rounding to int
							value[0]=int(res*factor+0.5);

							if (m_whiteIs0 == 1)
								value[0]=maxInput-value[0];

							container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, &value[0]);
						}
						else
						{
							//to get value either littleEndian or bigEndian
							int res=getRawValue<unsigned char>(rawFile, m_littleEndian);

							//interpolation and rounding to int
							value[0]=int(res*factor+0.5);

							if (m_whiteIs0 == 1)
								value[0]=maxInput-value[0];

							container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, &value[0]);
						}
					}
					// RGB 24-bit
					else if (m_1RGB_0GRAY==1)
					{
						int res;
						// Channels
						for (int c=0; c<nbChannel; c++)
						{
							//to get value either littleEndian or bigEndian
							res=getRawValue<unsigned char>(rawFile, m_littleEndian);

							//interpolation and rounding to int
							value[c]=int(res*factor+0.5);

							if (m_whiteIs0 == 1)
								value[c]=maxInput-value[c];
						}

						container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, value);
					} // Gray-scale or RGB
					// XYZ VCT FORMAT
					else if (m_outputXYZ==1)
					{
						if (m_nbBitsPrecision==32)
						{
							//to get value either littleEndian or bigEndian
							float res=getRawValue<float>(rawFile, m_littleEndian);
							float XYZ[3];
							XYZ[0]=XYZ[1]=XYZ[2]=res;

							container->SetComponentElement(container->GetLastComponentNumber(), i*(m_width)+j, current_frame_number, &XYZ);
						}

						
					} // XYZ VCT FORMAT
				} // Pixels
			} // Lines
			current_frame_number++;
		} // Frames
		rawFile.close();
		// update resErr
		resErr = pos+m_frame_repeat+resErr - floor(pos+m_frame_repeat+resErr);
	} // Slices

	m_pMevicLogger->logProgress(" done");

	delete [] value;
}

SequenceRawGeneratorModule::~SequenceRawGeneratorModule()
{	
}
