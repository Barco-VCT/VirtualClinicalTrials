#include "color.h"

#include <math.h>

using namespace std;

#define PI 3.14159265

bradford::bradford( const string& src, const string& dst )
{
	float src_XYZ [3];
	float dst_XYZ [3];

	illuminant( src, src_XYZ );
	illuminant( dst, dst_XYZ );	

	mCone.mult( src_XYZ );
	mCone.mult( dst_XYZ );

	M[0][0] = dst_XYZ[0] / src_XYZ[0];
	M[0][1] = 0;
	M[0][2] = 0;
	M[1][0] = 0;
	M[1][1] = dst_XYZ[1] / src_XYZ[1];
	M[1][2] = 0;
	M[2][0] = 0;
	M[2][1] = 0;
	M[2][2] = dst_XYZ[2] / src_XYZ[2];	
}

int bradford::illuminant( const string& illum, float* XYZ ) const
{
	if( illum == "A" )
	{
		XYZ[0] = A_XYZ[0];
		XYZ[1] = A_XYZ[1];
		XYZ[2] = A_XYZ[2];
	}

	else if( illum == "B" )
	{
		XYZ[0] = B_XYZ[0];
		XYZ[1] = B_XYZ[1];
		XYZ[2] = B_XYZ[2];
	}

	else if( illum == "C" )
	{
		XYZ[0] = C_XYZ[0];
		XYZ[1] = C_XYZ[1];
		XYZ[2] = C_XYZ[2];
	}

	else if( illum == "D50" )
	{
		XYZ[0] = D50_XYZ[0];
		XYZ[1] = D50_XYZ[1];
		XYZ[2] = D50_XYZ[2];
	}
	
	else if( illum == "D55" )
	{
		XYZ[0] = D55_XYZ[0];
		XYZ[1] = D55_XYZ[1];
		XYZ[2] = D55_XYZ[2];
	}
	
	else if( illum == "D65" )
	{
		XYZ[0] = D65_XYZ[0];
		XYZ[1] = D65_XYZ[1];
		XYZ[2] = D65_XYZ[2];
	}
	
	else if( illum == "D70" )
	{
		XYZ[0] = D70_XYZ[0];
		XYZ[1] = D70_XYZ[1];
		XYZ[2] = D70_XYZ[2];
	}
	
	else if( illum == "F2" )
	{
		XYZ[0] = F2_XYZ[0];
		XYZ[1] = F2_XYZ[1];
		XYZ[2] = F2_XYZ[2];
	}
	
	else if( illum == "F7" )
	{
		XYZ[0] = F7_XYZ[0];
		XYZ[1] = F7_XYZ[1];
		XYZ[2] = F7_XYZ[2];
	}

	else if( illum == "F11" )
	{
		XYZ[0] = F11_XYZ[0];
		XYZ[1] = F11_XYZ[1];
		XYZ[2] = F11_XYZ[2];
	}
		
	else
	{
		XYZ[0] = D50_XYZ[0];
		XYZ[1] = D50_XYZ[1];
		XYZ[2] = D50_XYZ[2];
	}


	return 0;
}

void bradford::mult( float* XYZ ) const
{
	mCone.mult( XYZ );
	matrix::mult( XYZ );
	mConeInv.mult(XYZ );
}

matrix::matrix(		float M00, float M01, float M02,
					float M10, float M11, float M12, 
					float M20, float M21, float M22		)
{
	M[0][0] = M00;
	M[0][1] = M01;
	M[0][2] = M02;
	M[1][0] = M10;
	M[1][1] = M11;
	M[1][2] = M12;
	M[2][0] = M20;
	M[2][1] = M21;
	M[2][2] = M22;
}

void matrix::mult( float* XYZ ) const
{
	float X = XYZ[0], Y = XYZ[1], Z = XYZ[2];
	XYZ[0] = M[0][0] * X + M[0][1] * Y + M[0][2] * Z;
	XYZ[1] = M[1][0] * X + M[1][1] * Y + M[1][2] * Z;
	XYZ[2] = M[2][0] * X + M[2][1] * Y + M[2][2] * Z;
}

void normate(  float* RGB, float gamma, float* white )
{
	RGB[0] = 255 * pow( saturate(RGB[0]/white[0]), gamma );
	RGB[1] = 255 * pow( saturate(RGB[1]/white[1]), gamma );
	RGB[2] = 255 * pow( saturate(RGB[2]/white[2]), gamma );
}

float saturate( float x )
{
	if( x < 0.0 )
		return 0.0;
	else if( x > 1.0 )
		return 1.0;
	else
		return x;
}

void from_XYZ_to_Lab(float X, float Y, float Z, 
	float& rL, float& ra, float& rb, 
	float illuminant_X, float illuminant_Y, float illuminant_Z)
{
	static const float kappa = 903.3f;
	static const float epsilon = 0.008856f;

	const float ref[3] = {X/illuminant_X, Y/illuminant_Y, Z/illuminant_Z};

	float f[3];
	for(unsigned i = 0; i<3; ++i)
	{
		if(ref[i] > epsilon)
			f[i] = pow(ref[i],1.0f/3.0f);
		else
			f[i] = (kappa*ref[i]+16)/116;
	}

	rL = 116*f[1] - 16;
	ra = 500*(f[0]-f[1]);
	rb = 200*(f[1]-f[2]);
}

void from_Lab_to_XYZ(float L, float a, float b, 
	float& rX, float& rY, float& rZ, 
	float illuminant_X, float illuminant_Y, float illuminant_Z)
{
	static const float kappa = 903.3f;
	static const float epsilon = 0.008856f;

	float f[3];
	f[1] = (L+16)/116;
	f[0] = a/500+f[1];
	f[2] = f[1]-b/200;

	float r[3];
	if(L > kappa*epsilon)
		r[1] = pow(f[1],3);
	else
		r[1] = L/kappa;

	if(pow(f[0],3) > epsilon)
		r[0] = pow(f[0],3);
	else
		r[0] = (116*f[0]-16)/kappa;
	
	if(pow(f[2],3) > epsilon)
		r[2] = pow(f[2],3);
	else
		r[2] = (116*f[2]-16)/kappa;

	rX = r[0]*illuminant_X;
	rY = r[1]*illuminant_Y;
	rZ = r[2]*illuminant_Z;
}

void from_Yxy_to_XYZ(	float &X, float &Y, float &Z, 
						float x, float y)
{
	if (y!=0)
	{
		X = (x*Y)/y;
		Z = ((1-x-y)*Y)/y;
	}
	else
		X=Y=Z=0;
}

void from_Lab_to_LCH(float L, float &C, float &H, float a, float b)
{
	C = sqrt(pow(a,2)+pow(b,2));
	H = atan2(b,a)*180/PI; // H in deg
	if (H<0)
		H = (float) (H + 360);
	else
		if (H>=360)
			H = (float) (H - 360);
}

void from_XYZ_to_LCH(	float &L, float &C, float &H, 
						float X, float Y, float Z, 
						float X0, float Y0, float Z0)
{
	float a(0), b(0);

	// 1 Conversion from XYZ to Lab
	from_XYZ_to_Lab(X, Y, Z, L, a, b, X0, Y0, Z0);

	// 2 Conversion from Lab to LCH
	from_Lab_to_LCH(L, C, H, a, b);
}

double deltaE2000(double Lstd, double astd, double bstd, double Lsample, double asample, double bsample)
{
	double Cabstd = sqrt(astd * astd + bstd * bstd);
	double Cabsample = sqrt(asample * asample + bsample * bsample);
	double Cabarithmean = (Cabstd + Cabsample) * 0.5;

	/**/
	double G = 0.5 * ( 1.0 - sqrt( pow((double)Cabarithmean,7.0)/(pow((double)Cabarithmean,7.0) + pow(25.0,7.0))));
	/*/
	double Cabarithmean7 = Cabarithmean * Cabarithmean * Cabarithmean * Cabarithmean * Cabarithmean * Cabarithmean * Cabarithmean;
	double G = 0.5 * ( 1.0 - sqrt( Cabarithmean7/(Cabarithmean7 + POW_25_7)));
	/**/
	
	double apstd = (1.0 + G) * astd; // aprime in paper
	double apsample = (1.0 + G) * asample; // aprime in paper
	double Cpsample = sqrt(apsample * apsample + bsample * bsample);

	double Cpstd= sqrt(apstd * apstd + bstd * bstd);
	// Compute product of chromas
	double Cpprod = (Cpsample * Cpstd);

	// Ensure hue is between 0 and 2pi
	double hpstd = atan2((double)bstd, (double)apstd);
	if (hpstd < 0)
		hpstd += (double) (2.0 * PI);  // rollover ones that come -ve
	if ((fabs(apstd) + fabs(bstd)) == 0.0)
		hpstd = 0.0;

	double hpsample = atan2((double)bsample, (double)apsample);
	if (hpsample < 0)
		hpsample += (double) (2.0 * PI);  // rollover ones that come -ve
	if ((fabs(apsample) + fabs(bsample)) == 0.0)
		hpsample = 0.0;

	double dL = (Lsample - Lstd);
	double dC = (Cpsample - Cpstd);

	// Computation of hue difference
	double dhp = (hpsample - hpstd);
	if (dhp > PI)
		dhp -= 2.0 * PI;
	if (dhp <- PI) 
		dhp += 2.0 * PI;
	// set chroma difference to zero if the product of chromas is zero
	if (Cpprod == 0.0)
		dhp = 0.0;

	// Note that the defining equations actually need signed Hue and chroma differences which is different from prior color difference formulae

	//double dH = 2.0 * sqrt(Cpprod) * sin(dhp/2.0);
	double dH = 2.0 * sqrt(Cpprod) * sin(dhp*0.5);

	// weighting functions
	double Lp = (Lsample + Lstd) * 0.5;
	double Cp = (Cpstd + Cpsample) * 0.5;

	// Average Hue Computation
	// This is equivalent to that in the paper but simpler programmatically.
	// Note average hue is computed in radians and converted to degrees only where needed
	double hp = (hpstd + hpsample) * 0.5;
	// Identify positions for which abs hue diff exceeds 180 degrees 
	if ( fabs(hpstd - hpsample)  > PI ) hp -= PI;
	// rollover ones that come -ve
	if (hp < 0) hp += 2.0 * PI;

	// Check if one of the chroma values is zero, in which case set mean hue to the sum which is equivalent to other value
	if (Cpprod == 0.0) hp = hpsample + hpstd;

	double Lpm502 = (Lp - 50.0) * (Lp - 50.0);
	double Sl = 1.0 + 0.015 * Lpm502 / sqrt(20.0 + Lpm502);  
	double Sc = 1.0 + 0.045 * Cp;

	/**/
	double T = 1.0 - 0.17 * cos(hp - PI / 6.0) + 0.24 * cos(2.0 * hp) + 0.32 * cos(3.0 * hp + PI/30.0) - 0.20 * cos(4.0 * hp - 63.0 * PI / 180.0);
	/*/
	double T = 1.0 - 0.17 * cos(hp - PI_OVER_6) + 0.24 * cos(2.0 * hp) + 0.32 * cos(3.0 * hp + PI_OVER_30) - 0.20 * cos(4.0 * hp - 63.0 * PI_OVER_180);
	/**/
	
	double Sh = 1.0 + 0.015 * Cp * T;
	
	/**/
	double delthetarad = (PI / 6.0) * exp(- pow(((180.0 / PI * hp - 275.0) / 25.0), 2.0));
	/*/
	double delthetarad = PI_OVER_6 * exp(- ((PI_OVER_180_INV * hp - 275.0) / 25.0)*((PI_OVER_180_INV * hp - 275.0) / 25.0));
	/**/
	
	/**/
	double Rc = 2.0 * sqrt(pow((double)Cp, 7.0) / (pow((double)Cp, 7.0) + pow(25.0, 7.0)));
	/*/
	double num = Cp*Cp*Cp*Cp*Cp*Cp*Cp;
	double denom = num + POW_25_7;
	double Rc = 2.0 * sqrt(num / denom);
	/**/

	double RT = -sin(2.0 * delthetarad) * Rc;

	// The CIE 00 color difference
	/**/
	double dist = sqrt(pow((double)(dL / Sl), 2.0) + pow((double)(dC / Sc), 2.0) + pow((double)(dH / Sh), 2.0) + RT *(dC / Sc) * (dH / Sh));
	/*/
	double dist = sqrt((dL / Sl)*(dL / Sl) + (dC / Sc)*(dC / Sc) + (dH / Sh)*(dH / Sh) + RT *(dC / Sc) * (dH / Sh));
	/**/
	return dist;
}
