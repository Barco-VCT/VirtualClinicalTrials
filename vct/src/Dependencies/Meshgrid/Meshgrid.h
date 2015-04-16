//////////////////////////////////////////////////////////////////
//                Simulation                                    //
//                VCT: Virtual Clinical Trials                  //
//////////////////////////////////////////////////////////////////

/*!
 * \file Meshgrid.h
 * \brief Implementation of class MeshGrid
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
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


class Meshgrid
{
public:
	Meshgrid(int size_x = 0, int size_y=0);
	//Meshgrid(int size_x = 0, int size_y=0, float* Nloc=NULL);
public:
	virtual ~Meshgrid();
	//float** calculate_radius();
	//float** calculate_angle();
	int Initialize(); //build meshgrid (meshx, meshy and radius,angle)
	int Initialize(float* Nloc); //same as previous but with a displacement
	int Initialize(float* Nloc,float Nang);  //displacemetn + rotation
	int Initialize(float* Nloc,float Nang,float ratio); //displacement + anisotropic deformation + rotation
	//set functions
	int setsize(int s_x,int s_y);
	
	//getfunctions
	int getsizex();
	int getsizey();
	float** getradius();
	float** getangle();
private:
	int size_x; //size rows of the image (square for now)
	int size_y; //size rows of the image (square for now)
	float** meshx;
	float** meshy;
	float** radius;
	float** angle;
};

