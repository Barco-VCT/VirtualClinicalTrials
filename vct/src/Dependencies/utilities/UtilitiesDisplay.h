//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file UtilitiesDisplay.h
 * \brief Implementation of the class UtilitiesDisplay
 * \date 04-May-2009
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

#ifndef UTILITIESDISPLAY_H
#define UTILITIESDISPLAY_H

#include <DataExchanger/UtilitiesInterface.h>

#include <string>
#include <vector>

#include <cv.h>

/*!
  * \enum RGBCMYK
  * \brief Used to list the referenced native curved used by the unitilities display.
  */ 
enum RGBCMYK
{
	RED,
	GREEN,
	BLUE,
	CYAN,
	MAGENTA,
	YELLOW,
	GRAY
};

/*!
  * \class UtilitiesDisplay
  * \brief This class is the implementation of the virtual interface
  *		   to manage data exchange between modules.
  *		   It allows exchanging data between modules (display simulation)
  */ 
class UtilitiesDisplay:public UtilitiesInterface
{
public:

	/*!
	 *  Default constructor of the class UtilitiesInterface
	 */
	UtilitiesDisplay(int nbBits,
	                 int frequency,
	                 int screenWidth,
	                 int screenHeight,
	                 int imageTopColumnPosition,
	                 int imageTopRowPosition,
	                 double pitchPixelColumnDirection,
	                 double pitchPixelRowDirection,
	                 int _1Color_0Monochrome);

	/*!
	 * \fn loadnativeCurve
	 * \brief loading of the native curve: cd/m2 vs ddl
	 * \param filename: the file name to load the native curve
	 * \param color: the colored native curve to load
	 * \return bool: true if native curve is well loaded
	 */
	bool loadNativeCurve(const std::string & filename, RGBCMYK color);

	/*!
	 * \fn getDDL_sup. 
	 * \brief return ddl superior
	 * \param nb: the number of ddl
	 * \param step: the value of the step to be reach?
	 * \return int: the ddl superior
	 */ 
	int getDDL_sup (int nb, int step);

	/*!
	 * \fn getDDL_inf
	 * \brief return ddl inferior
	 * \param nb: the number of ddl
	 * \param step: the value of the step to be reach?
	 * \return int: the ddl inferior
	 */
	int getDDL_inf (int nb, int step);

	/*! \fn getDDL
	 *	\brief allowing to calculate the lower and upper landmarks following a step (distance between two values) and an initiale number
	 * \param nb: number for which the landmarks have to be researhed
	 * \param inf: pointer to the variables which will hosting the lower landmark
	 * \param sup: pointer to the variables which will hosting the upper landmark
	 * \param step: difference between two values
	 * \return void
	 */
	void getDDL (int nb, int *inf, int *sup, int step); 

	/*!
	 * \fn getMaxDDLLevelValue
	 * \brief get the maximum ddl value in the curve (2^n -1)
	 * \return int: the maximum ddl value in the curve (2^n -1)
	 */
	int getMaxDDLLevelValue(); 

	/*!
	 * \fn return_ddl_value_BinarySearch
	 * \brief Optimized ddl search in native curve using dichotomic process
	 * \param value: the value to search in the curve
	 * \param chanXYZ: 0= look in chan X, 1= look in chan Y, 2= look in chan Z
	 * \param curve: the curve to search in.
	 * \return int: the corresponding indice
	 */
	int return_ddl_value_BinarySearch(float value, int chanXYZ, RGBCMYK curve);

	/*! 
	 * \fn getNativeCurve	 
	 * \param curve: the native curve to return
	 * \return float**: the desired native curve
	 */
	float** getNativeCurve(RGBCMYK curve);

	/*!
	 * function getNbBits
	 * \return int: the nb bits
	 */
	int getNbBits();

	/*! 
	 * \fn getscreenWidth
	 * \return int: the screen width in number of pu
	 */
	int getScreenWidth();

	/*!
	 * \fn getscreenHeight
	 * \return int: the screen height in number of pu
	 */
	int getScreenHeight();

	/*!
	 * \fn imageTopColumnPosition
	 * \return int: the top leftposition of the simulated image in terms of column number starting from "0" until "screenWidth-1": unit = pixel
	 */
	int getImageTopColumnPosition();

	/*!
	 * \fn imageTopRowPosition
	 * \return int: the top leftposition of the simulated image in terms of row number starting from "0" until "screenHeight-1": unit = pixel
	 */
	int getImageTopRowPosition();

	/*!
	 * \fn PitchPixelColumnDirection
	 * \return double: pixel pitch in mm in column direction
	 */
	double getPitchPixelColumnDirection();

	/*!
	 * \fn PitchPixelRowDirection
	 * \return double: pixel pitch in mm in row direction
	 */
	double getPitchPixelRowDirection();

	/*! 
	 * \fn getCurveNbData
	 * \return int: the nb data in the curve
	 */	
	int getCurveNbData();

	/*!
	 * \fn getFrequency
	 * \return int: the frequency of the display
	 */	
	int getFrequency();

	/*!
 	 * \fn JND2lum
	 * \brief convert JND value in luminance value (cd/m2)
	 * \param j: the JND value
	 * \return double: the luminance value
 	 */ 
	static double JND2lum(double j);

	/*!
 	 * \fn lum2JND
	 * \brief convert luminance value (cd/m2) in JND value
	 * \param L: the luminance value
	 * \return double: the JND value
 	 */ 
	static double lum2JND(double L);

	/*! 
 	 * \fn JNDinInterval
	 * \brief get the JND in interval
	 * \param L1: the first luminance value
	 * \param L2: the second luminance value
	 * \return double: the JND interval
 	 */ 
	static double JNDinInterval(double L1, double L2);

	/*!
 	 * \fn fromRgbToXyz
	 * \brief transform RGB data into XYZ data
	 * \param r: the red ddl value 
	 * \param g: the green ddl value
	 * \param b: the blue ddl value
	 * \return float*: XYZ values in an array of 3 floats (0=X, 1=Y, 2=Z)
 	 */ 
	std::vector<float> fromRgbToXyz(unsigned int r, unsigned int g, unsigned int b);

	/*!
	 * \fn coefficientProcess
	 * \brief method to process the required coefficient for RGB to XYZ conversion
	 */
	void coefficientProcess();

	/*!
	 * Default destructor of the class UtilitiesInterface
	 */
	~UtilitiesDisplay();

private:
	/*! 
 	 * \fn calcCoeffR
	 * \brief calculate red coefficients
	 * \return void
 	 */ 
	void calcCoeffR();

	/*!
 	 * \fn calcCoeffG
	 * \brief calculate green coefficients
	 * \return void
 	 */ 
	void calcCoeffG();

	/*!
 	 * \fn calcCoeffB
	 * \brief calculate blue coefficients
	 * \return void
 	 */ 
	void calcCoeffB();

	/*!
 	 * \fn calcCoeffC
	 * \brief calculate cyan coefficients
	 * \return void
 	 */ 
	void calcCoeffC();

	/*!
 	 * \fn calcCoeffM
	 * \brief calculate magenta coefficients
	 * \return void
 	 */ 
	void calcCoeffM();

	/*! 
 	 * \fn calcCoeffY
	 * \brief calculate yellow coefficients
	 * \return void
 	 */ 
	void calcCoeffY();

	/*!
 	 * \fn calcCoeffK
	 * \brief calculate gray coefficients
	 * \return void
 	 */ 
	void calcCoeffK();

	/*!
 	 * \fn calcCoeffBasedOnEigenVectors
	 * \brief process parameters reduction based on eight vectors
	 * \param vects: the input vector
	 * \param coeff: the output coefficient
	 * \param pvector: the output pVector
	 * \return void
 	 */ 
	void calcCoeffBasedOnEigenVectors(const CvMat ** vects, float coeff[], float pVector[]);

	/*!
 	 * \fn calcApproxIntensity
	 * \param coeff: the input coefficient
	 * \param pVector: the input pVector
	 * \param i: the channel for coefficient
	 * \param XYZchan: the XYZ channel
	 * \return float: the approximation of the intensity
	 * \return void
 	 */ 
	float calcApproxIntensity(const float coeff[], const float pVector[], unsigned int i, int XYZchan);

	//private attributes
	int m_nbBits; //!< the number of bits of the input data (input nbBits= output nbBits)
	int m_screenWidth; //!< the number of pixels of the width of the display in number of pixels
	int m_screenHeight; //!< the number of pixels of the height of the display in number of pixels
	int m_imageTopColumnPosition; //!< the top leftposition of the simulated image in terms of column number starting from "0" until "screenHeight-1": unit = pixel
	int m_imageTopRowPosition; //!< the top leftposition of the simulated image in terms of row number starting from "0" until "screenWidth-1": unit = pixel
	double m_pitchPixelColumnDirection; //!< pixel pitch in mm in column direction
	double m_pitchPixelRowDirection; //!< pixel pitch in mm in row direction
	int m_curveNbData; //!< nb data of the native curve
	int m_frequency; //!< the frequency	of the display
	int m_1Color_0Monochrome; //!< 1-->color display, 0-->monochrome display
	
	float** m_nativeCurveGray; //!< the gray native curve
	float** m_nativeCurveRed; //!< the red native curve
	float** m_nativeCurveGreen; //!< the green native curve
	float** m_nativeCurveBlue; //!< the blue native curve
	float** m_nativeCurveMagenta; //!< the magenta native curve
	float** m_nativeCurveCyan; //!< the cyan native curve
	float** m_nativeCurveYellow; //!< the Yellow native curve

	float m_offsetxyz[3];

	float m_rxyz[6];
	float m_gxyz[6];
	float m_bxyz[6];
	float m_cxyz[6];
	float m_mxyz[6];
	float m_yxyz[6];
	float m_wxyz[6];

	float* m_cr;
	float* m_cg;
	float* m_cb;
	float* m_cc;
	float* m_cm;
	float* m_cy;
	float* m_ck;
};

#endif // UTILITIESDISPLAY_H