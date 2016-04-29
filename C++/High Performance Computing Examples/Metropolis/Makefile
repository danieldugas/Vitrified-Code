#CXX=g++-mp-4.8
 CXX?=g++

CFLAGS=-O3 -std=c++11 -DNDEBUG

all: disks_serial disks_openmp

disks_serial: disks_serial.cpp disks_mc.hpp vector_stats.hpp timer.hpp
	$(CXX) $(CFLAGS) $< -o $@

disks_openmp: disks_openmp.cpp disks_mc.hpp vector_stats.hpp timer.hpp
	$(CXX) $(CFLAGS) -fopenmp $< -o $@

clean:
	rm -f disks_serial disks_openmp
