//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file channel_laguerre.h
 * \brief Implementation of function for 1d, 2d, 3d Laguerre Guass channels
 * \date 1-May-2008
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
 
// Change log
// Ali on May 2, 2014: allow different spread in time (i.e., depth)


#ifndef LAGUERREGAUSS_H
#define LAGUERREGAUSS_H

#include <Meschach/matrix.h>

#ifndef	MATRIXH
struct MAT;
struct VEC;
#endif

/* macros */
#ifndef PI
#define PI				3.1415926535898
#endif

/*!
 * \fn channel_laguerre1d
 * \brief Compute the 1-D Laguerre-Gauss function of order j at values defined
 * \param j: order of LG functions
 * \return MAT*: pointer to the data
 */
#if defined __cplusplus
  extern "C" {
#endif
MAT * Laguerre1D(int j, MAT * x);
#if defined __cplusplus
  }
#endif

/*!
 * \fn channel_laguerre2d
 * \brief Generate a Laguerre-Gauss function of order j, with width parameter
 *		  Pass the optional r argument if you want to use your own coordinate system.
 *		  Laguerre-Gauss functions are used to approximate the  Hotelling observer.
 * \param dim_m: height
 * \param dim_n: width
 * \param j: order of LG functions
 * \param a: width parameter of LG functions
 * \param r: (optional) matrix defining coordinate system. This must match the size specified by dim
 * \return MAT*: pointer to the data of the channel
 */
#if defined __cplusplus
  extern "C" {
#endif
MAT * Laguerre2D(int dim_m, int dim_n, int j, double a, MAT * r);
#if defined __cplusplus
  }
#endif

/*!
 * \fn channel_laguerre3d
 * \brief allows different spread in time (i.e., depth)
 * \param dim_y: height
 * \param dim_x: width
 * \param dim_z: depth
 * \param j: order of LG functions
 * \param a: width parameter of LG functions
 * \param aT: depth of LG functions
 * \param r: (optional) matrix defining coordinate system. This must match the size specified by dim
 * \param float*: pointer to the data of the 3D channel
 * \return void
 */
// Ali on May 2, 2014: allow different spread in time (i.e., depth)
void Laguerre3D(int dim_y, int dim_x, int dim_z, int j, float a, float aT, float * r, float *output);
// Ali end

#endif // LAGUERREGAUSS_H

