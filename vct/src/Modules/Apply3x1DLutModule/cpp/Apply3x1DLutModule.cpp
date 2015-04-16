#include "../Apply3x1DLutModule.h"

#include <iostream>
#include <fstream>
#include <numeric>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <misc/misc.h>

using namespace std;

Apply3x1DLutModule::Apply3x1DLutModule():
m_lutR(NULL),
m_lutG(NULL),
m_lutB(NULL),
m_nbBits(0),
m_pathR(""),
m_pathG(""),
m_pathB("")
{

}	

void Apply3x1DLutModule::SetParameter( const string & name, const string & value )
{
	string sName = boost::to_lower_copy(name);

	try
	{
		if(sName == "filenamepathlutred")
		{
			m_pathR = value;
		}
		if(sName == "filenamepathlutgreen")
		{
			m_pathG = value;
		}
		if(sName == "filenamepathlutblue")
		{
			m_pathB = value;
		}
		if(sName == "nbbits")
		{
			m_nbBits = boost::lexical_cast<unsigned int>(value);
		}
		else
			m_pMevicLogger->logWarning("Apply3x1DLutModule::SetParameter: " + sName + " is not an accepted parameter for this module");
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::SetParameter: the parameter provided in \"" + name + "\" cannot be casted to the desired type");
	}
}

void Apply3x1DLutModule::Simulate(vector<DSContainer*>& list)
{
	// check parameters
	if (m_nbBits == 0)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: the number of bit should be different to 0");
	}

	// calculate number of elements for 1 Lut
	m_nbElemLut = (unsigned) pow(2.0, (double) (m_nbBits));
	m_max = m_nbElemLut - 1;

	// allocate and load the R, G, B 1D Luts
	m_lutR = new LookUpTable(m_pathR.c_str(), true, m_nbElemLut);
	m_lutG = new LookUpTable(m_pathG.c_str(), true, m_nbElemLut);
	m_lutB = new LookUpTable(m_pathB.c_str(), true, m_nbElemLut);

	// check LUT content and max = 1
	float maxR = this->getLutMax(m_lutR);
	float maxG = this->getLutMax(m_lutG);
	float maxB = this->getLutMax(m_lutB);
	if (maxR > 1)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: max of LUT R should not be superior to 1");
	}
	if (maxG > 1)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: max of LUT G should not be superior to 1");
	}
	if (maxB > 1)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: max of LUT B should not be superior to 1");
	}
	
	// check number of elements consistency between the xml parameter and the data contained in the file
	unsigned avgSize = (unsigned) ( ((double) m_lutR->getSize() + (double) m_lutR->getSize() + (double) m_lutR->getSize()) / 3.0 );
	if (m_nbElemLut != avgSize)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: data LUT sizes within the input file does not match input number of bits");
	}

	// The last component of the last container in the list is retrieved
	DSContainer* container = list.back();

	unsigned inCompNb = container->GetLastComponentNumber();
	unsigned outCompNb= inCompNb+1;

	// The type contained in the retrieved component is checked
	enum ComponentType element_type;
	container->GetComponentElementType(inCompNb,&element_type);
	if(element_type != TYPE_IMAGE_RGB)
	{
		m_pMevicLogger->logError("Apply3x1DLutModule::Simulate: the input component is not TYPE_IMAGE_RGB which is the only one supported");
	}

	unsigned long width, height;
	unsigned inputNbFrames, nbBits;
	string unit, obs,illum;
	float whtPt[3], frameRate;

	container->GetComponentNumberOfColumnElementsPerFrame(inCompNb, &width);
	container->GetComponentNumberOfRowElementsPerFrame(inCompNb, &height);
	container->GetComponentNumberOfFrames(inCompNb, &inputNbFrames);
	container->GetComponentUnit(inCompNb, unit);
	container->GetComponentIlluminant(inCompNb, illum);
	container->GetComponentNumberOfBits(inCompNb, &nbBits);
	container->GetComponentObserver(inCompNb, obs);
	container->GetComponentWhitePoint(inCompNb, whtPt);
	container->GetComponentFrameRate(inCompNb, &frameRate);

	// create new component
	container->CreateComponent(height, width, 
									  0, 0, unit,
									  nbBits, whtPt, illum, obs, TYPE_IMAGE_RGB, 
									  inputNbFrames, 0);

	int rgbIn[3]={0};
	int rgbOut[3]={0};

	// apply 3x 1D LUT to input
	for ( int i = 0; i < (int) height; i++)
	{
		for ( int j = 0; j < (int) width; j++)
		{
			for ( int k = 0; k < (int) inputNbFrames; k++)
			{
				container->GetComponentElement(inCompNb, i*width+j, k, &rgbIn);
				rgbOut[0] = (int) ((m_lutR->at(rgbIn[0])) * (float) (m_max) + 0.5);
				rgbOut[1] = (int) ((m_lutG->at(rgbIn[1])) * (float) (m_max) + 0.5);
				rgbOut[2] = (int) ((m_lutB->at(rgbIn[2])) * (float) (m_max) + 0.5);
				container->SetComponentElement(outCompNb, i*width+j, k, &rgbOut);
			}
		}
	}
}

float Apply3x1DLutModule::getLutMax(LookUpTable *lut)
{
	float maxVal(-10000);
	float val(0); 

	for (int i = 0; i < (int) lut->getSize(); i++)
	{
		float val = m_lutR->at(i);
		if (val > maxVal)
		{
			maxVal = val;
		}
	}

	return maxVal;
}

Apply3x1DLutModule::~Apply3x1DLutModule()
{	
	if (m_lutR != NULL)
	{
		delete m_lutR;
		m_lutR = NULL;
	}
	if (m_lutG != NULL)
	{
		delete m_lutG;
		m_lutG = NULL;
	}
	if (m_lutB != NULL)
	{
		delete m_lutB;
		m_lutB = NULL;
	}
}
