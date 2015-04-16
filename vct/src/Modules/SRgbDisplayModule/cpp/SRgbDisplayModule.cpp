#include "../SRgbDisplayModule.h"

#include <iostream>
#include <fstream>
#include <numeric>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <misc/misc.h>

using namespace std;


SRgbDisplayModule::SRgbDisplayModule():
m_gamma((float) 2.2),
m_contrast(750),
m_lumMin((float) 0.0),
m_lumMax((float) 250.0)
{
	m_lumMin = (float) (m_lumMax / m_contrast);
}	

void SRgbDisplayModule::SetParameter( const string & name, const string & value )
{
	string sName = boost::to_lower_copy(name);

	try
	{
		if(sName == "gamma")
		{
			m_gamma = boost::lexical_cast<float>(value);
		}
		else if(sName == "contrast")
		{
			m_contrast = boost::lexical_cast<unsigned int >(value);
		}	
		else if(sName == "lummax")
		{
			m_lumMax = boost::lexical_cast<float>(value);
		}	
		else if(sName == "illum")
		{
			m_illum = value;
		}
		else
			m_pMevicLogger->logWarning(sName + " is not an accepted parameter for this module");
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("SRgbDisplayModule::SetParameter: the parameter provided in \"" + name + "\" cannot be casted to the desired type");
	}
}

void SRgbDisplayModule::Simulate(vector<DSContainer*>& list)
{
	// check parameters
	if (m_contrast == 0)
	{
		m_pMevicLogger->logError("SRgbDisplayModule::Simulate: contrast can not be equal to 0");
	}
	if (m_lumMax == 0)
	{
		m_pMevicLogger->logError("SRgbDisplayModule::Simulate: lumMax can not be equal to 0");
	}

	// calculate m_lumMin in function of m_contrast and m_lumMax
	m_lumMin = m_lumMax / m_contrast;

	// The last component of the last container in the list is retrieved
	DSContainer* container = list.back();

	unsigned inCompNb = container->GetLastComponentNumber();
	unsigned outCompNb= inCompNb+1;

	// The type contained in the retrieved component is checked
	enum ComponentType element_type;
	container->GetComponentElementType(inCompNb,&element_type);
	if(element_type != TYPE_IMAGE_RGB)
	{
		m_pMevicLogger->logError("SRgbDisplayModule::Simulate: the input component is not TYPE_IMAGE_RGB which is the only one supported");
	}

	unsigned long width, height;
	unsigned inputNbFrames, nbBits;
	string unit, obs;
	float whtPt[3], frameRate;

	container->GetComponentNumberOfColumnElementsPerFrame(inCompNb, &width);
	container->GetComponentNumberOfRowElementsPerFrame(inCompNb, &height);
	container->GetComponentNumberOfFrames(inCompNb, &inputNbFrames);
	container->GetComponentUnit(inCompNb, unit);
	container->GetComponentNumberOfBits(inCompNb, &nbBits);
	container->GetComponentObserver(inCompNb, obs);
	container->GetComponentFrameRate(inCompNb, &frameRate);

	// white point
	vector<float> wtPtVect;
	wtPtVect.resize(3);
	wtPtVect = this->sRGB(255.0, 255.0, 255.0);
	whtPt[0] = wtPtVect[0];
	whtPt[1] = wtPtVect[1];
	whtPt[2] = wtPtVect[2];

	// illum
	ILLUMINANT illum;
	if (m_illum == "D50")
	{
		illum = D50;
	}
	else if (m_illum == "D65")
	{
		illum = D65;
	}

	// create new compoenent
	container->CreateComponent(height, width, 
									  0, 0, unit,
									  nbBits, whtPt, m_illum, obs, TYPE_IMAGE_XYZ, 
									  inputNbFrames, 0);
	
	int rgb[3]={0};
	float XYZ[3]={0.0};

	for ( int i = 0; i < (int) height; i++)
	{
		for ( int j = 0; j < (int) width; j++)
		{
			for ( int k = 0; k < (int) inputNbFrames; k++)
			{
				container->GetComponentElement(inCompNb, i*width+j, k, &rgb);
				vector<float> XYZVec;
				XYZVec.resize(3);
				XYZVec = this->sRGB((float) rgb[0], (float) rgb[1], (float) rgb[2]);

				XYZ[0]= m_lumMin * wtPtVect[0] + XYZVec[0] * (m_lumMax-m_lumMin) * wtPtVect[0];
				XYZ[1]= m_lumMin + XYZVec[1] * (m_lumMax-m_lumMin);
				XYZ[2]= m_lumMin * wtPtVect[2] + XYZVec[2] * (m_lumMax-m_lumMin) * wtPtVect[2];
				
				container->SetComponentElement(outCompNb, i*width+j, k, &XYZ);
			}
		}
	}
}


vector<float> SRgbDisplayModule::sRGB(float R, float G, float B)
{
	if (R>255.0 || G>255.0|| B>255.0)
	{
		m_pMevicLogger->logError("SRgbDisplayModule::sRGB: input R or G or B superior to 255");
	}
	
	R = (float) (R/255.0);
	G = (float) (G/255.0);
	B = (float) (B/255.0);
	
	//Inverse sRGB Companding
	if(R > 0.04045)
	{
		R = (float) pow(double ((R + 0.055)/1.055), (double) m_gamma);
	}
	else
	{
		R = (float) (R/12.92);
	}

	if(G > 0.04045)
	{
		G = (float) pow(double ((G + 0.055)/1.055), (double) m_gamma);
	}
	else
	{
		G = (float) (G/12.92);
	}

	if(B > 0.04045)
	{
		B = (float) pow(double ((B + 0.055)/1.055), (double) m_gamma);
	}
	else
	{
		B = (float) (B/12.92);
	}

	vector<float> XYZ;
	XYZ.resize(3);

	if (m_illum == "D50")
	{
		XYZ[0] = (float) ((R * 0.4360747) + (G * 0.3850649) + (B * 0.1430804));
		XYZ[1] = (float) ((R * 0.2225045) + (G * 0.7168786) + (B * 0.0606169));
		XYZ[2] = (float) ((R * 0.0139322) + (G * 0.0971045) + (B * 0.7141733));
	}
	else if (m_illum == "D65")
	{
		XYZ[0] = (float) ((R * 0.4124564) + (G * 0.3575761) + (B * 0.1804375));
		XYZ[1] = (float) ((R * 0.2126729) + (G * 0.7151522) + (B * 0.0721750));
		XYZ[2] = (float) ((R * 0.0193339) + (G * 0.1191920) + (B * 0.9503041));
	}
	else 
	{
		m_pMevicLogger->logError("SRgbDisplayModule::sRGB: the parameter illum: " + m_illum + " is not supported");
	}
	
	return XYZ;     
}

SRgbDisplayModule::~SRgbDisplayModule()
{	

}
