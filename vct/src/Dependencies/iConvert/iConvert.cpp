#include "iConvert.h"

using namespace std;

namespace iConvert{

	string wcToMb(const wchar_t *in) {
		return wcToMb((wchar_t *)in);
	}

	string wcToMb(wchar_t *in){
		char * elBouforOut = new char [(wcslen(in)+1)*4];
		wcstombs(elBouforOut, in, wcslen(in)*4);
		string toRet = elBouforOut;
		delete [] elBouforOut;
		return toRet;
	}

	wchar_t* mbToWc(const char *in) {
		return mbToWc((char *)in);
	}

	wchar_t* mbToWc(string in) {
		return mbToWc(in.c_str());
	}

	wchar_t* mbToWc(char *in){
		const int size = 260;
		wchar_t * elBouforOut = new wchar_t [size];
		mbstowcs(elBouforOut, in, size);
		return elBouforOut;
	}

	int getSize (wchar_t* in){
		return wcslen(in);
	}

	int getSize (char* in){
		int i = 0;
		while (in[i]!=0) i ++;
		return i;
	}
}
