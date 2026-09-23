// goaram.h (our name): GoARAM.c's heap of audio RAM (ARAM). The heap hands out ARAM addresses; it
// keeps its bookkeeping in main memory, as block records in circular lists sorted by offset.

#ifndef CORE_GOARAM_H
#define CORE_GOARAM_H

#include "engine.h"

// A stretch of the heap, free or in use. Records that describe nothing wait in the spare list.
typedef struct ARAMBlock {
    struct ARAMBlock* pNext;    // 0x0
    struct ARAMBlock* pPrev;    // 0x4
    u32               uSize;    // 0x8    bytes, the alignment padding included
    u32               uOffset;  // 0xC    from the heap's base
    u32               uStart;   // 0x10   the aligned offset handed out (in-use blocks)
} ARAMBlock;
LAYOUT_ASSERT(ARAMBlock, 0x14);

// The heap's header; its nBlocks block records follow it in the same allocation.
typedef struct ARAMHeap {
    ARAMBlock* pFree;           // 0x0    the free blocks, lowest offset first
    ARAMBlock* pUsed;           // 0x4    the blocks handed out, lowest offset first
    ARAMBlock* pBlocks;         // 0x8    the block records
    ARAMBlock* pSpare;          // 0xC    the unused records
    u32        uFree;           // 0x10   free bytes
    u32        uSize;           // 0x14   the heap's size
    u32        uBase;           // 0x18   the ARAM address of offset 0
    u32        nBlocks;         // 0x1C   block records
    void*      p20;             // 0x20   freed with the heap (fn_800B5D08)
} ARAMHeap;
LAYOUT_ASSERT(ARAMHeap, 0x24);

// GoARAM.c's state (reached through lbl_802814C8). Its size is not known yet.
typedef struct ARAMState {
    ARAMHeap* pHeap;            // 0x0
    void*     p4;               // 0x4    freed with fn_8009527C
    void*     p8;               // 0x8    freed with fn_8009527C
    u32       unkC;             // 0xC
    void*     p10;              // 0x10   a list of transfers (fn_800B6214)
    void*     p14;              // 0x14   a list of transfers (fn_800B6214)
    void*     p18;              // 0x18   freed with fn_80009E70
    u32       unk1C;            // 0x1C
    s32       n20;              // 0x20   counts down as fn_800B6214 moves a transfer to p14
    s32       n24;              // 0x24   counts up as fn_800B6214 moves a transfer to p14
} ARAMState;

extern ARAMState* lbl_802814C8;

#endif
