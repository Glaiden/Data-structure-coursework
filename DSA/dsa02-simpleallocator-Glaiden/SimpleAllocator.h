/**
 * @file SimpleAllocator.h
 * @author Chek
 * @brief SimpleAllocator class definition
 *        Note that this file is lengthy 
 *        as it tries to contain as much information as possible
 *        for students to understand the requirements
 * @date 25 Aug 2023
 */

#ifndef SIMPLEALLOCATOR_H
#define SIMPLEALLOCATOR_H
#include <string>
#include <iostream>

// Defaults for SimpleAllocator construction when client does not specify
static const int DEFAULT_OBJECTS_PER_PAGE = 4;
static const int DEFAULT_MAX_PAGES = 3;

/**
 * @class SimpleAllocatorException
 * @brief this class defines custom exceptions that are thrown by SimpleAllocator
 *        this includes exceptions for things like memory allocation failures
 *        and memory corruption. The specific exception codes are defined in the 
 *        ExceptionCode enum
 */
class SimpleAllocatorException {
public:
    // Exception codes
    enum ExceptionCode {
        E_NO_MEMORY, // No memory available (operator new fails)
        E_NO_PAGE, // No page available (max pages reached)
        E_BAD_BOUNDARY, // block address is on a page but not a block boundary
        E_MULTIPLE_FREE, // block has already been freed
        E_CORRUPTED_BLOCK // block has been corrupted (pad bytes overwritten)
    };

    /**
     * Constructor
     * @param code exception code
     * @param message exception message
     */
    SimpleAllocatorException(ExceptionCode code, const std::string& message) : code_(code), message_(message) {}

    /**
     * Get exception code
     * @return exception code
     */
    ExceptionCode code() const {
        return code_;
    }

    /**
     * Get text message describing exception in raw c_str format
     * @return text message (NUL-terminated)
     */
    const char* what() const {
        return message_.c_str(); 
    }

    private:
        ExceptionCode code_; // Exception code
        std::string message_; // Exception message
};

/**
 * SimpleAllocator configuration parameters struct
 */
struct SimpleAllocatorConfig {
    /**
     * Different header types
     */
    enum HeaderType {
        // no header
        NO_HEADER,

        // basic header, with mem layout:
        // | alloc num    | flag |
        // | unsigned int | bool |
        BASIC_HEADER,

        // extended header, with mem layout:
        // | user-defined | use count        | alloc num      | flag   |
        // | char        | unsigned short  | unsigned int  | bool  |
        EXTENDED_HEADER,

        // external header, with mem layout:
        // | MemBlockInfo*  | (it stores a pointer to a MemBlockInfo struct)
        // | MemBlockInfo** | (so the header ptr is a pointer to a pointer)
        EXTERNAL_HEADER,
    };

    /**
     * Header Block Information
     * - this struct contains information pertaining to different header types
     */
    struct HeaderBlockInfo {
        HeaderType type; // Header type, one of the 4 types
        size_t size; // Header size
        size_t userDefinedSize; // Additional 
        //++++++++++++user-defined bytes (for extended header type)
    
        /**
         * Constructor
         * @param _type header type
         * @param _size header size
         * @param _userDefinedSize size of additional user-defined bytes
         */
        HeaderBlockInfo(HeaderType _type = NO_HEADER, size_t _size = 0, size_t _userDefinedSize = 0) 
            : type(_type), size(_size), userDefinedSize(_userDefinedSize) {
            if (_type == BASIC_HEADER) {
                size = sizeof(unsigned) + 1; // allocation number + free flag
            } else if (type == EXTENDED_HEADER) {
                // set to allocation number + free flag + flag byte + user-defined bytes
                size = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + userDefinedSize;
            } else if (type == EXTERNAL_HEADER) {
                size = sizeof(void*); // pointer to MemBlockInfo
            }
        }

    };

    /**
     * Constructor
     * @param useCPPMemManager use C++ memory manager (operator new) instead of malloc
     * @param objectsPerPage number of objects per page
     * @param maxPages maximum number of pages
     * @param headerBlockInfo header block information
     * @param alignment this refering to the boundary to align to
     * @param padBytes pad bytes
     * @param debug true if debug mode is on
     */
    SimpleAllocatorConfig(
            bool _useCPPMemManager = false,
            unsigned _objectsPerPage = DEFAULT_OBJECTS_PER_PAGE, 
            unsigned _maxPages = DEFAULT_MAX_PAGES, 
            const HeaderBlockInfo& headerBlockInfo = HeaderBlockInfo(), 
            unsigned _alignmentBoundary = 0, 
            unsigned _padBytesSize = 0, 
            bool _isDebug = false) : 
        useCPPMemManager(_useCPPMemManager), 
        objectsPerPage(_objectsPerPage), 
        maxPages(_maxPages), 
        headerBlockInfo(headerBlockInfo), 
        alignmentBoundary(_alignmentBoundary), 
        leftAlignBytesSize(0),
        interAlignBytesSize(0),
        padBytesSize(_padBytesSize), 
        isDebug(_isDebug){}

    bool useCPPMemManager; // Use C++ memory manager (operator new) instead of malloc
    unsigned objectsPerPage; // Number of objects per page
    unsigned maxPages; // Maximum number of pages
    HeaderBlockInfo headerBlockInfo; // Header block information
    unsigned alignmentBoundary; // the boundary to align to
    unsigned leftAlignBytesSize; // num bytes in left alignment (computed from alignmentBoundary)
    unsigned interAlignBytesSize; // num bytes in inter alignment (computed from alignmentBoundary)
    unsigned padBytesSize; // num bytes in padding
    bool isDebug; // True if debug mode is on
};

/**
 * A memory block info for EXTERNAL_HEADERs
 */
struct MemBlockInfo {
    bool inUse; // True if block is in use
    char* pLabel; // dynamically allocated NUL-terminated string (Label)
    unsigned allocNum; // allocation number
};

/**
 * SimpleAllocator statistics struct
 */
struct SimpleAllocatorStats { //track what i allocate and free, update state everytime free or allocated
    /**
     * ConstructorSimpleAllocator::
     * - all params are initialized to 0
     */ 
    SimpleAllocatorStats() : 
        objectSize(0), 
        pageSize(0), 
        freeObjects(0), 
        objectsInUse(0), 
        pagesInUse(0), 
        mostObjects(0), 
        allocations(0), 
        deallocations(0) {}
#include "SimpleAllocator.h"
    size_t objectSize;      // fixed sizeimpleAllocatorStats::SimpleAllocatorStats(objectSize) of each object
    size_t pageSize;        // fixed size of each pagediscrete math precedence list
    unsigned freeObjects;   // current number of free objects
    unsigned objectsInUse; // current number of objects in use
    unsigned pagesInUse; // current number of pages in use
    unsigned mostObjects; // most objects in use over lifetime
    unsigned allocations; // total number of allocations over lifetime
    unsigned deallocations; // total number of deallocations over lifetime
};

/**
 * A generic struct for nodes in any linked list
 * - this is basically an object that can be used to point to any type of object
 * - it does not have any data members
 * - it only knows how to point to the next object
 * - the only structure it has is that the first set of bytes is a pointer to the next object
 * - when you cast something to Node*, you can only access the first set of bytes, 
 *   which is a total of 4 bytes, or 8 bytes, depending on the platform
 * - to work with the actual data that may lie beyond the Node, 
 *   you need to cast it to the appropriate type
 */
struct Node {
    Node* pNext; // Pointer to next object
};

/**
 * The SimpleAllocator class
 */
class SimpleAllocator {
public:
    /**
     * Callback function for dumping memory blocks
     * @param ptr pointer to memory block
     * @param size size of memory block
     * TODO: this is unused, for future consideration
     */
    //typedef void (*DUMPCALLBACK) (const void*, size_t);

    /**
     * Callback function for validating blocks
     * @param ptr pointer to memory block
     * @param size size of memory block
     * TODO: this is unused, for future consideration
     */
    //typedef void (*VALIDATECALLBACK) (const void*, size_t); 

    // some const values for memory signatures
    static const unsigned char UNALLOCATED_PATTERN = 0xAA; // unallocated memory never touched by the client
    static const unsigned char ALLOCATED_PATTERN = 0xBB; // allocated memory owned by the client
    static const unsigned char FREED_PATTERN = 0xCC; // freed memory returned by the client
    static const unsigned char PAD_PATTERN = 0xDD; // pad signature to detect buffer overruns
    static const unsigned char ALIGN_PATTERN = 0xEE; // alignment bytes

    /**
     * Constructor
     * @param objectSize object size
     * @param SimpleAllocatorConfig configuration
     * @throws SimpleAllocatorException if construction fails
     */
    SimpleAllocator(size_t objectSize, const SimpleAllocatorConfig& config);

    /**
     * Destructor
     * (never throws)
     */
    ~SimpleAllocator();

    /**
     * Allocate memory
     * @param label label for memory block (only for EXTERNAL_HEADER)
     * @return pointer to allocated memory
     */
    void* allocate(const char* pLabel = 0); //default 0

    /**
     * Free (deallocate) memory
     * @param obj pointer to object to deallocate
     */
    void free(void* pObj); //

    /**
     * Runs the callback fn on each block of allocated memory
     * @param fn callback function
     * @return number of blocks
     */
    //unsigned dumpMemoryInUse(DUMPCALLBACK fn) const;

    /**
     * Runs the callback fn on each block that is potentially corrupted
     * @param fn callback function
     * @return number of blocks
     * TODO this is unimplemented, for future consideration
     */
    //unsigned dumpCorruptedMemory(DUMPCALLBACK fn) const;

    /**
     * Free all empty pages
     * TODO this is unimplemented, for future consideration
     */
    //void freeEmptyPages();

    /**
     * Set debug state after construction
     * @param debug state to indicate if debug mode is on
     */
    void setDebug(bool _isDebug);

    /**
     * Get ptr to head of internal free list
     * @return ptr to head of internal free list
     */
    const void* getFreeList() const;

    /**
     * Get ptr to head of internal page list
     * @return ptr to head of internal page list
     */
    const void* getPageList() const;

    /**
     * Get the configuration parameters struct
     * @return configuration parameters
     */
    SimpleAllocatorConfig getConfig() const;

    /**
     * Get statistics struct
     * @return statistics
     */
    SimpleAllocatorStats getStats() const;

    void corrupttest(char*block);

     

private:
    // Disable copy constructor and assignment operator
    SimpleAllocator(const SimpleAllocator&) = delete;
    SimpleAllocator& operator=(const SimpleAllocator&) = delete;

    // Private attributes and methods below
    SimpleAllocatorConfig config_; // Configuration parameters
    SimpleAllocatorStats stats_; // Statistics //update constantly
    Node* pFreeList_; // Head of internal free list
    Node* pPageList_; // Head of internal page list
                    
    /**
     * Allocate a new page
     */
    void allocateNewPage();
    //moved allocateNewpage to public
    // The private attributes and methods above are simply examples,
    // feel free to change and add your own private stuff.
};

#endif // SIMPLEALLOCATOR_H
