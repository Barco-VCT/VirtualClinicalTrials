//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file OpenCLHostTypes.h
 * \brief Implementation of the class int2, int3, int4, uint2, float2, float3, float4
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
 * Make some of the types that OpenCL compilers have built-in for kernels available for host code.
 * This makes passing parameters of these types to a kernel look more correct.
 * OpenCL also defines cl_intN, cl_uint2, cl_floatN etc types for host code, but these are declared as vector registers,
 * so you cannot access members using .x and .y, like with the OpenCL kernel equivalents.
 * Even so, swizzling (e.g. val.yzw) is not supported.
 */

class int2 {
public:
	int x, y;
};
class int3 {
public:
	int x, y, z;
};
class int4 {
public:
	int x, y, z, w;
};

class uint2 {
public:
	unsigned int x, y;
};
class uint3 {
public:
	unsigned int x, y, z;}
};
class uint4 {
public:
	unsigned int x, y, z, w;
};

class float2 {
public:
	float x, y;
};
class float3 {
public:
	float x, y, z;
};
class float4 {
public:
	float x, y, z, w;
};
