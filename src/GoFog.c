// GoFog.c (TW06's hi-rendering/gofog.c, by its FG_ types): the distance fog. The fog has a colour
// and distance for four directions; each frame the two the camera faces are blended into the fog the
// renderer draws with, so the haze can change colour and depth as the camera turns.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "fog.h"

FG_SFogEnvironment* gpFogEnvironment = &gFogEnvironment;

FG_SFogEnvironment gFogEnvironment;

// Blends the fog for the camera's heading: the heading (plus the fog's rotation) picks the two
// directions either side of it, and the fog colour and distance are mixed between them. The colour
// comes out 0..255 for the renderer.
void FG_vBlendFogForCamera(void) {
    FG_SFogEnvironment* pFog = gpFogEnvironment;
    CamLens* pLens = Camera_GetCurrentLens();
    f32 fX;
    f32 fZ;
    f32 fLen2;
    f32 fScale;
    f32 fCos;
    f32 fSin;
    f32 fAngle;
    f32* pFrom;
    f32* pTo;
    f32 fT;

    fZ = pLens->m4[2][2];
    fX = pLens->m4[2][0];
    fLen2 = fX * fX + fZ * fZ;
    fScale = (fLen2 == 0.0f) ? 0.0f : 1.0f / Math_Sqrtf(fLen2);
    fX *= fScale;
    fZ *= fScale;
    fCos = (fX < -1.0f) ? -1.0f : (fX > 1.0f) ? 1.0f : fX;
    fSin = (fZ < -1.0f) ? -1.0f : (fZ > 1.0f) ? 1.0f : fZ;
    if (fSin > 0.0f) {
        fAngle = Math_Acos(fCos);
    } else {
        fAngle = TWOPI - Math_Acos(fCos);
    }
    fAngle += pFog->fRotation;
    if (fAngle >= TWOPI) {
        fAngle -= TWOPI;
    }
    if (fAngle < 0.0f) {
        fAngle += TWOPI;
    }
    if (fAngle >= 0.0f && fAngle < PI / 2) {
        pFrom = pFog->aDirection[0];
        pTo = pFog->aDirection[1];
        fT = fAngle * (2.0f / PI);
    } else if (fAngle >= PI / 2 && fAngle < PI) {
        pFrom = pFog->aDirection[1];
        pTo = pFog->aDirection[2];
        fT = (fAngle - PI / 2) * (2.0f / PI);
    } else if (fAngle >= PI && fAngle < 1.5f * PI) {
        pFrom = pFog->aDirection[2];
        pTo = pFog->aDirection[3];
        fT = (fAngle - PI) * (2.0f / PI);
    } else {
        pFrom = pFog->aDirection[3];
        pTo = pFog->aDirection[0];
        fT = (fAngle - 1.5f * PI) * (2.0f / PI);
    }
    LLMath_Interpolate(pFrom, pTo, fT, pFog->vCurrent);
    Vec3_Scale(255.0f, pFog->vCurrent, pFog->vCurrent);
}

// The default fog: the same grey-blue in every direction, 125 yards out, no rotation.
void FG_vSetDefaultFog(FG_SFogEnvironment* pFog) {
    f32 aColour[4];

    FG_vSetFogRotation(pFog, 0.0f);
    aColour[0] = 0.38f;
    aColour[1] = 0.41f;
    aColour[2] = 0.41f;
    aColour[3] = 0.0f;
    FG_spSetFogDirection(pFog, 0, aColour, 125.0f);
    FG_spSetFogDirection(pFog, 1, aColour, 125.0f);
    FG_spSetFogDirection(pFog, 2, aColour, 125.0f);
    FG_spSetFogDirection(pFog, 3, aColour, 125.0f);
}

asm void LLMath_Interpolate(register f32* pA, register f32* pB, register f32 fT, register f32* pOut) {
    nofralloc
    fmr      f4, fT
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 0, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 0, 0
    ps_sub   f2, f2, f0
    ps_sub   f3, f3, f1
    ps_madds0 f2, f2, f4, f0
    ps_madds0 f3, f3, f4, f1
    psq_st   f2, 0(pOut), 0, 0
    psq_st   f3, 8(pOut), 0, 0
    blr
}

f32* FG_spSetFogDirection(FG_SFogEnvironment* pFog, int nDirection, f32* pColour, f32 fDistance) {
    f32* pDst = pFog->aDirection[nDirection];

    pDst[0] = pColour[0];
    pDst[1] = pColour[1];
    pDst[2] = pColour[2];
    pDst[3] = pColour[3];
    pDst[3] = fDistance;
    return pDst;
}

void FG_vSetFogRotation(FG_SFogEnvironment* pFog, f32 fRotation) {
    pFog->fRotation = fRotation;
}
