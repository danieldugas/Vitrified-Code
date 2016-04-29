all: integral_seq integral_mt

integral_seq: integral_seq.cpp
	g++ -Wall -O3 --std=c++11 -o integral_seq integral_seq.cpp  


integral_mt: integral_mt.cpp
	g++ -Wall -O3 --std=c++11 -pthread -o integral_mt integral_mt.cpp  


clean:
	rm -f integral_seq integral_mt
