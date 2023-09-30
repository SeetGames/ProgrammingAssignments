# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++11

# Object files
OBJS = uShell.o uShell_ref.obj shellmain2.obj

# Target executable
TARGET = uShell2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

uShell.o: uShell.cpp
	$(CC) $(CFLAGS) -c uShell.cpp

clean:
	rm -f *.o $(TARGET)
