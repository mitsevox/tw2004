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

// ---- the GameCube's ARAM libraries (port: GameCube only) -------------------------------------

// A queued ARAM DMA (the ARQ library's request).
typedef struct ARQRequest {
    struct ARQRequest* next;        // 0x0
    u32                owner;       // 0x4
    u32                type;        // 0x8    0 main memory to ARAM, 1 ARAM to main memory
    u32                priority;    // 0xC
    u32                source;      // 0x10
    u32                dest;        // 0x14
    u32                length;      // 0x18
    void (*callback)(struct ARQRequest* pRequest); // 0x1C   port: the library passes it as a u32
} ARQRequest;
LAYOUT_ASSERT(ARQRequest, 0x20);

u32  ARInit(u32* pStack, u32 nEntries);
u32  ARGetBaseAddress(void);
u32  fn_80133B18(void);         // the end of the ARAM GoARAM.c manages
void ARQPostRequest(ARQRequest* pRequest, u32 uOwner, u32 uType, u32 uPriority, u32 uSource, u32 uDest,
                    u32 uLength, void (*pfnCallback)(ARQRequest* pRequest));
void ARQRemoveRequest(ARQRequest* pRequest);

// ---- GoARAM.c ---------------------------------------------------------------------------------

#define ARAM_NUM_BLOCKS    500  // block records in the heap
#define ARAM_NUM_TRANSFERS 16

// A transfer between main memory and ARAM. The unused ones and the queued ones sit in two
// circular lists of the state.
typedef struct ARAMTransfer {
    ARQRequest request;                 // 0x0
    void (*pfnDone)(u32 uOwner);        // 0x20   called when the DMA is done
    u32        uOwner;                  // 0x24
    s32        nState;                  // 0x28   0 unused, 1 queued, 2 done
    int        nType;                   // 0x2C   as request.type
    u32        uMain;                   // 0x30   the main-memory address
    u32        uLength;                 // 0x34
    u8         uFlags;                  // 0x38   1: give it back once done; 2: fn_800B6728 cancels it
    struct ARAMTransfer* pNext;         // 0x3C
    struct ARAMTransfer* pPrev;         // 0x40
} ARAMTransfer;
LAYOUT_ASSERT(ARAMTransfer, 0x44);

// GoARAM.c's state (lbl_801F6680, reached through lbl_802814C8).
typedef struct ARAMState {
    ARAMHeap*     pHeap;        // 0x0
    void*         pHeapMem;     // 0x4    the heap and its block records (fn_800951A0)
    void*         p8;           // 0x8    the transfers, from fn_800951A0 (NULL when that failed)
    ARAMTransfer* pTransfers;   // 0xC    p8 or p18
    ARAMTransfer* pQueued;      // 0x10
    ARAMTransfer* pUnused;      // 0x14
    void*         p18;          // 0x18   the transfers, from fn_80009B34 when p8 is NULL
    s32           nTransfers;   // 0x1C
    s32           nQueued;      // 0x20
    s32           nUnused;      // 0x24
} ARAMState;
LAYOUT_ASSERT(ARAMState, 0x28);

extern ARAMState* lbl_802814C8;

// The heap (startUp.c keeps a second heap inside one block of this one).
ARAMHeap*     fn_800B5C40(u32 uSize, u32 uBase, u32 nBlocks, ARAMHeap* pHeap);
u32           fn_800B5D34(ARAMHeap* pHeap, u32 uSize, u32 uAlign);     // returns the ARAM address
void          fn_800B5E88(ARAMHeap* pHeap, u32 uAram);

// GoARAM.c's own heap and transfers.
u32           fn_800B6564(u32 uSize);                  // take ARAM, returns its address
void          fn_800B6594(u32 uAram);                  // give it back
ARAMTransfer* fn_800B65C0(u32 uSource, u32 uDest, u32 uLength, int nType, u32 uPriority,
                          void (*pfnDone)(u32 uOwner), u32 uOwner, u8 uFlags);
int           fn_800B6728(u32 uOwner);                 // cancel uOwner's transfers
void          fn_800B67EC(ARAMTransfer* pTransfer);    // wait for a transfer, then free it
ARAMTransfer* fn_800B6844(void* pSrc, u32 uAram, u32 uSize);   // copy to ARAM
ARAMTransfer* fn_800B68B4(void* pDst, u32 uAram, u32 uSize);   // copy from ARAM

#endif
