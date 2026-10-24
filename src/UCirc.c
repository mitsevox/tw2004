// UCirc.c: the game's own heap, about 20 MB taken from the system heap at start-up and emptied each
// time the per-mode systems start. A sorted table of addresses holds its free spans; blocks are cut
// from the low or the high end of a span, and the system heap stands in when it is full.

#include "engine.h"

// The table of free spans: sorted addresses, a free span from [i] to [i + 1] for each odd i, and
// the space between two spans taken by blocks; [0] and [gCircularLastIndex] are the heap's ends.
s32* gpCircularInfoBlocks;
s32  gCircularLastIndex;        // the table's last index (odd: 3 when the heap is one span)
s32  gCircularCursor;           // a span's index: MEMTYPE_LOW searches up from it, MEMTYPE_HIGH and
                                // MEMTYPE_HIGH_BEST_FIT down from the span below it
s32  gCircularMaxIndex;         // the table's last usable index (0x403: 1028 entries)
s32  gCircularUnused;           // set to 0x302 by Circular_CreateBuffer and never read
u8*  gpCircularBuffer;          // the heap
s32  gCircularBufferSize;       // the heap's size in bytes
TTypeAcces gCircularCurrentMemType;     // what the EA Sports library's blocks come from
s32  gCircularRecordedSize;     // the bytes taken while the recorder is on
u8   gCircularRecorderOn;       // the recorder is on: blocks taken and given back change the count

static int Circular_FindNextFreeBloc(int MemSize);
static int Circular_FindPreviousFreeBloc(int MemSize);
static int Circular_FindPreviousSmallestFreeBloc(int MemSize);
static int Circular_FindBloc(s32 Ptr);

// Gives the heap and its table back to the system heap, in the shut-down list main runs last.
void Circular_DestroyBuffer(void) {
    if (gpCircularBuffer != NULL) {
        Mem_FreeFunc(gpCircularBuffer);
    }
    if (gpCircularInfoBlocks != NULL) {
        Mem_FreeFunc(gpCircularInfoBlocks);
    }
    gpCircularBuffer = NULL;
    gpCircularInfoBlocks = NULL;
}

// Empty: main calls it first, before its start-up list.
void Circular_InitOnce(void) {
}

// Empty: main's start-up list calls it second, before Circular_CreateBuffer.
void Circular_PreInit(void) {
}

// Takes the heap from the system heap at start-up: first the table (1028 entries), then the heap,
// 20,028,336 bytes (0x14C7FC3 less the table and 0x1AD400 bytes left to the system heap, rounded
// down to 4), or 4 KB less each time the system heap cannot give that much. The printed size is a
// fixed number (144,888), not the heap's. Circular_InitBuffer then makes the heap one free span.
void Circular_CreateBuffer(void) {
    gpCircularBuffer = NULL;
    gpCircularInfoBlocks = NULL;
    gCircularMaxIndex = 0x403;
    gCircularUnused = 0x302;
    gpCircularInfoBlocks = Mem_AllocFunc((gCircularMaxIndex + 1) * sizeof(s32), 16, 1);
    printf("StaticMemory: %d\n", 0x235F8);
    gCircularBufferSize = (0x14C7FC3 - ((gCircularMaxIndex + 1) * sizeof(s32) + 0x1AD400)) & ~3;
    gpCircularBuffer = Mem_AllocFunc(gCircularBufferSize, 16, 1);
    if (gpCircularBuffer == NULL) {
        while (gpCircularBuffer == NULL) {
            gCircularBufferSize -= 0x1000;
            gpCircularBuffer = Mem_AllocFunc(gCircularBufferSize, 16, 1);
        }
    }
}

// Empties the heap: fills it with 0x77 and makes the table one free span over all of it ([0] and
// [3] the heap's ends, [1] to [2] the span), with the cursor on that span. Every block taken before
// is lost. The main loop runs it first in its once-only set-up and again each time a mode's
// systems start (gomainloop.c).
void Circular_InitBuffer(void) {
    s32 BufferSize;

    BufferSize = gCircularBufferSize;
    Mem_set(gpCircularBuffer, 0x77, BufferSize);
    gpCircularInfoBlocks[0] = (s32)gpCircularBuffer;
    gpCircularInfoBlocks[1] = (s32)gpCircularBuffer;
    gpCircularInfoBlocks[2] = (s32)(gpCircularBuffer + BufferSize);
    gpCircularInfoBlocks[3] = gpCircularInfoBlocks[2];
    gCircularLastIndex = 3;
    gCircularCursor = 1;
}

// Empty: the last call of the main loop's once-only set-up and of each mode's shut-down
// (gomainloop.c).
void Circular_CloseBuffer(void) {
}

// The first free span of at least MemSize bytes, searching up from the cursor to the top, then from
// the bottom up to the cursor: its table index (the span runs from [i] to [i + 1]), or -1 when none
// is big enough.
static int Circular_FindNextFreeBloc(int MemSize) {
    s32* pInfoBloc;
    int i;

    pInfoBloc = &gpCircularInfoBlocks[gCircularCursor];
    for (i = gCircularCursor; i < gCircularLastIndex; pInfoBloc += 2, i += 2) {
        if (pInfoBloc[1] - pInfoBloc[0] >= MemSize) {
            return i;
        }
    }
    pInfoBloc = &gpCircularInfoBlocks[1];
    for (i = 1; i < gCircularCursor; pInfoBloc += 2, i += 2) {
        if (pInfoBloc[1] - pInfoBloc[0] >= MemSize) {
            return i;
        }
    }
    return -1;
}

// The first free span of at least MemSize bytes, searching down from the span below the cursor to
// the bottom, then from the top down to the cursor: its table index, or -1 when none is big enough.
static int Circular_FindPreviousFreeBloc(int MemSize) {
    s32* pInfoBloc;
    int i;

    pInfoBloc = &gpCircularInfoBlocks[gCircularCursor - 2];
    for (i = gCircularCursor - 2; i > 0; pInfoBloc -= 2, i -= 2) {
        if (pInfoBloc[1] - pInfoBloc[0] >= MemSize) {
            return i;
        }
    }
    pInfoBloc = &gpCircularInfoBlocks[gCircularLastIndex - 2];
    for (i = gCircularLastIndex - 2; i >= gCircularCursor; pInfoBloc -= 2, i -= 2) {
        if (pInfoBloc[1] - pInfoBloc[0] >= MemSize) {
            return i;
        }
    }
    return -1;
}

// The smallest free span of at least MemSize bytes (an exact fit is taken at once; of equal ones,
// the last in the walk), walking the table in Circular_FindPreviousFreeBloc's order: its table
// index, or -1 when none is big enough.
static int Circular_FindPreviousSmallestFreeBloc(int MemSize) {
    s32* pInfoBloc;
    int i;
    int minFreeSize;
    int memIndex;
    int freeSize;

    minFreeSize = 0x40000000;
    memIndex = -1;
    pInfoBloc = &gpCircularInfoBlocks[gCircularCursor - 2];
    for (i = gCircularCursor - 2; i > 0; pInfoBloc -= 2, i -= 2) {
        freeSize = pInfoBloc[1] - pInfoBloc[0];
        if (freeSize == MemSize) {
            return i;
        }
        if (freeSize >= MemSize && freeSize <= minFreeSize) {
            memIndex = i;
            minFreeSize = freeSize;
        }
    }
    pInfoBloc = &gpCircularInfoBlocks[gCircularLastIndex - 2];
    for (i = gCircularLastIndex - 2; i >= gCircularCursor; pInfoBloc -= 2, i -= 2) {
        freeSize = pInfoBloc[1] - pInfoBloc[0];
        if (freeSize == MemSize) {
            return i;
        }
        if (freeSize >= MemSize && freeSize <= minFreeSize) {
            memIndex = i;
            minFreeSize = freeSize;
        }
    }
    return memIndex;
}

// Allocates MemSize bytes aligned to align (16 when below 2) from where TypeMem says; NULL for 0
// bytes or when nothing fits. When the system heap has no room MEMTYPE_LOW is tried; when this heap
// has none the system heap is, and its answer stands. A TypeMem not in TTypeAcces gives NULL.
// A block from this heap is cut with room for its alignment and an 8-byte header ([-2] the start
// and [-1] the size of the piece cut, which Circular_MemFreeFunc reads) and filled with 0x33 (low
// end) or 0x55 (high end). filename and lineNumber, the caller's source position, are not read.
void* Circular_MemAllocFunc(int MemSize, TTypeAcces TypeMem, int align, const char* filename,
                            int lineNumber) {
    s32 memAddr;
    int userSize;
    int CanRetry;
    int Nb;
    s32 padNeeded;
    s32* pInfoBloc;
    s32 Ptr;

    if (align < 2) {
        align = 16;
    }
    userSize = MemSize;
    if (MemSize == 0) {
        return NULL;
    }
    memAddr = 0;
    CanRetry = 1;
    for (;;) {
        switch (TypeMem) {
        case MEMTYPE_SYSTEM:
            memAddr = (s32)Mem_AllocFunc(MemSize, (u8)align, 1);
            if (memAddr == 0 && CanRetry) {
                TypeMem = MEMTYPE_LOW;
                CanRetry = 0;
                continue;
            }
            break;
        case MEMTYPE_LOW:
            MemSize += align + 7;
            if (MemSize % 4 != 0) {
                MemSize += 4 - MemSize % 4;
            }
            Nb = Circular_FindNextFreeBloc(MemSize);
            if (Nb < 0) {
                MemSize = userSize;
                TypeMem = MEMTYPE_SYSTEM;
                CanRetry = 0;
                continue;
            }
            gCircularCursor = Nb;
            Ptr = gpCircularInfoBlocks[Nb] + 8;
            padNeeded = Ptr & (align - 1);
            if (padNeeded != 0) {
                Ptr += align - padNeeded;
            }
            ((s32*)Ptr)[-1] = MemSize;
            ((s32*)Ptr)[-2] = gpCircularInfoBlocks[Nb];
            memAddr = Ptr;
            Mem_set((void*)memAddr, 0x33, userSize);
            gpCircularInfoBlocks[Nb] += MemSize;
            pInfoBloc = &gpCircularInfoBlocks[Nb];
            if (pInfoBloc[0] == pInfoBloc[1] && gCircularLastIndex > 3) {
                Mem_cpy(pInfoBloc, pInfoBloc + 2, (gCircularLastIndex - Nb - 1) * sizeof(s32));
                gCircularLastIndex -= 2;
            }
            break;
        case MEMTYPE_HIGH:
        case MEMTYPE_HIGH_BEST_FIT:
            MemSize += align + 7;
            if (MemSize % 4 != 0) {
                MemSize += 4 - MemSize % 4;
            }
            if ((u32)TypeMem == MEMTYPE_HIGH) {
                Nb = Circular_FindPreviousFreeBloc(MemSize);
            } else {
                Nb = Circular_FindPreviousSmallestFreeBloc(MemSize);
            }
            if (Nb < 0) {
                MemSize = userSize;
                TypeMem = MEMTYPE_SYSTEM;
                CanRetry = 0;
                continue;
            }
            gpCircularInfoBlocks[Nb + 1] -= MemSize;
            memAddr = gpCircularInfoBlocks[Nb + 1] + 8;
            padNeeded = memAddr & (align - 1);
            if (padNeeded != 0) {
                memAddr += align - padNeeded;
            }
            ((s32*)memAddr)[-1] = MemSize;
            ((s32*)memAddr)[-2] = gpCircularInfoBlocks[Nb + 1];
            Mem_set((void*)memAddr, 0x55, userSize);
            pInfoBloc = &gpCircularInfoBlocks[Nb];
            if (pInfoBloc[0] == pInfoBloc[1] && gCircularLastIndex > 3) {
                Mem_cpy(pInfoBloc, pInfoBloc + 2, (gCircularLastIndex - Nb - 1) * sizeof(s32));
                gCircularLastIndex -= 2;
                if (gCircularCursor >= Nb + 2) {
                    gCircularCursor -= 2;
                }
            }
            break;
        }
        break;
    }
    if (gCircularRecorderOn) {
        gCircularRecordedSize += MemSize;
    }
    return (void*)memAddr;
}

// The index of the last table entry at or below Ptr (a binary search of the sorted table; 0 when Ptr
// is below them all). Circular_MemFreeFunc passes a freed block's start to find the free spans on
// either side of it.
static int Circular_FindBloc(s32 Ptr) {
    int Begin;
    int End;
    int Mid;

    Begin = 0;
    End = gCircularLastIndex - 1;
    while (End - Begin > 1) {
        Mid = (Begin + End) / 2;
        if (gpCircularInfoBlocks[Mid] > Ptr) {
            End = Mid;
        } else {
            Begin = Mid;
        }
    }
    if (gpCircularInfoBlocks[End] <= Ptr) {
        return End;
    }
    return Begin;
}

// Frees a block: fills the piece it was cut from with 0x11 and gives it back to the table, joined to
// the free spans it touches (making one span of the two when it fills the space between them) or as
// a new span. A block from outside the heap goes back to the system heap.
void Circular_MemFreeFunc(void* Ptr) {
    int Nb;
    s32 Begin;
    s32 End;
    s32 memSize;
    s32* pInfoBloc;

    if ((s32)Ptr >= gpCircularInfoBlocks[0] && (s32)Ptr < gpCircularInfoBlocks[gCircularLastIndex]) {
        Begin = ((s32*)Ptr)[-2];
        memSize = ((s32*)Ptr)[-1];
        End = Begin + memSize;
        Nb = Circular_FindBloc(Begin);
        Mem_set((void*)Begin, 0x11, End - Begin);
        if (gCircularRecorderOn) {
            gCircularRecordedSize -= memSize;
        }
        pInfoBloc = &gpCircularInfoBlocks[Nb];
        if (Begin == pInfoBloc[0]) {
            if (End == pInfoBloc[1]) {
                // it fills the space between two spans (or a span and an end of the heap)
                if (Nb != 0 && Nb != gCircularLastIndex - 1) {
                    Mem_cpy(pInfoBloc, pInfoBloc + 2, (gCircularLastIndex - Nb - 1) * sizeof(s32));
                    gCircularLastIndex -= 2;
                    if (gCircularCursor >= Nb) {
                        gCircularCursor -= 2;
                    }
                } else if (Nb == 0) {
                    gpCircularInfoBlocks[1] = Begin;
                } else {
                    pInfoBloc[0] = End;
                }
            } else if (Nb > 0) {
                pInfoBloc[0] = End;
            } else {
                Mem_move(&gpCircularInfoBlocks[3], &gpCircularInfoBlocks[1],
                         gCircularLastIndex * sizeof(s32));
                gpCircularInfoBlocks[1] = Begin;
                gpCircularInfoBlocks[2] = End;
                gCircularLastIndex += 2;
                gCircularCursor += 2;
            }
        } else {
            s32* pInfoBlocEnd = pInfoBloc + 1;

            if (End == pInfoBloc[1]) {
                if (Nb + 1 < gCircularLastIndex) {
                    *pInfoBlocEnd = Begin;
                } else {
                    gpCircularInfoBlocks[gCircularLastIndex + 2] = gpCircularInfoBlocks[gCircularLastIndex];
                    gpCircularInfoBlocks[gCircularLastIndex] = Begin;
                    gpCircularInfoBlocks[gCircularLastIndex + 1] = End;
                    gCircularLastIndex += 2;
                }
            } else {
                Mem_move(pInfoBloc + 3, pInfoBlocEnd, (gCircularLastIndex - Nb) * sizeof(s32));
                gCircularLastIndex += 2;
                if (gCircularCursor > Nb) {
                    gCircularCursor += 2;
                }
                gpCircularInfoBlocks[Nb + 1] = Begin;
                gpCircularInfoBlocks[Nb + 2] = End;
            }
        }
    } else {
        Mem_FreeFunc(Ptr);
    }
}

// Sets what the EA Sports library's allocator TibExtMemAlloc takes its blocks from. It starts as
// MEMTYPE_SYSTEM; EASBio_InitOnce sets MEMTYPE_SYSTEM while the library starts, then MEMTYPE_LOW.
void Circular_SetCurrentMemType(TTypeAcces eType) {
    gCircularCurrentMemType = eType;
}

TTypeAcces Circular_GetCurrentMemType(void) {
    return gCircularCurrentMemType;
}

// Starts a new count of the bytes taken.
void Circular_ClearMemorySizeRecorder(void) {
    gCircularRecordedSize = 0;
}

void Circular_StartMemorySizeRecorder(void) {
    gCircularRecorderOn = 1;
}

void Circular_StopMemorySizeRecorder(void) {
    gCircularRecorderOn = 0;
}

// The bytes taken while the recorder was on since Circular_ClearMemorySizeRecorder: what
// Circular_MemAllocFunc added (a block's padded size, or the size asked for when the block came from
// the system heap or nothing fitted) less the pieces Circular_MemFreeFunc gave back to this heap.
s32 Circular_GetCurrentMemorySizeRecorder(void) {
    return gCircularRecordedSize;
}
