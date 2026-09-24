// Code8006F154.c (our name; a file of its own by its data, name unknown): the terrain settings'
// colour by the camera's heading. Four colours sit round the compass (TerSettings.aColours); the
// colour between the two the camera faces is blended into TerSettings.f44..f50 (GoTerrain.c hands
// it to the renderer).

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "terrain.h"

void fn_8006F3D0(f32* pA, f32* pB, f32* pOut, f32 t);
f32* fn_8006F400(TerSettings* pSettings, int nColour, f32* pColour, f32 fW);
void fn_8006F430(TerSettings* pSettings, f32 fAngle);

TerSettings* lbl_802811E0 = &lbl_801D70A8;

TerSettings lbl_801D70A8;

// Blend the colour for the camera's current heading.
void fn_8006F154(void) {
    TerSettings* pSettings = lbl_802811E0;
    CamLens* pLens = fn_8001F004();
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
    fScale = (fLen2 == 0.0f) ? 0.0f : 1.0f / fn_80029B64(fLen2);
    fX *= fScale;
    fZ *= fScale;
    fCos = (fX < -1.0f) ? -1.0f : (fX > 1.0f) ? 1.0f : fX;
    fSin = (fZ < -1.0f) ? -1.0f : (fZ > 1.0f) ? 1.0f : fZ;
    if (fSin > 0.0f) {
        fAngle = fn_80009614(fCos);
    } else {
        fAngle = TWOPI - fn_80009614(fCos);
    }
    fAngle += pSettings->f40;
    if (fAngle >= TWOPI) {
        fAngle -= TWOPI;
    }
    if (fAngle < 0.0f) {
        fAngle += TWOPI;
    }
    if (fAngle >= 0.0f && fAngle < PI / 2) {
        pFrom = pSettings->aColours[0];
        pTo = pSettings->aColours[1];
        fT = fAngle * (2.0f / PI);
    } else if (fAngle >= PI / 2 && fAngle < PI) {
        pFrom = pSettings->aColours[1];
        pTo = pSettings->aColours[2];
        fT = (fAngle - PI / 2) * (2.0f / PI);
    } else if (fAngle >= PI && fAngle < 1.5f * PI) {
        pFrom = pSettings->aColours[2];
        pTo = pSettings->aColours[3];
        fT = (fAngle - PI) * (2.0f / PI);
    } else {
        pFrom = pSettings->aColours[3];
        pTo = pSettings->aColours[0];
        fT = (fAngle - 1.5f * PI) * (2.0f / PI);
    }
    fn_8006F3D0(pFrom, pTo, &pSettings->f44, fT);
    fn_8001EF34(255.0f, &pSettings->f44, &pSettings->f44);
}

// The default colours: all four the same grey-blue, no turn.
void fn_8006F334(TerSettings* pSettings) {
    f32 aColour[4];

    fn_8006F430(pSettings, 0.0f);
    aColour[0] = 0.38f;
    aColour[1] = 0.41f;
    aColour[2] = 0.41f;
    aColour[3] = 0.0f;
    fn_8006F400(pSettings, 0, aColour, 125.0f);
    fn_8006F400(pSettings, 1, aColour, 125.0f);
    fn_8006F400(pSettings, 2, aColour, 125.0f);
    fn_8006F400(pSettings, 3, aColour, 125.0f);
}

// Four floats: pOut gets pA + (pB - pA) * t.
#ifdef __MWERKS__
asm void fn_8006F3D0(register f32* pA, register f32* pB, register f32* pOut, register f32 t) {
    nofralloc
    fmr      f4, t
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
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8006F3D0(f32* pA, f32* pB, f32* pOut, f32 t) {
    pOut[0] = pA[0] + (pB[0] - pA[0]) * t;
    pOut[1] = pA[1] + (pB[1] - pA[1]) * t;
    pOut[2] = pA[2] + (pB[2] - pA[2]) * t;
    pOut[3] = pA[3] + (pB[3] - pA[3]) * t;
}
#endif

// Set colour nColour to pColour with its last value replaced by fW; returns the colour.
f32* fn_8006F400(TerSettings* pSettings, int nColour, f32* pColour, f32 fW) {
    f32* pDst = pSettings->aColours[nColour];

    pDst[0] = pColour[0];
    pDst[1] = pColour[1];
    pDst[2] = pColour[2];
    pDst[3] = pColour[3];
    pDst[3] = fW;
    return pDst;
}

void fn_8006F430(TerSettings* pSettings, f32 fAngle) {
    pSettings->f40 = fAngle;
}
