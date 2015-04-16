//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file iq.h
 * \brief Implementation of functions necessary for the CHO, msCHO, volumetric CHO
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
 
#ifndef IQ_H
#define IQ_H

/* macros */
#ifndef PI
#define PI				3.1415926535898
#endif

#ifdef __cplusplus
extern "C" {
#endif

VEC * symmeig2(MAT* A, MAT* eigvec, VEC* eigval);

/* compute the decision variables (testing step) */
void chotelling(MAT * vS, MAT *vN, VEC * wCh, VEC ** tS, VEC ** tN);

/* compute the channel responses for a given slice */
void chotelling_chresponse(MAT * IS, MAT *IN, MAT * Temps, MAT * vS, MAT * vN, VEC * sbar, int framenumber);

/* compute the channel template based on the channel outputs */
void chotelling_chtemplate_from_outputs(MAT *vS, MAT *vN, VEC * sbar, VEC ** wCh);

/* compute the channel template (training step) */
void chotelling_chtemplate(MAT * IS, MAT *IN, MAT * Temps, VEC ** wCh);

/* 
 * \fn WilcoxonAUC
 * \brief Computes the AUC, and the ROC operating points (by simple thresholding) using the Sout and Nout decision variable data.
 * \param[in] Sout: with-signal decision variable data (1xN matrix)
 * \param[in] Nout: without-signal decision variable data (1xM matrix)
 * \param[out] AUC: Wilcoxon area under the ROC curve
 * \param[out] tpf: True-positive fraction values
 * \param[out] fpf: False-positive fraction values
 * \note this routine assumes that the signal decision
		 variable data is (generally) larger than the noise, ie, large decision
		 variable outputs correspond to high confidence of signal present.
 * \return void
*/
void WilcoxonAUC(VEC *Sout, VEC *Nout, double *AUC, VEC **ptpf, VEC **pfpf);

/*! 
 * \fn snr_t
 * \brief Implementation of the signal-to-noise ratio method. 
 *		  function [dp] = dprime(Sout,Nout)
 *		  dp = dprime(Sout,Nous)
 *		  Compute the d' (SNR) for the decision variable data given by Sout,	
 *		  and Nout -- the signal and noise outputs, respectively
 *		  dp = sqrt( ((mean(Sout) - mean(Nout)).^2) / (.5*var(Sout) + .5*var(Nout)));
 */
double snr_t(VEC *Sout, VEC *Nout);

/*! 
 * \fn snr_auc
 * \brief Implementation of the signal-to-noise ratio method. 
 *		  function [dp] = dprime(Sout,Nout)
 *		  dp = dprime(Sout,Nous)
 *		  Compute the d' (SNR) for the decision variable data given by Sout,	
 *		  and Nout -- the signal and noise outputs, respectively
 *		  dp = sqrt( ((mean(Sout) - mean(Nout)).^2) / (.5*var(Sout) + .5*var(Nout)));
 */
double snr_auc(double auc);

#ifdef __cplusplus
}
#endif


#endif
