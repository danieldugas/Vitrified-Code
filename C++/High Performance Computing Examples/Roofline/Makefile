# Makefile

SHELL := /bin/bash

#######################
# USER SETUP
#######################
# use release or debug settings
config ?= release
omp ?= 1
numa ?= 1
triadd ?= 0
range ?= 0
# define compiler
CC=gcc
LD=$(CC)
#######################

# define flags (should work as-is)

# test flags with ifneq (,$(VAR))
ISGCC := $(filter gcc,$(CC))

ifeq "$(omp)" "1"
CFLAGS = -fopenmp
endif

ifeq "$(numa)" "1"
CFLAGS += -D_USE_NUMA_
endif

ifeq "$(triadd)" "1"
CFLAGS += -D_TRIADD_
endif

ifeq "$(range)" "1"
CFLAGS += -D_RANGE_
endif


ifeq "$(config)" "release"
  CFLAGS += -O3 
else
  CFLAGS += -g
endif

STATIC_LIBRARIES = -lstdc++ -lm

ifeq "$(numa)" "1"
STATIC_LIBRARIES += -lnuma
endif



# define paths for source files
VPATH = .

all: computepower bandwidth

computepower: computepower.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $(STATIC_LIBRARIES) $^ -o $@

bandwidth: bandwidth.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $(STATIC_LIBRARIES) $^ -o $@
	
clean:
	rm -f *.o
	rm -f computepower bandwidth
