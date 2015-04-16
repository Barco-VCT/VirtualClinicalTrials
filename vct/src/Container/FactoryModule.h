//////////////////////////////////////////////////////////////////
//							Container							//
//			MEVIC: MEdical Virtual Image  Chain					//
//				January 2006 - TIG group - Barco MID			//
//					Cédric Marchessoux							//
//////////////////////////////////////////////////////////////////

/*
 * @file: FatoryModule.cpp
 * @author: Cédric Marchessoux
 * @date 01-01-06
 */

#ifndef FACTORYMODULE_H
#define FACTORYMODULE_H

#include "Module.h"
#include "../Container/ErrorManagement/MevicLogger.h"

class FactoryModule 
{ 
public:
	/*!
	 * Construct a Module
	 * @param[in] moduleName   Name of the Module to construct (e.g. VideoCardModule, PatternGeneratorModule ...)
	 * @param[in] pMevicLogger Pointer to MevicLogger object to assign with the Module to construct
	 */
	Module * construct(std::string moduleName, MevicLogger * pMevicLogger);
};

#endif // FACTORYMODULE_H
