# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Object files
OBJS = uShell.o shellmain3.obj uShell_ref.obj uShell2_ref.obj

# Target executable
TARGET = uShell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

uShell.o: uShell.cpp uShell3.h
	$(CXX) $(CXXFLAGS) -c uShell.cpp

clean:
	rm -f *.o $(TARGET)
