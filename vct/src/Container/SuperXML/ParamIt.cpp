#include "ParamIt.h"

using namespace std;

ParamIt::ParamIt() {
	_min		= 0.0;
	_max		= 0.0;
	_step	= 0.0;
	_current = 0.0;
}

bool ParamIt::CreateParameter(string parameterValue)
{
	string pv = parameterValue.substr(parameterValue.find_first_of("["));

	double* res;
	res			= hash(pv);
	_min		= res[0];
	_step		= res[1];
	_max		= res[2];
	_current	= _min;

	return true;
}

int ParamIt::CheckConsistency(int nbExpected){
	int nbCalc = (int)(_max/_step-_min/_step+1);
	if (nbCalc == nbExpected) return 0;
	else if (nbExpected < nbCalc) return 1;
	else if (nbExpected > nbCalc) return 2;

	return -1; //!< The impossible error
}

string ParamIt::GetCurrentPosition(){

	if (_current > _max) return "";
	else {
		ostringstream temp;
		temp << _current;
		return temp.str();
	}

}

bool ParamIt::Iterate(){
	if (_current > _max) return false;
	_current += _step;

	return true;
}

string ParamIt::GetNextIterationValue(){
	Iterate();
	return GetCurrentPosition();
}

string ParamIt::GetCurrentPositionAndIterate(){
	string current = GetCurrentPosition();
	Iterate();
	return current;
}

double* ParamIt::hash(string str) {
	string* ress = new string[3];
	double* res = new double [3];

	ress[0] = getSubstr(str, "[", ":");
	ress[1] = getSubstr(str, ":", ":");
	ress[2] = str.substr(str.find(":",str.find_first_of(":")+1)+1,str.find("]")-(str.find(":",str.find_first_of(":")+1)+1));

	res[0] = strtod(ress[0].c_str(), NULL);
	res[1] = strtod(ress[1].c_str(), NULL);
	res[2] = strtod(ress[2].c_str(), NULL);

	delete [] ress;
	return res;
};

bool ParamIt::match(string & p)
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
		|| pv.find("[")                   == string::npos
		|| pv.find(":")                   == string::npos
		|| pv.find(":", pv.find(":") + 1) == string::npos
		|| pv.find("]")                   == string::npos
		|| pv.find_first_of("[") != pv.find_last_of("[")
		|| pv.find_first_of(":") == pv.find_last_of(":")
		|| pv.find_first_of("]") != pv.find_last_of("]")
		|| pv.substr(0,               1) != "["
		|| pv.substr(pv.length() - 1, 1) != "]"
	) return false;
	return true;
}
