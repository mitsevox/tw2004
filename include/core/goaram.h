// goaram.h: GoARAM.c, the auxiliary RAM (ARAM): its heaps, which hand out ARAM addresses and keep
// their bookkeeping in main memory as block records in circular lists sorted by offset, and the
// queue of DMA transfers between main memory and ARAM.

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
    void*      pCircularMem;    // 0x20   a block of the game's heap, freed with the heap
                                //        (GoARAM_HeapClose); nothing in this build sets one
} ARAMHeap;
LAYOUT_ASSERT(ARAMHeap, 0x24);

// ---- the GameCube's ARAM libraries ------------------------------------------------------------

// ARQRequest.type: which way a DMA goes.
#define ARQ_TYPE_MRAM_TO_ARAM 0         // main memory to ARAM
#define ARQ_TYPE_ARAM_TO_MRAM 1         // ARAM to main memory

// ARQPostRequest's priority: the high queue is served before the low one.
#define ARQ_PRIORITY_LOW  0
#define ARQ_PRIORITY_HIGH 1

// A queued ARAM DMA (the ARQ library's request).
typedef struct ARQRequest {
    struct ARQRequest* next;        // 0x0
    u32                owner;       // 0x4
    u32                type;        // 0x8    an ARQ_TYPE_*
    u32                priority;    // 0xC
    u32                source;      // 0x10
    u32                dest;        // 0x14
    u32                length;      // 0x18
    void (*callback)(struct ARQRequest* pRequest); // 0x1C   called when the DMA is done
} ARQRequest;
LAYOUT_ASSERT(ARQRequest, 0x20);

u32  ARInit(u32* pStack, u32 nEntries);
u32  ARGetBaseAddress(void);    // the first ARAM address past what the ARAM library keeps
u32  ARGetSize(void);           // the ARAM's size, so the end of the ARAM GoARAM.c manages
void ARQPostRequest(ARQRequest* pRequest, u32 uOwner, u32 uType, u32 uPriority, u32 uSource, u32 uDest,
                    u32 uLength, void (*pfnCallback)(ARQRequest* pRequest));
void ARQRemoveRequest(ARQRequest* pRequest);

// ---- GoARAM.c ---------------------------------------------------------------------------------

#define ARAM_NUM_BLOCKS    500  // block records in the heap
#define ARAM_NUM_TRANSFERS 16

// Where a transfer is (ARAMTransfer.nState).
typedef enum ARAMTransferState {
    ARAM_TRANSFER_UNUSED = 0,   // in the unused list
    ARAM_TRANSFER_QUEUED = 1,   // in the queued list, its DMA posted
    ARAM_TRANSFER_DONE   = 2    // its DMA is done; still in the queued list until it is given back
} ARAMTransferState;

// ARAMTransfer.uFlags
#define ARAM_TRANSFER_RELEASE_WHEN_DONE 0x1     // given back as soon as its DMA is done
#define ARAM_TRANSFER_CANCELLABLE       0x2     // GoARAM_CancelTransfers may take it back

// A transfer between main memory and ARAM. The unused ones and the queued ones sit in two
// circular lists of the state.
typedef struct ARAMTransfer {
    ARQRequest        request;          // 0x0
    void (*pfnDone)(u32 uOwner);        // 0x20   called when the DMA is done
    u32               uOwner;           // 0x24   also the request's owner
    ARAMTransferState nState;           // 0x28
    int               nType;            // 0x2C   an ARQ_TYPE_*, as request.type
    u32               uMain;            // 0x30   the main-memory address
    u32               uLength;          // 0x34
    u8                uFlags;           // 0x38   ARAM_TRANSFER_* bits
    struct ARAMTransfer* pNext;         // 0x3C
    struct ARAMTransfer* pPrev;         // 0x40
} ARAMTransfer;
LAYOUT_ASSERT(ARAMTransfer, 0x44);

// GoARAM.c's state (gARAMState, reached through gpARAMState).
typedef struct ARAMState {
    ARAMHeap*     pHeap;                // 0x0
    void*         pHeapMem;             // 0x4    the heap and its block records (Mem_AllocFunc)
    void*         pTransfersMem;        // 0x8    the transfers, from Mem_AllocFunc (NULL when that
                                        //        failed)
    ARAMTransfer* pTransfers;           // 0xC    pTransfersMem or pTransfersCircular
    ARAMTransfer* pQueued;              // 0x10
    ARAMTransfer* pUnused;              // 0x14
    void*         pTransfersCircular;   // 0x18   the transfers, from Circular_MemAllocFunc when
                                        //        pTransfersMem is NULL
    s32           nTransfers;           // 0x1C
    s32           nQueued;              // 0x20
    s32           nUnused;              // 0x24
} ARAMState;
LAYOUT_ASSERT(ARAMState, 0x28);

extern ARAMState* gpARAMState;

// A heap of ARAM (startUp.c keeps a second heap inside one block of GoARAM.c's own).
ARAMHeap*     GoARAM_HeapInit(u32 uSize, u32 uBase, u32 nBlocks, ARAMHeap* pHeap);
u32           GoARAM_HeapAlloc(ARAMHeap* pHeap, u32 uSize, u32 uAlign);     // returns the ARAM address
void          GoARAM_HeapFree(ARAMHeap* pHeap, u32 uAram);

// Boot and shut-down, and the main loop's start-up and shut-down of the systems each mode needs.
void          GoARAM_InitOnce(void);
void          GoARAM_CloseOnce(void);
void          GoARAM_InitModule(void);
void          GoARAM_CloseModule(void);

// GoARAM.c's own heap and transfers.
u32           GoARAM_Alloc(u32 uSize);                  // take ARAM, returns its address
void          GoARAM_Free(u32 uAram);                   // give it back
ARAMTransfer* GoARAM_QueueTransfer(u32 uSource, u32 uDest, u32 uLength, int nType, u32 uPriority,
                                   void (*pfnDone)(u32 uOwner), u32 uOwner, u8 uFlags);
int           GoARAM_CancelTransfers(u32 uOwner);
void          GoARAM_WaitTransfer(ARAMTransfer* pTransfer);      // wait for a transfer, then free it
ARAMTransfer* GoARAM_CopyToAram(void* pSrc, u32 uAram, u32 uSize);
ARAMTransfer* GoARAM_CopyFromAram(void* pDst, u32 uAram, u32 uSize);

#endif
