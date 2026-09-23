// morphanim.h (our name): GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts),
// the list of morph animations the shader objects play.

#ifndef MORPHANIM_H
#define MORPHANIM_H

#include "engine.h"

// A morph animation; only the fields the manager uses.
typedef struct MorphAnim {
    u8   unk0[9];
    u8   nIndex;                // 0x09  its slot in the manager (fn_800975B0)
    u16  nFrames;               // 0x0A  0: nothing loaded
    f32  fC;                    // 0x0C
    void* p10;                  // 0x10  } freed by fn_80097624
    void* p14;                  // 0x14  }
    void* p18;                  // 0x18  }
} MorphAnim;

// The manager (lbl_80281F70, 0x968 bytes, allocated by fn_80097208).
typedef struct MorphAnimMgr {
    s32  nCount;                // 0x000
    s32  nFrames;               // 0x004  the sum of the animations' nFrames
    void* ap8[(0x968 - 0x8) / 4];   // 0x008
} MorphAnimMgr;
LAYOUT_ASSERT(MorphAnimMgr, 0x968);

extern MorphAnimMgr* lbl_80281F70;

#endif
