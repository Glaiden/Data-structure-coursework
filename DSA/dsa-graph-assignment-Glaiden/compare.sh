# compare.sh
# This script is used to compare the outputs of completed tests
# - the expected output is in the file `expected-output.txt`
# - the program output is in the file `output.txt`
#
# Usage: 
# bash ./compare.sh <test-number>
# The above command works in terminals in Linux and Mac OS X
# as well as in Git Bash and other linux emulators in Windows.
# - if you choose to run `./compare.sh` directly as an executable,
#   you will need to run `chmod +x ./compare.sh` first
#
# Author: Chek
# Updated: 24 Aug 2023

# run the program and save the output to output.txt
# - this is already ran in the Makefile
# - this is here in case you want to run it manually
#make test > output.txt

# take in an argument from the command line in the code below   
if [ $# -ne 1 ]; then
    echo "ERROR: usage: bash ./compare.sh <test-number>, e.g., bash ./compare.sh 1"
    exit 1
fi

# fail if expected-output<test-number>.txt does not exist
if [ ! -f expected-output$1.txt ]; then
    echo "ERROR: expected-output$1.txt not found"
    exit 1
fi
#
# fail if output<test-number>.txt does not exist
if [ ! -f output$1.txt ]; then
    echo "ERROR: output$1.txt not found"
    exit 1
fi

# fail if output<test-number>.txt is different from expected-output<test-number>.txt
# /dev/null is a special file that discards all data written to it
# 2>&1 redirects stderr to stdout
diff output$1.txt expected-output$1.txt > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: output$1.txt is different from expected-output$1.txt"
    exit 1
fi

# if we get here, the test passed
echo "SUCCESS: test$1 passed"
echo ""

# exit with success
exit 0
