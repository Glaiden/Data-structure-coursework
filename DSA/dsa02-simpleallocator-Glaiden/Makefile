# set some vars to make it easier to change the compiler and flags
SOURCES = test.cpp SimpleAllocator.cpp prng.cpp
FLAGS = -std=c++17 -Wall

# compile: compile the program (the default target)
# g++: use the g++ compiler
# -o out: output the executable to a file called out
# -std=c++17: use the C++17 standard
# -Wall: enable all warnings
compile:
	echo "Compiling..."
	g++ -o out $(SOURCES) $(FLAGS)

# test%-real: compile and run test <test-number> and show real addresses
# - this target will show real addresses so that you can debug using actual addresses
# - the 1st arg is the test number
# - the 2nd arg (SHOW_REAL_ADDRESSES) by default will be 1
# - create a target with a dynamic name based on <test-number> fetched into $*
# - redirect the output to a file called output1.txt, output2.txt, etc.
test%-real: compile
	echo "Running test$* with real addresses..."
	@./out $* 1 > output$*.txt

# test%: compile and run test <test-number> and show masked addresses
# - use masked addresses to perform the comparisons with expected output
# - so that the tests will pass on different machines
# - the 1st arg is the test number
# - the 2nd arg (SHOW_REAL_ADDRESSES) is set to 0
# - use a pattern rule to  only register the % behind test if it is a number
test%: compile
	@if [ "$$#" -gt 2 ]; then \
		echo "Error: Too many arguments. Usage: make test<test-number>"; \
	elif echo "$*" | grep '^[0-9][0-9]*$$' > /dev/null; then \
		echo "Running test$* with masked addresses..."; \
        ./out $* 0 > output$*.txt; \
        bash ./compare.sh $*; \
    else \
        echo "Skipping target $@ because it's not a number."; \
    fi

# debug: compile and run the program with valgrind
debug: compile
	@valgrind -q --leak-check=full --tool=memcheck ./out > output.txt 2>&1 

# all: clean, compile, and test
all: compile test1 test2 test3 test4 test5 test6 test7 test8 test9 test10

# clean: remove all executables and object files
clean:
	@rm -f *-app *.o *.obj out *.txt
