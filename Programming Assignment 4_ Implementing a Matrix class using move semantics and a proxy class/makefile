# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target binary name
TARGET = matrix_proxy_test

# Objects
OBJS = matrix-proxy-driver.o

.PHONY: all clean

all: $(TARGET)

# Linking phase
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compilation phase
matrix-proxy-driver.o: matrix-proxy-driver.cpp matrix-proxy.hpp
	$(CXX) $(CXXFLAGS) -c matrix-proxy-driver.cpp

clean:
	rm -f $(TARGET) $(OBJS) out*

test: $(TARGET)
	./$(TARGET) 0 > my_out0
	./$(TARGET) 1 > my_out1
	./$(TARGET) 2 > my_out2
	./$(TARGET) 3 > my_out3
	./$(TARGET) > my_out-all.txt
