// UMemPool.c: pools of fixed-size nodes carved from one block of the game's heap: the file
// streamer's done list, the kernel's small and large objects, the animation blender's nodes and
// poses, and the grass's render records.

#include "engine.h"

// Makes a pool of numToPreallocate nodes of sizeOfEntry bytes each from one Circular_MemAllocFunc
// block of memoryType: the header is padded to alignment (a power of two) and each node's size is
// rounded up to it. The nodes are filled with 0xDD and chained into the free list, the last one
// first. NULL when the allocation fails.
MemPoolPtr CreateMemPool(int numToPreallocate, u32 sizeOfEntry, TTypeAcces memoryType, u32 alignment) {
    MemPoolPtr pool;
    u32 actualEntrySize;
    char* entryPtr;
    UMemPoolNode* lastEntryPtr;
    u32 poolBlockSize;

    actualEntrySize = alignment + sizeOfEntry;
    actualEntrySize = (actualEntrySize - 1) & ~(alignment - 1);
    poolBlockSize = alignment + numToPreallocate * actualEntrySize;
    pool = Circular_MemAllocFunc(poolBlockSize, memoryType, alignment, "UMemPool.c", 82);
    if (pool != NULL) {
        pool->nNodes = numToPreallocate;
        pool->nFree = numToPreallocate;
        pool->uNodeSize = actualEntrySize;
        pool->pEnd = (u8*)(poolBlockSize + (uptr)pool);
        entryPtr = (char*)pool + alignment;
        lastEntryPtr = NULL;
        while (numToPreallocate-- > 0) {
            Mem_set(entryPtr, 0xDD, actualEntrySize);
            ((UMemPoolNode*)entryPtr)->pNext = lastEntryPtr;
            lastEntryPtr = (UMemPoolNode*)entryPtr;
            entryPtr += actualEntrySize;
        }
        pool->pFree = lastEntryPtr;
    }
    return pool;
}

// Frees a pool and all its nodes at once (its one Circular_MemAllocFunc block).
void DeleteMemPool(MemPoolPtr memPool) {
    Circular_MemFreeFunc(memPool);
}

// Takes a node off the free list and fills it with 0xBB; NULL when the pool is empty.
void* AllocPoolMem(MemPoolPtr memPool) {
    UMemPoolNode* entry;

    entry = memPool->pFree;
    if (entry != NULL) {
        memPool->pFree = entry->pNext;
        memPool->nFree--;
        Mem_set(entry, 0xBB, memPool->uNodeSize);
    }
    return entry;
}

// Fills a node with 0x99 and puts it back on the free list.
void ReturnPoolMem(MemPoolPtr memPool, void* poolEntry) {
    Mem_set(poolEntry, 0x99, memPool->uNodeSize);
    ((UMemPoolNode*)poolEntry)->pNext = memPool->pFree;
    memPool->pFree = poolEntry;
    memPool->nFree++;
}
