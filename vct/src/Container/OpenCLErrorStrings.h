//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file OpenCLErrorStrings.h
 * \brief Error strings corresponding to CL/cl.h error codes. cl.hpp supplies something similar but with function name strings.
 * 		  "Unknown errors" may become populated on newer OpenCL versions.
 * 		  Messages are straightforward from defined error names. Some are extended once found unclear (too specific).
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

const char* openclStrError(int clErrno);

