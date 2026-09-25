// GoLightFogEnv.c (EA's name: TW07's golf/hi-rendering/GoLightFogEnv.c starts the same way,
// LF_vInitModule and LF_vCloseModule looping over the light environments, then
// LF_vSetLightingEnvironment from the hole's lighting data): split off goballfx.c at 0x80093524.
// Its .sdata pointer lbl_80281380 is padded to 8 (0x80281384..0x80281388), where goballfx.c's
// .sdata starts; its .bss (lbl_801D9278) follows the previous unit's.

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

LightSets  lbl_801D9278;
LightSets* lbl_80281380 = &lbl_801D9278;

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
// record (with none among the first five it reads one record past them), the point lights from
// the records around it.
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
    fn_8000AE28(0.5f, pDir->vColor, pLight->u.dir.vColor);
    pLight->u.dir.f10 = 1.0f;
    pLight->u.dir.fC = 1.0f;

    bSkip = 0;
    if (pLights->aLight[0].nType == 1) {
        bSkip = 1;
    }
    pRec = bSkip ? &pLights->aLight[1] : &pLights->aLight[0];
    pLight = pSet->group.apLight[0];
    pLight->nType = 2;
    fn_8000AE28(0.5f, pRec->vColor, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bSkip = 0;
    for (i = 0; i < 2; i++) {
        if (pLights->aLight[i].nType == 1) {
            bSkip = 1;
        }
    }
    pRec = bSkip ? &pLights->aLight[2] : &pLights->aLight[1];
    pLight = pSet->group.apLight[1];
    pLight->nType = 2;
    fn_8000AE28(0.5f, pRec->vColor, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bSkip = 0;
    for (i = 0; i < 3; i++) {
        if (pLights->aLight[i].nType == 1) {
            bSkip = 1;
        }
    }
    pRec = bSkip ? &pLights->aLight[3] : &pLights->aLight[2];
    pLight = pSet->group.apLight[2];
    pLight->nType = 2;
    fn_8000AE28(0.5f, pRec->vColor, pLight->u.point.vColor);
    Vec_Copy(pRec->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    // EA bug: the last point light always takes the fourth record, even when that is the
    // directional one
    pLight = pSet->group.apLight[3];
    pLight->nType = 2;
    fn_8000AE28(0.5f, pLights->aLight[3].vColor, pLight->u.point.vColor);
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
    return fn_80009614(fn_8000C5FC(pLens->m4[2], pLight->u.point.vPos));
}
