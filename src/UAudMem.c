// UAudMem.c: the sound engine's memory, 384 KB of main memory run as a stack (UAudMemStack.c) of up
// to 64 blocks, each rounded up to 64 bytes. The sound engine's voices, tracks, banks and stream
// buffers, among others, are taken from it; blocks must be given back last-first.

#include "core/audcontainers.h"

#define AUDMEM_SIZE         0x60000     // the sound engine's memory: 384 KB
#define AUDMEM_ALIGN        64          // the buffer's alignment and every block's
#define AUDMEM_NUM_BLOCKS   64          // blocks the stack can hand out at once

void* gAudMemBuffer;                            // the buffer of the sound engine's stack
AudMemStack gAudMemStack;                       // the sound engine's stack
MemStackBlock gAudMemBlocks[AUDMEM_NUM_BLOCKS]; // its block table

// The first step of Aud_InitOnce: takes 384 KB of main memory, 64-byte aligned, and sets it up as a
// stack of up to 64 blocks.
void AudMem_Init(void) {
    gAudMemBuffer = Mem_AllocFunc(AUDMEM_SIZE, AUDMEM_ALIGN, 0);
    AudMemStack_Init(&gAudMemStack, gAudMemBuffer, AUDMEM_SIZE, AUDMEM_NUM_BLOCKS, gAudMemBlocks,
                     AUDMEM_ALIGN);
}

// Takes nBytes (rounded up to 64) from the sound engine's memory; NULL when they do not fit or 64
// blocks are already out.
void* AudMem_Alloc(u32 nBytes) {
    return AudMemStack_AllocTop(&gAudMemStack, nBytes);
}

// Gives back the block at addr. The stack's top drops by the block's size, so only the last block
// taken can be given back without freeing memory still in use.
void AudMem_Free(void* addr) {
    AudMemStack_FreeTop(&gAudMemStack, addr);
}

// Empty. main's start-up list calls it just before GoARAM_InitOnce.
void AudMem_InitOnce(void) {
}

// Empty. main's shut-down list calls it just after GoARAM_CloseOnce.
void AudMem_CloseOnce(void) {
}

// Empty. The module start-up list calls it right after Circular_InitBuffer.
void AudMem_InitModule(void) {
}

// Empty. The module shut-down list calls it just before Circular_CloseBuffer.
void AudMem_CloseModule(void) {
}
