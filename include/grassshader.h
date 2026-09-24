// grassshader.h (our name): the grass shader object (GoShaderObject_Grass_Gc.c): the grass drawn in
// shells over the hole's terrain. Its type data holds the vertex buffer the shells are packed into
// (TW06's GrassPacket calls) and a pool of per-object render records.

#ifndef GRASSSHADER_H
#define GRASSSHADER_H

#include "engine.h"
#include "gx.h"

// One word of the grass vertex buffer. A vertex is four words: x, y, z, then four bytes: n374 at
// the time, 1, the row's vertex count (on a row's first vertex) and 1 on a row's first vertex.
// The buffer is walked in words (GrassPacket_iEndPacket divides the word count by 4).
typedef union GrassWord {
    f32 f;
    u8  b[4];
} GrassWord;

// One shell's run of vertices in the buffer.
typedef struct GrassShell {
    GrassWord* pStart;          // 0x0
    u32        nVerts;          // 0x4
} GrassShell;

// The grass parameters SD_vShaderObject_Grass_Type_SetParameters is given (what the drawing reads).
typedef struct GrassParams {
    u32 unk0;
    f32 a04[3];                 // 0x04  [0] and [1]: the x and z the fade is measured from; [nAxis]
                                //       also the base of the grass texture's s
    f32 a10[2];                 // 0x10  per shell set: the opacity
    s32 a18[2];                 // 0x18  per shell set: which of its two vertex runs to draw
    f32 f20;                    // 0x20  the height of the upper vertices
    s32 n24;                    // 0x24  which shell set to draw
} GrassParams;

// A grass object's render record (one 32-byte node of the type's pool): two sets of two vertex runs.
typedef struct GrassRenderData {
    GrassWord* apVerts[2][2];   // 0x00
    s32        anVerts[2][2];   // 0x10
} GrassRenderData;

// The grass type's data (TW06: SD_SShaderTypeData_Grass_Static, SD_gGrassTypeData).
typedef struct SD_SShaderTypeData_Grass_Static {
    u16        a000[2][128];    // 0x000
    u8         a200[2][128];    // 0x200
    GrassShell aShells[10];     // 0x300
    GrassWord* pBase;           // 0x350  the start of the packet being built
    GrassWord* pCur;            // 0x354  the next free vertex
    GrassWord* pEnd;            // 0x358  the end of the buffer
    GrassWord* p35C;            // 0x35C
    GrassWord* pRow;            // 0x360  the first vertex of the current row
    u32        unk364;
    UMemPool*  pPool;           // 0x368  32 render records
    GrassParams* pParams;       // 0x36C  set by SD_vShaderObject_Grass_Type_SetParameters
    f32        f370;            // 0x370
    s8         n374;            // 0x374
    u8         unk375[3];
} SD_SShaderTypeData_Grass_Static;
LAYOUT_ASSERT(SD_SShaderTypeData_Grass_Static, 0x378);

extern SD_SShaderTypeData_Grass_Static* SD_gpGrassTypeData;

// One of GoGrass.c's 0x4C-byte records (GrassManager.pEC).
typedef struct GrassRecord {
    u8    unk0[0x40];
    void* p40;                  // 0x40  freed with the records (fn_80120194)
    u8    unk44[0x4C - 0x44];
} GrassRecord;

// A grass data chunk as fn_8011E4D8 is given it: n2 0x30-byte entries follow the header, then the
// data (GrassChunkData).
typedef struct GrassChunk {
    s16 unk0;
    s16 n2;                     // 0x2
} GrassChunk;

typedef struct GrassChunkData {
    s32 n0;                     // 0x0
    f32 f4;                     // 0x4
} GrassChunkData;

// GoGrass.c's state (*lbl_80281900). Only the fields the decompiled code uses; its size is not known.
typedef struct GrassManager {
    u8           unk0[0x1C];
    s32          n1C;           // 0x1C  the chunks added (fn_8011E4D8); cleared when the grass is freed
    struct GrassChunkData* a20[10];   // 0x20  each chunk's data, byte-swapped in place
    struct GrassChunk*     a48[10];   // 0x48  the chunks
    void*        p70;           // 0x70
    void*        p74;           // 0x74
    void*        p78;           // 0x78
    void*        p7C;           // 0x7C
    u8           unk80[0xD8 - 0x80];
    void**       apD8;          // 0xD8  a stack of objects (fn_8011FDC4 pushes, fn_8011FF58 empties)
    void**       apDC;          // 0xDC  16 slots (fn_8011FD74 fills the first free one)
    s32          nE0;           // 0xE0  the records at pEC
    u32          nE4;           // 0xE4  apD8's depth
    s32          nE8;           // 0xE8  apDC's used slots
    GrassRecord* pEC;           // 0xEC
    void**       apF0[2];       // 0xF0  two object lists; n100 picks the one in use
    s32          anF8[2];       // 0xF8  their lengths
    s32          n100;          // 0x100
    u8           unk104[0x370 - 0x104];
    void*        p370;          // 0x370  an allocation; set while the grass is on (fn_8012022C)
    u8           unk374[0x3A4 - 0x374];
    s32          n3A4;          // 0x3A4  the last chunk's GrassChunkData.n0
    u8           unk3A8[0x3B8 - 0x3A8];
    f32          f3B8;          // 0x3B8  the last chunk's GrassChunkData.f4
    u8           unk3BC[0x3CC - 0x3BC];
    s32          n3CC;          // 0x3CC
    u8           unk3D0[0x3E0 - 0x3D0];
    s32          n3E0;          // 0x3E0
} GrassManager;

extern GrassManager* lbl_80281900;
extern s32 lbl_80282514;        // cleared when the grass is freed (fn_8011E3B4)
extern void* lbl_80282510;      // the grass's 256x256 texture buffer (fn_8011EB80)
extern GXTexObj lbl_8026038C;   // its texture

// The grass parameters GoGrass.c hands over once per hole (SD_vSetGrassParamsOnce).
extern f32 lbl_802607D0[16][4];
extern f32 lbl_802608D0[4];
extern f32 lbl_802608E0[8];
extern f32 lbl_80260900[2][4];
extern f32 lbl_80260920[2][4];

#endif
