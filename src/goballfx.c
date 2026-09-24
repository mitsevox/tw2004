// goballfx.c (TW06's golf/hi-rendering/goballfx.c; BFX_vInit is paired with TW06's): the scene's
// light sets and the ball marker (include/lighting.h). It keeps four sets of five lights (four point
// lights and a directional one), fills the current set from the hole's lights (fn_800935CC) or from
// a caller's settings (fn_80093854), and draws a small marker quad on the ground under the ball.

#include "lighting.h"
#include "camera.h"
#include "ball.h"

void fn_80035FFC(void);
void fn_80093990(LightSet* pSet);
void fn_800939CC(LightSet* pSet);
f32  fn_80093A04(s32 nLight, CamLens* pLens);
f32  fn_80093A50(GoLight* pLight, CamLens* pLens);

// GoLighting.c
void fn_8006F144(LightSet* pSet);
void fn_8006F148(LightSet* pSet);
void fn_8006F400(LightSet* pSet, s32 nLight, f32* pVec, f32 f);
void fn_8006F430(f32 f);

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);
void fn_800360D4(u8* pMesh);
void fn_80036100(u8* pMesh, void* pDesc, int n);

// GoTerrainCollision.c
f32 fn_8004D80C(CourseInfo* pCourse, f32* pPos);

void fn_80093524(void) {
    int i;
    LightSet* pSet = lbl_80281380->aSet;
    for (i = 0; i < NUM_LIGHT_SETS; i++) {
        fn_80093990(pSet);
        pSet++;
    }
    fn_80035338(0);
    fn_80035FFC();
    fn_80035308();
}

void fn_80093580(void) {
    int i;
    LightSet* pSet = lbl_80281380->aSet;
    for (i = 0; i < NUM_LIGHT_SETS; i++) {
        fn_800939CC(pSet);
        pSet++;
    }
}

// Fill the current set from the hole's lights: the directional light from the first directional
// record (the fifth record if none of the first four is one), the point lights from the records
// around it.
void fn_800935CC(CourseLights* pLights) {
    LightSet* pSet;
    GoLight* pLight;
    CourseLight* pRec;
    CourseLight* pDir;
    u8 bSkip;
    int i;

    pSet = fn_8003532C();
    for (i = 0; i < 5; i++) {
        pDir = &pLights->aLight[i];
        if (pDir->nType == 1) break;
    }
    pLight = pSet->group.apLight[4];
    pLight->nType = 1;
    fn_8000AE28(pDir->vColor, 0.5f, pLight->u.dir.vColor);
    pLight->u.dir.f10 = 1.0f;
    pLight->u.dir.fC = 1.0f;

    bSkip = 0;
    if (pLights->aLight[0].nType == 1) {
        bSkip = 1;
    }
    if (bSkip) {
        pRec = &pLights->aLight[1];
    } else {
        pRec = &pLights->aLight[0];
    }
    pLight = pSet->group.apLight[0];
    pLight->nType = 2;
    fn_8000AE28(pRec->vColor, 0.5f, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bSkip = 0;
    if (pLights->aLight[0].nType == 1) {
        bSkip = 1;
    }
    if (pLights->aLight[1].nType == 1) {
        bSkip = 1;
    }
    if (bSkip) {
        pRec = &pLights->aLight[2];
    } else {
        pRec = &pLights->aLight[1];
    }
    pLight = pSet->group.apLight[1];
    pLight->nType = 2;
    fn_8000AE28(pRec->vColor, 0.5f, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bSkip = 0;
    if (pLights->aLight[0].nType == 1) {
        bSkip = 1;
    }
    if (pLights->aLight[1].nType == 1) {
        bSkip = 1;
    }
    if (pLights->aLight[2].nType == 1) {
        bSkip = 1;
    }
    if (bSkip) {
        pRec = &pLights->aLight[3];
    } else {
        pRec = &pLights->aLight[2];
    }
    pLight = pSet->group.apLight[2];
    pLight->nType = 2;
    fn_8000AE28(pRec->vColor, 0.5f, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    // EA bug: the last point light always takes the fourth record, even when that is the
    // directional one
    pLight = pSet->group.apLight[3];
    pLight->nType = 2;
    fn_8000AE28(pLights->aLight[3].vColor, 0.5f, pLight->u.point.vColor);
    Vec_Copy(pLights->aLight[3].vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;
}

void fn_80093824(void) {
    fn_8003532C();
    fn_80093A04(0, fn_8001F004());
}

void fn_80093854(LightParams* pParams) {
    LightSet* pSet;
    GoLight* pLight;
    pSet = fn_8003532C();
    Vec3Copy(pParams->v0, pSet->group.v28);
    pLight = pSet->group.apLight[4];
    pLight->u.dir.f10 = pParams->f10;
    pLight = pSet->group.apLight[0];
    pLight->u.point.fC = pParams->f20;
    pLight->u.point.f10 = pParams->f24;
    pLight->u.point.f14 = pParams->f24;
    pLight = pSet->group.apLight[1];
    pLight->u.point.fC = pParams->f1C;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight = pSet->group.apLight[2];
    pLight->u.point.fC = pParams->f18;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight = pSet->group.apLight[3];
    pLight->u.point.fC = pParams->f14;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
}

void fn_80093900(CourseLightBlock* pBlock) {
    LightSet* pSet;
    pSet = fn_8003532C();
    fn_8006F430(pBlock->f80);
    fn_8006F400(pSet, 0, pBlock->a[0].v0, pBlock->a[0].f10);
    fn_8006F400(pSet, 1, pBlock->a[1].v0, pBlock->a[1].f10);
    fn_8006F400(pSet, 2, pBlock->a[2].v0, pBlock->a[2].f10);
    fn_8006F400(pSet, 3, pBlock->a[3].v0, pBlock->a[3].f10);
}

void fn_80093990(LightSet* pSet) {
    fn_8006E5A8(&pSet->group, NUM_SET_LIGHTS);
    fn_8006F144(pSet);
}

void fn_800939CC(LightSet* pSet) {
    fn_8006E62C(&pSet->group);
    fn_8006F148(pSet);
}

// The angle between the camera's direction and light nLight of the current set.
f32 fn_80093A04(s32 nLight, CamLens* pLens) {
    return fn_80093A50(fn_8003532C()->group.apLight[nLight], pLens);
}

f32 fn_80093A50(GoLight* pLight, CamLens* pLens) {
    return fn_80009614(fn_8000C5FC(pLens->v24, pLight->u.point.vPos));
}

void BFX_vInit(void) {
    s32 desc[2];
    fn_800102DC(fn_8000BEE4("marker"), &lbl_80281F44, &lbl_80281F48);
    desc[0] = 0x10;
    desc[1] = 4;
    fn_80036054(lbl_801D94B0, 0, desc);
}

void fn_80093AD4(void) {
    lbl_80281F40 = 1;
}

// Draws the player's ball marker: a quad 0.04 across in the player's colour, just above the ground
// under the ball; nothing where there is no ground.
void fn_80093AE0(Ball* pBall, int nPlayer) {
    s16 aIndex[4] = {0, 1, 2, 3};
    TrailMeshDesc desc;
    f32* pPos;
    u8* pColour;
    f32* pUV;
    f32 fGround;
    f32 fY;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    fGround = fn_8004D80C(fn_8000C594(), pBall->vPos);
    if (-65536.125f != fGround) {
        fn_8005CC64(lbl_80281F44, lbl_80281F48);
        fn_80016B9C();
        fn_80035118(4, 5);
        pPos = lbl_801D95C8[nPlayer];
        fY = 0.01f + fGround;
        pPos[0] = pBall->vPos[0] - 0.02f;
        pPos[1] = fY;
        pPos[2] = pBall->vPos[2] - 0.02f;
        pPos[3] = pBall->vPos[0] - 0.02f;
        pPos[4] = fY;
        pPos[5] = pBall->vPos[2] + 0.02f;
        pPos[6] = pBall->vPos[0] + 0.02f;
        pPos[7] = fY;
        pPos[8] = pBall->vPos[2] - 0.02f;
        pPos[9] = pBall->vPos[0] + 0.02f;
        pPos[10] = fY;
        pPos[11] = pBall->vPos[2] + 0.02f;
        r = lbl_80189CB0[nPlayer][0];
        g = lbl_80189CB0[nPlayer][1];
        b = lbl_80189CB0[nPlayer][2];
        a = lbl_80189CB0[nPlayer][3];
        pColour = lbl_801D9578[nPlayer];
        pUV = lbl_801D94D8[nPlayer];
        pColour[0] = r;
        pColour[1] = g;
        pColour[2] = b;
        pColour[3] = a;
        pColour[4] = r;
        pColour[5] = g;
        pColour[6] = b;
        pColour[7] = a;
        pColour[8] = r;
        pColour[9] = g;
        pColour[10] = b;
        pColour[11] = a;
        pColour[12] = r;
        pColour[13] = g;
        pColour[14] = b;
        pColour[15] = a;
        pUV[0] = 0.0f;
        pUV[1] = 0.0f;
        pUV[2] = 0.0f;
        pUV[3] = 1.0f;
        pUV[4] = 1.0f;
        pUV[5] = 0.0f;
        pUV[6] = 1.0f;
        pUV[7] = 1.0f;
        fn_80014118(0x70);
        fn_80035138(0);
        fn_80012EF8();
        desc.n0 = 4;
        desc.nVerts = 4;
        desc.pDraw = NULL;
        desc.pIndices = aIndex;
        desc.pPos = pPos;
        desc.pColour = pColour;
        desc.pUV = pUV;
        fn_80036100(lbl_801D94B0, &desc, lbl_80281F40);
        fn_800360D4(lbl_801D94B0);
        lbl_80281F40 = 0;
    }
}

void fn_80093D14(void) {
    fn_800360A0(lbl_801D94B0);
}
