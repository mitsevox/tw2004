// maddec.c (EA's name: TW06's PDB puts madinit..MAD_decodemacroblock in Golf\rcmp\maddec.c, and
// NFSMW's copy of EA's rcmp package has them, in this order, in maddec.cpp): the core of EA's MAD
// picture and movie decoder: the bit reader, the decoder's tables and the macroblock decoder, which
// MAD_DecodeFrame (rcmp_mad_codec.c) and PictInt_Decode (LLPictInt.c) run 16x16 pixels at a time.
// A block's coefficients are decoded by madvlcdecode (maddeca.c) and turned back into pixels by
// idctcompute (madidct.c).
//
// A MAD file is a picture: 'MADk' a key frame, 'MADm' and 'MADe' frames coded against the last
// key or 'MADm' frame. The coded data is read 16 bits at a time, little-endian, into a 32-bit
// bit buffer.
//
// MAD_ReadLittleEndian and MAD_FixedMul come last, after MAD_decodemacroblock, and serve all three
// files (maddeca.c's discardbits reads through the first, madidct.c's passes multiply with the
// second): most likely helpers of a shared header that the compiler emitted once, at the end of
// the first file using them (NFSMW's maddec.cpp inlines realcore getm.inl, MAD_ReadLittleEndian's
// job, in discardbits and MAD_initdecode; neither is known by name).
// Data: the decoder's whole .sbss (the bit reader, 0x802821A8..0x802821C0, padded after
// maddataptr), its .bss up to 0x801F8258 (madidct.c's after), the codes and the intra matrix
// (.rodata up to 0x80184B68; maddeca.c's scan order after) and idctprescale (.data).

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// .sbss and .bss: defined in reverse address order (CodeWarrior lays them out last-defined-first)
u8* maddataptr;                 // the next coded byte
u32 madshiftreg;                // the bit buffer, next bit at the top
s32 madbitcount;                // bits left in madshiftreg
s32 gMadMotion;                 // 0: a key frame, 1: coded against a reference (MAD_initdecode)
s32 gbMadTablesBuilt;           // madinit has built the decoder's tables

// The run and level codes madinit spreads into madvlctbl1 (codes of up to 9 bits) and madvlctbl3
// (the rest, after their six zero bits); entry 0 is not used.
const MadCode gMadCoefCodes[95] = {
    {2, 0xFE00, 0, 0x8000}, {3, 0x1, 0, 0xC000}, {3, 0x3FF, 0, 0xE000}, {4, 0x401, 0, 0x6000},
    {4, 0x7FF, 0, 0x7000}, {5, 0x2, 0, 0x4000}, {5, 0x3FE, 0, 0x4800}, {5, 0x801, 0, 0x5000},
    {5, 0xBFF, 0, 0x5800}, {6, 0x3, 0, 0x2800}, {6, 0x3FD, 0, 0x2C00}, {6, 0xC01, 0, 0x3800},
    {6, 0xFFF, 0, 0x3C00}, {6, 0x1001, 0, 0x3000}, {6, 0x13FF, 0, 0x3400}, {7, 0x402, 0, 0x1800},
    {7, 0x7FE, 0, 0x1A00}, {7, 0x1401, 0, 0x1C00}, {7, 0x17FF, 0, 0x1E00}, {7, 0x1801, 0, 0x1400},
    {7, 0x1BFF, 0, 0x1600}, {7, 0x1C01, 0, 0x1000}, {7, 0x1FFF, 0, 0x1200}, {8, 0x4, 0, 0xC00},
    {8, 0x3FC, 0, 0xD00}, {8, 0x802, 0, 0x800}, {8, 0xBFE, 0, 0x900}, {8, 0x2001, 0, 0xE00},
    {8, 0x23FF, 0, 0xF00}, {8, 0x2401, 0, 0xA00}, {8, 0x27FF, 0, 0xB00}, {9, 0x5, 0, 0x2600},
    {9, 0x3FB, 0, 0x2680}, {9, 0x6, 0, 0x2100}, {9, 0x3FA, 0, 0x2180}, {9, 0x403, 0, 0x2500},
    {9, 0x7FD, 0, 0x2580}, {9, 0xC02, 0, 0x2400}, {9, 0xFFE, 0, 0x2480}, {9, 0x2801, 0, 0x2700},
    {9, 0x2BFF, 0, 0x2780}, {9, 0x2C01, 0, 0x2300}, {9, 0x2FFF, 0, 0x2380}, {9, 0x3001, 0, 0x2200},
    {9, 0x33FF, 0, 0x2280}, {9, 0x3401, 0, 0x2000}, {9, 0x37FF, 0, 0x2080}, {11, 0x7, 0, 0x280},
    {11, 0x3F9, 0, 0x2A0}, {11, 0x404, 0, 0x300}, {11, 0x7FC, 0, 0x320}, {11, 0x803, 0, 0x2C0},
    {11, 0xBFD, 0, 0x2E0}, {11, 0x1002, 0, 0x3C0}, {11, 0x13FE, 0, 0x3E0}, {11, 0x1402, 0, 0x240},
    {11, 0x17FE, 0, 0x260}, {11, 0x3801, 0, 0x380}, {11, 0x3BFF, 0, 0x3A0}, {11, 0x3C01, 0, 0x340},
    {11, 0x3FFF, 0, 0x360}, {11, 0x4001, 0, 0x200}, {11, 0x43FF, 0, 0x220}, {13, 0x8, 0, 0x1D0},
    {13, 0x3F8, 0, 0x1D8}, {13, 0x9, 0, 0x180}, {13, 0x3F7, 0, 0x188}, {13, 0xA, 0, 0x130},
    {13, 0x3F6, 0, 0x138}, {13, 0xB, 0, 0x100}, {13, 0x3F5, 0, 0x108}, {13, 0x405, 0, 0x1B0},
    {13, 0x7FB, 0, 0x1B8}, {13, 0x804, 0, 0x140}, {13, 0xBFC, 0, 0x148}, {13, 0xC03, 0, 0x1C0},
    {13, 0xFFD, 0, 0x1C8}, {13, 0x1003, 0, 0x120}, {13, 0x13FD, 0, 0x128}, {13, 0x1802, 0, 0x1E0},
    {13, 0x1BFE, 0, 0x1E8}, {13, 0x1C02, 0, 0x150}, {13, 0x1FFE, 0, 0x158}, {13, 0x2002, 0, 0x110},
    {13, 0x23FE, 0, 0x118}, {13, 0x4401, 0, 0x1F0}, {13, 0x47FF, 0, 0x1F8}, {13, 0x4801, 0, 0x1A0},
    {13, 0x4BFF, 0, 0x1A8}, {13, 0x4C01, 0, 0x190}, {13, 0x4FFF, 0, 0x198}, {13, 0x5001, 0, 0x170},
    {13, 0x53FF, 0, 0x178}, {13, 0x5401, 0, 0x160}, {13, 0x57FF, 0, 0x168},
};
// The longest run and level codes, stored without their zero bits: madinit spreads them into
// madvlctbl2 (after nine zero bits) and madvlctbl3 (after six).
const MadCode gMadCoefCodesLong[128] = {
    {6, 0xC, 0, 0xD000}, {6, 0x3F4, 0, 0xD400}, {6, 0xD, 0, 0xC800}, {6, 0x3F3, 0, 0xCC00},
    {6, 0xE, 0, 0xC000}, {6, 0x3F2, 0, 0xC400}, {6, 0xF, 0, 0xB800}, {6, 0x3F1, 0, 0xBC00},
    {6, 0x406, 0, 0xB000}, {6, 0x7FA, 0, 0xB400}, {6, 0x407, 0, 0xA800}, {6, 0x7F9, 0, 0xAC00},
    {6, 0x805, 0, 0xA000}, {6, 0xBFB, 0, 0xA400}, {6, 0xC04, 0, 0x9800}, {6, 0xFFC, 0, 0x9C00},
    {6, 0x1403, 0, 0x9000}, {6, 0x17FD, 0, 0x9400}, {6, 0x2402, 0, 0x8800}, {6, 0x27FE, 0, 0x8C00},
    {6, 0x2802, 0, 0x8000}, {6, 0x2BFE, 0, 0x8400}, {6, 0x5801, 0, 0xF800}, {6, 0x5BFF, 0, 0xFC00},
    {6, 0x5C01, 0, 0xF000}, {6, 0x5FFF, 0, 0xF400}, {6, 0x6001, 0, 0xE800}, {6, 0x63FF, 0, 0xEC00},
    {6, 0x6401, 0, 0xE000}, {6, 0x67FF, 0, 0xE400}, {6, 0x6801, 0, 0xD800}, {6, 0x6BFF, 0, 0xDC00},
    {7, 0x10, 0, 0x7C00}, {7, 0x3F0, 0, 0x7E00}, {7, 0x11, 0, 0x7800}, {7, 0x3EF, 0, 0x7A00},
    {7, 0x12, 0, 0x7400}, {7, 0x3EE, 0, 0x7600}, {7, 0x13, 0, 0x7000}, {7, 0x3ED, 0, 0x7200},
    {7, 0x14, 0, 0x6C00}, {7, 0x3EC, 0, 0x6E00}, {7, 0x15, 0, 0x6800}, {7, 0x3EB, 0, 0x6A00},
    {7, 0x16, 0, 0x6400}, {7, 0x3EA, 0, 0x6600}, {7, 0x17, 0, 0x6000}, {7, 0x3E9, 0, 0x6200},
    {7, 0x18, 0, 0x5C00}, {7, 0x3E8, 0, 0x5E00}, {7, 0x19, 0, 0x5800}, {7, 0x3E7, 0, 0x5A00},
    {7, 0x1A, 0, 0x5400}, {7, 0x3E6, 0, 0x5600}, {7, 0x1B, 0, 0x5000}, {7, 0x3E5, 0, 0x5200},
    {7, 0x1C, 0, 0x4C00}, {7, 0x3E4, 0, 0x4E00}, {7, 0x1D, 0, 0x4800}, {7, 0x3E3, 0, 0x4A00},
    {7, 0x1E, 0, 0x4400}, {7, 0x3E2, 0, 0x4600}, {7, 0x1F, 0, 0x4000}, {7, 0x3E1, 0, 0x4200},
    {8, 0x20, 0, 0x3000}, {8, 0x3E0, 0, 0x3100}, {8, 0x21, 0, 0x2E00}, {8, 0x3DF, 0, 0x2F00},
    {8, 0x22, 0, 0x2C00}, {8, 0x3DE, 0, 0x2D00}, {8, 0x23, 0, 0x2A00}, {8, 0x3DD, 0, 0x2B00},
    {8, 0x24, 0, 0x2800}, {8, 0x3DC, 0, 0x2900}, {8, 0x25, 0, 0x2600}, {8, 0x3DB, 0, 0x2700},
    {8, 0x26, 0, 0x2400}, {8, 0x3DA, 0, 0x2500}, {8, 0x27, 0, 0x2200}, {8, 0x3D9, 0, 0x2300},
    {8, 0x28, 0, 0x2000}, {8, 0x3D8, 0, 0x2100}, {8, 0x408, 0, 0x3E00}, {8, 0x7F8, 0, 0x3F00},
    {8, 0x409, 0, 0x3C00}, {8, 0x7F7, 0, 0x3D00}, {8, 0x40A, 0, 0x3A00}, {8, 0x7F6, 0, 0x3B00},
    {8, 0x40B, 0, 0x3800}, {8, 0x7F5, 0, 0x3900}, {8, 0x40C, 0, 0x3600}, {8, 0x7F4, 0, 0x3700},
    {8, 0x40D, 0, 0x3400}, {8, 0x7F3, 0, 0x3500}, {8, 0x40E, 0, 0x3200}, {8, 0x7F2, 0, 0x3300},
    {9, 0x40F, 0, 0x1300}, {9, 0x7F1, 0, 0x1380}, {9, 0x410, 0, 0x1200}, {9, 0x7F0, 0, 0x1280},
    {9, 0x411, 0, 0x1100}, {9, 0x7EF, 0, 0x1180}, {9, 0x412, 0, 0x1000}, {9, 0x7EE, 0, 0x1080},
    {9, 0x1803, 0, 0x1400}, {9, 0x1BFD, 0, 0x1480}, {9, 0x2C02, 0, 0x1A00}, {9, 0x2FFE, 0, 0x1A80},
    {9, 0x3002, 0, 0x1900}, {9, 0x33FE, 0, 0x1980}, {9, 0x3402, 0, 0x1800}, {9, 0x37FE, 0, 0x1880},
    {9, 0x3802, 0, 0x1700}, {9, 0x3BFE, 0, 0x1780}, {9, 0x3C02, 0, 0x1600}, {9, 0x3FFE, 0, 0x1680},
    {9, 0x4002, 0, 0x1500}, {9, 0x43FE, 0, 0x1580}, {9, 0x6C01, 0, 0x1F00}, {9, 0x6FFF, 0, 0x1F80},
    {9, 0x7001, 0, 0x1E00}, {9, 0x73FF, 0, 0x1E80}, {9, 0x7401, 0, 0x1D00}, {9, 0x77FF, 0, 0x1D80},
    {9, 0x7801, 0, 0x1C00}, {9, 0x7BFF, 0, 0x1C80}, {9, 0x7C01, 0, 0x1B00}, {9, 0x7FFF, 0, 0x1B80},
};

// MPEG-1's default intra quantizer matrix
const s32 gMadIntraQuant[64] = {
    8,  16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};

// the scaled IDCT's factors for each coefficient (0x2000 = 1.0; the reciprocals of the AAN IDCT's
// scale factors, 0x1712 = 1 / 1.387), which MAD_initdecode folds into madquant
s32 idctprescale[64] = {
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x1712, 0x10A2, 0x11A8, 0x139F, 0x1712, 0x1D5D, 0x2AA1, 0x539F,
    0x187E, 0x11A8, 0x12BF, 0x14D4, 0x187E, 0x1F2C, 0x2D41, 0x58C5,
    0x1B37, 0x139F, 0x14D4, 0x1725, 0x1B37, 0x22A3, 0x3249, 0x62A3,
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x28BA, 0x1D5D, 0x1F2C, 0x22A3, 0x28BA, 0x33D6, 0x4B42, 0x939F,
    0x3B21, 0x2AA1, 0x2D41, 0x3249, 0x3B21, 0x4B42, 0x6D41, 0xD650,
    0x73FC, 0x539F, 0x58C5, 0x62A3, 0x73FC, 0x939F, 0xD650, 0x1A463,
};

u32 madvlctbl1[512];           // } the coefficient codes: the first 9 bits index madvlctbl1;
u32 madvlctbl2[256];            // } longer codes continue in madvlctbl2 (after nine zero bits)
u32 madvlctbl3[256];            // } and madvlctbl3 (after six)
u32 madvlctbl4[64];             // the delta codes (getdelta), by the buffer's top 6 bits
s32 madquant[64];               // the quantizer for this picture
s32 gMadLumaBlock[256];         // a macroblock's 16x16 Y block, 16.16 values
s32 gMadChromaBlocks[2][64];    // a macroblock's U and V blocks, 16.16 values
u8 gMadClamp[512];              // a value (-256..255, by its low 9 bits) clamped to a pixel 0..255

void madinit(void);
u32 MAD_ReadLittleEndian(u8* pData, int nBytes);
s32 MAD_FixedMul(s32 a, s32 b);
void MAD_decodemacroblock(u8* src_y, u8* src_cb, u8* src_cr, u8* dest_y, u8* dest_cb, u8* dest_cr, int width);
int madvlcdecode(void);
void idctcompute(s32* dest, int stride);

// A code's table entry: its length in the low byte, the run in bits 16-21, the level on top.
#define MAD_ENTRY(nLen, nValue) ((nLen) | ((((u32)(nValue) & 0x3FF) << 22) | (((nValue) & 0xFC00) << 6)))

// fake match: an identity inline around each MAD_ENTRY in madinit moves the entry's
// computation after the loop setup, as in EA's code
static inline u32 fn_800B769C_Read(u32 uEntry) {
    return uEntry;
}

// Build the decoder's tables: the pixel clamp, the coefficient code lookups and the DC codes.
void madinit(void) {
    s32 nCode;
    s32 nValue;
    s32 nValue2;                        // fake match: the second table's value in its own local
    u32 uEntry;
    int nIndex;
    int nCount;
    int nLen;
    int nBits;                          // the length left after the table's prefix
    int i;
    int j;
    int n;

    // EA bug: the loop stops at 254, so gMadClamp[255] stays 0: a value of +255 comes out as pixel
    // 0 (black) where +128..+254 give 255 (white). (Values from +256 wrap to the table's negative
    // end by their low 9 bits and come out black as well.)
    for (i = -256; i < 255; i++) {
        n = i;
        if (n < -128) {
            n = -128;
        } else if (n > 127) {
            n = 127;
        }
        gMadClamp[i & 0x1FF] = n + 128;
    }

    // the first 9 bits: the escape and end-of-block prefixes, and where longer codes continue
    madvlctbl1[0] = 0xF;
    for (i = 1; i < 8; i++) {
        madvlctbl1[i] = 0x1F;
    }
    for (i = 8; i < 16; i++) {
        madvlctbl1[i] = 0x2F;
    }
    for (i = 0; i < 128; i++) {
        madvlctbl1[256 + i] = 0x3F;
    }

    for (i = 1; i < 95; i++) {
        nCode = gMadCoefCodes[i].nCode;
        nLen = gMadCoefCodes[i].nLen;
        nValue = gMadCoefCodes[i].nValue;
        if (nCode & 0xFC00) {
            // up to 9 bits: every 9-bit index that starts with the code
            nIndex = nCode >> 7;
            nCount = 1 << (9 - nLen);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nLen, nValue));
            for (j = 0; j < nCount; j++) {
                madvlctbl1[nIndex + j] = uEntry;
            }
        } else {
            // six zero bits first: the next 8 bits index madvlctbl3
            nIndex = nCode >> 2;
            nBits = nLen - 6;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue));
            for (j = 0; j < nCount; j++) {
                madvlctbl3[nIndex + j] = uEntry;
            }
        }
    }

    for (i = 0; i < 128; i++) {
        nCode = gMadCoefCodesLong[i].nCode;
        nLen = gMadCoefCodesLong[i].nLen;
        nValue2 = gMadCoefCodesLong[i].nValue;
        if (!(nCode & 0x8000)) {
            nBits = nLen - 1;
            nIndex = nCode >> 7;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue2));
            for (j = 0; j < nCount; j++) {
                madvlctbl2[nIndex + j] = uEntry;
            }
        } else {
            nBits = nLen + 2;
            nIndex = nCode >> 10;
            nCount = 1 << (8 - nBits);
            uEntry = fn_800B769C_Read(MAD_ENTRY(nBits, nValue2));
            for (j = 0; j < nCount; j++) {
                madvlctbl3[nIndex + j] = uEntry;
            }
        }
    }

    // the DC codes, by the top 6 bits: a 0 bit is 0; 1 and five bits are 1..16 or -16..-1
    for (i = 0; i < 32; i++) {
        madvlctbl4[i] = 1;
    }
    for (i = 0; i < 16; i++) {
        madvlctbl4[32 + i] = ((u32)(i + 1) << 22) | 6;
        madvlctbl4[48 + i] = ((u32)(i - 16) << 22) | 6;
    }
    gbMadTablesBuilt = 1;
}

// Drop the top `bits` bits of the buffer, refilling 16 at a time.
void discardbits(int bits) {
    madshiftreg <<= bits;
    madbitcount -= bits;
    if (madbitcount < 16) {
        madshiftreg |= MAD_ReadLittleEndian(maddataptr, 2) << (16 - madbitcount);
        madbitcount += 16;
        maddataptr += 2;
    }
}

// Reads one delta code from the bit buffer (madvlctbl4, by its top 6 bits): a 0 bit is 0; a 1 bit
// and five more are 1..16 or -16..-1. MAD_decodemacroblock reads a macroblock's motion (x, then y)
// and a reference block's level change with it.
s32 getdelta(void) {
    s32 nCode = madvlctbl4[madshiftreg >> 26];
    discardbits(nCode & 0xFF);
    return nCode >> 22;
}

// Fills an 8x8 block (rows stride words apart) with the DC value idctinput[0]: what the inverse DCT
// gives a block whose only coefficient is the DC one (madvlcdecode returned 1), without running it.
void dcblock(s32* dest, int stride) {
    int i;

    for (i = 0; i < 8; i++) {
        dest[0] = idctinput[0];
        dest[1] = idctinput[0];
        dest[2] = idctinput[0];
        dest[3] = idctinput[0];
        dest[4] = idctinput[0];
        dest[5] = idctinput[0];
        dest[6] = idctinput[0];
        dest[7] = idctinput[0];
        dest += stride;
    }
}

// Copies an 8x8 block of reference pixels (rows stride bytes apart) into the 16-wide luma block as
// 16.16 values, correction added to each (MAD_decodemacroblock passes a delta x 2 - 128: the
// block's level change, and the -128 that centres pixels on 0).
void getluma(const u8* src, int stride, s32* dest, int correction) {
    int i;

    for (i = 0; i < 8; i++) {
        dest[0] = (src[0] + correction) << 16;
        dest[1] = (src[1] + correction) << 16;
        dest[2] = (src[2] + correction) << 16;
        dest[3] = (src[3] + correction) << 16;
        dest[4] = (src[4] + correction) << 16;
        dest[5] = (src[5] + correction) << 16;
        dest[6] = (src[6] + correction) << 16;
        dest[7] = (src[7] + correction) << 16;
        src += stride;
        dest += 16;
    }
}

// getluma for a U or V block: into an 8-wide block.
void getchroma(const u8* src, int stride, s32* dest, int correction) {
    int i;

    for (i = 0; i < 8; i++) {
        dest[0] = (src[0] + correction) << 16;
        dest[1] = (src[1] + correction) << 16;
        dest[2] = (src[2] + correction) << 16;
        dest[3] = (src[3] + correction) << 16;
        dest[4] = (src[4] + correction) << 16;
        dest[5] = (src[5] + correction) << 16;
        dest[6] = (src[6] + correction) << 16;
        dest[7] = (src[7] + correction) << 16;
        src += stride;
        dest += 8;
    }
}

// Turns the 16x16 luma block of 16.16 values (centred on 0) back into pixels 0..255 through
// gMadClamp, rows stride bytes apart.
void setluma(const s32* src, u8* dest, int stride) {
    int i;

    for (i = 0; i < 16; i++) {
        dest[0] = gMadClamp[(src[0] >> 16) & 0x1FF];
        dest[1] = gMadClamp[(src[1] >> 16) & 0x1FF];
        dest[2] = gMadClamp[(src[2] >> 16) & 0x1FF];
        dest[3] = gMadClamp[(src[3] >> 16) & 0x1FF];
        dest[4] = gMadClamp[(src[4] >> 16) & 0x1FF];
        dest[5] = gMadClamp[(src[5] >> 16) & 0x1FF];
        dest[6] = gMadClamp[(src[6] >> 16) & 0x1FF];
        dest[7] = gMadClamp[(src[7] >> 16) & 0x1FF];
        dest[8] = gMadClamp[(src[8] >> 16) & 0x1FF];
        dest[9] = gMadClamp[(src[9] >> 16) & 0x1FF];
        dest[10] = gMadClamp[(src[10] >> 16) & 0x1FF];
        dest[11] = gMadClamp[(src[11] >> 16) & 0x1FF];
        dest[12] = gMadClamp[(src[12] >> 16) & 0x1FF];
        dest[13] = gMadClamp[(src[13] >> 16) & 0x1FF];
        dest[14] = gMadClamp[(src[14] >> 16) & 0x1FF];
        dest[15] = gMadClamp[(src[15] >> 16) & 0x1FF];
        src += 16;
        dest += stride;
    }
}

// setluma for a U or V block: 8x8.
void setchroma(const s32* src, u8* dest, int stride) {
    int i;

    for (i = 0; i < 8; i++) {
        dest[0] = gMadClamp[(src[0] >> 16) & 0x1FF];
        dest[1] = gMadClamp[(src[1] >> 16) & 0x1FF];
        dest[2] = gMadClamp[(src[2] >> 16) & 0x1FF];
        dest[3] = gMadClamp[(src[3] >> 16) & 0x1FF];
        dest[4] = gMadClamp[(src[4] >> 16) & 0x1FF];
        dest[5] = gMadClamp[(src[5] >> 16) & 0x1FF];
        dest[6] = gMadClamp[(src[6] >> 16) & 0x1FF];
        dest[7] = gMadClamp[(src[7] >> 16) & 0x1FF];
        src += 8;
        dest += stride;
    }
}

// Starts decoding a picture: builds the tables on first use (madinit), loads the first 32 bits of
// src (its coded data) into the bit buffer and keeps motion (0 a key frame, 1 coded against a
// reference). quality scales MPEG-1's intra matrix into madquant (gMadIntraQuant[i] x quality / 8),
// with the IDCT's factors folded in (idctprescale); the DC coefficient's step does not change with
// quality (it is the quality-8 one).
void MAD_initdecode(u8* src, int motion, int quality) {
    int i;

    if (gbMadTablesBuilt == 0) {
        madinit();
    }
    madshiftreg = (MAD_ReadLittleEndian(src, 2) << 16) | MAD_ReadLittleEndian(src + 2, 2);
    madbitcount = 32;
    maddataptr = src + 4;
    gMadMotion = motion;
    madquant[0] = MAD_FixedMul(gMadIntraQuant[0] << 16, idctprescale[0]);
    for (i = 1; i < 64; i++) {
        madquant[i] = MAD_FixedMul((quality * gMadIntraQuant[i]) << 13, idctprescale[i]);
    }
}

// Decode one macroblock (16x16 Y, 8x8 U and V) into dest_y, dest_cb and dest_cr, width the Y rows'
// spacing (the U and V rows are half that). In a frame coded against a reference, a block with its
// bit set in the block pattern is the reference block (moved by the macroblock's motion) plus a
// level; the others are coded in full.
void MAD_decodemacroblock(u8* src_y, u8* src_cb, u8* src_cr, u8* dest_y, u8* dest_cb, u8* dest_cr,
                          int width) {
    int nHalf;
    u32 uPattern;
    int nAdd;
    int dx;
    int dy;
    int nOffset;

    nHalf = width >> 1;
    if (gMadMotion == 0) {
        uPattern = 0;
    } else if (!(madshiftreg & 0xC0000000)) {
        uPattern = 0;
        discardbits(2);
    } else {
        if (madshiftreg & 0x80000000) {
            uPattern = 0x3FF;
            discardbits(1);
        } else {
            uPattern = madshiftreg >> 24;
            discardbits(8);
        }
        dx = getdelta();
        dy = getdelta();
        src_y += dx + dy * width;
        nOffset = (dx >> 1) + (dy >> 1) * nHalf;
        src_cb += nOffset;
        src_cr += nOffset;
    }
    if (!(uPattern & 1)) {
        if (madvlcdecode() == 1) {
            dcblock(&gMadLumaBlock[0], 16);
        } else {
            idctcompute(&gMadLumaBlock[0], 16);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getluma(src_y, width, &gMadLumaBlock[0], nAdd);
    }
    if (!(uPattern & 2)) {
        if (madvlcdecode() == 1) {
            dcblock(&gMadLumaBlock[8], 16);
        } else {
            idctcompute(&gMadLumaBlock[8], 16);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getluma(src_y + 8, width, &gMadLumaBlock[8], nAdd);
    }
    if (!(uPattern & 4)) {
        if (madvlcdecode() == 1) {
            dcblock(&gMadLumaBlock[128], 16);
        } else {
            idctcompute(&gMadLumaBlock[128], 16);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getluma(src_y + width * 8, width, &gMadLumaBlock[128], nAdd);
    }
    if (!(uPattern & 8)) {
        if (madvlcdecode() == 1) {
            dcblock(&gMadLumaBlock[136], 16);
        } else {
            idctcompute(&gMadLumaBlock[136], 16);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getluma(src_y + width * 8 + 8, width, &gMadLumaBlock[136], nAdd);
    }
    if (!(uPattern & 0x10)) {
        if (madvlcdecode() == 1) {
            dcblock(gMadChromaBlocks[0], 8);
        } else {
            idctcompute(gMadChromaBlocks[0], 8);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getchroma(src_cb, nHalf, gMadChromaBlocks[0], nAdd);
    }
    if (!(uPattern & 0x20)) {
        if (madvlcdecode() == 1) {
            dcblock(gMadChromaBlocks[1], 8);
        } else {
            idctcompute(gMadChromaBlocks[1], 8);
        }
    } else {
        nAdd = getdelta() * 2 - 128;
        getchroma(src_cr, nHalf, gMadChromaBlocks[1], nAdd);
    }
    setluma(gMadLumaBlock, dest_y, width);
    setchroma(gMadChromaBlocks[0], dest_cb, nHalf);
    setchroma(gMadChromaBlocks[1], dest_cr, nHalf);
}

// Reads nBytes (1 to 4) bytes at pData as a little-endian number; 0 for any other count. The bit
// buffer's refills read 2 at a time.
u32 MAD_ReadLittleEndian(u8* pData, int nBytes) {
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

// a * b in 16.16 fixed point, rounded (through a 64-bit product).
s32 MAD_FixedMul(s32 a, s32 b) {
    return ((s64)a * b + 0x8000) >> 16;
}
