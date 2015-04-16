//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file color.h
 * \brief Implementation of color space converion functions, class matrix, bradford
 * \date 1-Feb-2008
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

#ifndef COLOUR_H
#define COLOUR_H
#include <string>
#include <iostream>

/*! 
 * \class matrix
 * \brief This class allows to manage matrices
 */ 
class matrix
{
	public:
	/*! 
 	 * Constructor of the class matrix
 	 * \param M00: the point M00
 	 * \param M01: the point M01
 	 * \param M02: the point M02
 	 * \param M10: the point M10
 	 * \param M11: the point M11
 	 * \param M12: the point M12
 	 * \param M20: the point M20
 	 * \param M21: the point M21
 	 * \param M22: the point M22
	 */
	matrix( float M00, float M01, float M02,
			float M10, float M11, float M12, 
			float M20, float M21, float M22	 );

	/*! 
 	 * Constructor of the class matrix
	 */
	 matrix( void ) {};

	/*! 
 	 * \fn mult
	 * \brief multiply matrix with XYZ value
 	 * \param[in,out] XYZ the reference on the XYZ to multiply
	 */
	void mult( float* XYZ ) const;

	float M [3][3]; //!< (3,3) matrix
};

/*! 
 * \class bradford
 * \brief This class allows to manage bradford for the matrix
 */ 
class bradford : public matrix
{
	public:
	/*! 
 	 * Constructor of the class bradford
 	 * \param src: the reference on the source
 	 * \param dst: the reference on the destination
	 */
	bradford( const std::string& src, const std::string& dst );

	/*! 
 	 * \fn mult
	 * \brief multiply bradford matrix with XYZ value
 	 * \param[in,out] XYZ the reference on the XYZ to multiply
	 */
	void mult( float* XYZ ) const;	

	/*!
 	 * \fn illuminant
	 * \brief calcul the illuminant
 	 * \param illum: the reference on the illuminant
 	 * \param XYZ: the reference on the XYZ
	 * \return int: the error status
	 */
	int illuminant( const std::string& illum, float* XYZ ) const;
};

/*! 
 * \fn saturate
 * \brief clip values between 0.0 & 1.0
 * \param x: the value to saturate
 * \return float: the saturated value
 */
inline float saturate( float x );

/*!
 * \fn normate (normalization)
 * \brief The passed RGB values is already passed to XYZ
 *		  Normalise from RGB to R'G'B'
 * \param RGB the reference on the RGB values to normalize
 * \param gamma the gamma value
 * \param white the array containing white point
 */
void normate(  float* RGB, float gamma, float* white );

/* Color conversion matrices */
static matrix mCone (	0.8951f,	0.2664f,  -0.1614f,
					   -0.7502f,	1.7135f,   0.0367f,
						0.0389f,    -0.0685f,  1.0296f	);

static matrix mConeInv (	0.9870f,   -0.1471f,	0.1600f,
							0.4323f,	0.5184f,	0.0493f,
						   -0.0085f,	0.0400f,	0.9685f	);

static matrix mWideGamut(	 1.4625f, -0.1845f, -0.2734f,
							-0.5228f,  1.4479f,  0.0681f,
							 0.0346f, -0.0958f,  1.2875f	);

static matrix mCie(	 2.3707f, -0.9001f, -0.4706f,
					 -0.5139f, 1.4253f, 0.0886f,
					 0.0053f, -0.0147f, 1.0094f );						

static matrix msRGB(	 3.2405f, -1.5371f, -0.4985f,
					 -0.9693f, 1.8760f, 0.0416f,
					 0.05556f, -0.2040f, 1.0572f )	;	

const static float A_XYZ [3]   = { 111.114f, 100.0f, 35.200f  };
const static float B_XYZ [3]   = { 99.090f,  100.0f, 85.324f  };
const static float C_XYZ [3]   = { 97.285f,  100.0f, 116.145f };
const static float D50_XYZ [3] = { 96.4295f, 100.0f, 82.5105f };
const static float D55_XYZ [3] = { 95.799f,  100.0f, 90.926f  };
const static float D65_XYZ [3] = { 94.811f,  100.0f, 107.304f };
const static float D70_XYZ [3] = { 94.416f,  100.0f, 120.641f };
const static float F2_XYZ [3]  = { 103.279f, 100.0f, 69.027f  };
const static float F7_XYZ [3]  = { 95.792f,  100.0f, 107.686f };
const static float F11_XYZ [3] = { 103.863f, 100.0f, 65.607f  };

/*!
 * \fn from_XYZ_to_Lab
 * \brief Converts color data from the CIE XYZ space to the CIE L*a*b* space
 * \param X X coordinate of the input color
 * \param Y Y coordinate of the input color
 * \param Z Z coordinate of the input color
 * \param rL: L* coordinate of the output color
 * \param ra: a* coordinate of the output color
 * \param rb: b* coordinate of the output color
 * \param illuminant_X: X coordinate of the reference white point
 * \param illuminant_Y: Y coordinate of the reference white point
 * \param illuminant_Z: Z coordinate of the reference white point
 */
void from_XYZ_to_Lab(float X, float Y, float Z, 
	float& rL, float& ra, float& rb, 
	float illuminant_X, float illuminant_Y, float illuminant_Z);

/*!
 * \fn
 * \brief Converts color data from the CIE L*a*b* space to the CIE XYZ space
 * \param L: L* coordinate of the input color
 * \param a: a* coordinate of the input color
 * \param b: b* coordinate of the input color
 * \param rX: X coordinate of the output color
 * \param rY: Y coordinate of the output color
 * \param rZ: Z coordinate of the output color
 * \param illuminant_X: X coordinate of the white point
 * \param illuminant_Y: Y coordinate of the white point
 * \param illuminant_Z: Z coordinate of the white point
 */
void from_Lab_to_XYZ(float L, float a, float b, 
	float& rX, float& rY, float& rZ, 
	float illuminant_X, float illuminant_Y, float illuminant_Z);

/*!
 * \fn
 * \brief Converts color data from Yxy space to the XYZ
 * \param X: coordinate of the output color
 * \param Y: coordinate of the output color
 * \param Z: coordinate of the output color
 * \param x: coordinate of the input color
 * \param y: coordinate of the input color
 */
void from_Yxy_to_XYZ(	float &X, float &Y, float &Z, 
						float x, float y);
/*!
 * \fn
 * \brief Converts color data from XYZ space to LCH
 * \param L: coordinate of the output color
 * \param C: coordinate of the output color
 * \param H: coordinate of the output color
 * \param X: coordinate of the input color
 * \param Y: coordinate of the input color
 * \param Z: coordinate of the input color
 * \param X0: coordinate of the white point
 * \param Y0: coordinate of the white point
 * \param Z0: coordinate of the white point
 */
void from_XYZ_to_LCH(	float &L, float &C, float &H, 
						float X, float Y, float Z, 
						float X0, float Y0, float Z0);

double deltaE2000(double Lstd, double astd, double bstd, double Lsample, double asample, double bsample);

#endif /* COLOUR_H*/

