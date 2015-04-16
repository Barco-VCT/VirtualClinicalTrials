#include "ErrorLogger.h"

#include <iostream>
#include <stdexcept>
#include <cstdio> // getchar()

using namespace std;

namespace ErrorLogger
{
	void message(const string message)
	{
		cout << endl << message;
		cout.flush();
	}

	void warning(const string message)
	{
		cerr << endl << "Platform warning: " << message;
		cerr.flush();
	}

	void logError(const string message)
	{
		cerr << endl << "Platform error: " << message;
		cerr << endl << "***ERROR*** VCT is going to abort";
		cerr.flush();
		cout<<endl<<"VCT platform error: "<<message<<endl;
		exit (1);
	}

	void debug(const string message)
	{
#ifdef _DEBUG
		cout << endl << message;
		cout.flush();
#endif // _DEBUG
	}

	void assertion(bool condition, string message)
	{
		if(condition == false)
			logError(message);
	}
}
