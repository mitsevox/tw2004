// StaticMemory.c (our name, from the "StaticMemory: %d" message its set-up prints): the game's own
// heap. At start-up it takes one big block (about 20 MB, less while the system heap cannot give that
// much) and keeps a sorted table of its free spans. Blocks come from the low end of a span (first
// fit, from a moving cursor) or from the high end (first fit searching down from the cursor, or
// best fit); the 8 bytes before a block hold the start and size of the piece it was cut from. A
// block outside the big one came from the system heap and goes back there.
//
// port: the span table holds addresses as signed 32-bit integers, and the code compares and
// aligns them as integers; a 64-bit port needs a wider table.

#include "engine.h"

// Written in reverse address order (CodeWarrior lays out .sbss last-defined-first).
s32* lbl_80281BD4;                      // the span table: addresses, a free span from [i] to [i + 1]
                                        // for each odd i; [0] and the last one bound the heap
s32  lbl_80281BD0;                      // the table's last index
s32  lbl_80281BCC;                      // the cursor: where the next low-end search starts
s32  lbl_80281BC8;                      // the table's capacity, less one
s32  lbl_80281BC4;
u8*  lbl_80281BC0;                      // the heap's memory
s32  lbl_80281BBC;                      // the heap's size
s32  lbl_80281BB8;
s32  lbl_80281BB4;                      // bytes taken since StaticMem_ResetCount, while counting is on
u8   lbl_80281BB0;                      // count the bytes taken (StaticMem_StartCount / StaticMem_StopCount)

int printf(const char* pFmt, ...);          // MSL
int  StaticMem_FindFitUp(int nSize);
int  StaticMem_FindFitDown(int nSize);
int  StaticMem_FindBestFit(int nSize);
int  StaticMem_FindEntry(s32 nAddr);

// Gives the heap and its table back to the system.
void StaticMem_Shutdown(void) {
    if (lbl_80281BC0 != NULL) {
        fn_8009527C(lbl_80281BC0);
    }
    if (lbl_80281BD4 != NULL) {
        fn_8009527C(lbl_80281BD4);
    }
    lbl_80281BC0 = NULL;
    lbl_80281BD4 = NULL;
}

// Empty in this build; main calls it first, before the start-up list fn_80005520. Left unnamed: an
// empty body says nothing about what it was for.
void fn_800097C4(void) {
}

// Empty in this build; called early in the start-up list fn_80005520, before StaticMem_Init. Left
// unnamed: an empty body says nothing about what it was for.
void fn_800097C8(void) {
}

// Takes the heap from the system heap at start-up: first a span table of 1028 entries, then the
// heap itself, 20,028,336 bytes (0x14C7FC3 less the table and 0x1AD400 bytes left to the system
// heap, rounded down to 4) or 4 KB less each time the system heap cannot give that much. The
// printed size is a fixed number (144,888), not the heap's. StaticMem_Reset then makes the heap one
// free span.
void StaticMem_Init(void) {
    lbl_80281BC0 = NULL;
    lbl_80281BD4 = NULL;
    lbl_80281BC8 = 0x403;
    lbl_80281BC4 = 0x302;
    lbl_80281BD4 = fn_800951A0((lbl_80281BC8 + 1) * sizeof(s32), 16, 1);
    printf("StaticMemory: %d\n", 0x235F8);
    lbl_80281BBC = (0x14C7FC3 - ((lbl_80281BC8 + 1) * sizeof(s32) + 0x1AD400)) & ~3;
    lbl_80281BC0 = fn_800951A0(lbl_80281BBC, 16, 1);
    if (lbl_80281BC0 == NULL) {     // fake match: the loop's own test, first on the returned value
        while (lbl_80281BC0 == NULL) {
            lbl_80281BBC -= 0x1000;
            lbl_80281BC0 = fn_800951A0(lbl_80281BBC, 16, 1);
        }
    }
}

// Empties the heap: fills it with 0x77 and makes the table one free span over all of it ([0] and
// [3] the heap's ends, [1] to [2] the span), with the cursor on that span. Every block taken before
// is lost. Runs in the once-only set-up (fn_8006C720) and each time the per-mode systems start
// (fn_8006C7A8).
void StaticMem_Reset(void) {
    s32 nSize;

    nSize = lbl_80281BBC;
    Mem_set(lbl_80281BC0, 0x77, nSize);
    // port: addresses stored as integers (see the top of the file).
    lbl_80281BD4[0] = (s32)lbl_80281BC0;
    // port: as above.
    lbl_80281BD4[1] = (s32)lbl_80281BC0;
    // port: as above.
    lbl_80281BD4[2] = (s32)(lbl_80281BC0 + nSize);
    lbl_80281BD4[3] = lbl_80281BD4[2];
    lbl_80281BD0 = 3;
    lbl_80281BCC = 1;
}

// Empty in this build; runs last in the once-only set-up fn_8006C720 and in the per-mode shut-down
// fn_8006C854. Left unnamed: an empty body says nothing about what it was for.
void fn_80009918(void) {
}

// The first free span of at least nSize bytes, searching up from the cursor to the top, then from
// the bottom up to the cursor: its table index (the span runs from [i] to [i + 1]), or -1 when none
// is big enough. StaticMem_Alloc's mode 2.
int StaticMem_FindFitUp(int nSize) {
    s32* pSpan;
    int i;

    pSpan = &lbl_80281BD4[lbl_80281BCC];
    for (i = lbl_80281BCC; i < lbl_80281BD0; pSpan += 2, i += 2) {
        if (pSpan[1] - pSpan[0] >= nSize) {
            return i;
        }
    }
    pSpan = &lbl_80281BD4[1];
    for (i = 1; i < lbl_80281BCC; pSpan += 2, i += 2) {
        if (pSpan[1] - pSpan[0] >= nSize) {
            return i;
        }
    }
    return -1;
}

// The first free span of at least nSize bytes, searching down from the span below the cursor to the
// bottom, then from the top down to the cursor: its table index (the span runs from [i] to [i +
// 1]), or -1 when none is big enough. StaticMem_Alloc's mode 1.
int StaticMem_FindFitDown(int nSize) {
    s32* pSpan;
    int i;

    pSpan = &lbl_80281BD4[lbl_80281BCC - 2];
    for (i = lbl_80281BCC - 2; i > 0; pSpan -= 2, i -= 2) {
        if (pSpan[1] - pSpan[0] >= nSize) {
            return i;
        }
    }
    pSpan = &lbl_80281BD4[lbl_80281BD0 - 2];
    for (i = lbl_80281BD0 - 2; i >= lbl_80281BCC; pSpan -= 2, i -= 2) {
        if (pSpan[1] - pSpan[0] >= nSize) {
            return i;
        }
    }
    return -1;
}

// The smallest free span of at least nSize bytes (an exact fit is taken at once; of equal ones, the
// last in the walk), walking the table in StaticMem_FindFitDown's order: its table index, or -1
// when none is big enough. StaticMem_Alloc's mode 4.
int StaticMem_FindBestFit(int nSize) {
    s32* pSpan;
    int i;
    int nBestSpan;
    int nBest;
    int nSpan;

    nBestSpan = 0x40000000;
    nBest = -1;
    pSpan = &lbl_80281BD4[lbl_80281BCC - 2];
    for (i = lbl_80281BCC - 2; i > 0; pSpan -= 2, i -= 2) {
        nSpan = pSpan[1] - pSpan[0];
        if (nSpan == nSize) {
            return i;
        }
        if (nSpan >= nSize && nSpan <= nBestSpan) {
            nBest = i;
            nBestSpan = nSpan;
        }
    }
    pSpan = &lbl_80281BD4[lbl_80281BD0 - 2];
    for (i = lbl_80281BD0 - 2; i >= lbl_80281BCC; pSpan -= 2, i -= 2) {
        nSpan = pSpan[1] - pSpan[0];
        if (nSpan == nSize) {
            return i;
        }
        if (nSpan >= nSize && nSpan <= nBestSpan) {
            nBest = i;
            nBestSpan = nSpan;
        }
    }
    return nBest;
}

// Allocates nSize bytes aligned to nAlign (16 when below 2); NULL for 0 bytes or when nothing fits.
// nMode 0: the system heap; 2: the low end of the first fitting span searching up from the cursor
// (the cursor moves to it); 1: the high end of the first fitting span searching down from the
// cursor; 4: the high end of the smallest fitting span. When one heap has no room the other is
// tried once (mode 0 falls back to mode 2, the others to the system heap); any other mode returns
// NULL. A block from this heap is cut with room for its alignment and an 8-byte header ([-2] the
// start and [-1] the size of the piece cut, which StaticMem_Free reads), and filled with 0x33 (low
// end) or 0x55 (high end). pFile and nLine (the caller's source position) are not read in this
// build.
void* StaticMem_Alloc(int nSize, int nMode, int nAlign, const char* pFile, int nLine) {
    s32 nBlock;
    int nWanted;
    int bRetry;
    int i;
    s32 nMisalign;
    s32* pSpan;

    if (nAlign < 2) {
        nAlign = 16;
    }
    nWanted = nSize;
    if (nSize == 0) {
        return NULL;
    }
    nBlock = 0;
    bRetry = 1;
    for (;;) {
        switch (nMode) {
        case 0:
            // port: addresses as integers (see the top of the file).
            nBlock = (s32)fn_800951A0(nSize, (u8)nAlign, 1);
            if (nBlock == 0 && bRetry) {
                nMode = 2;
                bRetry = 0;
                continue;
            }
            break;
        case 2:
            nSize += nAlign + 7;
            if (nSize % 4 != 0) {
                nSize += 4 - nSize % 4;
            }
            i = StaticMem_FindFitUp(nSize);
            if (i < 0) {
                nSize = nWanted;
                nMode = 0;
                bRetry = 0;
                continue;
            }
            lbl_80281BCC = i;
            {
                // fake match: the block address in its own local, copied to nBlock after the header
                s32 nAddr = lbl_80281BD4[i] + 8;

                nMisalign = nAddr & (nAlign - 1);
                if (nMisalign != 0) {
                    nAddr += nAlign - nMisalign;
                }
                // port: as above.
                ((s32*)nAddr)[-1] = nSize;
                // port: as above.
                ((s32*)nAddr)[-2] = lbl_80281BD4[i];
                nBlock = nAddr;
            }
            // port: as above.
            Mem_set((void*)nBlock, 0x33, nWanted);
            lbl_80281BD4[i] += nSize;
            pSpan = &lbl_80281BD4[i];
            if (pSpan[0] == pSpan[1] && lbl_80281BD0 > 3) {
                Mem_cpy(pSpan, pSpan + 2, (lbl_80281BD0 - i - 1) * sizeof(s32));
                lbl_80281BD0 -= 2;
            }
            break;
        case 1:
        case 4:
            nSize += nAlign + 7;
            if (nSize % 4 != 0) {
                nSize += 4 - nSize % 4;
            }
            if ((u32)nMode == 1) {  // fake match: the original compares unsigned here (cmplwi)
                i = StaticMem_FindFitDown(nSize);
            } else {
                i = StaticMem_FindBestFit(nSize);
            }
            if (i < 0) {
                nSize = nWanted;
                nMode = 0;
                bRetry = 0;
                continue;
            }
            lbl_80281BD4[i + 1] -= nSize;
            nBlock = lbl_80281BD4[i + 1] + 8;
            nMisalign = nBlock & (nAlign - 1);
            if (nMisalign != 0) {
                nBlock += nAlign - nMisalign;
            }
            // port: as above.
            ((s32*)nBlock)[-1] = nSize;
            // port: as above.
            ((s32*)nBlock)[-2] = lbl_80281BD4[i + 1];
            // port: as above.
            Mem_set((void*)nBlock, 0x55, nWanted);
            pSpan = &lbl_80281BD4[i];
            if (pSpan[0] == pSpan[1] && lbl_80281BD0 > 3) {
                Mem_cpy(pSpan, pSpan + 2, (lbl_80281BD0 - i - 1) * sizeof(s32));
                lbl_80281BD0 -= 2;
                if (lbl_80281BCC >= i + 2) {
                    lbl_80281BCC -= 2;
                }
            }
            break;
        }
        break;
    }
    if (lbl_80281BB0) {
        lbl_80281BB4 += nSize;
    }
    // port: as above.
    return (void*)nBlock;
}

// The index of the last span-table entry at or below nAddr (a binary search of the sorted table; 0
// when nAddr is below them all). StaticMem_Free passes a freed block's start to find the free spans
// on either side of it.
int StaticMem_FindEntry(s32 nAddr) {
    int nLo;
    int nHi;
    int nMid;

    nLo = 0;
    nHi = lbl_80281BD0 - 1;
    while (nHi - nLo > 1) {
        nMid = (nLo + nHi) / 2;
        if (lbl_80281BD4[nMid] > nAddr) {
            nHi = nMid;
        } else {
            nLo = nMid;
        }
    }
    if (lbl_80281BD4[nHi] <= nAddr) {
        return nHi;
    }
    return nLo;
}

// Frees a block (filled with 0x11): merged into the free span it touches, or a new span. A block
// from outside the heap goes back to the system heap.
void StaticMem_Free(void* p) {
    int i;
    s32 nStart;
    s32 nEnd;
    s32 nSize;
    s32* pSpan;

    // port: addresses compared as integers (see the top of the file).
    if ((s32)p >= lbl_80281BD4[0] && (s32)p < lbl_80281BD4[lbl_80281BD0]) {
        nStart = ((s32*)p)[-2];
        nSize = ((s32*)p)[-1];
        nEnd = nStart + nSize;
        i = StaticMem_FindEntry(nStart);
        // port: as above.
        Mem_set((void*)nStart, 0x11, nEnd - nStart);
        if (lbl_80281BB0) {
            lbl_80281BB4 -= nSize;
        }
        pSpan = &lbl_80281BD4[i];
        if (nStart == pSpan[0]) {
            if (nEnd == pSpan[1]) {
                // it fills the gap between two spans: they become one
                if (i != 0 && i != lbl_80281BD0 - 1) {
                    Mem_cpy(pSpan, pSpan + 2, (lbl_80281BD0 - i - 1) * sizeof(s32));
                    lbl_80281BD0 -= 2;
                    if (lbl_80281BCC >= i) {
                        lbl_80281BCC -= 2;
                    }
                } else if (i == 0) {
                    lbl_80281BD4[1] = nStart;
                } else {
                    pSpan[0] = nEnd;
                }
            } else if (i > 0) {
                pSpan[0] = nEnd;
            } else {
                Mem_move(&lbl_80281BD4[3], &lbl_80281BD4[1], lbl_80281BD0 * sizeof(s32));
                lbl_80281BD4[1] = nStart;
                lbl_80281BD4[2] = nEnd;
                lbl_80281BD0 += 2;
                lbl_80281BCC += 2;
            }
        } else {
            s32* pEnd = pSpan + 1;

            if (nEnd == pSpan[1]) {
                if (i + 1 < lbl_80281BD0) {
                    *pEnd = nStart;
                } else {
                    lbl_80281BD4[lbl_80281BD0 + 2] = lbl_80281BD4[lbl_80281BD0];
                    lbl_80281BD4[lbl_80281BD0] = nStart;
                    lbl_80281BD4[lbl_80281BD0 + 1] = nEnd;
                    lbl_80281BD0 += 2;
                }
            } else {
                Mem_move(pSpan + 3, pEnd, (lbl_80281BD0 - i) * sizeof(s32));
                lbl_80281BD0 += 2;
                if (lbl_80281BCC > i) {
                    lbl_80281BCC += 2;
                }
                lbl_80281BD4[i + 1] = nStart;
                lbl_80281BD4[i + 2] = nEnd;
            }
        }
    } else {
        fn_8009527C(p);
    }
}

// Sets the StaticMem_Alloc mode (0 system heap, 1, 2 or 4 this heap) that the EA Sports library's
// allocator TibExtMemAlloc uses. It starts at 0; EASBio_InitOnce sets 0 while the library starts,
// then 2.
void StaticMem_SetMode(s32 n) {
    lbl_80281BB8 = n;
}

// The mode set by StaticMem_SetMode.
s32 StaticMem_GetMode(void) {
    return lbl_80281BB8;
}

// Starts a new count of the bytes taken.
void StaticMem_ResetCount(void) {
    lbl_80281BB4 = 0;
}

void StaticMem_StartCount(void) {
    lbl_80281BB0 = 1;
}

void StaticMem_StopCount(void) {
    lbl_80281BB0 = 0;
}

// The bytes taken while counting was on since StaticMem_ResetCount: the padded sizes
// StaticMem_Alloc gave out (a system-heap block counts its size) less the sizes StaticMem_Free took
// back from this heap (a system-heap block is not taken off).
s32 StaticMem_GetCount(void) {
    return lbl_80281BB4;
}
