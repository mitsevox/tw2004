// GoLighting.c (EA's name, from its asserts; also in EA's 2002 source tree): the scene's lights.
// A pool of 25 lights (lighting.h, GoLighting) that light groups take their lights from, and the
// loading of a group into the graphics chip: up to four point lights and an ambient colour.
// Partly decompiled.

#include "lighting.h"
#include "camera.h"
#include "gx.h"

void    fn_8006E2A4(void);
void    fn_8006E424(void);
void    fn_8006E460(LightGroup* pGroup);
void    fn_8006E7A4(LightGroup* pGroup);
void    fn_8006ED70(void);
GoLight* fn_8006EFB8(void);
void    fn_8006F080(GoLight* pLight);

void fn_8000AE48(f32* pA, f32* pB, f32* pOut);     // pOut = pA * pB, element by element
void fn_80029BC8(f32* pVec);                        // sets a vector to lbl_80186838

// Makes the pool (every light free), loads no lights, and sets channel 4's ambient and material
// colours to white.
void fn_8006E2A4(void) {
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    GoLight* pLight;
    s32 i;

    lbl_802811D8->nPool = NUM_POOL_LIGHTS;
    lbl_802811D8->pPool = fn_80009B34(NUM_POOL_LIGHTS * sizeof(GoLight), 2, 0x10, "GoLighting.c", 0xF8);
    lbl_802811D8->p0 = lbl_802811D8->pPool;
    lbl_802811D8->pUsed = NULL;
    lbl_802811D8->pFree = NULL;
    pLight = lbl_802811D8->p0;
    for (i = 0; i < NUM_POOL_LIGHTS; i++) {
        pLight->pNext = pLight;
        pLight->pPrev = pLight;
        if (lbl_802811D8->pFree != NULL) {
            pLight->pNext = lbl_802811D8->pFree;
            pLight->pPrev = lbl_802811D8->pFree->pPrev;
            if (lbl_802811D8->pFree->pPrev != NULL) {
                lbl_802811D8->pFree->pPrev->pNext = pLight;
            }
            lbl_802811D8->pFree->pPrev = pLight;
        }
        lbl_802811D8->pFree = pLight;
        pLight++;
    }
    lbl_802811D8->nFree = NUM_POOL_LIGHTS;
    lbl_802811D8->nUsed = 0;
    lbl_802811D8->vAmbient[3] = 128.0f;
    lbl_802811D8->v1C[0] = 0.0f;
    lbl_802811D8->v1C[1] = 0.0f;
    lbl_802811D8->v1C[2] = 0.0f;
    lbl_802811D8->aPointColour2[0][3] = 0.0f;
    lbl_802811D8->aPointColour2[1][3] = 0.0f;
    lbl_802811D8->aPointColour2[2][3] = 0.0f;
    lbl_802811D8->aPointColour2[3][3] = 0.0f;
    fn_80029BC8(lbl_802811D8->vFC);
    fn_8006E7A4(NULL);
    GXSetChanAmbColor(4, white);
    GXSetChanMatColor(4, white);
}

// Frees the pool.
void fn_8006E424(void) {
    if (lbl_802811D8->pPool != NULL) {
        fn_80009E70(lbl_802811D8->pPool);
        lbl_802811D8->pPool = NULL;
    }
}

// Scales the loaded colours by the group's v18[0] (white without a group) into aPointColour2 and
// vAmbient2; the unused point lights get fn_80029BC8's vector.
void fn_8006E460(LightGroup* pGroup) {
    f32 vScale[4];
    s32 n;
    f32* pColour = lbl_802811D8->aPointColour[0];
    f32* pOut = lbl_802811D8->aPointColour2[0];

    if (pGroup != NULL) {
        vScale[0] = 1.0f;
        vScale[1] = 1.0f;
        vScale[2] = 1.0f;
        vScale[3] = 1.0f;
        fn_8001EF34(vScale, pGroup->v18[0], vScale);
    } else {
        vScale[0] = 1.0f;
        vScale[1] = 1.0f;
        vScale[2] = 1.0f;
        vScale[3] = 1.0f;
    }
    fn_8000AE48(lbl_802811D8->vAmbient, vScale, lbl_802811D8->vAmbient2);
    for (n = lbl_802811D8->nPoints; n > 0; n--) {
        fn_8000AE48(pColour, vScale, pOut);
        pColour += 4;
        pOut += 4;
    }
    vScale[0] = 1.0f;
    vScale[1] = 1.0f;
    vScale[2] = 1.0f;
    vScale[3] = 1.0f;
    if (pGroup != NULL) {
        pGroup->v28[3] = 1.0f;
        fn_8000AE48(pGroup->v28, vScale, vScale);
        fn_8001EF34(vScale, pGroup->v18[0], vScale);
    }
    fn_8000AE48(lbl_802811D8->aPointColour[0], vScale, lbl_802811D8->aPointColour2[0]);
    for (n = lbl_802811D8->nPoints; n < NUM_POINT_LIGHTS; n++) {
        fn_80029BC8(pOut);
        pOut += 4;
    }
}

// Takes nLights lights from the pool for the group.
void fn_8006E5A8(LightGroup* pGroup, s32 nLights) {
    s32 n = nLights;
    GoLight** ppLight = pGroup->apLight;

    pGroup->nLights = nLights;
    for (; n > 0; n--) {
        *ppLight = fn_8006EFB8();
        ppLight++;
    }
    pGroup->v28[0] = 1.0f;
    pGroup->v28[1] = 1.0f;
    pGroup->v28[2] = 1.0f;
    pGroup->v28[3] = 1.0f;
    pGroup->v18[0] = 1.0f;
    pGroup->v18[1] = 1.0f;
    pGroup->v18[2] = 1.0f;
    pGroup->v18[3] = 1.0f;
}

// Gives the group's lights back to the pool.
void fn_8006E62C(LightGroup* pGroup) {
    s32 n;
    GoLight** ppLight = pGroup->apLight;

    for (n = pGroup->nLights; n > 0; n--) {
        fn_8006F080(*ppLight);
        ppLight++;
    }
}

// Loads a group (or none): the directional light becomes the ambient colour (63.75 grey without
// one), the point lights fill the point slots in order; the slots left over are cleared.
void fn_8006E7A4(LightGroup* pGroup) {
    GoLight** ppLight;
    GoLight* pLight;
    s32 n;
    s32 i;
    u8 bAmbient = 0;

    lbl_802811D8->nPoints = 0;
    if (pGroup != NULL) {
        n = pGroup->nLights;
        ppLight = pGroup->apLight;
        for (; n > 0; n--) {
            pLight = *ppLight++;
            switch (pLight->nType) {
            case 1:
                bAmbient = 1;
                lbl_802811D8->vAmbient[0] =
                    255.0f * (pLight->u.dir.fC * (pLight->u.dir.vColor[0] * pLight->u.dir.f10));
                lbl_802811D8->vAmbient[1] =
                    255.0f * (pLight->u.dir.fC * (pLight->u.dir.vColor[1] * pLight->u.dir.f10));
                lbl_802811D8->vAmbient[2] =
                    255.0f * (pLight->u.dir.fC * (pLight->u.dir.vColor[2] * pLight->u.dir.f10));
                break;
            case 2:
                lbl_802811D8->aPointColour[lbl_802811D8->nPoints][0] =
                    255.0f * (pLight->u.point.vColor[0] * pLight->u.point.fC);
                lbl_802811D8->aPointColour[lbl_802811D8->nPoints][1] =
                    255.0f * (pLight->u.point.vColor[1] * pLight->u.point.fC);
                lbl_802811D8->aPointColour[lbl_802811D8->nPoints][2] =
                    255.0f * (pLight->u.point.vColor[2] * pLight->u.point.fC);
                lbl_802811D8->afPointX[lbl_802811D8->nPoints] = pLight->u.point.vPos[0];
                lbl_802811D8->afPointY[lbl_802811D8->nPoints] = pLight->u.point.vPos[1];
                lbl_802811D8->afPointZ[lbl_802811D8->nPoints] = pLight->u.point.vPos[2];
                Vec3Copy(pLight->u.point.vPos, lbl_802811D8->aPointPos[lbl_802811D8->nPoints]);
                lbl_802811D8->nPoints++;
                break;
            }
        }
    }
    if (!bAmbient) {
        lbl_802811D8->vAmbient[0] = 63.75f;
        lbl_802811D8->vAmbient[1] = 63.75f;
        lbl_802811D8->vAmbient[2] = 63.75f;
    }
    for (i = lbl_802811D8->nPoints; i < NUM_POINT_LIGHTS; i++) {
        lbl_802811D8->afPointX[i] = 0.0f;
        lbl_802811D8->afPointY[i] = 0.0f;
        lbl_802811D8->afPointZ[i] = 0.0f;
        lbl_802811D8->aPointColour[i][0] = 0.0f;
        lbl_802811D8->aPointColour[i][1] = 0.0f;
        lbl_802811D8->aPointColour[i][2] = 0.0f;
    }
    fn_8006E460(pGroup);
}

// Channel 4 unlit, with a grey ambient colour.
void fn_8006ED70(void) {
    GXColor grey = {0x80, 0x80, 0x80, 0x80};

    GXSetChanAmbColor(4, grey);
    GXSetChanCtrl(4, 0, 0, 1, 0, 2, 2);
}

// Takes a light from the pool (its type 0).
GoLight* fn_8006EFB8(void) {
    GoLight* pLight = lbl_802811D8->pFree;

    if (pLight == pLight->pNext) {
        lbl_802811D8->pFree = NULL;
    } else {
        if (pLight == lbl_802811D8->pFree) {
            lbl_802811D8->pFree = pLight->pNext;
        }
        pLight->pPrev->pNext = pLight->pNext;
        pLight->pNext->pPrev = pLight->pPrev;
        pLight->pPrev = pLight;
        pLight->pNext = pLight;
    }
    lbl_802811D8->nFree--;
    if (lbl_802811D8->pUsed != NULL) {
        pLight->pNext = lbl_802811D8->pUsed;
        pLight->pPrev = lbl_802811D8->pUsed->pPrev;
        if (lbl_802811D8->pUsed->pPrev != NULL) {
            lbl_802811D8->pUsed->pPrev->pNext = pLight;
        }
        lbl_802811D8->pUsed->pPrev = pLight;
    }
    lbl_802811D8->pUsed = pLight;
    lbl_802811D8->nUsed++;
    pLight->nType = 0;
    return pLight;
}

// Gives a light back to the pool.
void fn_8006F080(GoLight* pLight) {
    if (pLight == pLight->pNext) {
        lbl_802811D8->pUsed = NULL;
    } else {
        if (pLight == lbl_802811D8->pUsed) {
            lbl_802811D8->pUsed = pLight->pNext;
        }
        pLight->pPrev->pNext = pLight->pNext;
        pLight->pNext->pPrev = pLight->pPrev;
        pLight->pPrev = pLight;
        pLight->pNext = pLight;
    }
    lbl_802811D8->nUsed--;
    if (lbl_802811D8->pFree != NULL) {
        pLight->pNext = lbl_802811D8->pFree;
        pLight->pPrev = lbl_802811D8->pFree->pPrev;
        if (lbl_802811D8->pFree->pPrev != NULL) {
            lbl_802811D8->pFree->pPrev->pNext = pLight;
        }
        lbl_802811D8->pFree->pPrev = pLight;
    }
    lbl_802811D8->pFree = pLight;
    lbl_802811D8->nFree++;
}

void fn_8006F144(LightSet* pSet) {
}

void fn_8006F148(LightSet* pSet) {
}

void fn_8006F14C(void) {
}

void fn_8006F150(void) {
}
