/**
 * @file test.cpp
 * @author Chek
 * @date 20 Aug 2023
 * @brief Test file for the project. Adapted from Matthew Mead's OA Assignment
 * in DigiPen's CS280 class.
 */

#include "SimpleAllocator.h"
#include "prng.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::printf;
using Utils::randInt;

int SHOW_REAL_ADDRESSES = 1;
int SHOW_EXCEPTIONS = 1;

/**
 * Struct for sample student data
 * - note that we use `long long` so that the size is the same on 
 *   32-bit and 64-bit systems
 */
struct Student {
  int age;
  float gpa;
  long long year;
  long long id;
};

/**
 * Struct for sample employee data
 */
struct Employee {
  Employee *next;
  char lastName[12];
  char firstName[12];
  float salary;
  int years;
};

/**
 * Struct for sample person data
 */
struct Person {
  char lastName[12];
  char firstName[12];
  float salary;
  int years;
};


/**
 * Array of sample person data
 */
struct Person PEOPLE[] = {
    {"Smith", "John", 75000, 8},
    {"Johnson", "Emily", 85000, 10},
    {"Williams", "James", 90000, 12},
    {"Jones", "Olivia", 60000, 6},
    {"Brown", "Liam", 55000, 4},
    {"Davis", "Emma", 80000, 11},
    {"Miller", "Noah", 40000, 3},
    {"Wilson", "Ava", 95000, 9},
    {"Moore", "Sophia", 70000, 7},
    {"Anderson", "Isabella", 85000, 5},
    {"Thomas", "Lucas", 30000, 2},
    {"Jackson", "Mia", 55000, 6},
    {"White", "Aiden", 90000, 11},
    {"Harris", "Harper", 50000, 4},
    {"Martin", "Ethan", 65000, 8},
    {"Thompson", "Luna", 75000, 9},
    {"Garcia", "Liam", 45000, 5},
    {"Martinez", "Sophia", 70000, 10},
    {"Robinson", "Oliver", 55000, 7},
    {"Clark", "Amelia", 85000, 12},
    {"Allen", "Liam", 50000, 4},
    {"Young", "Ava", 80000, 9},
    {"Lee", "Emma", 60000, 6}
};


// an enum for the type of object to test
enum TestObjectType { STUDENT_TYPE, EMPLOYEE_TYPE };

// function prototypes
void dumpCallback(const void *block, size_t size);
void validateCallback(const void *block, size_t size);
void dumpPages(const SimpleAllocator *allocator, unsigned width);
void dumpPagesWithExtHdrs(const SimpleAllocator *allocator, unsigned width);
void dumpExternalHeaders(const SimpleAllocator *allocator,
                         const unsigned char *p);
void checkAndDumpLeaks(const SimpleAllocator *allocator);
void fillEmployee(Employee &e);
void printStats(const SimpleAllocator *allocator);
void printConfig(const SimpleAllocator *allocator);

SimpleAllocator *studentAllocator;
SimpleAllocator *employeeAllocator;

/**
 * Fills an employee with sample data from PEOPLE array
 * @param e employee to fill
 */
void fillEmployee(Employee &e) {
  static unsigned int index = 0;
  strcpy(e.lastName, PEOPLE[index].lastName);
  strcpy(e.firstName, PEOPLE[index].firstName);
  e.salary = PEOPLE[index].salary;
  e.years = PEOPLE[index].years;
  index = (index + 1) % (sizeof(PEOPLE) / sizeof(Person));
}

/**
 * A utility function to print the sizes of all primitives in your system
 */
void printSizes() {
  cout << "sizeof(char) = " << sizeof(char) << endl;
  cout << "sizeof(short) = " << sizeof(short) << endl;
  cout << "sizeof(int) = " << sizeof(int) << endl;
  cout << "sizeof(long) = " << sizeof(long) << endl;
  cout << "sizeof(long long) = " << sizeof(long long) << endl;
  cout << "sizeof(float) = " << sizeof(float) << endl;
  cout << "sizeof(double) = " << sizeof(double) << endl;
  cout << "sizeof(long double) = " << sizeof(long double) << endl;
  cout << "sizeof(void *) = " << sizeof(void *) << endl;
  cout << "sizeof(Student) = " << sizeof(Student) << endl;
  cout << "sizeof(Employee) = " << sizeof(Employee) << endl;
  cout << "sizeof(Person) = " << sizeof(Person) << endl;
  cout << "sizeof(MemBlockInfo) = " << sizeof(MemBlockInfo) << endl;
  cout << "sizeof(Node) = " << sizeof(Node) << endl;
  cout << "sizeof(SimpleAllocatorStats) = " << sizeof(SimpleAllocatorStats)
       << endl;
  cout << "sizeof(SimpleAllocatorConfig) = " << sizeof(SimpleAllocatorConfig)
       << endl;
  cout << "sizeof(SimpleAllocator) = " << sizeof(SimpleAllocator) << endl;
  cout << endl;
}

/**
 * Callback function for dumping memory
 * @param block pointer to the block of memory
 * @param size size of the block of memory
 * TODO: this is for future reference
 */
void dumpCallback(const void *block, size_t size) {
  // do nothing if block is NULL
  if (!block)
    return;

  // limit to 16 bytes
  if (size > 16)
    size = 16;

  // print information about the block
  if (SHOW_REAL_ADDRESSES)
    printf("Block at 0x%p, %u bytes long.\n", block,
           static_cast<unsigned>(size));
  else
    printf("Block at 0x00000000, %u bytes long.\n",
           static_cast<unsigned>(size));

  // print out the bytes in the block
  //   note: the const_cast is necessary because the block pointer is a
  //   const void pointer, and we need to cast it to an unsigned char
  //   pointer so that we can increment it in the loop (void pointers can't be
  //   incremented)
  unsigned char *data =
      const_cast<unsigned char *>(static_cast<const unsigned char *>(block));
  printf(" Data: <");                       // start of data bytes
  for (unsigned int i = 0; i < size; i++) { // print each byte of data
    unsigned char c = *data++;              // get the next byte of data
    if (c > 31 && c < 128) // if it's a printable character within range
      printf("%c", c);     // print the character
    else
      printf(" "); // otherwise print a space
  }
  printf(">"); // end of data bytes

  // reset data pointer and print out the bytes in hex
  //   note: the printf format specifier %02X prints the integer value as
  //   a hexadecimal number with at least 2 digits, padding with 0 if
  //   necessary
  data = const_cast<unsigned char *>(static_cast<const unsigned char *>(block));
  for (unsigned int i = 0; i < size; i++) // print each byte of data
    printf(" %02X",
           static_cast<int>(*data++)); // print the byte as a hex number
  printf("\n");
}

/**
 * Check if there are leaks and dump them
 * @param allocator allocator to check
 * TODO: this is for future reference
 */
//void checkAndDumpLeaks(const SimpleAllocator *allocator) {
  //if (allocator->getStats().objectsInUse) {
    //printf("Memory leaks detected!\n");
    //printf("Dumping objects ->\n");
    //unsigned leaks = allocator->dumpMemoryInUse(dumpCallback);
    //printf("Total leaks: [%u]\n", leaks);
  //} else {
    //printf("No memory leaks detected.\n");
  //}
//}

/**
 * Callback function to print out the address and size of a block of memory
 * @param block pointer to the block of memory
 * @param size size of the block of memory
 */
void validateCallback(const void *block, size_t size) {
  // do nothing if block is NULL
  if (!block)
    return;

  // print information about the block
  if (SHOW_REAL_ADDRESSES)
    printf("Block at 0x%p, %u bytes long.\n", block,
           static_cast<unsigned>(size));
  else
    printf("Block at 0x00000000, %u bytes long.\n",
           static_cast<unsigned>(size));
}

/**
 * Simple swap function
 * @param a first value
 * @param b second value
 */
template <typename T> void swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

/**
 * Shuffle an array
 * @param array array to shuffle
 * @param count number of elements in the array
 */
template <typename T> void shuffle(T *array, unsigned count) {
  for (unsigned int i = 0; i < count; i++) {
    int r = randInt(i, static_cast<int>(count) - 1);
    swap(array[i], array[r]);
  }
}

/**
 * This creates an allocator with the given parameters
 * 1. create a SimpleAllocatorConfig with the given parameters
 * 2. create a SimpleAllocator with the config
 * 3. return the SimpleAllocator
 * IMPORTANT NOTE: The responsibility of deleting the SimpleAllocator is on the
 * caller of this function.
 *
 * TODO if students know about smart ptrs, we should do it in 
 * a safe manner return a unique_ptr<SimpleAllocator> instead of a
 * SimpleAllocator*
 *
 * @param useCPPMemMgr true to use the C++ memory manager
 * @param objectsPerPage number of objects per page
 * @param maxPages maximum number of pages
 * @param headerType the type of header to use
 * @param alignment alignment to use
 * @param padbytes padding bytes to use
 * @param debug true to enable debug mode
 * @param objectType type of object to test
 */
SimpleAllocator* createAllocator(
        bool useCPPMemMgr = false,
        unsigned objectsPerPage = 4,
        unsigned maxPages = 2,
        SimpleAllocatorConfig::HeaderType headerType =
        SimpleAllocatorConfig::NO_HEADER,
        unsigned alignment = 0, 
        unsigned padbytes = 0,
        bool debug = true,
        bool objectType = TestObjectType::STUDENT_TYPE) {
  // configure the allocator
  SimpleAllocatorConfig config(
      useCPPMemMgr, objectsPerPage, maxPages,
      SimpleAllocatorConfig::HeaderBlockInfo(headerType), 
      alignment, padbytes, debug);

  // create the allocator
  if (objectType == TestObjectType::EMPLOYEE_TYPE)
    return new SimpleAllocator(sizeof(Employee), config);
  else
    return new SimpleAllocator(sizeof(Student), config);
}

/**
 * Basic allocation+free test of the allocator
 * 1. allocate a bunch of memory
 * 2. free all the memory
 * 3. see if any exceptions are thrown
 *
 * @param allocator an existing allocator to use
 * @param numObjsToAllocate number of objects to allocate
 * @param numObjsToFree number of objects to free
 * @param maskExternalHeaders true to mask the external headers
 *       (to cater for dumping external header addresses)
 */
void allocFreeTest( SimpleAllocator* allocator, unsigned numObjsToAllocate = 0,
        unsigned numObjsToFree = 0, bool maskExternalHeaders = false) {
  try {
    // print a title of the test
    cout << "Running allocFreeTest with: " << endl;

    // print the config
    printConfig(allocator);
    cout << endl;

    // init a ptrs array
    void *ptrs[numObjsToAllocate];

    // allocate memory into the ptrs array
    for (unsigned i = 0; i < numObjsToAllocate; i++) {
        // DEBUG
        // cout << "BEFORE allocating i=" << i << endl;

        // allocate memory into the ith element of the ptrs array
        // - pass a label to the allocator in case this is EXTERNAL_HEADER
        //   TODO make this a config option
        // - for the label, we first generate a random id using PRNG
        // - also concat the label by adding the prefix "allocFreeTeststu"
        // - convert the label to a c-string using std::to_string
        // - finally, pass the c-string to the allocator
        ptrs[i] = allocator->allocate(
                ("stu" + std::to_string(randInt(10000, 99999)))
                .c_str());

        // this simply allocates without label in the ext header
        //ptrs[i] = allocator->allocate(i);

        // DEBUG
        // cout << "AFTER allocating i=" << i << endl;
        // cout << endl;

        // printCounts(allocator);
    }

    // print the stats and dump the pages
    cout << "After " << numObjsToAllocate << " allocations..." << endl;
    printStats(allocator);
    if (maskExternalHeaders)
        dumpPagesWithExtHdrs(allocator, 32);
    else
        dumpPages(allocator, 32);

    // shuffle the ptrs array and free the memory
    shuffle(ptrs, numObjsToAllocate);
    for (unsigned i = 0; i < numObjsToFree; i++) {
      allocator->free(ptrs[i]);
    }

    // print the stats and dump the pages
    cout << "After " << numObjsToFree << " frees..." << endl;
    printStats(allocator);
    if (maskExternalHeaders)
        dumpPagesWithExtHdrs(allocator, 32);
    else
      dumpPages(allocator, 32);

    // catch and act on our custom exceptions
  } catch (const SimpleAllocatorException &e) {
    if (SHOW_EXCEPTIONS)
      cout << e.what() << endl;
    else
      cout << "Exception thrown during test." << endl;
    return;
  }
}

/**
 * Test the allocator with multiple allocations and frees.
 * 1. call allocFreeTest to allocate and free some memory
 * 2. call allocFreeTest again to allocate and free some memory
 * 3. call allocFreeTest again to allocate and free some memory
 * 4. see if any exceptions are thrown
 *
 * @param allocator an existing allocator to use
 * @param maskExternalHeaders true to mask the external headers
 *        (to cater for dumping external header addresses)
 */
void multiAllocFreeTest( SimpleAllocator* allocator, bool maskExternalHeaders = false) {
  try {
    // print a title of the test
    cout << "Running multiAllocFreeTest..." << endl;
    cout << endl;

    // num objects to allocate
    auto numObjsToAllocate = 10;
    auto numObjsToFree = 5;

    // print the config
    // this is already printed in allocFreeTest
    //printConfig(allocator);
    //cout << endl;

    // first allocation and free
    numObjsToAllocate = 10;
    numObjsToFree = 5;
    allocFreeTest(allocator, numObjsToAllocate, numObjsToFree, maskExternalHeaders);
  
    // second allocation and free
    numObjsToAllocate = 5;
    numObjsToFree = 3;
    allocFreeTest(allocator, numObjsToAllocate, numObjsToFree, maskExternalHeaders);

    // third allocation and free
    numObjsToAllocate = 5;
    numObjsToFree = 5;
    allocFreeTest(allocator, numObjsToAllocate, numObjsToFree, maskExternalHeaders);

    // catch and act on our custom exceptions
  } catch (const SimpleAllocatorException &e) {
    if (SHOW_EXCEPTIONS)
      cout << e.what() << endl;
    else
      cout << "Exception thrown during test." << endl;
    return;
  }
}

/**
 * Similar to multiAllocFreeTest, this test will use allocFreeTest to allocate
 * and free some memory. However, it will also fabricate some memory corruption
 * in the padding bytes bytes.
 * 
 * @param allocator an existing allocator to use
 * @param numObjsToAllocate number of objects to allocate
 * @param numObjsToFree number of objects to free
 */
void corruptionTest(SimpleAllocator* allocator) {
  try {
    // print a title of the test
    cout << "Running corruptionTest with: " << endl;

    // print the config
    printConfig(allocator);
    cout << endl;

    // num objects to allocate
    auto numObjsToAllocate = 8;

    // init a ptrs array
    void *ptrs[numObjsToAllocate];
    
    // allocate memory into the ptrs array
    for (auto i = 0; i < numObjsToAllocate; i++) {
      ptrs[i] = allocator->allocate();
    }

    // print the stats and dump the pages
    cout << "After " << numObjsToAllocate << " allocations..." << endl;
    printStats(allocator);
    dumpPages(allocator, 32);

    // fabricate some memory corruption in left pad bytes of a block
    // 1. get the first byte of the first object
    // 2. subtract the pad bytes from the address
    // 3. set some pad bytes to 0xFF (no such signature in SimpleAllocator)
    auto padBytes = allocator->getConfig().padBytesSize;
    auto pByte = static_cast<unsigned char*>(ptrs[0]) - padBytes;
    auto bytesToCorrupt = (padBytes > 3) ? 3 : padBytes;
    for (unsigned i = 0; i < bytesToCorrupt; i++) {
      *pByte++ = 0xFF;
    }

    // TODO: make an input arg to select whether to corrupt left or right
    // fabricate another memory corruption in the right pad bytes of a block
    // 1. get the first byte of the second object
    // 2. add the object size to the address
    // 3. set some pad bytes to 0xEE (the alignment signature)
    auto objSize = allocator->getStats().objectSize;
    pByte = static_cast<unsigned char*>(ptrs[1]) + objSize;
    for (auto i = bytesToCorrupt; i < padBytes ; i++) {
      *pByte++ = 0xEE;
    }
    
    // print the stats and dump the pages
    cout << "After some corruption..." << endl;
    printStats(allocator);
    dumpPages(allocator, 32);

    // now try to free the corrupted blocks
    // 1. free the first block
    // 2. free the second block
    // 3. let the exceptions show themselves
    allocator->free(ptrs[0]);
    allocator->free(ptrs[1]);

    // print stats and dump the pages if no exception from above
    // - the expected behavior is that the above will throw an exception
    cout << "After " << "2" << " frees..." << endl;
    printStats(allocator);
    dumpPages(allocator, 32);

    // catch and act on our custom exceptions
  } catch (const SimpleAllocatorException &e) {
    if (SHOW_EXCEPTIONS)
      cout << e.what() << endl;
    else
      cout << "Exception thrown during test." << endl;
    return;
  }
}

/**
 * Print stats about the allocator
 * @param allocator allocator to print stats about
 */
void printStats(const SimpleAllocator *allocator) {
  SimpleAllocatorStats stats = allocator->getStats();
  cout << "pagesInUse: " << stats.pagesInUse;
  cout << ", objectsInUse: " << stats.objectsInUse;
  cout << ", freeObjects: " << stats.freeObjects;
  cout << ", allocations: " << stats.allocations;
  cout << ", frees: " << stats.deallocations << endl;
  cout << endl;
}

/**
 * Print the configuration of the allocator
 * @param allocator allocator to print the configuration of
 */
void printConfig(const SimpleAllocator *allocator) {
  cout << "objectSize:" << allocator->getStats().objectSize;
  cout << ", pageSize:" << allocator->getStats().pageSize;
  cout << ", padBytes:" << allocator->getConfig().padBytesSize;
  cout << ", objectsPerPage:" << allocator->getConfig().objectsPerPage;
  cout << ", maxPages:" << allocator->getConfig().maxPages;
  cout << ", maxObjects:"
       << allocator->getConfig().objectsPerPage *
              allocator->getConfig().maxPages;
  cout << endl;

  cout << "alignment:" << allocator->getConfig().alignmentBoundary;
  cout << ", leftAlign:" << allocator->getConfig().leftAlignBytesSize;
  cout << ", interAlign:" << allocator->getConfig().interAlignBytesSize;
  cout << ", headerType:";
  if (allocator->getConfig().headerBlockInfo.type ==
      SimpleAllocatorConfig::NO_HEADER)
    cout << "NONE";
  else if (allocator->getConfig().headerBlockInfo.type ==
           SimpleAllocatorConfig::BASIC_HEADER)
    cout << "BASIC";
  else if (allocator->getConfig().headerBlockInfo.type ==
           SimpleAllocatorConfig::EXTENDED_HEADER)
    cout << "EXTENDED";
  else if (allocator->getConfig().headerBlockInfo.type ==
           SimpleAllocatorConfig::EXTERNAL_HEADER)
    cout << "EXTERNAL";
  cout << ", headerSize = " << allocator->getConfig().headerBlockInfo.size;
  cout << endl;
}

/**
 * Dump the pages of the allocator
 * @param allocator allocator to dump the pages of
 * @param width number of columns to print
 */
void dumpPages(const SimpleAllocator *allocator, unsigned width) {
  // get the pageList head
  const unsigned char *pagePtr =
      static_cast<const unsigned char *>(allocator->getPageList());
  const unsigned char *realpagePtr = pagePtr;
  size_t header_size = allocator->getConfig().headerBlockInfo.size;

  // loop through the pageList
  while (pagePtr) {
    unsigned count = 0;

    // print the page pointer address
    if (SHOW_REAL_ADDRESSES)
      printf("%p\n", pagePtr);
    else
      printf("XXXXXXXX\n");

    // print column header
    for (unsigned j = 0; j < width; j++)
      printf(" %2i", j);
    printf("\n");

    // "Next page" pointer in the page
    if (SHOW_REAL_ADDRESSES) {
      for (unsigned i = 0; i < sizeof(void *); i++, count++)
        printf(" %02X", *pagePtr++);
    } else {
      for (unsigned j = 0; j < sizeof(void *); pagePtr++, count++, j++)
        printf(" %s", "XX");
    }

    // Left leading alignment bytes
    if (allocator->getConfig().alignmentBoundary > 1) {
      // leading alignment block (if any)
      for (unsigned j = 0; j < allocator->getConfig().leftAlignBytesSize;
           count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pagePtr++);
      }
    }

    // Dump each object and its associated info
    for (unsigned int i = 0; i < allocator->getConfig().objectsPerPage; i++) {
      // inter-block alignment (not on first block)
      if (i > 0) {
        for (unsigned j = 0; j < allocator->getConfig().interAlignBytesSize;
             count++, j++) {
          if (count >= width) {
            printf("\n");
            count = 0;
          }
          printf(" %02X", *pagePtr++);
        }
      }

      // header block bytes
      for (unsigned j = 0; j < header_size; count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pagePtr++);
      }

      // left padding
      for (unsigned j = 0; j < allocator->getConfig().padBytesSize; count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }

        printf(" %02X", *pagePtr++);
      }

      // possible next pointer (zero it out)
      for (unsigned j = 0; j < sizeof(void *); count++, pagePtr++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        if (SHOW_REAL_ADDRESSES)
          printf(" %02X", *pagePtr);
        else
          printf(" %s", "XX");
      }

      // remaining bytes
      for (unsigned j = 0;
           j < allocator->getStats().objectSize - sizeof(void *);
           count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pagePtr++);
      }

      // right pad bytes
      for (unsigned j = 0; j < allocator->getConfig().padBytesSize; count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pagePtr++);
      }
    }
    printf("\n\n");

    // get the next page
    pagePtr = reinterpret_cast<const unsigned char *>(
        (reinterpret_cast<const Node *>(realpagePtr))->pNext);
    realpagePtr = pagePtr;
  }
}

/**
 * Dump the external headers of the allocator
 * @param allocator allocator to dump the external headers of
 * @param p pointer to the page to dump the external headers of
 */
void dumpExternalHeaders(const SimpleAllocator *allocator,
                         const unsigned char *p) {
  unsigned char *pagePtr = const_cast<unsigned char *>(p);
  unsigned padbytes = allocator->getConfig().padBytesSize;
  size_t header_size = allocator->getConfig().headerBlockInfo.size;
  unsigned leftalign = allocator->getConfig().leftAlignBytesSize;
  unsigned interalign = allocator->getConfig().interAlignBytesSize;
  unsigned count = allocator->getConfig().objectsPerPage;
  size_t offset = padbytes * 2 + header_size + interalign +
                  allocator->getStats().objectSize;

  pagePtr += sizeof(void *); // the 'next' pointer
  pagePtr += leftalign;      // the left alignment bytes (if any)

  // We're now pointing at the beginning of the header block
  MemBlockInfo *pm = reinterpret_cast<MemBlockInfo *>(
      *reinterpret_cast<MemBlockInfo **>(pagePtr));

  cout << endl;
  cout << "Dumping external header (in order of the blocks from left to right)..." << endl;
  cout << "  Label: " << (pm && pm->pLabel ? pm->pLabel : "") << endl;
  cout << " In use: " << (pm ? pm->inUse : 0) << endl;
  cout << "Alloc #: " << (pm && pm->inUse ? pm->allocNum : 0) << endl;

  for (unsigned i = 1; i < count; i++) {
    pagePtr += offset;
    MemBlockInfo *pm = reinterpret_cast<MemBlockInfo *>(
        *reinterpret_cast<MemBlockInfo **>(pagePtr));
    cout << endl;
    cout << "  Label: " << (pm && pm->pLabel ? pm->pLabel : "") << endl;
    cout << " In use: " << (pm ? pm->inUse : 0) << endl;
    cout << "Alloc #: " << (pm && pm->inUse ? pm->allocNum : 0) << endl;
  }
}

/**
 * Dump the pages of the allocator with external headers
 * @param allocator allocator to dump the pages of
 * @param width number of bytes to dump per line
 *
 * TODO refactor repeated code from dumpPages
 */
void dumpPagesWithExtHdrs(const SimpleAllocator *allocator, unsigned width) {
  const unsigned char *pages =
      static_cast<const unsigned char *>(allocator->getPageList());
  const unsigned char *realpage = pages;

  size_t header_size = allocator->getConfig().headerBlockInfo.size;

  while (pages) {
    unsigned count = 0;

    if (SHOW_REAL_ADDRESSES)
      printf("%p\n", pages);
    else
      printf("XXXXXXXX\n");

    // print column header
    for (unsigned j = 0; j < width; j++)
      printf(" %2i", j);
    printf("\n");

    if (SHOW_REAL_ADDRESSES) {
      for (unsigned i = 0; i < sizeof(void *); i++, count++)
        printf(" %02X", *pages++);
    } else {
      for (unsigned j = 0; j < sizeof(void *); pages++, count++, j++)
        printf(" %s", "XX");
    }

    // Left leading alignment bytes
    if (allocator->getConfig().alignmentBoundary > 1) {
      // leading alignment block (if any)
      for (unsigned j = 0; j < allocator->getConfig().leftAlignBytesSize;
           count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pages++);
      }
    }

    for (unsigned int i = 0; i < allocator->getConfig().objectsPerPage; i++) {
      // inter-block alignment (not on first block)
      if (i > 0) {
        for (unsigned j = 0; j < allocator->getConfig().interAlignBytesSize;
             count++, j++) {
          if (count >= width) {
            printf("\n");
            count = 0;
          }
          printf(" %02X", *pages++);
        }
      }

      // header block bytes
      for (unsigned j = 0; j < header_size; count++, j++, pages++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        /*=================================================================
        In external mode, pointer address is embedded into the header memory
        ==================================================================*/
        if (SHOW_REAL_ADDRESSES)
          printf(" %02X", *pages);
        else
          printf(" %s", "XX");
      }

      // left padding
      for (unsigned j = 0; j < allocator->getConfig().padBytesSize; count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pages++);
      }

      // possible next pointer (zero it out)
      for (unsigned j = 0; j < sizeof(void *); count++, pages++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        if (SHOW_REAL_ADDRESSES)
          printf(" %02X", *pages);
        else
          printf(" %s", "XX");
      }

      // remaining bytes
      for (unsigned j = 0;
           j < allocator->getStats().objectSize - sizeof(void *);
           count++, j++) {
        if (count >= width) {
          printf("\n");
          count = 0;
        }
        printf(" %02X", *pages++);
      }

      // right pad bytes
      for (unsigned j = 0; j < allocator->getConfig().padBytesSize; count++, j++) {
        if (count >= width) {
          printf("\n");

          count = 0;
        }
        printf(" %02X", *pages++);
      }
    } // end for
    printf("\n");
    dumpExternalHeaders(allocator, realpage);
    printf("\n");

    pages = reinterpret_cast<const unsigned char *>(
        (reinterpret_cast<const Node *>(realpage))->pNext);
    realpage = pages;
  }
}

/**
 * The main function
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * TODO alignmentTest next time. Yes we do not have a test for alignment
 *      so students do not really need to implement alignment in order 
 *      to pass the current tests. But it is still a recommended feature.
 *      Students are also encouraged to write their own tests.
 */
int main(int argc, char *argv[]) {
  // test number
  int test = 0;

  // check for command line arguments
  if (argc > 1)
    test = atoi(argv[1]);
  if (argc > 2)
    SHOW_REAL_ADDRESSES = atoi(argv[2]);
  if (argc > 3)
    SHOW_EXCEPTIONS = atoi(argv[5]);

  // create var to hold allocator
  SimpleAllocator* allocator = nullptr;

  // run the test
  switch (test) {
  case 0:
    cout << "=== Test minimal allocator with student object type" 
         << " ===" << endl;
    // create the allocator
    allocator = createAllocator(false, 
            2, 
            1, 
            SimpleAllocatorConfig::NO_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator);
    cout << endl;
    break;
  case 1:
    cout << "=== Test small allocator with employee object type" 
         << " with basic headers" 
         << " ===" << endl;
    // create the allocator
    allocator = createAllocator(false, 
            2, 
            1, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::EMPLOYEE_TYPE);

    // run the test
    allocFreeTest(allocator);
    cout << endl;
    break;
  case 2:
    cout << "=== Test allocator" 
         << " with basic headers" 
         << " with some allocations" 
         << " ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            6, 
            3, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator, 2, 0);
    cout << endl;
    break;
  case 3:
    cout << "=== Test allocator" 
         << " with basic headers" 
         << " with some allocations" 
         << " and some frees ===" << endl;
    
    // create the allocator
    allocator = createAllocator(false, 
            6, 
            3, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator, 4, 2);
    cout << endl;
    break;
  case 4:
    cout << "=== Test allocator" 
         << " with basic headers" 
         << " with max allocations" 
         << " and max frees ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            6, 
            3, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::EMPLOYEE_TYPE);

    // run the test
    allocFreeTest(allocator, 18, 18);
    cout << endl;
    break;
  case 5:
    cout << "=== Test allocator" 
         << " with excessive allocations" 
         << " ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            2, 
            2, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator, 5, 0);
    cout << endl;
    break;
  case 6:
    cout << "=== Test allocator" 
         << " with excessive frees" 
         << " ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            2, 
            2, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator, 3, 4);
    cout << endl;
    break;
  case 7:
    cout << "=== Test allocator" 
         << " with extended headers" 
         << " with repeated allocations" 
         << " and frees ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            4,  //objperpage
            8, //max page
            SimpleAllocatorConfig::EXTENDED_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    multiAllocFreeTest(allocator);
    cout << endl;
    break;
  case 8:
    cout << "=== Test allocator" 
         << " with some padding" 
         << " ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            4, 
            8, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            3,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    allocFreeTest(allocator, 9, 5);

    cout << endl;
    break;
  case 9:
    cout << "=== Test allocator" 
         << " with some padding" 
         << " with some allocations" 
         << " and some corruption ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            4, 
            8, 
            SimpleAllocatorConfig::BASIC_HEADER, 
            0, 
            5,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    corruptionTest(allocator);

    cout << endl;
    break;
  case 10:
    // TODO: need to mask the ptr address in the header
    cout << "=== Test allocator" 
         << " with external headers" 
         << " with repeated allocations" 
         << " and frees ===" << endl;

    // create the allocator
    allocator = createAllocator(false, 
            4, 
            8, 
            SimpleAllocatorConfig::EXTERNAL_HEADER, 
            0, 
            0,
            true,
            TestObjectType::STUDENT_TYPE);

    // run the test
    multiAllocFreeTest(allocator, true);
    //allocFreeTest(allocator, 3, 2, true);
    cout << endl;
    break;
  default:
    cout << "=== Bogus test number "<< test 
         << ", but here's some interesting info ===" << endl;
    
    // print sizes based on the system architecture
    printSizes();
    
    break;
  }

  // delete the allocator
  delete allocator;

  return 0;
}
