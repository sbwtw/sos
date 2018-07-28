
#ifndef __FIRST_FIT_ALLOCATOR_H_
#define __FIRST_FIT_ALLOCATOR_H_

#include "memorymanagement.h"

/**
 * MemoryAllocator implements with First-Fit Memory Allocate algorithm.
 */

class FirstFitAllocator : public MemoryAllocator
{
public:
    FirstFitAllocator(size_t start, size_t size);

    void *malloc(size_t size) override;
    void free(void *ptr) override;
};

#endif // __FIRST_FIT_ALLOCATOR_H_