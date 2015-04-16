/*
Version of the OneShot algorithm for continuous decision outputs.  
Should work efficiently with large datasets because we pre-compute all
the marginals.

NOTE:  64-bit integers are needed for this code.  I have also tried to
make this program as ANSI-standards compliant as I can.  

Questions and bugs should be addressed to:

	Matthew A. Kupinski, Ph.D.
	kupinski@radiology.arizona.edu
	Assistant Professor
	College of Optical Sciences and
	Department of Radiology
	The University of Arizona
	Tucson, Arizona  85721

References:

@article{Gallas:06,
	Author = {Brandon D. Gallas},
	Journal = {Academic Radiology},
	Pages = {353-362},
	Title = {One-shot estimate of MRMC Variance: AUC},
	Volume = {13},
	Year = {2006}}
	
@article{Clarkson:05,
	Author = {Eric Clarkson and Matthew A. Kupinski and Harrison H. Barrett},
	Journal = {Academic Radiology},
	Title = {A probabilistic development of the {MRMC} method},
	Year = {2006 (under review)}}
*/

#include "../OneShot.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// get uint64_t
#ifdef _MSC_VER // Microsoft compiler
#  include <basetsd.h> // [Ljiljana] UINT64
   typedef UINT64 uint64_t;
#else
#  include <stdint.h>
#endif
// [Ljiljana] #include <unistd.h>	// command option parsing (see: getopt(), optind)



// Check to make sure we have enough memory when we malloc.
// The while(0) at the end makes the programmer (me) add a ";" to 
// the end of the line.
#define MALLOCCHECK(expr) if ((expr) == NULL){\
	printf("Memory allocation error--Out of memory!\n");\
	exit(1);\
	}while(0)


void one_shot(char *toutFileName,char *foutFileName,char *outputFileName,int biasFlag,int verboseFlag)
{
	FILE *toutFile;
	FILE *foutFile;
	FILE * results; // store results in a file

	// Maximum number of image pairs is 65535.
	// Any more than this and we get beyond the 64-bit range
	// in the computations.
	double **tout;
	double **fout;
	int N0,N1,Nr,Nr1;
	
	uint64_t **Margin1;
	uint64_t **Margin0;
	uint64_t **MarginR;
	uint64_t *Margin01;
	uint64_t *MarginR0;
	uint64_t *MarginR1;
	uint64_t totalSqSum;
	uint64_t totalSumSq;
	
	uint64_t tmp;
	int i,j,r;
	
	uint64_t sjr2,sir2,sij2,sr2,sj2,si2;
	uint64_t M1,M2,M3,M4,M5,M6,M7,M8;
	double M1f,M2f,M3f,M4f,M5f,M6f,M7f,M8f;
	double Nrf,N0f,N1f;
	double a[7];
	double sDev;

	
	/* read scores for the diseased dataset*/
	if ((toutFile = fopen(toutFileName,"r")) == NULL) {
		printf("Cannot open file: %s\n",toutFileName);
		exit(1);
	}
	/* read scores for the healthy dataset*/
	if ((foutFile = fopen(foutFileName,"r")) == NULL) {
		printf("Cannot open file %s\n",foutFileName);
		exit(1);
	}
	/* store the results */
	if ((results = fopen(outputFileName,"wt")) == NULL) {
		printf("Cannot open file %s\n",outputFileName);
		exit(1);
	}
	
	printf ( "\n--- START: OneShot analysis\n\n" );
	fprintf(results, "--- START: OneShot analysis\n\n");

	// Read in the files
	fscanf(foutFile,"%d%d",&N0,&Nr);
	fscanf(toutFile,"%d%d",&N1,&Nr1);
	if (Nr1 != Nr) {
		printf("Number of readers must be the same between the two datasets - Nr1=%d, Nr=%d\n",Nr1,Nr);
  	fprintf(results, "Number of readers must be the same between the two datasets - Nr1=%d, Nr=%d\n",Nr1,Nr);
		exit(1);
	}

	// Test to make sure that we won't have precision problems
	// Hopefully this is me being overly cautious.
	if (((double)N0 * (double)N1 * (double)Nr) > 1.8e19) {
		printf("Too many images/readers.  This may cause the code to\n");
		printf("go outside the bounds of 64-bit arithmetic.\n");
		exit(1);
	}

	// Allocate space and read in data
	MALLOCCHECK(tout = malloc(sizeof(double *) * Nr));
	MALLOCCHECK(fout = malloc(sizeof(double *) * Nr));
	for (r = 0;r < Nr;r++) {
		MALLOCCHECK(tout[r] = malloc(sizeof(double) * N1));
		MALLOCCHECK(fout[r] = malloc(sizeof(double) * N0));
	}

	for (i = 0;i < N0;i++) {
		for (r = 0;r < Nr;r++) {
			fscanf(foutFile,"%lf",(double *)&(fout[r][i]));
		}
	}
	for (j = 0;j < N1;j++) {
		for (r = 0;r < Nr;r++) {
			fscanf(toutFile,"%lf",(double *)&(tout[r][j]));
		}
	}

	fclose(foutFile);
	fclose(toutFile);

	MALLOCCHECK(Margin1 = malloc(sizeof(uint64_t*) * Nr));
	MALLOCCHECK(Margin0 = malloc(sizeof(uint64_t*) * Nr));
	MALLOCCHECK(MarginR = malloc(sizeof(uint64_t*) * N0));
	MALLOCCHECK(Margin01 = malloc(sizeof(uint64_t) * Nr));
	MALLOCCHECK(MarginR1 = malloc(sizeof(uint64_t) * N0));
	MALLOCCHECK(MarginR0 = malloc(sizeof(uint64_t) * N1));
	for (r = 0;r < Nr;r++) {
		MALLOCCHECK(Margin1[r] = malloc(sizeof(uint64_t) * N0));
		MALLOCCHECK(Margin0[r] = malloc(sizeof(uint64_t) * N1));
	}
	for (i = 0;i < N0;i++) {
		MALLOCCHECK(MarginR[i] = malloc(sizeof(uint64_t) * N1));
	}

	/* A few notes about these computations.  First, we are using 64-bit integer
	arithmetic because it is dangerous to add a bunch of small floating-point numbers
	together.  This danger is especiall evident when the total sum is very large.
	
	All of these terms are sums of step functions.  The convention used here (from the
	Gallas paper) is that step(0) = 1/2.  To alleviate this floating-point number, we
	use the convention that step(<0) = 0; step(>0) = 2; step(0) = 1.  We then divide much
	later in this code to convert our terms to have the correct value.  This is a bit
	ugly but it keeps us in integer-arithmetic and it is still very fast.
	
	Finally, the equations in the Gallas reference are many sums deep... as many as 6 
	sums in a row.  This is just too computationally burdensome especially if N0 and N1
	are large.  Thus, we have were able to rewrite the equations in the paper using all
	six marginals that can be computed from a 3D "matrix", i.e., a matrix that is N0 x N1 x
	Nr.  This has greatly reduced the computation time.  In fact, blind applications of the
	equations in the Gallas reference would take upwards of 5 days to compute with 2500 N0s, 
	2500 N1s, and 5 Nrs.  This is now reduced to less than a second.
	*/

	// Compute the marginals
	totalSqSum = 0;
	totalSumSq = 0;
	for (r = 0;r < Nr;r++) {
		Margin01[r] = 0;
		for (i = 0;i < N0;i++) {
			Margin1[r][i] = 0;
		}
		for (j = 0;j < N1;j++) {
			Margin0[r][j] = 0;
		}
	}
	for (i = 0;i < N0;i++) {
		MarginR1[i] = 0;
		for (j = 0;j < N1;j++) {
			MarginR[i][j] = 0;
		}
	}
	for (j = 0;j < N1;j++) {
		MarginR0[j] = 0;
	}
				
	for (r = 0;r < Nr;r++) {
		for (i = 0;i < N0;i++) {
			for (j=0;j < N1;j++) {
				tmp = (uint64_t)(tout[r][j] >= fout[r][i]);
				tmp += (uint64_t) (tout[r][j] > fout[r][i]);
				totalSumSq += tmp;
				totalSqSum += tmp * tmp;
				Margin1[r][i] += tmp;
				Margin0[r][j] += tmp;
				MarginR[i][j] += tmp;
				Margin01[r] += tmp;
				MarginR1[i] += tmp;
				MarginR0[j] += tmp;
			}
		}
	}
	
	// Needed sums
	sjr2 = 0;
	sir2 = 0;
	sij2 = 0;
	sr2 = 0;
	sj2 = 0;
	si2 = 0;
	for (r = 0;r < Nr;r++) {
		sr2 += Margin01[r] * Margin01[r];
		for (i = 0;i < N0;i++){
			sir2 += Margin1[r][i] * Margin1[r][i];
		}
		for (j = 0;j < N1;j++) {
			sjr2 += Margin0[r][j] * Margin0[r][j];
		}
	}
	for (i = 0;i < N0;i++) {
		for (j = 0;j < N1;j++) {
			sij2 += MarginR[i][j] * MarginR[i][j];
		}
		si2 += MarginR1[i] * MarginR1[i];
	}
	for (j = 0;j < N1;j++) {
		sj2 += MarginR0[j] * MarginR0[j];
	}
	
	sjr2 /= 4;
	sir2 /= 4;
	sij2 /= 4;
	sr2 /= 4;
	sj2 /= 4;
	si2 /= 4;
	totalSqSum /= 4;
	totalSumSq = (totalSumSq*totalSumSq) / 4;
	
	// Compute the sums
	if (biasFlag) {
		M1 = totalSqSum;
		M2 = sjr2;
		M3 = sir2;
		M4 = sr2;
		M5 = sij2;
		M6 = sj2;
		M7 = si2;
		M8 = (totalSumSq);	
	} else {
		M1 = totalSqSum;
		M2 = sjr2 - totalSqSum;
		M3 = sir2 - totalSqSum;
		M4 = sr2 - sjr2 - sir2 + totalSqSum;
		M5 = sij2 - totalSqSum;
		M6 = sj2 - sij2 - sjr2 + totalSqSum;
		M7 = si2 - sij2 - sir2 + totalSqSum;
		M8 = (totalSumSq) - M1 - M2 - M3 - M4 - M5 - M6 - M7;
	}
	
	if (verboseFlag) {
		printf("Nonnormalized M's  (1-8) \n");
		printf("%llu %llu %llu %llu %llu %llu %llu %llu\n\n",M1,M2,M3,M4,M5,M6,M7,M8);
		fprintf(results,"Nonnormalized M's  (1-8) \n");
		fprintf(results,"%llu %llu %llu %llu %llu %llu %llu %llu\n\n",M1,M2,M3,M4,M5,M6,M7,M8);
	}


	Nrf = (double)Nr;
	N1f = (double)N1;
	N0f = (double)N0;
	
	// Special case if Nr = 1.  In this case, only the first 4 Ms are needed as well as 
	// the first 3 alphas.
	if (Nr == 1) {
		if (biasFlag) {
			M1f = (double)M1 / (Nrf*N0f*N1f);
			M2f = (double)M2 / (Nrf * N0f * N1f * N0f);
			M3f = (double)M3 / (Nrf * N0f * N1f * N1f);
			M4f = (double)M4 / (Nrf * N0f * N1f * N1f * N0f);
		} else {
			M1f = (double)M1 / (Nrf*N0f*N1f);
			M2f = (double)M2 / (Nrf * N0f * N1f * (N0f - 1.0));
			M3f = (double)M3 / (Nrf * N0f * N1f * (N1f - 1.0));
			M4f = (double)M4 / (Nrf * N0f * N1f * (N1f - 1.0) * (N0f - 1.0));
		}
		
		if (verboseFlag) {
			printf("Normalized M's  (1-4)\n");
			printf("%lf %lf %lf %lf\n\n",M1f,M2f,M3f,M4f);
			fprintf(results,"Normalized M's  (1-4)\n");
			fprintf(results,"%lf %lf %lf %lf\n\n",M1f,M2f,M3f,M4f);
		}
		
		
		a[0] = M3f - M4f;
		a[1] = M2f - M4f;
		a[2] = M1f - M2f - M3f + M4f;
		a[3] = 0.0;
		a[4] = 0.0;
		a[5] = 0.0;
		a[6] = 0.0;
	} else {
		if (biasFlag) {
			M1f = (double)M1 / (Nrf*N0f*N1f);
			M2f = (double)M2 / (Nrf * N0f * N1f * N0f);
			M3f = (double)M3 / (Nrf * N0f * N1f * N1f);
			M4f = (double)M4 / (Nrf * N0f * N1f * N1f * N0f);
			M5f = (double)M5 / (Nrf * N0f * N1f * Nrf);
			M6f = (double)M6 / (Nrf * N0f * N1f * Nrf * N0f);
			M7f = (double)M7 / (Nrf * N0f * N1f * Nrf * N1f);
			M8f = (double)M8 / (Nrf * N0f * N1f * Nrf * N0f * N1f);
		} else {
			M1f = (double)M1 / (Nrf*N0f*N1f);
			M2f = (double)M2 / (Nrf * N0f * N1f * (N0f - 1.0));
			M3f = (double)M3 / (Nrf * N0f * N1f * (N1f - 1.0));
			M4f = (double)M4 / (Nrf * N0f * N1f * (N1f - 1.0) * (N0f - 1.0));
			M5f = (double)M5 / (Nrf * N0f * N1f * (Nrf - 1.0));
			M6f = (double)M6 / (Nrf * N0f * N1f * (Nrf - 1.0) * (N0f - 1.0));
			M7f = (double)M7 / (Nrf * N0f * N1f * (Nrf - 1.0) * (N1f - 1.0));
			M8f = (double)M8 / (Nrf * N0f * N1f * (Nrf - 1.0) * (N0f - 1.0) * (N1f - 1.0));
		}

		if (verboseFlag) {
			printf("Normalized M's  (1-8)\n");
			printf("%lf %lf %lf %lf %lf %lf %lf %lf\n\n",M1f,M2f,M3f,M4f,M5f,M6f,M7f,M8f);
			fprintf(results,"Normalized M's  (1-8)\n");
			fprintf(results,"%lf %lf %lf %lf %lf %lf %lf %lf\n\n",M1f,M2f,M3f,M4f,M5f,M6f,M7f,M8f);
		}
	
		// NB:  alpha_i = a[i-1]
		a[0] = M7f - M8f;
		a[1] = M6f - M8f;
		a[2] = M5f - M6f - M7f + M8f;
		a[3] = M4f - M8f;
		a[4] = M3f - M4f - M7f + M8f;
		a[5] = M2f - M4f - M6f + M8f;
		a[6] = M1f - M2f - M3f + M4f - M5f + M6f + M7f - M8f;
	}
	
	if (verboseFlag) {
		double tpRes=0.0;
		sDev = sqrt(abs(a[0]/N0f + a[1]/N1f + a[2]/(N0f*N1f) + a[3]/(Nrf) + a[4]/(Nrf*N0f) + a[5]/(Nrf*N1f)	+ a[6]/(Nrf*N0f*N1f)));
		//		* a[6]/(Nrf*N0f*N1f));
		printf("AUC = %lf   SDev = %lf\n\n",sqrt((double)totalSumSq) / (N1f*N0f*Nrf),sDev);
	    fprintf(results, "AUC = %lf   SDev = %lf\n\n",sqrt((double)totalSumSq) / (N1f*N0f*Nrf),sDev);
	}
	
  if (verboseFlag) {
    printf("alphas (1-7)\n");
    fprintf(results, "\n");
    /* store the results */
    fprintf(results, "alphas (1-7)\n");
	  for (i = 0;i < 7;i++){
		  printf("%lf ",a[i]);
		  fprintf(results, "%lf ",a[i]);
	  }
	  printf("\n");
  }

	//  Free up memory
	for (i = 0;i < N0;i++) {
		free(MarginR[i]);
	}
	for (r = 0;r < Nr;r++) {
		free(Margin1[r]);
		free(Margin0[r]);
	}
	free(Margin1);
	free(Margin0);
	free(MarginR);
	free(Margin01);
	free(MarginR1);
	free(MarginR0);
	for (r = 0;r < Nr;r++) {
		free(tout[r]);
		free(fout[r]);
	}
	free(tout);
	free(fout);

  printf ( "\n--- END:   OneShot analysis\n" );
	fprintf(results, "\n--- END:   OneShot analysis\n");

	fclose(results);

}


void usage() {
	printf("\nSYNOPSIS\n");
	printf("	OneShot [-bv] toutFile foutFile\n\n");
	printf("DESCRIPTION\n");
	printf("	Computes the one-shot estimate of the alphas -- the coefficients characterizing\n");
	printf("	the variance of the estimate of the AUC as a function of the numbers of readers\n");
	printf("	and cases.\n\n");
	printf("	The following options are available:\n\n");
	printf("	-b        Produces biased estimates of the alphas.\n");
	printf("	          Basically, doesn't divide by N-1 in computing variances.\n\n");
	printf("	-v        Verbose output.\n\n");
	printf("	toutFile  The reader scores for the with-disease images. Text file containing\n");
	printf("	          floating-point scores.  Columns are differt readers.  Rows are\n");
	printf("	          different images.\n\n");
	printf("	foutFile  The reader scores for the without-disease images. Text file containing\n");
	printf("	          floating-point scores.  Columns are differt readers.  Rows are\n");
	printf("	          different images.\n\n");
	printf("	The first line of the toutFile and foutFile must have the numbers of images and\n");
	printf("	readers.  The variance of the AUC goes as:\n");
	printf("	alpha_1/N_0 + alpha_2/N_1 + alpha_3/N_0N_1 + alpha_4/Nr + alpha_5/NrN0 + \n");
	printf("	alpha_6/NrN1 + alpha_7/NrN0N1\n");
	printf("	where N0 is the number of without-disease images, N1 is the number of with-disease\n");
	printf("	images, and Nr is the number of readers\n\n");
	printf("REFERENCES:\n");
	printf("	See code comments for references\n\n");
}

