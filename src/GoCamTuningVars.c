// GoCamTuningVars.c (EA's name, from its asserts): the camera tuning values (camera.h's
// CamTuning), allocated and set to fixed values by CameraTuning_Init. TW07's file also has the
// tuning-variable registration (CreateGlobalCameraTuningVariables, not found in this build) and
// CameraTuning_Close, which here sits just after CameraTuning_Init but is filed as
// unsorted/sweep_80097E98.c's fn_80097E98.

#include "game_types.h"
#include "engine.h"
#include "camera.h"

void CameraTuning_Init(void);

// The camera tuning values every camera file reads (CamTuning), allocated and set by
// CameraTuning_Init.
CamTuning* gpCamTuning;

// Allocate the camera tuning values (gpCamTuning) and set each to its fixed value; run when the
// front end and each round start (GO_vInitFE, GO_vInitIG). fMaxPitchUp and fMaxPitchDown are in
// degrees (GolfCamera_ClampLookAngle converts them). fn_80097E98 (sweep_80097E98.c, TW07's
// CameraTuning_Close) frees them.
void CameraTuning_Init(void) {
    gpCamTuning = StaticMem_Alloc(sizeof(CamTuning), 2, 0, "GoCamTuningVars.c", 24);
    gpCamTuning->f0 = 10.0f;
    gpCamTuning->f4 = 15.0f;
    gpCamTuning->f8 = 0.0f;
    gpCamTuning->fC = 10.0f;
    gpCamTuning->f10 = 0.9f;
    gpCamTuning->f14 = 5.0f;
    gpCamTuning->f18 = 5.0f;
    gpCamTuning->f1C = 0.45f;
    gpCamTuning->f20 = 0.35f;
    gpCamTuning->f24 = 0.01f;
    gpCamTuning->f2C = 0.1f;
    gpCamTuning->f28 = 15.0f;
    gpCamTuning->f30 = 0.25f;
    gpCamTuning->f34 = 3.0f;
    gpCamTuning->f38 = 0.25f;
    gpCamTuning->f3C = 0.01f;
    gpCamTuning->f40 = 0.5f;
    gpCamTuning->f44 = 4.0f;
    gpCamTuning->f48 = 1.25f;
    gpCamTuning->f4C = 0.2f;
    gpCamTuning->f50 = 4.0f;
    gpCamTuning->f54 = 2.0f;
    gpCamTuning->f58 = 1.0f;
    gpCamTuning->f5C = 1.5f;
    gpCamTuning->f60 = 0.1f;
    gpCamTuning->f64 = 0.45f;
    gpCamTuning->v68[0] = 0.5f;
    gpCamTuning->v68[1] = 0.4f;
    gpCamTuning->v68[2] = 0.4f;
    gpCamTuning->v68[3] = 0.5f;
    gpCamTuning->f78 = 0.1f;
    gpCamTuning->f7C = 140.0f / 180.0f * PI;
    gpCamTuning->f80 = 10.0f / 180.0f * PI;
    gpCamTuning->f84 = 0.5f;
    gpCamTuning->f88 = 0.4f;
    gpCamTuning->f8C = 0.01f;
    gpCamTuning->f90 = 2.0f;
    gpCamTuning->f94 = 2.0f;
    gpCamTuning->f98 = 0.95f;
    gpCamTuning->f9C = 0.9f;
    gpCamTuning->fA0 = 0.95f;
    gpCamTuning->fA4 = 0.99f;
    gpCamTuning->fA8 = 10.0f;
    gpCamTuning->fAC = 35.0f;
    gpCamTuning->fB0 = 100.0f;
    gpCamTuning->fB4 = 1.15f;
    gpCamTuning->fB8 = 0.65f;
    gpCamTuning->nBeats = 4;
    gpCamTuning->nBeatFrames = 2;
    gpCamTuning->fC4 = 0.25f;
    gpCamTuning->fC8 = 0.5f;
    gpCamTuning->fCC = 0.0f;
    gpCamTuning->fD0 = 0.6f;
    gpCamTuning->fD4 = 0.5f;
    gpCamTuning->fD8 = 20.0f;
    gpCamTuning->fDC = 0.1f;
    gpCamTuning->fE0 = 0.01f;
    gpCamTuning->fE4 = PI / 10.0f;
    gpCamTuning->fE8 = 0.16f;
    gpCamTuning->fEC = 0.5f;
    gpCamTuning->fF0 = 0.05f;
    gpCamTuning->fF4 = 0.5f;
    gpCamTuning->fF8 = 0.1f;
    gpCamTuning->fFC = 0.95f;
    gpCamTuning->f100 = 80.0f;
    gpCamTuning->f104 = 2.0f;
    gpCamTuning->f108 = 0.25f;
    gpCamTuning->f10C = 4.0f;
    gpCamTuning->f110 = 15.0f / 180.0f * PI;
    gpCamTuning->f114 = DEG(50.0f);
    gpCamTuning->f118 = 0.001f;
    gpCamTuning->f11C = 1.0f;
    gpCamTuning->f120 = 1.0f;
    gpCamTuning->f124 = 2.5f;
    gpCamTuning->f128 = 5.0f;
    gpCamTuning->f12C = 5.0f;
    gpCamTuning->f130 = 10.0f;
    gpCamTuning->f134 = 7.0f;
    gpCamTuning->f138 = 0.8f;
    gpCamTuning->f13C = 3.0f;
    gpCamTuning->f140 = 0.1f;
    gpCamTuning->f144 = 0.2f;
    gpCamTuning->f148 = 0.35f;
    gpCamTuning->f14C = 0.05f;
    gpCamTuning->f150 = 0.005f;
    gpCamTuning->f154 = 1.0f;
    gpCamTuning->f158 = 2.0f;
    gpCamTuning->f15C = 4.0f;
    gpCamTuning->f160 = 2.0f;
    gpCamTuning->f164 = 0.1f;
    gpCamTuning->f168 = 0.2f;
    gpCamTuning->f16C = 5.2f;
    gpCamTuning->f170 = 0.25f;
    gpCamTuning->f174 = 1.0f;
    gpCamTuning->f178 = 0.25f;
    gpCamTuning->v17C[0] = 0.5f;
    gpCamTuning->v17C[1] = 0.5f;
    gpCamTuning->v17C[2] = 0.5f;
    gpCamTuning->v17C[3] = 0.5f;
    gpCamTuning->f18C = 1.0f;
    gpCamTuning->f190 = 1.0f;
    gpCamTuning->f194 = 0.01f;
    gpCamTuning->f198 = 0.7f;
    gpCamTuning->f19C = DEG(10.0f);
    gpCamTuning->f1A0 = 0.2f;
    gpCamTuning->f1A4 = 1.0f;
    gpCamTuning->f1A8 = 0.001f;
    gpCamTuning->f1B0 = 0.0f;
    gpCamTuning->f1AC = 0.01f;
    gpCamTuning->n1C0 = 0;
    gpCamTuning->n1C4 = 0;
    gpCamTuning->bCheckSlope = 1;
    gpCamTuning->bCheckTerrain = 1;
    gpCamTuning->f1D0 = 0.1f;
    gpCamTuning->f1D4 = 0.05f;
    gpCamTuning->f1D8 = 5.0f;
    gpCamTuning->n1DC = 30;
    gpCamTuning->fSlopeUp = 0.25f;
    gpCamTuning->fSlopeDown = 0.25f;
    gpCamTuning->f1E8 = -0.2f;
    gpCamTuning->f1EC = 1.1f;
    gpCamTuning->f1F0 = 0.05f;
    gpCamTuning->f1F4 = 1.5f;
    gpCamTuning->fMaxPitchUp = 12.0f;
    gpCamTuning->fMaxPitchDown = 30.0f;
    gpCamTuning->f1B4 = 0.7f;
    gpCamTuning->f1B8 = 0.3f;
    gpCamTuning->f1BC = 0.2f;
    gpCamTuning->f200 = 0.1f;
    gpCamTuning->f204 = 0.5f;
    gpCamTuning->f208 = 0.25f;
    gpCamTuning->f20C = 2.0f;
    gpCamTuning->f210 = 5.0f;
    gpCamTuning->f214 = 1.0f;
    gpCamTuning->f218 = 0.8f;
    gpCamTuning->f21C = 9.75f;
    gpCamTuning->f220 = 1.5f;
    gpCamTuning->f224 = 1.5f;
    gpCamTuning->n228 = 0;
    gpCamTuning->f22C = 0.25f;
    gpCamTuning->f230 = 0.05f;
    gpCamTuning->f234 = 1.5f;
    gpCamTuning->f238 = 1.5f;
    gpCamTuning->f23C = 0.5f;
    gpCamTuning->f240 = 1.0f;
    gpCamTuning->f244 = 0.95f;
    gpCamTuning->f248 = 0.0025f;
    gpCamTuning->n24C = 0;
    gpCamTuning->f250 = 50.0f;
    gpCamTuning->f254 = 0.7f;
    gpCamTuning->f258 = 0.005f;
}
