CXX?=g++
MPICXX?=mpic++

perf?=1
ifeq "$(perf)" "1"
	CFLAGS += -D_PERF_
endif

# these are for running on Euler!
CFLAGS+=-Wall -O3 -std=c++11
CFLAGS_THREADS=$(CFLAGS) -fopenmp


all: diffusion2d_serial diffusion2d_openmp diffusion2d_mpi

diffusion2d_serial: diffusion2d_openmp.cpp
	$(CXX) $(CFLAGS) -o $@ $<

diffusion2d_openmp: diffusion2d_openmp.cpp
	$(CXX) $(CFLAGS_THREADS) -o $@ $<

diffusion2d_mpi: diffusion2d_mpi.cpp
	$(MPICXX) $(CFLAGS) -o $@ $<

clean:
	rm -f diffusion2d_serial diffusion2d_openmp diffusion2d_mpi
