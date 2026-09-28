// uiobject.c (EA's uiObject.c, golf/ui core/istudio runtime in TW06 and TW07): the 3D objects the
// in-round UI draws for Swing.c: the power-boost display (a quad grown by the boost level and a
// ring per level) and the spin display (a model tilted and rolled by the spin asked for), drawn by
// UI_Obj_RenderBoostUI through a camera of its own. TW07's uiObject.c has UI_Obj_InitModule,
// UI_Obj_CloseModule, UI_Obj_InitForRender, UI_Obj_ResetBoostRings, ... UI_Obj_RenderBoostUI in
// this order and shape (TW07 adds the "tappa spinna" set-up between them).

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "dynobj.h"
#include "lighting.h"
#include "uiobject.h"
#include "game.h"
#include "golfer.h"

#pragma explicit_zero_data on
f32        gUIObjLookAtTarget[4] = {0.0f, 0.0f, 0.0f, 0.0f};   // where the display's camera looks
#pragma explicit_zero_data reset

UIObjSettings gUIObjSettings[2];        // per view: the display's place, tilt, scale and roll
f32        gUIObjBoostRingSize[8];      // per boost level: its ring's size now
LightGroup gUIObjLights;                // the spin model's one directional light

UObject*   gpUIObjModel;                // the spin model ('TEO ' object 10003)
CamLens*   gpUIObjLens;                 // the display's own camera
TexEntry*  gpUIObjBaseTexture;          // } "toball": the base quad
TexEntry*  gpUIObjBoostTexture;         // } "toball" again: the quad grown by the boost level
TexEntry*  gpUIObjRingTexture;          // "ring": the boost rings
TexBank*   gpUIObjTexBank;              // the bank of the three textures
f32        gUIObjLightRed;              // the light's red (0: nothing writes it)

void fn_80013E38(u8* p, s32 v);  // GoRenderCtx_Gc.c
void UI_Obj_InitModule(void);
void UI_Obj_CloseModule(void);
void UI_Obj_InitForRender(void);
void UI_Obj_ResetBoostRings(int nPlayer);
void UI_Obj_RenderBoostUI(int nObj);
void UI_Obj_SetCurrentRenderCtxLens(CamLens* pLens);
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
void RC_ApplyCurrentViewport(void);
void RC_UpdateCurrentScreenMatrices(void);
void SW_vGetCurrentSpin(int nPlayer, f32* pfSide, f32* pfForward);   // Swing.c: the spin asked for
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                          // identity
void LLMath_mat44fltMultiplyList33(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // VecMath.c
void LLMath_CopyMat44(f32 (*pSrc)[4], f32 (*pDst)[4]);          // copy a matrix
void fn_8000C5A4(f32 (*pMtx)[4]);
void UI_Obj_DrawSpinModel(void);
void UI_Obj_DrawMesh(UObjMesh* pMesh);
void LI_LoadLightGroup(LightGroup* pGroup);   // Skin.c: load the group's lights (fn_8006E7A4)
void fn_8006EADC(UObject* pObj);        // GoLighting.c: light the object
void fn_8006ED70(void);                 // GoLighting.c
void fn_80035294(void);                 // GoTerrain.c

f32 gUIObjLightGreen = 0.05f;           // the light's green
f32 gUIObjLightBlue = 0.476f;           // the light's blue
f32 gUIObjAlpha = 0.19f;                // the spin model's constant alpha (times 255)

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80284018), before the 1.35f UI_Obj_InitModule uses first; its body is unknown.
static f32 uiobject_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Set the power-boost and spin display up (drawn by UI_Obj_RenderBoostUI for Swing.c): both views'
// settings (position, scale, tilt and roll; object 0 also the rings' sizes), the spin model
// (UI_Obj_InitForRender), the display's own camera, the "toball" and "ring" textures and one
// directional light. GO_vInitIG calls it as a round is set up.
void UI_Obj_InitModule(void) {
    u64 uName;
    int i;

    gpUIObjModel = NULL;
    gUIObjSettings[0].a28[0] = 1.35f;
    gUIObjSettings[0].a28[1] = 0.74f;
    gUIObjSettings[0].a28[2] = 0.03f;
    gUIObjSettings[0].a28[3] = 0.0f;
    UI_Obj_InitForRender();
    gpUIObjLens = CA_spCreateCamera();
    CA_vInitCamera(gpUIObjLens);
    for (i = 0; i < 2; i++) {
        gUIObjSettings[i].a0[0] = -0.345f;
        gUIObjSettings[i].a0[1] = -0.23f;
        gUIObjSettings[i].a0[2] = -3.13f;
        gUIObjSettings[i].a0[3] = 0.0f;
        gUIObjSettings[i].a0[4] = 0.0f;
        gUIObjSettings[i].a0[5] = 0.0f;
        gUIObjSettings[i].a0[6] = 0.00168f;
        gUIObjSettings[i].a0[7] = 0.02f;
        gUIObjSettings[i].a0[8] = 0.02f;
        gUIObjSettings[i].a0[9] = 0.0f;
    }
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &gpUIObjTexBank, &gpUIObjBaseTexture);
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &gpUIObjTexBank, &gpUIObjBoostTexture);
    uName = fn_8000BEE4("ring");
    fn_800102DC(uName, &gpUIObjTexBank, &gpUIObjRingTexture);
    fn_8006E5A8(&gUIObjLights, 1);
    gUIObjLights.apLight[0]->nType = 1;
    gUIObjLights.apLight[0]->u.dir.f10 = 1.0f;
    gUIObjLights.apLight[0]->u.dir.fC = 1.0f;
}

// Free the spin model, the display's camera and its light (gomainloop.c fn_8006CDC4, as a round
// shuts down).
void UI_Obj_CloseModule(void) {
    if (gpUIObjModel != NULL) {
        fn_80048860(gpUIObjModel);
    }
    gpUIObjModel = NULL;
    CA_vReleaseCamera(gpUIObjLens);
    fn_8006E62C(&gUIObjLights);
}

// Make the spin display's model from its 'TEO ' model object (id 10003), unless it is made already.
// port: a 'TEO ' object's UStreamObject.uUnk4 holds its model (see rcmp_mad_codec.c FE_CrAPBall_MakeObjects).
void UI_Obj_InitForRender(void) {
    if (gpUIObjModel == NULL) {
        gpUIObjModel = fn_80048808((UObjModel*)fn_8000B70C('TEO ', 10003)->uUnk4);
    }
}

// Put all eight boost rings (gUIObjBoostRingSize) back to their start size (object 0's a28[3]) so
// they grow afresh; Swing.c calls it on a boost, when the boosts are cleared and when a swing is
// set up. nPlayer is unused.
void UI_Obj_ResetBoostRings(int nPlayer) {
    gUIObjBoostRingSize[0] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[1] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[2] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[3] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[4] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[5] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[6] = gUIObjSettings[0].a28[3];
    gUIObjBoostRingSize[7] = gUIObjSettings[0].a28[3];
}

// Draw the power-boost and spin display of view nObj (Swing.c SW_vUIRender2D calls it): the depth
// under its corner of the screen is cleared; then, through the display's own camera, the base quad,
// the same quad grown by the player's power-boost level in that level's colour, and a ring per
// level that grows every frame and fades out between its fade size and its largest. With spin asked
// for (nSpinBoost), the spin model is drawn tilted toward the spin and rolling faster the more is
// asked. The display rises with the GameBreaker letterbox. The view is put back afterwards.
void UI_Obj_RenderBoostUI(int nObj) {
    f32 mRoll[4][4];
    f32 mTilt[4][4];
    f32 mRot[4][4];
    f32 mScale[4][4];
    f32 mSave0[4][4];
    f32 mSave40[4][4];
    f32 mSave80[4][4];
    f32 aXYZ[4][4];
    f32 vPos[4];
    f32 aDir[4];
    f32 aUp[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    f32 aBase[4] = {0.5f, 0.5f, 0.5f, 0.25f};
    f32 aColour[4] = {0.5f, 0.5f, 0.5f, 0.25f};
    f32 aBlack[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 aEye[4] = {0.0f, 0.0f, -100.0f, 0.0f};
    f32 aQuad[4][3] = {
        {-0.03f, 0.03f, 0.0f},
        {0.03f, 0.03f, 0.0f},
        {-0.03f, -0.03f, 0.0f},
        {0.03f, -0.03f, 0.0f},
    };
    f32 aUV[4][4] = {
        {1.0f, 0.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},
    };
    f32 aRect[2][4];
    CamLens* pLens;
    f32 fSpinY;
    f32 fSpinX;
    f32 fY;
    f32 fBoost;
    f32 fMax;
    f32 fFade;
    f32 fDot;
    f32 fSinRoll;
    f32 fCosRoll;
    f32 fSinTilt;
    f32 fCosTilt;
    int nPlayer;
    int i;
    int j;

    nPlayer = ViewController_GetActivePlayerNumber(nObj);
    // the object rises with the GameBreaker letterbox
    if (gGameEffects.bGameBreaker == 0) {
        fY = -0.23f;
    } else {
        fY = (0.23f - 0.17f) * (GameEffects_GetLetterboxHeight() / 0.15f) + -0.23f;
    }
    gUIObjSettings[nObj].a0[1] = fY;
    vPos[0] = gUIObjSettings[nObj].a0[0];
    vPos[1] = gUIObjSettings[nObj].a0[1];
    vPos[2] = gUIObjSettings[nObj].a0[2];
    vPos[3] = 1.0f;
    aRect[0][0] = 0.5f;
    aRect[0][1] = 0.5f;
    aRect[0][2] = 1.0f;
    aRect[0][3] = 1.0f;
    aRect[1][0] = 1.0f;
    aRect[1][1] = 1.0f;
    aRect[1][2] = 1.0f;
    aRect[1][3] = 1.0f;

    // clear the depth under the object's corner of the screen
    RenderView_SetUseCurrentMatrices(0);
    RenderView_SetColor(aBlack);
    RC_ApplyCurrentViewport();
    RenderState_SetDrawFlags(0);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    RenderState_SetRenderSurface(0, 0x200, 0x1C0, lbl_80281B88 & 1, 2, 1);
    RenderState_Flush();
    GXSetZMode(1, 7, 1);
    RenderView_DrawPrimitive(0xA1, aRect[0], NULL, NULL, 2);
    DS_vSetAlphaTestMode(1, 6, 0x80);
    DS_vSetZBufferMode(3);
    RenderState_SetRenderSurface(0, 0x200, 0x1C0, lbl_80281B88 & 1, 8, 1);
    RenderState_Flush();

    // the object's own lens
    pLens = Camera_GetCurrentLens();
    CA_vInitCamera(gpUIObjLens);
    CA_vSetLookAt(gpUIObjLens, aEye, gUIObjLookAtTarget);
    fn_80045470(gpUIObjLens, 0.00879646f);
    UI_Obj_SetCurrentRenderCtxLens(gpUIObjLens);
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_SetCameraMatrices();

    fBoost = (f32)gPlayers[nPlayer].swing.nPowerBoost / 8.0f;
    RenderState_SetDrawFlags(0x50);
    DS_vEnableZBufferUpdate(1);
    RenderView_SetUseCurrentMatrices(1);
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(1, 4, 1);

    // the base quad
    RenderState_SetBankTexture(gpUIObjTexBank, gpUIObjBaseTexture);
    RenderState_Flush();
    for (i = 0; i < 4; i++) {
        aXYZ[i][0] = aQuad[i][0] + gUIObjSettings[nObj].a0[0];
        aXYZ[i][1] = aQuad[i][1] + gUIObjSettings[nObj].a0[1];
        aXYZ[i][2] = aQuad[i][2] + gUIObjSettings[nObj].a0[2];
        aXYZ[i][3] = 1.0f;
    }
    RenderView_SetColor(aBase);
    RenderView_DrawPrimitive(0x98, aXYZ[0], NULL, aUV[0], 4);

    // the same, grown by the boost level, in the level's colour
    for (i = 0; i < 4; i++) {
        aXYZ[i][0] = fBoost * aQuad[i][0] + gUIObjSettings[nObj].a0[0];
        aXYZ[i][1] = fBoost * aQuad[i][1] + gUIObjSettings[nObj].a0[1];
        aXYZ[i][2] = fBoost * aQuad[i][2] + gUIObjSettings[nObj].a0[2];
        aXYZ[i][3] = 1.0f;
    }
    RenderState_SetBankTexture(gpUIObjTexBank, gpUIObjBoostTexture);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    if (gPlayers[nPlayer].swing.nPowerBoost > 0) {
        Vec3Copy(gBoostLevelColours[gPlayers[nPlayer].swing.nPowerBoost - 1], aColour);
    }
    RenderView_SetColor(aColour);
    RenderState_Flush();
    RenderView_DrawPrimitive(0x98, aXYZ[0], NULL, aUV[0], 4);

    // a ring per level: each grows until it passes the largest size, fading out on the way
    RenderState_SetBankTexture(gpUIObjTexBank, gpUIObjRingTexture);
    RenderState_Flush();
    fMax = gUIObjSettings[0].a28[0];
    fFade = gUIObjSettings[0].a28[1];
    for (i = 0; i < gPlayers[nPlayer].swing.nPowerBoost; i++) {
        if (!(gUIObjBoostRingSize[i] > fMax)) {
            for (j = 0; j < 4; j++) {
                aXYZ[j][0] = aQuad[j][0] * gUIObjBoostRingSize[i] + gUIObjSettings[nObj].a0[0];
                aXYZ[j][1] = aQuad[j][1] * gUIObjBoostRingSize[i] + gUIObjSettings[nObj].a0[1];
                aXYZ[j][2] = aQuad[j][2] * gUIObjBoostRingSize[i] + gUIObjSettings[nObj].a0[2];
                aXYZ[j][3] = 1.0f;
            }
            if (gUIObjBoostRingSize[i] < fFade) {
                aColour[0] = gBoostLevelColours[i][0];
                aColour[1] = gBoostLevelColours[i][1];
                aColour[2] = gBoostLevelColours[i][2];
                aColour[3] = 1.0f;
            } else {
                aColour[0] = gBoostLevelColours[i][0];
                aColour[1] = gBoostLevelColours[i][1];
                aColour[2] = gBoostLevelColours[i][2];
                aColour[3] = 1.0f - (gUIObjBoostRingSize[i] - fFade) / (fMax - fFade);
            }
            RenderView_SetColor(aColour);
            RenderView_DrawPrimitive(0x98, aXYZ[0], NULL, aUV[0], 4);
            gUIObjBoostRingSize[i] += gUIObjSettings[0].a28[2];
        }
    }

    // the model: tilted toward the spin asked for, rolling faster the more is asked
    if (gPlayers[nPlayer].swing.nSpinBoost > 0) {
        SW_vGetCurrentSpin(nPlayer, &fSpinY, &fSpinX);
        if (fSpinY != 0.0f || fSpinX != 0.0f) {
            aDir[0] = fSpinX;
            aDir[1] = fSpinY;
            aDir[2] = 0.0f;
            aDir[3] = 1.0f;
            LLMath_Normalize3(aDir, aDir);
            fDot = (Vec3_Dot(aUp, aDir) < -1.0f) ? -1.0f
                 : ((Vec3_Dot(aUp, aDir) > 1.0f) ? 1.0f : Vec3_Dot(aUp, aDir));
            gUIObjSettings[nObj].a0[5] = Math_Acos(fDot);
            if (fSpinY < 0.0f) {
                gUIObjSettings[nObj].a0[5] = -gUIObjSettings[nObj].a0[5];
            }
            LLMath_IdentifyMat(mRot);
            LLMath_IdentifyMat(mRoll);
            LLMath_IdentifyMat(mTilt);
            LLMath_IdentifyMat(mScale);
            mScale[0][0] = gUIObjSettings[nObj].a0[6];
            mScale[1][1] = gUIObjSettings[nObj].a0[6];
            mScale[2][2] = gUIObjSettings[nObj].a0[6];
            fSinRoll = Math_Sin(gUIObjSettings[nObj].a0[9]);
            fCosRoll = Math_Cos(gUIObjSettings[nObj].a0[9]);
            fSinTilt = Math_Sin(gUIObjSettings[nObj].a0[5]);
            fCosTilt = Math_Cos(gUIObjSettings[nObj].a0[5]);
            if (fSpinX < 0.0f && fSpinY < 0.0f) {
                gUIObjSettings[nObj].a0[9] += (fabsf(fSpinX) > fabsf(fSpinY)) ? fabsf(fSpinX) : fabsf(fSpinY);
            } else {
                gUIObjSettings[nObj].a0[9] += (fabsf(fSpinX) > fabsf(fSpinY)) ? fabsf(fSpinX) : fabsf(fSpinY);
            }
            if (gUIObjSettings[nObj].a0[9] > 2.0f * PI) {
                gUIObjSettings[nObj].a0[9] = 0.0f;
            } else if (gUIObjSettings[nObj].a0[9] < 0.0f) {
                gUIObjSettings[nObj].a0[9] = 2.0f * PI;
            }
            mRoll[1][1] = fCosRoll;
            mRoll[1][2] = fSinRoll;
            mRoll[2][1] = -fSinRoll;
            mRoll[2][2] = fCosRoll;
            mTilt[0][0] = fCosTilt;
            mTilt[0][1] = fSinTilt;
            mTilt[1][0] = -fSinTilt;
            mTilt[1][1] = fCosTilt;
            LLMath_mat44fltMultiplyList33(mTilt, mRoll, mRot, 3);

            // draw it with the rotation, scale and position, then put its matrices back
            LLMath_CopyMat44(gpUIObjModel->m0, mSave0);
            LLMath_CopyMat44(gpUIObjModel->m40, mSave40);
            LLMath_CopyMat44(gpUIObjModel->m80, mSave80);
            LLMath_mat44fltMultiplyList33(mRot, gpUIObjModel->m0, gpUIObjModel->m0, 3);
            LLMath_mat44fltMultiplyList33(mScale, gpUIObjModel->m40, gpUIObjModel->m40, 3);
            fn_8000C5A4(gpUIObjModel->m0);
            LLMath_CopyVec(vPos, gpUIObjModel->m80[3]);
            gpUIObjModel->m80[3][3] = 1.0f;
            UI_Obj_DrawSpinModel();
            // EA bug: m0's copy goes back into m40 and m40's into m0
            LLMath_CopyMat44(mSave0, gpUIObjModel->m40);
            LLMath_CopyMat44(mSave40, gpUIObjModel->m0);
            LLMath_CopyMat44(mSave80, gpUIObjModel->m80);
        }
    }

    // the view as it was
    RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
    UI_Obj_SetCurrentRenderCtxLens(pLens);
    RenderState_SetCameraMatrices();
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
    RenderState_SetConstantAlphaOn(0);
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(1, 6, 0x80);
    DS_vSetZBufferMode(3);
    RenderState_Flush();
}

// Draw the spin display's model: its light in the display's colour (gUIObjLightRed,
// gUIObjLightGreen, gUIObjLightBlue), constant alpha 255 times gUIObjAlpha, the model's own matrix
// (m80) as the transform, then the mesh of its first level of detail (UI_Obj_DrawMesh).
void UI_Obj_DrawSpinModel(void) {
    gUIObjLights.apLight[0]->u.dir.vColor[0] = gUIObjLightRed;
    gUIObjLights.apLight[0]->u.dir.vColor[1] = gUIObjLightGreen;
    gUIObjLights.apLight[0]->u.dir.vColor[2] = gUIObjLightBlue;
    gUIObjLights.apLight[0]->u.dir.vColor[3] = 0.0f;
    LI_LoadLightGroup(&gUIObjLights);
    fn_8006EADC(gpUIObjModel);
    RenderState_SetBlendFactors(4, 5);
    RenderState_SetConstantAlphaOn(1);
    RenderState_SetConstantAlpha(255.0f * gUIObjAlpha);
    RenderState_Flush();
    RC_vSetCurrentRenderCtxTransformationMatrix(gpUIObjModel->m80);
    fn_80035294();
    RenderState_SetCameraMatrices();
    RenderState_SetClipMode(1);
    RenderState_Flush();
    UI_Obj_DrawMesh(gpUIObjModel->pModel->apLod[0]);
    fn_8006ED70();
}

// Make pLens the current render context's lens (GoRenderCtx_Gc.c fn_80013E38).
void UI_Obj_SetCurrentRenderCtxLens(CamLens* pLens) {
    // port: fn_80013E38 (GoRenderCtx_Gc.c, still sweep code) takes the lens as an s32
    fn_80013E38((u8*)*lbl_80280DF0, (s32)pLens);
}

// Draw the mesh's current part, if it is used (UObject.c's fn_80048A84 again).
void UI_Obj_DrawMesh(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}
