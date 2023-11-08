CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread
SRCS = main.cpp dispatcher.cpp worker.cpp request.cpp mm.cpp
HDRS = dispatcher.h worker.h request.h mm.h
OBJS = $(SRCS:.cpp=.o)
MAIN = main

all: $(MAIN)

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)