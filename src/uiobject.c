// uiobject.c (our name, after TW06's golf/ui core/istudio runtime/uiobject.c; UI_Obj_InitModule is
// a TW06 pair): the 3D objects the in-game UI draws (TW06: the confidence-boost rings and the
// "tappa spinna" UI): here the power boost and spin display (fn_800AE3F8).

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "dynobj.h"
#include "lighting.h"
#include "uiobject.h"
#include "game.h"
#include "golfer.h"

void fn_80013E38(u8* p, s32 v);  // GoRenderCtx_Gc.c
void UI_Obj_InitModule(void);
void fn_800AE338(void);
void fn_800AE380(void);
void fn_800AE3C4(void);
void fn_800AE3F8(int nObj);
void fn_800AF0A8(CamLens* pLens);
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80035F1C(void);
void fn_800352BC(void);
void fn_8005C15C(int nPlayer, f32* pSpinY, f32* pSpinX);   // Swing.c: the spin asked for
void fn_8000ADC0(f32 (*pMtx)[4]);                          // identity
void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);   // VecMath.c
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);          // copy a matrix
void fn_8000C5A4(f32 (*pMtx)[4]);
void fn_800AEFE4(void);
void fn_800AF0D4(UObjMesh* pMesh);
void fn_8003612C(LightGroup* pGroup);   // Skin.c: load the group's lights (fn_8006E7A4)
void fn_8006EADC(UObject* pObj);        // GoLighting.c: light the object
void fn_8006ED70(void);                 // GoLighting.c
void fn_80035294(void);                 // GoTerrain.c

// Set up the objects: their settings, lens, textures and one directional light.
void UI_Obj_InitModule(void) {
    u64 uName;
    int i;

    lbl_802820D0 = NULL;
    lbl_801F5B98[0].a28[0] = 1.35f;
    lbl_801F5B98[0].a28[1] = 0.74f;
    lbl_801F5B98[0].a28[2] = 0.03f;
    lbl_801F5B98[0].a28[3] = 0.0f;
    fn_800AE380();
    lbl_802820CC = CA_spCreateCamera();
    CA_vInitCamera(lbl_802820CC);
    for (i = 0; i < 2; i++) {
        lbl_801F5B98[i].a0[0] = -0.345f;
        lbl_801F5B98[i].a0[1] = -0.23f;
        lbl_801F5B98[i].a0[2] = -3.13f;
        lbl_801F5B98[i].a0[3] = 0.0f;
        lbl_801F5B98[i].a0[4] = 0.0f;
        lbl_801F5B98[i].a0[5] = 0.0f;
        lbl_801F5B98[i].a0[6] = 0.00168f;
        lbl_801F5B98[i].a0[7] = 0.02f;
        lbl_801F5B98[i].a0[8] = 0.02f;
        lbl_801F5B98[i].a0[9] = 0.0f;
    }
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C8);
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C4);
    uName = fn_8000BEE4("ring");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C0);
    fn_8006E5A8(&lbl_801F5B40, 1);
    lbl_801F5B40.apLight[0]->nType = 1;
    lbl_801F5B40.apLight[0]->u.dir.f10 = 1.0f;
    lbl_801F5B40.apLight[0]->u.dir.fC = 1.0f;
}

// Free the object, the lens and the lights.
void fn_800AE338(void) {
    if (lbl_802820D0 != NULL) {
        fn_80048860(lbl_802820D0);
    }
    lbl_802820D0 = NULL;
    CA_vReleaseCamera(lbl_802820CC);
    fn_8006E62C(&lbl_801F5B40);
}

// Make the object from its 'TEO ' model once that has streamed in.
// port: a 'TEO ' object's UStreamObject.uUnk4 holds its model (see rcmp_mad_codec.c fn_800B9B48).
void fn_800AE380(void) {
    if (lbl_802820D0 == NULL) {
        lbl_802820D0 = fn_80048808((UObjModel*)fn_8000B70C('TEO ', 10003)->uUnk4);
    }
}

void fn_800AE3C4(void) {
    lbl_801F5B78[0] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[1] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[2] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[3] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[4] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[5] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[6] = lbl_801F5B98[0].a28[3];
    lbl_801F5B78[7] = lbl_801F5B98[0].a28[3];
}

// Draw object nObj's screen: the power boost quads (the base, one grown by the boost level, and a
// ring per level that grows and fades), then the ball-like model tilted toward the spin asked
// for and rolling with it. The object's own lens is used, and the view put back afterwards.
void fn_800AE3F8(int nObj) {
    f32 aBlack[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 aEye[4] = {0.0f, 0.0f, -100.0f, 0.0f};
    f32 aColour[4] = {0.5f, 0.5f, 0.5f, 0.25f};
    f32 aBase[4] = {0.5f, 0.5f, 0.5f, 0.25f};
    f32 aUp[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    f32 aDir[4];
    f32 vPos[4];
    f32 aRect[8];
    f32 aUV[16] = {
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };
    f32 aQuad[12] = {
        -0.03f, 0.03f,  0.0f, 0.03f,  0.03f,  0.0f,
        -0.03f, -0.03f, 0.0f, 0.03f,  -0.03f, 0.0f,
    };
    f32 aXYZ[16];
    f32 mSave80[4][4];
    f32 mSave40[4][4];
    f32 mSave0[4][4];
    f32 mScale[4][4];
    f32 mRot[4][4];
    f32 mRoll[4][4];
    f32 mTilt[4][4];
    UIObjSettings* pSet;
    CamLens* pLens;
    f32 fSpinX;
    f32 fSpinY;
    f32 fY;
    f32 fBoost;
    f32 fMax;
    f32 fFade;
    f32 fRange;
    f32 fRing;
    f32 fDot;
    f32 fSinRoll;
    f32 fCosRoll;
    f32 fSinTilt;
    f32 fCosTilt;
    int nPlayer;
    int i;

    nPlayer = fn_8001707C(nObj);
    // the object rises with the GameBreaker letterbox
    if (lbl_80202898.bGameBreaker == 0) {
        fY = -0.23f;
    } else {
        fY = (0.23f - 0.17f) * (GameEffects_GetLetterboxHeight() / 0.15f) + -0.23f;
    }
    pSet = &lbl_801F5B98[nObj];
    vPos[3] = 1.0f;
    aRect[0] = 0.5f;
    pSet->a0[1] = fY;
    vPos[0] = pSet->a0[0];
    vPos[1] = pSet->a0[1];
    vPos[2] = pSet->a0[2];
    aRect[1] = 0.5f;
    aRect[2] = 1.0f;
    aRect[3] = 1.0f;
    aRect[4] = 1.0f;
    aRect[5] = 1.0f;
    aRect[6] = 1.0f;
    aRect[7] = 1.0f;

    // clear the depth under the object's corner of the screen
    fn_8001425C(0);
    fn_80014194(aBlack);
    fn_80035F1C();
    fn_80014118(0);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    fn_800140E8(0, 0x200, 0x1C0, lbl_80281B88 & 1, 2, 1);
    fn_80012EF8();
    GXSetZMode(1, 7, 1);
    fn_8001644C(0xA1, aRect, NULL, NULL, 2);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_800140E8(0, 0x200, 0x1C0, lbl_80281B88 & 1, 8, 1);
    fn_80012EF8();

    // the object's own lens
    pLens = fn_8001F004();
    CA_vInitCamera(lbl_802820CC);
    CA_vSetLookAt(lbl_802820CC, aEye, lbl_8018EC10);
    fn_80045470(lbl_802820CC, 0.00879646f);
    fn_800AF0A8(lbl_802820CC);
    fn_800352BC();
    fn_80013CCC(fn_8001614C());
    fn_80013EEC(fn_8001614C());
    fn_80016B9C();

    fBoost = (f32)gPlayers[nPlayer].swing.nPowerBoost * 0.125f;
    fn_80014118(0x50);
    fn_80012F34(1);
    fn_8001425C(1);
    fn_80035118(4, 5);
    fn_80012F50(1, 4, 1);

    // the base quad
    fn_8005CC64(lbl_802820BC, lbl_802820C8);
    fn_80012EF8();
    for (i = 0; i < 4; i++) {
        aXYZ[i * 4 + 0] = aQuad[i * 3 + 0] + pSet->a0[0];
        aXYZ[i * 4 + 1] = aQuad[i * 3 + 1] + pSet->a0[1];
        aXYZ[i * 4 + 2] = aQuad[i * 3 + 2] + pSet->a0[2];
        aXYZ[i * 4 + 3] = 1.0f;
    }
    fn_80014194(aBase);
    fn_8001644C(0x98, aXYZ, NULL, aUV, 4);

    // the same, grown by the boost level, in the level's colour
    for (i = 0; i < 4; i++) {
        aXYZ[i * 4 + 0] = fBoost * aQuad[i * 3 + 0] + pSet->a0[0];
        aXYZ[i * 4 + 1] = fBoost * aQuad[i * 3 + 1] + pSet->a0[1];
        aXYZ[i * 4 + 2] = fBoost * aQuad[i * 3 + 2] + pSet->a0[2];
        aXYZ[i * 4 + 3] = 1.0f;
    }
    fn_8005CC64(lbl_802820BC, lbl_802820C4);
    fn_80012F50(0, 6, 0x80);
    if (gPlayers[nPlayer].swing.nPowerBoost > 0) {
        Vec3Copy(lbl_8018830C[gPlayers[nPlayer].swing.nPowerBoost - 1], aColour);
    }
    fn_80014194(aColour);
    fn_80012EF8();
    fn_8001644C(0x98, aXYZ, NULL, aUV, 4);

    // a ring per level: each grows until it passes the largest size, fading out on the way
    fn_8005CC64(lbl_802820BC, lbl_802820C0);
    fn_80012EF8();
    fMax = lbl_801F5B98[0].a28[0];
    fFade = lbl_801F5B98[0].a28[1];
    fRange = fMax - fFade;
    for (i = 0; i < gPlayers[nPlayer].swing.nPowerBoost; i++) {
        fRing = lbl_801F5B78[i];
        if (!(fRing > fMax)) {
            aXYZ[0] = aQuad[0] * fRing + pSet->a0[0];
            aXYZ[1] = aQuad[1] * fRing + pSet->a0[1];
            aXYZ[2] = aQuad[2] * fRing + pSet->a0[2];
            aXYZ[4] = aQuad[3] * fRing + pSet->a0[0];
            aXYZ[5] = aQuad[4] * fRing + pSet->a0[1];
            aXYZ[6] = aQuad[5] * fRing + pSet->a0[2];
            aXYZ[8] = aQuad[6] * fRing + pSet->a0[0];
            aXYZ[9] = aQuad[7] * fRing + pSet->a0[1];
            aXYZ[10] = aQuad[8] * fRing + pSet->a0[2];
            aXYZ[3] = 1.0f;
            aXYZ[7] = 1.0f;
            aXYZ[11] = 1.0f;
            aXYZ[12] = aQuad[9] * fRing + pSet->a0[0];
            aXYZ[13] = aQuad[10] * fRing + pSet->a0[1];
            aXYZ[14] = aQuad[11] * fRing + pSet->a0[2];
            aXYZ[15] = 1.0f;
            aColour[0] = lbl_8018830C[i][0];
            aColour[1] = lbl_8018830C[i][1];
            aColour[2] = lbl_8018830C[i][2];
            if (lbl_801F5B78[i] < fFade) {
                aColour[3] = 1.0f;
            } else {
                aColour[3] = 1.0f - (lbl_801F5B78[i] - fFade) / fRange;
            }
            fn_80014194(aColour);
            fn_8001644C(0x98, aXYZ, NULL, aUV, 4);
            lbl_801F5B78[i] += lbl_801F5B98[0].a28[2];
        }
    }

    // the model: tilted toward the spin asked for, rolling faster the more is asked
    if (gPlayers[nPlayer].swing.nSpinBoost > 0) {
        fn_8005C15C(nPlayer, &fSpinY, &fSpinX);
        if (fSpinY != 0.0f || fSpinX != 0.0f) {
            aDir[0] = fSpinX;
            aDir[1] = fSpinY;
            aDir[2] = 0.0f;
            aDir[3] = 1.0f;
            fn_800BAF04(aDir, aDir);
            if (fn_8000C5FC(aUp, aDir) < -1.0f) {
                fDot = -1.0f;
            } else if (fn_8000C5FC(aUp, aDir) > 1.0f) {
                fDot = 1.0f;
            } else {
                fDot = fn_8000C5FC(aUp, aDir);
            }
            lbl_801F5B98[nObj].a0[5] = fn_80009614(fDot);
            if (fSpinY < 0.0f) {
                lbl_801F5B98[nObj].a0[5] = -lbl_801F5B98[nObj].a0[5];
            }
            fn_8000ADC0(mRot);
            fn_8000ADC0(mRoll);
            fn_8000ADC0(mTilt);
            fn_8000ADC0(mScale);
            mScale[0][0] = mScale[1][1] = mScale[2][2] = lbl_801F5B98[nObj].a0[6];
            fSinRoll = fn_800095F0(lbl_801F5B98[nObj].a0[9]);
            fCosRoll = fn_80009638(lbl_801F5B98[nObj].a0[9]);
            fSinTilt = fn_800095F0(lbl_801F5B98[nObj].a0[5]);
            fCosTilt = fn_80009638(lbl_801F5B98[nObj].a0[5]);
            if (fSpinX < 0.0f && fSpinY < 0.0f) {
                lbl_801F5B98[nObj].a0[9] += (fabsf(fSpinX) > fabsf(fSpinY)) ? fabsf(fSpinX) : fabsf(fSpinY);
            } else {
                lbl_801F5B98[nObj].a0[9] += (fabsf(fSpinX) > fabsf(fSpinY)) ? fabsf(fSpinX) : fabsf(fSpinY);
            }
            if (lbl_801F5B98[nObj].a0[9] > 2.0f * PI) {
                lbl_801F5B98[nObj].a0[9] = 0.0f;
            } else if (lbl_801F5B98[nObj].a0[9] < 0.0f) {
                lbl_801F5B98[nObj].a0[9] = 2.0f * PI;
            }
            mRoll[1][1] = fCosRoll;
            mRoll[1][2] = fSinRoll;
            mRoll[2][1] = -fSinRoll;
            mRoll[2][2] = fCosRoll;
            mTilt[0][0] = fCosTilt;
            mTilt[0][1] = fSinTilt;
            mTilt[1][0] = -fSinTilt;
            mTilt[1][1] = fCosTilt;
            fn_800BAE5C(mTilt, mRoll, mRot, 3);

            // draw it with the rotation, scale and position, then put its matrices back
            fn_8000A0E8(lbl_802820D0->m0, mSave0);
            fn_8000A0E8(lbl_802820D0->m40, mSave40);
            fn_8000A0E8(lbl_802820D0->m80, mSave80);
            fn_800BAE5C(mRot, lbl_802820D0->m0, lbl_802820D0->m0, 3);
            fn_800BAE5C(mScale, lbl_802820D0->m40, lbl_802820D0->m40, 3);
            fn_8000C5A4(lbl_802820D0->m0);
            Vec_Copy(vPos, lbl_802820D0->m80[3]);
            lbl_802820D0->m80[3][3] = 1.0f;
            fn_800AEFE4();
            fn_8000A0E8(mSave40, lbl_802820D0->m40);
            fn_8000A0E8(mSave0, lbl_802820D0->m0);
            fn_8000A0E8(mSave80, lbl_802820D0->m80);
        }
    }

    // the view as it was
    fn_80035240(NULL);
    fn_800AF0A8(pLens);
    fn_80016B9C();
    fn_800352BC();
    fn_80013CCC(fn_8001614C());
    fn_80035098(0);
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012EF8();
}

// Draw the object: its light's colour, the renderer state, the view, then the model.
void fn_800AEFE4(void) {
    lbl_801F5B40.apLight[0]->u.dir.vColor[0] = lbl_802820B8;
    lbl_801F5B40.apLight[0]->u.dir.vColor[1] = lbl_80281470;
    lbl_801F5B40.apLight[0]->u.dir.vColor[2] = lbl_80281474;
    lbl_801F5B40.apLight[0]->u.dir.vColor[3] = 0.0f;
    fn_8003612C(&lbl_801F5B40);
    fn_8006EADC(lbl_802820D0);
    fn_80035118(4, 5);
    fn_80035098(1);
    fn_80035154(255.0f * lbl_80281478);
    fn_80012EF8();
    fn_80035240(lbl_802820D0->m80);
    fn_80035294();
    fn_80016B9C();
    fn_80035138(1);
    fn_80012EF8();
    fn_800AF0D4(lbl_802820D0->pModel->apLod[0]);
    fn_8006ED70();
}

// Make pLens the render context's lens.
void fn_800AF0A8(CamLens* pLens) {
    // port: fn_80013E38 (GoRenderCtx_Gc.c, still sweep code) takes the lens as an s32
    fn_80013E38((u8*)*lbl_80280DF0, (s32)pLens);
}

// Draw the mesh's current part, if it is used (UObject.c's fn_80048A84 again).
void fn_800AF0D4(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}
