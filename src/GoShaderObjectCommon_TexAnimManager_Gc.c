// GoShaderObjectCommon_TexAnimManager_Gc.c (EA's name, from its asserts): the texture animation
// manager (engine.h's TexAnimManager), scrolling textures on course objects. Each animation owns a
// 2x4 texture matrix; every frame the matrices are moved along by the clock and handed to GX as its
// texture matrix array.

#include "game_types.h"
#include "engine.h"
#include "gx.h"
#include "core/startup.h"

void fn_80076BF0(void);
u16 fn_80076D68(f32 fU, f32 fV, f32 fScale, f32 fInv);

// .sbss, in reverse address order (CodeWarrior lays it out last-defined-first).
f32 lbl_80281EC4;                   // the frame count the clock was last set from
TexAnimManager* lbl_80281EC0;       // the manager

// Make the manager, empty.
void fn_80076B7C(void) {
    TexAnimManager* pMgr;

    pMgr = fn_80009B34(sizeof(TexAnimManager), 2, 32, "GoShaderObjectCommon_TexAnimManager_Gc.c", 100);
    lbl_80281EC0 = pMgr;
    fn_80005AE8(pMgr, 0, sizeof(TexAnimManager));
}

// Free the manager.
void fn_80076BC4(void) {
    fn_80009E70(lbl_80281EC0);
    lbl_80281EC0 = 0;
}

// Point GX's texture matrix array at the animations' matrices.
void fn_80076BF0(void) {
    GXSetArray(23, lbl_80281EC0->aMtx, 32);
}

// Set the clock from the frame count and move every animation's matrix: the fraction of rate times
// clock, snapped to fScale steps.
void fn_80076C20(f32 fFrame) {
    TexAnim* pAnim;
    int i;
    f32 fU;
    f32 fV;
    f32 fStepU;
    f32 fStepV;
    f32 fScale;
    f32 fTime;

    if (fFrame != lbl_80281EC4) {
        lbl_80281EC4 = fFrame;
        fTime = fFrame;
        fTime *= 0.00001f;      // fake match: a copy, then the multiply, keeps fFrame first in fmuls
        lbl_80281EC0->fTime = fTime;
        for (i = 0; i < lbl_80281EC0->nAnims; i++) {
            pAnim = &lbl_80281EC0->aAnims[i];
            fU = pAnim->fU * fTime;
            fV = pAnim->fV * fTime;
            fStepU = fU - (s32)fU;
            fStepV = fV - (s32)fV;
            fScale = pAnim->fScale;
            fStepU = fStepU * fScale;
            fStepV = fStepV * fScale;
            lbl_80281EC0->aMtx[i][0][3] = (s32)fStepU * pAnim->fInv;
            lbl_80281EC0->aMtx[i][1][3] = (s32)fStepV * pAnim->fInv;
        }
        DCStoreRange(lbl_80281EC0->aMtx, lbl_80281EC0->nAnims * sizeof(lbl_80281EC0->aMtx[0]));
    }
}

// The animation with these values: an existing one, or a new one with an unmoved matrix.
u16 fn_80076D68(f32 fU, f32 fV, f32 fScale, f32 fInv) {
    int i;
    TexAnimManager* pMgr = lbl_80281EC0;
    TexAnim* pAnim;

    for (i = 0; i < pMgr->nAnims; i++) {
        pAnim = &pMgr->aAnims[i];
        if (pAnim->fU == fU && pAnim->fV == fV && pAnim->fScale == fScale && pAnim->fInv == fInv) {
            return i;
        }
    }
    pAnim = &pMgr->aAnims[i];
    pAnim->fU = fU;
    pAnim->fV = fV;
    pAnim->fScale = fScale;
    pAnim->fInv = fInv;
    lbl_80281EC0->nAnims++;
    lbl_80281EC0->aMtx[i][0][0] = 1.0f;
    lbl_80281EC0->aMtx[i][1][1] = 1.0f;
    return i;
}

// The clock.
f32 fn_80076E18(void) {
    return lbl_80281EC0->fTime;
}

// Forget every animation.
void fn_80076E24(void) {
    if (lbl_80281EC0 != NULL) {
        lbl_80281EC0->nAnims = 0;
        lbl_80281EC0->fTime = 0.0f;
    }
}
