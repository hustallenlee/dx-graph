CC= g++


USE_BOOST= -lboost_system  -lboost_thread -lzmq 
BIN_DIR= ./bin
ALGORITHMS_DIR= ./algorithms
MACRO1= -DCOMPACT_GRAPH 
MACRO2= -DWEIGHT_EDGE

DEBUG= -DDEBUG
CXXFLAGS= -std=c++0x 

ALL_TARGETS= pagerank wcc bfs 

all:
	make $(ALL_TARGETS)

pagerank: $(ALGORITHMS_DIR)/pagerank.cpp
	$(CC) $(ALGORITHMS_DIR)/pagerank.cpp $(CXXFLAGS) $(DEBUG) $(MACRO1) $(USE_BOOST) \
		-o $(BIN_DIR)/pagerank

wcc: $(ALGORITHMS_DIR)/wcc.cpp
	$(CC) $(ALGORITHMS_DIR)/wcc.cpp $(CXXFLAGS) $(DEBUG) $(MACRO1) $(USE_BOOST) \
		-o $(BIN_DIR)/wcc

bfs: $(ALGORITHMS_DIR)/bfs.cpp
	$(CC) $(ALGORITHMS_DIR)/bfs.cpp $(CXXFLAGS) $(DEBUG) $(MACRO1) $(USE_BOOST) \
		-o $(BIN_DIR)/bfs

.PHONY : clean
clean:
	rm $(BIN_DIR)/*
