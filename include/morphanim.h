// morphanim.h (our name): GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts),
// the list of morph animations the shader objects play.

#ifndef MORPHANIM_H
#define MORPHANIM_H

#include "engine.h"

// A morph animation, which also holds its shader object's display list (StaticShaderObject.anim,
// built by ShaderData_BuildDisplayList); only the fields known.
typedef struct MorphAnim {
    u32  nListSize;             // 0x00  the display list's size in bytes
    void* pList;                // 0x04  the display list
    u8   bNoPosArray;           // 0x08  1: the object's data counts more than one frame, so its list
                                //       sets no position array and the object's render sets it
    u8   nIndex;                // 0x09  its slot in the manager (fn_800975B0)
    u16  nFrames;               // 0x0A  0: nothing loaded
    f32  fC;                    // 0x0C
    u16* p10;                   // 0x10  } a vertex number per morphed vertex (fn_80097330); freed
    f32* p14;                   // 0x14  }   by MorphAnim_Free. Its position (three floats)
    s8*  p18;                   // 0x18  }   and its morph offset (three s8s)
    f32* p1C;                   // 0x1C  the positions (ShaderData_BuildDisplayList)
} MorphAnim;

// The manager (lbl_80281F70, 0x968 bytes, allocated by fn_80097208).
typedef struct MorphAnimMgr {
    s32  nCount;                // 0x000
    s32  nFrames;               // 0x004  the sum of the animations' nFrames
    u32  au8[(0x968 - 0x8) / 4];    // 0x008  per slot: the frame (gSession.nFrameCount) the
                                    //        animation was last brought up to date in (MorphAnim_Update)
} MorphAnimMgr;
LAYOUT_ASSERT(MorphAnimMgr, 0x968);

extern MorphAnimMgr* lbl_80281F70;

s32  MorphAnim_GetCount(void);
u32  MorphAnim_GetLastUpdateFrame(u8 nIndex); // lbl_80281F70->au8[nIndex]
void MorphAnim_Free(MorphAnim* pAnim);
void fn_8009727C(MorphAnim* pAnim);
// Records a vertex's morph offset (three s8s) while its display list is built.
void fn_80097330(MorphAnim* pAnim, u16 nVert, s8* pDelta);
void fn_80097474(MorphAnim* pAnim);
void MorphAnim_Update(MorphAnim* pAnim, f32 fWeight);

#endif
