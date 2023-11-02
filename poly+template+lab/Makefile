PRG=gnu.exe
GCC=g++
GCCFLAGS=-std=c++17 -O2 -pedantic-errors -Wall -Wextra -Werror -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

OBJECTS0=
DRIVER0=driver.cpp
DRIVER1_NO_COMPILE=driver-no-comp-1.cpp
DRIVER2_NO_COMPILE=driver-no-comp-2.cpp
DRIVER3_NO_COMPILE=driver-no-comp-3.cpp

VALGRIND_OPTIONS=-q --leak-check=full

gcc0:
	$(GCC) -o $(PRG) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
gcc1_NC:
	$(GCC) -o $@.exe $(DRIVER1_NO_COMPILE) $(OBJECTS0) $(GCCFLAGS)
gcc2_NC:
	$(GCC) -o $@.exe $(DRIVER2_NO_COMPILE) $(OBJECTS0) $(GCCFLAGS)
gcc3_NC:
	$(GCC) -o $@.exe $(DRIVER3_NO_COMPILE) $(OBJECTS0) $(GCCFLAGS)
0 1 2 3 4 5 6:
	@echo "running test$@"
	@echo "should run in less than 500 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
mem0 mem1 mem2 mem3 mem4 mem5 mem6:
	@echo "running memory test $@"
	@echo "should run in less than 2500 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.tds *.o *.obj *manifest* studentout* diff*
