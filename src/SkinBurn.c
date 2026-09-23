// SkinBurn.c (EA's name, from its asserts): "burns" a skin: drops the unused parts of a skin's data,
// renumbers what is left and packs it into one allocation. Only partly decompiled.
#include "game_types.h"
#include "platform.h"

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 when there is no
// array (p is NULL).
s32 fn_801275F0(const void* p, s32 nCount, s32 nSize, s32 nAlign) {
    s32 n = 0;

    if (p != NULL) {
        n = (nCount * nSize + (nAlign - 1)) & ~(nAlign - 1);
    }
    return n;
}

// Copies nSize bytes of pSrc to pBase + *pOffset and moves *pOffset past them, rounded up to
// nAlign. Gives where the copy went (NULL, and nothing done, when pSrc is NULL).
void* fn_80127614(u8* pBase, s32* pOffset, const void* pSrc, s32 nSize, s32 nAlign) {
    void* pDst = NULL;

    if (pSrc != NULL) {
        pDst = pBase + *pOffset;
        memcpy(pDst, pSrc, nSize);
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return pDst;
}

// Takes nSize bytes at pBase + *pOffset the same way, without copying anything.
void* fn_801276A8(u8* pBase, s32* pOffset, s32 nSize, s32 nAlign) {
    void* p = NULL;

    if (nSize != 0) {
        p = pBase + *pOffset;
        *pOffset += nSize;
        *pOffset = (*pOffset + (nAlign - 1)) & ~(nAlign - 1);
    }
    return p;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80127140();
void fn_801272B4();
void fn_801276E4();
void fn_80127B10(s32 p0);

void fn_80127B10(s32 p0) {
    fn_80127140();
    fn_801272B4(p0);
    fn_801276E4(p0);
}

// ---- end of sweep code ----
