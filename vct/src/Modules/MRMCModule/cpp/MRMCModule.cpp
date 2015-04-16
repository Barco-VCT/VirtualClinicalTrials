#include "../MRMCModule.h"
#include "../OneShot.h"

using namespace std;

const char * MRMCModule::parameter_names[5] =
{
	"file_path_scores_diseased",
	"file_path_scores_healthy",
  "file_path_results",
	"flag_estimate_biased",
	"flag_verbose_required"
};

MRMCModule::MRMCModule()
:Module()
{
	height=width=0;
	number_of_bits=0;
	input_number_of_frames=0;
	output_number_of_frames=0;
	input_component_number=0;
	output_component_number=0;
	frame_rate=0;
	white_point[0]=0;
	white_point[1]=0;
	white_point[2]=0;

	_file_path_scores_diseased = new char [300];
	_file_path_scores_healthy = new char [300];
	_file_path_results = new char [300];
	_flag_estimate_biased = 0;
	_flag_verbose_required = 0;
}

MRMCModule::~MRMCModule()
{	
}
 
int MRMCModule::FindParameter(const string & name)
{
	for (int i = 0; parameter_names[i] != NULL; i ++)
		if (parameter_names[i] == name)
			return i;
	m_pMevicLogger->logError("MRMCModule: unrecognized parameter name: " + name + "!");
	return -1;
}

void MRMCModule::SetParameter(const string & parameter_name, const string & parameter_value)
{
	switch (FindParameter(parameter_name))
	{
	case 0:
		strcpy(_file_path_scores_diseased, parameter_value.c_str()); break;
	case 1:
    strcpy(_file_path_scores_healthy, parameter_value.c_str()); break;
	case 2:
    strcpy(_file_path_results, parameter_value.c_str()); break;
	case 3:
    _flag_estimate_biased = atoi(parameter_value.c_str()); break;
	case 4:
    _flag_verbose_required = atoi(parameter_value.c_str()); break;
	}
}


void MRMCModule::Simulate( vector<DSContainer*>& list)
{
	DSContainer* container = list.back();

	// calculate the variance using OneShot algorithm
	one_shot(_file_path_scores_diseased,_file_path_scores_healthy,_file_path_results,_flag_estimate_biased,_flag_verbose_required);

}



