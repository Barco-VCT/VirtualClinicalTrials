#include "ParamStep.h"

using namespace std;

ParamStep::ParamStep() {
	_current	= 0;
}

bool ParamStep::CreateParameter(string parameterValue){
	//! Checkouts
	if (parameterValue.compare(0, 1, "[") != 0) return false;
	if (parameterValue.compare(parameterValue.length()-1, 1, "]") != 0) return false;

	//! Getting of the different values
	int pos=1, comma=0;

	while (comma != string::npos) {
		comma = parameterValue.find(",", pos);
		if (comma != string::npos) _values.push_back(parameterValue.substr(pos, comma-pos));
		pos = comma+1;
	}

	_values.push_back(parameterValue.substr(parameterValue.find_last_of(",")+1, parameterValue.find("]")-parameterValue.find_last_of(",")-1));

	//! Return
	return true;
}

int ParamStep::CheckConsistency(int nbExpected) {
	if (_values.size() == nbExpected) return 0;
	else if (unsigned(nbExpected) < _values.size()) return 1;
	else if (unsigned(nbExpected) > _values.size()) return 2;
	
	return -1; //!< The impossible error
}

string ParamStep::GetCurrentPosition(){
	if (unsigned(_current) > _values.size()-1) return "";
	string toret;
	toret = _values[_current];
	return toret;
}

string ParamStep::GetCurrentPositionAndIterate(){
	string current = GetCurrentPosition();
	Iterate();
	return current;
}

bool ParamStep::Iterate(){
	if(unsigned(_current) < _values.size()) _current++;
	return true;
}

string ParamStep::GetNextIterationValue(){
	Iterate();
	return GetCurrentPosition();
}

bool ParamStep::match(string & p)
{
	// Strip off Windows Drive letters in path, e.g. D:/... if any
	string pv;
	if (p.find("/") != string::npos)
		pv = p.substr(p.find_first_of("/"));
	else
		pv = p;

	if
	(
		// not allowed: : *
		   pv.find(":") != string::npos
		|| pv.find("*") != string::npos
		|| pv.find("[") == string::npos
		|| pv.find(",") == string::npos
		|| pv.find("]") == string::npos
		|| pv.find_first_of("[") != pv.find_last_of("[")
		|| pv.find_first_of("]") != pv.find_last_of("]")
		|| pv.substr(0,               1) != "["
		|| pv.substr(pv.length() - 1, 1) != "]"
	) return false;
	return true;
}

