//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file TimeLimit.h
 * \brief Implementation of the function checkTimeLimit
 * \date 1-Dec-2009
 * (C) Copyright 2014 Barco NV and others.
 *
 * This software code is licensed to you under the Barco VCT Research License. The text of this license
 * is found in the file License.TXT which is available in the same repository, in absence of which you need
 * to contact Barco first before you are allowed to engage in any act of familiarization or use of whatever
 * kind. Any familiarization or use of whatever kind of this software implies acceptance of the pre-mentioned
 * license. You warrant that the modifications you make are “original” and are able to be subject to the provisions
 * of the pre-mentioned license. Any contribution containing “non-original” modification require the prior written
 * approval from Barco.
 *
 */

#ifndef TIMELIMIT_H
#define TIMELIMIT_H

#include <windows.h>

#include <boost/lexical_cast.hpp> // to use Boost's string casting operations
#include <boost/date_time/posix_time/posix_time.hpp>

#include <secblock.h>
#include <hex.h>
#include <aes.h>
#include <modes.h>
#include <filters.h>

static void checkTimeLimit()
{
	const unsigned char key[]={219,136,136,202,243, 93, 42,101,  7,171,165,140, 87,105,155,231};
	const unsigned char iv[] ={177, 57,251,116,194,163,141,198,226,178,154,  4,194, 19,161,153};
	using namespace std;
	try{
		/*
		 * A. Check that the key file exists
		 */
		const int envVarSize = 256;
		char buffer[envVarSize];

		DWORD envSuccess = GetEnvironmentVariableA("USERPROFILE",buffer,envVarSize);

		if(!envSuccess || envSuccess > (envVarSize-1))
			throw runtime_error("Not able to retrieve environment variables.");

		string pathToCipherFile = string(buffer) + "\\mevic\\key";

		ifstream cipherFile(pathToCipherFile.c_str(),ios::binary);
		if(!cipherFile.is_open())
		{
			stringstream msg;
			msg << "The program cannot open the activation key file at " << pathToCipherFile;
			throw runtime_error(msg.str());
		}
		cipherFile.exceptions(ios::badbit | ios::failbit);

		/*
		 * B. Read the content of the file
		 */
		const int cipherSize = 1024;
		char ciphertext[cipherSize];
		cipherFile.read(ciphertext,cipherSize);
		cipherFile.close();

		/*
		 * C. Decipher the limit date
		 */
		char deciphertext[cipherSize];
		CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption aesd(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
		aesd.ProcessData((byte*)deciphertext,(byte*)ciphertext,cipherSize);

		string sYear(deciphertext,deciphertext+4);
		string sMonth(deciphertext+4,deciphertext+6);
		string sDay(deciphertext+6,deciphertext+8);

		int year  = boost::lexical_cast<int>(sYear);
		int month = boost::lexical_cast<int>(sMonth);
		int day   = boost::lexical_cast<int>(sDay);

		boost::posix_time::ptime limitDate(boost::gregorian::date(year,month,day));
		boost::posix_time::ptime today(boost::gregorian::day_clock::local_day());

		/*
		 * D. Compare the limit date with today
		 */
		if(limitDate < today)
		{
			stringstream msg;
			msg << "Your key has expired (limit date was " << limitDate << ')';
			throw runtime_error(msg.str());
		}

	} catch(std::exception& e) {
		std::cout << "error: " << e.what() << std::endl;
		getchar();
		exit(EXIT_FAILURE);
	}
}

#endif // TIMELIMIT_H