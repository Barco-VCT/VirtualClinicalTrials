#include "ParamString.h"

using namespace std;

ParamString::ParamString() {
	_part1 = "";
	_part2 = "";
	_ap = NULL;
}

bool ParamString::CreateParameter(string parameterValue){
	//! Extraction of the different parts of the string
	_part1 = parameterValue.substr(0, parameterValue.find_first_of("["));
	_part2 = parameterValue.substr(parameterValue.find_last_of("]")+1, parameterValue.length()-parameterValue.find_last_of("]")-1);

	//! Identification of the pattern and instanciation of the correct parameter
	string temp = parameterValue.substr(parameterValue.find_first_of("["), (parameterValue.find_last_of("]")-parameterValue.find_first_of("[")+1));
	if (ParamItFormatted::match(temp)) {
		_ap = new ParamItFormatted();
	}
	else if (ParamIt::match(temp)) {
		_ap = new ParamIt();
	}
	else if (ParamStep::match(temp)) {
		_ap = new ParamStep();
	}
	else return false;

	//! Creation of the parameter
	_ap->CreateParameter(temp);

	//! Return
	return true;
}

string ParamString::GetCurrentPosition() {
	//! Getting of the value
	string toreturn;

	toreturn = _ap->GetCurrentPosition();

	return _part1+toreturn+_part2;
}

string ParamString::GetCurrentPositionAndIterate() {
	//! Getting of the value
	string toreturn = GetCurrentPosition();

	Iterate();

	return toreturn;
}

int ParamString::CheckConsistency(int nbExpected) {
	return _ap->CheckConsistency(nbExpected);
}

string ParamString::GetNextIterationValue(){
	Iterate();
	return GetCurrentPosition();
}

bool ParamString::Iterate(){
	return _ap->Iterate();
}

bool ParamString::match(string & p)
{
	// Strip off Windows Drive letters in path, e.g. D:/... if any
	string pv;
	if (p.find("/") != string::npos)
		pv = p.substr(p.find_first_of("/"));
	else
		pv = p;

	if
	(
		// not allowed: *
		   pv.find("*") != string::npos
		// syntax: String + ParamStep/ParamIt/ParamItFormatted
		|| pv.find("[") == string::npos
		|| pv.find("]") == string::npos
	) return false;
	return true;
}

ParamString::~ParamString() {
	if (_ap != NULL) {
		delete _ap;
	}
}