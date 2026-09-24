// StaticMemory.c (our name, from the "StaticMemory: %d" message its set-up prints): the game's own
// heap. At start-up it takes one big block (about 20 MB, less while the system heap cannot give that
// much) and keeps a sorted table of its free spans. Blocks come from the low end of a span (first
// fit, from a moving cursor) or from the high end (last fit or best fit); each block starts with
// its span address and size. Anything outside the block came from the system heap and goes back
// there.
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
s32  lbl_80281BB4;                      // bytes taken since fn_8000A0BC, while counting is on
u8   lbl_80281BB0;                      // count the bytes taken (fn_8000A0C8 / fn_8000A0D4)

int printf(const char* pFmt, ...);          // MSL
int  fn_8000991C(int nSize);
int  fn_800099BC(int nSize);
int  fn_80009A60(int nSize);
int  fn_80009E04(s32 nAddr);

// Gives the heap and its table back to the system.
void fn_8000977C(void) {
    if (lbl_80281BC0 != NULL) {
        fn_8009527C(lbl_80281BC0);
    }
    if (lbl_80281BD4 != NULL) {
        fn_8009527C(lbl_80281BD4);
    }
    lbl_80281BC0 = NULL;
    lbl_80281BD4 = NULL;
}

void fn_800097C4(void) {
}

void fn_800097C8(void) {
}

// Takes the heap from the system heap, 4 KB less each time it does not fit.
void fn_800097CC(void) {
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

// Empties the heap (filled with 0x77): one free span over all of it.
void fn_8000989C(void) {
    s32 nSize;

    nSize = lbl_80281BBC;
    fn_80005AE8(lbl_80281BC0, 0x77, nSize);
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

void fn_80009918(void) {
}

// The first free span of at least nSize bytes, from the cursor up, then from the bottom up to the
// cursor; -1 when none is big enough.
int fn_8000991C(int nSize) {
    int i;

    for (i = lbl_80281BCC; i < lbl_80281BD0; i += 2) {
        if (lbl_80281BD4[i + 1] - lbl_80281BD4[i] >= nSize) {
            return i;
        }
    }
    for (i = 1; i < lbl_80281BCC; i += 2) {
        if (lbl_80281BD4[i + 1] - lbl_80281BD4[i] >= nSize) {
            return i;
        }
    }
    return -1;
}

// The same search downwards: from below the cursor to the bottom, then from the top down to it.
int fn_800099BC(int nSize) {
    int i;

    for (i = lbl_80281BCC - 2; i > 0; i -= 2) {
        if (lbl_80281BD4[i + 1] - lbl_80281BD4[i] >= nSize) {
            return i;
        }
    }
    for (i = lbl_80281BD0 - 2; i >= lbl_80281BCC; i -= 2) {
        if (lbl_80281BD4[i + 1] - lbl_80281BD4[i] >= nSize) {
            return i;
        }
    }
    return -1;
}

// The smallest free span of at least nSize bytes (an exact fit at once), searched in the order of
// fn_800099BC; -1 when none is big enough.
int fn_80009A60(int nSize) {
    int i;
    int nBestSpan;
    int nBest;
    int nSpan;

    nBestSpan = 0x40000000;
    nBest = -1;
    for (i = lbl_80281BCC - 2; i > 0; i -= 2) {
        nSpan = lbl_80281BD4[i + 1] - lbl_80281BD4[i];
        if (nSpan == nSize) {
            return i;
        }
        if (nSpan >= nSize && nSpan <= nBestSpan) {
            nBest = i;
            nBestSpan = nSpan;
        }
    }
    for (i = lbl_80281BD0 - 2; i >= lbl_80281BCC; i -= 2) {
        nSpan = lbl_80281BD4[i + 1] - lbl_80281BD4[i];
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

// Allocates nSize bytes aligned to nAlign (16 when less than 2). nMode 0 asks the system heap; 2
// takes the low end of the first fitting span from the cursor (and moves the cursor there); 1 the
// high end of the last fitting span, 4 of the best fitting one. When one way fails the other is
// tried once: the static heap for mode 0, the system heap for the others. New memory is filled
// with 0x33 (low end) or 0x55 (high end).
void* fn_80009B34(int nSize, int nMode, int nAlign, const char* pFile, int nLine) {
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
            i = fn_8000991C(nSize);
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
            fn_80005AE8((void*)nBlock, 0x33, nWanted);
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
                i = fn_800099BC(nSize);
            } else {
                i = fn_80009A60(nSize);
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
            fn_80005AE8((void*)nBlock, 0x55, nWanted);
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

// The index of the table entry at or below nAddr (a binary search).
int fn_80009E04(s32 nAddr) {
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
void fn_80009E70(void* p) {
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
        i = fn_80009E04(nStart);
        // port: as above.
        fn_80005AE8((void*)nStart, 0x11, nEnd - nStart);
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
                fn_80005884(&lbl_80281BD4[3], &lbl_80281BD4[1], lbl_80281BD0 * sizeof(s32));
                lbl_80281BD4[1] = nStart;
                lbl_80281BD4[2] = nEnd;
                lbl_80281BD0 += 2;
                lbl_80281BCC += 2;
            }
        } else if (nEnd == pSpan[1]) {
            if (i + 1 < lbl_80281BD0) {
                pSpan[1] = nStart;
            } else {
                lbl_80281BD4[lbl_80281BD0 + 2] = lbl_80281BD4[lbl_80281BD0];
                lbl_80281BD4[lbl_80281BD0] = nStart;
                lbl_80281BD4[lbl_80281BD0 + 1] = nEnd;
                lbl_80281BD0 += 2;
            }
        } else {
            fn_80005884(pSpan + 3, pSpan + 1, (lbl_80281BD0 - i) * sizeof(s32));
            lbl_80281BD0 += 2;
            if (lbl_80281BCC > i) {
                lbl_80281BCC += 2;
            }
            lbl_80281BD4[i + 1] = nStart;
            lbl_80281BD4[i + 2] = nEnd;
        }
    } else {
        fn_8009527C(p);
    }
}

void fn_8000A0AC(s32 n) {
    lbl_80281BB8 = n;
}

s32 fn_8000A0B4(void) {
    return lbl_80281BB8;
}

// Starts a new count of the bytes taken.
void fn_8000A0BC(void) {
    lbl_80281BB4 = 0;
}

void fn_8000A0C8(void) {
    lbl_80281BB0 = 1;
}

void fn_8000A0D4(void) {
    lbl_80281BB0 = 0;
}

s32 fn_8000A0E0(void) {
    return lbl_80281BB4;
}
