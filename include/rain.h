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

#endif
