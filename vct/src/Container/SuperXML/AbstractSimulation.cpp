#include "AbstractSimulation.h"

using namespace iConvert;
using namespace std;


void AbstractSimulation::SetMevicLogger(MevicLogger * pMevicLogger)
{
	this->m_pMevicLogger = pMevicLogger;
}

string AbstractSimulation::getName(XMLNode node){
	//! If module tag present
	if (node.isAttributeSet(_T("module"))) return wcToMb(node.getAttribute(_T("module")));
	else return wcToMb(node.getName());
}

bool AbstractSimulation::instantiateArgs(vector<string>& args){
	//! Initialization of the parameters
	unsigned i = 0;

	//! Process
	if (args.size() == 0) return true;

	while (i < args.size()) {
		_args[i] = args[i];
		i++;
	}

	//! Return
	return true;
}

string AbstractSimulation::getParamValue(std::string paramValue){
	//! Instanciation / initialization of the variables
	string toReturn = "",
		part1="",
		snb="",
		part2="",
		temp = paramValue;

	int nb;

	//! Process
	while (temp.find("#") != string::npos) {
		//! Hash
		part1		=	temp.substr(0, temp.find("#"));
		snb			=	temp.substr(temp.find("#")+1, 1);
		part2		=	temp.substr(temp.find("#")+2, temp.length()-(temp.find("#")+2));

		//! Process
		nb			=	atoi(snb.c_str());

		//! Add to the return value
		toReturn	+=	part1;
		toReturn	+=	_args[nb];

		//! Recursivity
		temp		=	part2;
	}

	toReturn		+=	part2;

	//! Return
	if (toReturn.compare("") == 0) return paramValue;
	return toReturn;
}

AbstractSimulation::~AbstractSimulation() { }

