// ska_shared.c (TW06's golf/animation/ska_shared.c, beside ska.c and ska_util.c): EA's SKA
// skeletal-animation clips, the code the golfer's character shares. A clip (Clip) holds one track
// per bone, timed events, an optional BlendClip, and its frames as Euler angles in two streams (16
// bits per angle, and 8 bits per angle off a base angle), plus packed positions for the tracks that
// move. The loaders byte-swap a clip read from disc and point its fields at its blocks
// (SKA_LoadFromMem; or SKA_SwapClip then SKA_PatchMemory, with the frame data in ARAM).
// SKA_Update poses a skeleton from a clip at a time: it decodes the two keys around that time into
// quaternions (SKAUtil_ExpandSingleFrameToDest) and blends them. EA's names come from 007
// Everything or Nothing's SKA.cpp and SKA_Util.cpp (reference/007eon-ps2), TW07's
// SKA_Base.c / SKA_Util.c / UBitArray.h and TW06's map; the rest are read from the code.

#include "game_types.h"
#include "core/goaram.h"
#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "endian.h"

// Blends rotation pA toward pB by fT into pOut (a slerp from pA to pOut after copying pB there);
// a non-zero result is renormalised.
void SKA_BlendQuat(f32* pA, f32* pB, f32* pOut, f32 fT) {
    Quat_Copy(pB, pOut);
    Quat_Slerp(pA, pOut, fT);
    if (0.0f != pOut[0] || 0.0f != pOut[1] || 0.0f != pOut[2] || 0.0f != pOut[3]) {
        LLMath_Normalize(pOut, pOut);
    }
}

// Blends point pA toward pB by fT (0: pA, 1: pB) into pOut's x, y and z; pOut's w is set to 0.
void SKA_BlendVec3(f32* pA, f32* pB, f32* pOut, f32 fT) {
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
void SKA_Expand16BitPoint(u16* pPacked, f32* aRange, f32* pOut) {
    pOut[0] = (pPacked[0] / 65535.0f) * (aRange[1] - aRange[0]) + aRange[0];
    pOut[1] = (pPacked[1] / 65535.0f) * (aRange[3] - aRange[2]) + aRange[2];
    pOut[2] = (pPacked[2] / 65535.0f) * (aRange[5] - aRange[4]) + aRange[4];
}

// Starts copying uSize bytes from ARAM address uAram to pDst and returns the transfer;
// SKA_WaitAramRead waits for it.
ARAMTransfer* SKA_StartAramRead(u32 uAram, void* pDst, u32 uSize) {
    return GoARAM_CopyFromAram(pDst, uAram, uSize);
}

void SKA_WaitAramRead(ARAMTransfer* pTransfer) {
    GoARAM_WaitTransfer(pTransfer);
}

void SKAUtil_EulerAnglesToQTs16(u16* pFrame, f32* pOut, s32 nBones, u32* pBits);
void SKAUtil_EulerAnglesToQTs8(u8* pFrame, f32* pOut, s32 nBones, u32* pBits, u16* aBase);
u8 SKAUtil_ExpandSingleFrameToDest(Clip* pClip, int nFrame, f32* pPose2, f32* pPose1, u8* pExtra);
f32 SKA_GetBlendClipFraction(Clip* pClip, f32 fTime);

// This file's .sbss (character.h), in reverse address order as the compiler lays it out.
u8* gSKAAramKeys;
u8* gSKAAramRanges;
u8* gSKAAram16BitFrame;
u8* gSKAAram8BitFrame;
u8  gSKALeftHanded;

// Poses pPose from pClip at fTime (seconds). The keys before and after fTime (f10 apart; n0E is the
// last) are decoded by SKAUtil_ExpandSingleFrameToDest into two of the character's four key-frame
// buffers (Character.buffers, reused while they still hold that key of that clip), and each track
// is blended between them by where fTime falls: its rotation (track flag 4: the clip's fixed
// rotation, decoded from pFixedRots once per buffer; flag 8: the 8-bit stream's with flag 0x20,
// else the 16-bit stream's) and, for a track with keys, its position. pPose's a0 gets a bit for
// each bone rotated and a10 one for each bone moved; aBits, if not NULL, gets both. With the clip's
// data in ARAM (clip flag 4) the tracks' ranges and keys are fetched first. A time at or past the
// character's pending event nClampEvent is held there once (nClampEvent then -1); the clip's fCC is
// updated when it has a BlendClip.
void SKA_Update(Character* pChar, Clip* pClip, SkelPose* pPose, u32* aBits, f32 fTime) {
    // fake match: the declaration order (found by tools/matching/declsearch.py) sets the registers
    u8* pKeys;
    f32* pRot;
    ClipTrack* pTrack;
    int* pFree;
    f32* pA10;
    int nNext;
    f32* pA14;
    f32* pB10;
    int aSlot[2];
    f32* pB14;
    int j;
    int i;
    int aFree[2];
    f32 aA[4];
    u8* pRange;
    int nKey;
    f32 fPos;
    BonePose* pBone;
    f32 aB[4];
    u32 aTmp[8];               // fake match: size unknown; EA's frame has room for 8 (0x80 bits are used)
    u32 uFlags;
    f32 fFrac;
    int aFrame[2];

    if (pChar != NULL && pClip->pEvents != NULL && pChar->nClampEvent >= 0) {
        if (fTime >= pClip->pEvents[pChar->nClampEvent].fTime) {
            fTime = pClip->pEvents[pChar->nClampEvent].fTime;
            pChar->nClampEvent = -1;
        }
    }
    if (pClip->pD8 != NULL) {
        pClip->fCC = SKA_GetBlendClipFraction(pClip, fTime);
    }
    BitArray_ClearArray(pPose->a0, 0x80);
    BitArray_ClearArray(pPose->a10, 0x80);
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
    pFree = aFree;              // fake match: EA's search keeps aFree on the stack, not in registers
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
                pFree[1] = pFree[0];
                pFree[0] = i;
            }
        } else if (pFree[0] < 0) {
            pFree[0] = i;
        } else if (pFree[1] < 0) {
            pFree[1] = i;
        }
    }

    // decode the keys not held yet
    for (i = 0; i < 2; i++) {
        if (aSlot[i] < 0) {
            aSlot[i] = aFree[0];
            aFree[0] = aFree[1];
            pChar->buffers[aSlot[i]].n00 = aFrame[i];
            if (pChar->buffers[aSlot[i]].p04 != pClip) {
                pChar->buffers[aSlot[i]].p04 = pClip;
                pChar->buffers[aSlot[i]].p0C = pChar->buffers[aSlot[i]].pBuf;
                if (i == 1) {
                    Mem_cpy(pChar->buffers[aSlot[1]].p0C, pChar->buffers[aSlot[0]].p0C,
                            pClip->nFixedBones * 16);
                    pChar->buffers[aSlot[i]].p10 = pChar->buffers[aSlot[i]].pBuf + pClip->nFixedBones * 16;
                } else if (pClip->pFixedRots != NULL) {
                    BitArray_ClearArray(aTmp, 0x80);
                    SKAUtil_EulerAnglesToQTs16((u16*)pClip->pFixedRots, (f32*)pChar->buffers[aSlot[i]].p0C,
                                               pClip->nFixedBones, aTmp);
                    pChar->buffers[aSlot[i]].p10 = pChar->buffers[aSlot[i]].pBuf + pClip->nFixedBones * 16;
                } else {
                    pChar->buffers[aSlot[i]].p10 = pChar->buffers[aSlot[i]].pBuf;
                }
                pChar->buffers[aSlot[i]].p14 = pChar->buffers[aSlot[i]].p10 + pClip->n8BitBones * 16;
                pChar->buffers[aSlot[i]].p18 = pChar->buffers[aSlot[i]].p14 + pClip->n16BitBones * 16;
            }
            if (SKAUtil_ExpandSingleFrameToDest(pClip, aFrame[i], (f32*)pChar->buffers[aSlot[i]].p10,
                                                (f32*)pChar->buffers[aSlot[i]].p14,
                                                pChar->buffers[aSlot[i]].p18) == 0 &&
                aBits != NULL) {
                BitArray_ClearArray(aBits, 0x80);
            }
            if (nKey == nNext) {
                aSlot[1] = aSlot[0];
                break;
            }
        }
    }

    pRot = (f32*)pChar->buffers[aSlot[0]].p0C;
    pA10 = (f32*)pChar->buffers[aSlot[0]].p10;
    pA14 = (f32*)pChar->buffers[aSlot[0]].p14;
    pB10 = (f32*)pChar->buffers[aSlot[1]].p10;
    pB14 = (f32*)pChar->buffers[aSlot[1]].p14;

    // keys kept in ARAM: fetch the tracks' ranges and keys, and point the tracks at them
    if (pClip->n4C != 0 && (pClip->uFlags & 4)) {
        // port: pEC and pF0 hold ARAM addresses here
        SKA_WaitAramRead(SKA_StartAramRead((uptr)pClip->pEC, gSKAAramRanges, pClip->n50));
        pRange = gSKAAramRanges;
        SKA_WaitAramRead(SKA_StartAramRead((uptr)pClip->pF0, gSKAAramKeys, pClip->n4C));
        pKeys = gSKAAramKeys;
        for (j = 0; j < pClip->n1C; j++) {
            pTrack = &((ClipTrack*)pClip->pD0)[j];
            if (pTrack->uFlags & 0x10) {
                pTrack->pKeys = (u16*)pKeys;
                pKeys += pClip->nFrames * 6;
                pTrack->aRange = (f32*)pRange;
                pRange += 0x18;
            } else {
                pTrack->pKeys = NULL;
                pTrack->aRange = NULL;
            }
        }
    }

    for (i = 0; i < pClip->n1C; i++) {
        pTrack = &((ClipTrack*)pClip->pD0)[i];
        pBone = &pPose->aBones[i];
        uFlags = pTrack->uFlags;
        if (uFlags & 4) {
            Quat_Copy(pRot, pBone->q0);
            pRot += 4;
        } else if (uFlags & 8) {
            if (uFlags & 0x20) {
                SKA_BlendQuat(pA10, pB10, pBone->q0, fFrac);
                pA10 += 4;
                pB10 += 4;
            } else {
                SKA_BlendQuat(pA14, pB14, pBone->q0, fFrac);
                pA14 += 4;
                pB14 += 4;
            }
        } else {
            continue;
        }
        BitArray_SetBit(pPose->a0, i);
        if (pTrack->pKeys != NULL) {
            SKA_Expand16BitPoint(pTrack->pKeys + nKey * 3, pTrack->aRange, aA);
            SKA_Expand16BitPoint(pTrack->pKeys + nNext * 3, pTrack->aRange, aB);
            SKA_BlendVec3(aA, aB, pBone->v10, fFrac);
            BitArray_SetBit(pPose->a10, i);
        }
    }
    if (aBits != NULL) {
        BitArray_MergeArrayWithOr(pPose->a0, pPose->a10, aBits, 0x80);
    }
}

// Decodes frame nFrame of pClip into quaternions (4 floats per bone): its 8-bit frame (n8BitStride
// bytes per frame at p8BitFrames, n8BitBones bones, base angles pBaseAngles) into pPose2 with
// SKAUtil_EulerAnglesToQTs8 when b8BitFrames is not 0, and its 16-bit frame (n16BitStride halfwords
// per frame at uAram, n16BitBones bones) into pPose1 with SKAUtil_EulerAnglesToQTs16 when
// b16BitFrames is not 0. With the clip's data in ARAM (flag 4) the frames are first fetched into
// the staging buffers, and then, when pF0 is set, n28 bytes at offset n2A of the 16-bit frame are
// also copied to pExtra. Always returns 1.
u8 SKAUtil_ExpandSingleFrameToDest(Clip* pClip, int nFrame, f32* pPose2, f32* pPose1, u8* pExtra) {
    ARAMTransfer* pTransfer = NULL;
    u8* pFrame2;
    u16* pFrame1;
    int nSize;

    if (pClip->b8BitFrames != 0) {
        if (pClip->uFlags & 4) {
            // port: p8BitFrames holds an ARAM address here
            SKA_WaitAramRead(
                SKA_StartAramRead((uptr)pClip->p8BitFrames + pClip->n8BitStride * nFrame, gSKAAram8BitFrame,
                                  pClip->n8BitStride));
            pTransfer = NULL;
            pFrame2 = gSKAAram8BitFrame;
            if (pClip->b16BitFrames != 0) {
                pTransfer = SKA_StartAramRead(pClip->uAram + pClip->n16BitStride * nFrame * 2,
                                              gSKAAram16BitFrame, pClip->n16BitStride * 2);
            }
        } else {
            pFrame2 = pClip->p8BitFrames + pClip->n8BitStride * nFrame;
        }
        SKAUtil_EulerAnglesToQTs8(pFrame2, pPose2, pClip->n8BitBones, (u32*)pClip->p8BitAxes,
                                  (u16*)pClip->pBaseAngles);
    }
    if (pClip->b16BitFrames != 0) {
        if (pClip->uFlags & 4) {
            nSize = pClip->n16BitStride * 2;
            if (pTransfer == NULL) {
                pTransfer = SKA_StartAramRead(pClip->uAram + pClip->n16BitStride * nFrame * 2,
                                              gSKAAram16BitFrame, nSize);
            }
            SKA_WaitAramRead(pTransfer);
            pFrame1 = (u16*)gSKAAram16BitFrame;
            if (pClip->pF0 != NULL) {
                Mem_cpy(pExtra, gSKAAram16BitFrame + pClip->n2A, pClip->n28);
            }
        } else {
            // port: uAram holds a RAM address here
            pFrame1 = (u16*)(pClip->uAram + pClip->n16BitStride * nFrame * 2);
        }
        SKAUtil_EulerAnglesToQTs16(pFrame1, pPose1, pClip->n16BitBones, (u32*)pClip->p16BitAxes);
    }
    return 1;
}

// Samples pClip's BlendClip at fTime (held to its key range) into pOut's six values, blending the
// two keys around it. Returns 1, or 0 when the clip has none. SW_vStateBackSwing samples the
// backswing clip's at the top of the swing into Character.afSwingTop.
int SKA_SampleBlendClip(Clip* pClip, f32* pOut, f32 fTime) {
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
f32 SKA_SampleBlendClipProgress(Clip* pClip, f32 fTime) {
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

// Points a clip's data pointers at its blocks, laid out after its tracks: the fixed rotations
// pFixedRots and the 8-bit stream's base angles pBaseAngles from pC8 (rounded up to 16 bytes), then
// the 16-bit frame stream (uAram), the 8-bit frame stream (p8BitFrames) and the tracks' ranges and
// keys (pEC, pF0). With uAram not 0, everything from the 16-bit stream on is in ARAM from that
// address: flag 4 is set and every track's key pointers stay NULL (SKA_Update fetches them).
// Otherwise tracks with flag 0x10 get their keys and ranges.
void SKA_DistributePointers(Clip* pClip, u32 uAram) {
    u8* p = pClip->pC8;
    ClipTrack* pTrack;
    int i;
    u16* pKeys;
    f32* aRange;

    if ((uptr)p & 15) {
        p = (u8*)(((uptr)p & ~15) + 16);
    }
    if (pClip->nFixedRotBytes != 0) {
        pClip->pFixedRots = p;
        p += pClip->nFixedRotBytes;
    } else {
        pClip->pFixedRots = NULL;
    }
    if (pClip->nBaseAngleBytes != 0) {
        pClip->pBaseAngles = p;
        p += pClip->nBaseAngleBytes;
    } else {
        pClip->pBaseAngles = NULL;
    }
    if (uAram != 0) {
        // port: an ARAM address, not a pointer; the field holds either
        p = (u8*)uAram;
        pClip->uFlags |= 4;
    } else {
        pClip->uFlags &= ~4;
    }
    pClip->uAram = (uptr)p;
    p += pClip->n16BitBytes;
    if (pClip->n8BitBytes != 0) {
        pClip->p8BitFrames = p;
        p += pClip->n8BitBytes;
    } else {
        pClip->p8BitFrames = NULL;
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

// Byte-swaps a laid-out clip's data blocks in place (SKA_LoadFromMem): the 16-bit frame stream, the
// base angles pBaseAngles and fixed rotations pFixedRots (halfwords), the tracks' ranges (words)
// and keys (halfwords); the 8-bit frame stream needs none.
void SKA_SwapFrameData(Clip* pClip) {
    u8* pSrc;

    pSrc = (u8*)pClip->uAram;
    BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->n16BitBytes, 2);
    // tests n8BitBytes but swaps the nBaseAngleBytes bytes at pBaseAngles
    if (pClip->n8BitBytes != 0) {
        pSrc = pClip->pBaseAngles;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->nBaseAngleBytes, 2);
    }
    if (pClip->nFixedRotBytes != 0) {
        pSrc = pClip->pFixedRots;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->nFixedRotBytes, 2);
    }
    if (pClip->n4C != 0) {
        pSrc = pClip->pEC;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->n50, 4);
        pSrc = pClip->pF0;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->n4C, 2);
    }
}

// Byte-swaps a clip's 0x100-byte header in place.
void SKA_SwapHeader(Clip* pClip) {
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

    ByteSwap_Records(&pSrc, &pDst, aHeader, 57, 1);
}

// Byte-swaps nCount of a clip's events (four words each) in place.
void SKA_SwapEvents(ClipEvent* pEvents, int nCount) {
    SwapField aEvent[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pEvents;
    pDst = pEvents;
    ByteSwap_Records(&pSrc, &pDst, aEvent, 4, nCount);
}

// Byte-swaps a clip's BlendClip in place: its four header words, then its 20 keys.
void SKA_SwapBlendClip(BlendClip* pBlend) {
    SwapField aHeader[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aKey[6] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pBlend;
    pDst = pBlend;
    ByteSwap_Records(&pSrc, &pDst, aHeader, 4, 1);
    ByteSwap_Records(&pSrc, &pDst, aKey, 6, 20);
}

// Byte-swaps nCount of a clip's tracks (ClipTrack, four words each) in place.
void SKA_SwapTracks(void* pRecords, int nCount) {
    SwapField aRecord[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    void* pSrc;
    void* pDst;

    pSrc = pRecords;
    pDst = pRecords;
    ByteSwap_Records(&pSrc, &pDst, aRecord, 4, nCount);
}

// Byte-swaps the clip read from disc at p in place: header, events, BlendClip and tracks, then the
// data blocks after them (SKA_DistributePointers's layout, all in memory), its pMtaLib library
// (MtaLib_SwapAndLink) and two bit arrays of 2 * n1C bits. It sets pD0, pC4/pC8 and the later
// blocks' pointers (uAram, p8BitFrames, pEC, pF0, pMtaLib, p8BitAxes, p16BitAxes), not pEvents,
// pD8, pFixedRots or pBaseAngles. AnimLib_MergeOverlay runs it on an overlay library's clips,
// copies each one's header out and finishes it with SKA_PatchMemory.
void SKA_SwapClip(u8* p) {
    Clip* pClip = (Clip*)p;
    u8* pSrc;
    int nBytes;

    SKA_SwapHeader(pClip);
    p += sizeof(Clip);
    if (pClip->nEvents != 0) {
        SKA_SwapEvents((ClipEvent*)p, pClip->nEvents);
        p += pClip->nEvents * sizeof(ClipEvent);
    }
    if (pClip->uFlags & 2) {
        SKA_SwapBlendClip((BlendClip*)p);
        p += sizeof(BlendClip);
    }
    SKA_SwapTracks(p, pClip->n1C);
    pClip->pD0 = p;
    p += pClip->n2C;
    pClip->pC4 = p;
    pClip->pC8 = p;
    if ((uptr)p & 15) {
        p = (u8*)(((uptr)p & ~15) + 16);
    }
    if (pClip->nFixedRotBytes != 0) {
        pSrc = p;
        p += pClip->nFixedRotBytes;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->nFixedRotBytes, 2);
    }
    if (pClip->nBaseAngleBytes != 0) {
        pSrc = p;
        p += pClip->nBaseAngleBytes;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->nBaseAngleBytes, 2);
    }
    pSrc = p;
    pClip->uAram = (uptr)p;
    p += pClip->n16BitBytes;
    BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->n16BitBytes, 2);
    if (pClip->n8BitBytes != 0) {
        pClip->p8BitFrames = p;
        p += pClip->n8BitBytes;
    }
    if (pClip->n4C != 0) {
        pSrc = p;
        pClip->pEC = p;
        BYTESWAP_SWAPDATA(&pSrc, pSrc, pClip->n50, 4);
        p += pClip->n50;
        pClip->pF0 = p;
        pSrc = p;
        BYTESWAP_SWAPDATA(&pSrc, p, pClip->n4C, 2);
        p += pClip->n4C;
    }
    if (pClip->n64 != 0) {
        pClip->pMtaLib = p;
        MtaLib_SwapAndLink((MtaLib*)pClip->pMtaLib, NULL);
        p += pClip->n64;
    } else {
        pClip->pMtaLib = NULL;
    }
    nBytes = (pClip->n1C * 2 + 31) / 32 * 4;
    pClip->p8BitAxes = p;
    pSrc = p;
    BYTESWAP_SWAPDATA(&pSrc, p, nBytes, 4);
    p += nBytes;
    pClip->p16BitAxes = p;
    pSrc = p;
    BYTESWAP_SWAPDATA(&pSrc, p, nBytes, 4);
}

// Takes the clip at pData rounded up to align bytes (pC0 keeps pData), byte-swaps it in place and
// lays it out with its frame data in memory (SKA_DistributePointers with no ARAM address). Returns
// the clip; *iSize gets its u30 when iSize is not NULL. skalib.c and AnimStream.c run it on clips
// just read from disc.
Clip* SKA_LoadFromMem(u8* pData, u32* iSize, u32 align) {
    Clip* pSKA;
    u8* pSrc;
    u32 iOffset;
    int nBytes;

    iOffset = align - ((uptr)pData & (align - 1));
    if (iOffset == align) {
        iOffset = 0;
    }
    pSKA = (Clip*)(pData + iOffset);
    SKA_SwapHeader((Clip*)(pData + iOffset));  // fake match: the sum passed again, not pSKA (register order)
    pSKA->pC0 = pData;
    pData = (u8*)pSKA + sizeof(Clip);
    if (pSKA->nEvents != 0) {
        pSKA->pEvents = (ClipEvent*)pData;
        SKA_SwapEvents((ClipEvent*)pData, pSKA->nEvents);
        pData += pSKA->nEvents * sizeof(ClipEvent);
    }
    if (pSKA->uFlags & 2) {
        pSKA->pD8 = (BlendClip*)pData;
        SKA_SwapBlendClip((BlendClip*)pData);
        pData += sizeof(BlendClip);
    }
    pSKA->pD0 = pData;
    SKA_SwapTracks(pData, pSKA->n1C);
    pData += pSKA->n2C;
    pSKA->pC4 = pData;
    pData += pSKA->n54;
    pSKA->pC8 = pSKA->pC4;
    SKA_DistributePointers(pSKA, 0);
    SKA_SwapFrameData(pSKA);
    if (pSKA->n64 != 0) {
        pSKA->pMtaLib = pData;
        MtaLib_SwapAndLink((MtaLib*)pSKA->pMtaLib, NULL);
        pData += pSKA->n64;
    } else {
        pSKA->pMtaLib = NULL;
    }
    nBytes = (pSKA->n1C * 2 + 31) / 32 * 4;
    pSKA->p8BitAxes = pData;
    pSrc = pData;
    BYTESWAP_SWAPDATA(&pSrc, pData, nBytes, 4);
    pData += nBytes;
    pSKA->p16BitAxes = pData;
    pSrc = pData;
    BYTESWAP_SWAPDATA(&pSrc, pData, nBytes, 4);
    if (iSize != NULL) {
        *iSize = pSKA->u30;
    }
    return pSKA;
}

// Sets up in place a clip already in this machine's byte order (SKA_SwapClip): points pEvents, pD8
// and pD0 at the events, BlendClip and tracks after the header, lays out the rest with
// SKA_DistributePointers (its frame data in ARAM from uAram when that is not 0) and links its
// morph library pMtaLib when n64 is not 0. Returns pClip. AnimLib_MergeOverlay uses it for an overlay
// library's clips.
Clip* SKA_PatchMemory(Clip* pClip, u32 uAram) {
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
    SKA_DistributePointers(pClip, uAram);
    if (pClip->n64 != 0) {
        MtaLib_Link((MtaLib*)pClip->pMtaLib);
    }
    return pClip;
}

// Builds into pOut the rotation quaternion (x, y, z, w) of the Euler angles fX, fY and fZ
// (radians). The frame decoders pass a bone's three stored angles last first (p[2], p[1], p[0]).
// EA's 007 build has this step inline, as SKAUtil_EulerAnglesRPY(ex, ey, ez, Q).
void SKAUtil_EulerAnglesRPY(f32* pOut, f32 fX, f32 fY, f32 fZ) {
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
    fSinX = Math_Sin(fHalfX);
    fCosX = Math_Cos(fHalfX);
    fHalfZ = 0.5f * fZ;
    fSinZ = Math_Sin(fHalfZ);
    fCosZ = Math_Cos(fHalfZ);
    fHalfY = 0.5f * fY;
    fSinY = Math_Sin(fHalfY);
    fCosY = Math_Cos(fHalfY);
    fCosXY = fCosX * fCosY;
    pOut[3] = fCosZ * fCosXY - fSinZ * (fSinX * fSinY);
    pOut[0] = fSinZ * (fCosX * fSinY) + fSinX * (fCosZ * fCosY);
    pOut[1] = fSinZ * (-fCosY * fSinX) + fSinY * (fCosZ * fCosX);
    pOut[2] = fSinZ * fCosXY + fSinY * (fCosZ * fSinX);
}

// Decodes nBones bone rotations stored as u16 angles (0x10000 to a turn) from p into pOut's
// quaternions (4 floats each). Two bits per bone in the axis mask pBits give its kind: one angle
// about z (1), about x (2) or about y (3), or three angles (0). The angle passed on is negated for
// a lone x angle always, for a lone z or y angle while gSKALeftHanded (the left-handed flag) is
// clear, and for the second and third of three while it is set.
void SKAUtil_EulerAnglesToQTs16(u16* p, f32* pOut, s32 nBones, u32* pBits) {
    int nBit;   // fake match: 2 * i kept in its own counter for the second bit (the first is (u32)i * 2)
    int i;
    u64 uKind;  // fake match: a 64-bit switch value (the asm compares register pairs)

    for (i = 0, nBit = 0; i < nBones; pOut += 4, nBit += 2, i++) {
        uKind = 0;
        if (BitArray_TestBit(pBits, (u32)i * 2)) {
            uKind = 1;
        }
        if (BitArray_TestBit(pBits, nBit + 1)) {
            uKind |= 2;
        }
        switch (uKind) {
        case 1:
            if (gSKALeftHanded) {
                Legacy_Quat_BuildFromYaw(TWOPI * p[0] / 65536.0f, pOut);
            } else {
                Legacy_Quat_BuildFromYaw(-(TWOPI * p[0]) / 65536.0f, pOut);
            }
            p += 1;
            break;
        case 3:
            if (gSKALeftHanded) {
                Legacy_Quat_BuildFromPitch(TWOPI * p[0] / 65536.0f, pOut);
            } else {
                Legacy_Quat_BuildFromPitch(-(TWOPI * p[0]) / 65536.0f, pOut);
            }
            p += 1;
            break;
        case 2:
            Legacy_Quat_BuildFromRoll(-(TWOPI * p[0]) / 65536.0f, pOut);
            p += 1;
            break;
        case 0:
        default:
            if (gSKALeftHanded) {
                SKAUtil_EulerAnglesRPY(pOut, TWOPI * p[2] / 65536.0f, -(TWOPI * p[1]) / 65536.0f,
                            -(TWOPI * p[0]) / 65536.0f);
            } else {
                SKAUtil_EulerAnglesRPY(pOut, TWOPI * p[2] / 65536.0f, TWOPI * p[1] / 65536.0f, TWOPI * p[0]
                                       / 65536.0f);
            }
            p += 3;
            break;
        }
    }
}

// Like SKAUtil_EulerAnglesToQTs16, for frames stored as one byte per angle: each angle is the
// bone's base angle in aBase (three u16 angles per bone) minus the byte times 16 (same units).
void SKAUtil_EulerAnglesToQTs8(u8* p, f32* pOut, s32 nBones, u32* pBits, u16* aBase) {
    int nBit;   // fake match: as in SKAUtil_EulerAnglesToQTs16
    int i;
    u64 uKind;  // fake match: a 64-bit switch value (the asm compares register pairs)

    for (i = 0, nBit = 0; i < nBones; aBase += 3, pOut += 4, nBit += 2, i++) {
        uKind = 0;
        if (BitArray_TestBit(pBits, (u32)i * 2)) {
            uKind = 1;
        }
        if (BitArray_TestBit(pBits, nBit + 1)) {
            uKind |= 2;
        }
        switch (uKind) {
        case 1:
            if (gSKALeftHanded) {
                Legacy_Quat_BuildFromYaw(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f, pOut);
            } else {
                Legacy_Quat_BuildFromYaw(-(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            }
            p += 1;
            break;
        case 3:
            if (gSKALeftHanded) {
                Legacy_Quat_BuildFromPitch(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f, pOut);
            } else {
                Legacy_Quat_BuildFromPitch(-(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            }
            p += 1;
            break;
        case 2:
            Legacy_Quat_BuildFromRoll(-(TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f), pOut);
            p += 1;
            break;
        case 0:
        default:
            if (gSKALeftHanded) {
                SKAUtil_EulerAnglesRPY(pOut, TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[2] << 4)
                                       / 65536.0f,
                            -(TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[1] << 4) / 65536.0f),
                            -(TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f));
            } else {
                SKAUtil_EulerAnglesRPY(pOut, TWOPI * aBase[2] / 65536.0f - TWOPI * ((u32)(u16)p[2] << 4)
                                       / 65536.0f,
                            TWOPI * aBase[1] / 65536.0f - TWOPI * ((u32)(u16)p[1] << 4) / 65536.0f,
                            TWOPI * aBase[0] / 65536.0f - TWOPI * ((u32)(u16)p[0] << 4) / 65536.0f);
            }
            p += 3;
            break;
        }
    }
}

// Sets gSKALeftHanded, which makes the frame decoders (SKAUtil_EulerAnglesToQTs16 and
// SKAUtil_EulerAnglesToQTs8) mirror the angles for a left-handed golfer. Character_UpdateAnimation
// and Character_SetupForShot pass the model's bEE (Character_IsLeftHanded) before posing a clip.
void SKA_SetLeftHanded(u8 v) {
    gSKALeftHanded = v;
}

// Each bit of aOut is set where aA or aB has it (bit arrays of nBits bits).
void BitArray_MergeArrayWithOr(u32* aA, u32* aB, u32* aOut, u32 nBits) {
    u32 i;

    for (i = 0; i < (nBits + 31) >> 5; i++) {
        aOut[i] = aA[i] | aB[i];
    }
}

// How far along pClip's BlendClip fTime is: SKA_SampleBlendClipProgress's value over the last key's
// (0 when the clip has none). SKA_Update keeps it in the clip's fCC, which the swing reads as how
// far along the backswing is (Character.fBackswing).
f32 SKA_GetBlendClipFraction(Clip* pClip, f32 fTime) {
    BlendClip* pBlend = pClip->pD8;

    if (pBlend != NULL) {
        return SKA_SampleBlendClipProgress(pClip, fTime) / pBlend->aKeys[19].a[5];
    }
    return 0.0f;
}
