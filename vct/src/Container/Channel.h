//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Channel.h
 * \brief Implementation of the class Channel
 * \date 5-April-2009
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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "DSContainer.h"

/*!
 * \class Channel
 * \brief class used to extract one channel from a Component
 */
class Channel
{
public:
	/*!
	 * Extracts a Channel from a frame of a component of a DSContainer object
	 * \param Dcontainer: the container to read
	 * \param component: the index of the component to read in the container
	 * \param nFrame: the index of the frame to read in the component
	 * \param nChannel: the index of the channel to extract in the frame
	 * \return static Channel*: the extracted channel
	 */
	static Channel* extractChannel(DSContainer* container, 
		unsigned nComponent, unsigned nFrame, unsigned nChannel);

	/*!
	 * \fn width
	 * \brief witdh accessor
	 * \return Channel width, number of columns
	 */
	unsigned long width() const{return m_width;}

	/*!
	 * \fn height
	 * \brief height accessor
	 * \return unsigned long: channel width, number of rows
	 */
	unsigned long height() const{return m_height;}

	/*!
	 * \fn dataType
	 * \brief data type accessor
	 * \return int: data type (cf. DSContainer.h)
	 */
	int dataType() const{return m_dataType;}

	/*!
	 * \fn elementSize
	 * \brief element size accessor
	 * \return unsigned: size of one element/pixel in bytes
	 */
	unsigned elementSize() const{return m_elementSize;}

	/*!
	 * \fn getDataPointer
	 * \brief pointer to char* accessor, the raw data
	 * \return const char*: const pointer to raw data
	 */
	const char* getDataPointer() const{return m_data;}

	/*!
	 * \fn dataSize
	 * \brief data size accessor
	 * \return unsigned: size of the raw data in bytes
	 */
	unsigned dataSize() const{return m_width*m_height*m_elementSize;}

	/*!
	 * \fn getElement
	 * \brief Allocates and return an array of bytes 
	 *		  containing the data of one element
	 * \param row: Row of the element
	 * \param col: Column of the element
	 * \return char*: the data array
	 */
	char* getElement(unsigned row, unsigned col) const;

	/*!
	 * \fn getElement
	 * \brief Copy the data of one element in a pre-allocated array
	 * \param row: Row of the element
	 * \param col: Column of the element
	 * \param element: The array in which the data will be copied
	 */
	void getElement(unsigned row, unsigned col, char* element) const;

	/*!
	 * Destructor 
	 */
	~Channel();

private:
	/*! 
	 * Private constructor 
	 */
	Channel();
	char* m_data;			//!< Pointer to the raw data
	enum ComponentType m_dataType;	//!< Data type code
	unsigned m_elementSize;	//!< Size of an element in bytes
	unsigned long m_width;	//!< Width - number of columns
	unsigned long m_height;	//!< Height - number of rows
};

#endif //CHANNEL_H
