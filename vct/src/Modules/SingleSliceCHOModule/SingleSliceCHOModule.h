//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file SingleSliceCHOModule.h
 * \brief Implementation of class SingleSliceCHOModule
 * \date 1-June-2007
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


#ifndef SINGLESLICESHOMODULE_H
#define SINGLESLICESHOMODULE_H

#include <Module.h>
#include <Meschach/matrix.h>

#ifndef	MATRIXH
struct MAT;
struct VEC;
#endif

#include <HotellingObserver/ChannelFilter.h>

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif


/*!
 * \class SingleSliceCHOModule
 * \brief implementation of a static Channelized Hotelling Observer with LG channels
 */
class SingleSliceCHOModule : public Module
{
public:

	/*!
	 * Constructor 
	 */
	SingleSliceCHOModule( );

	/*!
	 * \fn Simulate
	 * \brief for running the simulation
	 * \param  [in/out] container: vector of the containers 
	 * \return void
	 */
	void Simulate( std::vector<DSContainer*>& container );

	/*!
	 * \fn SetParameter
	 * \brief Method assigns the given value to the specified parameter.
	 * \param  [in] name: name of the parameter
	 * \param  [in] value: value of the parameter
	 * \return void
	 */
	void SetParameter( const std::string & name, const std::string & value );

	/*!
	 * Destructor
	 */
	~SingleSliceCHOModule( );

	private:

		char unit[100];
		char illum[100];
		char obs[100];
		float white_point[3];
		unsigned int number_of_bits;
		unsigned int input_number_of_frames;
		unsigned int output_number_of_frames;
		float frame_rate;
		int input_component_number;
		int output_component_number;

		VEC * m_wCh; //!< CHO template

		ChannelFilter m_channels; //!> 2D LG channels

		/** module parameters */
		unsigned long m_reader_id;								//!< identifier of the reader
		unsigned long m_image_height;							//!< height of the images [pixels]
		unsigned long m_image_width;							//!< width of the images [pixels]
		unsigned long m_image_depth;							//!< depth of the images [pixels]
		unsigned long m_target_slice_id;						//!< identifier of the single slice to be used when m_image_depth>1
		bool          m_one_image_file_per_frame;				//!< indicates if each frame is stored in a separate file; otherwise, all frames are assumed to be stored in a same file
		unsigned long m_num_image_pairs_training;				//!< number of training image pairs (signal-absent & signal-present image)
		unsigned long m_num_image_pairs_testing;				//!< number of testing image pairs (signal-absent & signal-present image)
		unsigned long m_channels_number;						//!< number of channels used by the CHO
		int           m_channels_type;							//!< type of channels used by the CHO: LG
		float         m_channels_lg_spread;						//!< for _channels_type='LG': spread of LG channels, often denoted a
		bool          m_save_channelized_images;				//!< indicates if the channel responses shall be saved
		bool          m_auto_create_outputfolders;				//!< if output folder do not exist, create them automatically
		std::tstring  m_path_in_training_healthy;				//!< path to the folder containing the set of trainer images (signal-absent)
		std::tstring  m_path_in_training_diseased;				//!< path to the folder containing the set of trainer images (signal-present) 
		std::tstring  m_path_in_testing_healthy;				//!< path to the folder containing the set of test images (signal-absent)
		std::tstring  m_path_in_testing_diseased;				//!< path to the folder containing the set of test images (signal-present)
		std::tstring  m_path_out_channelized_training_healthy;  //!< path to the file where the channel outputs for trainer images shall be stored (signal-absent)
		std::tstring  m_path_out_channelized_training_diseased; //!< path to the file where the channel outputs for trainer images shall be stored(signal-present) 
		std::tstring  m_path_out_channelized_testing_healthy;	//!< path to the file where the channel outputs for test images shall be stored(signal-absent)
		std::tstring  m_path_out_channelized_testing_diseased;  //!< path to the file where the channel outputs for test images shall be stored(signal-present)
		std::tstring  m_path_out_training_tpf_fpf;				//!< path to the file where tpf and fpf values shall be stored (training phase)
		std::tstring  m_path_out_training_auc_snr;				//!< path to the file where AUC and SNR values shall be stored (training phase)
		std::tstring  m_path_out_testing_tpf_fpf;				//!< path to the file where tpf and fpf values shall be stored (testing phase)
		std::tstring  m_path_out_testing_auc_snr;				//!< path to the file where AUC and SNR values shall be stored (testing phase)
		std::tstring  m_path_out_reader_score_healthy_ref;		//!< path to the file where the scores of the reference reader are stored (healthy)
		std::tstring  m_path_out_reader_score_healthy_new;		//!< path to the file where the scores of the new reader shall be stored (healthy)
		std::tstring  m_path_out_reader_score_diseased_ref;		//!< path to the file where the scores of the reference reader are stored (diseased)
		std::tstring  m_path_out_reader_score_diseased_new;		//!< path to the file where the scores of the new reader shall be stored (diseased)

		static const char * m_parameter_names[30];

		/*!
		 * \fn RunTraining
		 * \brief Method defining the training phase with predefined set of trainers 
		 * \return void
		 */
		void RunTraining();

		/*!
		 * \fn RunTesting
		 * \brief method defining the testing phase with predefined set of trainers 
		 * \param[out] roc_tpf: TPF values for ROC curve
		 * \param[out] roc_fpf: FPF values for ROC curve 
		 * \return void
		 */
 		void RunTesting(VEC *roc_tpf, VEC *roc_fpf);
		
		/*!
		 * \fn FindParameter
		 * \brief Method looking for the int identifier of the specified parameter name.
		 * \param[in] name: name of the parameter
		 * \return int: identifier of the parameter; -1 if the parameter is not found 
		 */
		int FindParameter(const std::string & name);
		
		/*!
		 * \fn ValidateModuleParameters
		 * \brief Method verifying if the specified parameter values match the required criteria.
		 *		  For incorrect values error messages are generated.
		 * \return bool: true if ok
		 */
		bool ValidateModuleParameters();
		
		/*!
		 * \fn ComputeChannelOutputs
		 * \brief Method calculating the channel responses for image data.
		 * \param[in/out] vS: channel responses for diseased data
		 * \param[in/out] vN: channel responses for healthy data
		 * \param[in/out] sbar: mean sum of the channelized data
		 * \param[in] is_training: 1 if training phase; 0 otherwise 
		 * \return void
		 */
		void ComputeChannelOutputs(MAT *vS, MAT *vN, VEC *sbar, bool is_training=false);
	  
		/*!
		 * \fn SaveChannelResponse
		 * \brief Method writing channel response data to the files.
		 * \param[in] chout_healthy: channel responses for healthy data
		 * \param[in] chout_diseased: channel responses for diseased data
		 * \param[in] is_training: 1 if training phase; 0 otherwise 
		 * \return void
		 */
		void SaveChannelResponse(MAT *chout_healthy, MAT *chout_diseased, bool is_training=false);
		
		/*!
		 * \fn SaveROCPoints
		 * \brief Method writing values of the ROC curve points to the file: (FPF,TPF)
		 * \param[in] roc_tpf: true positive fraction (TPF) values
		 * \param[in] roc_fpf: false positive fraction (FPF) values
		 * \param[in] is_training: 1 if training phase; 0 otherwise 
		 * \return void
		 */
		void SaveROCPoints(VEC *roc_tpf, VEC *roc_fpf, bool is_training=false);
		
		/*!
		 * \fn SaveAUCSNR
		 * \brief Method writing values of AUC and SNR to the file.
		 * \param[in] AUC: AUC value
		 * \param[in] SNR: SNR value
		 * \param[in] is_training: 1 if training phase; 0 otherwise 
		 * \return void
		 */
		void SaveAUCSNR(double AUC, double SNR, bool is_training=false);
		
		/*!
		 * \fn SaveTestStatistics
		 * \brief Method writing values of the test statistics to the file.
		 * \param[in] cho_tS: test statistics for diseased data
		 * \param[in] cho_tN: test statistics for healthy data 
		 * \return void
		 */
		void SaveTestStatistics(VEC *cho_tS, VEC *cho_tN);
};

#endif // SINGLESLICESHOMODULE_H

