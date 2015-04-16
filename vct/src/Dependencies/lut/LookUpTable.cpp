#include "LookUpTable.h"

#include <cfloat>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <math.h>

using namespace std;

LookUpTable::LookUpTable(size_t size, float *data)
: m_size(size)
, m_factor(1)
, m_offset(0)
{
	m_data = new float[m_size];
	if(data)
		memcpy(m_data,data,size*sizeof(float));
	else
		memset(m_data,0,size*sizeof(float));
}

LookUpTable::LookUpTable(const std::vector<float>& data)
: m_factor(1)
, m_offset(0)
{
	m_size = data.size();
	m_data = new float[m_size];
	std::copy(data.begin(),data.end(),m_data);
}

LookUpTable::LookUpTable(const char* filepath, bool isTxt, int size)
{
	if (isTxt == false)
	{
		ifstream is(filepath, ios::binary);
		if(is.good())
		{
			is.read((char*)&m_size,sizeof(size_t));
			is.read((char*)&m_factor,sizeof(float));
			is.read((char*)&m_offset,sizeof(float));
			m_data = new float[m_size];
			is.read((char*)m_data,m_size*sizeof(float));
			is.close();
		}
		else
		{
			ErrorLogger::logError("LookUpTable::LookUpTable: file is not good to read");
		}
	}
	else
	{
		string line;
		ifstream is(filepath);
		float valIn(0), valOut(0);
		if(is.good())
		{
			m_size = size;
			m_factor = 1;
			m_offset = 0;
			int indexLine(0);
			m_data = new float[m_size];
			while(!is.eof() && (int) indexLine < (int) m_size && m_size>0)
			{
				getline(is, line);
				stringstream sline;
				sline << line;
				sline >> valIn >> valOut;
				m_data[indexLine] = valOut;
				indexLine++;
			}
			is.close();
		}
		else
		{
			ErrorLogger::logError("LookUpTable::LookUpTable: file is not good to read");
		}
	}
}

float LookUpTable::eval(float i) const
{
	float i_to_lut_res = (i-m_offset)/m_factor;
	if(i_to_lut_res < 0)
		return m_data[0];
	if(i_to_lut_res > m_size-1)
		return m_data[m_size-1];
	return m_data[(size_t)floorf(i_to_lut_res+0.5f)];
}

float LookUpTable::interpolate(float i) const
{
	float i_to_lut_res = (i-m_offset)/m_factor;
	if(i_to_lut_res < 0)
		return m_data[0];
	if(i_to_lut_res > m_size-1)
		return m_data[m_size-1];
	float i_to_lut_res_floor = floorf(i_to_lut_res);
	return m_data[(size_t)i_to_lut_res_floor]+(m_data[(size_t)i_to_lut_res_floor+1]-m_data[(size_t)i_to_lut_res_floor])*(i_to_lut_res-i_to_lut_res_floor);
}

float LookUpTable::reverseEval(float i) const
{
	int lut_index;

	if(i <= m_data[0])
		lut_index = 0;
	else
	{
		int e;
		for(e=0; e< int(m_size-1); ++e)
		{
			if(i > m_data[e] && i <= m_data[e+1])
				break;
		}

		if(fabs(i - m_data[e]) < fabs(i - m_data[e+1]))
			lut_index = e;
		else
			lut_index = e+1;
	}

	return lut_index*m_factor+m_offset;
}

float LookUpTable::reverseInterp(float i) const
{
	float lut_index;

	if(i <= m_data[0])
		lut_index = 0;
	else
	{
		int e;
		for(e=0; e<int(m_size-1); ++e)
		{
			if(i > m_data[e] && i <= m_data[e+1])
				break;
		}

		lut_index = (e * fabs(i - m_data[e+1]) + (e+1) * fabs(i - m_data[e])) / ((fabs(i - m_data[e]) + fabs(i - m_data[e+1])));
	}

	return lut_index*m_factor+m_offset;
}

void LookUpTable::saveToFile(const char *filepath) const
{
	ofstream os(filepath, ios::binary);
	if(os.good())
	{
		os.write((char*)&m_size,sizeof(size_t));
		os.write((char*)&m_factor,sizeof(float));
		os.write((char*)&m_offset,sizeof(float));
		os.write((char*)m_data,m_size*sizeof(float));
		os.close();
	}
	else
	{
		//throw runtime_error("LookUpTable::saveToFile: file is not good to write");
		ErrorLogger::logError("LookUpTable::saveToFile: file is not good to write");
	}
}


LookUpTable::~LookUpTable()
{
	delete [] m_data;
}
