#include "UtilitiesDisplay.h"

#include <cmath>
#include <fstream>
#include <sstream>

#include <ErrorManagement/ErrorLogger.h>

using namespace std;

UtilitiesDisplay::UtilitiesDisplay(int nbBits,
                                   int frequency,
                                   int screenWidth,
                                   int screenHeight,
                                   int imageTopColumnPosition,
                                   int imageTopRowPosition,
                                   double pitchPixelColumnDirection,
                                   double pitchPixelRowDirection,
                                   int _1Color_0Monochrome)
:m_nbBits(nbBits),
 m_screenWidth(screenWidth),
 m_screenHeight(screenHeight),
 m_imageTopColumnPosition(imageTopColumnPosition),
 m_imageTopRowPosition(imageTopRowPosition),
 m_pitchPixelColumnDirection(pitchPixelColumnDirection),
 m_pitchPixelRowDirection(pitchPixelRowDirection),
 m_curveNbData((int)pow(float(2.0),float(nbBits))),
 m_frequency(frequency),
 m_1Color_0Monochrome(_1Color_0Monochrome),
 m_nativeCurveGray(0),
 m_nativeCurveRed(0),
 m_nativeCurveGreen(0),
 m_nativeCurveBlue(0),
 m_nativeCurveMagenta(0),
 m_nativeCurveCyan(0),
 m_nativeCurveYellow(0),
 m_cr(0),
 m_cg(0),
 m_cb(0),
 m_cc(0),
 m_cm(0),
 m_cy(0),
 m_ck(0)
{
	// Gray curve ------------------------------------------------
	m_nativeCurveGray= new float*[m_curveNbData]; //2^m_nbBits values
	for (int i=0; i<m_curveNbData; i++) m_nativeCurveGray[i]=new float [3];//for XYZ
	for (int i=0; i<m_curveNbData; i++)
		for (int j=0; j<3; j++)
			m_nativeCurveGray[i][j]=-1;
	//------------------------------------------------------------
	if (m_1Color_0Monochrome==1)
	{
		// Red curve -------------------------------------------------
		m_nativeCurveRed= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveRed[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveRed[i][j]=-1;
		//------------------------------------------------------------

		// Green curve -----------------------------------------------
		m_nativeCurveGreen= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveGreen[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveGreen[i][j]=-1;
		//------------------------------------------------------------
		
		// Blue curve ------------------------------------------------
		m_nativeCurveBlue= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveBlue[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveBlue[i][j]=-1;
		//------------------------------------------------------------

		// Cyan curve ------------------------------------------------
		m_nativeCurveCyan= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveCyan[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveCyan[i][j]=-1;
		//------------------------------------------------------------	
		
		// Magenta curve ---------------------------------------------
		m_nativeCurveMagenta= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveMagenta[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveMagenta[i][j]=-1;
		//------------------------------------------------------------	
			
		// Yellow curve ----------------------------------------------
		m_nativeCurveYellow= new float*[m_curveNbData]; //2^m_nbBits values
		for (int i=0; i<m_curveNbData; i++) m_nativeCurveYellow[i]=new float [3];//for XYZ
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveYellow[i][j]=-1;
		//------------------------------------------------------------	
	}
}	

void UtilitiesDisplay::coefficientProcess()
{
	//Coefficient processing
	m_offsetxyz[0] = m_nativeCurveRed[0][0];
	m_offsetxyz[1] = m_nativeCurveRed[0][1];
	m_offsetxyz[2] = m_nativeCurveRed[0][2];

	calcCoeffR();
	calcCoeffG();
	calcCoeffB();
	calcCoeffC();
	calcCoeffM();
	calcCoeffY();
	calcCoeffK();
}

bool UtilitiesDisplay::loadNativeCurve(const string & filename, RGBCMYK color)
{
	// Temporary curve
	float** tempCurve= new float*[m_curveNbData]; //2^m_nbBits values
	for (int i=0; i<m_curveNbData; i++) tempCurve[i]=new float [3];//for XYZ
	for (int i=0; i<m_curveNbData; i++)
		for (int j=0; j<3; j++)
			tempCurve[i][j]=-1;

	ifstream fCurve;
	fCurve.open(filename.c_str());
	if(!fCurve.good())
	{
		// Return
		return false;
	}

	try
	{
		char comma; // temporary dummy char to extract the seperator character comma (',') in the .csv file
		int dummy;
		float data[3];
		for (int i=0; i<m_curveNbData; i++) 
		{
			string line;
			stringstream sline;

			// one line of the .csv file
			getline(fCurve, line);
			sline << line;
			sline >> dummy >> comma >> data[0] >> comma >> data[1] >> comma >> data[2]; // white spaces are ignored since stream manipulator 'skipws' is on by default
			tempCurve[i][0]=data[0];
			tempCurve[i][1]=data[1];
			tempCurve[i][2]=data[2];
		}
		fCurve.close();
	}
	catch(...)
	{
		// Return
		return false;
	}

	//native curve storage
	switch(color)
	{
	case GRAY:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveGray[i][j]=tempCurve[i][j];
		break;
	case RED:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveRed[i][j]=tempCurve[i][j];
		break;
	case GREEN:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveGreen[i][j]=tempCurve[i][j];
		break;
	case BLUE:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveBlue[i][j]=tempCurve[i][j];
		break;
	case CYAN:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveCyan[i][j]=tempCurve[i][j];
		break;
	case MAGENTA:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveMagenta[i][j]=tempCurve[i][j];
		break;
	case YELLOW:
		for (int i=0; i<m_curveNbData; i++)
			for (int j=0; j<3; j++)
				m_nativeCurveYellow[i][j]=tempCurve[i][j];
		break;
	default:
		ErrorLogger::logError("UtilitiesDisplay::loadNativeCurve - Unreferenced native curve");
		break;
	}

	// memory clean-up
	for (int i=0; i<m_curveNbData; i++)
	{
		delete [] tempCurve[i];
	}
	delete [] tempCurve;

	// Return
	return true;
}

int UtilitiesDisplay::getDDL_sup(int nb, int step) 
{
	return	nb + (step-nb%step);
}

int UtilitiesDisplay::getDDL_inf(int nb, int step) 
{
	return nb - nb%step;
}

void UtilitiesDisplay::getDDL (int nb, int *inf, int *sup, int step) 
{
	*inf = getDDL_inf(nb, step);
	*sup = getDDL_sup(nb, step);
}

int UtilitiesDisplay::getMaxDDLLevelValue ()
{
	return ((int)pow(float(2),float(getNbBits()))-1);
}
int UtilitiesDisplay::return_ddl_value_BinarySearch(float value, int chanXYZ, RGBCMYK curve) 
{
	// dichotomic process to find the least difference
	int low = 0;
	int high = m_curveNbData - 1;
	int mid=0;

	float difference_ = value;

	float ** nativeCurve = getNativeCurve(curve);

	//dichotomic loop
	while (low <= high) 
	{
		mid = (int)((low + high) *0.5);
		difference_=nativeCurve[mid][chanXYZ]-value;

		if (difference_ ==0)
			break;
		else if (difference_ > 0)
			high = mid - 1;
		else if (difference_ < 0)
			low = mid + 1;
	}

	if (mid<0 || mid>m_curveNbData-1) 
	{
		ErrorLogger::logError("UtilitiesDisplay::return_ddl_value_BinarySearch failed");
		mid=-1;
	}	

	return mid;
}

float** UtilitiesDisplay::getNativeCurve(RGBCMYK curve)
	{
		switch(curve)
		{
		case RED:
			return m_nativeCurveRed;
			break;
		case GREEN:
			return m_nativeCurveGreen;
			break;
		case BLUE:
			return m_nativeCurveBlue;
			break;
		case CYAN:
			return m_nativeCurveCyan;
			break;
		case MAGENTA:
			return m_nativeCurveMagenta;
			break;
		case YELLOW:
			return m_nativeCurveYellow;
			break;
		case GRAY:
			return m_nativeCurveGray;
			break;
		default:
			ErrorLogger::logError("UtilitiesDisplay::getNativeCurve - Unreferenced native curve.");
			return m_nativeCurveGray;
			break;
		}
	}
int UtilitiesDisplay::getNbBits()
{
	return m_nbBits;
}

int UtilitiesDisplay::getScreenWidth()
{
	return m_screenWidth;
}

/** 
* function getscreenHeight
* @return int: the screen height in number of pixels
*/
int UtilitiesDisplay::getScreenHeight()
{
	return m_screenHeight;
}

/** 
 * function getImageTopColumnPosition
 * @return int: the top leftposition of the simulated image in terms of column number starting from "0" until "m_screenWidth-1": unit = pixel
 */
int UtilitiesDisplay::getImageTopColumnPosition()
{
	return m_imageTopColumnPosition;
}

/** 
 * function getImageTopRowPosition
 * @return int: the top leftposition of the simulated image in terms of row number starting from "0" until "m_screenHeight-1": unit = pixel
 */
int UtilitiesDisplay::getImageTopRowPosition()
{
	return m_imageTopRowPosition;
}

/** 
 * function PitchPixelColumnDirection
 * @return double: pixel pitch in mm in column direction
 */
double UtilitiesDisplay::getPitchPixelColumnDirection()
{
	return m_pitchPixelColumnDirection;
}

/** 
 * function PitchPixelRowDirection
 * @return double: pixel pitch in mm in row direction
 */
double UtilitiesDisplay::getPitchPixelRowDirection()
{
	return m_pitchPixelRowDirection;
}

/** 
 * function getCurveNbData
 * @return int: the nb data in the curve
 */	
int UtilitiesDisplay::getCurveNbData()
{
	return m_curveNbData;
}

/** 
 * function getFrequency
 * @return int: the frequency of the display
 */	
int UtilitiesDisplay::getFrequency()
{
	return m_frequency;
}

/** 
 * Function JND2lum
 * function to transform JND value in luminance value (cd/m²)
 * @param j: the JND value
 * @return double: the luminance value
 */ 
double UtilitiesDisplay::JND2lum(double j) 
{
	double a= -1.3011877   ;
	double b= -2.5840191E-2;
	double c=  8.0242636E-2;
	double d= -1.0320229E-1;
	double e=  1.3646699E-1;
	double f=  2.8745620E-2;
	double g= -2.5468404E-2;
	double h= -3.1978977E-3;
	double k=  1.2992634E-4;
	double m=  1.3635334E-3;

	double temp= 0.0;
	// log10 L= ...
	temp=	(a+ c*log(j)+ e*(log(j)*log(j))+ g*(log(j)*log(j)*log(j))+ m*(log(j)*log(j)*log(j)*log(j)))
			/
			(1.0 + b*log(j)+ d*(log(j)*log(j))+ f*(log(j)*log(j)*log(j))+ h*(log(j)*log(j)*log(j)*log(j))+ k*(log(j)*log(j)*log(j)*log(j)*log(j)))
		;

	temp= pow(10.0, temp);
	return temp;
}

/** 
 * Function lum2JND
 * function to transform luminance value (cd/m²) in JND value
 * @param L: the luminance value
 * @return double: the JND value
 */ 
double UtilitiesDisplay::lum2JND(double L) 
{
	double A= 71.498068;
	double B= 94.593053;
	double C= 41.912053;
	double D= 9.8247004;
	double E= 0.28175407;
	double F= -1.1878455;
	double G= -0.18014349;
	double H= 0.14710899;
	double I= -0.017046845;
	
	double temp=0.0;

	temp=	A+ B*log10(L)+ C*(log10(L)*log10(L))+ 
			D*(log10(L)*log10(L)*log10(L))+
			E*(log10(L)*log10(L)*log10(L)*log10(L))+
			F*(log10(L)*log10(L)*log10(L)*log10(L)*log10(L))+
			G*(log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L))+
			H*(log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L))+
			I*(log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L)*log10(L));

	return temp;
}

/** 
 * Function JNDinInterval
 * function to get the JND in interval
 * @param L1: the first luminance value
 * @param L2: the second luminance value
 * @return double: the JND interval
 */ 
double UtilitiesDisplay::JNDinInterval(double L1, double L2) 
{
	return (lum2JND(L2)-lum2JND(L1));
}

/**
 * Function fromRgbToXyz
 * function to transform RGB data into XYZ data
 * @param r: the red ddl value 
 * @param g: the green ddl value
 * @param b: the blue ddl value
 * @return float*: XYZ values in an array of 3 floats (0=X, 1=Y, 2=Z)
 */ 
vector<float>/*float**/ UtilitiesDisplay::fromRgbToXyz(unsigned int r, unsigned int g, unsigned int b)
{
	float X, Y, Z;
	if(r > g && r > b)
		if(g > b) // red - yellow - gray
		{
			unsigned end_k = b;
			unsigned start_y = b;
			unsigned end_y = g;
			unsigned start_r = g;
			unsigned end_r = r;
			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,0) - calcApproxIntensity(m_cy,m_yxyz,start_y,0)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,0) - calcApproxIntensity(m_cr,m_rxyz,start_r,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,1) - calcApproxIntensity(m_cy,m_yxyz,start_y,1)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,1) - calcApproxIntensity(m_cr,m_rxyz,start_r,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,2) - calcApproxIntensity(m_cy,m_yxyz,start_y,2)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,2) - calcApproxIntensity(m_cr,m_rxyz,start_r,2);
		}
		else // red - magenta - gray
		{
			unsigned end_k = g;
			unsigned start_m = g;
			unsigned end_m = b;
			unsigned start_r = b;
			unsigned end_r = r;

			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,0) - calcApproxIntensity(m_cm,m_mxyz,start_m,0)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,0) - calcApproxIntensity(m_cr,m_rxyz,start_r,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,1) - calcApproxIntensity(m_cm,m_mxyz,start_m,1)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,1) - calcApproxIntensity(m_cr,m_rxyz,start_r,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,2) - calcApproxIntensity(m_cm,m_mxyz,start_m,2)
				+ calcApproxIntensity(m_cr,m_rxyz,end_r,2) - calcApproxIntensity(m_cr,m_rxyz,start_r,2);
		}
	else if(g > b)
		if(r > b) // green - yellow - gray
		{
			unsigned end_k = b;
			unsigned start_y = b;
			unsigned end_y = r;
			unsigned start_g = r;
			unsigned end_g = g;
			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,0) - calcApproxIntensity(m_cy,m_yxyz,start_y,0)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,0) - calcApproxIntensity(m_cg,m_gxyz,start_g,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,1) - calcApproxIntensity(m_cy,m_yxyz,start_y,1)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,1) - calcApproxIntensity(m_cg,m_gxyz,start_g,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cy,m_yxyz,end_y,2) - calcApproxIntensity(m_cy,m_yxyz,start_y,2)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,2) - calcApproxIntensity(m_cg,m_gxyz,start_g,2);
		}
		else // green - cyan - gray
		{
			unsigned end_k = r;
			unsigned start_c = r;
			unsigned end_c = b;
			unsigned start_g = b;
			unsigned end_g = g;
			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,0) - calcApproxIntensity(m_cc,m_cxyz,start_c,0)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,0) - calcApproxIntensity(m_cg,m_gxyz,start_g,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,1) - calcApproxIntensity(m_cc,m_cxyz,start_c,1)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,1) - calcApproxIntensity(m_cg,m_gxyz,start_g,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,2) - calcApproxIntensity(m_cc,m_cxyz,start_c,2)
				+ calcApproxIntensity(m_cg,m_gxyz,end_g,2) - calcApproxIntensity(m_cg,m_gxyz,start_g,2);
		}
	else
		if(r > g) // blue - magenta - gray
		{
			unsigned end_k = g;
			unsigned start_m = g;
			unsigned end_m = r;
			unsigned start_b = r;
			unsigned end_b = b;
			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,0) - calcApproxIntensity(m_cm,m_mxyz,start_m,0)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,0) - calcApproxIntensity(m_cb,m_bxyz,start_b,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,1) - calcApproxIntensity(m_cm,m_mxyz,start_m,1)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,1) - calcApproxIntensity(m_cb,m_bxyz,start_b,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cm,m_mxyz,end_m,2) - calcApproxIntensity(m_cm,m_mxyz,start_m,2)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,2) - calcApproxIntensity(m_cb,m_bxyz,start_b,2);
		}
		else // blue - cyan - gray
		{
			unsigned end_k = r;
			unsigned start_c = r;
			unsigned end_c = g;
			unsigned start_b = g;
			unsigned end_b = b;
			X = m_offsetxyz[0] + calcApproxIntensity(m_ck,m_wxyz,end_k,0)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,0) - calcApproxIntensity(m_cc,m_cxyz,start_c,0)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,0) - calcApproxIntensity(m_cb,m_bxyz,start_b,0);
			Y = m_offsetxyz[1] + calcApproxIntensity(m_ck,m_wxyz,end_k,1)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,1) - calcApproxIntensity(m_cc,m_cxyz,start_c,1)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,1) - calcApproxIntensity(m_cb,m_bxyz,start_b,1);
			Z = m_offsetxyz[2] + calcApproxIntensity(m_ck,m_wxyz,end_k,2)
				+ calcApproxIntensity(m_cc,m_cxyz,end_c,2) - calcApproxIntensity(m_cc,m_cxyz,start_c,2)
				+ calcApproxIntensity(m_cb,m_bxyz,end_b,2) - calcApproxIntensity(m_cb,m_bxyz,start_b,2);
		}

	/*float* XYZ=new float[3];*/
	vector<float> XYZ(3);
	XYZ[0]=X;
	XYZ[1]=Y;
	XYZ[2]=Z;

	return XYZ;
}

//private functions
//--------------------------------------//
/** 
 * method calcCoeffK
 * method to calculate gray coefficients
 */ 
void UtilitiesDisplay::calcCoeffK()
{

	m_ck=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveGray[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_ck,m_wxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffR
 * method to calculate red coefficients
 */ 
void UtilitiesDisplay::calcCoeffR()
{
	m_cr=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveRed[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cr,m_rxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffG
 * method to calculate green coefficients
 */ 
void UtilitiesDisplay::calcCoeffG()
{
	m_cg=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveGreen[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cg,m_gxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffB
 * method to calculate blue coefficients
 */ 
void UtilitiesDisplay::calcCoeffB()
{
	m_cb=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveBlue[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cb,m_bxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffM
 * method to calculate magenta coefficients
 */ 
void UtilitiesDisplay::calcCoeffM()
{
	m_cm=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveMagenta[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cm,m_mxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffC
 * method to calculate cyan coefficients
 */ 
void UtilitiesDisplay::calcCoeffC()
{
	m_cc=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveCyan[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cc,m_cxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffY
 * method to calculate yellow coefficients
 */ 
void UtilitiesDisplay::calcCoeffY()
{
	m_cy=new float[m_curveNbData*2];

	CvMat** vects = new CvMat*[m_curveNbData];
	for(int i=0;i<m_curveNbData;i++)
	{
		vects[i] = cvCreateMat(1,3,CV_32FC1);
		for(int j=0;j<3;j++)
		{
			cvSetReal1D(vects[i],j,m_nativeCurveYellow[i][j]);
		}
	}

	calcCoeffBasedOnEigenVectors((const CvMat**)vects,m_cy,m_yxyz);

	for(int i=0;i<m_curveNbData;i++)
	{
		cvReleaseMat(&vects[i]);
	}
	delete vects;
}

/** 
 * method calcCoeffBasedOnEigenVectors
 * method to process parameters reduction based on eight vectors
 * @param vects: the input vector
 * @param coeff: the output coefficient
 * @param pvector: the output pVector
 */ 
void UtilitiesDisplay::calcCoeffBasedOnEigenVectors(const CvMat ** vects, float coeff[], float pVector[])
{
	CvMat* cov = cvCreateMat(3,3,CV_32FC1);
	CvMat* avg = cvCreateMat(1,3,CV_32FC1);
	cvCalcCovarMatrix((const CvArr**)vects,m_curveNbData,cov,avg,CV_COVAR_NORMAL||CV_COVAR_SCALE);

	CvMat* eigenVec = cvCreateMat(3,3,CV_32FC1);
	CvMat* eigenVal = cvCreateMat(3,3,CV_32FC1);
	CvMat* tempMat = cvCreateMat(3,3,CV_32FC1);
	cvSVD(cov,eigenVal,eigenVec,tempMat,CV_SVD_MODIFY_A||CV_SVD_U_T);
	cvReleaseMat(&tempMat);

	unsigned principalComponentIndex = 0;
	float principalComponentEigenValue = (float)cvGetReal2D(eigenVal,0,0);
	for(unsigned i=1; i<3; ++i)
	{
		float currentComponentEigenValue = (float)cvGetReal2D(eigenVal,i,i);
		if(currentComponentEigenValue > principalComponentEigenValue)
		{
			principalComponentEigenValue = currentComponentEigenValue;
			principalComponentIndex = i;
		}
	}

	unsigned secondComponentIndex = (principalComponentIndex == 0 ? 1 : 0);
	float secondComponentEigenValue = (float)cvGetReal2D(eigenVal,secondComponentIndex,secondComponentIndex);
	for(unsigned i=0; i<3; ++i)
	{
		if(i != principalComponentIndex)
		{
			float currentComponentEigenValue = (float)cvGetReal2D(eigenVal,i,i);
			if(currentComponentEigenValue > secondComponentEigenValue)
			{
				secondComponentEigenValue = currentComponentEigenValue;
				secondComponentIndex = i;
			}
		}
	}

	pVector[0] = (float)cvGetReal2D(eigenVec,0,principalComponentIndex);
	pVector[1] = (float)cvGetReal2D(eigenVec,1,principalComponentIndex);
	pVector[2] = (float)cvGetReal2D(eigenVec,2,principalComponentIndex);
	pVector[3] = (float)cvGetReal2D(eigenVec,0,secondComponentIndex);
	pVector[4] = (float)cvGetReal2D(eigenVec,1,secondComponentIndex);
	pVector[5] = (float)cvGetReal2D(eigenVec,2,secondComponentIndex);

	CvMat* cvPVector = cvCreateMat(1,3,CV_32FC1);
	CvMat* cvSVector = cvCreateMat(1,3,CV_32FC1);
	for(unsigned i=0;i<3;++i)
	{
		cvSetReal1D(cvPVector,i,pVector[i]);
		cvSetReal1D(cvSVector,i,pVector[i+3]);
	}
	
	CvMat* offset = cvCreateMat(1,3,CV_32FC1);
	CvMat* difference = cvCreateMat(1,3,CV_32FC1);

	for(unsigned i=0;i<3;++i)
		cvSetReal1D(offset,i,m_nativeCurveRed[0][i]);

	for(int i=0;i<m_curveNbData;++i)
	{
		cvSub(vects[i],offset,difference);
		coeff[i] = (float)cvDotProduct(difference,cvPVector);
		coeff[i+m_curveNbData] = (float)cvDotProduct(difference,cvSVector);
	}

	cvReleaseMat(&cov);
	cvReleaseMat(&avg);
	cvReleaseMat(&eigenVal);
	cvReleaseMat(&eigenVec);
	cvReleaseMat(&cvPVector);
	cvReleaseMat(&cvSVector);
}

/** 
 * function calcApproxIntensity
 * @param coeff: the input coefficient
 * @param pVector: the input pVector
 * @param i: the channel for coefficient
 * @param XYZchan: the XYZ channel
 * @return float: the approximation of the intensity
 */ 
float UtilitiesDisplay::calcApproxIntensity(const float coeff[], const float pVector[], unsigned int i, int XYZchan)
{
	return coeff[i]*pVector[unsigned(XYZchan)]+coeff[i+m_curveNbData]*pVector[unsigned(XYZchan)+3];
}

/**
 *  Default destructor of the class UtilitiesInterface
 */
UtilitiesDisplay::~UtilitiesDisplay()
{
	if (0 != m_nativeCurveGray)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveGray[i];
		}
		delete m_nativeCurveGray;
	}
	if (0 != m_nativeCurveRed)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveRed[i];
		}
		delete m_nativeCurveRed;
	}
	if (0 != m_nativeCurveGreen)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveGreen[i];
		}
		delete m_nativeCurveGreen;
	}
	if (0 != m_nativeCurveBlue)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveBlue[i];
		}
		delete m_nativeCurveBlue;
	}
	if (0 != m_nativeCurveCyan)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveCyan[i];
		}
		delete m_nativeCurveCyan;
	}
	if (0 != m_nativeCurveMagenta)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveMagenta[i];
		}
		delete m_nativeCurveMagenta;
	}
	if (0 != m_nativeCurveYellow)
	{
		for (int i = 0; i < m_curveNbData; i++)
		{
			delete [] m_nativeCurveYellow[i];
		}
		delete m_nativeCurveYellow;
	}

	delete [] m_cr;
	delete [] m_cg;
	delete [] m_cb;
	delete [] m_cc;
	delete [] m_cm;
	delete [] m_cy;
	delete [] m_ck;
}
