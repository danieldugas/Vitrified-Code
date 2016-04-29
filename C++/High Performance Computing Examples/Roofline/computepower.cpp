/*
 *  computepower.cpp
 *
 *  Created by Christian Conti on 9/21/11.
 *  Extended by Panagiotis Hadjidoukas on 11/1/14
 *  Copyright 2015 ETH Zurich. All rights reserved.
 *
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <cassert>

#include <algorithm>
#include <sys/time.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

#include "ArgumentParser.h"

#define MULADD(b, x, a) (b*(x+a))
#define SUM8(x0, x1, x2, x3, x4, x5, x6, x7) (((x0 + x1) + (x2 + x3)) + ((x4 + x5) + (x6 + x7)))


//========================================================================================
//
//	Helper Functions
//
//========================================================================================

// timer method
double mysecond()
{
#if defined(_OPENMP)
	return omp_get_wtime();
#else
	struct timeval tp;
	struct timezone tzp;
	int i;
	
	i = gettimeofday(&tp,&tzp);
	return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
#endif
}


void ComputePower(double * s, int iSize)
{
	const int static N = iSize;
	
	{		
		// store coefficients in registers
		const double a0 = 0.01f;
		const double a1 = 0.035f;
		const double a2 = 0.001f;
		const double a3 = 0.15f;
		
		const double b0 = 0.012f;
		const double b1 = 0.067f;
		const double b2 = 0.02f;
		const double b3 = 0.21f;
		
		
		// 8 independent streams of computations
		// this helps filling the processing pipeline
		// 8 data reads
		double x0 = s[0];
		double y0 = s[0];
		double z0 = s[0];
		double w0 = s[0];
		double r0 = s[0];
		double t0 = s[0];
		double u0 = s[0];
		double v0 = s[0];
		
		// compute independently 8 polynomial evaluations with horner's scheme of degree 4N
		// 4*8*2*N FLOP
		for (int i=0; i<N; i++)
		{
			double gx0 = MULADD(b0, x0, a0);
			double gy0 = MULADD(b0, y0, a0);
			double gz0 = MULADD(b0, z0, a0);
			double gw0 = MULADD(b0, w0, a0);
			double gr0 = MULADD(b0, r0, a0);
			double gt0 = MULADD(b0, t0, a0);
			double gu0 = MULADD(b0, u0, a0);
			double gv0 = MULADD(b0, v0, a0);
			
			double tx0 = MULADD(b1, gx0, a1);
			double ty0 = MULADD(b1, gy0, a1);
			double tz0 = MULADD(b1, gz0, a1);
			double tw0 = MULADD(b1, gw0, a1);
			double tr0 = MULADD(b1, gr0, a1);
			double tt0 = MULADD(b1, gt0, a1);
			double tu0 = MULADD(b1, gu0, a1);
			double tv0 = MULADD(b1, gv0, a1);
			
			double ux0 = MULADD(b2, tx0, a2);
			double uy0 = MULADD(b2, ty0, a2);
			double uz0 = MULADD(b2, tz0, a2);
			double uw0 = MULADD(b2, tw0, a2);
			double ur0 = MULADD(b2, tr0, a2);
			double ut0 = MULADD(b2, tt0, a2);
			double uu0 = MULADD(b2, tu0, a2);
			double uv0 = MULADD(b2, tv0, a2);
			
			x0 = MULADD(b3, ux0, a3);
			y0 = MULADD(b3, uy0, a3);
			z0 = MULADD(b3, uz0, a3);
			w0 = MULADD(b3, uw0, a3);
			r0 = MULADD(b3, ur0, a3);
			t0 = MULADD(b3, ut0, a3);
			u0 = MULADD(b3, uu0, a3);
			v0 = MULADD(b3, uv0, a3);
		}
		
		// sum the results of the 8 polynomial evaluations
		// and store in the output array
		// without this step, the compiler might simplify the code by discarding unused computation and data
		// 1 data write, 7 FLOP (irrelevant if N is large enough)
		s[0] = SUM8(x0, y0, z0, w0, r0, t0, u0, v0);
	}
}

//========================================================================================
//
//	main
//
//========================================================================================

int main(int argc, const char ** argv)
{
	ArgumentParser parser(argc,argv);

	// get the number of threads
	int nthreads = 0;
	#pragma omp parallel
	{
	#pragma omp atomic
	nthreads += 1;
	}
	
	// getting parameters
	int iSize = (int)parser("-size").asDouble(1.e8);
	int iIteration = (int)parser("-iterations").asDouble(10.);
	
	// running benchmarks
	double * timeHOI = new double[iIteration];
	map<string, vector<double> > peakPerformance;

	double * s = new double;

	// initialize value for the polynomial evaluation
	s[0] = 1e-6;
	
	// run the benchmark iIteration times
	for (int i=0; i<iIteration; i++)
	{
		#pragma omp parallel
		{
		ComputePower(s,iSize);
		}
	}

	for (int i=0; i<iIteration; i++)
	{
		//double * s = new double;
		//double s;
	
		timeHOI[i] = mysecond();
		#pragma omp parallel
		{
		ComputePower(s,iSize);
		}
		timeHOI[i] = mysecond() - timeHOI[i];
	}
	
	cout << "\nSummary\n";
	
	// compute performance of each benchmark run
	for (int i=0; i<iIteration; i++)
		peakPerformance["HOI"].push_back(1.e-9*(double)iSize*4*2*8*nthreads / timeHOI[i]);
	
	sort(peakPerformance["HOI"].begin(), peakPerformance["HOI"].end());
	
	// percentiles to be selected
	const int I1 = iIteration*.1;
	const int I2 = iIteration*.5;
	const int I3 = iIteration*.9;
	
	// output 10th, 50th, 90th percentiles
	cout << I1 << " Ranked Peak Performance\t" << I2 << " Ranked Peak Performance (median)\t" << I3 << " Ranked Peak Performance\n";
	cout << peakPerformance["HOI"][I1] << " GFLOP/s\t" << peakPerformance["HOI"][I2] << " GFLOP/s\t" << peakPerformance["HOI"][I3] << " GFLOP/s\n";
}
