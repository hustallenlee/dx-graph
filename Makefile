CC= g++


USE_BOOST= -lboost_system  -lboost_thread -lzmq 
BIN_DIR= ./bin
ALGORITHMS_DIR= ./algorithms
CONTROL_DIR= ./control
MACRO1= -DCOMPACT_GRAPH 
MACRO2= -DWEIGHT_EDGE

DEBUG= -DDEBUG -g
CXXFLAGS= -std=c++11 

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

server: $(CONTROL_DIR)/server.cpp
	$(CC) $(CONTROL_DIR)/server.cpp $(CXXFLAGS) $(DEBUG)  $(USE_BOOST) \
		-o $(BIN_DIR)/server

client: $(CONTROL_DIR)/client.cpp
	$(CC) $(CONTROL_DIR)/client.cpp $(CXXFLAGS) $(DEBUG)  $(USE_BOOST) \
		-o $(BIN_DIR)/client


.PHONY : clean
clean:
	rm $(BIN_DIR)/*

