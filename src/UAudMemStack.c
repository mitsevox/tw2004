// UAudMemStack.c (EA's name, from its asserts; also in EA's 2002 source tree): the sound engine's
// stack allocator (AudMemStack_, TW07's names). Blocks are cut from the top of one buffer and
// rounded up to the stack's alignment; giving one back lowers the top by its size, so blocks must
// be given back last-first. UAudMem.c (the sound engine's memory) and AudReverb.c (the effect
// memory) each run a stack.

#include "core/audcontainers.h"

// Sets up a stack over [pMem, pMem + uSize). With no table given, the stack allocates its own.
void AudMemStack_Init(UAudMemStack* pStack, u8* pMem, u32 uSize, u32 nMaxBlocks, UAudMemStackBlock* pBlocks,
                 u32 nAlign) {
    pStack->pTop = pMem;
    pStack->pEnd = pMem + uSize;
    pStack->nMaxBlocks = nMaxBlocks;
    pStack->nBlocks = 0;
    pStack->nBtmBlocks = 0;
    pStack->nAlign = nAlign;
    if (pBlocks == NULL) {
        pStack->pBlocks =
            StaticMem_Alloc(nMaxBlocks * sizeof(UAudMemStackBlock), 0, nAlign, "UAudMemStack.c", 40);
        pStack->bOwnBlocks = 1;
    } else {
        pStack->pBlocks = pBlocks;
        pStack->bOwnBlocks = 0;
    }
    Mem_set(pStack->pBlocks, 0, pStack->nMaxBlocks * sizeof(UAudMemStackBlock));
}

// Cuts a block of uSize bytes (rounded up to the alignment) from the top; NULL when the table or
// the buffer is full.
void* AudMemStack_AllocTop(UAudMemStack* pStack, u32 uSize) {
    UAudMemStackBlock* pBlock = &pStack->pBlocks[pStack->nBlocks];

    if (pStack->nBlocks + pStack->nBtmBlocks >= pStack->nMaxBlocks) return NULL;
    uSize = (uSize + (pStack->nAlign - 1)) & ~(pStack->nAlign - 1);
    if (pStack->pTop + uSize >= pStack->pEnd) return NULL;
    pBlock->pMem = pStack->pTop;
    pBlock->uSize = uSize;
    pStack->pTop += uSize;
    pStack->nBlocks++;
    return pBlock->pMem;
}

// Gives back the block starting at p (found in the table): the top drops by its size, so only the
// last block handed out can be given back without freeing live memory.
void AudMemStack_FreeTop(UAudMemStack* pStack, void* p) {
    UAudMemStackBlock* pBlock = pStack->pBlocks;
    u32 i;

    for (i = 0; i < pStack->nMaxBlocks; i++, pBlock++) {
        if (pBlock->pMem == p) {
            pStack->nBlocks--;
            pStack->pTop -= pBlock->uSize;
            pBlock->uSize = 0;
            return;
        }
    }
}
