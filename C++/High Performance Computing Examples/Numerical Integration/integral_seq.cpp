#include <iostream>
#include <iomanip>
#include <cmath>
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
	unsigned long const n = 1e9;
	const double dx = (b-a)/n;

	double S = 0;
	timer t;

	t.start();
	for (unsigned long i = 0; i < n; i++) {
		double xi = a + (i + 0.5)*dx;
		S += f(xi);
	}
	S *= dx;
	t.stop();

	std::cout << " Time=" << t.get_timing() << " seconds, Result=" << std::setprecision(8) << S << std::endl;

	return 0; 
}
