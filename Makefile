#makefile
CC= g++

CXXFLAGES = -std=c++0x

USE_BOOST = -lboost_system -lboost_program_options -lboost_thread 

all:
	make generators algorithms

pagerank: algorithms/pagerank.cpp
	$(CC) algorithms/pagerank.cpp $(CXXFLAGS) -o pagerank

clean:
	rm algorithms/*.exe
