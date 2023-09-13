PRG=gnu.exe

GCC=g++

GCCFLAGS=-std=c++17 -pedantic-errors -Wall -Wextra -Werror -O3

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

OBJECTS0= tddaa.cpp
DRIVER0=driver.cpp

gcc0:
	$(GCC) -o $(PRG) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) 
0 1:
	echo "running test$@"
	@echo "should run in less than 200 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
mem0 mem1:
	echo "running memory test $@"
	@echo "should run in less than 2000 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.o 
