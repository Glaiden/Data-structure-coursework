[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/ixO3hiiO)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=12689577)
# Practical Test

This practical test is for you to create an AVL Tree.

Just like your assignments, most of the work you need to do should be in a new [AVL.cpp](AVL.cpp) file, that conforms to the class provided in the [AVL.h](AVL.h) file.

As usual, the [test.cpp](test.cpp) file contains the test cases that your implementation will be tested against. It will generate each output<test-number>.txt that will be compared against the respective expected-output<test-number>.txt files provided. Your goal is to pass all tests.

The next section tells you how to compile and run the tests. 

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

To clean up the compiled files, run:

```
make clean
```

The following sections will provide some more details of what you need to do.

# Description

If you have been following the class, you would know that the AVL tree is the balanced version of the binary search tree (BST) that you should have already implemented. 

It is up to you how you want to implement the AVL tree, but we recommend that you inherit the BST you have already implemented as a base, and then override the necessary code to make it balanced. In the end the AVL tree is really a specialization of the BST.

To be absolutely clear, the AVL tree needs to ensure that the height of the left and right subtrees of every node differ by at most 1 at all times. When you make any changes to the tree, you will need to determine whether this balance property is violated, and make the necessary corrections as learnt in class. 

As usual start from test case 0 then work your way down. The first 2 test cases are just to make sure that you have the basic BST operations working. If you have done the DSA-BST assignment, this is almost zero work. The subsequent test cases will test that these operations actually ensure that the resultant tree are AVL trees (i.e., balanced).

# Rotations

It goes without saying that you need to implement the rotation operations as described in class. There are some suggested names based on the operations learnt in class but feel free to change and implement them in your own fashion. We are not testing your code at the level of rotations. 

Note that if you're following the pseudocode shown in class, the rotation method shown is local to a subtree rooted at a specific node. An implementation detail is that you will need to update the parent of the subtree to point to the new root of the subtree after the rotation. This is not shown in the pseudocode but is necessary for the tree to be updated properly. This is shown in the example below:

Suppose you have the following AVL tree:

```
    5
   / \
  3   7
 / \   \
1   4   8
```

Then you insert 9 into the tree, and you get the following tree:

```
    5
   / \
  3   7
 / \   \
1   4   8
         \
          9      
```

If you are using a stack to help trace the path from the root to the node where the insertion is to be done, you will have the following stack:

```
| |
|8|
|7|
|5|
---
```

Then we pop the stack one by one and check the balance factor of each node.
- for tree rooted at 8, the balance factor is +1, so we do nothing
- for tree rooted at 7, the balance factor is +2, so we need to fix it

So the pseudocode shown in class with be called on the subtree rooted at 7:

```
    7
     \
      8
       \
        9
```

Then since this is the RR case, we do a `rotateLeft(tree)` where `tree` is the subtree rooted at 7. This will give us the following tree:

```
    8
   / \
  7   9
```

Finally, note that the original parent of this subtree, which is 5, did not have any information that the root of this subtree changed. So we need to update the `right` child of 5 to point to the new root of the subtree, which is 8. This will give us the following tree:

```
     5
   /   \
  3     8
 / \   / \
1   4 7   9
```

And we're done with the balancing operation for this insertion (note that it is slightly different for deletion).

## Inorder Traversal

The last test case requires you to implement an inorder traversal print of the tree. This is not a difficult task (and in fact very few lines of code) if you have been following the class.

## Memory Allocation

Just like in the BST assignment, a dummy SimpleAllocator is given that will default to C++ new and delete when allocating/deallocating nodes.

To make the tests simple, we will default to using the SimpleAllocator. However, the tests do not test for memory allocation strategies and whether you are using the SimpleAllocator or not. So even if you ignore the `allocator_`, it will not impact your test results. Nevertheless please try to do the right thing as we assume you already have the knowledge to work with custom allocators.

# Grading

The test cases are designed to test your implementation in a variety of ways. You should not assume that the tests are exhaustive, but you should assume that they are representative of the kinds of tests that will be used to grade your work. In other words, there will be more test cases in our grading process.

You can roughly assume that if you pass all the test cases here, you will get 50% of the marks. If we find that you are simply generating an output file that matches the expected outputs without actually implementing the AVL operations properly, you will get the marks appropriately deducted as well.
