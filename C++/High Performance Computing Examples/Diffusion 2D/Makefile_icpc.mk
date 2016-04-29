CXX=icpc

perf=0

CXXFLAGS=-Wall -O3 -std=c++11
CXXFLAGS_THREADS=$(CXXFLAGS) -fopenmp

ifeq "$(perf)" "1"
	CXXFLAGS+=-D_PERF_
endif

all: diffusion2d_serial_icpc diffusion2d_openmp_icpc diffusion2d_openmp_barrier_icpc 

diffusion2d_serial_icpc: diffusion2d_serial.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS) -gcc-name=/cluster/apps/gcc/4.9.2/bin/gcc -o $@ $<

diffusion2d_openmp_icpc: diffusion2d_openmp.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS_THREADS) -gcc-name=/cluster/apps/gcc/4.9.2/bin/gcc -o $@ $<

diffusion2d_openmp_barrier_icpc: diffusion2d_openmp_barrier.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS_THREADS) -gcc-name=/cluster/apps/gcc/4.9.2/bin/gcc -o $@ $<

clean:
	rm -f diffusion2d_serial_icpc diffusion2d_openmp_icpc diffusion2d_openmp_barrier_icpc
