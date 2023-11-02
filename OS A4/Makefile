# Compiler
CXX = g++

# Compilation flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Source files
CORO_SRC = new-coro-lib.cpp

# Header files
CORO_HDR = new-coro-lib.h

# Test cases
TESTS = only-one-thread switching-threads-example wait-thread-example double-wait-test \
        return-test1 schedule-test context-saving-test-case fib-threads \
        matrix-multiply-test-case fib-source-sink

# Objects
OBJS = $(addsuffix .o, $(TESTS))

# Executables
EXECS = $(addsuffix .exe, $(TESTS))

.PHONY: all clean

all: $(EXECS)

%.o: %.cpp $(CORO_HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.exe: %.o
	$(CXX) $(CXXFLAGS) $(CORO_SRC) $< -o $@

# Additional target to run all tests and save outputs to text files
run_tests: $(EXECS)
	@for exe in $^; do \
		echo Running $$exe; \
		if [ "$$exe" = "fib-threads.exe" ] || [ "$$exe" = "fib-source-sink.exe" ]; then \
			./$$exe 12 > $$exe.output.txt 2>&1; \
		elif [ "$$exe" = "matrix-multiply-test-case.exe" ]; then \
			./$$exe 4 3 5 4 > $$exe.output.txt 2>&1; \
		else \
			./$$exe > $$exe.output.txt 2>&1; \
		fi \
	done

clean:
	rm -f $(OBJS) $(EXECS)
