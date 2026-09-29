// UAudMem.c (EA's name, from later builds: TW07's golf/audio/engine/utils/UAudMem.c has AudMem_Init,
// AudMem_Alloc and AudMem_Free in this order; TW06 lists utils/uaudmem.c beside uaudmemstack.c;
// TW2005 GC keeps the path .../Audio/Engine/Utils/UAudMem.c. This build has no string of it): the
// sound engine's memory, a stack (UAudMemStack.c) over 384 KB of main memory taken at boot.
// AudMem_Alloc hands out the voices, tracks and tables of the audio code from it, each block rounded
// up to 64 bytes; blocks must be given back last-first. The four empty AudMem_ steps at the end are
// called by the boot, shut-down and per-mode lists.

#include "core/audcontainers.h"

void  AudMem_Init(void);
void  AudMem_InitOnce(void);
void  AudMem_CloseOnce(void);
void  AudMem_InitModule(void);
void  AudMem_CloseModule(void);

// Defined last-address-first: CodeWarrior lays out .bss in reverse order of definition.
void* gAudMemBuffer;                    // the buffer of the sound engine's stack (0x60000 bytes)
UAudMemStack gAudMemStack;              // the sound engine's stack
UAudMemStackBlock gAudMemBlocks[64];    // its block table

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
