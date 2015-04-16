#include <iostream>
#include "../xml/xmlParser.h"
#include "./SuperMEVICXML.h"
#include "./ParamIt.h"
#include "./ParamItFormatted.h"
#include "./ParamString.h"
#include "./ParamStep.h"
#include "./ParamStar.h"
#include "../iConvert/iConvert.h"
#include "./FactoryParameter.h"
#include "./InOutManagement.h"
#include <sstream>
#include <vector>

using namespace std;
using namespace iConvert;

bool testParamIt();
bool testParamItFormatted();
bool testParamString();
bool testParamStep();
bool testParamStar();
bool testConfigureParam();
bool testInOutManagement();

int mainTestSuperXML (int argc, char ** argv) {
//int main (int argc, char ** argv) {

	/*ParamIt *p = new ParamIt();
	p->CreateParameter("[1.0:0.5:5.0]");

	string tempd=p->GetNextIterationValue();
	while (tempd!= ""){
		cout << tempd << endl;
		tempd = p->GetNextIterationValue();
	};

	cout << p->CheckConsistency(9);

	XMLNode root = XMLNode::openFileHelper("F:\\gusp\\Programs\\XMLParser\\XMLParser\\images\\TestSuperXML1.xml", "SuperXML"),
		loi,
		lop,
		tms;

	XMLCSTR temp = new char[100];
	int n;

	// Getting of the different nodes
	loi = root.getChildNode("LIST_OF_ITERATIONS");
	lop = root.getChildNode("LIST_OF_PARAMETERS");
	tms = root.getChildNode("TEMPLATE_MEVIC_SIMULATION");
	
	// Checkouts*/
	SuperMEVICXML *s = new SuperMEVICXML();

	//string PathToXML = "prout";

	//mbToWc(PathToXML.c_str(), PathToXML.length());

	/*string paramValue = "$AA";
	if (paramValue.compare(0,1,"$")==0 && paramValue.length()<4) cout << "ok" << endl;
	else cout << "not ok" << endl;*/

	/*if (testParamIt()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testParamItFormatted()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testParamString()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testParamStep()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testParamStar()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testConfigureParam()) cout << "ok" << endl;
	else cout <<"bad" << endl;
	if (testInOutManagement()) cout << "ok" << endl;
	else cout <<"bad" << endl;*/


	//s->StartSimulation("F:\\gusp\\Programs\\XMLParser\\XMLParser\\images\\TestJND.xml");

	// LIST_OF_ITERATIONS
	
	// LIST_OF_PARAMETERS


	// Return
	return 1;
}

bool testParamIt() {
	string totest;
	totest = "[0:9:81]";
	ParamIt *p = new ParamIt();
	p->CreateParameter(totest);

	if (p->CheckConsistency(10) == false) return false;

	ostringstream j;
	j << 0;
	int k = 0;
	for (int i = 0; i < 10; i ++) {
		if (p->GetCurrentPositionAndIterate().compare(j.str()) !=  0) return false;
		j.str("");
		k+=9;
		j << k;
	}

	if (p->GetCurrentPositionAndIterate().compare("") != 0) return false;

	/*p = new ParamIt();
	p->CreateParameter(totest);
	for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << " ";
	}

	cout << endl;*/

	return true;
}

bool testParamItFormatted() {
	string totest;
	totest = "[0:9:81][number:000000]";
	ParamItFormatted *p = new ParamItFormatted();
	p->CreateParameter(totest);

	if (p->CheckConsistency(10) == false) return false;

	ostringstream j;
	j << "000000";
	int k = 0;
	for (int i = 0; i < 10; i ++) {
		if (p->GetCurrentPositionAndIterate().compare(j.str()) !=  0) return false;
		j.str("");
		k+=9;
		if (k <10)j << "00000" << k;
		else j << "0000" << k;
	}

	if (p->GetCurrentPositionAndIterate().compare("") != 0) return false;

	/*p = new ParamItFormatted();
	p->CreateParameter(totest);
	for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << " ";
	}

	cout << endl;*/

	return true;
}

bool testParamString() {
	string totest;
	//totest = "F:\\prout\\images\\IM_[0:9:81][number:000000].raw";
	totest = "F:\\prout\\images\\IM_[0:9:81][number:000000].raw";
	ParamString *p = new ParamString();
	p->CreateParameter(totest);
	if (p->CheckConsistency(10) == false) return false;

	/*for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << endl;
	}*/

	/*if (p->CheckConsistency(10) == false) return false;

	ostringstream j;
	j << "000000";
	int k = 0;
	for (int i = 0; i < 10; i ++) {
		if (p->GetCurrentPositionAndIterate().compare(j.str()) !=  0) return false;
		j.str("");
		k+=9;
		if (k <10)j << "00000" << k;
		else j << "0000" << k;
	}

	if (p->GetCurrentPositionAndIterate().compare("") != 0) return false;

	p = new ParamItFormatted();
	p->CreateParameter(totest);
	for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << " ";
	}

	cout << endl;*/

	return true;
}

bool testParamStep() {
	string totest;
	//totest = "F:\\prout\\images\\IM_[0:9:81][number:000000].raw";
	totest = "[0,9,8,45]";
	ParamStep *p = new ParamStep();
	p->CreateParameter(totest);
	if (p->CheckConsistency(4) == false) return false;

	/*for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << endl;
	}*/

	/*if (p->CheckConsistency(10) == false) return false;

	ostringstream j;
	j << "000000";
	int k = 0;
	for (int i = 0; i < 10; i ++) {
		if (p->GetCurrentPositionAndIterate().compare(j.str()) !=  0) return false;
		j.str("");
		k+=9;
		if (k <10)j << "00000" << k;
		else j << "0000" << k;
	}

	if (p->GetCurrentPositionAndIterate().compare("") != 0) return false;

	p = new ParamItFormatted();
	p->CreateParameter(totest);
	for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << " ";
	}

	cout << endl;*/

	return true;
}

bool testParamStar() {
	string totest;
	//totest = "F:\\prout\\images\\IM_[0:9:81][number:000000].raw";
	totest = "F:\\gusp\\Programs\\XMLParser\\XMLParser\\images\\test\\[*].raw";
	ParamStar *p = new ParamStar();
	p->CreateParameter(totest);
	if (p->CheckConsistency(4) == false) return false;

	/*for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << endl;
	}*/

	/*if (p->CheckConsistency(10) == false) return false;

	ostringstream j;
	j << "000000";
	int k = 0;
	for (int i = 0; i < 10; i ++) {
		if (p->GetCurrentPositionAndIterate().compare(j.str()) !=  0) return false;
		j.str("");
		k+=9;
		if (k <10)j << "00000" << k;
		else j << "0000" << k;
	}

	if (p->GetCurrentPositionAndIterate().compare("") != 0) return false;

	p = new ParamItFormatted();
	p->CreateParameter(totest);
	for (int i = 0; i < 20; i ++) {
		cout <<p->GetCurrentPositionAndIterate() << " ";
	}

	cout << endl;*/

	return true;
}

bool testConfigureParam() {
	string				totest;
	AbstractParameter	* ap;
	ParamStar			* pstar;
	ParamString			* pstring;
	ParamStep			* pstep;
	ParamIt				* pit;
	ParamItFormatted	* pitformatted;
	
	totest = "F:\\gusp\\Programs\\XMLParser\\XMLParser\\images\\test\\[*].raw";
	ap = FactoryParameter::ConfigureParameter(totest);
	pstar = dynamic_cast<ParamStar*> (ap);
	if (pstar == NULL) return false;

	totest = "[0:9:81]";
	ap = FactoryParameter::ConfigureParameter(totest);
	pit = dynamic_cast<ParamIt*> (ap);
	if (pit == NULL) return false;

	totest = "[0:9:81][number:000000]";
	ap = FactoryParameter::ConfigureParameter(totest);
	pitformatted = dynamic_cast<ParamItFormatted*> (ap);
	if (pitformatted == NULL) return false;

	totest = "F:\\prout\\images\\IM_[0:9:81][number:000000].raw";
	ap = FactoryParameter::ConfigureParameter(totest);
	pstring = dynamic_cast<ParamString*> (ap);
	if (pstring == NULL) return false;

	totest = "F:\\prout\\images\\IM_[0:9:81].raw";
	ap = FactoryParameter::ConfigureParameter(totest);
	pstring = dynamic_cast<ParamString*> (ap);
	if (pstring == NULL) return false;

	totest = "[0,9,8,45]";
	ap = FactoryParameter::ConfigureParameter(totest);
	pstep = dynamic_cast<ParamStep*> (ap);
	if (pstep == NULL) return false;

	return true;
}

bool testInOutManagement(){
	string test;

	InOutManagement * iom = new InOutManagement();
	test = "[output:1]";
	cout << iom->manage(test) << endl;
	test = "[output:2]";
	cout << iom->manage(test) << endl;
	test = "[input:1]";
	cout << iom->manage(test) << endl;
	test = "[input:2]";
	cout << iom->manage(test) << endl;

	delete iom;

	return true;
}
