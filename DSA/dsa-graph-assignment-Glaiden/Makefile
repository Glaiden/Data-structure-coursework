# set some vars to make it easier to change the compiler and flags
SOURCES = test.cpp ALGraph.cpp prng.cpp
FLAGS = -std=c++17 -Wall

# compile: compile the program (the default target)
# g++: use the g++ compiler
# -o out: output the executable to a file called out
# -std=c++17: use the C++17 standard
# -Wall: enable all warnings
compile:
	echo "Compiling..."
	g++ -o out $(SOURCES) $(FLAGS)

# test%-nocompare: compile and run test <test-number> without comparing 
#                  with expected-output
# - the 1st arg is the test number
# - create a target with a dynamic name based on <test-number> fetched into $*
# - redirect the output to a file called output1.txt, output2.txt, etc.
test%-nocompare: compile
	echo "Running test$* with no comparison without expected-output..."
	@./out $* > output$*.txt

# test%-dot: compile and run test <test-number> without comparing with
#            expected-output and also output the png file
# - the 1st arg is the test number
# - create a target with a dynamic name based on <test-number> fetched into $*
# - redirect the output to a file called output1.txt, output2.txt, etc.
# - use dot (Graphviz) to generate the png file. Only generate if the dot file exists
test%-dot: compile
	@echo "Running test$* with no comparison without expected-output and generating png..."
	@./out $* > output$*.txt
	@if [ -f graph$*.dot ]; then \
		dot -Tpng graph$*.dot -o graph$*.png; \
	fi

# test%: compile and run test <test-number> to compare with expected-output
# - the 1st arg is the test number
# - use a pattern rule to  only register the % behind test if it is a number
test%: compile
	@if [ "$$#" -gt 1 ]; then \
		echo "Error: Too many arguments. Usage: make test<test-number>"; \
	elif echo "$*" | grep '^[0-9][0-9]*$$' > /dev/null; then \
		echo "Running test$*..."; \
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
	@rm -f *-app *.o *.obj out output*.txt *.dot *.png
