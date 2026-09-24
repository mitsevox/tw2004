// grassshader.h (our name): the grass shader object (GoShaderObject_Grass_Gc.c): the grass drawn in
// shells over the hole's terrain. Its type data holds the vertex buffer the shells are packed into
// (TW06's GrassPacket calls) and a pool of per-object render records.

#ifndef GRASSSHADER_H
#define GRASSSHADER_H

#include "engine.h"
#include "camera.h"
#include "unsorted/cull.h"
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

// The grass file ('gras' stream object, fn_8011E584): a word giving the size of the part after its
// 16-byte header, then this header, n0 halfwords, a word count and 16 bytes on, the records.
typedef struct GrassFileHeader {
    s32 n0;                     // 0x0
    u16 uVersion;               // 0x4  100: n6/n8/nA are given
    s16 n6;                     // 0x6
    s16 n8;                     // 0x8
    s16 nA;                     // 0xA
    u8  unkC[4];
} GrassFileHeader;

typedef struct GrassTile {
    s32 n0;                     // 0x0  its buffer needs (n0 * 32 + 0x580) / 16 vertices (fn_8011F544)
    f32 f4;                     // 0x4  } heights: the cell's bounding sphere spans f4 to f8 + f3B8
    f32 f8;                     // 0x8  } (fn_8011F7F8)
    u8  unkC[0x10 - 0xC];
    u32 u10;                    // 0x10  } offsets from the file's start (after its first 16 bytes),
    u32 u14;                    // 0x14  } made into addresses on load
} GrassTile;

// One of GoGrass.c's 16 buffers (0x4C bytes): the array at GrassManager.pEC, which fn_8011FFCC
// makes and puts in the apDC slots; apD8 and apF0 hold pointers into it too.
typedef struct GrassBuffer {
    f32   f0;                   // 0x00  } the x, z it was placed at (fn_8011F544); handed on by
    f32   f4;                   // 0x04  } fn_8011F3AC
    f32   f8;                   // 0x08  fn_8011F544's third argument
    s32   nC;                   // 0x0C  1 (fn_8011F544)
    u8    b10;                  // 0x10  1 when placed
    u8    unk11[0x14 - 0x11];
    u8    a14[0x40 - 0x14];     // 0x14  a render object: fn_8000827C sets it up (type 17),
                                //       fn_80008248 when the buffer is put back
    void* p40;                  // 0x40  n44 16-byte vertices (fn_8011FFCC); freed with the buffers
    s32   n44;                  // 0x44  its vertex count: fn_8011FDEC picks the smallest big enough
    struct GrassTile* p48;      // 0x48  the file record it was built from
} GrassBuffer;

// What fn_8011F544 hands to a buffer's render object when it builds it (our name).
typedef struct GrassBufferDesc {
    struct GrassTile* pTile;    // 0x00
    void*             pVerts;   // 0x04  GrassBuffer.p40
    s32               nVerts;   // 0x08  GrassBuffer.n44
    f32               fX;       // 0x0C
    f32               fZ;       // 0x10
    f32               f14;      // 0x14  GrassManager.f3B8
} GrassBufferDesc;
LAYOUT_ASSERT(GrassBuffer, 0x4C);

// GoGrass.c's state (*lbl_80281900). Only the fields the decompiled code uses; its size is not known.
typedef struct GrassManager {
    TexEntry*    pTex;          // 0x00  } the grass texture named by n3A4 (akgrass1..4), found by
    TexBank*     pBank;         // 0x04  } GrassRender_vBuildAndUploadOneTimeData
    s16*         p8;            // 0x08  the grass file's n0 halfwords (fn_8011E584)
    struct GrassTile* pC;       // 0x0C  the grass file's n10 records
    s32          n10;           // 0x10
    s16          n14;           // 0x14  from a version-100 file's header, else -500
    s16          n16;           // 0x16  from a version-100 file's header, else -500
    s16          n18;           // 0x18  from a version-100 file's header, else 400
    s16          n1A;           // 0x1A  the header's n0 / n18
    s32          n1C;           // 0x1C  the chunks added (fn_8011E4D8); cleared when the grass is freed
    struct GrassChunkData* a20[10];   // 0x20  each chunk's data, byte-swapped in place
    struct GrassChunk*     a48[10];   // 0x48  the chunks
    void*        pCamera;       // 0x70  the grass's render camera, made from the three below (fn_8011E9D8)
    CamLens*     pLens;         // 0x74  a flat lens, 20 x 20
    GoFrameBuf*  pFrameBuf;     // 0x78  256 x 256
    f32*         pRect;         // 0x7C  its screen rectangle
    GxTexture    tex80;         // 0x80  the 256 x 256 screen copy (fn_8011E170)
    f32          fMinX;         // 0xB0  } the placed buffers' bounds (fn_8011F544); the lens looks
    f32          fMaxX;         // 0xB4  } down on the corner fMinX, fMinZ (fn_8011EB04)
    f32          fMinZ;         // 0xB8  }
    f32          fMaxZ;         // 0xBC  }
    s32          nC0;           // 0xC0  10 at start
    u8           unkC4[0xD8 - 0xC4];
    GrassBuffer** apD8;         // 0xD8  a stack of buffers (fn_8011FDC4 pushes, fn_8011FF58 empties)
    GrassBuffer** apDC;         // 0xDC  16 free buffers (fn_8011FD74 puts one back, fn_8011FDEC
                                //       takes the best fit)
    s32          nE0;           // 0xE0  the records at pEC
    u32          nE4;           // 0xE4  apD8's depth
    u32          nE8;           // 0xE8  apDC's used slots
    GrassBuffer* pEC;           // 0xEC  the nE0 buffers
    GrassBuffer** apF0[2];      // 0xF0  two buffer lists; n100 picks the one in use
    s32          anF8[2];       // 0xF8  their lengths
    s32          n100;          // 0x100
    s32          n104;          // 0x104
    // 0x108..0x230: the parameters GrassRender_vBuildAndUploadOneTimeData hands to
    // SD_vSetGrassParamsOnce.
    f32          af108[2][4];   // 0x108  from the grass lens: 1 + (its position - half its view) /
                                //        its view, then -1 / its view
    u8           unk128[0x148 - 0x128];
    f32          af148[2][4];   // 0x148  0, 0, 0, 1 + f3C8, then 1, 1, 1, -1 / f3C4
    f32          af168[8];      // 0x168  eight tuning values set at start (fn_8011E170)
    u8           unk188[0x208 - 0x188];
    f32          af208[4];      // 0x208  f3BC / 255 three times, then 0.5
    f32          af218[4];      // 0x218  f3C0 * f3F8 / 255 three times, then 0.5
    f32          f228;          // 0x228  16 * f410 * cos(f40C) (fn_8011E6E8)
    f32          f22C;          // 0x22C  16 * f410 * sin(f40C)
    f32          av230[16][4];  // 0x230  16 points around a circle of radius f41C, phase f414
    u8           unk330[0x348 - 0x330];
    // 0x348..0x370: the block fn_8011F3AC hands to fn_8003519C (row 17) once per buffer and pass.
    f32          f348;          // 0x348  f3D0
    f32          f34C;          // 0x34C  the buffer's f0
    f32          f350;          // 0x350  the buffer's f4
    f32          f354;          // 0x354  f3B4
    f32          f358;          // 0x358  |z| of the camera's flattened direction
    f32          f35C;          // 0x35C  |x| of it
    s32          n360;          // 0x360  0 when its z is below 0, else 1
    s32          n364;          // 0x364  0 when its x is below 0, else 1
    f32          f368;          // 0x368  f3B8
    s32          n36C;          // 0x36C  the pass (0 or 1)
    void*        p370;          // 0x370  an allocation; set while the grass is on (fn_8012022C)
    u8           unk374[0x3A4 - 0x374];
    s32          n3A4;          // 0x3A4  the last chunk's GrassChunkData.n0
    // 0x3A8..0x420: values fn_8011E170 sets at start (given there).
    f32          f3A8;          // 0x3A8  2.5
    f32          f3AC;          // 0x3AC  2.5
    f32          f3B0;          // 0x3B0  the height the grass lens looks down from (fn_8011EB04); 500
    f32          f3B4;          // 0x3B4  0.6
    f32          f3B8;          // 0x3B8  the last chunk's GrassChunkData.f4; 0.13
    f32          f3BC;          // 0x3BC  128
    f32          f3C0;          // 0x3C0  128
    f32          f3C4;          // 0x3C4  1.5
    f32          f3C8;          // 0x3C8  2.6
    s32          n3CC;          // 0x3CC  0
    f32          f3D0;          // 0x3D0  128
    f32          f3D4;          // 0x3D4  2.5914
    f32          f3D8;          // 0x3D8  2.5914
    s32          n3DC;          // 0x3DC  1
    s32          n3E0;          // 0x3E0  1
    s32          n3E4;          // 0x3E4  1
    f32          f3E8;          // 0x3E8  4.9
    f32          f3EC;          // 0x3EC  0
    s32          n3F0;          // 0x3F0  1
    u8           unk3F4[0x3F8 - 0x3F4];
    f32          f3F8;          // 0x3F8
    f32          f3FC;          // 0x3FC  0
    f32          f400;          // 0x400  -0.36
    s32          n404;          // 0x404  } buffers placed with nC 1 / otherwise (fn_8011F544)
    s32          n408;          // 0x408  }
    f32          f40C;          // 0x40C  0.78
    f32          f410;          // 0x410  0.16
    f32          f414;          // 0x414  0
    f32          f418;          // 0x418  0.025
    f32          f41C;          // 0x41C  0.025
} GrassManager;

extern GrassManager* lbl_80281900;
extern s32 lbl_80282514;        // cleared when the grass is freed (fn_8011E3B4)
extern void* lbl_80282510;      // the grass's 256x256 texture buffer (fn_8011EB80)
extern GXTexObj lbl_8026038C;   // its texture

// The render object fn_8011F7F8 culls each grass cell with: its bounding sphere is set to the
// cell's before the test.
extern RenderObj     lbl_80260360;
extern RenderObjData lbl_802602C0;

// GoGrass.c's draw data (not in C yet).
extern char lbl_80194598[4][8]; // the grass textures' names, "akgrass1".."akgrass4" (by n3A4)
extern f32 lbl_801945B8[4];     // a colour: 0.21, 0.31, 0.1, 1 (fn_8011EC84)
extern f32 lbl_801945C8[8];     // the unit square's corners (fn_8011EC84)
extern f32 lbl_801945E8[4];     // a colour: 0.5 each (fn_8011EE4C)
extern f32 lbl_801945F8[8];     // (fn_8011EE4C)
extern f32 lbl_80194618[8];     // (fn_8011EE4C)

void SD_vSetGrassParamsOnce(f32* pUnused0, f32* pUnused1, f32 (*a2)[4], f32* p8, f32 (*b2)[4],
                            f32 (*a16)[4], f32 fA, f32 fB);

// The grass parameters GoGrass.c hands over once per hole (SD_vSetGrassParamsOnce).
extern f32 lbl_802607D0[16][4];
extern f32 lbl_802608D0[4];
extern f32 lbl_802608E0[8];
extern f32 lbl_80260900[2][4];
extern f32 lbl_80260920[2][4];

#endif
