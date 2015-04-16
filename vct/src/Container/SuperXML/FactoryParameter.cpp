#include "FactoryParameter.h"

using namespace std;

AbstractParameter* FactoryParameter::ConfigureParameter(string parameterValue)
{
	if (ParamIt::match(parameterValue))
	{
		ErrorLogger::debug("paramIt");
		return new ParamIt();
	}
	else if (ParamBasic::match(parameterValue))
	{
		ErrorLogger::debug("paramBasic");
		return new ParamBasic();
	}
	else if (ParamStep::match(parameterValue))
	{
		ErrorLogger::debug("ParamStep");
		return new ParamStep();
	}
	else if (ParamItFormatted::match(parameterValue))
	{
		ErrorLogger::debug("ParamItFormatted");
		return new ParamItFormatted();
	}
	else if (ParamStar::match(parameterValue))
	{
		ErrorLogger::debug("ParamStar");
		return new ParamStar();
	}
	else if (ParamString::match(parameterValue))
	{
		ErrorLogger::debug("ParamString");
		return new ParamString();
	}
	else
	{
		ErrorLogger::logError("[FactoryParameter::ConfigureParameter] No match for " + parameterValue);
	}

	return NULL;
}
