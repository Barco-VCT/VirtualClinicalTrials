//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file misc.h
 * \brief Implementation of the class misc
 * \date 01-Feb-2008
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


#ifndef MISC_H
#define MISC_H

#include <vector>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <wchar.h>

#define TCHARMISC wchar_t

#include <ErrorManagement/MevicLogger.h>
#include <ErrorManagement/ErrorLogger.h>
#include <DSContainer.h>

#include <Meschach/matrix.h>

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#ifndef	MATRIXH
struct MAT;
struct VEC;
#endif


#if defined __cplusplus
  extern "C" {
#endif
/*!
 * \fn printm
 * \brief Display the content of a given matrix
 * \param S: the matrix
 * \param caption: caption
 */
void printm(MAT * S, const char * caption);
#if defined __cplusplus
  }
#endif

#if defined __cplusplus
  extern "C" {
#endif
/*!
 * \fn printv
 * \brief Display the content of a given vector
 * \param v: the vector
 * \param caption: caption
 */
void printv(VEC * v, const char * caption);
#if defined __cplusplus
  }
#endif

/*!
 * \class misc
 * \brief class for converting vct containers to CHO datastructure
 */
class misc
{
public:    
	/*!
	 * \fn tchar_to_char
	 * \brief convert tchar to char
	 * \param t_ch: tchar to convert
	 * \return char: converted char
	 */
    static char tchar_to_char( TCHARMISC t_ch );

	/*!
	 * \fn char_to_tchar
	 * \brief convert char to tchar
	 * \param ch: char to convert
	 * \return TCHARMISC: converted tchar
	 */
    static TCHARMISC char_to_tchar( char ch );

	/*!
	 * \fn tstring_to_string
	 * \brief convert tchar to string
	 * \param t_ch: tchar to convert
	 * \return string: converted string
	 */
    static std::string tstring_to_string( const TCHARMISC * t_ch );

	/*!
	 * \fn tstring_to_string
	 * \brief convert tstring to string
	 * \param t_ch: tstring to convert
	 * \return string: converted string
	 */
    static std::string tstring_to_string( const std::tstring & t_str );

	/*!
	 * \fn string_to_tstring
	 * \brief convert char to tstring
	 * \param ch: char to convert
	 * \return tstring: converted tstring
	 */
    static std::tstring string_to_tstring( const char * ch );

	/*!
	 * \fn string_to_tstring
	 * \brief convert string to tstring
	 * \param str: string to convert
	 * \return tstring: converted tstring
	 */
    static std::tstring string_to_tstring( const std::string & str );

    static std::string BackSlash2ForwardSlash(const std::string & str, bool append_path_delimiter=false);

    /*! 
	 * \fn ExtractDirectoryPath
	 * \brief extracts the path to the directory,
     *		  removes the filename from the path.
     * \param path: full path
     * \return path to the directory 
	*/
    static std::string ExtractDirectoryPath(const std::string & path);
    
	/*! 
     * \fn DirectoryExists
	 * \brief verifies if the directory exists. 
     * \param dir: name of the directory
     * \return bool: true if directory exists, falseotheriwse 
	 */
    static bool DirectoryExists(const std::string & dir);

    /*! 
	 * \fn MakeDirectory
	 * \brief creates the directory. 
     *		  If the path includes the name of the file, it will simply be ignored.
     * \param dir: full path
     * \return bool: true if dir is successfully created, false otheriwse
	 */
    static bool MakeDirectory(const std::string & dir);

    /*! 
	 * \fn getFilePathList
	 * \brief retrieves the list of filenames that match the specified 
     *		  criterion and are saved under the given dirpath.
     * \param dirpath: path to the folder where the files are saved
     * \param search_criterion: the required search criterion
     * \param pLogger: (optional) logger
     * \return vector<string>: list of filenames as described by the input parameters 
	 */	
    static std::vector <std::string> getFilePathList( 
      const std::string & dirpath, const std::string & search_criterion, MevicLogger * pLogger=NULL);
    
    /*! 
	 * \fn containerToMAT
	 * \brief reads the elements of the container, usually image pixel values, 
     *		  and stores them in the Meshgrid matrix. 
     * \param matTo: matrix where the image elements are storedr
     * \param containerFrom: container from which the image elements are read
     * \param numComponent: number of the container's component from which the image elements are read
     * \param numFrame: number of the component's frame from which the image elements are read 
	 */
	static void containerToMAT(MAT * matTo, DSContainer * containerFrom, int numComponent);
    
    /*! 
	 * \fn frameToMAT
	 * \brief reads the elements of the specified frame of the container, 
     *		  usually image pixel values, and stores them in the Meshgrid matrix. 
     * \param matTo: matrix where the image elements are stored
     * \param containerFrom: container from which the image elements are read
     * \param numComponent: number of the container's component from which the image elements are read
     * \param numFrame: number of the component's frame from which the image elements are read 
	 */
    void frameToMAT(MAT * matTo, DSContainer * containerFrom, int numComponent, int numFrame);

};


#endif // MISC_H

