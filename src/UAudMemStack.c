// UAudMemStack.c (EA's name, from its asserts; also in EA's 2002 source tree): the sound engine's
// stack allocator. AudMem_Init takes 384 KB of main memory at boot, and AudMem_Alloc hands out the
// voices, tracks and tables of the audio code from it, each block rounded up to 64 bytes.

#include "core/audcontainers.h"

void  AudMemStack_FreeTop(UAudMemStack* pStack, void* p);
void  AudMem_Init(void);
void  AudMem_InitOnce(void);
void  AudMem_CloseOnce(void);
void  AudMem_InitModule(void);
void  AudMem_CloseModule(void);

// Defined last-address-first: CodeWarrior lays out .bss in reverse order of definition.
void* gAudMemBuffer;                     // the buffer of the sound engine's stack
UAudMemStack gAudMemStack;              // the sound engine's stack
UAudMemStackBlock gAudMemBlocks[64];     // its table

// Sets up a stack over [pMem, pMem + uSize). With no table given, the stack allocates its own.
void AudMemStack_Init(UAudMemStack* pStack, u8* pMem, u32 uSize, u32 nMaxBlocks, UAudMemStackBlock* pBlocks,
                 u32 nAlign) {
    pStack->pTop = pMem;
    pStack->pEnd = pMem + uSize;
    pStack->nMaxBlocks = nMaxBlocks;
    pStack->nBlocks = 0;
    pStack->n14 = 0;
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

    if (pStack->nBlocks + pStack->n14 >= pStack->nMaxBlocks) return NULL;
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

// The sound engine's memory (the first step of Aud_InitOnce): 384 KB (0x60000) of main memory,
// 64-byte aligned, as a stack of up to 64 blocks.
void AudMem_Init(void) {
    gAudMemBuffer = fn_800951A0(0x60000, 64, 0);
    AudMemStack_Init(&gAudMemStack, gAudMemBuffer, 0x60000, 64, gAudMemBlocks, 64);
}

// Takes uSize bytes (rounded up to 64) from the sound engine's memory; NULL when it is full.
void* AudMem_Alloc(u32 uSize) {
    return AudMemStack_AllocTop(&gAudMemStack, uSize);
}

// Gives a block back to the sound engine's memory (AudMemStack_FreeTop: safe only for the last one
// taken).
void AudMem_Free(void* p) {
    AudMemStack_FreeTop(&gAudMemStack, p);
}

// Empty in this build: the boot list fn_80005520 calls it just before GoARAM_Init.
void AudMem_InitOnce(void) {
}

// Empty in this build: the shut-down list fn_80005590 calls it just after GoARAM_Shutdown.
void AudMem_CloseOnce(void) {
}

// Empty in this build: the per-mode start-up fn_8006C7A8 calls it right after StaticMem_Reset.
void AudMem_InitModule(void) {
}

// Empty in this build: the per-mode shut-down fn_8006C854 calls it just before
// StaticMem_Checkpoint.
void AudMem_CloseModule(void) {
}
