// UAudMemStack.c (EA's name, from its asserts; also in EA's 2002 source tree): the sound engine's
// stack allocator. fn_800B5B80 takes 384 KB of main memory at boot, and fn_800B5BD8 hands out the
// voices, tracks and tables of the audio code from it, each block rounded up to 64 bytes.

#include "core/audcontainers.h"

void  fn_800B5B2C(UAudMemStack* pStack, void* p);
void  fn_800B5B80(void);
void  fn_800B5C30(void);
void  fn_800B5C34(void);
void  fn_800B5C38(void);
void  fn_800B5C3C(void);

// Defined last-address-first: CodeWarrior lays out .bss in reverse order of definition.
void* lbl_80282180;                     // the buffer of the sound engine's stack
UAudMemStack lbl_801F6660;              // the sound engine's stack
UAudMemStackBlock lbl_801F6360[64];     // its table

// Sets up a stack over [pMem, pMem + uSize). With no table given, the stack allocates its own.
void fn_800B5A14(UAudMemStack* pStack, u8* pMem, u32 uSize, u32 nMaxBlocks, UAudMemStackBlock* pBlocks,
                 u32 nAlign) {
    pStack->pTop = pMem;
    pStack->pEnd = pMem + uSize;
    pStack->nMaxBlocks = nMaxBlocks;
    pStack->nBlocks = 0;
    pStack->n14 = 0;
    pStack->nAlign = nAlign;
    if (pBlocks == NULL) {
        pStack->pBlocks =
            fn_80009B34(nMaxBlocks * sizeof(UAudMemStackBlock), 0, nAlign, "UAudMemStack.c", 40);
        pStack->bOwnBlocks = 1;
    } else {
        pStack->pBlocks = pBlocks;
        pStack->bOwnBlocks = 0;
    }
    fn_80005AE8(pStack->pBlocks, 0, pStack->nMaxBlocks * sizeof(UAudMemStackBlock));
}

// Cuts a block of uSize bytes (rounded up to the alignment) from the top; NULL when the table or
// the buffer is full.
void* fn_800B5AAC(UAudMemStack* pStack, u32 uSize) {
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

// Gives back the block starting at p: the top drops by its size.
void fn_800B5B2C(UAudMemStack* pStack, void* p) {
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

void fn_800B5B80(void) {
    lbl_80282180 = fn_800951A0(0x60000, 64, 0);
    fn_800B5A14(&lbl_801F6660, lbl_80282180, 0x60000, 64, lbl_801F6360, 64);
}

void* fn_800B5BD8(u32 uSize) {
    return fn_800B5AAC(&lbl_801F6660, uSize);
}

void fn_800B5C04(void* p) {
    fn_800B5B2C(&lbl_801F6660, p);
}

void fn_800B5C30(void) {
}

void fn_800B5C34(void) {
}

void fn_800B5C38(void) {
}

void fn_800B5C3C(void) {
}
