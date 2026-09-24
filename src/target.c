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
    int  nClub;
    u8   bInRange;
    u8   bMoved;
    f32* pTarget;

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
        gPlayers[nPlayer].vTarget[0] = -fSin * gPlayers[nPlayer].fDistance + gPlayers[nPlayer].vBall[0];
        gPlayers[nPlayer].vTarget[2] = fCos * gPlayers[nPlayer].fDistance + gPlayers[nPlayer].vBall[2];
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
        gPlayers[nPlayer].vTarget[0] += fStep * -fSin;
        gPlayers[nPlayer].vTarget[2] += fStep * fCos;
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
            gPlayers[nPlayer].vTarget[0] = -fSin * gPlayers[nPlayer].fDistance + gPlayers[nPlayer].vBall[0];
            gPlayers[nPlayer].vTarget[2] = fCos * gPlayers[nPlayer].fDistance + gPlayers[nPlayer].vBall[2];
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
    fX -= 0.5f * fn_80012C30(sz);
    fY -= 0.5f * fn_8006A9FC();
    fn_800128F8(sz, fX, fY);
}

// fA88: the heading from the game's point (gpGame->p130) to the placement point, less a quarter
// turn; and the aim marker's usual settings.
void fn_8006A6C4(int nPlayer) {
    f32 vDir[4];

    fn_8006A988(gPlayers[nPlayer].vPlacement, gpGame->p130, vDir);
    vDir[1] = 0.0f;
    Vec_Normalize(vDir, vDir);
    gPlayers[nPlayer].fA88 = fn_8000AD78(vDir[2], vDir[0]) - PI / 2.0f;
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
    fn_80012EC4()->nA4 = 0x12;
    fn_80012E54(pColor, (u8*)&fn_80012EC4()->u5C);
}

// The current font's line height, scaled as the text is drawn.
f32 fn_8006A9FC(void) {
    return fn_8006A8A8((u8*)fn_8006AA3C()) * fn_80012EC4()->f80;
}

// The font the current text settings draw with.
LLFont* fn_8006AA3C(void) {
    return lbl_80280DE0->apFonts[fn_80012EC4()->nFont];
}
