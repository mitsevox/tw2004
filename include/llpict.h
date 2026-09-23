// llpict.h (our name): EA's low-level pictures (LLPict_Gc.c, LLPictInt.c): a picture in memory
// (0x74 bytes, its pixels as three planes, Y then U then V), and the picture file LLPictInt.c
// decodes into one ("MADk", EA's MAD codec, stored little-endian).

#ifndef LLPICT_H
#define LLPICT_H

#include "engine.h"

typedef struct LLPict {
    u8   unk0[0x60];
    u8*  pPixels;               // 0x60  Y (nWidth * nHeight), then U and V (a quarter of that each)
    s32  nWidth;                // 0x64
    s32  nHeight;               // 0x68
    f32  f6C;                   // 0x6C
    f32  f70;                   // 0x70
} LLPict;
LAYOUT_ASSERT(LLPict, 0x74);

// A "MADk" picture file (its header is little-endian; PictInt_Decode swaps it in place while it
// works and back after).
typedef struct PictFile {
    u32  uMagic;                // 0x00  'MADk'
    u8   unk4[0xC - 0x4];
    u32  uC;                    // 0x0C
    s16  nWidth;                // 0x10
    s16  nHeight;               // 0x12
    u8   unk14;
    u8   n15;                   // 0x15  given to fn_800B8528
    u8   unk16[2];
    u8   aData[4];              // 0x18  the coded picture (any length)
} PictFile;

// LLPict_Gc.c: the picture's planes.
u8*  fn_800301D0(LLPict* pPict);    // V
u8*  fn_800301F4(LLPict* pPict);    // U
u8*  fn_8003020C(LLPict* pPict);    // Y

// LLPictInt.c
LLPict* PictInt_Decode(PictFile* pFile);

#endif
