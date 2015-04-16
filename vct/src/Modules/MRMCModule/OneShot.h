//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file OneShot.h
 * \brief Implementation of methods for the one shot analysis (MRMC)
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

#ifndef OneShot_H
#define OneShot_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \fn usage
 * \brief Method displays a guideline to usage scenario
 * \return void
 */
void usage();

/*!
  * \fn one_shot
  * \brief Method calculates the variance for the given MRMC scores.
  *		   The variance of the AUC is calculated as:
  *		   alpha_1/N_0 + alpha_2/N_1 + alpha_3/N_0N_1 + alpha_4/Nr + alpha_5/NrN0 + alpha_6/NrN1 + alpha_7/NrN0N1
  *		   where 
  *		   N0 is the number of without-disease images
  *		   N1 is the number of with-disease images, and 
  *		   Nr is the number of readers
  *
  * \param  [in] toutFileName:         path to the file where the reader scores for with-disease images are saved
  * \param  [in] foutFileName:         path to the file where the reader scores for without-disease images are saved 
  * \param  [in] outputFileName:       path to the file where the results shall be stored
  * \param  [in] biasFlag:             indicates if the biased estimates of the alphas shall be produced
  * \param  [in] verboseFlag:          indicates if verbose output is also required
  * \return None 
  */	
void one_shot(char *toutFileName,char *foutFileName,char *outputFileName,int biasFlag,int verboseFlag);

#ifdef __cplusplus
}
#endif

#endif

