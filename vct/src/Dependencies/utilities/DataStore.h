//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file DataStore.h
 * \brief Implementation of the class DataStore
 * \date 01-May-2010
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

#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>	 // boost library
#include <boost/algorithm/string.hpp> // boost library

class DataStore {

public:
	
	/*!
	 * Constructor of the class DataStore
	 */
	DataStore():m_data(){} //to initialize data

	/*!
	 * Destructor of the class DataStore
	 */
	~DataStore(){}

	/*!
	 * \fn SetData
	 * \param key: name of the file
	 * \param element: data saved (all types)
	 * \return void
	 */
	template<class T>
	void setData(const std::string& key, T element)
	{
		m_data[key] = element;
	}

	/*!
	 * \fn GetData
	 * \param key: name of the file
	 * \return T
	 */
	template<class T>
	T getData(const std::string& key)
	{			
		return boost::any_cast<T>(m_data[key]);
	}

	/*!
	 * \fn SaveData
	 * \param directory: file path where the file will be saved without / in the end
	 * \param mapSt: name of the file
	 * \param testTypeSt: type of the data saved
	 * \param rowNb: number of row of the file
	 * \param columnNb: number of column of the file
	 * \return void
	 */
	void saveData(std::string directory, std::string mapSt, std::string testTypeSt, int rowNb, int columnNb);

	/*!
	 * \fn SaveDataVector
	 * \param directory: file path where the file will be saved without / in the end
	 * \param mapSt: name of the file
	 * \param testTypeSt: type of the data saved
	 * \param columnNb: number of column of the file
	 * \return void
	 */
	void saveDataVector(std::string directory, std::string mapSt, std::string testTypeSt, int columnNb);

	/*!
	 * \fn SaveDataString
	 * \param directory: file path where the file will be saved without / in the end
	 * \param mapSt: name of the file
	 */
	void saveDataString(std::string directory, std::string mapSt);

	/*!
	 * \fn SaveDataRaw
	 * \param directory: file path where the file will be saved without / in the end
	 * \param mapSt: name of the file
	 * \param testTypeSt: type of the data saved
	 * \param size: size of the raw file
	 * \return void
	 */
	void saveDataRaw(std::string directory, std::string mapSt, std::string testTypeSt, int size);

	/*!
	 * \fn Exist
	 * \brief check if the file exist
	 * \param key: name of the file
	 * \return bool: true if file exist
	 */
	bool exist(std::string key)
	  {
		  return m_data.find(key)!=m_data.end();
	  }

private:
	std::map<std::string, boost::any> m_data; //!< data
};
#endif // DATASTORE_H
