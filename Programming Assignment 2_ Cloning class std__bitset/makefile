# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Source and object files
SRC = driver-bitset.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = test_bitset

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp bitset.h bitset.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET)

# Run target
run: $(TARGET)
	./$(TARGET)

# Valgrind target
valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

.PHONY: all clean run valgrind
