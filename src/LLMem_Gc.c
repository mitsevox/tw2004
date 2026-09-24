// LLMem_Gc.c (our name, after TW06's legacy/ll/xbox/llmem_xbox.c, which holds Mem_cpy): the
// engine's memory copy, fill and compare. Only the compare is decompiled so far.

#include "engine.h"

// memset: short fills byte by byte; longer ones align to 8 and store the byte pattern 32 and then
// 8 bytes at a time through a double. Draft: the original's loops count in ctr without unrolling.
void* fn_80005AE8(void* pDst, int nValue, u32 uLen) {
    u8* p;
    u32 n;
    u32 uFill;
    u8 b;
    union {
        u32 a[2];
        f64 d;
    } fill;

    b = nValue;
    if (uLen < 32) {
        p = pDst;
        n = uLen;
        if (n != 0) {
            do {
                *p++ = b;
            } while (--n);
        }
    } else {
        p = pDst;
        n = (8 - (uptr)p) & 7;
        if (n > 0) {
            do {
                *p++ = b;
                uLen--;
            } while (--n);
        }
        uFill = (b << 8) | b;
        uFill = (uFill << 16) | uFill;
        fill.a[0] = uFill;
        fill.a[1] = uFill;
        n = uLen >> 5;
        if (uLen > 31) {
            do {
                ((f64*)p)[0] = fill.d;
                uLen -= 32;
                ((f64*)p)[1] = fill.d;
                ((f64*)p)[2] = fill.d;
                ((f64*)p)[3] = fill.d;
                p += 32;
            } while (--n);
        }
        n = uLen >> 3;
        if (uLen > 7) {
            do {
                *(f64*)p = fill.d;
                p += 8;
                uLen -= 8;
            } while (--n);
        }
        n = uLen;
        if (n != 0) {
            do {
                *p++ = b;
            } while (--n);
        }
    }
    return pDst;
}

int fn_80005BC8(const void* pA, const void* pB, u32 uLen) {
    return memcmp(pA, pB, uLen);
}
