#include "InOutManagement.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

InOutManagement::InOutManagement()
{
	//! Cleaning if temp directory already exist
	if (boost::filesystem::exists("pgmGenerated"))
	{
		boost::filesystem::remove_all("pgmGenerated");
	}

	//! Initialization of the variables
	_current = 0;
}

string InOutManagement::manage(string value){
	//! First create a directory to host the temp file
	if (boost::filesystem::exists("pgmGenerated") == false)
	{
		boost::filesystem::create_directory("pgmGenerated");
	}

	//! Instanciation of the variables
	string type,
		snumber,
		tempPath,
		syscmd;
	int number;
	char* temp = new char[10];

	//! Check
	if (check(value) == false) return "";

	//! Hash
	type = value.substr(1, value.find(":")-1);
	snumber = value.substr(value.find(":")+1, value.length()-2-value.find(":"));
	number = atoi(snumber.c_str());

	//! If input
	if (type.compare("output")==0) {
		tempPath = "./pgmGenerated/";
		tempPath += boost::lexical_cast<std::string>(number);
		tempPath += ".temp";
		_paths[number] = tempPath;
	}
	else if (type.compare("outputfolder")==0) {
		tempPath = "./pgmGenerated/";
		tempPath += boost::lexical_cast<std::string>(number);
		boost::filesystem::create_directory(tempPath.c_str());
		_paths[number] = tempPath;
	}
	else if (type.compare("input")==0) {

	}
	else return "";

	//! return
	return _paths[number];
}

InOutManagement::~InOutManagement(){
	//! Delete of the temporary folder previously created
	boost::filesystem::remove_all("pgmGenerated");
}

bool InOutManagement::check(string val){
	if (val.substr(0,1).compare("[") == 0 &&
		val.substr(val.length()-1,1).compare("]") == 0 &&
		val.substr(1, val.length()-2).find(":") != string::npos) return true;
	return false;
}
