CXX?=g++
CXXFLAGS?=-std=c++11 -O3 -Wall
OMPFLAG?=-fopenmp
PRINT_CONFIGS?=


all: nbody_serial_nocells nbody_omp_cells

nbody_serial_nocells: nbody_serial_nocells.cpp
	$(CXX) $(CXXFLAGS) -I. $(PRINT_CONFIGS) $^ -o $@

nbody_omp_cells: nbody_omp_cells.cpp
	$(CXX) $(CXXFLAGS) $(OMPFLAG) -I. $(PRINT_CONFIGS) $^ -o $@

clean:
	rm -f nbody_serial_nocells nbody_omp_cells
