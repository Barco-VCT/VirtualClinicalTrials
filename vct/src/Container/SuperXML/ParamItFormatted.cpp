#include "ParamItFormatted.h"

using namespace std;

bool ParamItFormatted::CreateParameter(string parameterValue){
	//! Extraction of the part to process by the mother class
	ParamIt::CreateParameter(parameterValue.substr(0, parameterValue.find("]")+1));

	//! Extraction of the option
	if (parameterValue.find(":", parameterValue.find_first_of("]")) == string::npos) return false;
	_option = parameterValue.substr(parameterValue.find_first_of("]")+2, parameterValue.find(":", parameterValue.find_first_of("]"))-parameterValue.find_first_of("]")-2);

	if (_option.compare("") == 0) return false;

	if (_option.compare("number") == 0) _opt = NUMBER;
	else _opt = ParamItFormatted::UNDEFINED;

	//! Extraction of the formatting part
	_pattern = parameterValue.substr(parameterValue.find(":", parameterValue.find_first_of("]"))+1, parameterValue.length()-parameterValue.find(":", parameterValue.find_first_of("]"))-2);

	//! Return
	return true;
}

string ParamItFormatted::GetCurrentPosition(){
	//! Instanciation of the variables
	string toreturn;

	//! Getting of the output of the mother class
	toreturn = ParamIt::GetCurrentPosition();

	if (toreturn.compare("") == 0) return "";

	//! Formmatting
	if (_opt == ParamItFormatted::NUMBER) {
		for (unsigned i = 0; i <= _pattern.length(); i ++) {
			if (toreturn.length() < i) {
				toreturn.insert(0, "0");
			}
		}
	}

	//! Return
	return toreturn;
}

string ParamItFormatted::GetCurrentPositionAndIterate(){
	string toreturn = GetCurrentPosition();

	ParamIt::Iterate();

	return toreturn;
}

string ParamItFormatted::GetNextIterationValue(){
	if (ParamIt::Iterate()) return GetCurrentPosition();
	else return "";
}

bool ParamItFormatted::match(string & p)
{
	// Strip off Windows Drive letters in path, e.g. D:/... if any
	string pv;
	if (p.find("/") != string::npos)
		pv = p.substr(p.find_first_of("/"));
	else
		pv = p;

	if
	(
		// not allowed: , *
		   pv.find(",") != string::npos
		|| pv.find("*") != string::npos
		|| pv.find_first_of("[")                   == string::npos
		|| pv.find_first_of(":")                   == string::npos
		|| pv.find_first_of(":", pv.find(":") + 1) == string::npos
		|| pv.find_first_of("]")                   == string::npos
		|| pv.substr(pv.find_first_of("]") + 1, pv.length() - (pv.find_first_of("]") + 1)).find("[")                   == string::npos
		|| pv.substr(pv.find_first_of("]") + 1, pv.length() - (pv.find_first_of("]") + 1)).find(":")                   == string::npos
		|| pv.substr(pv.find_first_of("]") + 1, pv.length() - (pv.find_first_of("]") + 1)).find(":", pv.find(":") + 1) == string::npos
		|| pv.substr(pv.find_first_of("]") + 1, pv.length() - (pv.find_first_of("]") + 1)).find("]")                   == string::npos
			// a bit more checking
		|| pv.find_first_of("[") == pv.find_last_of("[")
		|| pv.find_first_of(":") == pv.find_last_of(":")
		|| pv.find_first_of("]") == pv.find_last_of("]")
		|| pv.substr(0,               1) != "["
		|| pv.substr(pv.length() - 1, 1) != "]"
	) return false;
	return true;
}

