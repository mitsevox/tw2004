// greengrid.h (our name): GoGreenGrid.c's state, the grid drawn over the putting green
// (lbl_801E3068, 0x118 bytes, reached through lbl_802813C0). Per view: a mesh object and its
// vertex arrays, the grid's corner and direction, and the ground heights sampled so far.

#ifndef GREENGRID_H
#define GREENGRID_H

#include "engine.h"

typedef struct GreenGrid {
    u8   aMesh[2][0x28];        // 0x000  a mesh object per view (Skin.c's fn_80036054 sets it up,
                                //        fn_800360A0 frees it); the second only in split screen
    f32* apVert[2];             // 0x050  vertex positions, three floats each
    f32* apUV[2];               // 0x058  texture coordinates, two floats each
    u8*  apColor[2];            // 0x060  vertex colours, four bytes each
    s16* apIndex[2];            // 0x068  the line strip's vertex indexes
    s32  nVerts;                // 0x070  vertices built so far (GR_BuildGridRenderData)
    s32  nIndices;              // 0x074  indexes built so far
    f32* apHeight[2];           // 0x078  ground height per grid point, nCols x anRows[view]
    f32  aCorner[2][4];         // 0x080  the grid's first point (x, -, z)
    f32  aDir[2][4];            // 0x0A0  unit direction from the ball to the target, flat
    f32  aTarget[2][4];         // 0x0C0  the target the grid was laid out for
    s32  anDone[2];             // 0x0E0  grid points sampled so far (fn_8009BE08, 4 a frame)
    s32  nCols;                 // 0x0E8  points across (4)
    s32  anRows[2];             // 0x0EC  points along, per view
    f32  fCellW;                // 0x0F4  spacing across
    f32  fCellD;                // 0x0F8  spacing along
    f32  fFC;                   // 0x0FC
    f32  f100;                  // 0x100  0.125, scales the frame count for the texture scroll
    s32  b104;                  // 0x104  1; picks fn_80014118(0x70) over 0x60
    s32  anColor[4];            // 0x108  the vertex colour bytes, in order (0x80, 0x80, 0x40, 0x41)
} GreenGrid;
LAYOUT_ASSERT(GreenGrid, 0x118);

extern GreenGrid* lbl_802813C0;

#endif
