/**
 * @file SimpleAllocator.cpp
 * @brief Implementation of the SimpleAllocator class.
 * @date March 2019
 */

// #define DEBUG
#include "SimpleAllocator.h"
#include <cstdio>
#include <cstring>
#include <iostream>

SimpleAllocator::SimpleAllocator(size_t objectSize,
                                 const SimpleAllocatorConfig& config)
    : config_(config), stats_{}{
    stats_.objectSize = objectSize;
    config_.useCPPMemManager = true; // always true for this dummy allocator
}

SimpleAllocator::~SimpleAllocator() {
}

void* SimpleAllocator::allocate(const char* pLabel) {
    // use cpp mem manager if enabled
    if (config_.useCPPMemManager) {
        // update stats assuming allocation succeeds
        ++stats_.allocations;
        ++stats_.mostObjects;

        // return exact number of bytes requested using char
        return new char[stats_.objectSize];
    }
    else {
        // this is supposed to be the custom allocation code but this is a dummy
        // - this should never be called in this dummy allocator
        return nullptr;
    }
}

void SimpleAllocator::free(void* pObject) {
    if (config_.useCPPMemManager) {
        // update stats assuming allocation successful
        ++stats_.deallocations;
        --stats_.allocations;

        // delete exact number of bytes represented using char
        //::operator delete(static_cast<char*>(p_object), stats_.ObjectSize_);
        delete[] static_cast<char*>(pObject);

        pObject = nullptr;
    } 
    else {
        // again this should never be called in this dummy allocator
    }
}

SimpleAllocatorConfig SimpleAllocator::getConfig() const { return config_; }

SimpleAllocatorStats SimpleAllocator::getStats() const { return stats_; }
