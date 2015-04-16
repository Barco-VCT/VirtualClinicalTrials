
 //////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file LookUpTable.h
 * \brief Implementation of class LookUpTable
 * \date 1-June-2010
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

#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <cstring>
#include <vector>

#include <ErrorManagement/ErrorLogger.h>

/*!
 * \class LookUpTable
 */
class LookUpTable
{
public:
	/*!
	 * Constructor
	 * \brief sets the offset at 0 and the factor at 1, Performs a deep copy of the content of the data parameter if it is provided
	 * \param size Size of the internal array of the LUT. This conditions the precision the function approximation.
	 * \param data Array (of the appropriate size), containing the data to be copied to the internal array of the LUT.
	 * \warning This function is not memory safe, make sure the memory allocated to "data" is the right size
	 */
	LookUpTable(size_t size, float* data = NULL);

	/*!
	 * Constructor
	 * \brief sets the offset at 0 and the factor at 1, performs a deep copy of the content of the data parameter
	 * \param data Vector containing the data to be copied to the internal array of the LUT. The size/precision of the LUT is the same as the provided vector.
	 */
	LookUpTable(const std::vector<float>& data);

	/*!
	 * Constructor
	 * \brief Create a large LUT from a sample using the Spline1D class
	 * \param size Desired size of the internal array of the LUT. This conditions the precision the function approximation.
	 * \param input_size Size of the provided sample
	 * \param x Array containing inputs for the modeled function
	 * \param f Array containing the outputs of the function corresponding to the inputs provided in x
	 */
	LookUpTable(size_t size, size_t input_size, double* x, double* f);

	/*!
	 * Constructor
	 * \brief Reads a LUT from a file
	 * \param filepath The path to the file containing the LUT data
	 * \throw std::runtime_error if the file could not be opened
	 */
	LookUpTable(const char* filepath, bool isTxt=false, int size=0);


	// accessors

	/*!
	 * \fn getSize
	 * \brief accessor for getting the size of the 1D Lut data
	 * \return size_t: unsigned int, size of the the data
	 */
	size_t getSize(){ return m_size;};

	/*!
	 * Destructor 
	 */
	~LookUpTable();

	/*!
	 * \fn setDataAt
	 * \brief Changes one element of the internal array
	 * \param i Position of the element to change
	 * \param val New value
	 * \return void
	 */
	void setDataAt(size_t i, float val){m_data[i] = val;}
	
	/*!
	 * \fn setData
	 * \brief Sets the complet internal array
	 * \warning This function is not memory safe, make sure the memory allocated to "data" is the right size
	 * \param data The new internal array, which is the same size as the original internal array
	 * \return void
	 */
	void setData(float* data){memcpy(m_data,data,m_size);}

	/*!
	 * \fn at
	 * \brief Return an element of the internal array
	 * \param i Index of the element of return
	 * \return float: The element of the internal array at position i
	 */
	float at(size_t i)const{return m_data[i];}

	/*!
	 * \fn getFactor
	 * \brief accessors to get m_factor
	 * \return float: The scale factor between the input of the function and the size of the internal array
	 */
	float getFactor()const{return m_factor;}

	/*!
	 * \fn setFactor
	 * \brief Sets the scale factor between the input of the function and the size of the internal array
	 * \param val New value of the scale factor
	 * \return void
	 */
	void setFactor(float val){m_factor = val;}

	/*!
	 * \fn getOffset
	 * \return float: The value of the input corresponding ot index 0 of the internal array
	 */
	float getOffset()const{return m_offset;}

	/*!
	 * \fn setOffset
	 * \brief Sets the value of the input corresponding ot index 0 of the internal array
	 * \param val New value of the offset
	 * \return void
	 */
	void setOffset(float val){m_offset = val;}

	/*!
	 * \fn eval
	 * \brief Evaluates the input with the LUT:
	 *		  - the input i is converted to the LUT resolution
	 *		  - the exact content of LUT value corresponding to the closest LUT entry is returned
	 * \param i input of the approximated function
	 * \return float: the approximate result of the function
	 */
	float eval(float i)const;

	/*!
	 * \fn interpolate
	 * \brief Evaluates the input by linearly interpolating the values of the LUT
	 * \param i input of the approximated function
	 * \return float: the approximate result of the function
	 */
	float interpolate(float i)const;

	/*!
	 * \fn reverseEval
	 * \param i: Desired output of the function modeled by the LUT
	 * \return float: The input value to be provided to the eval() function in order for it to return a value of i
	 */
	float reverseEval(float i)const;
	float reverseInterp(float i)const;

	/*!
	 * \fn saveToFile
	 * \brief Saves the LUT to a file
	 * \param filepath Path to the file that will contain the LUT data
	 * \throw runtime_error if the file could not be opened
	 * \return void
	 */
	void saveToFile(const char* filepath) const;

private:
	float* m_data;	//!< Data array of the LUT
	size_t m_size;	//!< Size of the data array
	float m_factor;	//!< Scale factor between the input of the function and the size of the internal array
	float m_offset;	//!< Value of the input corresponding ot index 0 of the internal array
};

#endif //LOOKUPTABLE_H