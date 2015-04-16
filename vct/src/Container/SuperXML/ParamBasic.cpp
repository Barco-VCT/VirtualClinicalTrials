#include "ParamBasic.h"

using namespace std;

ParamBasic::ParamBasic(){
	_value = "";
}

bool ParamBasic::CreateParameter(string parameterValue){
	_value = parameterValue;
	return true;
}

int ParamBasic::CheckConsistency(int nbExpected){
	return 0;
}

string ParamBasic::GetCurrentPosition(){
	return _value;
}

string ParamBasic::GetCurrentPositionAndIterate(){
	return GetCurrentPosition();
}

string ParamBasic::GetNextIterationValue(){
	return GetCurrentPosition();
}

bool ParamBasic::Iterate(){
	return true;
}

bool ParamBasic::match(string & p)
{
	// Strip off Windows Drive letters in path, e.g. D:/... if any
	string pv;
	if (p.find("/") != string::npos)
		pv = p.substr(p.find_first_of("/"));
	else
		pv = p;

	if
	(
		// not allowed: [ : ] , *
		   pv.find("[") != string::npos
		|| pv.find(":") != string::npos
		|| pv.find("]") != string::npos
		|| pv.find(",") != string::npos
		|| pv.find("*") != string::npos
		// syntax: parameter
	) return false;
	return true;
}

