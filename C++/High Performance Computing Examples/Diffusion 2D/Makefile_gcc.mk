CXX=g++

perf=0

CXXFLAGS=-Wall -O3 -std=c++11
CXXFLAGS_THREADS=$(CXXFLAGS) -fopenmp

ifeq "$(perf)" "1"
	CXXFLAGS+=-D_PERF_
endif

all: diffusion2d_serial_gcc diffusion2d_openmp_gcc diffusion2d_openmp_barrier_gcc 

diffusion2d_serial_gcc: diffusion2d_serial.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

diffusion2d_openmp_gcc: diffusion2d_openmp.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS_THREADS) -o $@ $<

diffusion2d_openmp_barrier_gcc: diffusion2d_openmp_barrier.cpp inc/timer.hpp
	$(CXX) $(CXXFLAGS_THREADS) -o $@ $<

clean:
	rm -f diffusion2d_serial_gcc diffusion2d_openmp_gcc diffusion2d_openmp_barrier_gcc
