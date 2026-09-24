// LLMem_Gc.c (our name, after TW06's legacy/ll/xbox/llmem_xbox.c, which holds Mem_cpy): the
// engine's memory copy, fill and compare. Only the compare is exact so far.

#include "engine.h"

// Copies n units of type T from pSrc to pDst: eight at a time (all eight read before any is
// written), then one at a time.
#define MEM_COPY_UNITS(T, n)                    \
    {                                           \
        T* pD = pDst;                           \
        const T* pS = pSrc;                     \
        u32 k = (n) >> 3;                       \
        if ((n) > 7) {                          \
            do {                                \
                T t0 = pS[0];                   \
                T t1 = pS[1];                   \
                T t2 = pS[2];                   \
                T t3 = pS[3];                   \
                T t4 = pS[4];                   \
                T t5 = pS[5];                   \
                T t6 = pS[6];                   \
                T t7 = pS[7];                   \
                pS += 8;                        \
                pD[0] = t0;                     \
                pD[1] = t1;                     \
                pD[2] = t2;                     \
                pD[3] = t3;                     \
                pD[4] = t4;                     \
                pD[5] = t5;                     \
                pD[6] = t6;                     \
                pD[7] = t7;                     \
                pD += 8;                        \
                (n) -= 8;                       \
            } while (--k);                      \
        }                                       \
        if ((n) != 0) {                         \
            do {                                \
                *pD++ = *pS++;                  \
            } while (--(n));                    \
        }                                       \
    }

// memcpy: in the widest unit (8, 4, 2 or 1 bytes) that both addresses and the length are aligned
// to. Draft (76%): the original's loops count in ctr (mtctr before the n > 7 test), ours in a
// register; for/while forms get unrolled again, and GC/2.0 gives the same code.
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
        u32 k = (n) >> 3;                       \
        if ((n) > 7) {                          \
            do {                                \
                T t0, t1, t2, t3, t4, t5, t6, t7; \
                t1 = pS[-7];                    \
                t2 = pS[-6];                    \
                t3 = pS[-5];                    \
                t4 = pS[-4];                    \
                t5 = pS[-3];                    \
                t6 = pS[-2];                    \
                t7 = pS[-1];                    \
                t0 = *(pS -= 8);                \
                pD[-8] = t0;                    \
                pD[-7] = t1;                    \
                pD[-6] = t2;                    \
                pD[-5] = t3;                    \
                pD[-4] = t4;                    \
                pD[-3] = t5;                    \
                pD[-2] = t6;                    \
                pD[-1] = t7;                    \
                pD -= 8;                        \
                (n) -= 8;                       \
            } while (--k);                      \
        }                                       \
        if ((n) != 0) {                         \
            do {                                \
                *--pD = *--pS;                  \
            } while (--(n));                    \
        }                                       \
    }

// memmove: Mem_cpy unless pSrc is below pDst and runs into it; then the copy runs backwards from
// the ends, in the widest unit the ends and the length are aligned to. Draft (79%): the loops as
// Mem_cpy's, and the original compares pSrc >= pDst signed (cmpw), which C pointers do not.
void* fn_80005884(void* pDst, const void* pSrc, u32 uLen) {
    void* pDstEnd;
    const void* pSrcEnd;

    if (pSrc >= pDst || (const u8*)pSrc + uLen <= (u8*)pDst) {
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
