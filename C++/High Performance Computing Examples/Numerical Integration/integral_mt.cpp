#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include "timer.hpp"

double f(double x)
{
	return std::log(x)*std::sqrt(x);
}

// WolframAlpha: integral_1^4 log(x) sqrt(x) dx = 4/9 (4 log(64)-7) ~~ 4.28245881486164
int main(int argc, char *argv[])
{
	double a = 1.0;
	double b = 4.0;
	unsigned long n = 200UL*35389440UL;

	unsigned int nthreads = 1;
	// Get number of threads from program arguments (if provided)
	if (argc > 1)
		nthreads = atoi(argv[1]);
	unsigned long nsteps = n / nthreads;
	if(nsteps * nthreads != n)
	{
		std::cout << "WARNING: n=" << n << " is not a multiple of nthreads=" << nthreads << ". ";
		n = nsteps * nthreads;
		std::cout << "Using n=" << n << " instead." << std::endl;
	}

	const double dx = (b-a)/n;

	std::vector<std::thread> threads(nthreads);

	std::pair<double,std::mutex> S;
	S.first = 0;

	timer t;
	t.start();
	for (unsigned thr = 0; thr < nthreads; thr++) {
#if DBG
		std::cout << "spawning thread " << thr << std::endl;
#endif
		threads[thr] = std::thread([&S,a,nsteps,dx,thr]() {
			double local_S = 0;
			const double x0 = a + (thr * nsteps + 0.5)*dx;
			for (unsigned long i = 0; i < nsteps; i++) {
				double xi = x0 + i*dx;
				local_S += f(xi);
			}
			local_S *= dx;
			std::lock_guard<std::mutex> l(S.second);
			S.first += local_S;
		});
	}

	for (std::thread& thr : threads)
		thr.join();

	t.stop();

	std::cout << "Threads=" << nthreads << " Time=" << t.get_timing() << " seconds, Result=" << std::setprecision(8) << S.first << std::endl;

	return 0;
}
