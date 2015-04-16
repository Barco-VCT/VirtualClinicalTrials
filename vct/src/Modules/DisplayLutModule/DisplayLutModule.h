//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file DisplayLutModule.h
 * \brief Implementation of class DisplayLutModule
 * \date 14-Dec-2009
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


#ifndef DISPLAYLUTMODULE_H
#define DISPLAYLUTMODULE_H

#include <Module.h>

class UtilitiesDisplay;

/** 
 * This class allows to calibrate the display using DICOM by interchanging the ddl with a LUT
 * by modifying the native curve to be as close as possible to the GSDF
 * RGB dlls only considered in this module as monochrome!
 */ 
class DisplayLutModule : public Module
{

public:
	/** 
	 * Constructor of the class. 
	 */ 
	DisplayLutModule();

	/** 
	 * Method Simulate. 
	 * Main function for the simulation
	 * @param list: list of containers to process
	 * @return void
	 */
	void Simulate( std::vector<DSContainer*>& container);

	/**
	 * method SetParameter
	 * Method allowing to set the value of a class parameter
	 * @param name:  name of the parameter
	 * @param value: value of the parameter, string type
	 * @return void
	 */
	void SetParameter(const std::string & name, const std::string & value );

	/** 
	 * Method ReturnDdlValue
	 * Optimized search in native curve the corresponding luminance/intensity value
	 * using dichotomic process
	 * the native curve has to be order
	 * @param luminance_value: the luminance value (cd/m^2)
	 * @return int: the intensity value (ddl)
	 */
	int ReturnDdlValue(float luminance_value);

	/** 
	 * Destructor of the class. 
	 */ 
	~DisplayLutModule();

private:

	std::string m_unit;
	std::string m_illum;
	std::string m_obs;
	float m_whitePoint[3];


	unsigned int m_inputNumberOfBits;
	unsigned int m_outputNumberOfBits;

	unsigned int m_inputNumberOfFrames;
	unsigned int m_outputNumberOfFrames;
	float m_frameRate;

	int m_inputComponentNumber;
	int m_outputComponentNumber;

	unsigned long m_width, m_height, m_i, m_j, m_k;

	std::vector<unsigned int> m_lut;

	std::vector<double> m_jndTargetCurve;
	std::vector<double> m_jndNativeCurve;

	double m_minJnd;
	double m_maxJnd;

	UtilitiesDisplay* m_ud;

	/** path to the file containing the optional precalculated LUT */
	std::string m_precalcLutPath;
	std::string m_debugCalibratedLutOutput;
	std::string m_debugTransferCurveOutput;
};

#endif //DISPLAYLUTMODULE_H
