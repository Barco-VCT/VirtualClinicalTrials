#include "../DisplayModule.h"	// corresponding header

using namespace std;

/*
 * Constructor of the class DisplayModule
 */
DisplayModule::DisplayModule()
: m_nbBits(0),
m_frequency(0),
m_1Color_0Monochrome(0),
m_screenWidth(0),
m_screenHeight(0),
m_imageTopColumnPosition(0),
m_imageTopRowPosition(0),
m_pitchPixelColumnDirection(0),
m_pitchPixelRowDirection(0)
{
}	

/** 
 * Method SetParameter. 
 * @param name: const char* the name of the supeXML parameter 
 * @param value: const char* the value of the supeXML parameter 
 * @return void
 */ 
void DisplayModule::SetParameter(const string & name, const string & value)
{
	// Conversion of the parameters to lower cased strings for easier handling.
	string sName = boost::to_lower_copy(string(name));

	int sizeLimit = 20000;
	if(sName == "_1color_0monochrome")
	{
		try
		{
			m_1Color_0Monochrome = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"_1color_0monochrome\" cannot be casted to int.");
		}
	}

	else if(sName == "filenamenativecurvegray")
	{
		m_filenameNativeCurveGray = value; 
	}

	else if(sName == "filenamenativecurvered")
	{
		m_filenameNativeCurveRed = value; 
	}

	else if(sName == "filenamenativecurvegreen")
	{
		m_filenameNativeCurveGreen = value; 
	}

	else if(sName == "filenamenativecurveblue")
	{
		m_filenameNativeCurveBlue = value; 
	}

	else if(sName == "filenamenativecurvecyan")
	{
		m_filenameNativeCurveCyan = value; 
	}

	else if(sName == "filenamenativecurvemagenta")
	{
		m_filenameNativeCurveMagenta = value; 
	}

	else if(sName == "filenamenativecurveyellow")
	{
		m_filenameNativeCurveYellow = value; 
	}

	else if(sName == "nbbits")
	{
		try
		{
			m_nbBits = boost::lexical_cast<int>(value);
			if (m_nbBits < 0 || m_nbBits > 100)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_nbBits value should be between 0 and 100");
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"nbBits\" cannot be casted to int.");
		}
	}	

	else if(sName == "frequency")
	{
		try
		{
			m_frequency = boost::lexical_cast<int>(value);
			if (m_frequency < 0 || m_frequency > 200)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_frequency value should be between 0 and 200");
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"frequency\" cannot be casted to int.");
		}
	}	

	else if(sName == "screenwidth")
	{
		try
		{
			m_screenWidth = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"screenWidth\" cannot be casted to int.");
			if (m_screenWidth < 0 || m_screenWidth > sizeLimit)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_screenWidth value should be between 0 and sizeLimit");
		}
	}

	else if(sName == "screenheight")
	{
		try
		{
			m_screenHeight = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"screenHeight\" cannot be casted to int.");
			if (m_screenHeight < 0 || m_screenHeight > sizeLimit)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_screenHeight value should be between 0 and sizeLimit");
		}
	}

	else if(sName == "imagetopcolumnposition")
	{
		try
		{
			m_imageTopColumnPosition = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"imageTopColumnPosition\" cannot be casted to int.");
			if (m_imageTopColumnPosition < 0 || m_imageTopColumnPosition > sizeLimit-1)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_imageTopColumnPosition value should be between 0 and sizeLimit-1");
		}
	}

	else if(sName == "imagetoprowposition")
	{
		try
		{
			m_imageTopRowPosition = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"imageTopRowPosition\" cannot be casted to int.");
			if (m_imageTopRowPosition < 0 || m_imageTopRowPosition > sizeLimit-1)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_imageTopRowPosition value should be between 0 and sizeLimit-1");
		}
	}

	else if(sName == "pitchpixelcolumndirection")
	{
		try
		{
			m_pitchPixelColumnDirection = boost::lexical_cast<float>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"pitchPixelColumnDirection\" cannot be casted to float.");
			if (m_pitchPixelColumnDirection < 0 || m_pitchPixelColumnDirection > sizeLimit-1)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_pitchPixelColumnDirection value should be between 0 and sizeLimit-1");
		}
	}

	else if(sName == "pitchpixelrowdirection")
	{
		try
		{
			m_pitchPixelRowDirection = boost::lexical_cast<float>(value);
		}
		catch (boost::bad_lexical_cast e)
		{
			m_pMevicLogger->logError("DisplayModule::SetParameter: the parameter provided in \"pitchPixelRowDirection\" cannot be casted to float.");
			if (m_pitchPixelRowDirection < 0 || m_pitchPixelRowDirection > sizeLimit-1)
				m_pMevicLogger->logError("DisplayModule::SetParameter: m_pitchPixelRowDirection value should be between 0 and sizeLimit-1");
		}
	}		

	else
	{
		m_pMevicLogger->logWarning(name + " is not an accepted parameter for this module.");
	}
}

/** 
 * Method Simulate. 
 * In this module, nothing is done in the container!!
 * Main function for the simulation
 * @param list: list of containers to process
 * @return void
 */
void DisplayModule::Simulate(std::vector<DSContainer*>& list)
{
	m_pMevicLogger->logMessage("Display utilities creation");
	m_ud=new UtilitiesDisplay(m_nbBits, m_frequency, m_screenWidth, m_screenHeight, m_imageTopColumnPosition, m_imageTopRowPosition, m_pitchPixelColumnDirection, m_pitchPixelRowDirection, m_1Color_0Monochrome);

	m_pMevicLogger->logMessage("Gray native curve creation");
	if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveGray.c_str(), GRAY))
	{
		m_pMevicLogger->logError("gray native curve loading failed");
	}

	if (m_1Color_0Monochrome==1)
	{
		m_pMevicLogger->logMessage("Color native curve creation");
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveRed.c_str(), RED))
		{
			m_pMevicLogger->logError("red native curve loading failed");
		}
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveGreen.c_str(), GREEN))
		{
			m_pMevicLogger->logError("green native curve loading failed");
		}
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveBlue.c_str(), BLUE))
		{
			m_pMevicLogger->logError("blue native curve loading failed");
		}
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveCyan.c_str(), CYAN))
		{
			m_pMevicLogger->logError("cyan native curve loading failed");
		}
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveMagenta.c_str(), MAGENTA))
		{
			m_pMevicLogger->logError("magenta native curve loading failed");
		}
		if (!m_ud->loadNativeCurve((char*) m_filenameNativeCurveYellow.c_str(), YELLOW))
		{
			m_pMevicLogger->logError("yellow native curve loading failed");
		}
		m_ud->coefficientProcess();
	}

	//utilities addiiton for display
	m_pUtilitiesExchanger->addUtilityObject("UtilitiesDisplay",m_ud);

	m_pMevicLogger->logMessage("done");
}

/*
 * Destructor of the class DisplayModule
 */
DisplayModule::~DisplayModule()
{
}
