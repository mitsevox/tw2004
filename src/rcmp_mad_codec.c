// rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree): the decoder for
// the 'MAD' movie format (TW06's rcmp folder splits it into maddec.c and madidct.c): the block
// decoder and inverse DCT, the movie decoder that hands out frames (MadDecoder, llpict.h). After
// it comes front-end code: the 'TEO '/'BALF' stream handlers and the ball models and logo drawn
// on the create-a-player golfer (FEgolferanim.c).
//
// A MAD file is a picture: 'MADk' a key frame, 'MADm' and 'MADe' frames coded against the last
// key or 'MADm' frame. The coded data is read 16 bits at a time, little-endian, into a 32-bit
// bit buffer.

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

s32 lbl_802821A8;               // the decoder's tables are built
s32 lbl_802821AC;               // 0: a key frame, 1: coded against a reference
s32 lbl_802821B0;               // bits left in lbl_802821B4
u32 lbl_802821B4;               // the bit buffer, next bit at the top
u8* lbl_802821B8;               // the next coded byte

// MPEG-1's default intra quantizer matrix
const s32 lbl_80184A68[64] = {
    8,  16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};

// the scaled IDCT's factors for each coefficient (0x2000 = 1.0)
s32 lbl_80190FE0[64] = {
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x1712, 0x10A2, 0x11A8, 0x139F, 0x1712, 0x1D5D, 0x2AA1, 0x539F,
    0x187E, 0x11A8, 0x12BF, 0x14D4, 0x187E, 0x1F2C, 0x2D41, 0x58C5,
    0x1B37, 0x139F, 0x14D4, 0x1725, 0x1B37, 0x22A3, 0x3249, 0x62A3,
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x28BA, 0x1D5D, 0x1F2C, 0x22A3, 0x28BA, 0x33D6, 0x4B42, 0x939F,
    0x3B21, 0x2AA1, 0x2D41, 0x3249, 0x3B21, 0x4B42, 0x6D41, 0xD650,
    0x73FC, 0x539F, 0x58C5, 0x62A3, 0x73FC, 0x939F, 0xD650, 0x1A463,
};
// the scan order of a block's coefficients
const s32 lbl_80184B68[64] = {
    0,  8,  1,  2,  9,  16, 24, 17, 10, 3,  4,  11, 18, 25, 32, 40,
    33, 26, 19, 12, 5,  6,  13, 20, 27, 34, 41, 48, 56, 49, 42, 35,
    28, 21, 14, 7,  15, 22, 29, 36, 43, 50, 57, 58, 51, 44, 37, 30,
    23, 31, 38, 45, 52, 59, 60, 53, 46, 39, 47, 54, 61, 62, 55, 63,
};

u8 lbl_801F6858[512];           // a pixel value's clamp to 0..255, by its low 9 bits
s32 lbl_801F6A58[2][64];        // a macroblock's U and V blocks
s32 lbl_801F6C58[256];          // a macroblock's 16x16 Y block
s32 lbl_801F7058[64];           // the quantizer for this picture
u32 lbl_801F7158[64];           // looked up by the buffer's top 6 bits
u32 lbl_801F7258[256];          // } the coefficient codes: the first 9 bits index
u32 lbl_801F7658[256];          // } lbl_801F7A58; longer codes continue in these two
u32 lbl_801F7A58[512];          // }
s32 lbl_801F8258[64];           // the inverse DCT's first pass
s32 lbl_801F8358[64];           // a block's coefficients

void fn_800B769C(void);
u32 fn_800B8984(u8* pData, int nBytes);
s32 fn_800B8A04(s32 a, s32 b);
void fn_800B8618(u8* pRefY, u8* pRefU, u8* pRefV, u8* pY, u8* pU, u8* pV, int nStride);
int fn_800B8AA0(void);
void fn_800B8F28(s32* pOut, int nStride);

// A code's table entry: its length in the low byte, the run in bits 16-21, the level on top.
#define MAD_ENTRY(nLen, nValue) ((nLen) | (((u32)(nValue) << 22) | (((nValue) << 6) & 0x3F0000)))

// fake match: an identity inline around each MAD_ENTRY in fn_800B769C moves the entry's
// computation after the loop setup, as in EA's code
static inline u32 fn_800B769C_Read(u32 uEntry) {
    return uEntry;
}

// Build the decoder's tables: the pixel clamp, the coefficient code lookups and the DC codes.
void fn_800B769C(void) {
    s32 nCode;
    s32 nValue;
    u32 uEntry;
    int nIndex;
    int nCount;
    int nLen;
    int nBits;                          // the length left after the table's prefix
    int i;
    int j;
    int n;

    for (i = -256; i < 255; i++) {
        n = i;
        if (n < -128) {
            n = -128;
        } else if (n > 127) {
            n = 127;
        }
        lbl_801F6858[i & 0x1FF] = n + 128;
    }

    // the first 9 bits: the escape and end-of-block prefixes, and where longer codes continue
    lbl_801F7A58[0] = 0xF;
    for (i = 1; i < 8; i++) {
        lbl_801F7A58[i] = 0x1F;
    }
    for (i = 8; i < 16; i++) {
        lbl_801F7A58[i] = 0x2F;
    }
    for (i = 0; i < 128; i++) {
        lbl_801F7A58[256 + i] = 0x3F;
    }

    for (i = 1; i < 95; i++) {
        nCode = lbl_80183C78[i].nCode;
        nLen = lbl_80183C78[i].nLen;
        nValue = lbl_80183C78[i].nValue;
        if (nCode & 0xFC00) {
            // up to 9 bits: every 9-bit index that starts with the code
            nIndex = nCode >> 7;
            nCount = 1 << (9 - nLen);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nLen, nValue));
            for (j = 0; j < nCount; j++) {
                lbl_801F7A58[nIndex + j] = uEntry;
            }
        } else {
            // six zero bits first: the next 8 bits index lbl_801F7258
            nIndex = nCode >> 2;
            nBits = nLen - 6;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue));
            for (j = 0; j < nCount; j++) {
                lbl_801F7258[nIndex + j] = uEntry;
            }
        }
    }

    for (i = 0; i < 128; i++) {
        nCode = lbl_80184268[i].nCode;
        nLen = lbl_80184268[i].nLen;
        nValue = lbl_80184268[i].nValue;
        if (!(nCode & 0x8000)) {
            nBits = nLen - 1;
            nIndex = nCode >> 7;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue));
            for (j = 0; j < nCount; j++) {
                lbl_801F7658[nIndex + j] = uEntry;
            }
        } else {
            nBits = nLen + 2;
            nIndex = nCode >> 10;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue));
            for (j = 0; j < nCount; j++) {
                lbl_801F7258[nIndex + j] = uEntry;
            }
        }
    }

    // the DC codes, by the top 6 bits: a 0 bit is 0; 1 and five bits are 1..16 or -16..-1
    for (i = 0; i < 32; i++) {
        lbl_801F7158[i] = 1;
    }
    for (i = 0; i < 16; i++) {
        lbl_801F7158[32 + i] = ((u32)(i + 1) << 22) | 6;
        lbl_801F7158[48 + i] = ((u32)(i - 16) << 22) | 6;
    }
    lbl_802821A8 = 1;
}

// Drop nBits bits from the buffer, refilling 16 at a time.
void fn_800B7D80(int nBits) {
    lbl_802821B4 <<= nBits;
    lbl_802821B0 -= nBits;
    if (lbl_802821B0 < 16) {
        lbl_802821B4 |= fn_800B8984(lbl_802821B8, 2) << (16 - lbl_802821B0);
        lbl_802821B0 += 16;
        lbl_802821B8 += 2;
    }
}

// The code at the top of the buffer: its entry's low byte is its length in bits.
s32 fn_800B7DF4(void) {
    s32 nCode = lbl_801F7158[lbl_802821B4 >> 26];
    fn_800B7D80(nCode & 0xFF);
    return nCode >> 22;
}

// Fill an 8x8 block (rows nStride words apart) with lbl_801F8358[0].
void fn_800B7E38(s32* pOut, int nStride) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = lbl_801F8358[0];
        pOut[1] = lbl_801F8358[0];
        pOut[2] = lbl_801F8358[0];
        pOut[3] = lbl_801F8358[0];
        pOut[4] = lbl_801F8358[0];
        pOut[5] = lbl_801F8358[0];
        pOut[6] = lbl_801F8358[0];
        pOut[7] = lbl_801F8358[0];
        pOut += nStride;
    }
}

// An 8x8 block of pixels into a 16-wide block of 16.16 values, nAdd added to each.
void fn_800B8064(const u8* pSrc, int nStride, s32* pOut, int nAdd) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = (pSrc[0] + nAdd) << 16;
        pOut[1] = (pSrc[1] + nAdd) << 16;
        pOut[2] = (pSrc[2] + nAdd) << 16;
        pOut[3] = (pSrc[3] + nAdd) << 16;
        pOut[4] = (pSrc[4] + nAdd) << 16;
        pOut[5] = (pSrc[5] + nAdd) << 16;
        pOut[6] = (pSrc[6] + nAdd) << 16;
        pOut[7] = (pSrc[7] + nAdd) << 16;
        pSrc += nStride;
        pOut += 16;
    }
}

// The same into an 8-wide block.
void fn_800B8180(const u8* pSrc, int nStride, s32* pOut, int nAdd) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = (pSrc[0] + nAdd) << 16;
        pOut[1] = (pSrc[1] + nAdd) << 16;
        pOut[2] = (pSrc[2] + nAdd) << 16;
        pOut[3] = (pSrc[3] + nAdd) << 16;
        pOut[4] = (pSrc[4] + nAdd) << 16;
        pOut[5] = (pSrc[5] + nAdd) << 16;
        pOut[6] = (pSrc[6] + nAdd) << 16;
        pOut[7] = (pSrc[7] + nAdd) << 16;
        pSrc += nStride;
        pOut += 8;
    }
}

// A 16x16 block of 16.16 values back to pixels, clamped through lbl_801F6858.
void fn_800B829C(const s32* pIn, u8* pOut, int nStride) {
    int i;

    for (i = 0; i < 16; i++) {
        pOut[0] = lbl_801F6858[(pIn[0] >> 16) & 0x1FF];
        pOut[1] = lbl_801F6858[(pIn[1] >> 16) & 0x1FF];
        pOut[2] = lbl_801F6858[(pIn[2] >> 16) & 0x1FF];
        pOut[3] = lbl_801F6858[(pIn[3] >> 16) & 0x1FF];
        pOut[4] = lbl_801F6858[(pIn[4] >> 16) & 0x1FF];
        pOut[5] = lbl_801F6858[(pIn[5] >> 16) & 0x1FF];
        pOut[6] = lbl_801F6858[(pIn[6] >> 16) & 0x1FF];
        pOut[7] = lbl_801F6858[(pIn[7] >> 16) & 0x1FF];
        pOut[8] = lbl_801F6858[(pIn[8] >> 16) & 0x1FF];
        pOut[9] = lbl_801F6858[(pIn[9] >> 16) & 0x1FF];
        pOut[10] = lbl_801F6858[(pIn[10] >> 16) & 0x1FF];
        pOut[11] = lbl_801F6858[(pIn[11] >> 16) & 0x1FF];
        pOut[12] = lbl_801F6858[(pIn[12] >> 16) & 0x1FF];
        pOut[13] = lbl_801F6858[(pIn[13] >> 16) & 0x1FF];
        pOut[14] = lbl_801F6858[(pIn[14] >> 16) & 0x1FF];
        pOut[15] = lbl_801F6858[(pIn[15] >> 16) & 0x1FF];
        pIn += 16;
        pOut += nStride;
    }
}

// An 8x8 block of 16.16 values back to pixels.
void fn_800B83DC(const s32* pIn, u8* pOut, int nStride) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = lbl_801F6858[(pIn[0] >> 16) & 0x1FF];
        pOut[1] = lbl_801F6858[(pIn[1] >> 16) & 0x1FF];
        pOut[2] = lbl_801F6858[(pIn[2] >> 16) & 0x1FF];
        pOut[3] = lbl_801F6858[(pIn[3] >> 16) & 0x1FF];
        pOut[4] = lbl_801F6858[(pIn[4] >> 16) & 0x1FF];
        pOut[5] = lbl_801F6858[(pIn[5] >> 16) & 0x1FF];
        pOut[6] = lbl_801F6858[(pIn[6] >> 16) & 0x1FF];
        pOut[7] = lbl_801F6858[(pIn[7] >> 16) & 0x1FF];
        pIn += 8;
        pOut += nStride;
    }
}

// Start a picture: pData is its coded data, nMode 0 for a key frame and 1 for one coded against
// a reference, nQuant its quality.
void fn_800B8528(u8* pData, int nMode, int nQuant) {
    int i;

    if (lbl_802821A8 == 0) {
        fn_800B769C();
    }
    lbl_802821B4 = (fn_800B8984(pData, 2) << 16) | fn_800B8984(pData + 2, 2);
    lbl_802821B0 = 32;
    lbl_802821B8 = pData + 4;
    lbl_802821AC = nMode;
    lbl_801F7058[0] = fn_800B8A04(lbl_80184A68[0] << 16, lbl_80190FE0[0]);
    for (i = 1; i < 64; i++) {
        lbl_801F7058[i] = fn_800B8A04((nQuant * lbl_80184A68[i]) << 13, lbl_80190FE0[i]);
    }
}

// Decode one macroblock (16x16 Y, 8x8 U and V) into pY, pU and pV, nStride the Y rows' spacing
// (the U and V rows are half that). In a frame coded against a reference, a block with its bit
// set in the block pattern is the reference block (moved by the macroblock's motion) plus a
// level; the others are coded in full.
void fn_800B8618(u8* pRefY, u8* pRefU, u8* pRefV, u8* pY, u8* pU, u8* pV, int nStride) {
    int nHalf;
    u32 uPattern;
    int dx;
    int dy;
    int nOffset;

    nHalf = nStride >> 1;
    if (lbl_802821AC == 0) {
        uPattern = 0;
    } else if (!(lbl_802821B4 & 0xC0000000)) {
        uPattern = 0;
        fn_800B7D80(2);
    } else {
        if (lbl_802821B4 & 0x80000000) {
            uPattern = 0x3FF;
            fn_800B7D80(1);
        } else {
            uPattern = lbl_802821B4 >> 24;
            fn_800B7D80(8);
        }
        dx = fn_800B7DF4();
        dy = fn_800B7DF4();
        pRefY += dx + dy * nStride;
        nOffset = (dx >> 1) + (dy >> 1) * nHalf;
        pRefU += nOffset;
        pRefV += nOffset;
    }
    if (!(uPattern & 1)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(&lbl_801F6C58[0], 16);
        } else {
            fn_800B8F28(&lbl_801F6C58[0], 16);
        }
    } else {
        fn_800B8064(pRefY, nStride, &lbl_801F6C58[0], fn_800B7DF4() * 2 - 128);
    }
    if (!(uPattern & 2)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(&lbl_801F6C58[8], 16);
        } else {
            fn_800B8F28(&lbl_801F6C58[8], 16);
        }
    } else {
        fn_800B8064(pRefY + 8, nStride, &lbl_801F6C58[8], fn_800B7DF4() * 2 - 128);
    }
    if (!(uPattern & 4)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(&lbl_801F6C58[128], 16);
        } else {
            fn_800B8F28(&lbl_801F6C58[128], 16);
        }
    } else {
        fn_800B8064(pRefY + nStride * 8, nStride, &lbl_801F6C58[128], fn_800B7DF4() * 2 - 128);
    }
    if (!(uPattern & 8)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(&lbl_801F6C58[136], 16);
        } else {
            fn_800B8F28(&lbl_801F6C58[136], 16);
        }
    } else {
        fn_800B8064(pRefY + nStride * 8 + 8, nStride, &lbl_801F6C58[136], fn_800B7DF4() * 2 - 128);
    }
    if (!(uPattern & 0x10)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(lbl_801F6A58[0], 8);
        } else {
            fn_800B8F28(lbl_801F6A58[0], 8);
        }
    } else {
        fn_800B8180(pRefU, nHalf, lbl_801F6A58[0], fn_800B7DF4() * 2 - 128);
    }
    if (!(uPattern & 0x20)) {
        if (fn_800B8AA0() == 1) {
            fn_800B7E38(lbl_801F6A58[1], 8);
        } else {
            fn_800B8F28(lbl_801F6A58[1], 8);
        }
    } else {
        fn_800B8180(pRefV, nHalf, lbl_801F6A58[1], fn_800B7DF4() * 2 - 128);
    }
    fn_800B829C(lbl_801F6C58, pY, nStride);
    fn_800B83DC(lbl_801F6A58[0], pU, nHalf);
    fn_800B83DC(lbl_801F6A58[1], pV, nHalf);
}

// nBytes bytes at pData, little-endian.
u32 fn_800B8984(u8* pData, int nBytes) {
    if (nBytes == 1) {
        return pData[0];
    }
    if (nBytes == 2) {
        return (pData[1] << 8) | pData[0];
    }
    if (nBytes == 3) {
        return pData[0] | ((pData[2] << 16) | (pData[1] << 8));
    }
    if (nBytes == 4) {
        return pData[0] | ((pData[1] << 8) | ((pData[3] << 24) | (pData[2] << 16)));
    }
    return 0;
}

// a * b in 16.16 fixed point, rounded.
s32 fn_800B8A04(s32 a, s32 b) {
    return ((s64)a * b + 0x8000) >> 16;
}

// The same as fn_800B7D80.
void fn_800B8A2C(int nBits) {
    lbl_802821B4 <<= nBits;
    lbl_802821B0 -= nBits;
    if (lbl_802821B0 < 16) {
        lbl_802821B4 |= fn_800B8984(lbl_802821B8, 2) << (16 - lbl_802821B0);
        lbl_802821B0 += 16;
        lbl_802821B8 += 2;
    }
}

// Decode one block's coefficients into lbl_801F8358, dequantized, in natural order. The result
// is one past the last coefficient's scan position: 1 when there is only the DC one.
int fn_800B8AA0(void) {
    u32 uCode;
    int nLen;
    int n;
    int i;
    int nDC;
    s32* p;

    nDC = (s32)lbl_802821B4 >> 24;
    lbl_801F8358[0] = nDC * lbl_801F7058[0];
    fn_800B8A2C(8);
    // fake match: the 63 words cleared three per pass; a loop of single stores unrolls 9-way, not
    // EA's 21 stores x 3
    p = &lbl_801F8358[1];
    for (i = 0; i < 21; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p += 3;
    }
    n = 1;
    while (1) {
        uCode = lbl_801F7A58[lbl_802821B4 >> 23];
        nLen = uCode & 0xFF;
        if (nLen > 9) {
            if (!(nLen & 0x20)) {
                if (!(nLen & 0x10)) {
                    fn_800B8A2C(9);
                    uCode = lbl_801F7658[lbl_802821B4 >> 24];
                    nLen = uCode & 0xFF;
                } else {
                    fn_800B8A2C(6);
                    uCode = lbl_801F7258[lbl_802821B4 >> 24];
                    nLen = uCode & 0xFF;
                }
            } else if (!(nLen & 0x10)) {
                // escape: the run and level follow as they are
                fn_800B8A2C(6);
                uCode = lbl_802821B4;
                nLen = 16;
            } else {
                // end of block
                fn_800B8A2C(2);
                return n;
            }
        }
        fn_800B8A2C(nLen);
        n += (uCode >> 16) & 0x3F;
        i = lbl_80184B68[n++];
        lbl_801F8358[i] = ((s32)uCode >> 22) * lbl_801F7058[i];
    }
}

// The inverse DCT's first pass: eight coefficients in, a column of pOut (8 apart) out.
void fn_800B8C54(s32* pIn, s32* pOut) {
    s32 t10;
    s32 z11;
    s32 z13;
    s32 z5;
    s32 t11;
    s32 o2;
    s32 z10;
    s32 e0;
    s32 e1;
    s32 z12;
    s32 e3;
    s32 e2;
    s32 t;
    s32 s;

    if ((pIn[1] | pIn[2] | pIn[3] | pIn[4] | pIn[5] | pIn[6] | pIn[7]) == 0) {
        // only the DC coefficient: the column is flat
        pOut[0] = pIn[0];
        pOut[8] = pIn[0];
        pOut[16] = pIn[0];
        pOut[24] = pIn[0];
        pOut[32] = pIn[0];
        pOut[40] = pIn[0];
        pOut[48] = pIn[0];
        pOut[56] = pIn[0];
        return;
    }
    z10 = pIn[5] - pIn[3];
    z12 = pIn[1] - pIn[7];
    z11 = pIn[1] + pIn[7];
    z13 = pIn[5] + pIn[3];
    t11 = z11 - z13;
    // register note: z13 and z5 are reused for the later sums (z13: the odd sum, then o0; z5: t12,
    // then o1), which gives EA's register allocation
    z13 = z13 + z11;
    z5 = fn_800B8A04(z10 + z12, 0x61F8);
    t10 = z5 + fn_800B8A04(z10, 0x8A8C);
    t11 = fn_800B8A04(t11, 0xB505);
    z5 = fn_800B8A04(z12, 0x14E7B) - z5;
    z13 = z13 + z5;
    z5 = z5 + t11;
    o2 = t11 + t10;
    e0 = pIn[0] + pIn[4];
    e1 = pIn[0] - pIn[4];
    t = fn_800B8A04(pIn[2] - pIn[6], 0xB505);
    e2 = e1 - t;
    e1 = e1 + t;
    s = pIn[2] + pIn[6] + t;
    e3 = e0 - s;
    e0 = e0 + s;
    pOut[0] = e0 + z13;
    pOut[8] = e1 + z5;
    pOut[16] = e2 + o2;
    pOut[24] = e3 + t10;
    pOut[32] = e3 - t10;
    pOut[40] = e2 - o2;
    pOut[48] = e1 - z5;
    pOut[56] = e0 - z13;
}

// The second pass: a row of the first pass's output into eight 16.16 values.
void fn_800B8DF4(s32* pIn, s32* pOut) {
    s32 t10;
    s32 z11;
    s32 z13;
    s32 z5;
    s32 t11;
    s32 o2;
    s32 z10;
    s32 e0;
    s32 e1;
    s32 z12;
    s32 e3;
    s32 e2;
    s32 t;
    s32 s;

    z10 = pIn[5] - pIn[3];
    z11 = pIn[1] + pIn[7];
    z12 = pIn[1] - pIn[7];
    z13 = pIn[5] + pIn[3];
    t11 = z11 - z13;
    // register note: z13 and z5 reused as in fn_800B8C54
    z13 = z13 + z11;
    z5 = fn_800B8A04(z10 + z12, 0x61F8);
    t10 = z5 + fn_800B8A04(z10, 0x8A8C);
    t11 = fn_800B8A04(t11, 0xB505);
    z5 = fn_800B8A04(z12, 0x14E7B) - z5;
    z13 = z13 + z5;
    z5 = z5 + t11;
    o2 = t11 + t10;
    e0 = pIn[0] + pIn[4];
    e1 = pIn[0] - pIn[4];
    t = fn_800B8A04(pIn[2] - pIn[6], 0xB505);
    e2 = e1 - t;
    e1 = e1 + t;
    s = pIn[2] + pIn[6] + t;
    e3 = e0 - s;
    e0 = e0 + s;
    pOut[0] = e0 + z13;
    pOut[1] = e1 + z5;
    pOut[2] = e2 + o2;
    pOut[3] = e3 + t10;
    pOut[4] = e3 - t10;
    pOut[5] = e2 - o2;
    pOut[6] = e1 - z5;
    pOut[7] = e0 - z13;
}

// The inverse DCT of lbl_801F8358 into an 8x8 block (rows nStride words apart).
void fn_800B8F28(s32* pOut, int nStride) {
    fn_800B8C54(&lbl_801F8358[0], &lbl_801F8258[0]);
    fn_800B8C54(&lbl_801F8358[8], &lbl_801F8258[1]);
    fn_800B8C54(&lbl_801F8358[16], &lbl_801F8258[2]);
    fn_800B8C54(&lbl_801F8358[24], &lbl_801F8258[3]);
    fn_800B8C54(&lbl_801F8358[32], &lbl_801F8258[4]);
    fn_800B8C54(&lbl_801F8358[40], &lbl_801F8258[5]);
    fn_800B8C54(&lbl_801F8358[48], &lbl_801F8258[6]);
    fn_800B8C54(&lbl_801F8358[56], &lbl_801F8258[7]);
    fn_800B8DF4(&lbl_801F8258[0], pOut);
    fn_800B8DF4(&lbl_801F8258[8], pOut + nStride);
    fn_800B8DF4(&lbl_801F8258[16], pOut + nStride * 2);
    fn_800B8DF4(&lbl_801F8258[24], pOut + nStride * 3);
    fn_800B8DF4(&lbl_801F8258[32], pOut + nStride * 4);
    fn_800B8DF4(&lbl_801F8258[40], pOut + nStride * 5);
    fn_800B8DF4(&lbl_801F8258[48], pOut + nStride * 6);
    fn_800B8DF4(&lbl_801F8258[56], pOut + nStride * 7);
}
