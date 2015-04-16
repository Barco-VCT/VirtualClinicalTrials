#include "../DeltaE2000Module.h"

#include <iostream>
#include <fstream>
#include <numeric>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <misc/misc.h>

#include<color/color.h>

using namespace std;


DeltaE2000Module::DeltaE2000Module():
m_refDSCont(NULL),
m_testDSCont(NULL),
m_index(-1)
{

}	

void DeltaE2000Module::SetParameter( const string & name, const string & value )
{
	string sName = boost::to_lower_copy(name);

	try
	{
		/*if(sName == "gamma")
		{
			;//m_gamma = boost::lexical_cast<float>(value);
		}
		else
			m_pMevicLogger->logWarning(sName + " is not an accepted parameter for this module");*/
	}
	catch(boost::bad_lexical_cast e)
	{
		m_pMevicLogger->logError("DeltaE2000Module::SetParameter: the parameter provided in \"" + name + "\" cannot be casted to the desired type");
	}
}

void DeltaE2000Module::Simulate(vector<DSContainer*>& listContainer)
{
	// Check if container vector is long enough
	if( listContainer.size() < 2 )
		m_pMevicLogger->logError( "DeltaE2000Module::Simulate error: list of containers too short" );

	// The two input containers
	m_refDSCont = listContainer[listContainer.size() - 2];
	m_testDSCont = listContainer[listContainer.size() - 1];

	m_index = m_refDSCont->GetLastComponentNumber();

	if( m_index < 0 )
		m_pMevicLogger->logError( "DeltaE2000Module::Simulate error: no component in container" );

	// width
	unsigned long width;
	m_refDSCont->GetComponentNumberOfColumnElementsPerFrame( m_index, &width );
	// Height
	unsigned long height;
	m_refDSCont->GetComponentNumberOfRowElementsPerFrame( m_index, &height );
	// number of frames
	unsigned int inputNbFrames(0);
	m_refDSCont->GetComponentNumberOfFrames( m_index, &inputNbFrames);
	// number of frames
	unsigned int nbBits(0);
	m_refDSCont->GetComponentNumberOfFrames( m_index, &nbBits);

	// Verify that it is the correct type of component
	enum ComponentType type;
	m_refDSCont->GetComponentElementType(m_index, &type);
	if ( type != TYPE_IMAGE_XYZ )
	{
		m_pMevicLogger->logError( "DeltaE2000Module::Simulate error: type should be TYPE_IMAGE_XYZ" );	
	}

	// white point
	float whPtRef[3]={0}, wtPtTest[3]={0};
	m_refDSCont->GetComponentWhitePoint( m_index, whPtRef );
	m_testDSCont->GetComponentWhitePoint( m_index, wtPtTest );


	// Store the jnd result into a new container 
	DSContainer* resDScont = new DSContainer();
	// create new compoenent
	resDScont->CreateComponent(height, width, 
									  0, 0, "JND",
									  nbBits, whPtRef, "D50", "no_observer", TYPE_IMAGE_JND1, 
									  inputNbFrames, 0);

	float XYZRef[3]={0.0};
	float LabRef[3]={0.0};

	float XYZTest[3]={0.0};
	float LabTest[3]={0.0};

	// Calculate DeltaE2000
	for ( int i = 0; i < (int) height; i++)
	{
		for ( int j = 0; j < (int) width; j++)
		{
			for ( int k = 0; k < (int) inputNbFrames; k++)
			{
				m_refDSCont->GetComponentElement(m_index, i*width+j, k, &XYZRef);
				m_testDSCont->GetComponentElement(m_index, i*width+j, k, &XYZTest);
				
				from_XYZ_to_Lab(XYZRef[0], XYZRef[1], XYZRef[2], LabRef[0], LabRef[1], LabRef[2], whPtRef[0], whPtRef[1], whPtRef[2]);
				from_XYZ_to_Lab(XYZTest[0], XYZTest[1], XYZTest[2], LabTest[0], LabTest[1], LabTest[2], wtPtTest[0], wtPtTest[1], wtPtTest[2]);

				float dE = (float) deltaE2000(LabRef[0], LabRef[1], LabRef[2], LabTest[0], LabTest[1], LabTest[2]);
				
				resDScont->SetComponentElement(0, i*width+j, k, &dE);
			}
		}
	}

	// Return the component 
	listContainer.push_back(resDScont); 
}

DeltaE2000Module::~DeltaE2000Module()
{	

}
