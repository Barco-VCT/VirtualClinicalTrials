//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file MRMCModule.h
 * \brief Implementation of class MRMCModule
 * \date 1-Feb-2008
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

#ifndef MRMCModule_H
#define MRMCModule_H

#include <DSContainer.h>
#include <Module.h>

#include <misc/misc.h>


class MRMCModule : public Module
{
  public:

	/** 
 	 * Constructor of the class MRMCModule. 
 	 */ 
    MRMCModule();

	/** 
	  * Method Simulate
	  * Method defines activity of the MRMCModule.
	  * @param name: the name of the parameter
	  * @return int: -1 if error
 	  */ 
    void Simulate( std::vector<DSContainer*>& container);

	/** 
	  * Method SetParameter
	  * Method assigns the given value to the specified parameter. 
	  * @param name: the name of the parameter
	  * @return int: -1 if error
 	  */ 
	void SetParameter(const std::string & Name, const std::string & Value);

	/** 
 	 * Destructor of the class MRMCModule. 
 	 */ 
    ~MRMCModule();

  private:

		char unit[100];
		char illum[100];
		char obs[100];
		float white_point[3];
		unsigned int number_of_bits;
		enum ComponentType element_type;
		unsigned int input_number_of_frames;
		unsigned int output_number_of_frames;
		float frame_rate;

		int last_component_number;

		unsigned long width, height, i, j, k;
		float max, min;

		int input_component_number;
		int output_component_number;


		/** parameters */
		char* _file_path_scores_diseased;	// path to the file containing the reader scores - diseased (with a signal, e.g. a nodule)
		char* _file_path_scores_healthy;	// path to the file containing the reader scores - healthy (without the signal)
		char* _file_path_results;	        // path to the file where the One-Shot analysis results shall be stored
		int	  _flag_estimate_biased;		  // produces biased estimate of the alphas (does not divide ny N-1 in computing variances)
		int	  _flag_verbose_required;			// verbose output

		static const char * parameter_names[5];

		/** 
		* Method FindParameter
		* Method returns the identifier of the MRMCModule's parameter named 'name'. 
		* @param name: the name of the parameter
		* @return int: -1 if error
 		*/ 
		int FindParameter(const std::string & name);

};


#endif

