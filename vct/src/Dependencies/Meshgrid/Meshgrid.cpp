#include <math.h>
#include "Meshgrid.h"


#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

Meshgrid::Meshgrid(int s_x, int s_y)
{
	size_x=s_x;
	size_y=s_y;

	meshx= new float*[size_x];
	meshy= new float*[size_x];
	radius = new float*[size_x];
	angle = new float*[size_x];
	
	for(int i=0;i<size_x;i++){
		*(meshx+i)=new float[size_y];
		*(meshy+i)=new float[size_y];
		*(radius+i)=new float[size_y];
		*(angle+i)=new float[size_y];
	}
}



int Meshgrid::Initialize(){

	for(int i=0;i<this->getsizex();i++)
		for(int j=0;j<this->getsizey();j++)
		{
			meshx[i][j]=(float)j;
			meshy[i][j]=(float)i;
			radius[i][j]=(float)sqrt((float)(j*j+i*i));
			angle[i][j]=(float)atan2((float)i,(float)j);
		}
	return 1;
}

int Meshgrid::Initialize(float* Nloc){

	for(int i=0;i<this->getsizex();i++)
		for(int j=0;j<this->getsizey();j++)
		{
			meshx[i][j]=(float)j-Nloc[0];
			meshy[i][j]=(float)i-Nloc[1];
			radius[i][j]=(float)sqrt(pow(j-Nloc[0],2)+pow(i-Nloc[1],2));
			angle[i][j]=(float)atan2((float)i,(float)j);
		}
	return 1;
}

int Meshgrid::Initialize(float* Nloc,float Nang){

	for(int i=0;i<this->getsizex();i++)
		for(int j=0;j<this->getsizey();j++)
		{
			meshx[i][j]=(float)j-Nloc[0];
			meshy[i][j]=(float)i-Nloc[1];
			radius[i][j]=(float)sqrt(pow(j-Nloc[0],2)+pow(i-Nloc[1],2));
			angle[i][j]=(float)atan2(i-Nloc[1],j-Nloc[0])-Nang;
		}
	return 1;
}

int Meshgrid::Initialize(float* Nloc,float Nang,float ratio){
	
	float a=(float)sqrt(ratio);
	float b=(float)1/a;
	for(int i=0;i<this->getsizex();i++)
		for(int j=0;j<this->getsizey();j++)
		{
			//angle[i][j]=(float)(atan2(i-Nloc[1],j-Nloc[0])-Nang);
			meshx[i][j]=(float)((j-Nloc[0])*cos(Nang) + (i-Nloc[1])*sin(Nang));
			meshy[i][j]=(float)(-(j-Nloc[0])*sin(Nang) + (i-Nloc[1])*cos(Nang));
			
			radius[i][j]=(float)sqrt(pow((((j-Nloc[0])*cos(Nang) + (i-Nloc[1])*sin(Nang)))/a,2)
						+pow((-(j-Nloc[0])*sin(Nang) + (i-Nloc[1])*cos(Nang))/b,2));
			
		}
	return 1;
}


// this is tricky better change this to something better
float** Meshgrid::getradius(){
		return radius;
}

float** Meshgrid::getangle(){
	return angle;
}


//end of tricky part, allthough the destructor will destroy these calculated radius and angle
//  these pointers will point to rubbish of used later on.


int Meshgrid::setsize(int s_x,int s_y){
	size_x=s_x;
	size_y=s_y;
	return 1;
}

Meshgrid::~Meshgrid()
{
	for(int i=0;i<size_x;i++){
		delete [] *(meshx+i);
		delete [] *(meshy+i);
		delete [] *(radius+i);
		delete [] *(angle+i);
	}
	delete [] meshx;
	delete [] meshy;
	delete [] radius;
	delete [] angle;

}

int Meshgrid::getsizex()
{
	return size_x;
}

int Meshgrid::getsizey()
{
	return size_y;
}

