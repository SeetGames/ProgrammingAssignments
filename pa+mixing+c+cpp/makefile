# Compiler settings
CC = gcc
CXX = g++

# Flags for the compilers
CFLAGS = -Wall -c
CXXFLAGS = -Wall -c
LDFLAGS =

# Targets
all: sj1.out sj2.out sj3.out sj4.out

# sj1.out: C executable with driver.c and splitter.c
sj1.out: driver_c.o splitter_c.o
	$(CC) $(LDFLAGS) driver_c.o splitter_c.o -o sj1.out

driver_c.o: driver.c
	$(CC) $(CFLAGS) driver.c -o driver_c.o

splitter_c.o: splitter.c
	$(CC) $(CFLAGS) splitter.c -o splitter_c.o

# sj2.out: C++ executable with driver.cpp and splitter.cpp
sj2.out: driver_cpp.o splitter_cpp.o
	$(CXX) $(LDFLAGS) driver_cpp.o splitter_cpp.o -o sj2.out

driver_cpp.o: driver.cpp
	$(CXX) $(CXXFLAGS) driver.cpp -o driver_cpp.o

splitter_cpp.o: splitter.cpp
	$(CXX) $(CXXFLAGS) splitter.cpp -o splitter_cpp.o

# sj3.out: C++ executable with driver.cpp and splitter.c
sj3.out: driver_cpp.o splitter_c.o
	$(CXX) $(LDFLAGS) driver_cpp.o splitter_c.o -o sj3.out

# sj4.out: C++ executable with driver.cpp and splitter.c compiled with g++
sj4.out: driver_cpp.o splitter_c_gpp.o
	$(CXX) $(LDFLAGS) driver_cpp.o splitter_c_gpp.o -o sj4.out

splitter_c_gpp.o: splitter.c
	$(CXX) $(CXXFLAGS) splitter.c -o splitter_c_gpp.o

# Clean rule
clean:
	rm -f *.o sj1.out sj2.out sj3.out sj4.out
