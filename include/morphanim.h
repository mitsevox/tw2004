// morphanim.h (our name): GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts),
// the list of morph animations the shader objects play.

#ifndef MORPHANIM_H
#define MORPHANIM_H

#include "engine.h"

// A morph animation, which also holds its shader object's display list (the object's data + 4,
// built by GoShaderObjectCommon_ShaderObjectsData_Gc.c's fn_80074DA8); only the fields known.
typedef struct MorphAnim {
    u32  n0;                    // 0x00  the display list's size
    void* p4;                   // 0x04  the display list
    u8   b8;                    // 0x08  1: the object's data counts more than one frame; its list
                                //       then sets no position array
    u8   nIndex;                // 0x09  its slot in the manager (fn_800975B0)
    u16  nFrames;               // 0x0A  0: nothing loaded
    f32  fC;                    // 0x0C
    u16* p10;                   // 0x10  } a vertex number per morphed vertex (fn_80097330); freed
    f32* p14;                   // 0x14  }   by fn_80097624. Its position (three floats)
    s8*  p18;                   // 0x18  }   and its morph offset (three s8s)
    f32* p1C;                   // 0x1C  the positions (fn_80074DA8)
} MorphAnim;

// The manager (lbl_80281F70, 0x968 bytes, allocated by fn_80097208).
typedef struct MorphAnimMgr {
    s32  nCount;                // 0x000
    s32  nFrames;               // 0x004  the sum of the animations' nFrames
    u32  au8[(0x968 - 0x8) / 4];    // 0x008  per slot: the frame (gSession.nFrameCount) the
                                    //        animation was last brought up to date in (fn_800976A8)
} MorphAnimMgr;
LAYOUT_ASSERT(MorphAnimMgr, 0x968);

extern MorphAnimMgr* lbl_80281F70;

u32  fn_80097694(u8 nIndex);        // lbl_80281F70->au8[nIndex]
void fn_8009727C(MorphAnim* pAnim);
// Records a vertex's morph offset (three s8s) while its display list is built.
void fn_80097330(MorphAnim* pAnim, u16 nVert, s8* pDelta);
void fn_80097474(MorphAnim* pAnim);
void fn_800976A8(MorphAnim* pAnim, f32 fWeight);

#endif
