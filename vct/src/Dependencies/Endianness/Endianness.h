//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Endianness.h
 * \brief Implementation of Endianness conversion for binary little or big endian raw files
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

#ifndef Endianness_H
#define Endianness_H

#include <cassert>

class Endianness
{
	public:
		enum ENDIAN { BE, LE };

	public:
		Endianness();
		virtual ~Endianness() {}

		ENDIAN GetProcessorEndianness() const;

	protected:
		void GetIntAsByteIndexes(int &IDX0, int &IDX1) const;
		ENDIAN m_file_endian;

	private:
		ENDIAN m_processor_endian;
};


inline
Endianness::Endianness()
{
	m_processor_endian = GetProcessorEndianness();
	m_file_endian = BE; // default
}


inline Endianness::ENDIAN
Endianness::GetProcessorEndianness() const
{
	int machine_uses_big_endian = 1;
	((unsigned char*) &machine_uses_big_endian)[0] = 0;
	return (machine_uses_big_endian ? BE : LE);
}


/* correlation between BE/LE file and BE/LE RAM:

                         MSB   LSB
-----------------------+-----+-----+---
BE file                | +0  | +1  |
16 bit int             |8 bit|8 bit|
-----------------------+-----+-----+---
                         BA ----->


                         LSB   MSB
-----------------------+-----+-----+---
LE file                | +0  | +1  |
16 bit int             |8 bit|8 bit|
-----------------------+-----+-----+---
                         BA ----->


             MSB               LSB
-----------+-----+-----+-----+-----+---   BE file byte 0 --> BA + 2
BE RAM     | +0  | +1  | +2  | +3  |      BE file byte 1 --> BA + 3
32 bit int |8 bit|8 bit|8 bit|8 bit|      LE file byte 0 --> BA + 3
-----------+-----+-----+-----+-----+---   LE file byte 1 --> BA + 2
             BA ----------------->


             MSB               LSB
-----------+-----+-----+-----+-----+---   BE file byte 0 --> BA + 1
LE RAM     | +3  | +2  | +1  | +0  |      BE file byte 1 --> BA + 0
32 bit int |8 bit|8 bit|8 bit|8 bit|      LE file byte 0 --> BA + 0
-----------+-----+-----+-----+-----+---   LE file byte 1 --> BA + 1
               <--------------- BA

BE file --> BE RAM : IDX0 = 2, IDX1 = 3
BE file --> LE RAM : IDX0 = 1, IDX1 = 0
LE file --> BE RAM : IDX0 = 3, IDX1 = 2
LE file --> LE RAM : IDX0 = 0, IDX1 = 1
*/

inline void
Endianness::GetIntAsByteIndexes(int &IDX0, int &IDX1) const
// returns the correct indexes for int's (at file level) interpreted as byte*
// is endianness-dependent
// src: 16-bit short int (file)
// dst: 32-bit int (RAM)
// only 16 bit of the 32-bit int will be addressed
// return values: IDX0: first byte, IDX1: second byte
{
	assert (sizeof(int)==4); // otherwise the IDX0 and IDX1 indexes won't match
	assert (m_file_endian == LE || m_file_endian == BE);
	assert (m_processor_endian == LE || m_processor_endian == BE);

	if (m_file_endian == LE && m_processor_endian == LE)
	{
		IDX0 = 0;
		IDX1 = 1;
	}
	else if (m_file_endian == BE && m_processor_endian == LE)
	{
		IDX0 = 1;
		IDX1 = 0;
	}

	else if (m_file_endian == LE && m_processor_endian == BE)
	{
		IDX0 = 3;
		IDX1 = 2;
	}

	else // (m_file_endian == BE && m_processor_endian == BE)
	{
		IDX0 = 2;
		IDX1 = 3;
	}
}

#endif // Endianness_H
