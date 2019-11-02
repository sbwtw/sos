
#include "memorymanagement.h"
#include "std/stdio.h"
#include "std/exception.h"

MemoryAllocator *activeMemoryAllocator = nullptr;

MemoryAllocator::MemoryAllocator(size_t start, size_t size)
{
    activeMemoryAllocator = this;
    firstThunk = (MemoryChunk *)start;

    firstThunk->prev = nullptr;
    firstThunk->next = nullptr;
    firstThunk->size = size;
    firstThunk->allocated = false;
}

void MemoryAllocator::dumpAllocatorInfo()
{
    MemoryChunk *c = firstThunk;

    printf("Memory Allocate Info\n");

    while (c)
    {
        printf("start: %x, alloc: %d, size: %x\n", c, c->allocated, c->size);

        c = c->next;
    }

    printf("\n");
}

void *operator new(size_t size)
{
    if (size)
        return activeMemoryAllocator->malloc(size);

    // TODO: throw std::bad_alloc
    return 0;
}

void *operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void *ptr)
{
    return activeMemoryAllocator->free(ptr);
}

void operator delete[](void *ptr)
{
    return operator delete(ptr);
}


