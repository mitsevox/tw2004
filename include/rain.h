#ifndef RAIN_H
#define RAIN_H

// rain.h (our name): GoShaderObject_Rain_Gc.c's state, the rain shader object (the only weather
// effect PsMgr.c drives on the GameCube). Only the fields read so far.

#include "engine.h"

// The rain's shared state (lbl_801F6320, reached through lbl_802814B8).
typedef struct RainState {
    s32       n0;               // 0x00  0 or 1, stepped by fn_800B4BD8
    s32       n4;               // 0x04  flipped by fn_800B4BB8
    f32       f8;               // 0x08  sqrt(3075), set by fn_800B4B5C
    TexBank*  pBank;            // 0x0C  } the "splash" texture (SD_vShaderObject_Rain_Dynamic_Init)
    TexEntry* pTex;             // 0x10  }
} RainState;
LAYOUT_ASSERT(RainState, 0x14);

extern RainState* lbl_802814B8;

// The drops' display list (fn_800B4C00 builds it).
typedef struct RainList {
    u32   uSize;                // 0x0
    void* pList;                // 0x4
} RainList;

// A point the drops' display list is drawn at (fn_800B4FA4), 18 to a RainData.apA buffer.
typedef struct RainPoint {
    f32 vPos[3];                // 0x00
    u8  aColor[4];              // 0x0C  the drops' colour array; drawn when [3] or a10[3] is set
    u8  a10[4];                 // 0x10
    u8  pad14[4];               // 0x14
} RainPoint;
LAYOUT_ASSERT(RainPoint, 0x18);

// A splash triangle (fn_800B4FA4), 25 to a RainData.apB buffer.
typedef struct RainSplash {
    f32 av[3][3];               // 0x00  its corners
    f32 fAlpha;                 // 0x24  0..1, drawn at half this
} RainSplash;
LAYOUT_ASSERT(RainSplash, 0x28);

#define RAIN_NUM_POINTS  18
#define RAIN_NUM_SPLASHES 25
#define RAIN_BUF_A_SIZE (RAIN_NUM_POINTS * sizeof(RainPoint))     // 0x1B0: RainData.apA's buffers
#define RAIN_BUF_B_SIZE (RAIN_NUM_SPLASHES * sizeof(RainSplash))  // 1000: RainData.apB's buffers

// A rain effect object's render data. Its size is not known yet.
typedef struct RainData {
    RainList    list;           // 0x00  900 drops
    RainPoint*  apA[4];         // 0x08  [RainState.n0 * 2 + RainState.n4], cleared
    RainSplash* apB[2];         // 0x18  [RainState.n0], cleared
} RainData;

// A rain effect object (PsMgr.c's lbl_801F16F4[0]).
typedef struct RainObject {
    u32      u0;                // 0x00
    RainData data;              // 0x04
} RainObject;

#endif
