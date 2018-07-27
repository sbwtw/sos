
#include "memorymanagement.h"
#include "sos_io.h"

MemoryManager *MemoryManager::activeMemoryManager = nullptr;

MemoryManager::MemoryManager(size_t start, size_t size)
{
    activeMemoryManager = this;
    firstThunk = (MemoryChunk *)start;

    firstThunk->prev = nullptr;
    firstThunk->next = nullptr;
    firstThunk->size = size;
    firstThunk->allocated = false;
}

void *MemoryManager::malloc(size_t size)
{
    MemoryChunk *r = nullptr;

    for (MemoryChunk *mc = firstThunk; mc; mc = mc->next)
    {
        if (!mc->allocated && mc->size > size)
        {
            r = mc;
            break;
        }
    }

    if (!r)
        return nullptr;

    if (r->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk *newChunk = (MemoryChunk *)((size_t)r + sizeof(MemoryChunk) + size);

        newChunk->allocated = false;
        newChunk->size = r->size - size - sizeof(MemoryChunk);
        newChunk->prev = r;
        newChunk->next = r->next;

        if (newChunk->next)
            newChunk->next->prev = newChunk;

        r->size = size;
        r->next = newChunk;
    }

    r->allocated = true;

    return (void *)((size_t)r + sizeof(MemoryChunk));
}

void MemoryManager::free(void *ptr)
{
    MemoryChunk *chunk = (MemoryChunk *)((size_t)ptr - sizeof(MemoryChunk));

    chunk->allocated = false;

    // merge prev
    if (chunk->prev && !chunk->prev->allocated)
    {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if (chunk->next)
            chunk->next->prev = chunk->prev;

        chunk = chunk->prev;
    }

    // merge next
    if (chunk->next && !chunk->next->allocated)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if (chunk->next)
            chunk->next->prev = chunk;
    }
}

void MemoryManager::dumpAllocatorInfo()
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

