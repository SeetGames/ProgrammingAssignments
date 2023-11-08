# Define the compiler
CXX = g++

# Define any compile-time flags
CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++17

# Define any directories containing header files
INCLUDES =

# Define source files
SOURCES = MemoryManager.cpp test1.cpp

# Define object files
OBJECTS = $(SOURCES:.cpp=.o)

# Define the executable file
MAIN = test1

.PHONY: depend clean

all:    $(MAIN)
	@echo Simple memory manager has been compiled.

$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJECTS)

# This is a suffix replacement rule for building .o's from .cpp's
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
