#include <cstdarg>
#include <vector>

#include "DataStore.h"

#include <ErrorManagement/ErrorLogger.h>

using namespace std;

void DataStore::saveData(string directory, string mapSt, string testTypeSt, int rowNb, int columnNb)
{
	string pathFilename= directory+"/"+mapSt+".txt";
	ofstream ofFile(pathFilename.c_str());
	string sName;
	sName = boost::to_lower_copy(testTypeSt);

	if (ofFile) 
	{
		if (sName == "int") 
		{
			try
			{
				int u = this->getData<int>(mapSt);
				ofFile<< u<<endl;
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and int case");
			}
		}
		else if (sName == "unsigned short") 
		{
			try
			{
				unsigned short u = this->getData<unsigned short>(mapSt);
				ofFile<< u<<endl;
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and unsigned short case");
			}
		}
		else if (sName == "float") 
		{
			try
			{
				float u = this->getData<float>(mapSt);
				ofFile.precision(7);
				ofFile<< u<<endl;
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and float case");
			}
		}
		else if (sName == "double") 
		{
			try
			{
				double u = this->getData<double>(mapSt);
				ofFile.precision(15);
				ofFile<< u<<endl;
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and double case");
			}
		}
		else if (sName == "int*") 
		{
			try
			{
				int* u = this->getData<int*>(mapSt);
				for (int i=0; i<rowNb; i++)
				{	
					for (int j=0; j<columnNb; j++)
					{
						ofFile<< u[i*columnNb+j]<<" ";
					}
					ofFile<<endl;
				}
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and int* case");
			}
		}
		else if (sName == "unsigned short*") 
		{
			try
			{
				unsigned short* u = this->getData<unsigned short*>(mapSt);
				for (int i=0; i<rowNb; i++)
				{	
					for (int j=0; j<columnNb; j++)
					{
						ofFile<< u[i*columnNb+j]<<" ";
					}
					ofFile<<endl;
				}
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and unsigned short* case");
			}
		}

		else if (sName == "float*") 
		{
			try
			{
				float* u = this->getData<float*>(mapSt);
				for (int i=0; i<rowNb; i++)
				{	
					for (int j=0; j<columnNb; j++)
					{
						ofFile.precision(7);
						ofFile<< u[i*columnNb+j]<<" ";
					}
					ofFile<<endl;
				}
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and float* case");
			}
		}
		else if (sName == "double*") 
		{
			try
			{
				double* u = this->getData<double*>(mapSt);
				for (int i=0; i<rowNb; i++)
				{	
					for (int j=0; j<columnNb; j++)
					{
						ofFile.precision(15);
						ofFile<< u[i*columnNb+j]<<" ";
					}
					ofFile<<endl;
				}
			}
			catch(boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveData with sName and double* case");
			}
		}


		else
		{
			ErrorLogger::logError("error in DataStore::SaveData with sName test");
		}
	}
	else 
	{
		ErrorLogger::logError("error in DataStore::SaveData");
	}
	ofFile.close();
}

void DataStore::saveDataVector(string directory, string mapSt, string testTypeSt, int columnNb)
{
	string pathFilename= directory+"/"+mapSt+".txt";
	const char * pathFilenameChar = pathFilename.c_str();
	ofstream ofFile(pathFilename.c_str());

	string sName;
	sName = boost::to_lower_copy(testTypeSt);

	if (ofFile) 
	{
		if (sName == "vector<int>*") 
		{
			try
			{
				int i;
				vector<int>* u = this->getData<vector<int>*>(mapSt);
				for (i=0; i<(int)u->size();++i)
				{
					int n=0;
					for (;n<columnNb;++n)
					{
						ofFile<< u->at(i)<<" ";
						i++;
					}
					ofFile<<endl;
					i--;
				}
			}
			catch (boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveDataVector with sName  and vector<int>* case");
			}
		}
		else if (sName == "vector<float>*") 
		{
			try
			{
				int i;				
				vector<float>* u = this->getData<vector<float>*>(mapSt);
				for (i=0;i<(int)u->size();++i)
				{
					ofFile.precision(7);
					int n=0;
					for (;n<columnNb;++n)
					{
						ofFile << u->at(i)<<" ";
						i++;
					}
					i--;
					ofFile<<endl;
				}
			}
			catch (boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveDataVector with sName  and vector<float>* case");
			}
		}

		else if (sName == "vector<double>*") 
		{
			try
			{
				int i;
				vector<double>* u = this->getData<vector<double>*>(mapSt);
				for (i=0;i<(int)u->size();++i)
				{
					ofFile.precision(15);
					int n=0;
					for (;n<columnNb;++n)
					{
						ofFile<< u->at(i)<<" ";
						i++;
					}
					ofFile<<endl;
					i--;
				}
			}
			catch (boost::bad_any_cast e)
			{
				ErrorLogger::logError("error in DataStore::SaveDataVector with sName  and vector<double>* case");
			}
		}									
		else
		{
			ErrorLogger::logError("error in DataStore::SaveDataVector with sName test");
		}
	}
	else 
	{
		ErrorLogger::logError("error in DataStore::SaveDataVector");
	}
	ofFile.close();
}

void DataStore::saveDataRaw(string directory, string mapSt, string testTypeSt, int size)
{
	string pathFilename= directory+"/"+mapSt+".raw";
	ofstream ofFile(pathFilename.c_str(),ios::binary);

	string sName;
	sName = boost::to_lower_copy(testTypeSt);

	if (ofFile) 
	{
		if (sName == "unsigned short*") 
		{
			try 
			{
				unsigned short* u = this->getData<unsigned short*>(mapSt);
				ofFile.write((char*)u, sizeof(*u)*size);
			}
			catch (ofstream::failure e)
			{
				ErrorLogger::logError("error in DataStore::SaveDataRaw with sName and unsigned short* case");
			}			
		}
		else
		{
			ErrorLogger::logError("error in DataStore::SaveDataRaw with sName test");
		}
	}
	else 
	{
		ErrorLogger::logError("error in DataStore::SaveDataRaw");
	}
	ofFile.close();
}

void DataStore::saveDataString(string directory, string mapSt)
{
	string pathFilename= directory+"/"+mapSt+".txt";
	ofstream ofFile(pathFilename.c_str());

	if(ofFile) 
	{
		std::string dataToWrite = this->getData<std::string>(mapSt);
		ofFile << dataToWrite;
	}
	else 
	{
		ErrorLogger::logError("DataStore::saveDataString: could not open file, " + pathFilename);
	}
	ofFile.close();
}