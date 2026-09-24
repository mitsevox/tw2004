// obfdata.h (our name): GoShaderObjectContainer_OBFData_Gc.c (EA's name, from its asserts). An
// OBFData (our name) holds eight buffers loaded from chunks: five by index (kind 2) and one each
// for kinds 0, 1 and 3, with each buffer's n02.

#ifndef OBFDATA_H
#define OBFDATA_H

#include "engine.h"

// One chunk: n02 * n04 bytes of data after an 8-byte head.
typedef struct OBFChunk {
    u8   nKind;                 // 0x0  0, 1 and 3: one buffer each; 2: aBufs[nIndex]
    u8   nIndex;                // 0x1
    u16  n02;                   // 0x2
    u16  n04;                   // 0x4
    u8   unk6[2];
    u8   aData[1];              // 0x8  (n02 * n04 bytes)
} OBFChunk;

// A count and that many chunks.
typedef struct OBFChunkList {
    s32  nChunks;               // 0x0
    OBFChunk* apChunks[1];      // 0x4  (nChunks of them)
} OBFChunkList;

typedef struct OBFData {
    u8*  aBufs[5];              // 0x00  kind 2's, by nIndex
    u8*  pBuf1;                 // 0x14  kind 1's
    u8*  pBuf0;                 // 0x18  kind 0's
    u8*  pBuf3;                 // 0x1C  kind 3's
    u16  an20[5];               // 0x20  aBufs' chunks' n02
    u16  n2A;                   // 0x2A  pBuf1's chunk's n02
    u16  n2C;                   // 0x2C  pBuf0's
    u16  n2E;                   // 0x2E  pBuf3's
} OBFData;
LAYOUT_ASSERT(OBFData, 0x30);

void fn_8006FCDC(OBFData* pData, OBFChunk* pChunk);
void fn_8006FDD4(OBFData* pData, OBFChunkList* pList);
void fn_8006FE44(OBFData* pData);

#endif
