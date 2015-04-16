#include "../Rgb2XYZDisplayModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

Rgb2XYZDisplayModule::Rgb2XYZDisplayModule()
: m_ud(0)
{
}	

void Rgb2XYZDisplayModule::SetParameter(const string & name, const string & value)
{
	// This function not only sets but also "translates" and checks the provided parameters.
	// These parameters should have the appropriate syntax and logic.
	// Clear error or warning messages should be displayed in case of incorrect parameters.
	// The different cases where a warning or an error should be used depends on whether or not the simulation can continue with an incorrect parameter.
	// What is an incorrect parameter is at the developer's discretion.

	// Conversion of the parameters to lower cased strings for easier handling.
	string sName  = boost::to_lower_copy(name);

	m_pMevicLogger->logWarning("Rgb2XYZDisplayModule::SetParameter: no input parameter");

}

void Rgb2XYZDisplayModule::Simulate( vector<DSContainer*>& list)
{
	DSContainer* container = list.back();

	string unit;
	string illum;
	string obs;

	float whPt[3];
	unsigned int nbBits;
	unsigned int inNbFrames,
	             outNbFrames;
	float frameRate;
	unsigned long width,
	              height;
	int inCompNb,
	    outCompNb;
	
	// extract or play with the last component, --> extract number of last component.
	inCompNb=container->GetLastComponentNumber();
	outCompNb=inCompNb+1;


	// test to know if last component is valid or not
	if (container->GetComponentValid(inCompNb)==-1) 
	{
		m_pMevicLogger->logError("Rgb2XYZDisplayModule::Simulate: Component non valid");
	}
	
	enum ComponentType eType;

	container->GetComponentElementType(inCompNb, &eType);

	int nbChannel =container->GetNbChannel(eType);

	if(eType!=TYPE_IMAGE_RGB)
	{
		m_pMevicLogger->logError("Rgb2XYZDisplayModule::Simulate: Module input must have type: TYPE_IMAGE_RGB");
	}

	try
	{
		//get utilitiesDisplay
		m_ud=dynamic_cast<UtilitiesDisplay*>(m_pUtilitiesExchanger->requestUtilityObject("UtilitiesDisplay"));
	}
	catch(...)
	{
		m_pMevicLogger->logError("Rgb2XYZDisplayModule::Simulate: Utilities display request failed");
	}
	if (m_ud==0)
		m_pMevicLogger->logError("Rgb2XYZDisplayModule::Simulate: Utilities display request failed");

	// width and height for the output component
	container->GetComponentNumberOfColumnElementsPerFrame(inCompNb,&width);
	container->GetComponentNumberOfRowElementsPerFrame(inCompNb, &height);
	container->GetComponentNumberOfFrames(inCompNb, &inNbFrames);
	
	outNbFrames=inNbFrames;

	// create new component to save output of ExampleModule
	container->GetComponentUnit(inCompNb, unit);
	container->GetComponentIlluminant(inCompNb, illum);
	container->GetComponentNumberOfBits(inCompNb, &nbBits);
	container->GetComponentObserver(inCompNb, obs);
	container->GetComponentWhitePoint(inCompNb, whPt);
	container->GetComponentFrameRate(inCompNb, &frameRate);

	container->CreateComponent(height, width, 0, 0, 
		unit,
		nbBits,
		whPt,
		illum,
		obs,
		TYPE_IMAGE_XYZ,
		outNbFrames,
		frameRate);

	unsigned int RGB[3]={0};
	float XYZ[3]={0};

	for (int k=0; k<(int)outNbFrames; k++)
	{
		for (int i=0; i<(int)height; i++)
		{
			for (int j=0; j<(int)width; j++)
			{
				container->GetComponentElement(inCompNb,i*width+j,k,RGB);
				vector<float> vXYZ=m_ud->fromRgbToXyz(RGB[0],RGB[1],RGB[2]);	
				std::copy(vXYZ.begin(),vXYZ.end(),XYZ);

				container->SetComponentElement(outCompNb,i*width+j,k,XYZ);
			}
		}
	}
}

Rgb2XYZDisplayModule::~Rgb2XYZDisplayModule()
{	
}
