// hwsBurn.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the matched
// small functions.

#include "game_types.h"
#include "platform.h"

// ---- sweep code (not yet cleaned up) ----

void fn_801109F0(u8* p0, s32 p1, s32 p2);
void fn_801109FC(u8* p0, s32 p1, s32 p2);
void fn_80110A0C(u8* p0, s32 p1, s32 p2);
void fn_80110A1C(u8* p, s32 v);
void fn_80110A24(u8* p0, s32 p1);

void fn_801109F0(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x68) = p1;
    *(s32*)(p0 + 0x6C) = p2;
}

void fn_801109FC(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0xC)) + (p1 << 2)) = p2;
}

void fn_80110A0C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x10)) + (p1 << 2)) = p2;
}

void fn_80110A1C(u8* p, s32 v) {
    *(s32*)(p + 0x4) = v;
}

void fn_80110A24(u8* p0, s32 p1) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x1C)) + (p1 << 2)) = 1;
}

// ---- end of sweep code ----

// The bytes nCount items of nSize take, rounded up to nAlign (a power of two); 0 without p.
s32 fn_80110E74(void* p, s32 nCount, s32 nSize, s32 nAlign) {
    s32 n = 0;

    if (p != NULL) {
        n = nCount * nSize;
        n = nAlign + n;
        n = (n - 1) & ~(nAlign - 1);
    }
    return n;
}

// Copy nSize bytes of pSrc to pBase + *pOffset and move *pOffset past them, rounded up to nAlign.
// Gives where they went (NULL without pSrc).
void* fn_80110E98(u8* pBase, s32* pOffset, void* pSrc, s32 nSize, s32 nAlign) {
    void* pDst = NULL;
    s32 n;

    if (pSrc != NULL) {
        pDst = pBase + *pOffset;
        memcpy(pDst, pSrc, nSize);
        *pOffset += nSize;
        n = nAlign + *pOffset;
        *pOffset = (n - 1) & ~(nAlign - 1);
    }
    return pDst;
}
