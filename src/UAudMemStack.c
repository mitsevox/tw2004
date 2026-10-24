// UAudMemStack.c: the sound engine's stack allocator. Blocks are cut from the top of one buffer and
// rounded up to the stack's alignment; giving one back lowers the top by its size, so blocks must be
// given back last-first. The sound engine's memory (UAudMem.c) and the effect memory each run one.

#include "core/audcontainers.h"

// Sets up a stack over [startAddr, startAddr + nBytes) with room for maxAllocs blocks. With no
// table given, the stack takes one from the game's heap and marks it as its own.
void AudMemStack_Init(AudMemStack* pAudMemStack, u8* startAddr, u32 nBytes, u32 maxAllocs,
                      MemStackBlock* memBlocksA, u32 alignPow2) {
    pAudMemStack->pTop = startAddr;
    pAudMemStack->pEnd = startAddr + nBytes;
    pAudMemStack->nMaxBlocks = maxAllocs;
    pAudMemStack->nBlocks = 0;
    pAudMemStack->nBtmBlocks = 0;
    pAudMemStack->nAlign = alignPow2;
    if (memBlocksA == NULL) {
        pAudMemStack->pBlocks =
            Circular_MemAllocFunc(maxAllocs * sizeof(MemStackBlock), MEMTYPE_SYSTEM, alignPow2,
                                  "UAudMemStack.c", 40);
        pAudMemStack->bOwnBlocks = 1;
    } else {
        pAudMemStack->pBlocks = memBlocksA;
        pAudMemStack->bOwnBlocks = 0;
    }
    Mem_set(pAudMemStack->pBlocks, 0, pAudMemStack->nMaxBlocks * sizeof(MemStackBlock));
}

// Cuts a block of nBytes (rounded up to the alignment) from the top. NULL when the table is full or
// the block would not end before the buffer's end.
void* AudMemStack_AllocTop(AudMemStack* pAudMemStack, u32 nBytes) {
    MemStackBlock* pmbl = &pAudMemStack->pBlocks[pAudMemStack->nBlocks];

    if (pAudMemStack->nBlocks + pAudMemStack->nBtmBlocks >= pAudMemStack->nMaxBlocks) return NULL;
    nBytes = (nBytes + (pAudMemStack->nAlign - 1)) & ~(pAudMemStack->nAlign - 1);
    if (pAudMemStack->pTop + nBytes >= pAudMemStack->pEnd) return NULL;
    pmbl->pMem = pAudMemStack->pTop;
    pmbl->uSize = nBytes;
    pAudMemStack->pTop += nBytes;
    pAudMemStack->nBlocks++;
    return pmbl->pMem;
}

// Gives back the block starting at addr (looked up in the table; nothing when it is not there): the
// top drops by its size, so only the last block handed out can be given back without freeing live
// memory.
void AudMemStack_FreeTop(AudMemStack* pAudMemStack, void* addr) {
    MemStackBlock* pmbl = pAudMemStack->pBlocks;
    u32 i;

    for (i = 0; i < pAudMemStack->nMaxBlocks; i++, pmbl++) {
        if (pmbl->pMem == addr) {
            pAudMemStack->nBlocks--;
            pAudMemStack->pTop -= pmbl->uSize;
            pmbl->uSize = 0;
            return;
        }
    }
}
