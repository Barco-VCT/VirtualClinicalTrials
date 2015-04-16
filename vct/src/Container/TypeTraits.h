//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file TypeTraits.h
 * \brief Implementation of the class TypeInterface, Type, TypeTraits, TypeException
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

#ifndef TYPETRAITS_H
#define TYPETRAITS_H

#include <map>
#include <exception>
#include <string>

#include "DSContainer.h"

template<typename T>
struct TypeLetter{static const char value = '\0';};

template<>
struct TypeLetter<unsigned char>{static const char value = 'c';};
template<>
struct TypeLetter<signed char>{static const char value = 'c';};
template<>
struct TypeLetter<char>{static const char value = 'c';};

template<>
struct TypeLetter<float>{static const char value = 'f';};

template<>
struct TypeLetter<double>{static const char value = 'd';};

template<>
struct TypeLetter<int>{static const char value = 'i';};

template<>
struct TypeLetter<long>{static const char value = 'i';};

template<>
struct TypeLetter<unsigned short>{static const char value = 'u';};

template<>
struct TypeLetter<unsigned int>{static const char value = 'u';};

/*!
 * \class TypeInterface
 * \brief Interface to the Type template instances
 */
class TypeInterface
{
public:
	/*!
	 * \fn getNChannels
	 * \return unsigned: Number of channels of the type
	 */
	virtual unsigned getNChannels()const = 0;

	/*!
	 * \fn getChannelSize
	 * \return unsigned: Size (in bytes) of one channel of one element
	 */
	virtual unsigned getChannelSize()const = 0;

	/*!
	 * \fn getTypeSize
	 * \return unsigned: Size (in bytes) of one element of the type
	 */
	virtual unsigned getTypeSize()const = 0;

	/*!
	 * \fn getTypeLetter
	 * \return char: Letter code corresponding to the underlying native type
	 */
	virtual char getTypeLetter()const = 0;

	/*!
	 * \fn getTypeString
	virtual std::string getTypeString()const = 0;
	 * \return string: string representation of the type
	 */
	virtual std::string getTypeString()const = 0;
};

/*!
 * \class Type
 * \brief Template to represent a VCT type
 */
template<typename T, unsigned nChannels>
class Type : public TypeInterface
{
public:
	/*!
	 * Constructor
	 */
	Type(std::string typeString):m_typeString(typeString){}

	/*!
	 * \fn getNChannels
	 * \return unsigned: Number of channels of the type
	 */
	unsigned getNChannels()const{return nChannels;}
	
	/*!
	 * \fn getChannelSize
	 * \return unsigned: Size (in bytes) of one channel of one element
	 */
	unsigned getChannelSize()const{return sizeof(T);}
	
	/*!
	 * \fn getTypeSize
	 * \return unsigned: Size (in bytes) of one element of the type
	 */
	unsigned getTypeSize()const{return nChannels*sizeof(T);}

	/*!
	 * \fn getTypeLetter
	 * \return char: Letter code corresponding to the underlying native type
	 */
	char getTypeLetter()const{return typeLetter;}

	/*!
	 * \fn getTypeString
	 * \return string: The string representation of the type
	 */
	virtual std::string getTypeString()const{return m_typeString;}

	typedef T nativeType;

	static const char typeLetter = TypeLetter<T>::value;
private:
	/*!
	 * Default constructor
	 * \brief Making it private prevents its use
	 */
	Type(){};

	std::string m_typeString;
};

/*!
 * \class TypeTraits
 * \brief Class giving access to a map of the properties of all MEVIC types
 */
class TypeTraits
{
public:
	/*!
	 * \fn getNChannels
	 * \param type A VCT type code
	 * \return Number of channels of the type
	 */
	static int getNChannels(enum ComponentType type);
	
	/*!
	 * \fn getChannelSize
	 * \param type A VCT type code
	 * \return Size (in bytes) of one channel of one element of the type
	 */
	static int getChannelSize(enum ComponentType type);
	
	/*!
	 * \fn getTypeSize
	 * \param type A VCT type code
	 * \return Size (in bytes) of one element of the type
	 */
	static int getTypeSize(enum ComponentType type);
	
	/*!
	 * \fn getTypeLetter
	 * \param type A VCT type code
	 * \return Letter code corresponding to the underlying native type
	 */
	static char getTypeLetter(enum ComponentType type);

	/*!
	 * \fn getTypeFromString
	 * \param str The string representation of a VCT type (case does not matter)
	 * \return A VCT type
	 * \exception TypeException The provided string doesn't match any type
	 */
	static int getTypeFromString(const std::string& str);

	/*!
	 * \fn init
	 * \brief Initialize the type traits map 
	 * return void
	 */
	static void init();

private:
	/*!
	 * Private constructor 
	 */
	TypeTraits(){};

	static std::map<enum ComponentType,TypeInterface*>* s_traits; //!< Type traits map 
};

/*!
 * \class TypeException
 * \brief Class for handling error management and exception for the type traits
 */
class TypeException : public std::exception
{
public:
	/*!
	 * Constructor
	 */
	TypeException(std::string m="TypeException") : msg(m) {}

	/*!
	 * Destructor
	 */
	~TypeException() throw() {}

	const char* what() const throw() { return msg.c_str(); }

private:
	std::string msg;
};

#endif //TYPETRAITS_H
