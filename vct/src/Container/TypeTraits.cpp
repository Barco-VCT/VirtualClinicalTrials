#include "TypeTraits.h"

#include <boost/algorithm/string.hpp>

using namespace std;

map<enum ComponentType, TypeInterface*>* TypeTraits::s_traits = 0;

void TypeTraits::init()
{
	if(s_traits == 0)
	{
		s_traits = new map<enum ComponentType, TypeInterface*>();
		(*s_traits)[TYPE_BYTE]			= new Type<unsigned char,1>("TYPE_BYTE");
		(*s_traits)[TYPE_FLOAT]			= new Type<float,1>("TYPE_FLOAT");
		(*s_traits)[TYPE_DOUBLE]		= new Type<double,1>("TYPE_DOUBLE");
		(*s_traits)[TYPE_LONG]			= new Type<long,1>("TYPE_LONG");
		(*s_traits)[TYPE_CHAR]			= new Type<signed char,1>("TYPE_CHAR");
		(*s_traits)[TYPE_INT]			= new Type<int,1>("TYPE_INT");
		(*s_traits)[TYPE_FFT]			= new Type<float,3>("TYPE_FFT");
		(*s_traits)[TYPE_IMAGE_GRAY]	= new Type<int,1>("TYPE_IMAGE_GRAY");
		(*s_traits)[TYPE_IMAGE_RGB]		= new Type<int,3>("TYPE_IMAGE_RGB");
		(*s_traits)[TYPE_IMAGE_XYZ]		= new Type<float,3>("TYPE_IMAGE_XYZ");
		(*s_traits)[TYPE_IMAGE_XYZR]	= new Type<float,4>("TYPE_IMAGE_XYZR");
		(*s_traits)[TYPE_IMAGE_LMS]		= new Type<float,3>("TYPE_IMAGE_LMS");
		(*s_traits)[TYPE_IMAGE_LMSR]	= new Type<float,4>("TYPE_IMAGE_LMSR");
		(*s_traits)[TYPE_IMAGE_Lab]		= new Type<float,3>("TYPE_IMAGE_Lab");
		(*s_traits)[TYPE_IMAGE_AC1C2]	= new Type<float,3>("TYPE_IMAGE_AC1C2");
		(*s_traits)[TYPE_IMAGE_JND1]	= new Type<float,1>("TYPE_IMAGE_JND1");
		(*s_traits)[TYPE_IMAGE_JND3]	= new Type<float,3>("TYPE_IMAGE_JND3");
		(*s_traits)[TYPE_IMAGE_ROC]		= new Type<float,2>("TYPE_IMAGE_ROC");
		(*s_traits)[TYPE_IMAGE_YUV]		= new Type<int,3>("TYPE_IMAGE_YUV");
		(*s_traits)[TYPE_UNSIGNED_SHORT]= new Type<unsigned short,1>("TYPE_UNSIGNED_SHORT");
		(*s_traits)[TYPE_UNSIGNED_INT]  = new Type<unsigned int,1>("TYPE_UNSIGNED_INT");
	}
}

int TypeTraits::getNChannels(enum ComponentType type)
{
	init();
	map<enum ComponentType, TypeInterface*>::const_iterator found = s_traits->find(type);
	if(found != s_traits->end())
		return found->second->getNChannels();
	else 
		return -1;
}

int TypeTraits::getChannelSize(enum ComponentType type)
{
	init();
	map<enum ComponentType, TypeInterface*>::const_iterator found = s_traits->find(type);
	if(found != s_traits->end())
		return found->second->getChannelSize();
	else 
		return -1;
}

int TypeTraits::getTypeSize(enum ComponentType type)
{
	init();
	map<enum ComponentType, TypeInterface*>::const_iterator found= s_traits->find(type);

	if(found != s_traits->end())
		return found->second->getTypeSize();
	else 
		return -1;
}

char TypeTraits::getTypeLetter(enum ComponentType type)
{
	init();
	map<enum ComponentType, TypeInterface*>::const_iterator found= s_traits->find(type);

	if(found != s_traits->end())
		return found->second->getTypeLetter();
	else 
		return '\0';
}

int TypeTraits::getTypeFromString(const std::string &str)
{
	init();
	std::map<enum ComponentType, TypeInterface*>::iterator it = s_traits->begin();
	while(it != s_traits->end())
	{
		if (boost::to_lower_copy(it->second->getTypeString()) == 
			boost::to_lower_copy(str))
			return it->first;
		++it;
	}
	throw TypeException("Provided string does not match any VCT type.");
}
