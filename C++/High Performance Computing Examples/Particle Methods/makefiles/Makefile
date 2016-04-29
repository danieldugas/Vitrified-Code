CC=mpic++

CPPFLAGS += -I../source/ -fopenmp
CPPFLAGSOPT += -O3
LIBS += -lstdc++ -fopenmp

OBJS_SHEET = \
	mainSheet.o \
	VelocitySolverNSquared.o

VPATH := ../source/
.DEFAULT_GOAL := sheet

sheet: $(OBJS_SHEET)
	$(CC) $(LIBS) $^ -o $@
	@echo done

%.o: %.cpp
	$(CC)  $(CPPFLAGS) $(CPPFLAGSOPT) -c $^ -o $@

clean:
	rm -f *.o
	rm -f sheet

all: sheet

new: clean all
