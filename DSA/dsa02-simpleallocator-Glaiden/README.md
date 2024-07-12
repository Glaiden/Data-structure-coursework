[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/1wlMwjs0)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=11724276)
# simple-allocator assignment

This assignment gives you a chance to implement a simple memory allocator. The main purpose is to form a deep appreciation for the data structure you will work with that is closest to the metal when you're developing software, i.e., the memory. At the same time, it follows DSA tradition in doing a linked list exercise as the first assignment :smile:.

Most of the work involves creating code in a new [SimpleAllocator.cpp](SimpleAllocator.cpp) file, that conforms to the SimpleAllocator class provided in the [SimpleAllocator.h](SimpleAllocator.h) file.

The [test.cpp](test.cpp) file contains the tests that your implementation will be tested against. Your goal is to make all the tests pass.

You are advised to approach the assignment in the following manner:
1. Read the SimpleAllocator.h file to understand the interface of the SimpleAllocator class.
2. Read the test.cpp file to understand what the tests are doing.
3. Implement the SimpleAllocator class in the SimpleAllocator.h and (newly created) SimpleAllocator.cpp files in order to pass **test0**.
4. Compile and test with test0.
5. If "SUCCESS" in 4. then proceed to 3. with test1, test2, ..., else repeat 3 with test0.
6. You're done once you reach test10.

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

Once you got your hands a bit dirty by attempting to run the tests and seeing all the epic failures, it is time to take a step back and understand the task at hand here. The remaining sections will attempt to help you with just that.

# SimpleAllocator

The main functionality of the SimpleAllocator is to allocate and free memory blocks.

When first constructed, the SimpleAllocator will allocate a single page of contiguous memory and "break-up" the memory into a linked list of blocks, each potentially having additionally memory to store auxiliary data for memory management. 

<!--When the SimpleAllocator is destroyed, it will free all the memory it has allocated. In this "simple" allocator exercise, we will not deal with features that enable the client to clean up memory manually.-->

In general, the memory layout will typically have the following structure:

```
ptr-next-pg | left-align | header | pad | block | pad | inter-align | header | pad | block | pad | ...
```

For example, if we have 6 blocks in a page, the layout is as below (follow the blocks from left to right, top to bottom):
(thanks Copilot for the ASCII art)

```
+----------------+----------------+----------------+----------------+----------------+----------------+
| ptr-next-pg    | left-align     | header         | pad            | block          | pad            |
+----------------+----------------+----------------+----------------+----------------+----------------+
| inter-align    | header         | pad            | block          | pad            | inter-align    |
+----------------+----------------+----------------+----------------+----------------+----------------+
| header         | pad            | block          | pad            | inter-align    | header         |
+----------------+----------------+----------------+----------------+----------------+----------------+
| pad            | block          | pad            | inter-align    | header         | pad            |
+----------------+----------------+----------------+----------------+----------------+----------------+
| block          | pad            | inter-align    | header         | pad            | block          |
+----------------+----------------+----------------+----------------+----------------+----------------+
| pad            | 
+----------------+
...
```

Note that we are not looking at a linked list here. We are looking at a page of memory in its raw form.

So where are the linked lists? There are basically 2 linked lists that the allocator will manage:
1. The linked list of pages
2. The linked list of free blocks

In the [SimpleAllocator.h](SimpleAllocator.h) file, you can see that there are suggested private variables to hold the head pointers to these lists:

```
    GenericObject* pFreeList_; // Head of internal free list
    GenericObject* pPageList_; // Head of internal page list
```

For the page list, you will need to create a linked list of pages. Each page will contain a pointer to the next page. The last page will have a null pointer. The page list will be used to keep track of all the pages that have been allocated by the allocator.

For the free list, you will need to create a linked list of free blocks. Each free block will contain a pointer to the next free block. The last free block will have a null pointer. The free list will be used to keep track of all the free blocks that are available for allocation.

Let's run through an example and do a step by step walk through of the memory layouts when we:
1. Construct the allocator
2. Allocate 2 blocks one by one
3. Free 1 block

### Constructing the allocator

When the allocator is constructed, it will allocate a page of memory. The size of the page is determined by the parameters passed via a configuration struct. In the [SimpleAllocator.h](SimpleAllocator.h) file, you can see that this SimpleAllocatorConfig struct that has the following parameters:

```
    bool useCPPMemManager; // Use C++ memory manager (operator new) instead of malloc
    unsigned objectsPerPage; // Number of objects per page
    unsigned maxPages; // Maximum number of pages
    HeaderBlockInfo headerBlockInfo; // Header block information
    unsigned alignmentBytesSize; // num bytes in alignment
    unsigned leftAlignBytesSize; // num bytes in left alignment (computed from alignmentBytesSize)
    unsigned interAlignBytesSize; // num bytes in inter alignment (computed from alignmentBytesSize)
    unsigned padBytesSize; // num bytes in padding
    bool isDebug; // True if debug mode is on
```

These are all the parameters you have to determine in order to configure the allocator. To make this exercise fairly representative of the real-world, we have tried to include some of the most common things that need to be configured when creating allocators.

To understand what to do with each of these parameters, you will need to:
1. understand the example below
2. scrutinize the expected-output*.txt files in this folder in relation to what a specific test* is testing in [test.cpp](test.cpp).
3. read the comments in the [SimpleAllocator.h](SimpleAllocator.h) file

Let use the following configuration parameters as a running example here:
- objectSize = 8
- objectsPerPage = 4
- maxPages = 2
- headerBlockInfo = {NO_HEADER}
- alignmentBytesSize = 0
- padBytesSize = 2
- isDebug = true


The first step is to use CPP's `new` to allocate a chunk of contiguous memory of pageSize bytes. Actually calculating pageSize is one of the first tasks you will need to do when constructing the allocator. First we calculate the blockSize based on the layout we saw earlier.

```
ptr-next-pg | left-align | header | pad | block | pad | inter-align | header | pad | block | pad | ...
8           | 0          | 0      | 2   | 8     | 2   | 0           | 0      | 2   | 8     | 2   | ...
```

This means the pageSize will be calculated as follows:

```
pageSize = blockSize * objectsPerPage
         = ptr-next-pg + left-align + (header + pad + block + pad) * objectsPerPage + inter-align * (objectsPerPage - 1)
         = 8           + 0          + (0      + 2   + 8     + 2  ) * 2              + 0           * (2              - 1)
         = 32
```

So the first step in construction is to allocate one page of 32 bytes of memory using CPP's `new`, and the initial memory layout will simply be one large block of 32 bytes before we break it up into blocks. The constructor will also initialize the [allocation statistics](#allocator-statistics) with this information.

Then update pPageList_ to point to the first page, and for pPageList->pNext to point to nullptr:

```
pPageList_ --> [pPageList_->pNext] --> nullptr
```

Then we "walk" through the bytes inside the 32 bytes of memory and break it up into blocks with the necessary auxiliary data based on the configuration parameters. For our example configuration, we will have the following memory layout:

```
+----------------+----------------+----------------+----------------+
| ptr-next-pg    | left-align     | header         | pad            |
+----------------+----------------+----------------+----------------+
| block1         | pad            | inter-align    | header         |
+----------------+----------------+----------------+----------------+
| pad            | block2         | pad            | inter-align    |
+----------------+----------------+----------------+----------------+
| header         | pad            | block3         | pad            |
+----------------+----------------+----------------+----------------+
| inter-align    | header         | pad            | block4         |
+----------------+----------------+----------------+----------------+
| pad            | inter-align    | header         | pad            |
+----------------+----------------+----------------+----------------+
```

Within each block*, we will have the following memory layout (not drawn to scale):

```
+----------------+----------------+
| ptr-next-free  |                |
+----------------+----------------+
```

The ptr-next-free pointer is used to point to the next free block in the free list. The last free block will point to nullptr, as with all linked list configurations. As mentioned in class when we look at the contiguous memory from left to right, the ptr-next-free pointers are actually linked in a reverse manner.

So the free list will look like this from left to right:

```
nullptr <-- block1 <-- block2 <-- block3 <-- block4 <-- pFreeList_
```

And there you go, the two linked lists are created. The pFreeList_ pointer points to the head of the free list, and the pPageList_ pointer points to the head of the page list.

Now when a client calls the `allocate()` method, the allocator will simply return the first free block in the free list, and update the free list accordingly (and also update [statistics](#allocator-statistics)). When a client calls the `free()` method, the allocator will simply add the block to the head of the free list.

## Allocating memory

The `allocate()` method is at the heart of the allocator. This is where you will need to allocate memory. The method signature is as follows:

```
    void* allocate(const char* pLabel = 0); // pLabel is only needed for EXTERNAL_HEADER type
```

In our running example, here's a view of the list when we allocate block to the client when it does `p1 = allocator.allocate()`:

```
nullptr <-- block1 <-- block2 <-- block3 <------------- pFreeList_

                                             block4
                                             ^
                                             |
                                             p1
```

Here's a view of the list when we allocate another block to the client when it does `p2 = allocator.allocate()`:

```
nullptr <-- block1 <-- block2 <------------------------ pFreeList_

                                  block3     block4
                                  ^          ^
                                  |          |
                                  p2         p1
```

## Freeing memory

The `free` method is responsible for freeing the block of memory pointed to by the pointer passed in. If the pointer is `nullptr`, it should do nothing.

The `free` method should mark the block of memory pointed to by the pointer passed in as free. The allocation statistics should be updated accordingly.
Now here's what it looks like when the client does `allocator.free(p1)`:

```
nullptr <-- block1 <-- block2 <------------- block4 <-- pFreeList_

                                  block3
                                  ^
                                  |
                                  p2
```

## Memory signatures

You can see that there are a number of memory signatures in the [SimpleAllocator.h](SimpleAllocator.h) file. These are used to check the integrity of the memory and will form the basis of most of the tests. In other words, we perform operations using your allocator and then compare the memory signatures with what is expected to occur.

```
    static const unsigned char UNALLOCATED_PATTERN = 0xAA; // unallocated memory never touched by the client
    static const unsigned char ALLOCATED_PATTERN = 0xBB; // allocated memory owned by the client
    static const unsigned char FREED_PATTERN = 0xCC; // freed memory returned by the client
    static const unsigned char PAD_PATTERN = 0xDD; // pad signature to detect buffer overruns
```

Have a look at the output files that are named expected-output*.txt. Every test dumps (fancy name for printing memory content) the memory into the files so that we can see what is happening after a series of allocations and frees, for example. These files contain the expected memory signatures after the tests have been run. You can use these to help you debug your code.

## Allocator statistics

In the [SimpleAllocator.h](SimpleAllocator.h) file, you can also see that there is a struct that contains the following:

```
    size_t objectSize; // Object size
    size_t pageSize; // Page size
    unsigned freeObjects; // Number of free objects
    unsigned objectsInUse; // Number of objects in use
    unsigned pagesInUse; // Number of pages in use
    unsigned mostObjects; // Most objects in use
    unsigned allocations; // Number of allocations
    unsigned deallocations; // Number of deallocations
```

The variable names speak for themselves. You will need to update these statistics as you allocate and free memory.

# Grading
These assignments are meant to be formative, so the points you receive on passing the tests will not count towards your formal grades. However, the summative assessments (quizzes, practical test, etc.) will be based on these assignments, so it is in your best interest to complete them.

This will take some time, so please start early. You will need to understand the code in the SimpleAllocator.h and the test.cpp test suite in order to understand what is being tested and how to fix the errors. There are loads of comments in the respective files given to help you understand the code in there.
