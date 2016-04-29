all: simple_sampling_serial simple_sampling_omp

simple_sampling_serial: simple_sampling_serial.cpp
	g++ -Wall -O3 --std=c++11 -o simple_sampling_serial simple_sampling_serial.cpp

simple_sampling_omp: simple_sampling_omp.cpp
	g++ -Wall -O3 --std=c++11 -fopenmp -o simple_sampling_omp simple_sampling_omp.cpp

clean:
	rm -f simple_sampling_serial simple_sampling_omp
