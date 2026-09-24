// ska_shared.c (TW06's golf/animation/ska_shared.c): the skeletal animation code the golfer's
// character shares: blending animation clips into bone rotations (quaternions) and reading the
// clip banks, and byte-swapping and laying out a clip read from disc.

#include "game_types.h"
#include "core/goaram.h"
#include "engine.h"
#include "character.h"
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

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80281CC0;

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
