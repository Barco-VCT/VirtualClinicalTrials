#include  "UtilitiesExchanger.h"
#include <ErrorManagement/ErrorLogger.h>

UtilitiesExchanger::UtilitiesExchanger()
{
}

bool UtilitiesExchanger::addUtilityObject(const std::string& exchangerName, UtilitiesInterface* ui)
{
bool noError=true;

	try
	{
	this->utilitiesMap[exchangerName] = ui;
	}
	catch(...)
	{
	ErrorLogger::logError("Error during object add in UtilitiesExchanger::addUtilityObject.");
	noError=false;
	}

return noError;
}

UtilitiesInterface* UtilitiesExchanger::requestUtilityObject(const std::string& exchangerName)
{
	std::map<std::string,UtilitiesInterface*>::iterator it = utilitiesMap.find(exchangerName);
	if(it == utilitiesMap.end())
	{
		ErrorLogger::logError("Error during object request in UtilitiesExchanger::requestUtilityObject.");
		return NULL;
	}
	else
	{
		return utilitiesMap[exchangerName];
	}
}

UtilitiesExchanger::~UtilitiesExchanger()
{
	// to delete all values in the map
	for(std::map<std::string,UtilitiesInterface*>::iterator it=this->utilitiesMap.begin() ; it!=this->utilitiesMap.end() ; ++it)
	{
	delete it->second; 
	}
}
