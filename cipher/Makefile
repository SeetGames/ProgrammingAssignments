PRG=gnu.exe
GCC=g++
GCCFLAGS=-std=c++17 -O2 -pedantic-errors -Wall -Wextra -Werror

OBJECTS0=cipher.cpp
DRIVER0=driver.cpp

VALGRIND_OPTIONS=-q --leak-check=full

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)

0 1 2 3 4 5 6:
	@echo "running test$@"
	@echo "should run in less than 100 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines printed after diff command are mismatches with master output -- see out$@"
	diff out$@ studentout$@ --strip-trailing-cr
7:
	@echo "running test$@"
	@echo "should run in less than 1500 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines printed after diff command are mismatches with master output -- see out$@"
	diff out$@ studentout$@ --strip-trailing-cr
mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7:
	@echo "running memory test $@"
	@echo "should run in less than 1500 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.tds *.o *.obj *manifest* studentout* diff*
