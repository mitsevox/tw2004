// ska_shared.c (TW06's golf/animation/ska_shared.c): the skeletal animation code the golfer's
// character shares: blending animation clips into bone rotations (quaternions) and reading the
// clip banks, and byte-swapping and laying out a clip read from disc.

#include "game_types.h"
#include "core/goaram.h"
#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "endian.h"

// Blends rotation pA toward pB by fT into pOut (a slerp from pA to pOut after copying pB there);
// a non-zero result is renormalised.
void fn_8001FB00(f32* pA, f32* pB, f32* pOut, f32 fT) {
    fn_8001E85C(pB, pOut);
    fn_8000883C(pA, pOut, fT);
    if (0.0f != pOut[0] || 0.0f != pOut[1] || 0.0f != pOut[2] || 0.0f != pOut[3]) {
        Vec_Normalize(pOut, pOut);
    }
}

// Blends point pA toward pB by fT into pOut; w is 0.
void fn_8001FBA4(f32* pA, f32* pB, f32* pOut, f32 fT) {
    f32 fInv = 1.0f - fT;

    pOut[0] = pA[0] * fInv;
    pOut[1] = pA[1] * fInv;
    pOut[2] = pA[2] * fInv;
    pOut[0] = pB[0] * fT + pOut[0];
    pOut[1] = pB[1] * fT + pOut[1];
    pOut[2] = pB[2] * fT + pOut[2];
    pOut[3] = 0.0f;
}

// Expands a point packed as three u16s back into the range aRange gives per axis (min, max pairs).
void fn_8001FC0C(u16* pPacked, f32* aRange, f32* pOut) {
    pOut[0] = (pPacked[0] / 65535.0f) * (aRange[1] - aRange[0]) + aRange[0];
    pOut[1] = (pPacked[1] / 65535.0f) * (aRange[3] - aRange[2]) + aRange[2];
    pOut[2] = (pPacked[2] / 65535.0f) * (aRange[5] - aRange[4]) + aRange[4];
}

// Starts copying uSize bytes from ARAM address uAram to pDst; fn_8001FCD4 waits for it.
ARAMTransfer* fn_8001FCA8(u32 uAram, void* pDst, u32 uSize) {
    return fn_800B68B4(pDst, uAram, uSize);
}

void fn_8001FCD4(ARAMTransfer* pTransfer) {
    fn_800B67EC(pTransfer);
}

void fn_80021134(u16* pFrame, f32* pOut, s32 nBones, u32* pBits);
void fn_8002148C(u8* pFrame, f32* pOut, s32 nBones, u32* pBits, u16* aBase);
u8 fn_80020328(Clip* pClip, int nFrame, f32* pPose2, f32* pPose1, u8* pExtra);
f32 fn_80021A98(Clip* pClip, f32 fTime);

// Poses pPose from pClip at fTime: the two keys around it are decoded into two of the character's
// buffers (kept while they still hold them) and blended by where fTime falls between them. A time
// at or past the pending event n5CC is held there once.
void fn_8001FCF4(Character* pChar, Clip* pClip, SkelPose* pPose, u32* aBits, f32 fTime) {
    CharBuffer* pBuf;
    ClipTrack* pTrack;
    BonePose* pBone;
    f32* pRot;
    f32* pA10;
    f32* pA14;
    f32* pB10;
    f32* pB14;
    u8* pRange;
    u8* pKeys;
    u32 uFlags;
    f32 fPos;
    f32 fFrac;
    int nKey;
    int nNext;
    int i;
    int* pSlot;
    int* pFrame;
    u32 aTmp[4];
    f32 aA[4];
    f32 aB[4];
    int aSlot[2];
    int aFree[2];
    int aFrame[2];

    if (pChar != NULL && pClip->pEvents != NULL && pChar->n5CC >= 0) {
        if (fTime >= pClip->pEvents[pChar->n5CC].fTime) {
            fTime = pClip->pEvents[pChar->n5CC].fTime;
            pChar->n5CC = -1;
        }
    }
    if (pClip->pD8 != NULL) {
        pClip->fCC = fn_80021A98(pClip, fTime);
    }
    fn_8001E938(pPose->a0, 0x80);
    fn_8001E938(pPose->a10, 0x80);
    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    fPos = fTime / pClip->f10;
    nKey = (int)fPos;
    nNext = nKey + 1;
    fFrac = fPos - nKey;
    if (nKey > pClip->n0E) {
        fFrac = 0.0f;
        nNext = pClip->n0E;
        nKey = pClip->n0E;
    } else if (nNext > pClip->n0E) {
        nNext = pClip->n0E;
        fFrac = 0.0f;
    }

    // find the buffers already holding the two keys, and the ones free to take them
    aFrame[0] = nKey;
    aFrame[1] = nNext;
    aFree[1] = -1;
    aFree[0] = -1;
    aSlot[1] = -1;
    aSlot[0] = -1;
    for (i = 0; i < 4; i++) {
        if (i == 1 && nKey == nNext) {
            aSlot[1] = aSlot[0];
            break;
        }
        if (pChar->buffers[i].p04 == pClip) {
            if (pChar->buffers[i].n00 == nKey) {
                aSlot[0] = i;
            }
            if (pChar->buffers[i].n00 == nNext) {
                aSlot[1] = i;
            }
            if (pChar->buffers[i].n00 != nKey && pChar->buffers[i].n00 != nNext) {
                aFree[1] = aFree[0];
                aFree[0] = i;
            }
        } else if (aFree[0] < 0) {
            aFree[0] = i;
        } else if (aFree[1] < 0) {
            aFree[1] = i;
        }
    }

    // decode the keys not held yet
    pSlot = aSlot;
    pFrame = aFrame;
    for (i = 0; i < 2; i++) {
        if (*pSlot < 0) {
            *pSlot = aFree[0];
            aFree[0] = aFree[1];
            pChar->buffers[*pSlot].n00 = *pFrame;
            if (pChar->buffers[*pSlot].p04 != pClip) {
                pChar->buffers[*pSlot].p04 = pClip;
                pBuf = &pChar->buffers[*pSlot];
                pBuf->p0C = pBuf->pBuf;
                if (i == 1) {
                    Mem_cpy(pChar->buffers[aSlot[1]].p0C, pChar->buffers[aSlot[0]].p0C,
                            pClip->n60 * 16);
                    pChar->buffers[*pSlot].p10 = pChar->buffers[*pSlot].pBuf + pClip->n60 * 16;
                } else if (pClip->pE8 != NULL) {
                    fn_8001E938(aTmp, 0x80);
                    fn_80021134((u16*)pClip->pE8, (f32*)pChar->buffers[*pSlot].p0C, pClip->n60, aTmp);
                    pChar->buffers[*pSlot].p10 = pChar->buffers[*pSlot].pBuf + pClip->n60 * 16;
                } else {
                    pBuf->p10 = pBuf->pBuf;
                }
                pChar->buffers[*pSlot].p14 = pChar->buffers[*pSlot].p10 + pClip->n58 * 16;
                pChar->buffers[*pSlot].p18 = pChar->buffers[*pSlot].p14 + pClip->n5C * 16;
            }
            if (fn_80020328(pClip, *pFrame, (f32*)pChar->buffers[*pSlot].p10,
                            (f32*)pChar->buffers[*pSlot].p14, pChar->buffers[*pSlot].p18) == 0 &&
                aBits != NULL) {
                fn_8001E938(aBits, 0x80);
            }
            if (nKey == nNext) {
                aSlot[1] = aSlot[0];
                break;
            }
        }
        pFrame++;
        pSlot++;
    }

    pRot = (f32*)pChar->buffers[aSlot[0]].p0C;
    pA10 = (f32*)pChar->buffers[aSlot[0]].p10;
    pA14 = (f32*)pChar->buffers[aSlot[0]].p14;
    pB10 = (f32*)pChar->buffers[aSlot[1]].p10;
    pB14 = (f32*)pChar->buffers[aSlot[1]].p14;

    // keys kept in ARAM: fetch the tracks' ranges and keys, and point the tracks at them
    if (pClip->n4C != 0 && (pClip->uFlags & 4)) {
        // port: pEC and pF0 hold ARAM addresses here
        fn_8001FCD4(fn_8001FCA8((uptr)pClip->pEC, lbl_80281CCC, pClip->n50));
        pRange = lbl_80281CCC;
        fn_8001FCD4(fn_8001FCA8((uptr)pClip->pF0, lbl_80281CD0, pClip->n4C));
        pKeys = lbl_80281CD0;
        for (i = 0; i < pClip->n1C; i++) {
            pTrack = &((ClipTrack*)pClip->pD0)[i];
            if (pTrack->uFlags & 0x10) {
                pTrack->pKeys = (u16*)pKeys;
                pTrack->aRange = (f32*)pRange;
                pRange += 0x18;
                pKeys += pClip->nFrames * 6;
            } else {
                pTrack->pKeys = NULL;
                pTrack->aRange = NULL;
            }
        }
    }

    pBone = pPose->aBones;
    for (i = 0; i < pClip->n1C; i++) {
        pTrack = &((ClipTrack*)pClip->pD0)[i];
        uFlags = pTrack->uFlags;
        if (uFlags & 4) {
            fn_8001E85C(pRot, pBone->q0);
            pRot += 4;
        } else if (uFlags & 8) {
            if (uFlags & 0x20) {
                fn_8001FB00(pA10, pB10, pBone->q0, fFrac);
                pA10 += 4;
                pB10 += 4;
            } else {
                fn_8001FB00(pA14, pB14, pBone->q0, fFrac);
                pA14 += 4;
                pB14 += 4;
            }
        } else {
            pBone++;
            continue;
        }
        fn_8001EA34(pPose->a0, i);
        if (pTrack->pKeys != NULL) {
            fn_8001FC0C(pTrack->pKeys + nKey * 3, pTrack->aRange, aA);
            fn_8001FC0C(pTrack->pKeys + nNext * 3, pTrack->aRange, aB);
            fn_8001FBA4(aA, aB, pBone->v10, fFrac);
            fn_8001EA34(pPose->a10, i);
        }
        pBone++;
    }
    if (aBits != NULL) {
        fn_80021980(pPose->a0, pPose->a10, aBits, 0x80);
    }
}

// Decodes frame nFrame of pClip: its second-stream frame into pPose2 (fn_8002148C) and its
// first-stream frame into pPose1 (fn_80021134), fetching them from ARAM first when the clip's
// frames live there (then n28 bytes of the first-stream frame are also copied to pExtra).
// Always returns 1.
u8 fn_80020328(Clip* pClip, int nFrame, f32* pPose2, f32* pPose1, u8* pExtra) {
    ARAMTransfer* pTransfer = NULL;
    u8* pFrame;

    if (pClip->n36 != 0) {
        if (pClip->uFlags & 4) {
            // port: pE4 holds an ARAM address here
            fn_8001FCD4(fn_8001FCA8((uptr)pClip->pE4 + pClip->n8E * nFrame, lbl_80281CC4, pClip->n8E));
            pTransfer = NULL;
            pFrame = lbl_80281CC4;
            if (pClip->n0A != 0) {
                pTransfer = fn_8001FCA8(pClip->uAram + pClip->n8C * nFrame * 2, lbl_80281CC8, pClip->n8C * 2);
            }
        } else {
            pFrame = pClip->pE4 + pClip->n8E * nFrame;
        }
        fn_8002148C(pFrame, pPose2, pClip->n58, (u32*)pClip->pF8, (u16*)pClip->pE0);
    }
    if (pClip->n0A != 0) {
        if (pClip->uFlags & 4) {
            if (pTransfer == NULL) {
                pTransfer = fn_8001FCA8(pClip->uAram + pClip->n8C * nFrame * 2, lbl_80281CC8, pClip->n8C * 2);
            }
            fn_8001FCD4(pTransfer);
            pFrame = lbl_80281CC8;
            if (pClip->pF0 != NULL) {
                Mem_cpy(pExtra, pFrame + pClip->n2A, pClip->n28);
            }
        } else {
            pFrame = (u8*)pClip->uAram + pClip->n8C * nFrame * 2;
        }
        fn_80021134((u16*)pFrame, pPose1, pClip->n5C, (u32*)pClip->pFC);
    }
    return 1;
}

// Samples pClip's BlendClip at fTime (held to its key range) into pOut's six values, blending the
// two keys around it. Returns 0 when the clip has none.
int fn_800204A0(Clip* pClip, f32* pOut, f32 fTime) {
    BlendClip* pBlend = pClip->pD8;
    f32 fPos;
    f32 fFrac;
    int nKey;
    int nNext;

    if (pBlend != NULL) {
        if (fTime < pBlend->f08) {
            fTime = pBlend->f08;
        } else if (fTime > pBlend->f0C) {
            fTime = pBlend->f0C;
        }
        fPos = (fTime - pBlend->f08) / pBlend->f04;
        nKey = (int)fPos;
        nNext = nKey + 1;
        fFrac = fPos - nKey;
        if (nKey > 19) {
            fFrac = 0.0f;
            nNext = 19;
            nKey = 19;
        } else if (nNext > 19) {
            nNext = 19;
            fFrac = 0.0f;
        }
        pOut[2] = fFrac * (pClip->pD8->aKeys[nNext].a[2] - pClip->pD8->aKeys[nKey].a[2]) +
                  pClip->pD8->aKeys[nKey].a[2];
        pOut[1] = fFrac * (pClip->pD8->aKeys[nNext].a[1] - pClip->pD8->aKeys[nKey].a[1]) +
                  pClip->pD8->aKeys[nKey].a[1];
        pOut[0] = fFrac * (pClip->pD8->aKeys[nNext].a[0] - pClip->pD8->aKeys[nKey].a[0]) +
                  pClip->pD8->aKeys[nKey].a[0];
        pOut[3] = fFrac * (pClip->pD8->aKeys[nNext].a[3] - pClip->pD8->aKeys[nKey].a[3]) +
                  pClip->pD8->aKeys[nKey].a[3];
        pOut[4] = fFrac * (pClip->pD8->aKeys[nNext].a[4] - pClip->pD8->aKeys[nKey].a[4]) +
                  pClip->pD8->aKeys[nKey].a[4];
        pOut[5] = fFrac * (pClip->pD8->aKeys[nNext].a[5] - pClip->pD8->aKeys[nKey].a[5]) +
                  pClip->pD8->aKeys[nKey].a[5];
        return 1;
    }
    return 0;
}

// The sixth value of pClip's BlendClip at fTime, blending the two keys around it (the first or
// last key's outside the key range); 0 when the clip has none.
f32 fn_800205F8(Clip* pClip, f32 fTime) {
    BlendClip* pBlend = pClip->pD8;
    f32 fPos;
    f32 fFrac;
    int nKey;
    int nNext;

    if (pBlend != NULL) {
        if (fTime <= pBlend->f08) {
            return pBlend->aKeys[0].a[5];
        }
        if (fTime >= pBlend->f0C) {
            return pBlend->aKeys[19].a[5];
        }
        fPos = (fTime - pBlend->f08) / pBlend->f04;
        nKey = (int)fPos;
        nNext = nKey + 1;
        fFrac = fPos - nKey;
        if (nKey > 19) {
            fFrac = 0.0f;
            nNext = 19;
            nKey = 19;
        } else if (nNext > 19) {
            nNext = 19;
            fFrac = 0.0f;
        }
        return fFrac * (pClip->pD8->aKeys[nNext].a[5] - pClip->pD8->aKeys[nKey].a[5]) +
               pClip->pD8->aKeys[nKey].a[5];
    }
    return 0.0f;
}

// Lays out a clip's data after its tracks: the pE8 and pE0 blocks from pC8 (rounded up to 16
// bytes), then the first frame stream (in ARAM at uAram when uAram is not 0), the second, and the
// tracks' ranges and keys. Tracks only get their key pointers when the frames stay in memory.
void fn_800206C8(Clip* pClip, u32 uAram) {
    u8* p = pClip->pC8;
    ClipTrack* pTrack;
    int i;
    u16* pKeys;
    f32* aRange;

    if ((uptr)p & 15) {
        p = (u8*)(((uptr)p & ~15) + 16);
    }
    if (pClip->n40 != 0) {
        pClip->pE8 = p;
        p += pClip->n40;
    } else {
        pClip->pE8 = NULL;
    }
    if (pClip->n3C != 0) {
        pClip->pE0 = p;
        p += pClip->n3C;
    } else {
        pClip->pE0 = NULL;
    }
    if (uAram != 0) {
        // port: an ARAM address, not a pointer; the field holds either
        p = (u8*)uAram;
        pClip->uFlags |= 4;
    } else {
        pClip->uFlags &= ~4;
    }
    pClip->uAram = (uptr)p;
    p += pClip->n38;
    if (pClip->n04 != 0) {
        pClip->pE4 = p;
        p += pClip->n04;
    } else {
        pClip->pE4 = NULL;
    }
    if (pClip->n4C != 0) {
        pClip->pEC = p;
        p += pClip->n50;
        pClip->pF0 = p;
    } else {
        pClip->pF0 = NULL;
        pClip->pEC = NULL;
    }
    if (uAram == 0) {
        pKeys = (u16*)pClip->pF0;
        aRange = (f32*)pClip->pEC;
        for (i = 0; i < pClip->n1C; i++) {
            pTrack = (ClipTrack*)pClip->pD0 + i;
            if (pTrack->uFlags & 0x10) {
                pTrack->pKeys = pKeys;
                pKeys += pClip->nFrames * 3;
                pTrack->aRange = aRange;
                aRange += 6;
            } else {
                pTrack->pKeys = NULL;
                pTrack->aRange = NULL;
            }
        }
    } else {
        for (i = 0; i < pClip->n1C; i++) {
            pTrack = (ClipTrack*)pClip->pD0 + i;
            pTrack->pKeys = NULL;
            pTrack->aRange = NULL;
        }
    }
}

// Byte-swaps a clip's frame streams, ranges and keys in place (the clip is laid out in memory).
void fn_80020858(Clip* pClip) {
    u8* pSrc;

    pSrc = (u8*)pClip->uAram;
    fn_80076158(&pSrc, pSrc, pClip->n38, 2);
    if (pClip->n04 != 0) {  // tests n04 but swaps the n3C bytes at pE0
        pSrc = pClip->pE0;
        fn_80076158(&pSrc, pSrc, pClip->n3C, 2);
    }
    if (pClip->n40 != 0) {
        pSrc = pClip->pE8;
        fn_80076158(&pSrc, pSrc, pClip->n40, 2);
    }
    if (pClip->n4C != 0) {
        pSrc = pClip->pEC;
        fn_80076158(&pSrc, pSrc, pClip->n50, 4);
        pSrc = pClip->pF0;
        fn_80076158(&pSrc, pSrc, pClip->n4C, 2);
    }
}

// Byte-swaps a clip's 0x100-byte header in place.
void fn_8002091C(Clip* pClip) {
    void* pSrc = pClip;
    void* pDst = pClip;
    SwapField aHeader[57] = {
        { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 },
        { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 16, 4 }, { 12, 4 }, { 2, 2 }, { 2, 2 }, { 8, -8 }, { 8, 4 }, { 16, 1 },
        { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
    };

    fn_8001F08C(&pSrc, &pDst, aHeader, 57, 1);
}

// Byte-swaps nCount of a clip's events (four words each) in place.
void fn_80020984(ClipEvent* pEvents, int nCount) {
    SwapField aEvent[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pEvents;
    pDst = pEvents;
    fn_8001F08C(&pSrc, &pDst, aEvent, 4, nCount);
}

// Byte-swaps a clip's BlendClip in place: its four header words, then its 20 keys.
void fn_80020A20(BlendClip* pBlend) {
    SwapField aHeader[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aKey[6] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pBlend;
    pDst = pBlend;
    fn_8001F08C(&pSrc, &pDst, aHeader, 4, 1);
    fn_8001F08C(&pSrc, &pDst, aKey, 6, 20);
}

// Byte-swaps nCount 16-byte records (four words each) in place.
void fn_80020B2C(void* pRecords, int nCount) {
    SwapField aRecord[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pRecords;
    pDst = pRecords;
    fn_8001F08C(&pSrc, &pDst, aRecord, 4, nCount);
}

// Byte-swaps the clip read from disc at p in place and sets its pointers as it goes: header, events,
// BlendClip and tracks, then the data blocks after them (fn_800206C8's layout, all in memory),
// its pF4 library (fn_8001F110) and two bit arrays of 2 * n1C bits (pF8, pFC).
void fn_80020BC8(u8* p) {
    Clip* pClip = (Clip*)p;
    u8* pSrc;
    int nBytes;

    fn_8002091C(pClip);
    p += sizeof(Clip);
    if (pClip->nEvents != 0) {
        fn_80020984((ClipEvent*)p, pClip->nEvents);
        p += pClip->nEvents * sizeof(ClipEvent);
    }
    if (pClip->uFlags & 2) {
        fn_80020A20((BlendClip*)p);
        p += sizeof(BlendClip);
    }
    fn_80020B2C(p, pClip->n1C);
    pClip->pD0 = p;
    p += pClip->n2C;
    pClip->pC4 = p;
    pClip->pC8 = p;
    if ((uptr)p & 15) {
        p = (u8*)(((uptr)p & ~15) + 16);
    }
    if (pClip->n40 != 0) {
        pSrc = p;
        p += pClip->n40;
        fn_80076158(&pSrc, pSrc, pClip->n40, 2);
    }
    if (pClip->n3C != 0) {
        pSrc = p;
        p += pClip->n3C;
        fn_80076158(&pSrc, pSrc, pClip->n3C, 2);
    }
    pSrc = p;
    pClip->uAram = (uptr)p;
    p += pClip->n38;
    fn_80076158(&pSrc, pSrc, pClip->n38, 2);
    if (pClip->n04 != 0) {
        pClip->pE4 = p;
        p += pClip->n04;
    }
    if (pClip->n4C != 0) {
        pSrc = p;
        pClip->pEC = p;
        fn_80076158(&pSrc, pSrc, pClip->n50, 4);
        p += pClip->n50;
        pClip->pF0 = p;
        pSrc = p;
        fn_80076158(&pSrc, p, pClip->n4C, 2);
        p += pClip->n4C;
    }
    if (pClip->n64 != 0) {
        pClip->pF4 = p;
        fn_8001F110((MtaLib*)pClip->pF4, NULL);
        p += pClip->n64;
    } else {
        pClip->pF4 = NULL;
    }
    nBytes = (pClip->n1C * 2 + 31) / 32 * 4;
    pClip->pF8 = p;
    pSrc = p;
    fn_80076158(&pSrc, p, nBytes, 4);
    p += nBytes;
    pClip->pFC = p;
    pSrc = p;
    fn_80076158(&pSrc, p, nBytes, 4);
}

// Takes the clip at pData rounded up to nAlign bytes, byte-swaps it in place and lays it out with
// its frames in memory; *pu30 gets its u30 when pu30 is not NULL.
Clip* fn_80020DD4(u8* p, u32* pu30, u32 nAlign) {
    Clip* pClip;
    u8* pSrc;
    u32 uPad;
    int nBytes;

    uPad = nAlign - ((uptr)p & (nAlign - 1));
    if (uPad == nAlign) {
        uPad = 0;
    }
    pClip = (Clip*)(p + uPad);
    fn_8002091C(pClip);
    pClip->pC0 = p;
    p = (u8*)pClip + sizeof(Clip);
    if (pClip->nEvents != 0) {
        pClip->pEvents = (ClipEvent*)p;
        fn_80020984((ClipEvent*)p, pClip->nEvents);
        p += pClip->nEvents * sizeof(ClipEvent);
    }
    if (pClip->uFlags & 2) {
        pClip->pD8 = (BlendClip*)p;
        fn_80020A20((BlendClip*)p);
        p += sizeof(BlendClip);
    }
    pClip->pD0 = p;
    fn_80020B2C(p, pClip->n1C);
    p += pClip->n2C;
    pClip->pC4 = p;
    p += pClip->n54;
    pClip->pC8 = pClip->pC4;
    fn_800206C8(pClip, 0);
    fn_80020858(pClip);
    if (pClip->n64 != 0) {
        pClip->pF4 = p;
        fn_8001F110((MtaLib*)pClip->pF4, NULL);
        p += pClip->n64;
    } else {
        pClip->pF4 = NULL;
    }
    nBytes = (pClip->n1C * 2 + 31) / 32 * 4;
    pClip->pF8 = p;
    pSrc = p;
    fn_80076158(&pSrc, p, nBytes, 4);
    p += nBytes;
    pClip->pFC = p;
    pSrc = p;
    fn_80076158(&pSrc, p, nBytes, 4);
    if (pu30 != NULL) {
        *pu30 = pClip->u30;
    }
    return pClip;
}

// Links a clip already in our byte order in place: its events and BlendClip follow the header,
// then its tracks; fn_800206C8 lays out the rest (the first frame stream in ARAM at uAram when it
// is not 0). Its pF4 library (n64 bytes) is linked too.
Clip* fn_80020F60(Clip* pClip, u32 uAram) {
    u8* p = (u8*)pClip + sizeof(Clip);

    pClip->pC0 = pClip;
    if (pClip->nEvents != 0) {
        pClip->pEvents = (ClipEvent*)p;
        p += pClip->nEvents * sizeof(ClipEvent);
    }
    if (pClip->uFlags & 2) {
        pClip->pD8 = (BlendClip*)p;
        p += sizeof(BlendClip);
    }
    pClip->pD0 = p;
    p += pClip->n2C;
    pClip->pC4 = p;
    pClip->pC8 = pClip->pC4;
    fn_800206C8(pClip, uAram);
    if (pClip->n64 != 0) {
        fn_8001F578((MtaLib*)pClip->pF4);
    }
    return pClip;
}

// The rotation quaternion (x, y, z, w) of the angles fX, fY and fZ (radians).
void fn_80020FF8(f32* pOut, f32 fX, f32 fY, f32 fZ) {
    f32 fHalfX;
    f32 fHalfZ;
    f32 fHalfY;
    f32 fSinX;
    f32 fCosX;
    f32 fSinY;
    f32 fCosY;
    f32 fSinZ;
    f32 fCosZ;
    f32 fCosXY;

    fHalfX = 0.5f * fX;
    fSinX = fn_800095F0(fHalfX);
    fCosX = fn_80009638(fHalfX);
    fHalfZ = 0.5f * fZ;
    fSinZ = fn_800095F0(fHalfZ);
    fCosZ = fn_80009638(fHalfZ);
    fHalfY = 0.5f * fY;
    fSinY = fn_800095F0(fHalfY);
    fCosY = fn_80009638(fHalfY);
    fCosXY = fCosX * fCosY;
    pOut[3] = fCosZ * fCosXY - fSinZ * (fSinX * fSinY);
    pOut[0] = fSinZ * (fCosX * fSinY) + fSinX * (fCosZ * fCosY);
    pOut[1] = fSinZ * (-fCosY * fSinX) + fSinY * (fCosZ * fCosX);
    pOut[2] = fSinZ * fCosXY + fSinY * (fCosZ * fSinX);
}

// Decodes nBones bone rotations packed as u16 angles (0x10000 to a turn) from pFrame into pOut's
// quaternions (4 floats each). Two bits per bone in pBits give its kind: one angle about z (1), about
// x (2) or about y (3), or three angles (0). While lbl_80281CC0 is clear the z and y angles and the
// last two of three angles are negated (fn_80021978 sets it).
void fn_80021134(u16* p, f32* pOut, s32 nBones, u32* pBits) {
    int nBit;   // fake match: 2 * i kept in its own counter for the second bit (i + i gives the first)
    int i;
    u64 uKind;  // fake match: a 64-bit switch value (the asm compares register pairs)

    for (i = 0, nBit = 0; i < nBones; pOut += 4, nBit += 2, i++) {
        uKind = 0;
        if (fn_8001E9CC(pBits, i + i)) {
            uKind = 1;
        }
        if (fn_8001E9CC(pBits, nBit + 1)) {
            uKind |= 2;
        }
        switch (uKind) {
        case 1:
            if (lbl_80281CC0) {
                fn_800093AC(TWOPI * p[0] / 65536.0f, pOut);
            } else {
                fn_800093AC(-(TWOPI * p[0]) / 65536.0f, pOut);
            }
            p += 1;
            break;
        case 3:
            if (lbl_80281CC0) {
                fn_80009410(TWOPI * p[0] / 65536.0f, pOut);
            } else {
                fn_80009410(-(TWOPI * p[0]) / 65536.0f, pOut);
            }
            p += 1;
            break;
        case 2:
            fn_80009474(-(TWOPI * p[0]) / 65536.0f, pOut);
            p += 1;
            break;
        case 0:
        default:
            if (lbl_80281CC0) {
                fn_80020FF8(pOut, TWOPI * p[2] / 65536.0f, -(TWOPI * p[1]) / 65536.0f,
                            -(TWOPI * p[0]) / 65536.0f);
            } else {
                fn_80020FF8(pOut, TWOPI * p[2] / 65536.0f, TWOPI * p[1] / 65536.0f, TWOPI * p[0] / 65536.0f);
            }
            p += 3;
            break;
        }
    }
}

// Like fn_80021134, for frames stored as one byte per angle: each angle is the bone's base angle
// in aBase (three u16 angles per bone) minus the byte times 16 (same units).
void fn_8002148C(u8* p, f32* pOut, s32 nBones, u32* pBits, u16* aBase) {
    int nBit;   // fake match: as in fn_80021134
    int i;
    u64 uKind;  // fake match: a 64-bit switch value (the asm compares register pairs)

    for (i = 0, nBit = 0; i < nBones; aBase += 3, pOut += 4, nBit += 2, i++) {
        uKind = 0;
        if (fn_8001E9CC(pBits, i + i)) {
            uKind = 1;
        }
        if (fn_8001E9CC(pBits, nBit + 1)) {
            uKind |= 2;
        }
        switch (uKind) {
        case 1:
            if (lbl_80281CC0) {
                fn_800093AC(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f, pOut);
            } else {
                fn_800093AC(-(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            }
            p += 1;
            break;
        case 3:
            if (lbl_80281CC0) {
                fn_80009410(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f, pOut);
            } else {
                fn_80009410(-(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            }
            p += 1;
            break;
        case 2:
            fn_80009474(-(TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            p += 1;
            break;
        case 0:
        default:
            if (lbl_80281CC0) {
                fn_80020FF8(pOut, TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[2] << 4) / 65536.0f,
                            -(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[1] << 4) / 65536.0f),
                            -(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f));
            } else {
                fn_80020FF8(pOut, TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[2] << 4) / 65536.0f,
                            TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[1] << 4) / 65536.0f,
                            TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f);
            }
            p += 3;
            break;
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80021978(u8 v) {
    lbl_80281CC0 = v;
}

// aOut = aA | aB over bit arrays of nBits bits.
void fn_80021980(u32* aA, u32* aB, u32* aOut, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aOut[i] = aA[i] | aB[i];
    }
}

// fn_800205F8's value at fTime as a fraction of the last key's.
f32 fn_80021A98(Clip* pClip, f32 fTime) {
    BlendClip* pBlend = pClip->pD8;

    if (pBlend != NULL) {
        return fn_800205F8(pClip, fTime) / pBlend->aKeys[19].a[5];
    }
    return 0.0f;
}

// ---- end of sweep code ----
