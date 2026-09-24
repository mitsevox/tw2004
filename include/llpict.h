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

// A decoded frame of the MAD codec (rcmp_mad_codec.c, fn_800B94CC): the same three planes.
typedef struct PictFrame {
    s32  nWidth;                // 0x00
    s32  nHeight;               // 0x04
    u8*  pPixels;               // 0x08
} PictFrame;

// What LLVideo.c hands LLPict_Gc.c for a movie: the decoder (fn_8002FEB0 makes it, 0x50 bytes)
// and its current frame.
typedef struct PictStream {
    void* pDecoder;             // 0x00
    PictFrame* pFrame;          // 0x04
} PictStream;

// LLPict_Gc.c: the planes of a picture and of a frame.
u8*  fn_800301D0(LLPict* pPict);    // V
u8*  fn_800301F4(LLPict* pPict);    // U
u8*  fn_8003020C(LLPict* pPict);    // Y
u8*  fn_80030214(PictFrame* pFrame);    // V
u8*  fn_80030234(PictFrame* pFrame);    // U
u8*  fn_8003024C(PictFrame* pFrame);    // Y
// A movie's picture and decoder (LLVideo.c): pfnRead(pArg) hands the decoder its next data.
void fn_8002FEB0(LLPict* pPict, PictStream* pStream, void* (*pfnRead)(void* pArg), void* pArg);
void fn_8002FF38(LLPict* pPict, PictStream* pStream);
void fn_8002FF94(LLPict* pPict, PictStream* pStream);                     // empty
void fn_8002FF98(LLPict* pPict, PictStream* pStream);
u8   fn_8003001C(LLPict* pPict, PictStream* pStream);
u8   fn_80030040(LLPict* pPict, PictStream* pStream);
void fn_8003009C(LLPict* pPict, PictStream* pStream, int n2);             // empty
void fn_800300A0(LLPict* pPict, PictStream* pStream);

// A picture from a "MADk" file in memory (LLPict_Gc.c).
LLPict* fn_8002FD00(u8* pData, u32 uSize);

// LLPictInt.c
LLPict* PictInt_Decode(PictFile* pFile);

#endif
