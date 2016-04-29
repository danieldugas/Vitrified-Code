/*
 *  bandwidth.cpp
 *
 *  Created by Christian Conti on 21/9/12.
 *  Extended by	Panagiotis Hadjidoukas on 11/1/14
 *  Copyright 2015 ETH Zurich. All rights reserved.
 *
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <cassert>
#include <sys/time.h>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include <numa.h>

#include "ArgumentParser.h"

using namespace std;

//========================================================================================
//
//	Benchmark
//
//========================================================================================

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

void mycopy(const float* const a, float* const b, const int N)
{
#if !defined(_RANGE_)
	#pragma omp parallel for 
#endif
	for (int j=0; j<N; j++)
		b[j] = a[j];
}

void myadd(const float* const a,  float* const b,  float* const c,  const int N)
{
#if !defined(_RANGE_)
	#pragma omp parallel for 
#endif
	for (int j=0; j<N; j++)
		b[j] = a[j]+c[j];
}

vector<double> benchmark(int N, size_t NTIMES)
{
#if !defined(_USE_NUMA_)
#define numa_alloc_interleaved malloc
#define numa_free(x,y)	       free(x)
#endif

	float * a = (float*)numa_alloc_interleaved(sizeof(float)*N);
	float * b = (float*)numa_alloc_interleaved(sizeof(float)*N);
#if defined(_TRIADD_)
	float * c = (float*)numa_alloc_interleaved(sizeof(float)*N);
#endif
	// check that the arrays are correctly allocated
	if (a == NULL || ((size_t)a & 0xf) != 0)
	{
		cout << "a is NULL! aborting" << endl;
		abort();
	}
	
	if (b == NULL || ((size_t)b & 0xf) != 0)
	{
		cout << "b is NULL! aborting" << endl;
		abort();
	}

#if defined(_TRIADD_)
	if (c == NULL || ((size_t)c & 0xf) != 0)
	{
		cout << "c is NULL! aborting" << endl;
		abort();
	}
#endif
	
	// fill the arrays
#if !defined(_RANGE_)
	#pragma omp parallel for 
#endif
	for (int j=0; j<N; j++)
	{
		a[j] = 2.0;
		b[j] = 1.0;
#if defined(_TRIADD_)
		c[j] = 1.0;
#endif
	}
	
	// vector containing timing of each sample
	vector<double> result(NTIMES);
	
	// run benchmark
	for (int j=0; j<NTIMES; ++j)
	{
		const double tstart = mysecond();
#if defined(_TRIADD_)
		myadd(a, b, c, N);
#else
		mycopy(a, b, N);
#endif
		const double tend = mysecond();
		
		result[j] =  tend - tstart;
		
		swap(a, b);
	}
	
	sort(result.begin(), result.end());
	
	numa_free(a, sizeof(float)*N);
	numa_free(b, sizeof(float)*N);
#if defined(_TRIADD_)
	numa_free(c, sizeof(float)*N);
#endif
	
	return result;
}

void driver(const int N, int NTIMES, const int wordsize)
{
	// running benchmarks
	{
		vector<double> timings;
		
		timings = benchmark(N, NTIMES);
		
#if defined(_TRIADD_)
		const double denom = 1.0E-9 * 3. * wordsize * N;
#else
		const double denom = 1.0E-9 * 2. * wordsize * N;
#endif
		
		if (timings.size() == 0)
		{
		    cout << "Error: 0 Samples collected. Aborting.\n";
		    abort();
		}
		
		static bool first = true;
		if (first)
		{
			cout << "Footprint [KB]\t\tN\t\tBandwidth[GB/s]\n";
			first = false;
		}
		
		// we look at the 50th percentile
		const int i2 = 0.50*NTIMES;

#if defined(_TRIADD_)
		cout << 3*wordsize*N/1024. << "\t\t" << N << "\t\t" << denom/(timings[i2]) << endl;
#else
		cout << 2*wordsize*N/1024. << "\t\t" << N << "\t\t" << denom/(timings[i2]) << endl;
#endif
	}	
}

int main(int argc, const char ** argv)
{
	ArgumentParser parser(argc,argv);
	
	// getting parameters
	const bool RANGE = parser("-range").asBool(false);
	const int N = parser("-size").asDouble(40000000);
	size_t NTIMES = parser("-iterations").asDouble(100);
	const int wordsize = sizeof(float);
	
	const float KB = 2.*wordsize*N/1024.;
	
	if (KB < 2)
	{
		cout << "This size is to small! aborting. " << KB << " KB\n" ;
		abort();
	}			 
	
	// running benchmarks
	if(!RANGE)
		driver(16*((N+15)/16), NTIMES, wordsize);
	else 
	{
		double myN = N;
		const size_t NEND = N*5e5;
		while (myN < NEND)
		{
			driver(16*(int)((myN+15)/16), NTIMES, wordsize);
			
			// this factor is arbitrary and can be tuned
			myN *= 1.05;
		}
	}	
}
