
#ifndef __MEMORY_MANAGEMENT_H_
#define __MEMORY_MANAGEMENT_H_

#include "types.h"

struct MemoryChunk
{
    MemoryChunk *prev;
    MemoryChunk *next;

    bool allocated;
    size_t size;
};

class MemoryManager
{
public:

    MemoryManager(size_t start, size_t size);

    void dumpAllocatorInfo();

    void *malloc(size_t size);
    void free(void *ptr);

private:
    MemoryChunk *firstThunk;
};

void *operator new(size_t size);
void operator delete(void *ptr);

#endif // __MEMORY_MANAGEMENT_H_