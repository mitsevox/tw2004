// Code80070EC4.c (our name): made by fold.py from 6 sweep files. The static shader object hooks
// of rows 4 and 2 of lbl_80188E88 (row 2's render plays its morph animation).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "shaderdata.h"
#include "terrain.h"

void fn_80070EC4(TerWaveData* pWave);
void fn_80070F00(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070F34(StaticShaderObject* pObj);
void fn_80070F5C(StaticShaderObject* pObj);
void fn_80070FB0(f32* pfWeight);
void fn_80070FBC(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070FF0(StaticShaderObject* pObj);
void fn_80071028(StaticShaderObject* pObj);
void fn_80070168(void);     // sweep_80070168.c: calls a display list
void fn_8007524C(void);     // GoShaderObjectCommon_ShaderObjectsData_Gc.c: empty
s32 fn_80097688(void);      // GoShaderObjectCommon_MorphAnimManager_Gc.c: how many animations
void fn_80097624(MorphAnim* pAnim);     // GoShaderObjectCommon_MorphAnimManager_Gc.c: free its data

// Row 4's data hook: set the texture animation clock from the frame count.
void fn_80070EC4(TerWaveData* pWave) {
    fn_80076C20(pWave->nFrame);
}

// Row 4's static init: build the object's display list from pCmds.
void fn_80070F00(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 4's static close; it also forgets every texture animation.
void fn_80070F34(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
    fn_80076E24();
}

// Row 4's static render: the display list (with the object's positions when the list sets none).
void fn_80070F5C(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Row 2's data hook: the morph weight.
void fn_80070FB0(f32* pfWeight) {
    lbl_80281E78 = *pfWeight;
}

// Row 2's static init: build the object's display list from pCmds.
void fn_80070FBC(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 2's static close: free the morph animation's data.
void fn_80070FF0(StaticShaderObject* pObj) {
    fn_80097624(&pObj->anim);
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 2's static render: the display list, with the morph animation brought up to date first once
// in every two frames (three in split screen), each animation on its own frame of the cycle.
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
