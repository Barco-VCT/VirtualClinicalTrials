#include "../ConvertRawToChoBinModule.h"

#include <boost/algorithm/string.hpp> // to use Boost's string manipulation operations
#include <boost/lexical_cast.hpp> // to use Boost's string casting operations
#include <boost/filesystem.hpp>

#include <FileFinder/FileFinder.h>

using namespace std;

ConvertRawToChoBinModule::ConvertRawToChoBinModule()
{
	m_nSlices = 1; // assign default value to avoid breaking existing sims
}

ConvertRawToChoBinModule::~ConvertRawToChoBinModule()
{}

void ConvertRawToChoBinModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameter name to lower cased strings for easier handling.
	string sName = boost::to_lower_copy(string(name));
	
	try
	{
		if(sName == "inputfolder")
		{
			m_inputFolder = value;
		}
		
		else if(sName == "outputfolder")
		{
			m_outputFolder = value;
		}
		
		else if(sName == "numberofrows")
		{
			m_nRows = boost::lexical_cast<unsigned>(value);
		}
		
		else if(sName == "numberofcolumns")
		{
			m_nColumns = boost::lexical_cast<unsigned>(value);
		}

		else if(sName == "numberofslices")
		{
			m_nSlices = boost::lexical_cast<unsigned>(value);
		}
	} catch(boost::bad_lexical_cast e) {
		m_pMevicLogger->logError("ConvertRawToChoBinModule::SetParameter: the parameter provided in \"" + name + "\" cannot be casted to the desired type.");
	}
}

void ConvertRawToChoBinModule::Simulate(vector<DSContainer*>& container_list)
{
	if(!m_inputFolder.empty() && !m_outputFolder.empty() &&
	   m_nColumns && m_nRows && m_nSlices)
	{
		FileFinder ff;
		bool finderState = ff.initSearch(m_inputFolder, ".*\\.raw");
		if(!finderState)
		{
			m_pMevicLogger->logError("ConvertRawToChoBinModule::Simulate: "
				"Unable to initialize file search. Verify that the input "
				"folder exists and that it contains raw files that can be read.");
		}
		
		string foundFileName;
		bool foundState = ff.findFirst(foundFileName);
		size_t bufferSize = *m_nRows * *m_nColumns * *m_nSlices;
		//std::vector<unsigned short> fileData(bufferSize/2,2);
		unsigned short* buffer = new unsigned short[bufferSize];
		//buffer[0] = fileData.at(0);
		//buffer = &(fileData[0]);
		
		while(foundState)
		{
			try
			{
				string filePath = m_inputFolder + '\\' + foundFileName;
				ifstream fileStream(filePath.c_str(),ios::binary);
				fileStream.exceptions(ios::badbit | ios::failbit);
				fileStream.read((char*)buffer,bufferSize*sizeof(*buffer));

				m_pMevicLogger->logMessage("Processing file: " + filePath);
				
				DSContainer* container = new DSContainer();
				// TO REVIEW
				float whitePoint[3]={0,0,0};
				container->CreateComponent(*m_nRows,*m_nColumns,
					0,0,no_unit,16,whitePoint,no_illuminant,no_observer,TYPE_IMAGE_XYZ,*m_nSlices,0);
				int outputComponentNumber = container->GetLastComponentNumber();
				
				float converted[3];
				unsigned int frame_number;
				unsigned int element_number_per_frame;
				for(unsigned i=0;i<bufferSize;++i)
				{
					converted[0] = converted[1] = converted[2] = buffer[i];
					frame_number = i / (*m_nRows * *m_nColumns);
					element_number_per_frame = i % (*m_nRows * *m_nColumns);
					container->SetComponentElement(outputComponentNumber,element_number_per_frame,frame_number,converted);
				}
				
				if(!boost::filesystem::exists(m_outputFolder))
					boost::filesystem::create_directory(m_outputFolder);
				
				string outputFilePath = m_outputFolder + '\\' +
					foundFileName.substr(0,foundFileName.size()-3) + "bin";
				
				container->SaveToFileWithCompression(outputFilePath);
				
				foundState = ff.findNext(foundFileName);

				delete container;
			} catch(exception& e) {
				m_pMevicLogger->logError(e.what());
			}
		}

		delete [] buffer;
	}
	else
	{
		m_pMevicLogger->logError("One of the required parameters was not provided");
	}
}

