// LLMem_Gc.c: the engine's memory copy, move, fill and compare, its own memcpy, memmove, memset and
// memcmp.

#include "engine.h"

// Copies nUnits units of type T from pFrom to pTo: eight at a time (all eight read before any is
// written), then one at a time. Counts nUnits down to 0.
#define MEM_COPY_UNITS(T, pTo, pFrom, nUnits)                                   \
    {                                                                           \
        T* pDstUnit = (pTo);                                                    \
        const T* pSrcUnit = (pFrom);                                            \
        while ((nUnits) > 7) {                                                  \
            T unit0, unit1, unit2, unit3, unit4, unit5, unit6, unit7;           \
            unit1 = pSrcUnit[1];                                                \
            unit2 = pSrcUnit[2];                                                \
            unit3 = pSrcUnit[3];                                                \
            unit4 = pSrcUnit[4];                                                \
            unit5 = pSrcUnit[5];                                                \
            unit6 = pSrcUnit[6];                                                \
            unit7 = pSrcUnit[7];                                                \
            unit0 = pSrcUnit[0];                                                \
            pSrcUnit += 8;                                                      \
            pDstUnit[0] = unit0;                                                \
            pDstUnit[1] = unit1;                                                \
            pDstUnit[2] = unit2;                                                \
            pDstUnit[3] = unit3;                                                \
            pDstUnit[4] = unit4;                                                \
            pDstUnit[5] = unit5;                                                \
            pDstUnit[6] = unit6;                                                \
            pDstUnit[7] = unit7;                                                \
            pDstUnit += 8;                                                      \
            (nUnits) -= 8;                                                      \
        }                                                                       \
        while ((nUnits) != 0) {                                                 \
            *pDstUnit++ = *pSrcUnit++;                                          \
            (nUnits)--;                                                         \
        }                                                                       \
    }

// memcpy: copies nLen bytes in the widest unit (8, 4, 2 or 1 bytes) that both addresses and the
// length are aligned to, and returns pDst.
void* Mem_cpy(void* pDst, const void* pSrc, u32 nLen) {
    if ((((uptr)pDst | (uptr)pSrc | nLen) & 7) == 0) {
        nLen >>= 3;
        MEM_COPY_UNITS(f64, pDst, pSrc, nLen);
    } else if ((((uptr)pDst | (uptr)pSrc | nLen) & 3) == 0) {
        nLen >>= 2;
        MEM_COPY_UNITS(u32, pDst, pSrc, nLen);
    } else if ((((uptr)pDst | (uptr)pSrc | nLen) & 1) == 0) {
        nLen >>= 1;
        MEM_COPY_UNITS(s16, pDst, pSrc, nLen);
    } else {
        MEM_COPY_UNITS(u8, pDst, pSrc, nLen);
    }
    return pDst;
}

// Copies nUnits units of type T backwards, from the ends pFromEnd and pToEnd down: eight at a time
// (all eight read before any is written), then one at a time. Counts nUnits down to 0.
#define MEM_COPY_UNITS_BACK(T, pToEnd, pFromEnd, nUnits)                        \
    {                                                                           \
        T* pDstUnit = (pToEnd);                                                 \
        const T* pSrcUnit = (pFromEnd);                                         \
        while ((nUnits) > 7) {                                                  \
            T unit0, unit1, unit2, unit3, unit4, unit5, unit6, unit7;           \
            unit1 = pSrcUnit[-7];                                               \
            unit2 = pSrcUnit[-6];                                               \
            unit3 = pSrcUnit[-5];                                               \
            unit4 = pSrcUnit[-4];                                               \
            unit5 = pSrcUnit[-3];                                               \
            unit6 = pSrcUnit[-2];                                               \
            unit7 = pSrcUnit[-1];                                               \
            unit0 = *(pSrcUnit -= 8);                                           \
            pDstUnit[-8] = unit0;                                               \
            pDstUnit[-7] = unit1;                                               \
            pDstUnit[-6] = unit2;                                               \
            pDstUnit[-5] = unit3;                                               \
            pDstUnit[-4] = unit4;                                               \
            pDstUnit[-3] = unit5;                                               \
            pDstUnit[-2] = unit6;                                               \
            pDstUnit[-1] = unit7;                                               \
            pDstUnit -= 8;                                                      \
            (nUnits) -= 8;                                                      \
        }                                                                       \
        while ((nUnits) != 0) {                                                 \
            *--pDstUnit = *--pSrcUnit;                                          \
            (nUnits)--;                                                         \
        }                                                                       \
    }

// memmove: Mem_cpy unless pSrc is below pDst and runs into it; then the copy runs backwards from
// the ends, in the widest unit the ends and the length are aligned to. Returns pDst.
void* Mem_move(void* pDst, const void* pSrc, u32 nLen) {
    void* pDstEnd;
    const void* pSrcEnd;

    if ((int)pSrc >= (int)pDst || (const u8*)pSrc + nLen <= (u8*)pDst) {
        return Mem_cpy(pDst, pSrc, nLen);
    }
    pSrcEnd = (const u8*)pSrc + nLen;
    pDstEnd = (u8*)pDst + nLen;
    if ((((uptr)pDstEnd | (uptr)pSrcEnd | nLen) & 7) == 0) {
        nLen >>= 3;
        MEM_COPY_UNITS_BACK(f64, pDstEnd, pSrcEnd, nLen);
    } else if ((((uptr)pDstEnd | (uptr)pSrcEnd | nLen) & 3) == 0) {
        nLen >>= 2;
        MEM_COPY_UNITS_BACK(u32, pDstEnd, pSrcEnd, nLen);
    } else if ((((uptr)pDstEnd | (uptr)pSrcEnd | nLen) & 1) == 0) {
        nLen >>= 1;
        MEM_COPY_UNITS_BACK(s16, pDstEnd, pSrcEnd, nLen);
    } else {
        MEM_COPY_UNITS_BACK(u8, pDstEnd, pSrcEnd, nLen);
    }
    return pDst;
}

// memset: fills nLen bytes at pDst with the low byte of nValue and returns pDst. Under 32 bytes it
// goes byte by byte; longer fills go byte by byte up to an 8-byte boundary, then store a double
// holding the byte eight times, 32 and then 8 bytes at a time, and end byte by byte.
void* Mem_set(void* pDst, int nValue, u32 nLen) {
    u8* pByte;
    u8* pWide;
    int nToAlign;
    u32 uFill;
    u8 nByte;
    union {
        u32 aWords[2];
        f64 fDouble;
    } fill;

    nByte = nValue;
    if (nLen < 32) {
        pByte = pDst;
        while (nLen != 0) {
            *pByte++ = nByte;
            nLen--;
        }
    } else {
        pWide = pDst;
        nToAlign = (8 - (uptr)pWide) & 7;
        while (nToAlign > 0) {
            *pWide++ = nByte;
            nLen--;
            nToAlign--;
        }
        uFill = (nByte << 8) | nByte;
        uFill = (uFill << 16) | (uFill & 0xFFFF);
        fill.aWords[0] = uFill;
        fill.aWords[1] = uFill;
        while (nLen > 31) {
            ((f64*)pWide)[0] = fill.fDouble;
            nLen -= 32;
            ((f64*)pWide)[1] = fill.fDouble;
            ((f64*)pWide)[2] = fill.fDouble;
            ((f64*)pWide)[3] = fill.fDouble;
            pWide += 32;
        }
        while (nLen > 7) {
            *(f64*)pWide = fill.fDouble;
            pWide += 8;
            nLen -= 8;
        }
        pByte = pWide;
        while (nLen != 0) {
            *pByte++ = nByte;
            nLen--;
        }
    }
    return pDst;
}

// memcmp: -1, 0 or 1 as the first byte that differs is lower in pA, no byte differs, or the first
// that differs is higher in pA (the bytes compared unsigned).
int Mem_compare(const void* pA, const void* pB, u32 nLen) {
    return memcmp(pA, pB, nLen);
}
