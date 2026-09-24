// Code80070EC4.c (our name): made by fold.py from 6 sweep files. The static shader object hooks
// of rows 4 and 2 of lbl_80188E88 (row 2's render plays its morph animation).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "shaderdata.h"
#include "terrain.h"

void fn_80070EC4(TerWaveData* pWave);
void fn_80071028(StaticShaderObject* pObj);
s32 fn_80097688(void);      // GoShaderObjectCommon_MorphAnimManager_Gc.c: how many animations

// Row 4's data hook: set the texture animation clock from the frame count.
void fn_80070EC4(TerWaveData* pWave) {
    fn_80076C20(pWave->nFrame);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80074DA8();
void fn_80070F00(u8* p0, s32 p1);
void fn_8007524C();
void fn_80076E24();
void fn_80070F34(u8* p0);
s32 fn_80070168(s32, s32);
void fn_80070F5C(void* arg0);
void fn_80070FB0(u8* p0);
void fn_80070FBC(u8* p0, s32 p1);
void fn_80097624();
void fn_80070FF0(u8* p0);

void fn_80070F00(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070F34(u8* p0) {
    fn_8007524C((p0 + 0x4));
    fn_80076E24();
}

void fn_80070F5C(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80070FB0(u8* p0) {
    lbl_80281E78 = *(f32*)p0;
}

void fn_80070FBC(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070FF0(u8* p0) {
    fn_80097624((p0 + 0x4));
    fn_8007524C((p0 + 0x4));
}

// ---- end of sweep code ----

// Row 2's static render: the display list, with the morph animation brought up to date first once in every
// two frames (three in split screen), each animation on its own frame of the cycle.
void fn_80071028(StaticShaderObject* pObj) {
    MorphAnim* pAnim = &pObj->anim;
    int nEvery = (gSession.nSplitScreen != 0) + 2;

    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    if (fn_80097688() != 0 && (u32)gSession.nFrameCount % nEvery == pAnim->nIndex % nEvery &&
        (u32)gSession.nFrameCount > fn_80097694(pAnim->nIndex)) {
        fn_800976A8(pAnim, lbl_80281E78);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}
