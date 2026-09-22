// UStream.c: EA's streaming asset loader (the CTRL / SHOC / SONO chunk files: .hog, .gcb,
// .ngc). The file name comes from the assert string at 0x801868A0; function names are ours.
// CodeWarrior GC/2.5, -O4,p. See docs/formats/ctrl-container.md.

#include "game_types.h"

// Byte fill, used by the decompressor for run-length fills. Returns the destination.
u8* UStream_Fill(u8* pDst, u32 value, u32 uCount) {
    u8 v = (u8)value;
    u8* p = pDst;
    while (uCount--) {
        *p++ = v;
    }
    return pDst;
}

// Forward copy that may overlap (source before destination); copies halfwords when both
// pointers have the same alignment. Used by the decompressor for back-references.
// The eight-at-a-time blocks read into temporaries first: that is what lets the compiler
// hoist the loads above the stores.
u8* UStream_Copy(u8* pDst, const u8* pSrc, u32 uCount) {
    if (((u32)pDst & (u32)pSrc) & 1) {
        *pDst++ = *pSrc++;
        uCount--;
    }
    if ((((u32)pDst | (u32)pSrc) & 1) == 0) {
        s16* d = (s16*)pDst;
        const s16* s = (const s16*)pSrc;
        u32 uOdd = uCount & 1;
        uCount >>= 1;
        while (uCount > 7) {
            {
                s16 t0 = s[0], t1 = s[1], t2 = s[2], t3 = s[3], t4 = s[4], t5 = s[5], t6 = s[6], t7 = s[7];
                d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3; d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                d += 8; s += 8;
            }
            uCount -= 8;
        }
        while (uCount--) {
            *d++ = *s++;
        }
        if (uOdd) {
            *(u8*)d = *(const u8*)s;
        }
    } else {
        u8* d = pDst;
        while (uCount > 7) {
            {
                u8 t0 = pSrc[0], t1 = pSrc[1], t2 = pSrc[2], t3 = pSrc[3], t4 = pSrc[4], t5 = pSrc[5], t6 = pSrc[6], t7 = pSrc[7];
                d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3; d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                d += 8; pSrc += 8;
            }
            uCount -= 8;
        }
        while (uCount--) {
            *d++ = *pSrc++;
        }
    }
    return pDst;
}

// Decompress an 'Rdat' payload. Two-byte command words, big-endian:
//   1nnn1ooo cccccccc  nnn != 0: fill (c + 3) bytes with the byte at dst - (ooo<<3 | nnn)
//   10001lll llllllll  literal run of l bytes (l = low 11 bits)
//   mlll dddd dddddddd  back-reference: length lll (+ next byte if 7) + 3, distance d;
//                       m = 0 forward copy, m = 1 mirrored copy (bytes read backwards)
void UStream_Decompress(const void* pSrc, void* pDst, u32 uSize) {
    u8* pEnd;
    u8* d = (u8*)pDst;
    const u8* s = (const u8*)pSrc;
    int nLen;
    pEnd = d + uSize;
    do {
        u32 uCode;
        uCode = s[0] << 8 | s[1];
        s += 2;
        if ((uCode & 0x8800) != 0x8800) {
            u32 uDist = uCode & 0xFFF;
            const u8* pFrom;
            // NOT YET EXACT (3 instructions): the original keeps this length in r28 before and
            // after the +3 and adds in place. Every plain spelling (nLen += 3 after an if) makes
            // CodeWarrior fold the +3 into each later use instead; the ternary keeps one add but
            // lets the pre-add value sit in a scratch register. Same behaviour either way.
            nLen = (uCode >> 12) & 7;
            nLen = ((nLen == 7) ? (nLen + *s++) : nLen) + 3;
            pFrom = d - uDist;
            if ((uCode & 0x8000) == 0) {
                UStream_Copy(d, pFrom, nLen);
                d += nLen;
            } else {
                // Mirrored copy: the reference is read backwards, so a run of bytes comes out
                // reversed. Eight at a time through temporaries, then the remainder.
                int nBlocks;
                pFrom += 2;
                if (nLen >= 8) {
                    for (nBlocks = nLen / 8; nBlocks > 0; nBlocks--) {
                        u8 t7 = pFrom[-7], t6 = pFrom[-6], t5 = pFrom[-5], t4 = pFrom[-4];
                        u8 t3 = pFrom[-3], t2 = pFrom[-2], t1 = pFrom[-1], t0 = pFrom[0];
                        d[0] = t0; d[1] = t1; d[2] = t2; d[3] = t3;
                        d[4] = t4; d[5] = t5; d[6] = t6; d[7] = t7;
                        d += 8; pFrom -= 8;
                    }
                }
                nBlocks = nLen & 7;
                while (nBlocks > 0) {
                    *d++ = *pFrom--;
                    nBlocks--;
                }
            }
        } else {
            nLen = (uCode >> 12) & 7;
            if (nLen == 0) {
                nLen = uCode & 0x7FF;
                while (nLen > 0) {
                    *d++ = *s++;
                    nLen--;
                }
            } else {
                u32 uDist = nLen | ((uCode >> 5) & 0x38);
                u32 uCount = uCode & 0xFF;
                UStream_Fill(d, d[-(int)uDist], uCount + 3);
                d += uCount + 3;
            }
        }
    } while (d < pEnd);
}
