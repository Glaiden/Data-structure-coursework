[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/X0aiKzZF)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=12100227)
# BST assignment

This assignment is designed to enable you to create a Binary Search Tree (BST). While a BST is a relatively simple tree structure, the experience gained from implementing it as a somewhat complete Abstract Data Type (ADT) container will empower you to create more complex custom trees. For example, you'll be better prepared to work with trees like Octrees used for spatial partitioning or Merkel Trees commonly employed in blockchains.

Furthermore, this assignment will help you become more comfortable with recursion, a highly powerful programming technique. This can be especially beneficial if you haven't had much prior experience with it.

As usual most of the work you need to do should be in a new BST.cpp file, that conforms to the class provided in the [BST.h](BST.h) file.

The [test.cpp](test.cpp) file contains the tests that your implementation will be tested against. Your goal is to make all the tests pass.

You are again advised to approach the assignment in the same test-driven development (TDD) manner as with previous assignments.

Here's the usual next section that tells you how to compile and run the tests. 

# Compilation and Testing

Everything is done via the [Makefile](Makefile) in the terminal. To compile the code, run:

```
make
```

To run the tests, run:

```
make test<test_number>
```

Replace test_number with the correct one you are currently working on. We recommend that you start with test0 and work your way up.
For example, to run the first test, run:

```
make test1
```

To run the tests without comparing to the expected output, for example for test1, run:

```
make test1-nocompare
```

To clean up the compiled files, run:

```
make clean
```

# Description

As usual, the combination of the header file and the test file should give you a good idea of what you need to do to address our requirements. However, here are some pointers that you may find useful.

## Allocator

As a continuation to your previous assignment, the BST class should use the allocator to manage memory for the nodes. A dummy [SimpleAllocator.h](SimpleAllocator.h) is provided in case you do not have a working version. This dummy allocator simply forwards `allocate` and `free` to the CPP `new` and `delete`, i.e., it perpetually treats `UseCPPMemManager = true`.

## Data

The data stored in the BST should be treated like a "key" and cannot contain duplicates. This means that when the client calls `insert` and you detect a duplicate, you should throw `BST_EXCEPTION::E_DUPLICATE`.

## Templated

As we move towards making actually usable ADTs, we need to make our API more generic, hence the use of templates.

The BST class can store any type of data. The test file uses `int` and `char` as the data type, but you should be able to use any type.

Note that the header should include the implementation file, i.e., `#include "BST.cpp"`. This is because the implementation is templated and the compiler needs to see the implementation when compiling the header.

## Subscript Operator

Arguably, the more difficult test to pass will be test9 that tests the subscript `[]` operator.

Firstly note that near the top of the driver file, there is this line:

```c++
#define SHOW_COUNTS
```

This is what turns on/off the counts (total nodes in the tree rooted at the node containing the count) when displaying the tree. By default it is commented out to generate the outputs to be compared. If uncommented, you'll see them. Viewing th counts may help you implement this functionality much easier.

Since the subscript operator (`[]`) that the client calls cannot be recursive in itself, you'll need to call a helper function to do the recursion:

```c++
getNode_(node, index);
```

where node is the node (initially the root) and index is the position in the tree.

Suppose we're looking for the i'th node and we're at some node (the root initially) and assume that L is the number of nodes in the left subtree. This will be roughly what the `getNode_(node, i)` algorithm looks like.

- If node is nullptr, return nullptr. (Base case)
-	If the left subtree has more than i nodes (L > i), then it's in the left subtree.
    -	Recursively call the method with the left node and the same index, i.e, `getNode_(left, i)`.
- If the left subtree has less than i nodes (L < i), then it's in the right subtree.
    - Recursively call the method with the right node and i - L - 1, i.e., `getNode_(left, i-L-1)`.
-	Else the left subtree has exactly i nodes so i refers to the current node.

Looking at the example below, the values represent: Key[count]. Key is the value in the node and count is the number of nodes in the (sub)tree. Because we inserted Keys from 0 to 9, the Key is the index as well. This makes following the example below easy. On non-sequential data (random, strings, etc.) this will not necessarily be the case.

```
                          5[10]

           2[5]                          8[4]

 0[2]           3[2]           6[2]           9[1]

      1[1]           4[1]           7[1]
```

Suppose we're looking for tree[3] (L is the number of nodes in the left subtree, i is the index):
- Starting at the root:
- L > i so it's in the left subtree. (Recurse left with node 2[5] and i, which is 3)
- Now, L < i, so it's in the right subtree. (Recurse right with node 3[2] and i - L - 1, which is 0)
- Now, L == i, so the value is in the current node, 3[2].
- Return the node.

Suppose we're looking for tree[8] (L is the number of nodes in the left subtree, i is the index):
- Starting at the root:
- L < i so it's in the right subtree. (Recurse right with node 8[4] and i - L - 1, which is 2)
- Now, L == i, so it's in the current node, 8[4].
- Return the node.

Suppose we're looking for tree[7] (L is the number of nodes in the left subtree, i is the index):
- Starting at the root:
- L < i so it's in the right subtree. (Recurse right with node 8[4] and i - L - 1, which is 1.)
- Now, L > i, so it's in the left subtree. (Recurse left with node 6[2] and i, which is 1.)
- Now, L < i, so it's in the right subtree. (Recurse right with node 7[1] and i - L - 1, which is 0)
- Now, L == i, so the value is in the current node, 7[1].
- Return the node.

Try a couple more on your own to convince yourself that this works.

You should also check the value of the index and if it's out of range, return the appropriate `BSTException` as indicated in the test.

## Stress Test

You will find that test10 is rather slow to run. This is because it is a stress test that inserts and removes a large number of nodes. It is left as an exercise for you to figure out how to speed this up.

# Grading
This is the same blurb about how these assignments are meant to be formative, so the points you receive on passing the tests will not count towards your formal grades. However, the summative assessments (quizzes, practical test, etc.) will be based on these assignments, so it is in your best interest to complete them.
