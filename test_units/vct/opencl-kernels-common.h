//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file opencl-kernels-common.h
 * \brief Implementation of Common definitions for OpenCL kernels
 * \date 29-April-2009
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

/*!
 *
 * Common definitions for OpenCL kernels (compiler portability work-arounds, different build modes, etc) can be added here.
 * Unrecognized pragmas cause a kernel compilation error on the AMD platform (AMD APP 2.4), so use ifdef.
 */

// OpenCLManager passes the right -D (_DEBUG or NDEBUG) arg to the compiler to make this work.
#if defined _DEBUG

// Enable printf() extension if available. (This may fail with some compilers, so then you'll just have to copy/paste the directives into your .cl.)
#ifdef cl_amd_printf
#pragma OPENCL EXTENSION cl_amd_printf : enable
#elif defined cl_nvidia_printf		// once NVIDIA adds it; printf() is in CUDA
#pragma OPENCL EXTENSION cl_nvidia_printf : enable
#elif defined cl_intel_printf
#pragma OPENCL EXTENSION cl_intel_printf : enable
#else								// can defuse all printf() stmts if no extension supports it
//#define printf	(void)
#endif


#endif // DEBUG mode

// AMD Stream 2.3 / APP 2.4 define M_PI_F, but not M_PI. (maybe you need to enable fp double)
// NVIDIA CUDA/OpenCL-1.0 the other way around. (maybe M_PI_F is only in OpenCL 1.1)
#ifndef M_PI_F
#  define M_PI_F	3.14159265358979323846f
#endif
