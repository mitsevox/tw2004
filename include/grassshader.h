// grassshader.h (our name): the grass shader object (GoShaderObject_Grass_Gc.c): the grass drawn in
// shells over the hole's terrain. Its type data holds the vertex buffer the shells are packed into
// (TW06's GrassPacket calls) and a pool of per-object render records.

#ifndef GRASSSHADER_H
#define GRASSSHADER_H

#include "engine.h"

// A shader object as the shader object table's callbacks get it (TW06: SD_SShaderObject_Static).
typedef struct SD_SShaderObject_Static {
    u32   unk0;
    void* pData;                // 0x4  the object's render data (for the grass, a node of the pool)
} SD_SShaderObject_Static;

// One word of the grass vertex buffer. A vertex is four words: x, y, z, then four bytes.
typedef union GrassWord {
    f32 f;
    u8  b[4];
} GrassWord;

// One shell's run of vertices in the buffer.
typedef struct GrassShell {
    GrassWord* pStart;          // 0x0
    u32        nVerts;          // 0x4
} GrassShell;

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
    void*      pParams;         // 0x36C  set by SD_vShaderObject_Grass_Type_SetParameters
    f32        f370;            // 0x370
    s8         n374;            // 0x374
    u8         unk375[3];
} SD_SShaderTypeData_Grass_Static;
LAYOUT_ASSERT(SD_SShaderTypeData_Grass_Static, 0x378);

extern SD_SShaderTypeData_Grass_Static* SD_gpGrassTypeData;

#endif
