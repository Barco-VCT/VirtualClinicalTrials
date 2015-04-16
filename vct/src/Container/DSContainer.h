//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file DSContainer.h
 * \brief Implementation of the class DSContainer
 * \date 01-Dec-2006
 * (C) Copyright 2014 Barco NV and others.
 *
 * This software code is licensed to you under the Barco VCT Research License. The text of this license
 * is found in the file License.TXT which is available in the same repository, in absence of which you need
 * to contact Barco first before you are allowed to engage in any act of familiarization or use of whatever
 * kind. Any familiarization or use of whatever kind of this software implies acceptance of the pre-mentioned
 * license. You warrant that the modifications you make are “original” and are able to be subject to the provisions
 * of the pre-mentioned license. Any contribution containing “non-original” modification require the prior written
 * approval from Barco.
 *
 */

#ifndef DSCONTAINER_H
#define DSCONTAINER_H

#include <string>
#include <vector>
#include <map>

#ifdef USE_OPENCL
// This file is included widely; avoid including OpenCLManager.h with all the OpenCL headers.
namespace cl {
	class Kernel;
	struct ImageFormat;
	class Error;
}
#endif

/*!
 * \enum ComponentType
 * \brief color of the white point
 *		  Not used in the virtual image capture part
 *		  Used in the virtual display part
 *		  Used in the virtual observer part
 */
enum ComponentType
{
	TYPE_BYTE=0,
	TYPE_FLOAT=1,
	TYPE_DOUBLE=2,
	TYPE_LONG=3,
	TYPE_CHAR=4,
	TYPE_INT=5,

	TYPE_FFT=6, // 2 channels: Fast Fourier Transform, channel 1: Real or Amplitude part, channel 2: Imaginary or Phase part

	// Color space representation
	TYPE_IMAGE_GRAY=7,	// 1 channel: Gray
	TYPE_IMAGE_RGB=8,	// 3 channels: Red Green Blue
	TYPE_IMAGE_XYZ=9,	// 3 channels, X Y Z channels in absolute cd/m2
	TYPE_IMAGE_XYZR=10,	// 4 channels, X Y Z channels in absolute cd/m2, R: rod channels
	TYPE_IMAGE_LMS=11,	// 3 channels, L (long), M (medium), S (short) cone response 
	TYPE_IMAGE_LMSR=12,	// 4 channels, L (long), M (medium), S (short) cone response and R (rods) rod response
	TYPE_IMAGE_Lab=13,	// from CIE (Commmission Internationale de l'Eclairage), L (Luminance), a (contrast Red/Green), b (contrast blue/Yellow)
	TYPE_IMAGE_AC1C2=14, // 3 channels, antagonist color space: A (Achromatic), C1 (contrast Red/Green), C2 (contrast blue/Yellow)

	TYPE_IMAGE_JND1=15,	// 1 channel = 1 map for JNDmetrix result, Just Noticeable Differences
	TYPE_IMAGE_JND3=16,	// 3 channels = 3 maps for JNDmetrix result, Just Noticeable Differences
	TYPE_IMAGE_ROC=17,	// output of the Channelized Hotelling Observer: ROC curve  (contains X and Y component)

	TYPE_IMAGE_YUV=18,	// 3 channels: Y U V
	TYPE_UNSIGNED_SHORT=19, // 1 channel: Gray 16bit integer
	TYPE_UNSIGNED_INT=20, // 1 channel: Gray 32 bit integer
};

/*!
 * \enum ILLUMINANT
 * \brief color of the white point
 *		  Not used in the virtual image capture part
 *		  Used in the virtual display part
 *		  Used in the virtual observer part
 */
enum ILLUMINANT {
	no_illuminant=0,
	A=1,
	B=2,
	C=3,
	D50=4,
	D55=5,
	D65=6,
	D75=7,
	F2=8,
	F7=9,
	F11=10
};

/*!
 * \enum OBSERVER
 * \brief Observer parameter defined by the CIE 31 or 76, 2 deg. or 10 deg. in the fovea
 *		  Not used in the virtual image capture part
 *		  Not used in the virtual display part
 *		  Used in the virtual observer part
 */
enum OBSERVER {
	no_observer=0,
	deg_2=2,
	deg_10=10
};

/*!
 * \enum UNIT
 * \brief Unit of the pixel
 */
enum UNIT {
	no_unit=0,
	cd_m2=1,
	contrast_ratio=2,
	video_level=3,
	kelvin=4,
	degrees_celsius=5,
	percent=6,
	JND=7
};

/*!
 * \enum AllocatorType
 * \brief What memory allocator is used for base_address.
 */
enum AllocatorType {
	default_allocator	= 0
#ifdef USE_OPENCL
	,
	opencl_buffer		= 1,	// cl::Buffer object (wrapped ptr). Use this for 1D buffers, or nD if you do the little coord math yourself, or if your image fmt is not supp.
	opencl_image2D		= 2,	// cl::Image2D object (idem). Use this for buffers with 2D spatial access locality, or if you need image interpol / coord clamping support.
	opencl_image3D		= 3		// cl::Image3D object (idem). Idem, 3D spatial access locality, etc.
#endif
};


/*!
 * \class ComponentHeader
 * \brief this class store all the information about a component
 */ 
class ComponentHeader 
{ 
	public:
		/*!
 		 * Constructor
 		 */ 
		ComponentHeader();

		/*! 
 		 * Destructor
 		 */ 
		~ComponentHeader();

		static const size_t DESCRIPTION_SIZE = 1024; //!< description size 1024 characters
		
		// component header attributes
		unsigned char * m_baseAddress; //!< raw data
		enum AllocatorType m_allocType;
		unsigned long m_length;
		unsigned long m_nbElemPerFrame;
		enum ComponentType m_elemType;
		char m_description[DESCRIPTION_SIZE];
		int m_validComp;
		float m_frameRate; //!< m_frameRate=0 means not a video, static image
		unsigned int m_nbFrames;
		unsigned long m_nbRowElemPerFrame; //!< height
		unsigned long m_nbColumnElemPerFrame; //!< width
		unsigned int m_nbBits; //!< 8 10 12 ...
		float m_elemSizeCol; //!< in mm
		float m_elemSizeRow; //!< in mm
		enum UNIT m_unit; //!< cd/m2, gray, JND
		float m_whitePt[3]; //!< Xo, Yo, Zo in cd/m2		
		enum ILLUMINANT m_illum; //!< A, B, C....D50...F11
		enum OBSERVER m_obs; //!< no_observer, deg_2, deg_10
		int m_elemSize;

	
	};

/*!
 * \class DSContainer
 * \brief This class store a list of components
 *		  and allow to manage them
 *		  There are 4 possibility to create a component:
 *			- with default component values
 *			- with all component values using enum
 *			- with all component values without using enum 
 *			- from an existing component (used to transfer components between different memory spaces (OpenCL))
 */ 
class DSContainer 
{

	public:
		/*! 
 		 * Constructor
 		 */ 
		DSContainer();
		
		/*!
 		 * Destructor
 		 */ 
		~DSContainer();

		/*! 
		 * \fn SaveToFile
		 * \brief method for saving file without compression
		 * \param filename: the filename to save
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SaveToFile (const std::string& filename);

		/*! 
		 * \fn SaveToFileWithCompression
		 * \brief method for saving file with compression
		 * \param filename: the filename to save
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SaveToFileWithCompression (const std::string& filename);

		/*! 
		 * \fn SaveToFileWithCompressionLastComponent
		 * \brief save only last component with compression
		 * \param filename: the filename to save
		 * \return int: 0 if ok, -1 otherwise
 		 */
		int SaveToFileWithCompressionLastComponent (const std::string& filename);

		/*!
		 * \fn LoadFromFile
		 * \bried for container loading from file without compression
		 * \param filename: the filename to load
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int LoadFromFile (const std::string& filename);

		/*!
		 * \fn LoadFromFile
		 * \bried for container loading from file with compression
		 * \param filename: the filename to load
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int LoadFromFileWithCompression (const std::string& filename);

		/*!
		 * \fn GetNumberOfComponents
 		 * \brief accessor for getting the number of container components
		 * \return int: total number of components
		 */
		int GetNumberOfComponents();
		
		/*!
		 * \fn CreateComponent
		 * \brief function to create a component without enum information
		 * \param nbRowElemPerFrame: nb row per frame
		 * \param nbColumnElemPerFrame: nb column per frame
		 * \param elemSizeCol: the size of a column
		 * \param elemSizeRow: the size of a row
		 * \param elemType: the enum type of element
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, enum ComponentType elemType);

		/*!
		 * \fn CreateComponent
		 * \brief Method CreateComponent using char*
		 *		  Function to create a component
		 * \param nbRowElemPerFrame: nb row per frame
		 * \param nbColumnElemPerFrame: nb column per frame
		 * \param elemSizeCol: the size of a column
		 * \param elemSizeRow: the size of a row
		 * \param unit: the enum unit of the component
		 * \param nbBits: the number of bits of the component (8, 16, 24...)
		 * \param whitePt: X0 Y0 Z0 values of the white point
		 * \param illum: the enum illuminant
		 * \param obs: the enum observer
		 * \param elemType: the enum type of element
		 * \param nbFrames: the number of frames
		 * \param frameRate: the m_frameRate if we work on a video
		 * \param allocator_type: type of memory where the data is stored (optional)
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, 
							float elemSizeCol, float elemSizeRow, 
							const std::string& unit, 
							unsigned int nbBits, 
							const float whitePt[3], 
							const std::string& illum, 
							const std::string& obs, 
							enum ComponentType elemType, 
							unsigned int nbFrames,
							float frameRate,
							enum AllocatorType allocator_type = default_allocator
							);

		/*!
		 * \fn CreateComponent
		 * \brief function to create a component using enum
		 * \param nbRowElemPerFrame: nb row per frame
		 * \param nbColumnElemPerFrame: nb column per frame
		 * \param elemSizeCol: the size of a column
		 * \param elemSizeRow: the size of a row
		 * \param unit: the enum unit of the component
		 * \param nbBits: the number of bits of the component (8, 16, 24...)
		 * \param whitePt: X0 Y0 Z0 values of the white point
		 * \param illum: the enum illuminant
		 * \param obs: the enum observer
		 * \param elemType: the enum type of element
		 * \param nbFrames: the number of frames
		 * \param frameRate: the m_frameRate if we work on a video
		 * \param allocType: type of memory where the data is stored (optional)
		 * \return int: 0 if ok, -1 otherwise
		 */
		int CreateComponent(unsigned long nbRowElemPerFrame, unsigned long nbColumnElemPerFrame, 
							float elemSizeCol, float elemSizeRow, 
							enum UNIT unit,
							unsigned int nbBits, 
							const float whitePt[3], 
							enum ILLUMINANT illum, 
							enum OBSERVER obs, 
							enum ComponentType elemType,
							unsigned int nbFrames,
							float frameRate,
							enum AllocatorType allocType = default_allocator
							);

		/*!
		 * \fn CreateComponent
		 * \brief function to create a component with default values
		 * \param nbComp: the number of the component to create from
		 * \param targetAllocator: the allocator to use for the new component
		 * \return int: 0 if ok, -1 otherwise
		 */
		int CreateComponent(int nbComp, enum AllocatorType targetAllocator);

		/*!
		 * \fn DeleteComponentData
		 * \brief Delete component data and set it to invalid.
		 * \param nbComp: the number of the component
		 * \return void
		 */
		void DeleteComponentData(int nbComp);

		/*!
		 * \fn GetComponentAllocatorTyp
		 * \brief accessor for getting the allocator type
		 * \param nbComp: the number of the component
		 * \param atype: output parameter with the allocator type of the component data
		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentAllocatorType(int nbComp, enum AllocatorType& atype);

		/*!
		 * \fn GetComponentDescription
		 * \brief accessor for getting the component description
		 * \param nbComp: the number of the component
		 * \param description: the description of the component to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentDescription(int nbComp, std::string& description);

		/*!
		 * \fn SetComponentDescription
		 * \brief accessor for setting the component description
		 * \param nbComp: the number of the component
		 * \param description: the description of the component to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentDescription(int nbComp, const std::string& description);

		/*!
		 * \fn GetComponentElementType
		 * \brief accessor for getting the component element type
		 * \param nbComp: the number of the component
		 * \param element_type: the element type to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentElementType(int nbComp, enum ComponentType* element_type);

		/*!
		 * \fn GetComponentElement
		 * \brief accessor for getting a component element
		 * \param nbComp: the number of the component
		 * \param nbElem: element number to set
		 * \param nbFrame: the number of the frame
		 * \param element: the element to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentElement(int nbComp, int nbElement, int nbFrame, void* element);
		
		/*!
		 * \fn SetComponentElement
		 * \brief accessor for setting the value of a component element
		 * \param nbComp: the number of the component
		 * \param nbElem: element number to set
		 * \param nbFrame: the number of the frame
		 * \param element: the element value to set
 		 * \return int: 0 if ok, -1 otherwise
		 */		
		int SetComponentElement(int nbComp, int nbElem, unsigned int nbFrame, void* element);

		/*!
		 * \fn GetComponentNumberOfBits
		 * \brief accessor for getting the number of bits of a compenent
		 * \param nbComp: the number of the component
		 * \param nbBits: the number of bits to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentNumberOfBits(int nbComp, unsigned int* nbBits);

		/*! 
		 * \fn SetComponentNumberOfBits
		 * \brief accessor for setting the value of number of bits of a component
		 * \param nbComp: the number of the component
		 * \param nbBits: the number of bits to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentNumberOfBits(int nbComp, unsigned int* nbBits);

		/*!
		 * \fn GetComponentNumberOfRowElementsPerFrame
		 * \brief accessor for getting the height of a component (number of rows per frame)
		 * \param nbComp: the number of the component
		 * \param nbRowElems: the number of row elements per frame to get (height)
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentNumberOfRowElementsPerFrame(int nbComp, unsigned long * nbRowElems);

		/*!
		 * \fn GetComponentNumberOfColumnElementsPerFrame
		 * \brief accessor for getting the width of a component (number of columns per frame)
		 * \param nbComp: the number of the component
		 * \param nbColElems: the number of columns elements per frame to get (width)
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentNumberOfColumnElementsPerFrame(int nbComp, unsigned long* nbColElems);

		/*!
		 * \fn GetComponentElementSizeColumn
		 * \brief accessor for getting the element size column direction in mm
		 * \param nbComp: the number of the component
		 * \param elemSizeCol: the element size column to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentElementSizeColumn(int nbComp, float* elemSizeCol);
		
		/*!
		 * \fn SetComponentElementSizeColumn
		 * \brief accessor for setting the element size column direction in mm
		 * \param nbComp: the number of the component
		 * \param elemSizeCol: the element size column to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentElementSizeColumn(int nbComp, float* elemSizeCol);

		/*!
		 * \fn GetComponentElementSizeRow
		 * \brief accessor for getting the element size row direction in mm
		 * \param nbComp: the number of the component
		 * \param elemSizeRow: the element size row to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentElementSizeRow(int nbComp, float* elemSizeRow);

		/** 
		 * \fn SetComponentElementSizeRow
		 * \brief accessor for setting the element size row direction in mm
		 * \param nbComp: the number of the component
		 * \param elemSizeRow: the element size row to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentElementSizeRow(int nbComp, float* elemSizeRow);

		/*!
		 * \fn GetComponentIlluminant
		 * \brief accessor for getting the component illuminant
		 * \param nbComp: the number of the component
		 * \param illum: the string illuminant component to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentIlluminant(int nbComp, std::string& illum);
				
		/*!
		 * \fn SetComponentIlluminant
		 * \brief accessor for setting the value of the component illuminant
		 * \param nbComp: the number of the component
		 * \param illum: the string illuminant component to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentIlluminant(int nbComp, const std::string& illum);
		
		/*! 
		 * \fn GetComponentObserver
		 * \brief accessor for getting the component observer value
		 * \param nbComp: the number of the component
		 * \param observer: the string observer to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentObserver(int nbComp, std::string& observer);

		/*!
		 * \fn SetComponentObserver
		 * \brief accessor for setting the component observer value
		 * \param nbComp: the number of the component
		 * \param observer: the string observer to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentObserver(int nbComp, const std::string& observer);

		/*!
		 * \fn GetComponentUnit
		 * \brief accessor for getting the component unit
		 * \param nbComp: the number of the component
		 * \param unit: the string unit to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentUnit(int nbComp, std::string& unit);

		/*!
		 * \fn SetComponentUnit
		 * \brief accessor for setting the component unit value
		 * \param nbComp: the number of the component
		 * \param unit: the string unit to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentUnit(int nbComp, const std::string& unit);

		/*!
		 * \fn GetComponentWhitePoint
		 * \brief accessor for getting the component white point value
		 * \param nbComp: the number of the component
		 * \param whitePt: the white point to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentWhitePoint(int nbComp, float* whitePt);
		
		/*!
		 * \fn SetComponentWhitePoint
		 * \brief accessor for setting the component white point value
		 * \param nbComp: the number of the component
		 * \param whitePt: the white point to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentWhitePoint(int nbComp, float* whitePt);

		/*! 
		 * \fn GetComponentNumberOfFrames
		 * \brief accessor for getting the component number of frames
		 * \param nbComp: the number of the component
		 * \param nbFrames: the number of frames to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentNumberOfFrames(int nbComp, unsigned int* nbFrames);

		/*!
		 * \fn GetComponentFrameRate
		 * \brief accessor for getting the component frame rate
		 * \param nbComp: the number of the component
		 * \param frameRate: the frame rate to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentFrameRate(int nbComp, float* frameRate);

		/*!
		 * \fn GetComponentNumberOfElementsPerFrame
		 * \brief accessor for getting the component number of elements per frame
		 * \param nbComp: the number of the component
		 * \param nbElemPerFrame: the number of elements per frame to get
 		 * \return int: 0 if ok, -1 otherwise
		 */	
		int GetComponentNumberOfElementsPerFrame(int nbComp, int* nbElemPerFrame);

		/*!
		 * \fn GetComponentLength
		 * \brief accessor for getting the component lenght
		 * \param nbComp: the number of the component
		 * \param length: the length of the component to get
 		 * \return int: 0 if ok, -1 otherwise
		 */	
		int GetComponentLength(int nbComp, int* length);

		/*!
		 * \fn GetComponentValid
		 * \brief accessor for checking if a component is valid
		 * \param nbComp: the number of the component
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetComponentValid(int nbComp);

		/*!
		 * \fn GetLastComponentNumber
 		 * \brief accessor for getting the number of the last component
		 * \return int: the number of the last component
		 */	
		int GetLastComponentNumber();

		/*!
		 * \fn GetComponentHeaderSize
 		 * \brief accessor for getting the component header size
		 * \return int: the size of the component header
		 */
		int GetComponentHeaderSize();

		/*!
		 * \fn GetContainerSize
		 * \brief accessor for getting the container size
		 * \return int: the size of the container
		 */
		int GetContainerSize();

		/*!
		 * \fn GetContainerDescription
		 * \brief accessor for getting the container description
		 * \param description: the description of the component to get
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int GetContainerDescription(std::string& description);

		/*!
		 * \fn SetContainerDescription
		 * \brief accessor for setting the container description
		 * \param description: the description of the component to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetContainerDescription(const std::string& description);

		/*!
		 * \fn SaveFrameBMP
		 * \brief function to save a frame of a component in bmp format
		 * \param nbComp: the number of the component
		 * \param frame_number: the frame number
		 * \param filename: output filename
		 * \param cond: condition for normalization
		 * \param nbChannels: number of channels
 		 * \return int: 0 if ok, -1 otherwise
		 */ 
		int SaveFrameBMP(int nbComp, int frame_number, const std::string& filename, bool cond, int nbChannels);
		
		/*!
		 * \fn initimageJmap
		 * \brief accessor for getting the map with parameters of ImageJ in case of raw file
		 * \return the map of the imageJ type function of the ComponentType
		 */
		std::map<int, char*> initimageJmap();
		
		/*!
 		 * \fn SaveFrameRAW. 
 		 * \brief function to save frame(s) of a component in raw format
		 * \param nbComp: the number of the component to process
		 * \param nbFrame: the frame of the component to write (-1 to write all frames)
		 * \param filename: the filename location to save
		 * \param channelToWrite: the channel of the component to write (if several, -1=all)
		 * \return bool: true if the process run well
 		 */ 
		bool SaveFrameRAW(int nbComp, int nbFrame, const std::string& filename, int channelToWrite);

		/*!
 		 * \fn SaveFrameTXT. 
 		 * \brief function to save frame(s) of a component in txt format
		 * \param nbComp: the number of the component to process
		 * \param nbFrame: the frame numer of the component to write
		 * \param filename: the save filename
		 * \param channelToWrite: the channel of the component to write (if several, 0=all)
		 * \return bool: true if the process run well
 		 */ 		
		bool SaveFrameTXT(int nbComp, int nbFrame, const std::string& filename, int channelToWrite);

		/*!
 		 * \fn Save1dPlotTXT. 
 		 * \brief function to save a 1D plot at a certain pixel position for all frames
		 * \param nbComp: the number of the component to process
		 * \param nbFrame: the frame of the component to write (-1 to write all frames)
		 * \param filename: the filename location to save
		 * \param chanelTowrite: the channel of the component to write (if several, -1=all)
		 * \param xy: x and y coordinates of the pixel to save
		 * \return bool: true if the process run well
 		 */ 
		bool Save1dPlotTXT(int nbComp, int nbFrame, const std::string& filename, int channelToWrite, int xy[2]);

		/*!
 		 * \fn GetNbChannel. 
 		 * \brief accessor for getting the number of channel for a certain element type
		 * \param element_type: the element type define in DSContainer.h
		 * \return int: the number of channel(-1 if invalid element type)
 		 */ 
		int GetNbChannel(enum ComponentType element_type);

		char GetComponentElementTypeChar(enum ComponentType element_type);
		
		/*!
		 * \fn GetNbChannelsOfComponent
		 * \brief accessor for getting the number of channels of a component
		 * \param nbComp the index of the component to access
		 * \return the number of channels of the component
		 */
		int GetNbChannelsOfComponent(int nbComp);
		
		/*!
		 * \fn GetChannelSizeOfComponent
		 * \brief accessor for getting the channel size of a component
		 * \param nbComp the index of the component to access
		 * \return the base size (in bytes) of the elements in the component (for only ONE channel)
		 */
		int GetChannelSizeOfComponent(int nbComp);
		
		/*!
		 * \fn GetTypeSizeOfComponent
		 * \brief accessor for getting the type size of a component
		 * \param nbComp: the index of the component to access
		 * \return the size (in bytes) of one element in the component (all channels combined)
		 */
		int GetTypeSizeOfComponent(int nbComp);

#ifdef USE_OPENCL
		/*!
		 * \fn SetOpenCLKernelArg
		 * \brief accessor for setting the OpenCL Kernel arguments
		 * \param nbComp the index of the component to access
		 * \param kernel: the kernel to set the component data as argument to
		 * \param nbArg: the number (zero-indexed) of the kernel argument
		 */
		void SetOpenCLKernelArg(int nbComp, cl::Kernel& kernel, unsigned int nbArg);

		/*!
		 * \fn CreateOpenCLImageFormat. 
		 * \brief maps MEVIC component types into OpenCL image element types.
		 * \param elemType: the type of image element
		 * \return cl::ImageFormat: image format that most accurately corresponds to elemType (or invokes an error handler)
		 */
		static cl::ImageFormat CreateOpenCLImageFormat(enum ComponentType elemType);
#endif

		static const size_t DESCRIPTION_SIZE = 1024; //!< description size of the container

		/*!
		 * \fn GetDataArray
		 * \brief accessor for getting pointer to data array for a compocnent
		 * \param nbComp the index of the component to access
		 * \return const char*: poitner to the data array of the component
		 */
		const char* GetDataArray(int nbComp);

	private:
		
		static const size_t MAX_NUMBER_COMPONENTS = 256; //!< maximum number of components per container simulation
		ComponentHeader ComponentHeaderList[MAX_NUMBER_COMPONENTS]; //!< component header of the container simulation

		enum ILLUMINANT GetIlluminant(const std::string& illum); //!< illuminant of the container simulation
		enum OBSERVER GetObserver(const std::string& inputstring); //!< observer of the container simulation
		enum UNIT GetUnit(const std::string& inputstring); //!< unit of the container simulation

		/*!
		 * \fn SetComponentNumberOfColumnElements
		 * \brief accessor for setting the number of elements column direction (width)
		 * \param nbComp: the number of the component
		 * \param nbColumnElemPerFrame: the number of column elements per frame to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentNumberOfColumnElements(int nbComp, unsigned long* nbColumnElemPerFrame);

		/*!
		 * \fn SetComponentNumberOfRowElements
		 * \brief accessor for setting the number of elements row direction (height)
		 * \param nbComp: the number of the component
		 * \param nbRowElemPerFrame: the number of row elements per frame to set
 		 * \return int: 0 if ok, -1 otherwise
		 */
		int SetComponentNumberOfRowElements(int nbComp, unsigned long* nbRowElemPerFrame);

		// container header
		std::string m_description; //!< container description

};

#endif // DSCONTAINER_H

