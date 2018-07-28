
#ifndef __MEMORY_MANAGEMENT_H_
#define __MEMORY_MANAGEMENT_H_

#include "types.h"

/**
 * Memory Block Information.
 */

struct MemoryChunk
{
    MemoryChunk *prev;
    MemoryChunk *next;

    bool allocated;
    size_t size;
};

/**
 * Abstract base class for Memory Allocator.
 */

class MemoryAllocator
{
public:
    MemoryAllocator(size_t start, size_t size);

    void dumpAllocatorInfo();

    // FIXME: we can't create pure virtual function before implements `__cxa_pure_virtual`
    virtual void *malloc(size_t size) { return nullptr; }
    virtual void free(void *ptr) {};

protected:
    MemoryChunk *firstThunk;
};

// C++ memory allocate/free stuff.
void *operator new(size_t size);
void operator delete(void *ptr);

#endif // __MEMORY_MANAGEMENT_H_