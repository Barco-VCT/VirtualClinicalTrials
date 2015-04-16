// VCT main.cpp
#include <cctype>	// isdigit()
#include <cstring>	// strcmp(), strncmp()
#include <string>	// strcmp(), strncmp()
#include <DSContainer.h>
#include <SuperXML/SuperMEVICXML.h>
#include <vector>

#include <iostream>


#ifdef MEVIC_TIME_LIMIT
#include <TimeLimit.h>
#endif // MEVIC_TIME_LIMIT 

using namespace std;

int main (int argc, char **argv){

	#ifdef MEVIC_TIME_LIMIT
		checkTimeLimit();
	#endif // MEVIC_TIME_LIMIT

	if (!strcmp(argv[1], "SuperXML")) 
	{
		string progName = argv[0];
		vector<string> progArgs;
		string xmlPath;
		vector<string> simArgs;
		int option = 0; // optional single digit arg

		for (int i = 2; i < argc; i++) 
		{
			if (std::strncmp(argv[i], "--", 2) == 0) 
			{
				progArgs.push_back(argv[i]);
				
			} else if (xmlPath == "") { // first arg not starting with -- is the xml file
				xmlPath = argv[i];
				
			} //else if (isdigit(argv[i][0]) && argv[i][1] == '\0') { // single digit 'option'?
				else if ((i == argc-1) && (argv[i][0]  == '0' || argv[i][0]  == '1' || argv[i][0]  == '2') && (isdigit(argv[i][0]))) { // single digit 'option' for last argument
					option = argv[i][0] - '0';
				
			} else { // simulation arg
				simArgs.push_back(argv[i]);	
				cout << argv[i] << endl;					
			}
		}
		SuperMEVICXML smx;
		smx.StartSimulation(progName, progArgs, xmlPath, simArgs, option);
	}	
	
	return 0;

}
 