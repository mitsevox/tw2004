// audcontainers.h: the sound engine's containers (UAudContainers.c: a doubly linked list, a
// ring-buffer queue and a pool of fixed-size blocks), its stack allocator (UAudMemStack.c) and its
// allocator (UAudMem.c).

#ifndef CORE_AUDCONTAINERS_H
#define CORE_AUDCONTAINERS_H

#include "engine.h"

// A link in a TListDbl. It sits at the start of whatever the list holds.
typedef struct TListDblLink_S {
    struct TListDblLink_S* pPrev;   // 0x0    towards the head, NULL at the head
    struct TListDblLink_S* pNext;   // 0x4    towards the tail, NULL at the tail
} TListDblLink;
LAYOUT_ASSERT(TListDblLink, 0x8);

// A doubly linked list.
typedef struct TListDbl {
    TListDblLink* pHead;        // 0x0
    TListDblLink* pTail;        // 0x4
    u16           nSlots;       // 0x8    set by UList_Reset, never read
    u16           nCount;       // 0xA    links in the list
} TListDbl;
LAYOUT_ASSERT(TListDbl, 0xC);

// A ring buffer of nMax items of nItemSize bytes each, from pBase to pEnd.
typedef struct TQueue {
    u8*  pBase;                 // 0x0    the buffer
    u8*  pEnd;                  // 0x4    one past its end
    u8*  pRead;                 // 0x8    the oldest item, taken by UQueue_Pop
    u8*  pWrite;                // 0xC    where UQueue_Push puts the next item
    u16  nMax;                  // 0x10   items the buffer holds
    u16  nCount;                // 0x12   items in the queue
    u32  nItemSize;             // 0x14
} TQueue;
LAYOUT_ASSERT(TQueue, 0x18);

// A link in a singly linked list: a free block in a TPool links to the next free block with its
// first word.
typedef struct TListSglLink_S {
    struct TListSglLink_S* pNext;   // 0x0
} TListSglLink;
LAYOUT_ASSERT(TListSglLink, 0x4);

// A pool of fixed-size blocks. The free blocks form a chain from head.pNext that ends at the pool
// itself (&head), not at NULL.
typedef struct TPool_S {
    TListSglLink head;          // 0x0    head.pNext: the first free block
    u32          nFree;         // 0x4    free blocks
} TPool;
LAYOUT_ASSERT(TPool, 0x8);

void          UList_Reset(TListDbl* plist, u16 nslots);
void          UList_PushHead(TListDbl* plist, TListDblLink* plink);
void          UList_PushTail(TListDbl* plist, TListDblLink* plink);
void          UList_InsertAt(TListDbl* plist, TListDblLink* plink, TListDblLink* pdestlink);
void          UList_DeleteAt(TListDbl* plist, TListDblLink* plink);
void          UQueue_Reset(TQueue* pq, void* buff, u16 nslots, u32 itemSize);
u8*           UQueue_Push(TQueue* pq);
u8*           UQueue_Pop(TQueue* pq);
void          UPool_Init(TPool* pPool, void* pBuffer, u32 nEntries, u32 entrySizeBytes);
TListSglLink* UPool_Alloc(TPool* pPool);
void          UPool_Free(TPool* pPool, TListSglLink* pNode);

// UAudMemStack.c: a stack allocator. Blocks are cut from the top of one buffer, and each one is
// recorded in a table so it can be given back by its address.
typedef struct MemStackBlock_S {
    u8*  pMem;                  // 0x0    the block's start
    u32  uSize;                 // 0x4    its size, rounded up to the stack's alignment; 0 once freed
    u32  unk8;                  // 0x8
} MemStackBlock;
LAYOUT_ASSERT(MemStackBlock, 0xC);

typedef struct AudMemStack_S {
    u8*            pTop;        // 0x0    where the next block starts
    u8*            pEnd;        // 0x4    one past the buffer's end
    u32            nMaxBlocks;  // 0x8    entries in pBlocks
    MemStackBlock* pBlocks;     // 0xC    the block table; AudMemStack_AllocTop fills entry nBlocks
    u32            nBlocks;     // 0x10   blocks handed out from the top
    u32            nBtmBlocks;  // 0x14   blocks cut from the bottom: nothing in this game cuts one, so
                                //        always 0; counted with nBlocks against nMaxBlocks
    u32            nAlign;      // 0x18   a power of two
    u8             bOwnBlocks;  // 0x1C   pBlocks was taken from the game's heap by the stack itself
} AudMemStack;
LAYOUT_ASSERT(AudMemStack, 0x20);

void  AudMemStack_Init(AudMemStack* pAudMemStack, u8* startAddr, u32 nBytes, u32 maxAllocs,
                       MemStackBlock* memBlocksA, u32 alignPow2);
void* AudMemStack_AllocTop(AudMemStack* pAudMemStack, u32 nBytes);
void  AudMemStack_FreeTop(AudMemStack* pAudMemStack, void* addr);

// UAudMem.c: the sound engine's memory, one stack of 64-byte blocks.
void  AudMem_Init(void);
void* AudMem_Alloc(u32 nBytes);
void  AudMem_Free(void* addr);
void  AudMem_InitOnce(void);
void  AudMem_CloseOnce(void);
void  AudMem_InitModule(void);
void  AudMem_CloseModule(void);

#endif
