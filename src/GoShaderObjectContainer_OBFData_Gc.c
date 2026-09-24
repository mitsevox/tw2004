// GoShaderObjectContainer_OBFData_Gc.c (EA's name, from its asserts; also in EA's 2002 source
// tree): partly decompiled; the sweep code below is the matched small functions.

#include "game.h"
#include "ball.h"
#include "psmgr.h"
#include "obfdata.h"
#include "core/startup.h"

void fn_80055C24(int n);        // Ball.c
void fn_800B24E0(f32 f);        // shadow.c

void fn_8006FB10(void);
void fn_8006FCC4(u32 uSeed);
void fn_8006FCC8(void);
u8   fn_8006FCCC(void);         // a flag (fn_8006F650 tests it as a byte)
void fn_8006FCD4(void);
void fn_8006FCD8(void);

// Picks lbl_802811F0's flags for the hole: bit 0 in split screen, bit 4 in a strong wind on
// courses 0, 1, 2 and 6, then by the game option nC: a roll over the course's lbl_80188900 choices
// (mode 1, and mode 2, which keeps a choice for a rolled number of calls while f18 rises then
// falls by 0.3), bit 1 (mode 3) or bit 0 (modes 0 and 4). Not on course 4's hole 18.
void fn_8006F650(void) {
    u32 uSeed;

    if (gSession.nSplitScreen || (gSession.uFlags & 0x4000)) {
        lbl_802811F0->uFlags |= 1;
    } else {
        if (Wind_Get(NULL) > 6.0f && gSession.options.nWind != 0) {
            if (Game_GetCourse() == 0 || Game_GetCourse() == 1 || Game_GetCourse() == 2 ||
                Game_GetCourse() == 6) {
                lbl_802811F0->uFlags |= 0x10;
            }
        }
        if (lbl_802811F0->b1C) {
            lbl_802811F0->uFlags |= 2;
        } else if (Game_GetCourse() != 4 || fn_80015464() != 17) {
            switch (gSession.options.nC) {
            case 0:
            case 4:
                lbl_802811F0->uFlags |= 1;
                break;
            case 2: {
                f32 fSum;
                f32 fRoll;
                f32 fMin;
                f32 fRange;
                s32 nMax;
                int i;

                if (lbl_802811F0->n10 >= lbl_802811F0->n0C || lbl_802811F0->b14) {
                    fSum = 0.0f;
                    fRoll = Rand_Float(1);
                    lbl_802811F0->n08 = 0;
                    for (i = 0; fRoll > fSum; i++) {
                        fSum += lbl_80188900[Game_GetCourse()][i].fShare;
                        if (fRoll <= fSum) {
                            lbl_802811F0->uFlags |= 1 << i;
                            lbl_802811F0->n08 = i;
                            break;
                        }
                    }
                    fMin = lbl_80188900[Game_GetCourse()][i].nMin;
                    nMax = lbl_80188900[Game_GetCourse()][i].nMax;
                    fRange = nMax - lbl_80188900[Game_GetCourse()][i].nMin;
                    lbl_802811F0->n0C = Rand_Float(1) * fRange + fMin;
                    lbl_802811F0->b14 = 0;
                    lbl_802811F0->n10 = 1;
                    lbl_802811F0->f18 = 0.3f;
                } else {
                    lbl_802811F0->n10++;
                    lbl_802811F0->uFlags |= 1 << lbl_802811F0->n08;
                    if (lbl_802811F0->n0C % 2 == 0) {
                        if (lbl_802811F0->n10 <= lbl_802811F0->n0C / 2) {
                            lbl_802811F0->f18 += 0.3f;
                        } else if (lbl_802811F0->n10 != lbl_802811F0->n0C / 2 + 1) {
                            lbl_802811F0->f18 -= 0.3f;
                        }
                    } else if (lbl_802811F0->n10 <= lbl_802811F0->n0C / 2 + 1) {
                        lbl_802811F0->f18 += 0.3f;
                    } else {
                        lbl_802811F0->f18 -= 0.3f;
                    }
                }
                break;
            }
            case 1: {
                f32 fSum;
                f32 fRoll;
                int i;

                fSum = 0.0f;
                fRoll = Rand_Float(1);
                for (i = 0; fRoll > fSum; i++) {
                    fSum += lbl_80188900[Game_GetCourse()][i].fShare;
                    if (fRoll <= fSum) {
                        lbl_802811F0->uFlags |= 1 << i;
                        lbl_802811F0->n08 = i;
                        break;
                    }
                }
                lbl_802811F0->f18 = 0.75f * Rand_Float(1) + 0.25f;
                break;
            }
            case 3:
                lbl_802811F0->uFlags |= 2;
                lbl_802811F0->f18 = 0.75f * Rand_Float(1) + 0.25f;
                break;
            case 5:
            case 6:
                break;
            }
            lbl_802811F0->b14 = 0;
        }
    }
    if (fn_8006FCCC()) {
        fn_8006FCC8();
    } else {
        uSeed = Rand_Next(1);
        fn_8000B1D4(1, uSeed);
        fn_8006FCC4(uSeed);
    }
    fn_8006FB10();
}

// Applies lbl_802811F0's flags: bit 0 calls fn_800B24E0 with 1; bit 1 starts effects 0 (with f18
// kept to 0.1..1), 2 and 1 and calls fn_80055C24 with whether f18 is under 0.5; bit 4 starts
// effect 3.
void fn_8006FB10(void) {
    f32 fAmount;

    if (lbl_802811F0->uFlags & 1) {
        fn_800B24E0(1.0f);
    }
    if (lbl_802811F0->uFlags & 2) {
        fAmount = lbl_802811F0->f18 < 0.1f ? 0.1f : (lbl_802811F0->f18 > 1.0f ? 1.0f : lbl_802811F0->f18);
        fn_800A2A80(0, &fAmount, 2);
        fn_800A2A80(2, NULL, 2);
        fn_800A2A80(1, NULL, 2);
        if (lbl_802811F0->f18 < 0.5f) {
            fn_80055C24(1);
        } else {
            fn_80055C24(0);
        }
    }
    if (lbl_802811F0->uFlags & 0x10) {
        fn_800A2A80(3, NULL, 2);
    }
}

// Undoes fn_8006FB10: keeps the flags in u04, stops the effects and clears bits 0, 1, 2 and 4.
void fn_8006FBF8(void) {
    lbl_802811F0->b1C = 0;
    lbl_802811F0->u04 = lbl_802811F0->uFlags;
    if (lbl_802811F0->uFlags & 1) {
        lbl_802811F0->uFlags &= ~1;
    }
    if (lbl_802811F0->uFlags & 2) {
        fn_800A2B34(0);
        fn_800A2B34(2);
        fn_800A2B34(1);
        fn_80055C24(2);
        lbl_802811F0->uFlags &= ~2;
    }
    if (lbl_802811F0->uFlags & 4) {
        lbl_802811F0->uFlags &= ~4;
    }
    if (lbl_802811F0->uFlags & 0x10) {
        fn_800A2B34(3);
        lbl_802811F0->uFlags &= ~0x10;
    }
}

// ---- sweep code (not yet cleaned up) ----

// uSeed: unused (the stub takes the seed fn_8006F650 hands it)
void fn_8006FCC4(u32 uSeed) {
}

void fn_8006FCC8(void) {
}

u8 fn_8006FCCC(void) {
    return 0;
}

void fn_8006FCD4(void) {
}

void fn_8006FCD8(void) {
}

// ---- end of sweep code ----

// Copies pChunk's data into a new 32-byte aligned buffer (flushed for the graphics chip) and keeps
// it in pData by the chunk's kind, with the chunk's n02.
void fn_8006FCDC(OBFData* pData, OBFChunk* pChunk) {
    u8 nKind;
    u32 uSize;
    u8* pBuf;

    uSize = pChunk->n02 * pChunk->n04;
    pBuf = fn_80009B34((uSize + 31) & ~31, 2, 32, "GoShaderObjectContainer_OBFData_Gc.c", 0x41);
    nKind = pChunk->nKind;
    Mem_cpy(pBuf, pChunk->aData, uSize);
    DCFlushRange(pBuf, uSize);
    switch (nKind) {
    case 2:
        pData->aBufs[pChunk->nIndex] = pBuf;
        pData->an20[pChunk->nIndex] = pChunk->n02;
        break;
    case 0:
        pData->pBuf0 = pBuf;
        pData->n2C = pChunk->n02;
        break;
    case 1:
        pData->pBuf1 = pBuf;
        pData->n2A = pChunk->n02;
        break;
    case 3:
        pData->pBuf3 = pBuf;
        pData->n2E = pChunk->n02;
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006FDCC(void);
void fn_8006FDD0(void);
void fn_80074DA8();
void fn_8007524C();
void fn_8006FED4(u8* p0, s32 p1);
void fn_8006FF08(u8* p0);

void fn_8006FDCC(void) {
}

void fn_8006FDD0(void) {
}

// ---- end of sweep code ----

// Loads each chunk of pList into pData.
void fn_8006FDD4(OBFData* pData, OBFChunkList* pList) {
    s32 i;

    for (i = 0; i < pList->nChunks; i++) {
        fn_8006FCDC(pData, pList->apChunks[i]);
    }
}

// Frees pData's buffers.
void fn_8006FE44(OBFData* pData) {
    s32 i;

    for (i = 0; i < 5; i++) {
        if (pData->aBufs[i] != NULL) {
            fn_80009E70(pData->aBufs[i]);
        }
    }
    if (pData->pBuf1 != NULL) {
        fn_80009E70(pData->pBuf1);
    }
    if (pData->pBuf0 != NULL) {
        fn_80009E70(pData->pBuf0);
    }
    if (pData->pBuf3 != NULL) {
        fn_80009E70(pData->pBuf3);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006FED4(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_8006FF08(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

// ---- end of sweep code ----
