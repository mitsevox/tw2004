// grassshader.h (our name): the grass shader object (GoShaderObject_Grass_Gc.c): the grass drawn in
// shells over the hole's terrain. Its type data holds the vertex buffer the shells are packed into
// (TW06's GrassPacket calls) and a pool of per-object render records.

#ifndef GRASSSHADER_H
#define GRASSSHADER_H

#include "engine.h"

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

// The grass parameters GoGrass.c hands over once per hole (SD_vSetGrassParamsOnce).
extern f32 lbl_802607D0[16][4];
extern f32 lbl_802608D0[4];
extern f32 lbl_802608E0[8];
extern f32 lbl_80260900[2][4];
extern f32 lbl_80260920[2][4];

#endif
