#include "../SingleSliceCHOModule.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <misc/misc.h>
#include "HotellingObserver/iq.h"

#undef HUGE_VAL

#pragma warning (disable: 4005) // macro redefinition

using namespace std;

const double AUC_THRESHOLD = 0.97;

const char * SingleSliceCHOModule::m_parameter_names[30] =
{
  "reader_id",
  "image_height",
  "image_width",
  "image_depth",
  "target_slice_id",
  "one_image_file_per_frame",
  "num_image_pairs_training",
  "num_image_pairs_testing",
  "channels_number",	
  "channels_type",
  "channels_lg_spread",
  "save_channelized_images",
  "auto_create_outputfolders",
  "dirpath_in_training_healthy",
  "dirpath_in_training_diseased",
  "dirpath_in_testing_healthy",
  "dirpath_in_testing_diseased",
  "dirpath_out_channelized_training_healthy",
  "dirpath_out_channelized_training_diseased",
  "dirpath_out_channelized_testing_healthy",
  "dirpath_out_channelized_testing_diseased",
  "filepath_out_training_tpf_fpf",
  "filepath_out_training_auc_snr",
  "filepath_out_testing_tpf_fpf",
  "filepath_out_testing_auc_snr",
  "filepath_out_reader_score_healthy_ref",
  "filepath_out_reader_score_healthy_new",
  "filepath_out_reader_score_diseased_ref",
  "filepath_out_reader_score_diseased_new",
  NULL
};

SingleSliceCHOModule::SingleSliceCHOModule()
{
  number_of_bits=0;
  input_number_of_frames=0;
  output_number_of_frames=0;
  input_component_number=0;
  output_component_number=0;
  frame_rate=0;
  white_point[0]=0;
  white_point[1]=0;
  white_point[2]=0;
  
  m_wCh = NULL;

  m_reader_id                              = 1;
  m_image_height                           = 0;
  m_image_width                            = 0;
  m_image_depth                            = 0;
  m_target_slice_id                        = 1;
  m_one_image_file_per_frame               = false;
  m_num_image_pairs_training               = 0;
  m_num_image_pairs_testing                = 0;
  m_channels_number                        = 0;
  m_channels_type                          = 0;
  m_channels_lg_spread                     = 0;
  m_save_channelized_images                = false;
  m_auto_create_outputfolders              = false;
  m_path_in_training_healthy               = std::wstring();
  m_path_in_training_diseased              = std::wstring();
  m_path_in_testing_healthy                = std::wstring();
  m_path_in_testing_diseased               = std::wstring();
  m_path_out_channelized_training_healthy  = std::wstring();
  m_path_out_channelized_training_diseased = std::wstring();
  m_path_out_channelized_testing_healthy   = std::wstring();
  m_path_out_channelized_testing_diseased  = std::wstring();
  m_path_out_training_tpf_fpf              = std::wstring();
  m_path_out_training_auc_snr              = std::wstring();
  m_path_out_testing_tpf_fpf               = std::wstring();
  m_path_out_testing_auc_snr               = std::wstring();
  m_path_out_reader_score_healthy_ref      = std::wstring();
  m_path_out_reader_score_healthy_new      = std::wstring();
  m_path_out_reader_score_diseased_ref     = std::wstring();
  m_path_out_reader_score_diseased_new     = std::wstring();
}	

int SingleSliceCHOModule::FindParameter( const string & name )
{
	for (int i = 0; m_parameter_names[i] != NULL; i ++)
		if (m_parameter_names[i] == name)
			return i;
	m_pMevicLogger->logError("SingleSliceCHOModule: unrecognized parameter name: " + name + "!");
	return -1;
}

void SingleSliceCHOModule::SetParameter( const string & parameter_name, const string & parameter_value )
{
  switch( FindParameter( parameter_name ) )
  {
  case  0: m_reader_id = atoi( parameter_value.c_str() ); break;
  case  1: m_image_height = atoi( parameter_value.c_str() ); break;
  case  2: m_image_width = atoi( parameter_value.c_str() ); break;
  case  3: m_image_depth = atoi( parameter_value.c_str() ); break;
  case  4: m_target_slice_id = atoi( parameter_value.c_str() ); break;
  case  5: m_one_image_file_per_frame = strcmp( parameter_value.c_str(), "true") == 0 || atoi(parameter_value.c_str()) == 1; // either "true" or "1"
  case  6: m_num_image_pairs_training = atoi( parameter_value.c_str() ); break;
  case  7: m_num_image_pairs_testing = atoi( parameter_value.c_str() ); break;
  case  8: m_channels_number = atoi( parameter_value.c_str() ); break;
  //case  8: m_channels_type = misc::string_to_tstring( parameter_value.c_str() ); break ;
  case  9: { 
           string sName = boost::to_lower_copy(string(parameter_value.c_str()));
           if (!strcmp(sName.c_str(), "laguerre-gauss")||
             !strcmp(sName.c_str(), "lg"))
           m_channels_type = 1;
           else if (!strcmp(sName.c_str(), "dense-dog")||
             !strcmp(sName.c_str(), "d-dog")||
             !strcmp(sName.c_str(), "ddog"))
           m_channels_type = 2;
           else if (!strcmp(sName.c_str(), "sparse-dog")||
             !strcmp(sName.c_str(), "s-dog")||
             !strcmp(sName.c_str(), "sdog"))
           m_channels_type = 3;
           } break ;
  case 10: m_channels_lg_spread = float(atof( parameter_value.c_str()) ); break;
  case 11: m_save_channelized_images = strcmp( parameter_value.c_str(), "true") == 0 || atoi(parameter_value.c_str()) == 1; // either "true" or "1"
  case 12: m_auto_create_outputfolders = strcmp( parameter_value.c_str(), "true") == 0 || atoi(parameter_value.c_str()) == 1; // either "true" or "1"
  case 13: m_path_in_training_healthy = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 14: m_path_in_training_diseased = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 15: m_path_in_testing_healthy = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 16: m_path_in_testing_diseased = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 17: m_path_out_channelized_training_healthy = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 18: m_path_out_channelized_training_diseased = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 19: m_path_out_channelized_testing_healthy = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 20: m_path_out_channelized_testing_diseased = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str(), true) ); break ;
  case 21: m_path_out_training_tpf_fpf = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 22: m_path_out_training_auc_snr = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 23: m_path_out_testing_tpf_fpf = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 24: m_path_out_testing_auc_snr = misc::string_to_tstring( parameter_value.c_str() ); break ;
  case 25: m_path_out_reader_score_healthy_ref = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 26: m_path_out_reader_score_healthy_new = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 27: m_path_out_reader_score_diseased_ref = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
  case 28: m_path_out_reader_score_diseased_new = misc::string_to_tstring( misc::BackSlash2ForwardSlash(parameter_value.c_str()) ); break ;
 
  default: m_pMevicLogger->logError(std::string("SingleSliceCHOModule::SetParameter: parameter name ").append(parameter_name.c_str()).append(" not recognized"));
    break;
  }
}

void SingleSliceCHOModule::Simulate(vector<DSContainer*>& list)
{
  VEC *output_roc_tpf = v_get(0);
  VEC *output_roc_fpf = v_get(0);
  string temp ;

  if (!ValidateModuleParameters()) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::Simulate: Input parameters non valid."));
  }

  // compute the channel matrix
  m_channels = ChannelFilter(
    m_image_width, m_image_height, 1, 
    m_channels_type, m_channels_number, m_channels_lg_spread, 0);
  m_channels.create_channel_matrix( );

  // training phase
  // goal: estimate the template of the observer (reader)
  RunTraining();

  // testing phase
  // goal: estimate the detection performance of the observer (reader)
  RunTesting(output_roc_tpf,output_roc_fpf);

  // create new component to save the ROC curve of SingleSliceCHOModule
  DSContainer* container = list.back();

  if (container==NULL) {
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::Simulate: Pointer to the current component not valid!"));
  }

  // read the number of the last component
  input_component_number = container->GetLastComponentNumber();
  output_component_number = input_component_number+1;

   container->CreateComponent( 1, m_num_image_pairs_testing, 
      TYPE_IMAGE_ROC);

  for (unsigned i=0; i<m_num_image_pairs_testing; i ++)	{
    float data[2] = { float(output_roc_fpf->ve[i]), float(output_roc_tpf->ve[i]) };
    container->SetComponentElement(container->GetLastComponentNumber(), i, 0, data);		
  }

  m_pMevicLogger->logProgress(std::string(
    "SingleSliceCHOModule::Simulate: Output component created, TYPE_IMAGE_ROC."));

  // clean up
  v_free(output_roc_tpf); v_free(output_roc_fpf);

  m_pMevicLogger->logProgress(std::string(
    "SingleSliceCHOModule::Simulate: Exiting CHO module..."));

}

SingleSliceCHOModule::~SingleSliceCHOModule()
{	
  if (m_wCh != NULL) {
    v_free(m_wCh);
    m_wCh = NULL;
  }
}

bool SingleSliceCHOModule::ValidateModuleParameters() {
	
  bool status = true;

  if ( m_image_depth<1 || m_image_width<1 || m_image_height<1) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ValidateModuleParameters: Please verify that the image dimensions are correctly specified."));
    status = false;
  }

  // this is a single-slice CHO so the image must have exactly one slice or
  // the position of the slice to which the CHO is applied shall be specified
  if ( m_image_depth>1 && m_target_slice_id<1) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ValidateModuleParameters: The image contains multiple slices, target slice position must be specified."));
    status = false;
  }
  
  // for one slice images, the value of m_target_slice_id will be ignored
  if (m_image_depth==1 && m_target_slice_id>1) {
    m_target_slice_id = 1;
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::ValidateModuleParameters: The value of m_target_slice_id will be ignored, the image contains one slice only."));
  }

  // current implementation reuires image width to be equal to image height
  if ( m_image_width != m_image_height ) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ValidateModuleParameters: Image width must be equal to image height."));
    status = false;
  }

  // check if the channel type is valide
  if ( m_channels_type == 0 ) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ValidateModuleParameters: Channel type not recognized."));
    status = false;
  }

  // for LG channels, the spread of channels must be specified
  if ( m_channels_type==1 && m_channels_lg_spread==0 ) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::Simulate: For LG channels, rarameter m_channels_lg_spread must be specified."));
    status = false;
  }

  // for DDOG and SDOG channels, only the number of channels can be selected
  // all other parameters are predefined [Abbey and Barrett, 2001]
  if ( (m_channels_type==2 || m_channels_type==3) && m_channels_lg_spread!=0 ) {
    m_channels_lg_spread = 0;
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::Simulate: Selected channel type is DOG. Parameter m_channels_lg_spread will be ignored."));
  }

  // Paths to the input image data need to be specified
	if( m_path_in_training_healthy.empty() ||
      m_path_in_training_diseased.empty() ||
      m_path_in_testing_healthy.empty() ||
      m_path_in_testing_diseased.empty() ) {
		m_pMevicLogger->logError(
    "SingleSliceCHOModule::ValidateModuleParameters: you did not provide all image path_in parameters.");
    status = false;
  }

  if (m_save_channelized_images && (
      m_path_out_channelized_training_healthy.empty() || 
      m_path_out_channelized_training_diseased.empty() || 
      m_path_out_channelized_testing_healthy.empty() || 
      m_path_out_channelized_testing_diseased.empty() )) {
    m_pMevicLogger->logError(std::string(
    "SingleSliceCHOModule::ValidateModuleParameters: At least one of path_out_channelized_xxx not specified."));
    status = false;
  }

  if (m_path_out_training_tpf_fpf.empty() || 
      m_path_out_training_auc_snr.empty()) {
    m_pMevicLogger->logError(std::string(
    "SingleSliceCHOModule::ValidateModuleParameters: At least one of path_out_training_xxx not specified."));
    status = false;
  }

  if (m_path_out_testing_tpf_fpf.empty() || 
      m_path_out_testing_auc_snr.empty()) {
    m_pMevicLogger->logError(std::string(
    "SingleSliceCHOModule::ValidateModuleParameters: At least one of path_out_testing_xxx not specified."));
    status = false;
  }

  if (m_path_out_reader_score_healthy_new.empty() || 
      m_path_out_reader_score_diseased_new.empty()) {
    m_pMevicLogger->logMessage(std::string(
    "SingleSliceCHOModule::ValidateModuleParameters: At least one of path_out_reader_score_xxx_new not specified; test statistics will not be saved."));
  }
  else if (m_path_out_reader_score_healthy_ref.empty() || 
      m_path_out_reader_score_diseased_ref.empty()) {
    m_pMevicLogger->logMessage(std::string(
    "SingleSliceCHOModule::ValidateModuleParameters: At least one of path_out_reader_score_xxx_ref not specified; reference input for test statistics will be ignored."));
  }

  return status;

}

void SingleSliceCHOModule::RunTraining(){
  
  // reset the template of the observer
  if (m_wCh != NULL){
    v_free(m_wCh);
    m_wCh = NULL;
  }

  // Process
  
  // allocate matrices for storing the channel responses
  MAT *vS = m_get(m_channels_number,m_num_image_pairs_training);
  MAT *vN = m_get(m_channels_number,m_num_image_pairs_training);	
  VEC *sbar = v_get(m_channels_number);

  // apply the channels to compute channel outputs: vS, vN
  ComputeChannelOutputs(vS, vN, sbar, true);

  // store channelized image data
  if (m_save_channelized_images)
    SaveChannelResponse(vS, vN, true);

  // compute the mean of the channelized data: sbar
  sv_mlt(1.0/m_num_image_pairs_training, sbar, sbar); 

  // compute the channel template: m_wCh
  chotelling_chtemplate_from_outputs(vS, vN, sbar, &m_wCh);

  // [OPTIONAL] use the template to compute AUC and SNR for training phase */
  VEC *tS, *tN;
  VEC *training_tpf, *training_fpf;
  double training_auc;
  double training_snr;

  // for each image, compute the CHO test statistics
  /* tS - test statistic of a signal-present (diseased) image
     tN - test statistic of a signal-absent (healthy) image */
  chotelling(vS, vN, m_wCh, &tS, &tN);

  // ROC Curve generation
  /* The tS, and tN are decision variable outputs which we can
  perform ROC analysis on. 
  tpf = miss probability
  fpf = false-alarm probability */
  WilcoxonAUC(tS,tN,&training_auc,&training_tpf,&training_fpf);

  // store ROC points in the file
  SaveROCPoints(training_tpf, training_fpf, true);

  if (training_auc<AUC_THRESHOLD) {
    // use AUC to calculate SNR
    training_snr = snr_auc(training_auc);
  }
  else {
    // use test statistics to calculate SNR
    training_snr = snr_t(tS, tN);
  }

  // store AUC and SNR scores in the file
  SaveAUCSNR(training_auc, training_snr, true);

  // Free
  v_free(sbar);
  v_free(tS);
  v_free(tN);

  m_pMevicLogger->logProgress(std::string(
    "SingleSliceCHOModule::RunTraining: Training completed."));

}

void SingleSliceCHOModule::RunTesting(VEC *testing_tpf, VEC *testing_fpf){
  
  // check if the template of the observer exists
  if (m_wCh == NULL){
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::RunTesting: The CHO template does not exist! Abort test reading."));
    return;
  }

  // Process
  
  // allocate matrices for storing the channel responses
  MAT *vS = m_get(m_channels_number,m_num_image_pairs_testing);
  MAT *vN = m_get(m_channels_number,m_num_image_pairs_testing);	
  VEC *sbar = v_get(m_channels_number);

  // apply the channels to compute channel outputs: vS, vN
  ComputeChannelOutputs(vS, vN, sbar, false);

  // store channelized image data
  if (m_save_channelized_images)
    SaveChannelResponse(vS, vN);

  // use the template to compute AUC and SNR for testing phase */
  VEC *tS, *tN;
  double testing_auc;
  double testing_snr;

  // for each image, compute the CHO test statistics
  /* tS - test statistic of a signal-present (diseased) image
     tN - test statistic of a signal-absent (healthy) image */
  chotelling(vS, vN, m_wCh, &tS, &tN);

  // store test statistics for one-shot analysis
  // see MRMCModule
  SaveTestStatistics(tS, tN);

  // ROC Curve generation
  /* The tS, and tN are decision variable outputs which we can
  perform ROC analysis on. 
  tpf = miss probability
  fpf = false-alarm probability */
  WilcoxonAUC(tS,tN,&testing_auc,&testing_tpf,&testing_fpf);

  // store ROC points in the file
  SaveROCPoints(testing_tpf, testing_fpf);

  if (testing_auc<AUC_THRESHOLD) {
    // use AUC to calculate SNR
    testing_snr = snr_auc(testing_auc);
  }
  else {
    // use test statistics to calculate SNR
    testing_snr = snr_t(tS, tN);
  }

  // store AUC and SNR scores in the file
  SaveAUCSNR(testing_auc, testing_snr);

  // Free
  v_free(sbar);
  v_free(tS);
  v_free(tN);

  m_pMevicLogger->logProgress(std::string(
    "SingleSliceCHOModule::RunTesting: Observer test reading completed."));
}

void SingleSliceCHOModule::ComputeChannelOutputs(MAT *vS, MAT *vN, VEC *sbar, bool is_training)
{
  unsigned long num_images;
  unsigned long w = -1, h = -1;
  unsigned int d=-1;
  bool parameter_error = false;
  std::string path_healthy, path_diseased;
  std::string temp, log_name;
  MAT *imgmat_healthy, *imgmat_diseased;
  MAT *U;
  misc m;

  if ( is_training ) {
    num_images = m_num_image_pairs_training;
    path_healthy = misc::tstring_to_string(m_path_in_training_healthy);
    path_diseased = misc::tstring_to_string(m_path_in_training_diseased);
    log_name = std::string("training");
  } 
  else {
    num_images = m_num_image_pairs_testing;
    path_healthy = misc::tstring_to_string(m_path_in_testing_healthy);
    path_diseased = misc::tstring_to_string(m_path_in_testing_diseased);
    log_name = std::string("testing");
  }

  if (m_one_image_file_per_frame) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ComputeChannelOutputs: m_one_image_file_per_frame=true not supported in current version of VCT. Reserved for future development."));
    return;
    
 
  } // ...if m_one_image_file_per_frame = true
  else {
    // retrieve list of full paths to the image files
    vector <string> filelist_healthy, filelist_diseased;
    filelist_healthy = m.getFilePathList(path_healthy, std::string(".*\\.bin"), m_pMevicLogger);
    filelist_diseased = m.getFilePathList(path_diseased, std::string(".*\\.bin"), m_pMevicLogger);

    if(filelist_healthy.empty()) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::ComputeChannelOutputs: No image files found at ").append(path_healthy.c_str()));
      return;
    }
    if (filelist_healthy.size() < num_images) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::ComputeChannelOutputs: Number of healthy ").append(log_name).append(" images is less than required."));
      parameter_error = true;
    }
    if(filelist_diseased.empty()) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::ComputeChannelOutputs: No image files found at ").append(path_diseased.c_str()));
      return;
    }
    if (filelist_diseased.size() < num_images) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::ComputeChannelOutputs: Number of diseased ").append(log_name).append(" images is less than required."));
      parameter_error = true;
    }

    // create a dummy container for loading the images
    DSContainer *container = new DSContainer();

    // load the first images in the lists
    temp.clear();
    temp = filelist_healthy.at(0);
    container->LoadFromFileWithCompression((char*) temp.c_str());
    container->GetComponentNumberOfColumnElementsPerFrame(0, &w);
    container->GetComponentNumberOfRowElementsPerFrame(0, &h);
    container->GetComponentNumberOfFrames(0, &d);

    // and verify the required parameters for these images
    // note: we assume that all other images follow the same settings, and
    // to save the operations, we skip detailed check-ups for the other images
    if (d != m_image_depth) {
      m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ComputeChannelOutputs: Number of ").append(log_name).append(" image slices does not match the value of image_depth."));
      parameter_error = true;
    }
    if (w != m_image_width) {
      m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::ComputeChannelOutputs: Width of ").append(log_name).append(" image does not match the value of image_width."));
      parameter_error = true;
    }
    if (h != m_image_height) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::ComputeChannelOutputs: Height of ").append(log_name).append(" image does not match the value of image_height."));
      parameter_error = true;
    }

    if (parameter_error) {
      delete container;
      return;
    }

    // initialisation of the matrices holding image data
    imgmat_healthy = m_get (m_image_width*m_image_height,1);
    imgmat_diseased = m_get (m_image_width*m_image_height,1);

    // retriving the channel matrix
    U = m_channels.get_channel_matrix();

    for (unsigned img=0; img<num_images; img++) {
      temp.clear();
      temp = filelist_healthy.at(img);
      container->LoadFromFileWithCompression((char*)temp.c_str());
      m.frameToMAT(imgmat_healthy, container, container->GetLastComponentNumber(), m_target_slice_id-1);

      temp.clear();
      temp = filelist_diseased.at(img);
      container->LoadFromFileWithCompression((char*)temp.c_str());
      m.frameToMAT(imgmat_diseased, container, container->GetLastComponentNumber(), m_target_slice_id-1);
    
      // compute the channel response for each image individually (this is more efficient
      // than loading all images first into memory)
      chotelling_chresponse(imgmat_diseased, imgmat_healthy, U, vS, vN, sbar, img);
    }

    // clean up
    m_free(imgmat_healthy);
    m_free(imgmat_diseased);
    delete container;
  } // ...if m_one_image_file_per_frame = false
}


// *** SaveChannelResponse ***

void SingleSliceCHOModule::SaveChannelResponse(MAT *chout_healthy, MAT *chout_diseased, bool is_training)
{
  std::string path_healthy, path_diseased;
  unsigned img_id, ch_id;

  if ( is_training ) {
    path_healthy = misc::tstring_to_string(m_path_out_channelized_training_healthy);
    path_diseased = misc::tstring_to_string(m_path_out_channelized_training_diseased);
  } 
  else {
    path_healthy = misc::tstring_to_string(m_path_out_channelized_testing_healthy);
    path_diseased = misc::tstring_to_string(m_path_out_channelized_testing_diseased);
  }

  if (!misc::DirectoryExists(path_healthy) ||
      !misc::DirectoryExists(path_diseased)) {
    if (!m_auto_create_outputfolders) {
      m_pMevicLogger->logMessage(std::string(
      "SingleSliceCHOModule::SaveChannelResponse: m_auto_create_outputfolders=FALSE, no output files will be generated."));
      return;
    }
  }
  if (!misc::DirectoryExists(path_healthy) && 
    !misc::MakeDirectory(path_healthy)) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveChannelResponse: Output folder ").append(path_healthy).append(" could not be created. Creating channel response files aborted."));
    return;
  }
  if (!misc::DirectoryExists(path_diseased) && 
    !misc::MakeDirectory(path_diseased)) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveChannelResponse: Output folder ").append(path_diseased).append(" could not be created. Creating channel response files aborted."));
    return;
  }


  // storing channel response for healthy data
 // FILE * chout_healthy_values = fopen(path_healthy.c_str(), "wt");
  FILE * chout_healthy_values;
  fopen_s(&chout_healthy_values, path_healthy.c_str(), "wt");
  if (chout_healthy_values==NULL) {
     m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveChannelResponse: Output file could not be opened. Creating channel response files aborted."));
     return;
  }
  for (img_id=0; img_id<chout_healthy->n; img_id++) { // image pairs
    for (ch_id=0; ch_id<chout_healthy->m; ch_id++) { // number of channels
      fprintf(chout_healthy_values, "%f ", chout_healthy->me[ch_id][img_id]);
    }
    fprintf(chout_healthy_values, "\n");
	}
  fclose(chout_healthy_values);

  // storing channel response for diseased data
  //FILE * chout_diseased_values = fopen(path_diseased.c_str(), "wt");  
  FILE * chout_diseased_values;
  fopen_s(&chout_diseased_values, path_diseased.c_str(), "wt");
  if (chout_diseased_values==NULL) {
     m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveChannelResponse: Output file could not be opened. Creating channel response files aborted."));
     return;
  }
  for (img_id=0; img_id<chout_diseased->n; img_id++) { // image pairs
    for (ch_id=0; ch_id<chout_diseased->m; ch_id++) { // number of channels
      fprintf(chout_diseased_values, "%f ", chout_diseased->me[ch_id][img_id]);
    }
    fprintf(chout_diseased_values, "\n");
	}
  fclose(chout_diseased_values);

}

void SingleSliceCHOModule::SaveROCPoints(VEC *roc_tpf, VEC *roc_fpf, bool is_training)
{
  std::string path_out;

  if ( is_training ) {
    path_out = misc::tstring_to_string(m_path_out_training_tpf_fpf);
  } 
  else {
    path_out = misc::tstring_to_string(m_path_out_testing_tpf_fpf);
  }

  if (!misc::DirectoryExists(path_out)) {
    if (!m_auto_create_outputfolders) {
      m_pMevicLogger->logMessage(std::string(
      "SingleSliceCHOModule::SaveROCPoints: m_auto_create_outputfolders=FALSE, no output files will be generated."));
      return;
    }
    if (!misc::MakeDirectory(path_out)) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveROCPoints: Output folder ").append(path_out).append(" could not be created! Creating ROC output files aborted."));
      return;
    }
  }

  // store points of the ROC curve in the file
  /* file format:
     FPF (false-positive-fraction) | TPF (true-positive-fraction) */
  //FILE * tpf_fpf_values = fopen(path_out.c_str(), "wt");
  FILE * tpf_fpf_values;
  fopen_s(&tpf_fpf_values, path_out.c_str(), "wt");
  if (tpf_fpf_values==NULL) {
     m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveROCPoints: Output file could not be opened. Creating ROC output files aborted."));
     return;
  }
  for (unsigned i=0; i<roc_tpf->dim; i ++)	{
    float data2[2] = { float(roc_fpf->ve[i]), float(roc_tpf->ve[i]) };
    fprintf(tpf_fpf_values, "%f %f\n", data2[0], data2[1]);
  }
  fclose(tpf_fpf_values);

}

void SingleSliceCHOModule::SaveAUCSNR(double AUC, double SNR, bool is_training)
{
  std::string path_out;

  if ( is_training ) {
    path_out = misc::tstring_to_string(m_path_out_training_auc_snr);
  } 
  else {
    path_out = misc::tstring_to_string(m_path_out_testing_auc_snr);
  }

  if (!misc::DirectoryExists(path_out)) {
    if (!m_auto_create_outputfolders) {
      m_pMevicLogger->logMessage(std::string(
      "SingleSliceCHOModule::SaveAUCSNR: m_auto_create_outputfolders=FALSE, no output files will be generated."));
      return;
    }
    if (!misc::MakeDirectory(path_out)) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveAUCSNR: Output folder ").append(path_out).append(" could not be created! Creating AUC-SNR output files aborted."));
      return;
    }
  }

  // store AUC and SNR scores in the file
  /* file format:
     Redaer_ID | #Trainer pairs | AUC | SNR */
  //FILE * auc_snr_values = fopen(path_out.c_str(), "wt");  
  FILE * auc_snr_values;
  fopen_s(&auc_snr_values, path_out.c_str(), "wt");
  if (auc_snr_values==NULL) {
     m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveAUCSNR: Output file could not be opened. Creating AUC-SNR output files aborted."));
     return;
  }
  fprintf(auc_snr_values, "%02lu %03lu %f %f\n", m_reader_id, m_num_image_pairs_training, AUC, SNR);
  fclose(auc_snr_values);

}

void SingleSliceCHOModule::SaveTestStatistics(VEC *cho_tS, VEC *cho_tN)
{
  // if either of the output paths
  // m_path_out_reader_score_healthy_new or m_path_out_reader_score_diseased_new
  // is empty, skip writing test statistics to files
  if (m_path_out_reader_score_healthy_new.empty() || 
      m_path_out_reader_score_diseased_new.empty()) {
    m_pMevicLogger->logMessage(std::string(
    "SingleSliceCHOModule::SaveTestStatistics: path_out_xxx_new not specified; skip saving test statistics."));
    return;
  }

  if (!misc::DirectoryExists(misc::tstring_to_string(m_path_out_reader_score_healthy_new)) ||
      !misc::DirectoryExists(misc::tstring_to_string(m_path_out_reader_score_diseased_new))) {
    if (!m_auto_create_outputfolders) {
      m_pMevicLogger->logMessage(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: m_auto_create_outputfolders=FALSE, no output files will be generated."));
      return;
    }
  }
  if (!misc::DirectoryExists(misc::tstring_to_string(m_path_out_reader_score_healthy_new)) && 
    !misc::MakeDirectory(misc::tstring_to_string(m_path_out_reader_score_healthy_new).c_str())) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Output folder ").append(misc::tstring_to_string(m_path_out_reader_score_healthy_new)).append(" could not be created! The oneshot output files will not be generated."));
    return;
  }
  if (!misc::DirectoryExists(misc::tstring_to_string(m_path_out_reader_score_diseased_new)) && 
    !misc::MakeDirectory(misc::tstring_to_string(m_path_out_reader_score_diseased_new).c_str())) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Output folder ").append(misc::tstring_to_string(m_path_out_reader_score_diseased_new)).append(" could not be created! The oneshot output files will not be generated."));
    return;
  }

  /* test dataset: diseased */

  int number_of_images = 0;
  int number_of_readers = 0;
  unsigned i;
  std::string str;
  std::ofstream out;
  std::ifstream in;
  
  out.open( misc::tstring_to_string(m_path_out_reader_score_diseased_new).c_str(), ios::trunc );
  if( !out.good() ) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Cannot open file: ").append(misc::tstring_to_string(m_path_out_reader_score_diseased_new)).append(". Creating oneshot output files aborted."));
    return;
  }

  // if m_path_out_reader_score_diseased_ref file exists, 
  // refer to it when creating the m_path_out_reader_score_diseased_new file
  if (m_path_out_reader_score_diseased_ref.empty()) {
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Input file not specified; it will be ignored."));
  }
  else if (!boost::filesystem::exists(m_path_out_reader_score_diseased_ref.c_str())) {
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Input file: ").append(misc::tstring_to_string(m_path_out_reader_score_healthy_ref)).append(" could not be found; it will be ignored."));

    // write the 1st line of the output file
    // note: The 1st line of the output file must have the numbers of images and readers.
    out << cho_tS->dim << ' ' << ++number_of_readers << "\n";

    // write the reader scores
    for (i = 0; i < cho_tS->dim; i++) {
      out << cho_tS->ve [i] << "\n";
    }
  }
  else {
    in.open( misc::tstring_to_string(m_path_out_reader_score_diseased_ref).c_str() );
    if( !in.good() ) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveTestStatistics: Cannot open file: ").append(misc::tstring_to_string(m_path_out_reader_score_diseased_ref)).append(". Creating oneshot output files aborted."));
      return;
    }

    in.seekg(0);

    // read the 1st line of the input file
    // note: The 1st line of the input file must have the numbers of images and readers.
    in >> number_of_images;
    in >> number_of_readers;

    // the number of images must be the same between the two diseased datasets
    if ( number_of_images != cho_tS->dim ) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveTestStatistics: Number of images must be the same between the diseased datasets. Creating oneshot output files aborted."));
      return;
    }

    // write the 1st line of the output file
    // note: The 1st line of the output file must have the numbers of images and readers.
    out << cho_tS->dim << ' ' << ++number_of_readers << "\n";

    getline( in, str ); // reach end of the 1st line

    // write the reader scores (preserve existing scores from the input file, add a new column to store current results)
    for (i = 0; i < cho_tS->dim; i++) {
      getline( in, str );
      out << str << ' ' << cho_tS->ve [i] << "\n";
    }

    in.close(); in.clear();
  }

  out.close(); out.clear();


  /* test dataset: healthy */

  number_of_images = 0;
  number_of_readers = 0;

  out.open( misc::tstring_to_string(m_path_out_reader_score_healthy_new).c_str(), ios::trunc );
  if( !out.good() ) {
    m_pMevicLogger->logError(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Cannot open file: ").append(misc::tstring_to_string(m_path_out_reader_score_healthy_new)).append(". Creating oneshot output files aborted."));
    return;
  }

  // if m_path_out_reader_score_healthy_ref file exists, 
  // refer to it when creating the m_path_out_reader_score_healthy_ref file
  if (m_path_out_reader_score_healthy_ref.empty()) {
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Input file not specified; it will be ignored."));
  }
  else if (!boost::filesystem::exists(m_path_out_reader_score_healthy_ref.c_str())) {
    m_pMevicLogger->logWarning(std::string(
      "SingleSliceCHOModule::SaveTestStatistics: Input file: ").append(misc::tstring_to_string(m_path_out_reader_score_healthy_ref)).append(" could not be found; it will be ignored."));

    // write the 1st line of the output file
    // note: The 1st line of the output file must have the numbers of images and readers.
    out << cho_tN->dim << ' ' << ++number_of_readers << "\n";

    // write the reader scores
    for (i=0; i<cho_tN->dim; i++) {
      out << cho_tN->ve [i] << "\n";
    }
  }
  else {
    in.open( misc::tstring_to_string(m_path_out_reader_score_healthy_ref).c_str() );
    if( !in.good() ) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveTestStatistics: Cannot open file: ").append(misc::tstring_to_string(m_path_out_reader_score_healthy_ref)).append(". Creating oneshot output files aborted."));
      return;
    }

    in.seekg(0);

    // read the 1st line of the input file
    // note: The 1st line of the input file must have the numbers of images and readers.
    in >> number_of_images;
    in >> number_of_readers;

    // the number of images must be the same between the two healthy datasets
    if ( number_of_images != cho_tN->dim ) {
      m_pMevicLogger->logError(std::string(
        "SingleSliceCHOModule::SaveTestStatistics: Number of images must be the same between the healthy datasets. Creating oneshot output files aborted."));
      //printf("       Earlier experiment: N0= %d ; Current experiment: N0= %d\n", number_of_images, tN->dim);
      return;
    }

    // write the 1st line of the output file
    // note: The 1st line of the output file must have the numbers of images and readers.
    out << cho_tN->dim << ' ' << ++number_of_readers << "\n";

    getline( in, str ); // reach end of the 1st line

    // write the reader scores (preserve existing scores from the input file, add a new column to store current results)
    for (i=0; i<cho_tN->dim; i++) {
      getline( in, str );
      out << str << ' ' << cho_tN->ve [i] << "\n";
    }

    in.close();
  }

  out.close(); out.clear();
}