// LLMem_Gc.c (our name, after TW06's legacy/ll/xbox/llmem_xbox.c, which holds Mem_cpy): the
// engine's memory copy, fill and compare.

#include "engine.h"

// EA built this file optimized for size: its loops count in ctr and are not unrolled.
#pragma optimize_for_size on

// Copies n units of type T from pSrc to pDst: eight at a time (all eight read before any is
// written), then one at a time.
#define MEM_COPY_UNITS(T, n)                    \
    {                                           \
        T* pD = pDst;                           \
        const T* pS = pSrc;                     \
        while ((n) > 7) {                       \
            T t0, t1, t2, t3, t4, t5, t6, t7;   \
            t1 = pS[1];                         \
            t2 = pS[2];                         \
            t3 = pS[3];                         \
            t4 = pS[4];                         \
            t5 = pS[5];                         \
            t6 = pS[6];                         \
            t7 = pS[7];                         \
            t0 = pS[0];                         \
            pS += 8;                            \
            pD[0] = t0;                         \
            pD[1] = t1;                         \
            pD[2] = t2;                         \
            pD[3] = t3;                         \
            pD[4] = t4;                         \
            pD[5] = t5;                         \
            pD[6] = t6;                         \
            pD[7] = t7;                         \
            pD += 8;                            \
            (n) -= 8;                           \
        }                                       \
        while ((n) != 0) {                      \
            *pD++ = *pS++;                      \
            (n)--;                              \
        }                                       \
    }

// memcpy: in the widest unit (8, 4, 2 or 1 bytes) that both addresses and the length are aligned
// to.
void* Mem_cpy(void* pDst, const void* pSrc, u32 uLen) {
    if ((((uptr)pDst | (uptr)pSrc | uLen) & 7) == 0) {
        uLen >>= 3;
        MEM_COPY_UNITS(f64, uLen);
    } else if ((((uptr)pDst | (uptr)pSrc | uLen) & 3) == 0) {
        uLen >>= 2;
        MEM_COPY_UNITS(u32, uLen);
    } else if ((((uptr)pDst | (uptr)pSrc | uLen) & 1) == 0) {
        uLen >>= 1;
        MEM_COPY_UNITS(s16, uLen);
    } else {
        MEM_COPY_UNITS(u8, uLen);
    }
    return pDst;
}

// Copies n units of type T backwards, from the ends pSrcEnd and pDstEnd down: eight at a time (all
// eight read before any is written), then one at a time.
#define MEM_COPY_UNITS_BACK(T, n)               \
    {                                           \
        T* pD = pDstEnd;                        \
        const T* pS = pSrcEnd;                  \
        while ((n) > 7) {                       \
            T t0, t1, t2, t3, t4, t5, t6, t7;   \
            t1 = pS[-7];                        \
            t2 = pS[-6];                        \
            t3 = pS[-5];                        \
            t4 = pS[-4];                        \
            t5 = pS[-3];                        \
            t6 = pS[-2];                        \
            t7 = pS[-1];                        \
            t0 = *(pS -= 8);                    \
            pD[-8] = t0;                        \
            pD[-7] = t1;                        \
            pD[-6] = t2;                        \
            pD[-5] = t3;                        \
            pD[-4] = t4;                        \
            pD[-3] = t5;                        \
            pD[-2] = t6;                        \
            pD[-1] = t7;                        \
            pD -= 8;                            \
            (n) -= 8;                           \
        }                                       \
        while ((n) != 0) {                      \
            *--pD = *--pS;                      \
            (n)--;                              \
        }                                       \
    }

// memmove: Mem_cpy unless pSrc is below pDst and runs into it; then the copy runs backwards from
// the ends, in the widest unit the ends and the length are aligned to.
void* fn_80005884(void* pDst, const void* pSrc, u32 uLen) {
    void* pDstEnd;
    const void* pSrcEnd;

    // fake match: EA compares the two addresses as signed numbers (cmpw).
    // port: the casts cut 64-bit addresses; compare the pointers themselves.
    if ((s32)(uptr)pSrc >= (s32)(uptr)pDst || (const u8*)pSrc + uLen <= (u8*)pDst) {
        return Mem_cpy(pDst, pSrc, uLen);
    }
    pSrcEnd = (const u8*)pSrc + uLen;
    pDstEnd = (u8*)pDst + uLen;
    if ((((uptr)pDstEnd | (uptr)pSrcEnd | uLen) & 7) == 0) {
        uLen >>= 3;
        MEM_COPY_UNITS_BACK(f64, uLen);
    } else if ((((uptr)pDstEnd | (uptr)pSrcEnd | uLen) & 3) == 0) {
        uLen >>= 2;
        MEM_COPY_UNITS_BACK(u32, uLen);
    } else if ((((uptr)pDstEnd | (uptr)pSrcEnd | uLen) & 1) == 0) {
        uLen >>= 1;
        MEM_COPY_UNITS_BACK(s16, uLen);
    } else {
        MEM_COPY_UNITS_BACK(u8, uLen);
    }
    return pDst;
}

// memset: short fills byte by byte; longer ones align to 8 and store the byte pattern 32 and then
// 8 bytes at a time through a double.
void* fn_80005AE8(void* pDst, int nValue, u32 uLen) {
    u8* p;
    u8* pWide;
    int n;
    u32 uFill;
    u8 b;
    union {
        u32 a[2];
        f64 d;
    } fill;

    b = nValue;
    if (uLen < 32) {
        p = pDst;
        while (uLen != 0) {
            *p++ = b;
            uLen--;
        }
    } else {
        pWide = pDst;
        n = (8 - (uptr)pWide) & 7;
        while (n > 0) {
            *pWide++ = b;
            uLen--;
            n--;
        }
        uFill = (b << 8) | b;
        uFill = (uFill << 16) | (uFill & 0xFFFF);  // fake match: the mask gives EA's rlwimi
        fill.a[0] = uFill;
        fill.a[1] = uFill;
        while (uLen > 31) {
            ((f64*)pWide)[0] = fill.d;
            uLen -= 32;
            ((f64*)pWide)[1] = fill.d;
            ((f64*)pWide)[2] = fill.d;
            ((f64*)pWide)[3] = fill.d;
            pWide += 32;
        }
        while (uLen > 7) {
            *(f64*)pWide = fill.d;
            pWide += 8;
            uLen -= 8;
        }
        p = pWide;
        while (uLen != 0) {
            *p++ = b;
            uLen--;
        }
    }
    return pDst;
}

int fn_80005BC8(const void* pA, const void* pB, u32 uLen) {
    return memcmp(pA, pB, uLen);
}
