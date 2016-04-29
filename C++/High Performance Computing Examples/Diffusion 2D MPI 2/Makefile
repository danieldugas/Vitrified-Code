# Makefile

SHELL := /bin/bash
CXX?=g++
CXXFLAGS?= -Wall -O3 -std=c++11 -lstdc++
MPICXX?=mpicxx

.DEFAULT: all

.PHONY: all
all: diffusion_serial diffusion_mpi

diffusion_serial: diffusion2d_serial.cpp
	$(CXX) $(CXXFLAGS) -o diffusion_serial diffusion2d_serial.cpp

diffusion_mpi: diffusion2d_mpi.cpp
	$(MPICXX) $(CXXFLAGS) -o diffusion_mpi diffusion2d_mpi.cpp

.PHONY: clean
clean:
	-rm diffusion_serial
	-rm diffusion_mpi
