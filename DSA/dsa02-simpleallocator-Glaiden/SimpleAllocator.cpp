#include <string>
#include <iostream>
#include <cstring>
#include "SimpleAllocator.h"
unsigned int allonum = 0;

void SimpleAllocator::corrupttest(char*block)
{
    char *ppad = block - config_.padBytesSize; //first chunk of pads
    char*nextpad = block + stats_.objectSize; //last chunk of pads
    
    for(unsigned int i =0;i<config_.padBytesSize;i++)
    {
        unsigned int temp = ppad[i];
        // printf("has entered loop, %c\n");
        if(temp != 4294967261) //compare current byte to 0XDD pattern
        {
            throw SimpleAllocatorException(SimpleAllocatorException::E_CORRUPTED_BLOCK, "ERROR when checking pad bytes: memory corrupted before block.");
        }
        temp = nextpad[i];
        if(temp != 4294967261)
        {
            throw SimpleAllocatorException(SimpleAllocatorException::E_CORRUPTED_BLOCK, "ERROR when checking pad bytes: memory corrupted before block.");
        }
        
    }

}

SimpleAllocator::SimpleAllocator(size_t objectSize, const SimpleAllocatorConfig& config)
    : config_(config), stats_(), pFreeList_(nullptr), pPageList_(nullptr) {

    stats_.objectSize = objectSize;
    stats_.mostObjects += config_.objectsPerPage;
    //stats_.pagesInUse++;
    stats_.freeObjects = config_.objectsPerPage;
    allocateNewPage();
}

SimpleAllocator::~SimpleAllocator() {
    while (pPageList_ != nullptr) {
        Node* nextpage = pPageList_->pNext;
        delete pPageList_;
        pPageList_ = nextpage;
    }
}

void* SimpleAllocator::allocate(const char* pLabel) {
    if (pFreeList_== nullptr)
    {
        allocateNewPage();
    }
    
    const SimpleAllocatorConfig::HeaderBlockInfo& headerBlockInfo = config_.headerBlockInfo;
    Node* pAllocatedBlock = pFreeList_;
    char * pAllocatesize = reinterpret_cast<char*>(pAllocatedBlock); // cast to count bytes in 1

    pFreeList_ = pFreeList_->pNext; //update pFreeList_ to point to its next memory
    memset(pAllocatedBlock,ALLOCATED_PATTERN,stats_.objectSize);

   
    char *pheader = pAllocatesize - headerBlockInfo.size - config_.padBytesSize; //minus to reach the header's pointer which is in front
    if(headerBlockInfo.type == config_.BASIC_HEADER)
    {
 
        unsigned int*header = reinterpret_cast<unsigned int*>(pheader); // cast to unsigned int* as in SimpleAllocator.h the header datatype is unsigned int
        ++allonum;  //increment allocation number for assignment uses
        *header = allonum; // assign the value in must be a usigned int datatype due to header's pointer's data type
        
    }
    else if (headerBlockInfo.type == config_.EXTENDED_HEADER)
    {
        
        //allocation num
        //char * pAllocatesize = reinterpret_cast<char*>(pAllocatedBlock); // cast to count bytes in 1
        char*allocnum = pAllocatesize - config_.padBytesSize -5;//getting to the center of the header
        unsigned int*num = reinterpret_cast<unsigned*>(allocnum);
        ++allonum;
        *num = allonum;

        //first number
        char*freeallnum = pAllocatesize - headerBlockInfo.size;
        (*freeallnum)++;
       

    }
    else if (config_.headerBlockInfo.type == SimpleAllocatorConfig::EXTERNAL_HEADER) {
        //Allocate a new MemBlockInfo structure. This is used to store metadata about the memory block.
        MemBlockInfo* MBI = new MemBlockInfo;
        // Initialize the MemBlockInfo structure.
        //Set MBI->inUse to True to indicate that this memory block is in use.
        MBI->inUse = true;
        //Store the total number of allocations made up to this point.
        //MBI->allocNum = allocationNumber_;

        //If pLabel is not nullptr, it means that a label was assigned and will be stored in MBI->pLabel
        if (pLabel != nullptr) {
            //Allocate memory for MBI->pLabel
            MBI->pLabel = new char[strlen(pLabel) + 1];
            //Store pLabel in MBI->pLabel
            strcpy(MBI->pLabel, pLabel);
        }
        //If pLabel is nullptr, don't store anything in MBI
        else {
            MBI->pLabel = nullptr;
        }

        // Store the pointer to the MemBlockInfo structure in the header.
        MemBlockInfo** header = reinterpret_cast<MemBlockInfo**>(pheader);
        *header = MBI;
    }
    char *ppad = pheader + (headerBlockInfo.size -1);
    bool* padpointer = reinterpret_cast<bool*>(ppad);
    *padpointer = 1;
    ++stats_.objectsInUse;
    ++stats_.allocations;

    if (stats_.objectsInUse > stats_.mostObjects)
    {
        stats_.mostObjects = stats_.objectsInUse;
    }
    stats_.freeObjects -= 1;
    
    return pAllocatedBlock;
}

void SimpleAllocator::free(void* pObj) {
    unsigned num = 0;
    if (pObj == nullptr) {
        return;
    }
    if(stats_.deallocations>=stats_.allocations)
    {
        throw SimpleAllocatorException(SimpleAllocatorException::E_BAD_BOUNDARY, "Error during free: not on a block boundary in page.");
    }
    const SimpleAllocatorConfig::HeaderBlockInfo& headerBlockInfo = config_.headerBlockInfo;
    Node* pBlock = static_cast<Node*>(pObj);//current block //makes a chunk of mem for page
    char *pcurrentblock = reinterpret_cast<char*>(pBlock);
    if(config_.padBytesSize>0)
    {
        corrupttest(pcurrentblock);
    }
    memset(pBlock,FREED_PATTERN,stats_.objectSize);
    pBlock->pNext = pFreeList_; // Update pBlock's next to point to the current head of the free list 
    
    pFreeList_ = pBlock; // Update the free list to point to pBlock
   
    char *pheader = pcurrentblock - headerBlockInfo.size - config_.padBytesSize;
    if(headerBlockInfo.type == config_.BASIC_HEADER)
    {
        
        unsigned int* headervalue = reinterpret_cast<unsigned int*>(pheader);
        *headervalue = num;
    }
    else if(headerBlockInfo.type == config_.EXTENDED_HEADER)
    {
        char* extheader = pheader + 2;
        *extheader = 0;
    }
    //If it is an external header, I need to retrieve the metadata about the memory block stored in the header.
    else if (config_.headerBlockInfo.type == SimpleAllocatorConfig::EXTERNAL_HEADER) {
        //Get the pointer to the MemBlockInfo structure stored in the header.
        //It is a double pointer as the header was stored as a double pointer in the allocate function.
        MemBlockInfo** header = reinterpret_cast<MemBlockInfo**>(pheader);
        //Access the metadata by dereferencing the header pointer.
        MemBlockInfo* MBI = *header;
        //If MBI is not a nullptr
        if (MBI != nullptr) {
            //Set MBI->inUse to false to indicate the memory block is no longer in use.
            MBI->inUse = false;

            //Delete the pLabel stored in MBI
            if (MBI->pLabel != nullptr) {
                delete[] MBI->pLabel;
            }
            // Delete MBI, essentially freeing the metadata about the memory block.
            delete MBI;
            // Set the pointer in the header to nullptr.
            *header = nullptr;
        }
    }

    char *flag = pheader + (headerBlockInfo.size - 1);
    bool *flagvalue = reinterpret_cast<bool*>(flag);
    *flagvalue = false;
    
    
    --stats_.objectsInUse;
    ++stats_.deallocations;
    // Update the count of free objects
    ++stats_.freeObjects;
}


void SimpleAllocator::setDebug(bool _isDebug) {
    config_.isDebug = _isDebug;
    // Implement debug logic here if needed.
}

const void* SimpleAllocator::getFreeList() const {
    return static_cast<const void*>(pFreeList_);
}

const void* SimpleAllocator::getPageList() const {
    return static_cast<const void*>(pPageList_);
}

SimpleAllocatorConfig SimpleAllocator::getConfig() const {
    return config_;
}

SimpleAllocatorStats SimpleAllocator::getStats() const {
    return stats_;
}

void SimpleAllocator::allocateNewPage() {

    const SimpleAllocatorConfig::HeaderBlockInfo& headerBlockInfo = config_.headerBlockInfo;
    size_t alignmentSize = config_.alignmentBoundary;
    size_t padsize = config_.padBytesSize;
    size_t object = stats_.objectSize;
    size_t blockSize = headerBlockInfo.size + padsize + object + padsize + alignmentSize; //memory per block
    size_t pagesize = (config_.objectsPerPage) * blockSize + 8; // total memory value of blocks in page

    // void* pagemem = new char[pagesize]; // allocate page memory
    // pPageList_ = static_cast<Node*>(pagemem);// pointer to page
    // pPageList_->pNext = nullptr; // next page is nullptr till newpage is allocated
    void* pagemem = new char[pagesize];
    // Create a new page node
    Node* newPage = static_cast<Node*>(pagemem);
    //newPage->pNext = nullptr;
    newPage->pNext = pPageList_;
    pPageList_=newPage;
   

    char* currentPage = reinterpret_cast<char*>(pPageList_);

    for (unsigned i = 0; i < config_.objectsPerPage;++i) {
        char* blockStart = currentPage + headerBlockInfo.size + padsize + 8 + alignmentSize;
        memset(blockStart,UNALLOCATED_PATTERN,object);
        if(padsize >0)
        {
            char *ppad = blockStart - padsize;
            memset(ppad,PAD_PATTERN,config_.padBytesSize);
            char*nextpad = blockStart + object;
            memset(nextpad,PAD_PATTERN,config_.padBytesSize);
        }
        Node* block = reinterpret_cast<Node*>(blockStart);
        block->pNext = nullptr;
        currentPage += blockSize;
        block->pNext = pFreeList_;
        pFreeList_ = block;

    }

    stats_.pageSize = pagesize;
    stats_.objectSize = object;
    stats_.mostObjects += config_.objectsPerPage;
    ++stats_.pagesInUse;
    stats_.freeObjects = config_.objectsPerPage;
    if(stats_.pagesInUse > config_.maxPages)
    {
        throw SimpleAllocatorException(SimpleAllocatorException::E_NO_MEMORY, "ERROR when allocating new page: maximum number of pages has been allocated.");
    }
}