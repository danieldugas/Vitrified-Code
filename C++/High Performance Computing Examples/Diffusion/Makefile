CXX?=g++

CFLAGS=-Wall -O3 -std=c++11
CFLAGS_THREADS=$(CFLAGS) -pthread

all: barrier_test diffusion2d_serial diffusion2d_threaded diffusion2d_barrier

barrier_test: barrier_test.cpp inc/timer.hpp
	$(CXX) $(CFLAGS_THREADS) -o $@ $< -Iinc

diffusion2d_serial: diffusion2d_serial.cpp inc/timer.hpp
	$(CXX) $(CFLAGS) -o $@ $< -Iinc

diffusion2d_threaded: diffusion2d_threaded.cpp inc/timer.hpp
	$(CXX) $(CFLAGS_THREADS) -o $@ $< -Iinc

diffusion2d_barrier: diffusion2d_barrier.cpp inc/barrier.hpp inc/timer.hpp
	$(CXX) $(CFLAGS_THREADS) -o $@ $< -Iinc

clean:
	rm -f barrier_test diffusion2d_serial diffusion2d_threaded diffusion2d_barrier
