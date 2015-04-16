//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file iConvert.h
 * \brief Implementation of the namespace iConvert
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

#include <iostream>

#ifndef ICONVERT_H
#define ICONVERT_H

/*!
 * \brief namespace used to convert multibytes char* <-> unicode char*
 *		  Multibytes is the default one(char *, string, ...)
 *		  Unicode is wchar_t, wstring, ...
 */
namespace iConvert {
	/*!
	 * \fn wcToMb
	 * \brief Unicode to multibyte
	 * \param in: character string to convert
	 * \return the corresponding multibytes character string
	 */
	std::string wcToMb(wchar_t* in);

	/*!
	 * \fn wcToMb
	 * \brief Unicode to multibyte
	 * \param in: character string to convert
	 * \return the corresponding multibytes character string
	 */
	std::string wcToMb(const wchar_t* in);

	/*!
	 * \fn mbToWc
	 * \brief Multibyte to Unicode
	 * @param in: character string to convert
	 * \return the corresponding unicode character string
	 */
	wchar_t* mbToWc(char* in);

	/*!
	 * \fn mbToWc
	 * \brief Multibyte to Unicode
	 * \param in: character string to convert
	 * \return the corresponding unicode character string
	 */
	wchar_t* mbToWc(std::string in);

	/*!
	 * \fn mbToWc
	 * \brief Multibyte to Unicode
	 * \param in: character string to convert
	 * \return the corresponding unicode character string
	 */
	wchar_t* mbToWc(const char* in);

	/*!
	 * \fn getSize
	 * \brief Return the number of characters of an unicode character string
	 * \param in: character string where the number of characters have to be computed
	 * \return the number of characters of the character string
	 */
	int getSize(wchar_t* in);

	/*!
	 * \fn getSize
	 * \brief Return the number of characters of an multibytes character string
	 * \param in: character string where the number of characters have to be computed
	 * \return the number of characters of the character string
	 */
	int getSize(char* in);
}

#endif // ICONVERT_H
