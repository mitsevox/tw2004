// target.c (TW06's target.c, golf/ai/target.c): where the CPU aims, the aim marker drawn at the
// target, and where a ball may be placed or dropped (TW06's PlaceBall_* functions). Partly
// decompiled; the machine-style code at the end is the sweep's.

#include "game.h"
#include "camera.h"
#include "unsorted/cull.h"

void fn_80067B80(void);
void fn_80067CD4(int nPlayer);
void fn_80067DAC(int nPlayer);
void fn_80068AA8(int nPlayer);
void fn_800690C0(int nPlayer);
void fn_80069104(int nPlayer);
void fn_80069148(int nPlayer);
void fn_800691B0(int nPlayer);
void fn_800693A4(int nPlayer);
void fn_80069A84(int nPlayer);
void fn_80069AFC(int nPlayer);
void fn_80069B74(int nPlayer);
void fn_80069BEC(int nPlayer);
void fn_80069CDC(int nPlayer);
void fn_8006A7A8(u8* pChunk);
void fn_8006A89C(void);
void fn_8006A8B0(void);
void fn_8006A964(f32* pA, f32* pB, f32* pOut);
void fn_8006A988(f32* pA, f32* pB, f32* pOut);
f32  fn_8006A9FC(void);
LLFont* fn_8006AA3C(void);

// Set up at boot: the hole's chunk 3 loader, the marker textures and every player's marker.
void fn_80067B80(void) {
    int i;

    Course_RegisterLoader(3, fn_8006A7A8);
    lbl_80281E30 = NULL;
    fn_800102DC(fn_8000BEE4("tball"), &lbl_80281E3C, &lbl_80281E40);
    fn_800102DC(fn_8000BEE4("shadow"), &lbl_80281E34, &lbl_80281E38);
    for (i = 0; i < 5; i++) {
        lbl_801D5BF0[i].f0 = 0.01f;
        lbl_801D5BF0[i].f4 = 0.0f;
        lbl_801D5BF0[i].f8 = 0.04f;
        lbl_801D5BF0[i].fC = 0.5f;
        lbl_801D5BF0[i].n10 = 0;
        lbl_801D5BF0[i].f14 = -0.6f;
        lbl_801D5BF0[i].f18 = 0.3f;
        lbl_801D5BF0[i].f1C = 0.5f;
        lbl_801D5BF0[i].f20 = 0.2f;
        lbl_801D5BF0[i].f24 = 0.2f;
    }
}

f32  fn_8001EFFC(CamLens* pLens);      // char.c: the lens's fB0
void fn_8006752C(void);

// Each frame: while the ball is being placed, the placement cursor; otherwise, for a human
// lining up a shot (states 2..4, 8..10) before the swing starts, the aim marker.
void fn_80067CD4(int nPlayer) {
    // fake match: the state is kept as returned and narrowed at each test (the states fit an s8);
    // states 2..4 are one unsigned range test
    int nState = GOLFERSTATE_GetCurrentState(nPlayer);

    if (fn_800E2EAC(nPlayer)) {
        fn_80069CDC(nPlayer);
        return;
    }
    if ((s8)nState != GS_WAIT && !Player_IsCPU(nPlayer) && !fn_800E415C() && !fn_800E5098()
        && ((u8)(nState - GS_SHOT_SETUP) <= GS_ELEVATOR - GS_SHOT_SETUP || (s8)nState == GS_KNEE_CAM
            || (s8)nState == GS_SWING || (s8)nState == GS_GREEN_MORPH)
        && gPlayers[nPlayer].swing.nState == 0) {
        fn_80067DAC(nPlayer);
    }
}

void UFont_SetFlags_80012C84(int a);
void fn_800E5118(int a, int b, int nPlayer);                       // GameMessages.c
void fn_800E5178(int nPlayer, f32 a, f32 b, f32 c, f32 d);          // GameMessages.c

// The aim marker at the target, each frame while a human lines up a shot: a sign turned to face
// the camera, bigger the further the camera is, and its shadow; with f8 set it cycles through
// three heights (every 0.5 / f8 frames), drawn in three shades. Passes the target's screen
// position, the lie's height difference and the shot's share of the club's range to the HUD.
void fn_80067DAC(int nPlayer) {
    f32   aMarker[4][4];
    f32   aShadow[4][4];
    f32   aMarkerQuad[4][3];
    f32   aUV[16] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    f32   aShadowQuad[4][3];
    f32   aY2[4];
    f32   aY1[4];
    f32   aY0[4];
    f32   aFaint[4] = {1.0f, 1.0f, 1.0f, 0.125f};
    f32   aSolid[4] = {1.0f, 1.0f, 1.0f, 0.5f};
    f32   aLight[4] = {1.0f, 1.0f, 1.0f, 0.175f};
    f32   aText[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    f32   vUp[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    f32   vCross[4];
    f32   vPos[4];
    f32   vDir[4];
    f32   fX;
    f32   fX1;
    f32   fX2;
    f32   fY;
    f32   fCos;
    f32   fDot;
    f32   fCamDist;
    f32   fRise;
    f32   fSin;
    f32   fShare;
    f32   fDist;
    f32   fTilt;
    f32   fBob;
    f32   fScale;
    f32   fStep;
    f32   fShadow;
    f32*  pCamPos;
    f32*  pLook;
    View* pView;
    void* pCamera;
    int   nX;
    int   nY;
    int   nFrame;
    int   i;
    u8    bOnScreen;

    pCamera = fn_80017004(gPlayers[nPlayer].nView[0]);
    aMarkerQuad[0][0] = 2.0f * lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[0][1] = 2.0f * lbl_801D5BF0[nPlayer].f24 / fn_80017028(gPlayers[nPlayer].nView[0])->f54;
    aMarkerQuad[0][2] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[1][0] = 2.0f * lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[1][1] = 2.0f * lbl_801D5BF0[nPlayer].f24 / fn_80017028(gPlayers[nPlayer].nView[0])->f54;
    aMarkerQuad[1][2] = lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[2][0] = 0.0f;
    aMarkerQuad[2][1] = 0.0f;
    aMarkerQuad[2][2] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[3][0] = 0.0f;
    aMarkerQuad[3][1] = 0.0f;
    aMarkerQuad[3][2] = lbl_801D5BF0[nPlayer].f24;
    fDist = gPlayers[nPlayer].fDistance2;
    fShadow = lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[0][0] = fShadow;
    aShadowQuad[0][1] = 0.0f;
    aShadowQuad[0][2] = -fShadow;
    aShadowQuad[1][0] = fShadow;
    aShadowQuad[1][1] = 0.0f;
    aShadowQuad[1][2] = fShadow;
    aShadowQuad[2][0] = -fShadow;
    aShadowQuad[2][1] = 0.0f;
    aShadowQuad[2][2] = -fShadow;
    aShadowQuad[3][0] = -fShadow;
    aShadowQuad[3][1] = 0.0f;
    aShadowQuad[3][2] = fShadow;
    Vec_Copy(gPlayers[nPlayer].vTargetCopy, vPos);
    if (gPlayers[nPlayer].uFlagsEF0 & 2) {
        aShadowQuad[0][1] -= 1.0f / 9.0f;
        aShadowQuad[1][1] -= 1.0f / 9.0f;
        aShadowQuad[2][1] -= 1.0f / 9.0f;
        aShadowQuad[3][1] -= 1.0f / 9.0f;
    }

    pView = fn_80017028(gPlayers[nPlayer].nView[0]);
    pCamPos = fn_8001731C(pView);
    pLook = fn_80017314(pView);
    fCamDist = Vec_Distance(pCamPos, vPos);
    fn_8006A964(pLook, pCamPos, vDir);
    vDir[1] = 0.0f;
    if (0.0f == vDir[0] && 0.0f == vDir[2]) {
        return;
    }
    fn_800BAF04(vDir, vDir);
    fDot = fn_8000C5FC(vUp, vDir);
    vec4flt_CrossProduct(vDir, vUp, vCross);
    fCos = (fDot < -1.0f) ? -1.0f : ((fDot > 1.0f) ? 1.0f : fDot);
    fSin = fn_80009680(1.0f - fCos * fCos);
    if (vCross[1] < 0.0f) {
        fSin = -fSin;
    }

    // the HUD: where the target is on screen, how far above or below the ball, and the share of
    // the club's range (1..100)
    UFont_SetFlags_80012C84(0);
    fn_8006A9AC(aText);
    fn_8006434C(pCamera, vPos, &fX, &fY, NULL);
    fn_8006A8D4(pCamera, &fX, &fY);
    fRise = vPos[1] - gPlayers[nPlayer].vBall[1];
    fShare = 100.0f * (gPlayers[nPlayer].fDistance
                       / AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub));
    if (fShare < 1.0f) {
        fShare = 1.0f;
    }
    if (fShare > 100.0f) {
        fShare = 100.0f;
    }
    if (gPlayers[nPlayer].nShotKind != 0) {
        if (fDist < 1.0f) {
            fDist = 1.0f;
        } else if (3.0f * fDist < 1.0f) {   // never true after the test above
            fDist = 1.0f / 3.0f;
        }
    }
    fCamDist /= 300.0f;
    if (fRise > -0.015f && fRise < 0.015f) {
        fTilt = 0.0f;
        fStep = fTilt;
    } else {
        fTilt = 3.0f * fRise;
        fStep = 36.0f * fRise;
    }
    fn_800E5178(nPlayer, fDist, fTilt, fStep, fShare);

    fn_80012F50(0, 6, 0x7F);
    fn_80012F18(3);
    fn_80012F34(0);
    fn_80035118(4, 5);
    fn_8005CC64(lbl_80281E3C, lbl_80281E40);
    fn_80014118(0x50);
    fn_80012EF8();
    fn_8001425C(1);
    fScale = 8.0f * fCamDist + 1.0f;
    for (i = 0; i < 4; i++) {
        aMarker[i][2] = fScale * (fCos * aMarkerQuad[i][0]) + fScale * (fSin * aMarkerQuad[i][2]);
        aMarker[i][0] = fScale * (-fSin * aMarkerQuad[i][0]) + fScale * (fCos * aMarkerQuad[i][2]);
        aMarker[i][1] = fScale * aMarkerQuad[i][1];
    }
    for (i = 0; i < 4; i++) {
        aMarker[i][0] = vPos[0] + aMarker[i][0];
        aMarker[i][1] = vPos[1] + aMarker[i][1] - fScale * lbl_801D5BF0[nPlayer].f14;
        aMarker[i][2] = vPos[2] + aMarker[i][2];
        aMarker[i][3] = 1.0f;
        aShadow[i][0] = vPos[0] + aShadowQuad[i][0];
        aShadow[i][1] = vPos[1] + aShadowQuad[i][1] + lbl_801D5BF0[nPlayer].f20;
        aShadow[i][2] = vPos[2] + aShadowQuad[i][2];
        aShadow[i][3] = 1.0f;
    }
    bOnScreen = fn_8006434C(pCamera, aMarker[0], &fX1, &fY, NULL);
    fn_8006A8D4(pCamera, &fX1, &fY);
    bOnScreen &= fn_8006434C(pCamera, aMarker[1], &fX2, &fY, NULL);
    fn_8006A8D4(pCamera, &fX2, &fY);
    fX = 0.5f * (fX1 + fX2);

    if (0.0f != lbl_801D5BF0[nPlayer].f8) {
        // EA bug: steps by marker i's f8, i being 4 after the loop above, not the player's
        lbl_801D5BF0[nPlayer].fC -= lbl_801D5BF0[i].f8;
        if (lbl_801D5BF0[nPlayer].fC < 0.125f) {
            lbl_801D5BF0[nPlayer].n10++;
            if (lbl_801D5BF0[nPlayer].n10 > 2) {
                lbl_801D5BF0[nPlayer].n10 = 0;
            }
            lbl_801D5BF0[nPlayer].fC = 0.5f;
        }
        nFrame = lbl_801D5BF0[nPlayer].n10;
        if (nFrame == 0) {
            fBob = 0.5f * (fScale * lbl_801D5BF0[nPlayer].f18);
            for (i = 0; i < 4; i++) {
                aY2[i] = aMarker[i][1];
                aY1[i] = aMarker[i][1] - fBob;
                aY0[i] = aY1[i] - fBob;
            }
        }
        if (nFrame == 1) {
            fBob = 0.5f * (fScale * lbl_801D5BF0[nPlayer].f18);
            for (i = 0; i < 4; i++) {
                aY1[i] = aMarker[i][1];
                aY2[i] = aMarker[i][1] - fBob;
                aY0[i] = aY2[i] - fBob;
            }
        }
        if (nFrame == 2) {
            fBob = 0.5f * (fScale * lbl_801D5BF0[nPlayer].f18);
            for (i = 0; i < 4; i++) {
                aY1[i] = aMarker[i][1];
                aY0[i] = aMarker[i][1] - fBob;
                aY2[i] = aY0[i] - fBob;
            }
        }
        for (i = 0; i < 4; i++) {
            aMarker[i][1] = aY0[i];
        }
        if (bOnScreen && gPlayers[nPlayer].nSurface != -1) {
            fn_80014194(aFaint);
            fn_8001644C(0x98, aMarker[0], NULL, aUV, 4);
            for (i = 0; i < 4; i++) {
                aMarker[i][1] = aY1[i];
            }
            fn_80014194(aLight);
            fn_8001644C(0x98, aMarker[0], NULL, aUV, 4);
            for (i = 0; i < 4; i++) {
                aMarker[i][1] = aY2[i];
            }
            fn_80014194(aSolid);
            fn_8001644C(0x98, aMarker[0], NULL, aUV, 4);
        }
    } else {
        fBob = fScale * lbl_801D5BF0[nPlayer].f18;
        aMarker[0][1] -= fBob;
        aMarker[1][1] -= fBob;
        aMarker[2][1] -= fBob;
        aMarker[3][1] -= fBob;
        if (bOnScreen && gPlayers[nPlayer].nSurface != -1) {
            fn_80014194(aSolid);
            fn_8001644C(0x98, aMarker[0], NULL, aUV, 4);
        }
    }
    if (bOnScreen && gPlayers[nPlayer].nSurface != -1) {
        fn_8005CC64(lbl_80281E34, lbl_80281E38);
        fn_80012EF8();
        fn_8001644C(0x98, aShadow[0], NULL, aUV, 4);
    }
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012EF8();

    nX = 512.0f * fX;
    nY = 416.0f * fY;
    if (nY > 285) {
        nY = 285;
    }
    if (bOnScreen != lbl_801D5BF0[nPlayer].b28) {
        fn_80062C80(gPlayers[nPlayer].nC58, bOnScreen);
        lbl_801D5BF0[nPlayer].b28 = bOnScreen;
    }
    fn_800E5118(nX, nY, nPlayer);
}

// The marker's settings for the coming shot: one set for putts and shot kind 2, another for
// everything else.
void fn_800689D4(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == 0 || gPlayers[nPlayer].nShotKind == 2) {
        lbl_801D5BF0[nPlayer].f0 = 0.0f;
        lbl_801D5BF0[nPlayer].f4 = 0.0f;
        lbl_801D5BF0[nPlayer].f8 = 0.0f;
        lbl_801D5BF0[nPlayer].fC = 0.0f;
        lbl_801D5BF0[nPlayer].n10 = 0;
        lbl_801D5BF0[nPlayer].f14 = -0.11f;
        lbl_801D5BF0[nPlayer].f18 = 0.0f;
        lbl_801D5BF0[nPlayer].f1C = 0.036f;
        lbl_801D5BF0[nPlayer].f20 = 0.004f;
        lbl_801D5BF0[nPlayer].f24 = 0.1f;
        return;
    }
    lbl_801D5BF0[nPlayer].f0 = 0.01f;
    lbl_801D5BF0[nPlayer].f4 = 0.0f;
    lbl_801D5BF0[nPlayer].f8 = 0.04f;
    lbl_801D5BF0[nPlayer].fC = 0.5f;
    lbl_801D5BF0[nPlayer].n10 = 0;
    lbl_801D5BF0[nPlayer].f14 = -0.6f;
    lbl_801D5BF0[nPlayer].f18 = 0.3f;
    lbl_801D5BF0[nPlayer].f1C = 0.5f;
    lbl_801D5BF0[nPlayer].f20 = 0.2f;
    lbl_801D5BF0[nPlayer].f24 = 0.2f;
}

// The ball-placement cursor stops.
void fn_80068AA8(int nPlayer) {
    gPlayers[nPlayer].fA5C = 0.0f;
    gPlayers[nPlayer].fA60 = 0.0f;
}

// A human moving the aim point with the stick: fA5C turns it about the ball (a degree a tick at
// full stick, half that on a putt; slower the further the camera is from the ball), fA60 moves
// it nearer or further along the aim. A move past the shot's range (closer than 0.3 or 5, further
// than 180 feet on a putt, 30 on shot kind 2, else the longest usable club) is undone and fA60
// parked at -1000 or 1000 until the stick is let go. Returns whether the aim point moved.
u8 fn_80068AC8(int nPlayer) {
    f32* pTarget;
    f32  vToCamera[4];
    f32  vSaved[4];
    f32  vDir[4];
    f32  fZoom;
    f32  fStep;
    f32  fTurn;
    f32  fMin;
    f32  fSin;
    f32  fCos;
    f32  fCameraDist;
    f32  fDX;
    f32  fDZ;
    int  nClub;
    u8   bInRange;
    u8   bMoved;

    fStep = 0.5f;
    bInRange = 0;
    pTarget = gPlayers[nPlayer].vTarget;
    bMoved = 0;
    Vec3Copy(pTarget, vSaved);
    fZoom = fn_8001EFFC(fn_80008370(fn_80017004(gPlayers[nPlayer].nView[0])));

    // the turn eases off towards 0
    if (gPlayers[nPlayer].fA5C < 0.0f) {
        gPlayers[nPlayer].fA5C += 0.05f;
        if (gPlayers[nPlayer].fA5C > 0.0f) {
            gPlayers[nPlayer].fA5C = 0.0f;
        }
    } else if (gPlayers[nPlayer].fA5C > 0.0f) {
        gPlayers[nPlayer].fA5C -= 0.05f;
        if (gPlayers[nPlayer].fA5C < 0.0f) {
            gPlayers[nPlayer].fA5C = 0.0f;
        }
    }
    if (0.0f != gPlayers[nPlayer].fA5C) {
        bMoved = 1;
        fTurn = PI / 180.0f * gPlayers[nPlayer].fA5C;
        if (gPlayers[nPlayer].nShotKind == 0) {
            fTurn = PI / 360.0f * gPlayers[nPlayer].fA5C;
        }
        fn_8006A964(fn_80017028(gPlayers[nPlayer].nView[0])->v0, gPlayers[nPlayer].vBall, vToCamera);
        vToCamera[1] = 0.0f;
        fCameraDist = 0.05f * (f32)fn_80009680(fn_80009744(vToCamera));
        if (fCameraDist > 1.0f) {
            fTurn /= fCameraDist;
        }
        fTurn *= fZoom;
        gPlayers[nPlayer].fAim += 60.0f * fTurn * gSession.fFrameTime;
        if (gPlayers[nPlayer].fAim < -PI) {
            gPlayers[nPlayer].fAim += 2.0f * PI;
        } else if (gPlayers[nPlayer].fAim > PI) {
            gPlayers[nPlayer].fAim -= 2.0f * PI;
        }
        fSin = fn_800095F0(gPlayers[nPlayer].fAim);
        fCos = fn_80009638(gPlayers[nPlayer].fAim);
        fDX = -fSin * gPlayers[nPlayer].fDistance;
        fDZ = fCos * gPlayers[nPlayer].fDistance;
        gPlayers[nPlayer].vTarget[0] = fDX + gPlayers[nPlayer].vBall[0];
        gPlayers[nPlayer].vTarget[2] = fDZ + gPlayers[nPlayer].vBall[2];
        AI_PlanShot(nPlayer, pTarget);
        Vec_Copy(pTarget, gPlayers[nPlayer].vTarget2);
        fn_8001C804(nPlayer, 0, 1);
        fn_80062C38();
        fn_8006A8B0();
    }

    Vec3Copy(pTarget, vSaved);
    if (1000.0f == fabsf(gPlayers[nPlayer].fA60)) {
        return bMoved;
    }
    // the push eases off towards 0
    if (gPlayers[nPlayer].fA60 < 0.0f) {
        gPlayers[nPlayer].fA60 += 3.0f * gSession.fFrameTime;
        if (gPlayers[nPlayer].fA60 > 0.0f) {
            gPlayers[nPlayer].fA60 = 0.0f;
        }
    } else if (gPlayers[nPlayer].fA60 > 0.0f) {
        gPlayers[nPlayer].fA60 -= 3.0f * gSession.fFrameTime;
        if (gPlayers[nPlayer].fA60 < 0.0f) {
            gPlayers[nPlayer].fA60 = 0.0f;
        }
    }
    if (0.0f != gPlayers[nPlayer].fA60) {
        if (gPlayers[nPlayer].nShotKind == 0 || gPlayers[nPlayer].nShotKind == 2) {
            fStep = 0.2f;
            fMin = 0.3f;
        } else {
            fMin = 5.0f;
        }
        bMoved = 1;
        fStep *= gSession.fFrameTime * (60.0f * gPlayers[nPlayer].fA60);
        fSin = fn_800095F0(gPlayers[nPlayer].fAim);
        fCos = fn_80009638(gPlayers[nPlayer].fAim);
        fDX = fStep * -fSin;
        fDZ = fStep * fCos;
        gPlayers[nPlayer].vTarget[0] += fDX;
        gPlayers[nPlayer].vTarget[2] += fDZ;
        fn_8006A964(pTarget, gPlayers[nPlayer].vBall, vDir);
        vDir[1] = 0.0f;
        if (gPlayers[nPlayer].fA60 < 0.0f && (f32)fn_80009680(fn_80009744(vDir)) >= fMin) {
            bInRange = 1;
        } else if (gPlayers[nPlayer].fA60 > 0.0f
                   && ((gPlayers[nPlayer].nShotKind == 0 && 3.0f * gPlayers[nPlayer].fDistance < 180.0f)
                       || (gPlayers[nPlayer].nShotKind == 2 && gPlayers[nPlayer].fDistance < 30.0f)
                       || (gPlayers[nPlayer].nShotKind != 0 && gPlayers[nPlayer].nShotKind != 2
                           && gPlayers[nPlayer].fDistance
                                  < AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind,
                                                   AI_FirstUsableClub(nPlayer,
                                                                      gPlayers[nPlayer].nShotKind))))) {
            bInRange = 1;
        }
        if (bInRange) {
            AI_PlanShot(nPlayer, pTarget);
            fSin = fn_800095F0(gPlayers[nPlayer].fAim);
            fCos = fn_80009638(gPlayers[nPlayer].fAim);
            fDX = -fSin * gPlayers[nPlayer].fDistance;
            fDZ = fCos * gPlayers[nPlayer].fDistance;
            gPlayers[nPlayer].vTarget[0] = fDX + gPlayers[nPlayer].vBall[0];
            gPlayers[nPlayer].vTarget[2] = fDZ + gPlayers[nPlayer].vBall[2];
            AI_PlanShot(nPlayer, pTarget);
            Vec_Copy(pTarget, gPlayers[nPlayer].vTarget2);
            if (gPlayers[nPlayer].nShotKind != 2) {
                nClub = gPlayers[nPlayer].nClub;
                gPlayers[nPlayer].nClub = AI_ClubForShot(nPlayer, gPlayers[nPlayer].nShotKind, 1,
                                                         gPlayers[nPlayer].fDistance);
                if (nClub != gPlayers[nPlayer].nClub) {
                    fn_8006752C();
                }
            }
            gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
            fn_8001C774(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
            fn_8001C724(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
            fn_8001C804(nPlayer, 0, 1);
            fn_80062C38();
            fn_8006A8B0();
        } else {
            if (gPlayers[nPlayer].fA60 < 0.0f) {
                gPlayers[nPlayer].fA60 = -1000.0f;
            } else {
                gPlayers[nPlayer].fA60 = 1000.0f;
            }
            Vec3Copy(vSaved, pTarget);
        }
    }
    return bMoved;
}

// The placement cursor speeds up towards -x, at most -1.
void fn_800690C0(int nPlayer) {
    gPlayers[nPlayer].fA5C -= 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA5C < -1.0f) {
        gPlayers[nPlayer].fA5C = -1.0f;
    }
}

// The placement cursor speeds up towards +x, at most 1.
void fn_80069104(int nPlayer) {
    gPlayers[nPlayer].fA5C += 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA5C > 1.0f) {
        gPlayers[nPlayer].fA5C = 1.0f;
    }
}

// The placement cursor speeds up towards +z, at most 1. 1000 holds it; -1000 is reset to 0 first.
void fn_80069148(int nPlayer) {
    if (1000.0f == gPlayers[nPlayer].fA60) return;
    if (-1000.0f == gPlayers[nPlayer].fA60) {
        gPlayers[nPlayer].fA60 = 0.0f;
    }
    gPlayers[nPlayer].fA60 += 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA60 > 1.0f) {
        gPlayers[nPlayer].fA60 = 1.0f;
    }
}

// The placement cursor speeds up towards -z, at most -1. -1000 holds it; 1000 is reset to 0 first.
void fn_800691B0(int nPlayer) {
    if (-1000.0f == gPlayers[nPlayer].fA60) return;
    if (1000.0f == gPlayers[nPlayer].fA60) {
        gPlayers[nPlayer].fA60 = 0.0f;
    }
    gPlayers[nPlayer].fA60 -= 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA60 < -1.0f) {
        gPlayers[nPlayer].fA60 = -1.0f;
    }
}

// Whether a ball may be placed at pPos: in bounds, not in a free-drop area, on ground that takes
// a ball (not water, 7 and 16, or the cup, 12) and flat enough (the normal's y above cos 30);
// then class 1 always, else a valid drop surface or a spot with no object or hazard near it.
u8 fn_80069218(f32* pPos) {
    SurfaceType* pSurface;
    f32          vNormal[4];

    Ter_GetSupportingGroundData(fn_8000C594(), pPos, &pSurface, vNormal);
    if (Ter_PointInOOBNetwork(pPos) && !Ter_PointInFreeDropNetwork(pPos) && pSurface != NULL
        && (pSurface->u34 & 1) && pSurface->nClass != 7 && pSurface->nClass != 16 && pSurface->nClass != 12
        && fabsf(vNormal[1]) > 0.86603f) {
        if (pSurface->nClass == 1 || Ter_IsValidDropSurface(pSurface - gSurfaceTypes)
            || !Ter_CheckObjectAndHazardObstruction(pPos, 1.5f, 0, 1, 2.0f, 1, 0.577f)) {
            return 1;
        }
    }
    return 0;
}

// Puts the placement point over pPos, on the ground if there is any there.
void fn_80069330(int nPlayer, f32* pPos) {
    f32 fHeight;

    gPlayers[nPlayer].vPlacement[0] = pPos[0];
    gPlayers[nPlayer].vPlacement[2] = pPos[2];
    fHeight = Terrain_HeightAt(gPlayers[nPlayer].vPlacement, NULL);
    if (TER_NO_GROUND != fHeight) {
        gPlayers[nPlayer].vPlacement[1] = 0.001f + fHeight;
    }
}

// Stops the placement cursor and flags whether the ball can be placed where it is.
void fn_800693A4(int nPlayer) {
    gPlayers[nPlayer].fA5C = 0.0f;
    gPlayers[nPlayer].fA60 = 0.0f;
    if (fn_80069218(gPlayers[nPlayer].vPlacement)) {
        gPlayers[nPlayer].uFlagsEF0 |= 1;
    } else {
        gPlayers[nPlayer].uFlagsEF0 &= ~1;
    }
}

// Inside the hole's placement outline, or, with none loaded, the in-bounds outlines; with an
// outline, a point outside it still counts when some in-bounds outline holds it.
u8 fn_80069428(f32* pPos) {
    if (lbl_80281E30 != NULL) {
        if (fn_8000C140(pPos, lbl_80281E30, lbl_80281E30->nNumNodes)) {
            return 1;
        }
        if (Ter_iNumOOBNetworksLoaded() > 0) {
            return Ter_PointInOOBNetwork(pPos);
        }
        return 0;
    }
    return Ter_PointInOOBNetwork(pPos);
}

TNetwork* fn_80069498(void) {
    return lbl_80281E30;
}

// Each frame while the ball is being placed: fA80 and fA84 (the cursor's push, -1..1) ease off
// by 0.05 and move the placement point, slower on rough, sand or water (not in game mode 9);
// fA7C turns the heading fA88. A spot outside the placement outline is retried turned 15, 30 ..
// 90 degrees either way; with none inside, the cursor stops. Then flags whether the ball can be
// placed there. Returns 0 only when the cursor was stopped.
u8 PlaceBall_UpdateMomentums(int nPlayer, f32 fSpeed) {
    SurfaceType* pSurface;
    f32          vPos[3];
    f32          fGround;
    f32          fTicks;
    f32          fStep;
    f32          fZ;
    f32          fX;
    f32          fBaseX;
    f32          fBaseZ;
    f32          fSin;
    f32          fCos;
    f32          fTurn;
    f32          fAngle;
    f32          fHeading;
    u8           bMoved;

    if (Game_GetMode() != 9) {
        pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse,
                                                  gPlayers[nPlayer].vPlacement);
        switch (pSurface != NULL ? pSurface->nClass : 0) {
        case 1:
        case 2:
        case 3:
        case 4:
            fGround = 1.0f;
            break;
        case 5:
        case 20:
            fGround = 0.85f;
            break;
        case 11:
            fGround = 0.7f;
            break;
        case 6:
            fGround = 0.5f;
            break;
        case 7:
        case 15:
        case 16:
            fGround = 0.5f;
            break;
        case 8:
            fGround = 1.2f;
            break;
        case 19:
            if (pSurface->f20 <= 0.9f) {
                fGround = 1.0f;
            } else if (pSurface->f20 <= 1.0f) {
                fGround = 0.85f;
            } else {
                fGround = 0.5f;
            }
            break;
        default:
            fGround = 1.0f;
            break;
        }
    } else {
        fGround = 1.0f;
    }

    // port: NTSC rate; the frame's length in ticks
    fTicks = (59.94f / 60.0f) * (59.94f * gSession.fFrameTime);
    fStep = -0.5f * fTicks * fSpeed;
    fStep *= fGround;

    if (gPlayers[nPlayer].fA80 < 0.0f) {
        gPlayers[nPlayer].fA80 += 0.05f;
        if (gPlayers[nPlayer].fA80 > 0.0f) {
            gPlayers[nPlayer].fA80 = 0.0f;
        }
    } else if (gPlayers[nPlayer].fA80 > 0.0f) {
        gPlayers[nPlayer].fA80 -= 0.05f;
        if (gPlayers[nPlayer].fA80 < 0.0f) {
            gPlayers[nPlayer].fA80 = 0.0f;
        }
    }
    fX = fStep * gPlayers[nPlayer].fA80;
    gPlayers[nPlayer].vCBC[0] = fX;

    if (gPlayers[nPlayer].fA84 < 0.0f) {
        gPlayers[nPlayer].fA84 += 0.05f;
        if (gPlayers[nPlayer].fA84 > 0.0f) {
            gPlayers[nPlayer].fA84 = 0.0f;
        }
    } else if (gPlayers[nPlayer].fA84 > 0.0f) {
        gPlayers[nPlayer].fA84 -= 0.05f;
        if (gPlayers[nPlayer].fA84 < 0.0f) {
            gPlayers[nPlayer].fA84 = 0.0f;
        }
    }
    fZ = fStep * gPlayers[nPlayer].fA84;
    bMoved = 1;
    gPlayers[nPlayer].vCBC[2] = fZ;

    if (gPlayers[nPlayer].fA7C < 0.0f) {
        gPlayers[nPlayer].fA7C += 0.05f;
        if (gPlayers[nPlayer].fA7C > 0.0f) {
            gPlayers[nPlayer].fA7C = 0.0f;
        }
    } else if (gPlayers[nPlayer].fA7C > 0.0f) {
        gPlayers[nPlayer].fA7C -= 0.05f;
        if (gPlayers[nPlayer].fA7C < 0.0f) {
            gPlayers[nPlayer].fA7C = 0.0f;
        }
    }
    if (0.0f != gPlayers[nPlayer].fA7C) {
        fTurn = 2.0f * (fTicks * (PI / 180.0f * gPlayers[nPlayer].fA7C));
        gPlayers[nPlayer].fA88 += fTurn;
        if (gPlayers[nPlayer].fA88 < -PI) {
            gPlayers[nPlayer].fA88 += 2.0f * PI;
        } else if (gPlayers[nPlayer].fA88 > PI) {
            gPlayers[nPlayer].fA88 -= 2.0f * PI;
        }
        bMoved = 1;
    }

    if (bMoved) {
        fHeading = gPlayers[nPlayer].fA88;
        fSin = fn_800095F0(fHeading);
        fCos = fn_80009638(fHeading);
        fBaseX = gPlayers[nPlayer].vPlacement[0];
        fBaseZ = gPlayers[nPlayer].vPlacement[2];
        vPos[0] = fZ * -fSin + fBaseX + fX * fCos;
        vPos[2] = fZ * fCos + fBaseZ + fX * fSin;
        if (fn_80069428(vPos)) {
        place:
            gPlayers[nPlayer].vPlacement[0] = vPos[0];
            gPlayers[nPlayer].vPlacement[2] = vPos[2];
            fn_80069330(nPlayer, gPlayers[nPlayer].vPlacement);
        } else {
            // outside: try the step turned further and further either way
            for (fAngle = 15.0f; fAngle <= 90.0f; fAngle += 15.0f) {
                fTurn = PI / 180.0f * fAngle;
                fHeading = fTurn + gPlayers[nPlayer].fA88;
                if (fHeading > PI) {
                    fHeading -= 2.0f * PI;
                }
                fSin = fn_800095F0(fHeading);
                fCos = fn_80009638(fHeading);
                vPos[0] = fZ * -fSin + fBaseX + fX * fCos;
                vPos[2] = fZ * fCos + fBaseZ + fX * fSin;
                if (fn_80069428(vPos)) {
                    gPlayers[nPlayer].fA84 = 0.0f;
                    goto place;  // fake match: the original has one copy of the placing code
                }
                fHeading = gPlayers[nPlayer].fA88 - fTurn;
                if (fHeading < -PI) {
                    fHeading += 2.0f * PI;
                }
                fSin = fn_800095F0(fHeading);
                fCos = fn_80009638(fHeading);
                vPos[0] = fZ * -fSin + fBaseX + fX * fCos;
                vPos[2] = fZ * fCos + fBaseZ + fX * fSin;
                if (fn_80069428(vPos)) {
                    gPlayers[nPlayer].fA84 = 0.0f;
                    goto place;  // fake match: as above
                }
            }
            bMoved = 0;
            gPlayers[nPlayer].fA80 = 0.0f;
            gPlayers[nPlayer].fA84 = 0.0f;
            gPlayers[nPlayer].fA7C = 0.0f;
        }
    }
    if (fn_80069218(gPlayers[nPlayer].vPlacement)) {
        gPlayers[nPlayer].uFlagsEF0 |= 1;
    } else {
        gPlayers[nPlayer].uFlagsEF0 &= ~1;
    }
    return bMoved;
}

// fA84 (with nC38 set): as fn_80069148.
void fn_80069A84(int nPlayer) {
    if (gPlayers[nPlayer].nC38 == -1) return;
    if (1000.0f == gPlayers[nPlayer].fA84) return;
    if (-1000.0f == gPlayers[nPlayer].fA84) {
        gPlayers[nPlayer].fA84 = 0.0f;
    }
    gPlayers[nPlayer].fA84 += 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA84 > 1.0f) {
        gPlayers[nPlayer].fA84 = 1.0f;
    }
}

// fA84 (with nC38 set): as fn_800691B0.
void fn_80069AFC(int nPlayer) {
    if (gPlayers[nPlayer].nC38 == -1) return;
    if (-1000.0f == gPlayers[nPlayer].fA84) return;
    if (1000.0f == gPlayers[nPlayer].fA84) {
        gPlayers[nPlayer].fA84 = 0.0f;
    }
    gPlayers[nPlayer].fA84 -= 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA84 < -1.0f) {
        gPlayers[nPlayer].fA84 = -1.0f;
    }
}

// fA80 (with nC38 set): as fn_80069148.
void fn_80069B74(int nPlayer) {
    if (gPlayers[nPlayer].nC38 == -1) return;
    if (1000.0f == gPlayers[nPlayer].fA80) return;
    if (-1000.0f == gPlayers[nPlayer].fA80) {
        gPlayers[nPlayer].fA80 = 0.0f;
    }
    gPlayers[nPlayer].fA80 += 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA80 > 1.0f) {
        gPlayers[nPlayer].fA80 = 1.0f;
    }
}

// fA80 (with nC38 set): as fn_800691B0.
void fn_80069BEC(int nPlayer) {
    if (gPlayers[nPlayer].nC38 == -1) return;
    if (-1000.0f == gPlayers[nPlayer].fA80) return;
    if (1000.0f == gPlayers[nPlayer].fA80) {
        gPlayers[nPlayer].fA80 = 0.0f;
    }
    gPlayers[nPlayer].fA80 -= 4.5f * gSession.fFrameTime;
    if (gPlayers[nPlayer].fA80 < -1.0f) {
        gPlayers[nPlayer].fA80 = -1.0f;
    }
}

// Draw sz centred on (fX, fY).
void fn_80069C64(char* sz, f32 fX, f32 fY) {
    fX -= fn_80012C30(sz) / 2.0f;
    fY -= fn_8006A9FC() / 2.0f;
    UFont_DrawString(sz, fX, fY);
}

f32  fn_8001414C(u8* p);    // GoRenderCtx_Gc.c: of the screen rectangle

// Each frame while the ball is being placed: the bobbing marker ball and its shadow at the
// placement point, kept inside the camera's view, and the distances to the tee, the ball and the
// hole below it (the text red when the ball can't go there). A mode whose pfn230 says so, with
// nC3C's bit 0 set, is placing its ball rather than a tee: no text then.
void fn_80069CDC(int nPlayer) {
    char  szText[128];          // size unknown
    f32   aMarker[4][4];
    f32   aShadow[4][4];
    f32   aMarkerQuad[4][3];
    f32   aUV[16] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                     0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    f32   aShadowQuad[4][3];
    f32   vDir[4];
    f32   vRel[4];
    f32   vUp[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    f32   vCross[4];
    f32   vPos[4];
    f32   vTeeDir[4];
    f32   vHoleDir[4];
    f32   vBallDir[4];
    f32   fX;
    f32   fY;
    f32   fCos;
    f32   fDot;
    f32   fSin;
    f32   fTeeDist;
    f32   fHoleDist;
    f32   fBallDist;
    f32   fHalfFov;
    f32   fCos2;
    f32   fDotX;
    f32   fDotZ;
    f32   fA;
    f32   fB;
    f32   fC;
    f32   fDisc;
    f32   fDenom;
    f32   fLow;
    f32   fHigh;
    f32   fHeight;
    f32   fOld;
    View* pView;
    f32*  pCamPos;
    f32*  pLook;
    void* pCamera;
    int   i;
    u8    bInBounds;
    u8    bBall;

    bBall = 0;
    pCamera = fn_80017004(gPlayers[nPlayer].nView[0]);
    aMarkerQuad[0][0] = 0.0f;
    aMarkerQuad[0][1] = lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[0][2] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[1][0] = 0.0f;
    aMarkerQuad[1][1] = lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[1][2] = lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[2][0] = 0.0f;
    aMarkerQuad[2][1] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[2][2] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[3][0] = 0.0f;
    aMarkerQuad[3][1] = -lbl_801D5BF0[nPlayer].f24;
    aMarkerQuad[3][2] = lbl_801D5BF0[nPlayer].f24;
    aShadowQuad[0][0] = lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[0][1] = 0.0f;
    aShadowQuad[0][2] = -lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[1][0] = lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[1][1] = 0.0f;
    aShadowQuad[1][2] = lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[2][0] = -lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[2][1] = 0.0f;
    aShadowQuad[2][2] = -lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[3][0] = -lbl_801D5BF0[nPlayer].f1C;
    aShadowQuad[3][1] = 0.0f;
    aShadowQuad[3][2] = lbl_801D5BF0[nPlayer].f1C;
    if (gSession.nPaused != 0) {
        return;
    }
    if (gpGame->pfn230(nPlayer)) {
        if (!(gPlayers[nPlayer].nC3C & 1) || gSession.nPaused != 0) {
            return;
        }
        bBall = 1;
    }

    Vec_Copy(gPlayers[nPlayer].vPlacement, vPos);
    if (bBall) {
        fn_8006A988(vPos, gPlayers[nPlayer].ball.vPos, vBallDir);
        fBallDist = fn_80009680(vBallDir[0] * vBallDir[0] + vBallDir[2] * vBallDir[2]);
    } else {
        fn_8006A988(vPos, gPlayers[nPlayer].vA44, vTeeDir);
        fTeeDist = fn_80009680(vTeeDir[0] * vTeeDir[0] + vTeeDir[2] * vTeeDir[2]);
    }
    fn_8006A988(vPos, gpGame->p130, vHoleDir);
    fHoleDist = fn_80009680(vHoleDir[0] * vHoleDir[0] + vHoleDir[2] * vHoleDir[2]);
    bInBounds = Ter_PointInOOBNetwork(vPos);

    // the marker faces the camera: turned about y by the camera's heading
    pView = fn_80017028(gPlayers[nPlayer].nView[0]);
    pCamPos = fn_8001731C(pView);
    pLook = fn_80017314(pView);
    Vec_Distance(pCamPos, vPos);
    fn_8006A964(pLook, pCamPos, vDir);
    vDir[1] = 0.0f;
    if (0.0f == vDir[0] && 0.0f == vDir[2]) {
        return;
    }
    fn_800BAF04(vDir, vDir);
    fDot = fn_8000C5FC(vUp, vDir);
    vec4flt_CrossProduct(vDir, vUp, vCross);
    fCos = (fDot < -1.0f) ? -1.0f : ((fDot > 1.0f) ? 1.0f : fDot);
    fSin = fn_80009680(1.0f - fCos * fCos);
    if (vCross[1] < 0.0f) {
        fSin = -fSin;
    }
    fn_80012F50(0, 6, 0x7F);
    fn_80012F18(3);
    fn_80035118(4, 5);
    fn_8005CC64(lbl_80281E3C, lbl_80281E40);
    fn_80014118(0x50);
    fn_80012F34(0);
    fn_80012EF8();
    fn_8001425C(1);
    for (i = 0; i < 4; i++) {
        aMarker[i][2] = fCos * aMarkerQuad[i][0] + fSin * aMarkerQuad[i][2];
        aMarker[i][0] = -fSin * aMarkerQuad[i][0] + fCos * aMarkerQuad[i][2];
        aMarker[i][1] = aMarkerQuad[i][1];
    }

    // the marker's height: kept where the camera can see it (within 3/4 of its field of view)
    fHalfFov = fn_80014278(fn_8001F004());
    fHalfFov = 0.75f * fHalfFov * fn_8001414C((u8*)fn_8003526C());
    fn_8006A964(pView->v10, pView->v0, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    fn_8006A964(vPos, pView->v0, vRel);
    fDotX = vDir[0] * vRel[0];
    fDotZ = vDir[2] * vRel[2];
    fCos2 = fn_80009638(fHalfFov / 2.0f - PI / 180.0f);
    fCos2 *= fCos2;
    fA = fCos2 - vDir[1] * vDir[1];
    fB = -(2.0f * fDotX * vDir[1]) - 2.0f * fDotZ * vDir[1];
    fC = vRel[0] * (vRel[0] * fCos2) + vRel[2] * (vRel[2] * fCos2) - fDotX * fDotX - fDotZ * fDotZ
       - 2.0f * fDotX * fDotZ;
    fDisc = fB * fB - 4.0f * fA * fC;
    if (fDisc < 0.0f) {
        fDenom = 0.0f;
    } else {
        fDisc = fn_80009680(fDisc);
        fDenom = 2.0f * fA;
    }
    if (0.0f != fDenom) {
        fHigh = (-fB + fDisc) / fDenom;
        fOld = (-fB - fDisc) / fDenom;
        if (fHigh < fOld) {
            fLow = fHigh;
            fHigh = fOld;
        } else {
            fLow = fOld;
        }
    } else {
        fLow = -1000000.0f;
        fHigh = 1000000.0f;
    }
    fLow += pCamPos[1];
    fHigh += pCamPos[1];
    if (vPos[1] < fLow) {
        fHeight = fLow;
    } else if (vPos[1] > fHigh - lbl_801D5BF0[nPlayer].f18) {
        fHeight = fHigh - lbl_801D5BF0[nPlayer].f18;
    } else {
        fHeight = vPos[1];
    }
    for (i = 0; i < 4; i++) {
        aMarker[i][0] = vPos[0] + aMarker[i][0];
        aMarker[i][1] = fHeight + aMarker[i][1] - lbl_801D5BF0[nPlayer].f14;
        aMarker[i][2] = vPos[2] + aMarker[i][2];
        aMarker[i][3] = 1.0f;
        aShadow[i][0] = vPos[0] + aShadowQuad[i][0];
        aShadow[i][1] = vPos[1] + aShadowQuad[i][1] + lbl_801D5BF0[nPlayer].f20;
        aShadow[i][2] = vPos[2] + aShadowQuad[i][2];
        aShadow[i][3] = 1.0f;
    }

    // the bob: f4 runs between 0 and f18 at f0 a frame, and the shadow shrinks as it rises
    if (0.0f != lbl_801D5BF0[nPlayer].f0) {
        fOld = lbl_801D5BF0[nPlayer].f4;
        lbl_801D5BF0[nPlayer].f4 = fOld + lbl_801D5BF0[nPlayer].f0;
        aMarker[0][1] -= fOld;
        aMarker[1][1] -= fOld;
        aMarker[2][1] -= fOld;
        aMarker[3][1] -= fOld;
        if (lbl_801D5BF0[nPlayer].f4 > lbl_801D5BF0[nPlayer].f18 || lbl_801D5BF0[nPlayer].f4 < 0.0f) {
            lbl_801D5BF0[nPlayer].f0 = -lbl_801D5BF0[nPlayer].f0;
        }
        lbl_801D5BF0[nPlayer].f1C = 0.07f * -lbl_801D5BF0[nPlayer].f0 + lbl_801D5BF0[nPlayer].f1C;
    }
    fn_80014194(lbl_801887EC);
    fn_8001644C(0x98, aMarker[0], NULL, aUV, 4);
    fn_8005CC64(lbl_80281E34, lbl_80281E38);
    fn_80012EF8();
    fn_8001644C(0x98, aShadow[0], NULL, aUV, 4);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80014194(lbl_801887CC);
    fn_80012EF8();

    if (!bBall) {
        // the distances, under the marker and kept on screen
        UFont_SetFlags_80012C84(0);
        if (bInBounds && (gPlayers[nPlayer].uFlagsEF0 & 1)) {
            fn_8006A9AC(lbl_801887CC);
        } else {
            fn_8006A9AC(lbl_801887DC);
        }
        fn_8006434C(pCamera, vPos, &fX, &fY, NULL);
        fn_8006A8D4(pCamera, &fX, &fY);
        fY -= 3.0f * fn_8006A9FC();
        if (fY < 3.0f * fn_8006A9FC()) {
            fY = 3.0f * fn_8006A9FC();
        } else if (fY > 1.0f - 3.0f * fn_8006A9FC()) {
            fY = 1.0f - 3.0f * fn_8006A9FC();
        }
        if (bBall) {
            sprintf(szText, "Ball %0.0fyds", fBallDist);
        } else {
            sprintf(szText, "Tee %0.0fyds", fTeeDist);
        }
        fn_80069C64(szText, fX, fY);
        fY += fn_8006A9FC();
        sprintf(szText, "Hole %0.0fyds", fHoleDist);
        fn_80069C64(szText, fX, fY);
    }
}

// fA88: the heading from the game's point (gpGame->p130) to the placement point, less a quarter
// turn; and the aim marker's usual settings.
void fn_8006A6C4(int nPlayer) {
    f32 vDir[4];

    fn_8006A988(gPlayers[nPlayer].vPlacement, gpGame->p130, vDir);
    vDir[1] = 0.0f;
    Vec_Normalize(vDir, vDir);
    gPlayers[nPlayer].fA88 = atan2f(vDir[2], vDir[0]) - PI / 2.0f;
    lbl_801D5BF0[nPlayer].f0 = 0.01f;
    lbl_801D5BF0[nPlayer].f4 = 0.0f;
    lbl_801D5BF0[nPlayer].f8 = 0.04f;
    lbl_801D5BF0[nPlayer].fC = 0.5f;
    lbl_801D5BF0[nPlayer].n10 = 0;
    lbl_801D5BF0[nPlayer].f14 = -0.6f;
    lbl_801D5BF0[nPlayer].f18 = 0.3f;
    lbl_801D5BF0[nPlayer].f1C = 0.5f;
    lbl_801D5BF0[nPlayer].f20 = 0.2f;
    lbl_801D5BF0[nPlayer].f24 = 0.2f;
}

// The hole's chunk 3 loader: the placement outline. Each node is listed in lbl_801D5CCC, and any
// of its links that names the node itself is cleared.
void fn_8006A7A8(u8* pChunk) {
    TNetwork* pNet = (TNetwork*)pChunk;
    int i;
    int j;
    TNetNode* pNode;

    lbl_80281E30 = pNet;
    pNode = pNet->aNodes;
    lbl_80281E44 = 0;
    for (i = 0; i < pNet->nNumNodes; i++) {
        lbl_801D5CCC[i].pNode = pNode;
        for (j = 0; j < 10; j++) {
            if (pNode->aLinks[j] == i) {
                pNode->aLinks[j] = -1;
            }
        }
        lbl_80281E44++;
        pNode++;
    }
}

void fn_8006A89C(void) {
    lbl_80281E30 = NULL;
}

void fn_8006A8B0(void) {
    fn_800E58B4(32);
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_8006A964(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8006A964(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_8006A988(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8006A988(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// ---- sweep code (not yet cleaned up) ----

f32 fn_8006A8A8(u8* p);

f32 fn_8006A8A8(u8* p) {
    return *(f32*)(p + 0x0);
}

// ---- end of sweep code ----

// A point given as fractions of the camera's view (0..1) into the view's screen rectangle.
void fn_8006A8D4(void* pCamera, f32* pX, f32* pY) {
    f32* pRect = fn_80012EF0(pCamera);

    *pX = fn_80012EE8(pRect) + *pX * fn_80012ED8(pRect);
    *pY = fn_80012EE0(pRect) + *pY * fn_80012ED0(pRect);
}

// Draw text in one colour (pColor: RGBA).
void fn_8006A9AC(f32* pColor) {
    UFont_GetContext()->nA4 = 0x12;
    UFont_PackColor(pColor, (u8*)&UFont_GetContext()->u5C);
}

// The current font's line height, scaled as the text is drawn.
f32 fn_8006A9FC(void) {
    return fn_8006A8A8((u8*)fn_8006AA3C()) * UFont_GetContext()->f80;
}

// The font the current text settings draw with.
LLFont* fn_8006AA3C(void) {
    return lbl_80280DE0->apFonts[UFont_GetContext()->nFont];
}
