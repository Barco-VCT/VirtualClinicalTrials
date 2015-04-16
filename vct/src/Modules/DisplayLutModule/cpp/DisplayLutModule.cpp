#include "../DisplayLutModule.h" // corresponding header

#include <boost/algorithm/string.hpp> // to use Boost's string manipulation operations
#include <boost/lexical_cast.hpp> // to use Boost's string casting operations

#include <utilities/UtilitiesDisplay.h>

using namespace std;

DisplayLutModule::DisplayLutModule()
:
m_inputNumberOfBits(0),
m_outputNumberOfBits(0),
m_inputNumberOfFrames(0),
m_outputNumberOfFrames(0),
m_frameRate(0),
m_inputComponentNumber(0),
m_outputComponentNumber(0),
m_height(0),
m_width(0),
m_minJnd(0),
m_maxJnd(0),
m_precalcLutPath(""),
m_debugCalibratedLutOutput("")
{
	m_whitePoint[0] = 0;
	m_whitePoint[1] = 0;
	m_whitePoint[2] = 0;
}	

void DisplayLutModule::SetParameter(const string & name, const string & value)
{
	// Conversion of the parameter name to lower cased strings for easier handling.
	string sName = boost::to_lower_copy(string(name));
	
	try
	{
		if(sName == "precalculatedlutpath")
		{
			m_precalcLutPath = value;
		}
		else if(sName == "debugcalibratedlutoutput")
		{
			m_debugCalibratedLutOutput = value;
		}
		else if(sName == "debugtransfercurveoutput")
		{
			m_debugTransferCurveOutput = value;
		}
		else
		{
			// The use of streams is recommended in VCT as it is safer than the printf, sprintf, fprintf and similar functions
			stringstream warningMessage;
			warningMessage << name << " is not an accepted parameter for this module.";
			m_pMevicLogger->logWarning(warningMessage.str());
		}
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("DisplayLUTModule::SetParameter: the parameter provided in \"" 
			+ name 
			+ "\" cannot be casted to the appropriate type.");
	}
}

void DisplayLutModule::Simulate( vector < DSContainer*>& list)
{
	DSContainer* container = list.back();

	m_inputComponentNumber = container->GetLastComponentNumber();
	m_outputComponentNumber = m_inputComponentNumber+1;
	if (m_inputComponentNumber < 0)
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, m_inputComponentNumber value not valid /!\\");

	///////////////////////////////////////////////////////////////////////////////////
	// consistency check 
	///////////////////////////////////////////////////////////////////////////////////
	enum ComponentType eType;
	container->GetComponentElementType(m_inputComponentNumber, &eType);

	//color check
	if(eType!= TYPE_IMAGE_RGB)
	{
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, module not ready for entry ! =  TYPE_IMAGE_RGB /!\\");
	}
	int nbChannel  = container->GetNbChannel(eType);

	try
	{
		// get utilitiesDisplay
		m_ud = dynamic_cast < UtilitiesDisplay*>(m_pUtilitiesExchanger->requestUtilityObject("UtilitiesDisplay"));
	}
	catch(...)
	{
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, during Utilities display request /!\\");
	}
	if (m_ud == 0)
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, during Utilities display request /!\\");

	//check bits consistency
	container->GetComponentNumberOfBits(m_inputComponentNumber, &m_outputNumberOfBits);
	if(m_outputNumberOfBits != m_ud->getNbBits())
	{
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, display and component have not the same number of bits /!\\");
	}

	// test to know if last component is valid or not
	if (container->GetComponentValid(m_inputComponentNumber) == -1) 
	{
		m_pMevicLogger->logError("Component non valid in DisplayLutModule:Simulate");
	}

	// extract or play with the last component, --> extract number of last component.
	m_inputComponentNumber = container->GetLastComponentNumber();
	m_outputComponentNumber = m_inputComponentNumber+1;
	if (m_outputComponentNumber != m_inputComponentNumber+1 || m_outputComponentNumber < 0)
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, m_outputComponentNumber value not valid /!\\");
	
	// m_width and m_height for the output component
	container->GetComponentNumberOfColumnElementsPerFrame(m_inputComponentNumber,&m_width);
	container->GetComponentNumberOfRowElementsPerFrame(m_inputComponentNumber, &m_height);
	container->GetComponentNumberOfFrames(m_inputComponentNumber, &m_inputNumberOfFrames);
	
	m_outputNumberOfFrames = m_inputNumberOfFrames;
	if (m_outputNumberOfFrames < 0)
		m_pMevicLogger->logError("error in DisplayLutModule::Simulate, m_outputNumberOfFrames value not valid /!\\");

	// create new component to save output
	container->GetComponentUnit(m_inputComponentNumber, m_unit);
	container->GetComponentIlluminant(m_inputComponentNumber, m_illum);
	container->GetComponentNumberOfBits(m_inputComponentNumber, &m_inputNumberOfBits);
	container->GetComponentObserver(m_inputComponentNumber, m_obs);
	container->GetComponentWhitePoint(m_inputComponentNumber, m_whitePoint);

	container->CreateComponent(m_height, m_width, 0, 0, 
		m_unit,
		m_outputNumberOfBits,
		m_whitePoint,
		m_illum,
		m_obs,
		TYPE_IMAGE_RGB,
		m_outputNumberOfFrames,
		m_frameRate);

	///////////////////////////////////////////////////////////////////////////////////
	// Simulation
	///////////////////////////////////////////////////////////////////////////////////
	// set m_whitePoint[1] = Y of the component with Lmax:
	// m_whitePoint[0] = X and m_whitePoint[2] = Z should defined in the panel characterization of the display simulation
	m_whitePoint[1] = m_ud->getNativeCurve(GRAY)[m_ud->getCurveNbData()-1][1];
	container->SetComponentWhitePoint(m_outputComponentNumber, m_whitePoint);

	// dynamic memory allocation of vector
	m_jndNativeCurve.resize(m_ud->getCurveNbData());
	m_jndTargetCurve.resize(m_ud->getCurveNbData());

	//Convert native_curve in JND
	for (m_i = 0; m_i < unsigned(m_ud->getCurveNbData()); m_i++) 
		m_jndNativeCurve[m_i] = m_ud->lum2JND(m_ud->getNativeCurve(GRAY)[m_i][1]);

	//Generate target_curve in JND
	// black level videolevel = 0
	m_minJnd = m_jndTargetCurve[0] = m_jndNativeCurve[0];
	
	// white level video level = max
	m_maxJnd = m_jndTargetCurve[m_ud->getCurveNbData()-1] = m_jndNativeCurve[m_ud->getCurveNbData()-1];

	// convert string in char* 1024 characters for component description in container
	string destPart1, destPart2, dest;
	try
	{
		string destPart1 = boost::lexical_cast<string>(m_minJnd);
	}
	catch (boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("DisplayLutModule::Simulate: the parameter provided in \"destPart1\" cannot be casted to string.");
	}

	try
	{
		string destPart2 = boost::lexical_cast<string>(m_maxJnd);
	}
	catch (boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("DisplayLutModule::Simulate: the parameter provided in \"destPart2\" cannot be casted to string.");
	}
	dest = "m_mindJnd = " + destPart1 + "m_maxdJnd = " + destPart2;
	container->SetComponentDescription(m_outputComponentNumber,(char*) dest.substr(0,1024).c_str());


	//Generate target_curve in JND
	for (m_i = 1; m_i < unsigned(m_ud->getCurveNbData()-1); m_i++) 
	{
		m_jndTargetCurve[m_i] =  m_minJnd+ m_i*((m_maxJnd-m_minJnd)/(m_ud->getCurveNbData()-1));
	}

	//minimum distance target->native searching
	int new_i;
	//dynamic allocation of the LUT m_lut
	m_lut.resize(m_ud->getCurveNbData());
	if(m_precalcLutPath == "")
	{
		for (m_i = 0; m_i < pow((float)2,(float)m_outputNumberOfBits); m_i++) 
		{
			new_i = ReturnDdlValue((float)m_jndTargetCurve[m_i]);
			m_lut[m_i] = new_i;
		}
	}
	else
	{
		ifstream lutFile(m_precalcLutPath.c_str());
		if(!lutFile.good())
			m_pMevicLogger->logError("DisplayLutModule::Simulate: The file provided in parameter"
			"\"PrecalculatedLutPath\" couldn't be read.");
		for(m_i = 0; m_i < pow((float)2,(float)m_outputNumberOfBits); m_i++)
		{
			if(lutFile.eof())
				m_pMevicLogger->logError("DisplayLutModule::Simulate: The file provided in parameter"
				"\"PrecalculatedLutPath\" doesn't contain enough values to fill the lut.");
			lutFile >> m_lut[m_i];
			if(lutFile.fail())
				m_pMevicLogger->logError("DisplayLutModule::Simulate: One of the elements file provided in parameter"
				"\"PrecalculatedLutPath\" could not be interpreted as a LUT value.");
		}
	}

	if(m_debugCalibratedLutOutput != "")
	{
		ofstream debugLutStream(m_debugCalibratedLutOutput.c_str());
		if(!debugLutStream.bad())
		{
			for(int ddl=0; ddl<pow((float)2,(float)m_outputNumberOfBits); ++ddl)
			{
				debugLutStream << ddl << '\t' << m_lut[ddl] << endl;
			}
		}
		else
		{
			m_pMevicLogger->logWarning("DisplayLutModule::Simulate: the output file for the lut could not be opened.");
		}
		debugLutStream.close();
	}
	if(m_debugTransferCurveOutput != "")
	{
		ofstream debutTCurveStream(m_debugTransferCurveOutput.c_str());
		if(!debutTCurveStream.bad())
		{
			for(int ddl = 0; ddl<pow((float)2,(float)m_outputNumberOfBits); ++ddl)
			{
				debutTCurveStream << ddl << '\t' << m_ud->getNativeCurve(GRAY)[m_lut[ddl]][1] << endl;
			}
		}
		else
		{
			m_pMevicLogger->logWarning("DisplayLutModule::Simulate: the output file for the transfer curve could not be opened.");
		}
		debutTCurveStream.close();
	}

	int temp;
	int *rgb =  new int[nbChannel];
	// Set the new component with modified target curve & m_lut
	for (m_i = 0; m_i < int (m_height); m_i++) {
		for (m_j = 0; m_j < int (m_width); m_j++) {
			for (m_k = 0; m_k < int (m_outputNumberOfFrames);m_k++) {
				container->GetComponentElement(m_inputComponentNumber,m_i*m_width+m_j,m_k,rgb);
				for(int c = 0; c < nbChannel; c++)
					{
					//////////////////////////////////////////
					temp = rgb[c];
					//////////////////////////////////////////
					rgb[c] = m_lut[temp];
					}
				container->SetComponentElement(m_outputComponentNumber,m_i*m_width+m_j,m_k,rgb);
			}
		}
	}
	delete [] rgb;
}

int DisplayLutModule::ReturnDdlValue(float targetVal) 
{
	// binary process to find the least difference
	int low, mid, high, infLevel, supLevel, max;
	float midFloat, reachedVal, diff, diffInf, diffSup, epsilon;

	low = mid = infLevel = supLevel = 0;
	high = max = m_ud->getCurveNbData()-1;
	epsilon = 0.0001f;
	midFloat = reachedVal = diff = diffInf = diffSup = 0.0;

	// find mid ddl, inf ddl = mid-1 & sup ddl = mid+1
	while (low != high || high > low)
	{
		midFloat= (low + high) *0.5f;
		mid = (int)midFloat;
		reachedVal = (float)m_jndNativeCurve[mid];

		if (low == mid || high == mid) break;
		
		if (reachedVal < targetVal)
			low = mid;
		else
			high = mid;
	}

	// check if mid == min or == max to avoid mid=-1 or mid > max for the vector m_jndNativeCurve
	if (mid == 0) infLevel = 0; else infLevel = 1;
	if (mid == (m_ud->getCurveNbData()-1)) supLevel = 0; else supLevel = 1;

	// find correct dll level
	// compute 3 differences & find the minimum diff and define the mid for 3 choices: inf ddl, mid ddl or max ddl
	diff = fabs((float)m_jndNativeCurve[mid] - targetVal);
	diffSup = fabs((float)m_jndNativeCurve[mid+supLevel] - targetVal);
	diffInf = fabs(targetVal - (float)m_jndNativeCurve[mid-infLevel]);

	if (diffSup < diffInf && diffSup < diff)
	{
		mid = mid + supLevel; // mid = sup ddl
		// if debug
		infLevel = 1; supLevel = 0; 
	}
	else 
		if (diffInf < diffSup  && diffInf < diff)
		{
			mid = mid-infLevel; // mid = inf ddl
			// if debug
			infLevel = 0; supLevel = 1; 
		}
		else 
			if (diff < diffInf && diff < diffSup)
			{
				mid = mid; // mid = mid ddl
				// if debug
				infLevel = 1; supLevel = 1; 
			}

	// check if mid == min or == max to avoid mid=-1 or mid > max for the vector m_jndNativeCurve
	// mid = sup ddl
	if (mid == 0) infLevel = 0;
	if (mid == max) supLevel = 0;

	return mid;
}

DisplayLutModule::~DisplayLutModule()
{	
}


