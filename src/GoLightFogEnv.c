// GoLightFogEnv.c (EA's name, from TW07's golf/hi-rendering/GoLightFogEnv.c): the light-fog
// environments. Each pairs a fog with a group of lights; one is current, and the scene is lit and
// fogged with it. When a hole loads, its lighting and fog data fill the current environment.

#include "lighting.h"
#include "camera.h"
#include "ball.h"

LF_SLightFogEnvMgr  gLightFogEnvMgr;
LF_SLightFogEnvMgr* gpLightFogEnvMgr = &gLightFogEnvMgr;

void LF_vInitModule(void) {
    int i;
    LF_SLightFogEnvironment* pEnv = gpLightFogEnvMgr->aSet;
    for (i = 0; i < NUM_LIGHT_SETS; i++) {
        LF_vInitLightFogEnvironment(pEnv);
        pEnv++;
    }
    LF_vSetCurrentLightFogEnvironment(0);
    LF_LoadCurrentLights();
    LF_UseCurrentFogSettings();
}

void LF_vCloseModule(void) {
    int i;
    LF_SLightFogEnvironment* pEnv = gpLightFogEnvMgr->aSet;
    for (i = 0; i < NUM_LIGHT_SETS; i++) {
        LF_vFreeLightFogEnvironment(pEnv);
        pEnv++;
    }
}

// Lights the current environment from the hole's light descriptions: light 4 takes the first
// directional light, and lights 0-3 the point lights in order, stepping over the directional one.
// Every colour is halved.
void LF_vSetLightingEnvironment(TGD_LightingData* pLighting) {
    LF_SLightFogEnvironment* pEnv;
    GoLight* pLight;
    TGD_LightDesc* pDesc;
    TGD_LightDesc* pDir;
    u8 bAddOne;
    int i;

    pEnv = LF_spGetCurrentLightFogEnvironment();
    for (i = 0; i < 5; i++) {
        pDir = &pLighting->aLight[i];
        if (pDir->nType == 1) break;
    }
    pLight = pEnv->group.apLight[4];
    pLight->nType = 1;
    LLMath_Scale(0.5f, pDir->vColor, pLight->u.dir.vColor);
    pLight->u.dir.f10 = 1.0f;
    pLight->u.dir.fC = 1.0f;

    bAddOne = 0;
    if (pLighting->aLight[0].nType == 1) {
        bAddOne = 1;
    }
    pDesc = bAddOne ? &pLighting->aLight[1] : &pLighting->aLight[0];
    pLight = pEnv->group.apLight[0];
    pLight->nType = 2;
    LLMath_Scale(0.5f, pDesc->vColor, pLight->u.point.vColor);
    LLMath_CopyVec(pDesc->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bAddOne = 0;
    for (i = 0; i < 2; i++) {
        if (pLighting->aLight[i].nType == 1) {
            bAddOne = 1;
        }
    }
    pDesc = bAddOne ? &pLighting->aLight[2] : &pLighting->aLight[1];
    pLight = pEnv->group.apLight[1];
    pLight->nType = 2;
    LLMath_Scale(0.5f, pDesc->vColor, pLight->u.point.vColor);
    LLMath_CopyVec(pDesc->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    bAddOne = 0;
    for (i = 0; i < 3; i++) {
        if (pLighting->aLight[i].nType == 1) {
            bAddOne = 1;
        }
    }
    pDesc = bAddOne ? &pLighting->aLight[3] : &pLighting->aLight[2];
    pLight = pEnv->group.apLight[2];
    pLight->nType = 2;
    LLMath_Scale(0.5f, pDesc->vColor, pLight->u.point.vColor);
    LLMath_CopyVec(pDesc->vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;

    pLight = pEnv->group.apLight[3];
    pLight->nType = 2;
    LLMath_Scale(0.5f, pLighting->aLight[3].vColor, pLight->u.point.vColor);
    LLMath_CopyVec(pLighting->aLight[3].vPos, pLight->u.point.vPos);
    pLight->u.point.fC = 1.0f;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight->u.point.f18 = 1.0f;
}

void LF_vSetDynamicLightModifiers(void) {
    LF_spGetCurrentLightFogEnvironment();
    LF_fComputeAngleBetweenLightAndCamera(0, Camera_GetCurrentLens());
}

void LF_vSetLightModifiers(LightParams* pParams) {
    LF_SLightFogEnvironment* pEnv;
    GoLight* pLight;
    pEnv = LF_spGetCurrentLightFogEnvironment();
    Vec3Copy(pParams->v0, pEnv->group.v28);
    pLight = pEnv->group.apLight[4];
    pLight->u.dir.f10 = pParams->f10;
    pLight = pEnv->group.apLight[0];
    pLight->u.point.fC = pParams->f20;
    pLight->u.point.f10 = pParams->f24;
    pLight->u.point.f14 = pParams->f24;
    pLight = pEnv->group.apLight[1];
    pLight->u.point.fC = pParams->f1C;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight = pEnv->group.apLight[2];
    pLight->u.point.fC = pParams->f18;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
    pLight = pEnv->group.apLight[3];
    pLight->u.point.fC = pParams->f14;
    pLight->u.point.f10 = 1.0f;
    pLight->u.point.f14 = 1.0f;
}

void LF_vSetFoggingEnvironment(TGD_FoggingData* pFogging) {
    LF_SLightFogEnvironment* pEnv;
    pEnv = LF_spGetCurrentLightFogEnvironment();
    FG_vSetFogRotation(&pEnv->fog, pFogging->fRotation);
    FG_spSetFogDirection(&pEnv->fog, 0, pFogging->aDirection[0].vColour, pFogging->aDirection[0].fDistance);
    FG_spSetFogDirection(&pEnv->fog, 1, pFogging->aDirection[1].vColour, pFogging->aDirection[1].fDistance);
    FG_spSetFogDirection(&pEnv->fog, 2, pFogging->aDirection[2].vColour, pFogging->aDirection[2].fDistance);
    FG_spSetFogDirection(&pEnv->fog, 3, pFogging->aDirection[3].vColour, pFogging->aDirection[3].fDistance);
}

void LF_vInitLightFogEnvironment(LF_SLightFogEnvironment* pEnv) {
    fn_8006E5A8(&pEnv->group, NUM_SET_LIGHTS);
    fn_8006F144(pEnv);
}

void LF_vFreeLightFogEnvironment(LF_SLightFogEnvironment* pEnv) {
    fn_8006E62C(&pEnv->group);
    fn_8006F148(pEnv);
}

f32 LF_fComputeAngleBetweenLightAndCamera(s32 nLight, CamLens* pLens) {
    return LF_fComputeAngleToLight(LF_spGetCurrentLightFogEnvironment()->group.apLight[nLight], pLens);
}

f32 LF_fComputeAngleToLight(GoLight* pLight, CamLens* pLens) {
    return Math_Acos(Vec3_Dot(pLens->m4[2], pLight->u.point.vPos));
}
