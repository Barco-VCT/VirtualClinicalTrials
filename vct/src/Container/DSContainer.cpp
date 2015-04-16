#include "DSContainer.h"

#include <fstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include <zlib/zlib.h>
#include <i_o/bmp/EasyBMP.h>

#include <ErrorManagement/ErrorLogger.h>
#include "TypeTraits.h"
#include <map>
#ifdef USE_OPENCL
#include "OpenCLManager.h"
#endif

using namespace std;
 
ComponentHeader::ComponentHeader()
: m_baseAddress(0)
, m_allocType(default_allocator)
, m_length(0)
, m_nbElemPerFrame(0)
, m_elemType(TYPE_BYTE)
, m_validComp(0)
, m_frameRate(0)
, m_nbFrames(0)
, m_nbRowElemPerFrame(0)
, m_nbColumnElemPerFrame(0)
, m_nbBits(0)
, m_elemSizeCol(0)	// in mm
, m_elemSizeRow(0)		// in mm
, m_unit(no_unit)
, m_illum(no_illuminant)
, m_obs(no_observer)
, m_elemSize(0)
{
	memset(m_whitePt,0,sizeof(m_whitePt));

	memset(m_description,0,DESCRIPTION_SIZE);
}

ComponentHeader::~ComponentHeader() 
{
}

DSContainer::DSContainer() 
: m_description("")
{	
	for (int index=0; index<MAX_NUMBER_COMPONENTS; index++)
		ComponentHeaderList[index].m_baseAddress=0;
}

DSContainer::~DSContainer() 
{
	for (int index=0; index<MAX_NUMBER_COMPONENTS; index++) 
	{
		DeleteComponentData(index);
	}
}

int DSContainer::GetNumberOfComponents() 
{
	int count=0;
	for (int temp=0; temp<MAX_NUMBER_COMPONENTS; temp++) 
	{
		if (ComponentHeaderList[temp].m_validComp == 1) 
		{
			count++;
		}
	}
	
	return count;
}

int DSContainer::GetComponentHeaderSize() 
{
	return sizeof(ComponentHeader);
}

int DSContainer::GetContainerSize() 
{
	// size of number_of_components: int
	int contSize = sizeof(int);
	// size of component_description: 
	contSize += DESCRIPTION_SIZE*sizeof(char);
	// size of each component: header & elements-pixels
	for (int temp=0; temp<MAX_NUMBER_COMPONENTS; temp++) 
	{
		if (ComponentHeaderList[temp].m_validComp == 1) 
		{
			contSize = contSize+GetComponentHeaderSize()+ComponentHeaderList[temp].m_length;
		}
	}
	return contSize;
}

int DSContainer::GetLastComponentNumber() 
{
	return GetNumberOfComponents()-1;
}

enum ILLUMINANT DSContainer::GetIlluminant(const string& inputstring)
{
	enum ILLUMINANT illum;
	
	illum = no_illuminant;
	if(inputstring == "A") illum = A;
	else if(inputstring == "B") illum = B;
	else if(inputstring == "C") illum = C;
	else if(inputstring == "D50") illum = D50;
	else if(inputstring == "D55") illum = D55;
	else if(inputstring == "D65") illum = D65;
	else if(inputstring == "D75") illum = D75;
	else if(inputstring == "F2") illum = F2;
	else if(inputstring == "F7") illum = F7;
	else if(inputstring == "F11") illum = F11;

	return illum;
}

enum OBSERVER DSContainer::GetObserver(const string& inputString)
{
	enum OBSERVER obs;
	
	obs = no_observer;
	if(inputString == "deg_2") obs = deg_2;
	else if(inputString == "deg_10") obs = deg_10;

	return obs;
}

enum UNIT DSContainer::GetUnit(const string& inputString)
{
	enum UNIT unit;
	
	unit = no_unit;
	if(inputString == "cd_m2") unit = cd_m2;
	else if(inputString == "contrast_ratio") unit = contrast_ratio;
	else if(inputString == "video_level") unit = video_level;
	else if(inputString == "kelvin") unit = kelvin;
	else if(inputString == "degrees_celsius") unit = degrees_celsius;
	else if(inputString == "percent") unit = percent;
	else if(inputString == "JND") unit = JND;
			
	return unit;
}

int DSContainer::CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, 
								  float elemSizeCol, float elemSizeRow, const string& unit,
								  unsigned int nbBits, const float whitePt[3], const string& illum, const string& obs, enum ComponentType elemType, 
								  unsigned int nbFrames, float frameRate, enum AllocatorType allocType)
{
	return CreateComponent(nbRowElemPerFrame, nbColumnElemPerFrame,
		elemSizeCol, elemSizeRow, GetUnit(unit),
		nbBits, whitePt, GetIlluminant(illum), GetObserver(obs), elemType,
		nbFrames, frameRate, allocType);
}

int DSContainer::CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, 
								  float elemSizeCol, float elemSizeRow, enum UNIT unit,
								  unsigned int nbBits, const float whitePt[3], enum ILLUMINANT illum, enum OBSERVER obs, enum ComponentType elemType, 
								  unsigned int nbFrames, float frameRate, enum AllocatorType allocType)
{
	int nbElemPerFrame = nbRowElemPerFrame*nbColumnElemPerFrame;
	// find free component (this should use GetNumberOfComponents())
	int index = 0;
	int elemSize = 0;
	for (index=0; index<MAX_NUMBER_COMPONENTS; index++) 
	{
		if (ComponentHeaderList[index].m_validComp == 0) 
		{ 
			break;
		}
	}

	if (index>=MAX_NUMBER_COMPONENTS) 
	{
		ErrorLogger::logError("DSContainer::CreateComponent: Maximum number of components reached\n");
	}

	// clear description
	memset(ComponentHeaderList[index].m_description, 0, ComponentHeader::DESCRIPTION_SIZE);

	// determine element size
	elemSize = TypeTraits::getTypeSize(elemType);
	if(elemSize == -1)
	{
		ComponentHeaderList[index].m_validComp = 0;
		ErrorLogger::logError("DSContainer::CreateComponent: Invalid element type provided\n");
	}

	// allocate memory
	unsigned long dataSize = nbElemPerFrame * nbFrames * elemSize;
	if (allocType == default_allocator) {
		ComponentHeaderList[index].m_baseAddress = new unsigned char[dataSize];
	}
#ifdef USE_OPENCL
	else { 
		OpenCLManager* oclMgr = OpenCLManager::getInstance();
		const cl::Context& ctx = oclMgr->getContextReference();
		cl_mem clObj; // cl_mem is a ptr type

		if (allocator_type == opencl_buffer) {
			clObj = clCreateBuffer(ctx(), CL_MEM_READ_WRITE, dataSize, 0, 0);
		} else {
			cl::ImageFormat img_fmt(CreateOpenCLImageFormat(elemType));
			if (allocator_type == opencl_image2D) {
				clObj = clCreateImage2D(ctx(), CL_MEM_READ_WRITE, &img_fmt, nbColumnElemPerFrame,
										nbRowElemPerFrame, 0/*row_pitch*/, 0, 0);
			} else if (allocator_type == opencl_image3D) {
				clObj = clCreateImage3D(ctx(), CL_MEM_READ_WRITE, &img_fmt, nbColumnElemPerFrame,
										nbRowElemPerFrame, nbFrames, 0/*row_pitch*/, 0/*slice_pitch*/, 0, 0);
			} else {
				clObj = 0;
				ErrorLogger::logError("DSContainer::CreateComponent: Invalid allocator_type provided\n");
			}
		}

		ComponentHeaderList[index].m_base_address = (unsigned char*)clObj;
	}
#endif // USE_OPENCL

	ComponentHeaderList[index].m_allocType = allocType;
	ComponentHeaderList[index].m_length = nbElemPerFrame * elemSize*nbFrames;
	ComponentHeaderList[index].m_nbElemPerFrame = nbElemPerFrame;
	ComponentHeaderList[index].m_elemType = elemType;
	ComponentHeaderList[index].m_nbRowElemPerFrame = nbRowElemPerFrame;
	ComponentHeaderList[index].m_nbColumnElemPerFrame = nbColumnElemPerFrame;
	ComponentHeaderList[index].m_whitePt[0] = whitePt[0];
	ComponentHeaderList[index].m_whitePt[1] = whitePt[1];
	ComponentHeaderList[index].m_whitePt[2] = whitePt[2];
	ComponentHeaderList[index].m_elemSizeCol = elemSizeCol; //in mm
	ComponentHeaderList[index].m_elemSizeRow = elemSizeRow; // in mm
	ComponentHeaderList[index].m_illum = illum;
	ComponentHeaderList[index].m_obs = obs;
	ComponentHeaderList[index].m_unit = unit;
	ComponentHeaderList[index].m_nbBits = nbBits;
	ComponentHeaderList[index].m_elemSize = elemSize;
	// condition to test if component is valid
	ComponentHeaderList[index].m_validComp = 1;
	ComponentHeaderList[index].m_nbFrames = nbFrames;
	ComponentHeaderList[index].m_frameRate = frameRate;

	return 0;
}

int DSContainer::CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, enum ComponentType elemType) 
{
	const float whitePt[3] = {0.0f, 0.0f, 0.0f};
	return CreateComponent(nbRowElemPerFrame, nbColumnElemPerFrame, 0.0f, 0.0f,
							no_unit, 0, whitePt, no_illuminant, no_observer, elemType, 1, 1.0f);
}	

int DSContainer::CreateComponent(int nbComp, enum AllocatorType targetAllocator) {
	int cidx = nbComp;

	if (CreateComponent(ComponentHeaderList[cidx].m_nbRowElemPerFrame, ComponentHeaderList[cidx].m_nbColumnElemPerFrame,
						ComponentHeaderList[cidx].m_elemSizeCol, ComponentHeaderList[cidx].m_elemSizeRow, ComponentHeaderList[cidx].m_unit,
						ComponentHeaderList[cidx].m_nbBits, ComponentHeaderList[cidx].m_whitePt, ComponentHeaderList[cidx].m_illum, ComponentHeaderList[cidx].m_obs,
						ComponentHeaderList[cidx].m_elemType, ComponentHeaderList[cidx].m_nbFrames, ComponentHeaderList[cidx].m_frameRate, targetAllocator) == -1)
	{
		return -1;
	}

	int targetCompNb = GetNumberOfComponents() - 1; // CreateComponent() creates at the end (why doesn't it return this nb?!?)

#ifdef USE_OPENCL
	// Transfer data to/from OpenCL memory if involved.
	OpenCLManager* oclMgr = OpenCLManager::getInstance();
	const cl::CommandQueue& cq = oclMgr->getCommandQueueReference();

	// Not implemented (easy to add): copy buffer->buffer, image->image, buffer->image, image->buffer
	if (ComponentHeaderList[cidx].allocator_type == default_allocator) {
		if (targetAllocator == opencl_buffer) {
			clEnqueueWriteBuffer(cq(), (cl_mem)ComponentHeaderList[targetCompNb].m_baseAddress, CL_TRUE/*blocking*/, 0/*offset*/, ComponentHeaderList[cidx].length, ComponentHeaderList[cidx].base_address, 0/*#events_in_arr*/, 0/*event_wait_arr*/, 0/*this_event*/);
		} else if (targetAllocator == opencl_image2D || targetAllocator == opencl_image3D) {
			size_t origin[3] = {0};
			size_t region[3];
			region[0] = ComponentHeaderList[cidx].m_nbColumnElemPerFrame;
			region[1] = ComponentHeaderList[cidx].m_nbRowElemPerFrame;
			region[2] = ComponentHeaderList[cidx].m_nbFrames;

			clEnqueueWriteImage(cq(), (cl_mem)ComponentHeaderList[targetCompNb].m_baseAddress, CL_TRUE/*blocking*/, origin, region, 0/*row_pitch*/, 0/*slice_pitch*/, ComponentHeaderList[cidx].m_baseAddress, 0/*#events_in_arr*/, 0/*event_wait_arr*/, 0/*this_event*/);
		}
	} else if (ComponentHeaderList[cidx].allocator_type == opencl_buffer) {
		if (targetAllocator == default_allocator) {
			clEnqueueReadBuffer(cq(), (cl_mem)ComponentHeaderList[targetCompNb].m_baseAddress, CL_TRUE/*blocking*/, 0/*offset*/, ComponentHeaderList[cidx].m_length, ComponentHeaderList[targetCompNb].m_baseAddress, 0/*#events_in_arr*/, 0/*event_wait_arr*/, 0/*this_event*/);
		}
	} else if (ComponentHeaderList[cidx].m_alloType == opencl_image2D || ComponentHeaderList[cidx].m_alloType == opencl_image3D) {
		if (targetAllocator == default_allocator) {
			size_t origin[3] = {0};
			size_t region[3];
			region[0] = ComponentHeaderList[cidx].m_nbColumnElemPerFrame;
			region[1] = ComponentHeaderList[cidx].m_nbRowElemPerFrame;
			region[2] = ComponentHeaderList[cidx].m_nbFrames;

			clEnqueueReadImage(cq(), (cl_mem)ComponentHeaderList[cidx].m_baseAddress, CL_TRUE/*blocking*/, origin, region, 0/*row_pitch*/, 0/*slice_pitch*/, ComponentHeaderList[targetCompNb].m_baseAddress, 0/*#events_in_arr*/, 0/*event_wait_arr*/, 0/*this_event*/);
		}
	}
#endif // USE_OPENCL

	return 0;
}

void DSContainer::DeleteComponentData(int nbComp) 
{
	if (ComponentHeaderList[nbComp].m_validComp == 1)
	{
		ComponentHeaderList[nbComp].m_validComp = 0;
		if (ComponentHeaderList[nbComp].m_allocType == default_allocator)
		{ 
			delete[] ComponentHeaderList[nbComp].m_baseAddress;
		} else {
			int err = -1;
#ifdef USE_OPENCL
			// Do the type magic to have the right destructor called that releases the allocation in device memory.
			if (ComponentHeaderList[nbComp].m_allocType == opencl_buffer) {
				err = clReleaseMemObject((cl_mem)ComponentHeaderList[nbComp].m_baseAddress);
			} else if (ComponentHeaderList[nbComp].m_allocType == opencl_image2D) {
				err = clReleaseMemObject((cl_mem)ComponentHeaderList[nbComp].m_baseAddress);
			} else if (ComponentHeaderList[nbComp].m_allocType == opencl_image3D) {
				err = clReleaseMemObject((cl_mem)ComponentHeaderList[nbComp].m_baseAddress);
			}
#endif
			if (err != 0) {
				ErrorLogger::warning("DSContainer::DeleteComponentData: Failed to deallocate\n");
			}
		}
		ComponentHeaderList[nbComp].m_baseAddress = 0;
	}
}

int DSContainer::GetComponentAllocatorType(int nbComp, enum AllocatorType& atype)
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentAllocatorType: Invalid component number\n");
		return -1;
	}

	atype = ComponentHeaderList[nbComp].m_allocType;
	return 0;
}

int DSContainer::GetComponentDescription(int nbComp, string& description) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentDescription: Invalid component number\n");
		return -1;
	}

	description = ComponentHeaderList[nbComp].m_description;

	return 0;
}

int DSContainer::SetComponentDescription(int nbComp, const string& description) 
{

	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentDescription: Invalid component number\n");
		return -1;
	}

	memset(ComponentHeaderList[nbComp].m_description,0,ComponentHeader::DESCRIPTION_SIZE);

	for (unsigned temp=0; temp<1024; temp++) 
	{
		if (temp >= description.size())
			break;
		ComponentHeaderList[nbComp].m_description[temp] = description[temp];
		if (description[temp]=='\n')
			break;
	}

	return 0;
}

int DSContainer::GetComponentNumberOfFrames(int nbComp, unsigned int * nbFrames) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentNumberOfFrames: Invalid component number\n");
		return -1;
	}
	else 
	{
		*nbFrames=ComponentHeaderList[nbComp].m_nbFrames;
		return 0;
	}
	
}

int DSContainer::GetComponentFrameRate(int nbComp, float * frameRate) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentFrameRate: Invalid component number\n");
		return -1;
	}
	else 
	{
		*frameRate=ComponentHeaderList[nbComp].m_frameRate;
		return 0;
	}
	
}

int DSContainer::GetComponentElementType(int nbComp, enum ComponentType* elemType) 
{
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
			ErrorLogger::logError("DSContainer::GetComponentElementType: Invalid component number\n");
			return -1;
		}

	*elemType= (ComponentHeaderList[nbComp]).m_elemType;
	return 0;
}

int DSContainer::GetComponentElement(int nbComp, int nbElemPerFrame, int nbFrame, void* element) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentElement: Invalid component number\n");
		return -1;
	}

	int elemSize = ComponentHeaderList[nbComp].m_elemSize;
	if(elemSize == -1)
	{
		ErrorLogger::logError("DSContainer::GetComponentElement: Invalid component element type\n");
		return -1;
	}

	unsigned char * startAddress = (unsigned char *) (ComponentHeaderList[nbComp].m_baseAddress);
	startAddress += nbElemPerFrame * elemSize + nbFrame * ComponentHeaderList[nbComp].m_nbElemPerFrame * elemSize;

	memcpy(element, startAddress, elemSize);

	return 0;
}

int DSContainer::SetComponentElement(int nbComp, int nbElemPerFrame, unsigned int nbFrame, void* element) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentElement: Invalid component number\n");
		return -1;
	}
	if (nbFrame>=ComponentHeaderList[nbComp].m_nbFrames)
	{
		ErrorLogger::logError("DSContainer::SetComponentElement: Invalid nbFrame\n");
		return -1;
	}

	int elemSize = ComponentHeaderList[nbComp].m_elemSize;
	if(elemSize == -1)
	{
		ErrorLogger::logError("DSContainer::SetComponentElement: Invalid component element type\n");
		return -1;
	}
	if(unsigned(nbElemPerFrame) > ComponentHeaderList[nbComp].m_nbElemPerFrame)
	{
		ErrorLogger::logError("DSContainer::SetComponentElement: Set element is going out of frame\n");
		return -1;
	}

	unsigned char * startAddress;
	startAddress = (unsigned char *) (ComponentHeaderList[nbComp].m_baseAddress);
	startAddress += nbElemPerFrame * elemSize + nbFrame * ComponentHeaderList[nbComp].m_nbElemPerFrame * elemSize;

	memcpy(startAddress, element, elemSize);

	return 0;

}

int DSContainer::GetComponentNumberOfBits(int nbComp, unsigned int* nbBits) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentNumberOfBits: Invalid component number\n");
		return -1;
	}
	else 
	{
		*nbBits = ComponentHeaderList[nbComp].m_nbBits;
		return 0;
	}
}

int DSContainer::SetComponentNumberOfBits(int nbComp, unsigned int * nbBits) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentNumberOfBits: Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_nbBits =* nbBits;
		return 0;
	}
}

int DSContainer::GetComponentNumberOfRowElementsPerFrame(int nbComp, unsigned long* nbRowElemPerFrame) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentNumberOfRowElementsPerFrame: Invalid component number\n");
		return -1;
	}
	else 
	{
		*nbRowElemPerFrame = ComponentHeaderList[nbComp].m_nbRowElemPerFrame;
		return 0;
	}
}

int DSContainer::SetComponentNumberOfRowElements(int nbComp, unsigned long* nbRowElemPerFrame) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentNumberOfRowElements: Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_nbRowElemPerFrame =* nbRowElemPerFrame;
		return 0;
	}
}

int DSContainer::GetComponentNumberOfColumnElementsPerFrame(int nbComp, unsigned long* nbColumnElemPerFrame)
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentNumberOfColumnElementsPerFrame: Invalid component number\n");
		return -1;
	}
	else 
	{
		*nbColumnElemPerFrame = ComponentHeaderList[nbComp].m_nbColumnElemPerFrame;
		return 0;
	}
}

int DSContainer::SetComponentNumberOfColumnElements(int nbComp, unsigned long* nbColumnElemPerFrame) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentNumberOfColumnElements: Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_nbColumnElemPerFrame =* nbColumnElemPerFrame;
		return 0;
	}
}

int DSContainer::GetComponentElementSizeColumn(int nbComp, float* elemSizeCol) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentElementSizeColumn: Invalid component number\n");
		return -1;
	}
	else 
	{
		*elemSizeCol = ComponentHeaderList[nbComp].m_elemSizeCol;
		return 0;
	}
}

int DSContainer::SetComponentElementSizeColumn(int nbComp, float* elemSizeCol) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentElementSizeColumn: Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_elemSizeCol = *elemSizeCol;
		return 0;
	}
}

int DSContainer::GetComponentElementSizeRow(int nbComp, float* elemSizeRow) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentElementSizeRow: Invalid component number\n");
		return -1;
		}
	else 
	{
		*elemSizeRow = ComponentHeaderList[nbComp].m_elemSizeRow;
		return 0;
	}
}

int DSContainer::SetComponentElementSizeRow(int nbComp, float* elemSizeRow) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentElementSizeRow: Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_elemSizeRow = *elemSizeRow;
		return 0;
	}
}

int DSContainer::GetComponentWhitePoint(int nbComp, float* whitePt) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1)
	{
		ErrorLogger::logError("DSContainer::GetComponentWhitePoint: Invalid component number\n");
		return -1;
	}
	else
	{
		whitePt[0] = ComponentHeaderList[nbComp].m_whitePt[0];
		whitePt[1] = ComponentHeaderList[nbComp].m_whitePt[1];
		whitePt[2] = ComponentHeaderList[nbComp].m_whitePt[2];
		return 0;
	}
}

int DSContainer::SetComponentWhitePoint(int nbComp, float* whitePt) 
{
	
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("Invalid component number\n");
		return -1;
	}
	else 
	{
		ComponentHeaderList[nbComp].m_whitePt[0] = whitePt[0];
		ComponentHeaderList[nbComp].m_whitePt[1] = whitePt[1];
		ComponentHeaderList[nbComp].m_whitePt[2] = whitePt[2];
		return 0;
	}
}

int DSContainer::GetComponentIlluminant(int nbComp, string& illuminant)
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentIlluminant: Invalid component number\n");
		return -1;
	}
	else 
	{
		enum ILLUMINANT illum;
		illum = ComponentHeaderList[nbComp].m_illum;
	
		switch(illum)
		{
		case A:
			illuminant = "A";
			break;
		case B:
			illuminant = "B";
			break;
		case C:
			illuminant = "C";
			break;
		case D50:
			illuminant = "D50";
			break;
		case D55:
			illuminant = "D55";
			break;
		case D65:
			illuminant = "D65";
			break;
		case D75:
			illuminant = "D75";
			break;
		case F2:
			illuminant = "F2";
			break;
		case F7:
			illuminant = "F7";
			break;
		case F11:
			illuminant = "F11";
			break;
		default:
			illuminant = "no_illuminant";
			break;
		}

		return 0;
	}
}

int DSContainer::SetComponentIlluminant(int nbComp, const string& inString)
{
	if (ComponentHeaderList[nbComp].m_validComp!=1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentIlluminant: Invalid component number\n");
		return -1;
	}
	else 
	{
		enum ILLUMINANT illum = GetIlluminant(inString);

		ComponentHeaderList[nbComp].m_illum = illum;
		return 0;
	}
}

int DSContainer::GetComponentObserver(int nbComp, string& outstring) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentObserver: Invalid component number\n");
		return -1;
	}
	else 
	{
		enum OBSERVER obs;
	
		obs = ComponentHeaderList[nbComp].m_obs;
		if(obs == deg_2) outstring = "deg_2";
		else if(obs == deg_10) outstring = "deg_10";
		else outstring = "no_observer";
		return 0;
	}
}

int DSContainer::SetComponentObserver(int nbComp, const string& instring) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentObserver: Invalid component number\n");
		return -1;
	}
	else 
	{
		enum OBSERVER obs = GetObserver(instring);
		ComponentHeaderList[nbComp].m_obs = obs;
	return 0;
	}
}

int DSContainer::GetComponentUnit(int nbComp, string& outString) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentUnit: Invalid component number\n");
		return -1;
	}
	else
	{
		enum UNIT unit;
	
		unit = ComponentHeaderList[nbComp].m_unit;

		switch(unit)
		{
		case cd_m2:
			outString = "cd_m2";
			break;
		case contrast_ratio:
			outString = "contrast_ratio";
			break;
		case video_level:
			outString = "video_level";
			break;
		case kelvin:
			outString = "kelvin";
			break;
		case degrees_celsius:
			outString = "degrees_celsius";
			break;
		case percent:
			outString = "percent";
			break;
		case JND:
			outString = "JND";
			break;
		default:
			outString = "no_unit";
			break;
		}

		return 0;
	}
}

int DSContainer::SetComponentUnit(int nbComp, const string& inString) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::SetComponentUnit: Invalid component number\n");
		return -1;
	}
	else 
	{
		enum UNIT unit = GetUnit(inString);
		ComponentHeaderList[nbComp].m_unit = unit;
		return 0;
	}
}

int DSContainer::GetComponentValid(int nbComp) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentValid: Invalid component\n");
		return -1;
	}
	else 
	{
		return 0;
	}
}

int DSContainer::GetComponentNumberOfElementsPerFrame(int nbComp, int* nbElemPerFrame) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentNumberOfElementsPerFrame: Invalid component\n");
		return -1;
	}
	else 
	{
		*nbElemPerFrame = ComponentHeaderList[nbComp].m_nbElemPerFrame;
		return 0;
	}

}

int DSContainer::GetComponentLength(int nbComp, int* length) 
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("DSContainer::GetComponentLength: Invalid component\n");
		return -1;
	}
	else
	{
		*length = ComponentHeaderList[nbComp].m_length;
		return 0;
	}
}

int DSContainer::GetContainerDescription(string& description) 
{
	description = m_description;
	return 0;
}

int DSContainer::SetContainerDescription(const string& description) 
{
	int temp=0;
	for (temp=0; temp<1024; temp++)
	{
		if (description[temp]=='\n')
			break;
		else
			m_description[temp] = description[temp];
	}
	return 0;
}

int DSContainer::SaveToFileWithCompression (const string& filename) 
{
	int nbComponents = GetNumberOfComponents();
	if (nbComponents < 1) 
	{
		ErrorLogger::logError("DSContainer::SaveToFileWithCompression: No component exists, unable to save in file\n");
		return -1;
	}
	else 
	{
		gzFile compressedFile; // file compressed with gz
		compressedFile = gzopen (filename.c_str(), "wb"); 
		
		// check if the file exists and the path is correct
		ifstream testFile(filename.c_str());
		if (!testFile.good())
		{
			ErrorLogger::logError("DSContainer::SaveToFileWithCompression: the file given as parameter does not exist, please check the path");
		}

		int test = 0;

		test=gzwrite(compressedFile, (char *) (&nbComponents), sizeof (int));//writing test
		if(test == 0)
		{
			ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
		}

		char c_description[DESCRIPTION_SIZE];
		strncpy_s(c_description,DESCRIPTION_SIZE*sizeof(char), m_description.c_str(), DESCRIPTION_SIZE);

		test=gzwrite(compressedFile, c_description, sizeof(c_description));//writing test
		if(test == 0)
		{
			ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
		}

		for (int temp=0; temp<MAX_NUMBER_COMPONENTS; temp++) // for each component
		{
			if (ComponentHeaderList[temp].m_validComp==1)
			{
				//write the component header
				test = gzwrite (compressedFile, (char *) (&ComponentHeaderList[temp]), sizeof (ComponentHeader));
				if(test == 0)
				{
					ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
				}
				//write the frame component elements
				test = gzwrite (compressedFile, (char *) (ComponentHeaderList[temp].m_baseAddress), ComponentHeaderList[temp].m_length);
				if(test == 0)
				{
					ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
				}
			}
		}//end for each component
		gzclose(compressedFile);
		
		return 0;
	}
}

int DSContainer::SaveToFileWithCompressionLastComponent (const string& filename) 
{
	int nbComponents = 1;

	gzFile compressedFile; // file compressed with gz
	compressedFile=gzopen (filename.c_str(), "wb"); 
	
	// check if the file exists and the path is correct
	ifstream testFile(filename.c_str());
	if (!testFile.good())
	{
		ErrorLogger::logError("DSContainer::SaveToFileWithCompressionLastComponent: the file given as parameter does not exist, please check the path");
	}
	int test = 0;

	test = gzwrite(compressedFile, (char *) (&nbComponents), sizeof (int));//writing test
	if(test==0)
	{
		ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
	}

	char c_description[DESCRIPTION_SIZE];
	//strncpy(c_description, m_description.c_str(), DESCRIPTION_SIZE);
	strncpy_s(c_description,DESCRIPTION_SIZE*sizeof(char), m_description.c_str(), DESCRIPTION_SIZE);

	test=gzwrite(compressedFile, c_description, sizeof (c_description));//writing test
	if(test==0)
	{
		ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
	}
	if (ComponentHeaderList[GetNumberOfComponents()-1].m_validComp == 1)
	{
		//write the component header
		test = gzwrite (compressedFile, (char *) (&ComponentHeaderList[GetNumberOfComponents()-1]), sizeof (ComponentHeader));
		if(test == 0)
		{
			ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
		}
		//write the frame component elements
		test = gzwrite (compressedFile, (char *) (ComponentHeaderList[GetNumberOfComponents()-1].m_baseAddress), ComponentHeaderList[GetNumberOfComponents()-1].m_length);
		if(test == 0)
		{
			ErrorLogger::logError("DSContainer::SaveToFileWithCompression: file compression with zlib failed");
		}

	}
	gzclose(compressedFile);
	
	return 0;

}

int DSContainer::LoadFromFileWithCompression (const string& filename)
{

	gzFile decompressedFile;
	decompressedFile=gzopen(filename.c_str(), "rb");
	
	ifstream testFile(filename.c_str());
	if (!testFile.good())
	{
		ErrorLogger::logError("DSContainer::LoadFromFileWithCompression: the file given as parameter does not exist, please check the path");
	}

	int nbComponents, test;
	test=gzread(decompressedFile, (char *) (&nbComponents), sizeof (nbComponents));//reading test
	if(test == 0)
	{
		ErrorLogger::logError("DSContainer::LoadFromFileWithCompression: file decompression with zlib failed");
	}

	char c_description[DESCRIPTION_SIZE];

	gzread(decompressedFile, (char *) (c_description), sizeof(c_description));//reading test
	if(test == 0)
	{
		ErrorLogger::logError("DSContainer::LoadFromFileWithCompression: file decompression with zlib failed");
	}

	if(c_description[DESCRIPTION_SIZE-1] != '\0')
		c_description[DESCRIPTION_SIZE-1] = '\0';

	m_description = c_description;

	for (int temp = 0; temp < nbComponents; temp++)//for each component
	{
		delete [] ComponentHeaderList[temp].m_baseAddress;
		
		//read the component header
		gzread(decompressedFile, (char *) (&ComponentHeaderList[temp]), sizeof (ComponentHeader));	
		if(test==0)
		{
			ErrorLogger::logError("DSContainer::LoadFromFileWithCompression: file decompression with zlib failed");
		}
		ComponentHeaderList[temp].m_baseAddress = new unsigned char[
			ComponentHeaderList[temp].m_nbElemPerFrame *
			ComponentHeaderList[temp].m_nbFrames *
			ComponentHeaderList[temp].m_elemSize
		];
		
		//read the component elements
		gzread(decompressedFile, (char *) (ComponentHeaderList[temp].m_baseAddress), ComponentHeaderList[temp].m_length);	
		if(test == 0)
		{
			ErrorLogger::logError("DSContainer::LoadFromFileWithCompression: file decompression with zlib failed");
		}
	}//end for each component

	gzclose(decompressedFile);

	return 0;
}

int DSContainer::SaveToFile(const string& filename) 
{
	int nbComponents = GetNumberOfComponents();
	if (nbComponents<1) {
		ErrorLogger::logError("DSContainer::SaveToFile: No component exists, unable to save in file function: SaveToFile\n");
		return -1;
	}
	else 
	{
		ofstream myFileO (filename.c_str(), ios::binary); //file reading
		// check if the file exists and the path is correct
		if (!myFileO.good())
		{
			ErrorLogger::logError("DSContainer::SaveToFile: the file given as parameter does not exist, please check the path");
		}
		myFileO.write((const char *) (&nbComponents), sizeof(nbComponents));
		char c_description[DESCRIPTION_SIZE];
		memset(c_description, 0, DESCRIPTION_SIZE);
		//strcpy(c_description, m_description.substr(0,DESCRIPTION_SIZE).c_str());
		strcpy_s(c_description,DESCRIPTION_SIZE*sizeof(char), m_description.substr(0,DESCRIPTION_SIZE).c_str());
		myFileO.write(c_description, DESCRIPTION_SIZE*sizeof(char));
		
		for (int temp = 0; temp < MAX_NUMBER_COMPONENTS; temp++) //for each component
		{
			if (ComponentHeaderList[temp].m_validComp==1) 
			{
				myFileO.write((const char *) (&ComponentHeaderList[temp]), sizeof (ComponentHeader));
				myFileO.write((const char *) (ComponentHeaderList[temp].m_baseAddress), ComponentHeaderList[temp].m_length);
			}
		}//end for each component

		myFileO.close();
		
		return 0;
	}
}

int DSContainer::SaveFrameBMP(int nbComp, int nbFrame, const string& filename, bool normalization, int nbChannels)
{
	BMP bitmap;

	unsigned long width, height;
	unsigned int nbBit;
	this->GetComponentNumberOfColumnElementsPerFrame(nbComp, &width);
	this->GetComponentNumberOfRowElementsPerFrame(nbComp, &height);
	this->GetComponentNumberOfBits(nbComp, &nbBit);
	bitmap.SetSize(width, height);

	float factor = pow((float)2, (float)nbBit)-1;

	if (nbChannels == 3) {
		int rgb[3]={0};
		float frgb[3]={0};

		for(unsigned i=0;i<height;i++) 
			for(unsigned j=0;j<width;j++) {
				RGBApixel* bmp = bitmap( j, i );

				if (normalization == false) 
				{
					this->GetComponentElement(nbComp, i*width+j, nbFrame, &frgb);
					if (frgb[0]>255) {frgb[0]=frgb[1]=frgb[2]=255;}
					bmp->Red=(int) (0.5 +frgb[0]);
					bmp->Green=(int) (0.5 +frgb[1]);
					bmp->Blue=(int) (0.5 +frgb[2]);
				}
				else 
				{
					this->GetComponentElement(nbComp, i*width+j, nbFrame, &rgb);
					bmp->Red=(int) (0.5 +rgb[0]*255/factor);
					bmp->Green=(int) (0.5 +rgb[1]*255/factor);
					bmp->Blue=(int) (0.5 +rgb[2]*255/factor);
				}
		}
	}
	else {
		if (nbChannels == 1) {
			float gray = 0;
			float fgray = 0;

			for(unsigned i=0;i<height;i++) 
				for(unsigned j=0;j<width;j++) {
					RGBApixel* bmp = bitmap( j, i );

					if (normalization == false) 
					{
						this->GetComponentElement(nbComp, i*width+j, nbFrame, &fgray);
						if (fgray>255) {fgray=255;}
						bmp->Red=(int) (0.5 + fgray);
						bmp->Green=(int) (0.5 + fgray);
						bmp->Blue=(int) (0.5 + fgray);
					}
					else 
					{
						this->GetComponentElement(nbComp, i*width+j, nbFrame, &gray);
						bmp->Red = (int) (0.5 + gray*255.0/*/factor*/);
						bmp->Green = (int) (0.5 + gray*255.0/*/factor*/);
						bmp->Blue = (int) (0.5 + gray*255.0/*/factor*/);
					}
				}
		}
	}
	
	bitmap.WriteToFile(filename.c_str());
	
	return 0;
}

char DSContainer::GetComponentElementTypeChar(enum ComponentType elemType)
{
	char type = TypeTraits::getTypeLetter(elemType);
	if(type == '\0')
		ErrorLogger::logError("DSContainer::GetComponentElementTypeChar: Invalid element type\n");

	return type;
}

bool DSContainer::SaveFrameTXT(int nbComp, int nbFrame, const string& filename, int channelToWrite)
{

	bool noError=true;

	char type;//c=char; i=integer; f=float; d=double

	unsigned long width, height;
	enum ComponentType eType;
	this->GetComponentNumberOfColumnElementsPerFrame(nbComp, &width);
	this->GetComponentNumberOfRowElementsPerFrame(nbComp, &height);
	this->GetComponentElementType(nbComp, &eType);

	int cPlus1;
    string fileN = filename;

	int nbChannel = this->GetNbChannel(eType);
	unsigned int nbFrames = 0;
	this->GetComponentNumberOfFrames(nbComp, &nbFrames);

	if (channelToWrite<-1 || channelToWrite>nbChannel)
	{
		ErrorLogger::logError("DSContainer::SaveFrameTXT: illegal channel value\n");
		noError=false;
	}

	int number_of_frame = int(nbFrames);
	if (nbFrame<-1 || nbFrame>number_of_frame)
	{
		ErrorLogger::logError("DSContainer::SaveFrameTXT: illegal frame value\n");
		noError=false;
	}

	if (noError)
	{
		type = GetComponentElementTypeChar(eType);
		
		char* val = new char[GetTypeSizeOfComponent(nbComp)];
		for(unsigned f=0; f < nbFrames; ++f)
		{
			if(nbFrame == -1 || nbFrame == f)
			{
				for(int c = 0; c < nbChannel; ++c)
				{
					if(channelToWrite == -1 || channelToWrite == c+1)
					{
						cPlus1 = c+1;
						stringstream ss;
						ss << setw(4) << setfill('0') << f;
						string currentFilename = 
							filename + "_frame" + ss.str() + "_chan_" +
							boost::lexical_cast<std::string>(cPlus1) + ".txt";
						ofstream file(currentFilename.c_str());
						if(!file)
							ErrorLogger::logError("DSContainer::SaveFrameTXT: error during file creation\n");
						for(unsigned i = 0; i < height; ++i)
						{
							for(unsigned j = 0; j < width; ++j)
							{
								GetComponentElement(nbComp, i*width+j, f, val);
								if(type == 'c')
									file << (int)((char*)val)[c] << '\t';
								else if(type == 'i')
									file << ((int*)val)[c] << '\t';
								else if(type == 'f')
									file << ((float*)val)[c] << '\t';
								else if(type == 'd')
									file << ((double*)val)[c] << '\t';
								else if(type == 'u')
									file << ((unsigned short*)val)[c] << '\t';
								else
									ErrorLogger::logError("DSContainer::SaveFrameTXT: Unknown type: SaveFrameTXT\n");
							}
							file << endl;
						}
						file.close();
					}
				}
			}
		}
		delete [] val;
	}

	return noError;
}

bool DSContainer::Save1dPlotTXT(int nbComp, int nbFrame, const string& filename, int channelToWrite, int xy[2])
{

	bool noError=true;

	char type;//c=char; i=integer; f=float; d=double

	unsigned long width, height;
	enum ComponentType eType;
	this->GetComponentNumberOfColumnElementsPerFrame(nbComp, &width);
	this->GetComponentNumberOfRowElementsPerFrame(nbComp, &height);
	this->GetComponentElementType(nbComp,&eType);

	//int cPlus1;
    string fileN = filename;

	int nbChannel =this->GetNbChannel(eType);
	unsigned int nbFrames=0;
	this->GetComponentNumberOfFrames(nbComp,&nbFrames);

	if (channelToWrite<-1 || channelToWrite>nbChannel)
	{
		ErrorLogger::logError("DSContainer::Save1dPlotTXT: illegal channel value\n");
		noError=false;
	}

	int number_of_frame=int(nbFrames);
	if (nbFrame<-1 || nbFrame>number_of_frame)
	{
		ErrorLogger::logError("DSContainer::Save1dPlotTXT: illegal frame value:\n");
		noError=false;
	}

	if (noError)
	{
		type=GetComponentElementTypeChar(eType);
		
		char* val = new char[GetTypeSizeOfComponent(nbComp)];
		int iterator = 0;
		for(int c=0; c < nbChannel; ++c)
		{
			if(channelToWrite == -1 || channelToWrite == c+1)
			{
				string currentFilename = filename + ".txt";
				ofstream file(currentFilename.c_str());

				if(!file)
					ErrorLogger::logError("DSContainer::Save1dPlotTXT: error during file creation\n");
				for(unsigned f=0; f < nbFrames; ++f)
				{
					if(nbFrame==-1 || nbFrame==f)
					{
						for(int i = xy[0]; i < (xy[0]+1); ++i)
						{
							for(int j = xy[1]; j < (xy[1]+1); ++j)
							{
								GetComponentElement(nbComp, i*width+j, f, val);
								if(type == 'c')
									file << (int)((char*)val)[c] << '\t';
								else if(type == 'i')
									file << ((int*)val)[c] << '\t';
								else if(type == 'f')
									file << ((float*)val)[c] << '\t';
								else if(type == 'd')
									file << ((double*)val)[c] << '\t';
								else if(type == 'u')
									file << ((unsigned short*)val)[c] << '\t';
								else
									ErrorLogger::logError("DSContainer::Save1dPlotTXT: Unknown type\n");
							}
							file << endl;
						}
					}
					iterator++;
				}
				file.close();
			}
		}
		delete [] val;
	}

	return noError;
}
 
int DSContainer::GetNbChannel(enum ComponentType elemType)
{
	//int nb=0;
	int nb = TypeTraits::getNChannels(elemType);

	//-1 if invalid
	return nb;
}

map<int, char*> DSContainer::initimageJmap()
{
	map<int, char*> mapimageJ;
	mapimageJ[0] = "8-bit";
	mapimageJ[1] = "32-bit Real";
	mapimageJ[2] = "64-bit Real";
	mapimageJ[3] = "32-bit Unsigned";
	mapimageJ[4] = "8-bit";
	mapimageJ[5] = "32-bit Unsigned";
	mapimageJ[6] = "32-bit Real";
	mapimageJ[7] = "32-bit Unsigned";
	mapimageJ[8] = "32-bit Unsigned";
	mapimageJ[9] = "32-bit Real";
	mapimageJ[10] = "32-bit Real";
	mapimageJ[11] = "32-bit Real";
	mapimageJ[12] = "32-bit Real";
	mapimageJ[13] = "32-bit Real";
	mapimageJ[14] = "32-bit Real";
	mapimageJ[15] = "32-bit Real";
	mapimageJ[16] = "32-bit Real";
	mapimageJ[17] = "32-bit Real";
	mapimageJ[18] = "32-bit Unsigned";
	mapimageJ[19] = "16-bit Unsigned";
	mapimageJ[20] = "16-bit Unsigned";
	return mapimageJ;
}

bool DSContainer::SaveFrameRAW(int nbComp, int nbFrame, const string& filename, int channelToWrite)
{
	bool noError=true;

	char type;//c=char; i=integer; f=float; d=double

	unsigned long width, height;
	enum ComponentType eType;

	// Check if we're not trying to directly save an OpenCL buffer (Use the DataTransferOCLModule and/or adapt the ComponentToWrite parameter)
	enum AllocatorType atype/* = unknown_allocator*/;
	this->GetComponentAllocatorType(nbComp, atype);
	if (atype != default_allocator)
	{
		ErrorLogger::logError("DSContainer::SaveFrameRAW(): the specified component is not in main memory. To save it, copy it there first, e.g. using the DataTransferOCLModule\n");
		noError = false;
	}
	
	// we get back the width, the  height and the type of the last component
	//unsigned int * m_nbBits;
	map<int, char*> mapimageJ;
	mapimageJ = initimageJmap();
	this->GetComponentNumberOfColumnElementsPerFrame(nbComp, &width);
	this->GetComponentNumberOfRowElementsPerFrame(nbComp, &height);
	this->GetComponentElementType(nbComp,&eType);
	const string& filenamedesc=filename+"_description_raw.txt";
	string typeimageJ = mapimageJ[eType];
	ofstream desc (filenamedesc.c_str(), std::ofstream::out);
	desc<<"file name: "<<filename<<endl;
	desc<<"width: "<<width<<endl;
	desc<<"height: "<<height<<endl;
	desc<<"type imageJ: "<<typeimageJ<<endl;
	desc.close();
	int cPlus1;
    string fileN = filename;

	// we get back the number of channels and the number of frames of the last component
	int nbChannel =this->GetNbChannel(eType);
	unsigned int nbFrames=0;
	this->GetComponentNumberOfFrames(nbComp,&nbFrames);

	if (channelToWrite<-1 || channelToWrite>nbChannel)
	{
		ErrorLogger::logError("DSContainer::SaveFrameRAW: illegal channel value\n");
		noError=false;
	}

	int number_of_frame=int(nbFrames);
	if (nbFrame<-1 || nbFrame>number_of_frame)
	{
		ErrorLogger::logError("DSContainer::SaveFrameRAW: illegal frame value\n");
		noError=false;
	}
	
	if (noError)
	{
	
	// we get back the type in char of the last component
		type=GetComponentElementTypeChar(eType);

		char* val = new char[GetTypeSizeOfComponent(nbComp)];
		for(unsigned f=0; f < nbFrames; ++f)
		{
			if(nbFrame==-1 || nbFrame==f)
			{
				for(int c=0; c < nbChannel; ++c)
				{
					if(channelToWrite == -1 || channelToWrite == c+1)
					{
						cPlus1 = c+1;
						stringstream ss;
						ss << setw(4) << setfill('0') << f;
						string currentFilename = 
							filename + "_frame" + ss.str() + "_chan_" +
							boost::lexical_cast<std::string>(cPlus1) + ".raw";
						ofstream file(currentFilename.c_str(), ios::binary);
						if(!file)
							ErrorLogger::logError("DSContainer::SaveFrameRAW: error during file creation\n");
						for(unsigned i = 0; i < height; ++i)
						{
							for(unsigned j = 0; j < width; ++j)
							{
								GetComponentElement(nbComp, i*width+j, f, val);
								if(type == 'c')
									file.write(&((char*)val)[c],sizeof(char));
								else if(type == 'i')
									file.write((char*)&((int*)val)[c],sizeof(int));
								else if(type == 'f')
									file.write((char*)&((float*)val)[c],sizeof(float));
								else if(type == 'd')
									file.write((char*)&((double*)val)[c],sizeof(double));
								else if(type == 'u')
									file.write((char*)&((unsigned short*)val)[c],sizeof(unsigned short));
								else
									ErrorLogger::logError("DSContainer::SaveFrameRAW: Unknown type\n");
							}
						}
						file.close();
					}
				}
			}
		}
		delete [] val;

	
	}
	
	return noError;
}

int DSContainer::LoadFromFile (const string& filename)
{
	ifstream myFileI (filename.c_str(), ios::binary);

	if(!myFileI) {
		ErrorLogger::logError("DSContainer::LoadFromFile: can not open file\n");
	}

	int nbComponents;
	myFileI.read ((char *) (&nbComponents), sizeof(int) );

	char c_description[DESCRIPTION_SIZE];
	myFileI.read(c_description, sizeof(char[1024]));

	if(c_description[DESCRIPTION_SIZE-1] != '\0')
		c_description[DESCRIPTION_SIZE-1] = '\0';

	m_description = c_description;

	for (int temp=0; temp<nbComponents; temp++) {

		delete [] ComponentHeaderList[temp].m_baseAddress;

		myFileI.read ((char *) &(ComponentHeaderList[temp]), sizeof (ComponentHeader));	

		ComponentHeaderList[temp].m_baseAddress = new unsigned char[
			ComponentHeaderList[temp].m_nbElemPerFrame *
			ComponentHeaderList[temp].m_nbFrames *
			ComponentHeaderList[temp].m_elemSize
		];

		myFileI.read ((char *) (ComponentHeaderList[temp].m_baseAddress), ComponentHeaderList[temp].m_length);
	}

	myFileI.close();

	return 0;
}

int DSContainer::GetNbChannelsOfComponent(int nbComp)
{
	int result = TypeTraits::getNChannels(ComponentHeaderList[nbComp].m_elemType);
	if(result == -1)
		ErrorLogger::logError("DSContainer::GetNbChannelsOfComponent: this type is not supported by the TypeTraits class\n");
	return result;
}

int DSContainer::GetChannelSizeOfComponent(int nbComp)
{
	int result = TypeTraits::getChannelSize(ComponentHeaderList[nbComp].m_elemType);
	if(result == -1)
		ErrorLogger::logError("DSContainer::GetNbChannelsOfComponent: this type is not supported by the TypeTraits class\n");
	return result;
}

int DSContainer::GetTypeSizeOfComponent(int nbComp)
{
	return GetNbChannelsOfComponent(nbComp)*GetChannelSizeOfComponent(nbComp);
}

const char* DSContainer::GetDataArray(int nbComp)
{
	if (ComponentHeaderList[nbComp].m_validComp != 1) 
	{
		ErrorLogger::logError("Invalid component number function: GetDataArray");
		return NULL;
	}

	return (const char*)ComponentHeaderList[nbComp].m_baseAddress;
}

#ifdef USE_OPENCL

void DSContainer::SetOpenCLKernelArg(int nbComp, cl::Kernel& kernel, unsigned int nbArg) {
	// Take the component pointer which must be of type cl_mem and forward the work to the OpenCLManager.
	OpenCLManager::getInstance()->setKernelArg(kernel, nbArg, (cl_mem)ComponentHeaderList[nbComp].m_base_address);
}

cl::ImageFormat DSContainer::CreateOpenCLImageFormat(enum ComponentType elemType) {
	cl::ImageFormat fmt;

	switch (elemType) {
	// For the non-IMAGE_ types that are 1D, it is usually better to choose opencl_buffer than opencl_image_type. But on older GPUs this disable the (texture) cache and may be slower.
	case TYPE_BYTE: // treat this as unsigned to vary from TYPE_CHAR
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_UNSIGNED_INT8;
		break;
	case TYPE_FLOAT:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_DOUBLE: // not supp
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = /*CL_DOUBLE*/CL_FLOAT;
		ErrorLogger::logError("DSContainer::CreateOpenCLImageFormat: element type double not supported (yet) for OpenCL images\n");
		break;
	case TYPE_LONG: // not supp, at least if considered 64 bit
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = /*CL_SIGNED_INT64*/CL_SIGNED_INT32;
		ErrorLogger::logError("DSContainer::CreateOpenCLImageFormat: element type LONG (64 bits) not supported (yet) for OpenCL images\n");
		break;
	case TYPE_CHAR: // treat this as signed to vary from TYPE_BYTE
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_SIGNED_INT8;
		break;
	case TYPE_INT:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_SIGNED_INT32;
		break;
	case TYPE_FFT:
		fmt.image_channel_order = CL_RGBA; // wastes one component, because TypeTraits says 3 floats?!?! and there's no module using it. (real, complex) is 2 in my world
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_GRAY:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_SIGNED_INT32; // judging from various modules, this should be int, but (u)int16 would be fine, also for 10 and 12 bit
		break;
	case TYPE_IMAGE_RGB:
		fmt.image_channel_order = CL_RGBA; // wastes one component; CL_RGB requires 565, 555 or normalized x-10-10-10 bit fmt
		fmt.image_channel_data_type = CL_SIGNED_INT32; // judging from various modules, this should be int32
		break;
	case TYPE_IMAGE_XYZ:
		fmt.image_channel_order = CL_RGBA; // wastes one component
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_XYZR:
		fmt.image_channel_order = CL_RGBA; // the 'A' component stores the rod channels ('R') data.
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_Lab:
		fmt.image_channel_order = CL_RGBA; // wastes one component
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_AC1C2:
		fmt.image_channel_order = CL_RGBA; // wastes one component
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_JND1:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_JND3:
		fmt.image_channel_order = CL_RGBA; // wastes one component
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_ROC:
		fmt.image_channel_order = CL_RG;
		fmt.image_channel_data_type = CL_FLOAT;
		break;
	case TYPE_IMAGE_YUV: // judging from the YUV modules, it needs an int3. TypeTraits confirms.
		fmt.image_channel_order = CL_RGBA; // wastes one component, like stated earlier
		fmt.image_channel_data_type = CL_SIGNED_INT32;
		break;
	case TYPE_UNSIGNED_SHORT:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_UNSIGNED_INT16;
		break;
	case TYPE_UNSIGNED_INT:
		fmt.image_channel_order = CL_R;
		fmt.image_channel_data_type = CL_UNSIGNED_INT32;
		break;
	default:
		fmt.image_channel_order = CL_RGBA;
		fmt.image_channel_data_type = CL_SIGNED_INT32;
		ErrorLogger::logError("DSContainer::CreateOpenCLImageFormat: unknown element type\n");
	}

	return fmt;
}
#endif // USE_OPENCL