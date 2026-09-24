// PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the ball's
// particle effects, one emitter per view, and the sand trail (PsBallFxState in include/psmgr.h).

#include "psmgr.h"
#include "game.h"
#include "golfer.h"
#include "dynobj.h"
#include "camera.h"
#include "terrain.h"

void fn_80036054(ShaderObject* pObj, int nRow, const void* pDesc);  // Skin.c
void fn_800360A0(ShaderObject* pObj);                               // Skin.c
void fn_800360D4(ShaderObject* pObj);                               // Skin.c
void fn_800352BC(void);
void PsBallFx_TriggerTrail(Ball* pBall, int nPlayer);   // below; Ball.c declares it too
void fn_800A34C0(int nTrail, Ball* pBall, f32* pDir);     // below

// Set up the mesh and its buffers (50 quads; the second buffer gets each quad's texture corners),
// clear the emitters and find the "sandtrl" texture.
void PsBallFx_InitModule(void) {
    DynRenderSize size;
    int i;
    f32 fZero = 0.0f;
    f32 fOne = 1.0f;
    u64 uHash;

    size.nMaxVerts = 400;
    size.nMaxDraws = 2;
    fn_80036054(&lbl_80281408->mesh, 0, &size);
    lbl_80281408->ap2C[0] = fn_80009B34(0x640, 2, 16, "PsBallFx.c", 1435);
    lbl_80281408->ap30[0] = fn_80009B34(0x320, 2, 16, "PsBallFx.c", 1440);
    lbl_80281408->ap28[0] = fn_80009B34(0x960, 2, 16, "PsBallFx.c", 1445);
    lbl_80281408->ap50[0] = fn_80009B34(0x4B0, 2, 16, "PsBallFx.c", 1450);
    for (i = 0; i < 50; i++) {
        lbl_80281408->ap2C[0][i * 8 + 0] = fZero;
        lbl_80281408->ap2C[0][i * 8 + 1] = fZero;
        lbl_80281408->ap2C[0][i * 8 + 2] = fOne;
        lbl_80281408->ap2C[0][i * 8 + 3] = fZero;
        lbl_80281408->ap2C[0][i * 8 + 4] = fZero;
        lbl_80281408->ap2C[0][i * 8 + 5] = fOne;
        lbl_80281408->ap2C[0][i * 8 + 6] = fOne;
        lbl_80281408->ap2C[0][i * 8 + 7] = fOne;
    }
    lbl_80281408->ap74[0] = NULL;
    lbl_80281408->ap74[1] = NULL;
    lbl_80281408->apEmitter[0] = NULL;
    lbl_80281408->apEmitter[1] = NULL;
    uHash = fn_8000BEE4("sandtrl");
    fn_800102DC(uHash, &lbl_80281408->pBank, &lbl_80281408->pTex);
}

void fn_800A2E14(void) {
    fn_800360A0(&lbl_80281408->mesh);
    fn_80009E70(lbl_80281408->ap28[0]);
    fn_80009E70(lbl_80281408->ap2C[0]);
    fn_80009E70(lbl_80281408->ap30[0]);
    fn_80009E70(lbl_80281408->ap50[0]);
}

// Clear the sand trail's state and give emitters 0, 6 and 14 the current course's vectors (on
// course 18, lbl_8018E958's when fn_80015464 is 0, 1, 2 or 17).
void fn_800A2E68(void) {
    int i;

    lbl_80281408->an3C[0] = 0;
    lbl_80281408->an40[0] = 0;
    lbl_80281408->an34[0] = 0;
    lbl_80281408->an38[0] = 0;
    lbl_80281408->an44[0] = 0;
    for (i = 0; i < 4; i++) {
        lbl_80281408->a54[0][i] = 0.0f;
    }
    for (i = 0; i < 4; i++) {
        lbl_80281408->a64[0][i] = 0.0f;
    }
    if (Game_GetCourse() == 18 && (fn_80015464() == 0 || fn_80015464() == 1 || fn_80015464() == 2 ||
                                   fn_80015464() == 17)) {
        Vec_Copy(lbl_8018E958[0], lbl_8018CA98[0].vF0);
        Vec_Copy(lbl_8018E958[1], lbl_8018CA98[14].vF0);
        Vec_Copy(lbl_8018E958[1], lbl_8018CA98[6].vF0);
    } else {
        Vec_Copy(lbl_8018E6B8[Game_GetCourse()][0], lbl_8018CA98[0].vF0);
        Vec_Copy(lbl_8018E6B8[Game_GetCourse()][1], lbl_8018CA98[14].vF0);
        Vec_Copy(lbl_8018E6B8[Game_GetCourse()][1], lbl_8018CA98[6].vF0);
    }
}

// Start the effects of emitters 9 and 10 at nPlayer's ball (only with a club up to 8).
void fn_800A2FFC(int nPlayer, int bOn) {
    f32 vPos[4];
    ParticleParams* pDef;
    PsEmitter* pEmitter;
    int nView;

    if (!bOn || gPlayers[nPlayer].nClub > 8) {
        return;
    }
    Vec_Copy(gPlayers[nPlayer].ball.vPos, vPos);
    Vec3Copy(vPos, lbl_8018CA98[9].v80);
    pEmitter = fn_80099758(&lbl_8018CA98[9]);
    if (pEmitter != NULL) {
        fn_80099EA4(pEmitter);
    }
    pDef = &lbl_8018CA98[10];
    Vec3Copy(vPos, pDef->v80);
    nView = gPlayers[nPlayer].nView[0];
    lbl_80281408->ap74[nView] = fn_80099758(pDef);
    if (lbl_80281408->ap74[nView] != NULL) {
        fn_80099EA4(lbl_80281408->ap74[nView]);
    }
}

void fn_800A30E4(int nKind, Ball* pBall, int nPlayer, u8 bFlight, f32 fValue) {
    ParticleParams* pDef;
    int i;
    PsEmitter* pEmitter;

    for (i = 0; i < 5; i++) {
        if (lbl_8018C868[nKind][i].nDef != -1 && fValue >= lbl_8018C868[nKind][i].fMin) {
            pDef = &lbl_8018CA98[lbl_8018C868[nKind][i].nDef];
            Vec3Copy(pBall->vPos, pDef->v80);
            if (bFlight) {
                pDef->f40 = -atan2f(-gPlayers[nPlayer].ball.vVel[0], gPlayers[nPlayer].ball.vVel[2]);
            } else {
                pDef->f40 = -gPlayers[nPlayer].fAim;
            }
            pEmitter = fn_80099758(pDef);
            if (pEmitter != NULL) {
                fn_80099EA4(pEmitter);
            }
        }
    }
}

// Nothing for club 25 or on ground a ball can't stop on (f1C not 0.375). Otherwise the swing
// effect of the surface under the ball, on some lies and surfaces (a club above 8, shot kind 1
// or 4) the player's 'TEO ' objects at the ball, and fn_8004816C on lie 0.
void fn_800A31E0(Ball* pBall, int nPlayer) {
    f32 vPos[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    int nLie;

    if (gPlayers[nPlayer].nClub == 25) {
        return;
    }
    vPos[0] = pBall->vPos[0];
    vPos[1] = pBall->vPos[1];
    vPos[2] = pBall->vPos[2];
    vPos[3] = 1.0f;
    if (TER_NO_GROUND != Ter_GetSupportingGroundData(fn_8000C594(), vPos, &pSurface, vNormal)) {
        if (0.375f != pSurface->f1C) {
            return;
        }
        fn_800A30E4(pSurface->nSwingEffectId, pBall, nPlayer, 0, 0.0f);
    }
    nLie = gPlayers[nPlayer].ball.nLie;
    if ((nLie == 1 || (u32)(nLie - 2) <= 2 || nLie == 0) && pSurface != NULL &&
        (pSurface->nClass == 1 || pSurface->nClass == 2 || pSurface->nClass == 5 ||
         pSurface->nClass == 11) &&
        gPlayers[nPlayer].nClub > 8 &&
        (gPlayers[nPlayer].nShotKind == 1 || gPlayers[nPlayer].nShotKind == 4)) {
        fn_80046E1C(pBall->vPos, nPlayer);
        fn_80047A24(pBall->vPos, nPlayer);
    }
    if (gPlayers[nPlayer].ball.nLie == 0) {
        fn_8004816C(nPlayer);
    }
}

// The ball hit a surface: if a ball may stop on it, the surface's collision effect and the game
// mode's, with the ball's speed into the ground as the value, and the trail; then fn_80046C34
// (skipped only for a surface a ball can't stop on).
void fn_800A3348(Ball* pBall, int nPlayer) {
    f32 vPos[4];
    f32 vNormal[4];
    SurfaceType* pGround;
    SurfaceType* pSurface;
    f32 fDot;

    vPos[0] = pBall->vPos[0];
    pSurface = gPlayers[nPlayer].ball.pHitSurface;
    vPos[1] = pBall->vPos[1];
    vPos[2] = pBall->vPos[2];
    vPos[3] = 1.0f;
    if (pSurface != NULL) {
        if (0.375f != pSurface->f1C) {
            return;
        }
        if (pSurface->nClass == 6) {
            gPlayers[nPlayer].b30F = 1;
        }
        if (pSurface->nClass == 7 || pSurface->nClass == 16) {
            gPlayers[nPlayer].b30E = 1;
        }
        fn_8004DBB0(fn_8000C594(), vPos, &pGround, vNormal);
        fDot = fn_8000C5FC(gPlayers[nPlayer].ball.vVel, vNormal);
        if (fDot < 0.0f) {
            fDot *= -1.0f;
        }
        fn_800A30E4(pSurface->nCollisionEffectId, pBall, nPlayer, 1, fDot);
        fn_800A30E4(gpGame->pfn240(nPlayer), pBall, nPlayer, 1, fDot);
        PsBallFx_TriggerTrail(pBall, nPlayer);
    }
    fn_80046C34(pBall->vPos, nPlayer);
}

// A trail behind a ball moving over sand (surface class 6), along its flat direction; not in
// split screen.
void PsBallFx_TriggerTrail(Ball* pBall, int nPlayer) {
    f32 vPos[4];
    f32 vNormal[4];
    f32 vDir[4];
    SurfaceType* pSurface;

    if (gSession.nSplitScreen) {
        return;
    }
    vPos[0] = pBall->vPos[0];
    vPos[1] = pBall->vPos[1];
    vPos[2] = pBall->vPos[2];
    vPos[3] = 1.0f;
    if (TER_NO_GROUND == Ter_GetSupportingGroundData(fn_8000C594(), vPos, &pSurface, vNormal) ||
        0.375f != pSurface->f1C || (int)pSurface->nClass != 6) {    // EA compares the class signed here
        return;
    }
    Vec_Copy(gPlayers[nPlayer].ball.vVel, vDir);
    vDir[1] = 0.0f;
    if (vDir[0] != 0.0f || vDir[2] != 0.0f) {
        fn_800BAF04(vDir, vDir);
        fn_800A34C0(0, pBall, vDir);
    }
}

// Extend sand trail nTrail to the ball once it has moved on (squared distance 0.001): a pair of
// vertices 0.01 either side of the ball across pDir, in a ring of 200, and a strip of indices to them
// in a ring of 600. A pair far from the last one (squared 0.1) or a turn of more than 90 degrees first
// gets two indices that break the strip. Once the ring is full, the indices that used the pair being
// overwritten are dropped from the front.
void fn_800A34C0(int nTrail, Ball* pBall, f32* pDir) {
    s32 nVert;
    s32 nIndex;
    s32 nCur;
    s32 nPrev;
    s32 nUsed;
    s32 nOldest;
    f32 fGap;
    f32 fDot;

    nVert = lbl_80281408->an34[nTrail];
    nIndex = lbl_80281408->an38[nTrail];
    nUsed = lbl_80281408->an44[nTrail];
    if (fn_800BB028(pBall->vPos, lbl_80281408->a54[nTrail]) < 0.001f) {
        return;
    }
    Vec3Copy(pBall->vPos, lbl_80281408->a54[nTrail]);
    if (nVert + 2 <= 200) {
        nCur = nVert;
        nPrev = (nVert != 0) ? nVert - 2 : 198;
        nVert += 2;
    } else {
        nCur = 0;
        nPrev = 198;
        nVert = 2;
    }
    lbl_80281408->ap28[nTrail][nCur * 3 + 0] = pBall->vPos[0] - 0.01f * pDir[2];
    lbl_80281408->ap28[nTrail][nCur * 3 + 1] = pBall->vPos[1];
    lbl_80281408->ap28[nTrail][nCur * 3 + 2] = pBall->vPos[2] + 0.01f * pDir[0];
    lbl_80281408->ap28[nTrail][nCur * 3 + 3] = pBall->vPos[0] + 0.01f * pDir[2];
    lbl_80281408->ap28[nTrail][nCur * 3 + 4] = pBall->vPos[1];
    lbl_80281408->ap28[nTrail][nCur * 3 + 5] = pBall->vPos[2] - 0.01f * pDir[0];
    lbl_80281408->ap30[nTrail][nCur * 4 + 0] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 1] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 2] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 3] = 0;
    lbl_80281408->ap30[nTrail][nCur * 4 + 4] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 5] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 6] = 5;
    lbl_80281408->ap30[nTrail][nCur * 4 + 7] = 0;
    if (nUsed >= 2) {
        fGap = fn_800BB028(&lbl_80281408->ap28[nTrail][nCur * 3], &lbl_80281408->ap28[nTrail][nPrev * 3]);
    } else {
        fGap = 0.0f;
    }
    if (fGap <= 0.1f) {
        fDot = fn_8000C5FC(pDir, lbl_80281408->a64[nTrail]);
    } else {
        fDot = 1.0f;
    }
    Vec3Copy(pDir, lbl_80281408->a64[nTrail]);
    if (fGap > 0.1f || fDot < 0.0f) {
        if (nIndex + 2 <= 600) {
            lbl_80281408->ap50[nTrail][nIndex] = nPrev + 1;
            lbl_80281408->ap50[nTrail][nIndex + 1] = nCur;
            nIndex += 2;
        } else {
            nIndex = 4;
            lbl_80281408->ap50[nTrail][0] = lbl_80281408->ap50[nTrail][598];
            lbl_80281408->ap50[nTrail][1] = lbl_80281408->ap50[nTrail][599];
            lbl_80281408->ap50[nTrail][2] = lbl_80281408->ap50[nTrail][599];
            lbl_80281408->ap50[nTrail][3] = nCur;
        }
    }
    if (nIndex + 2 <= 600) {
        lbl_80281408->ap50[nTrail][nIndex] = nCur;
        lbl_80281408->ap50[nTrail][nIndex + 1] = nCur + 1;
        nIndex += 2;
    } else {
        nIndex = 2;     // EA's count: four indices were written
        lbl_80281408->ap50[nTrail][0] = lbl_80281408->ap50[nTrail][598];
        lbl_80281408->ap50[nTrail][1] = lbl_80281408->ap50[nTrail][599];
        lbl_80281408->ap50[nTrail][2] = nCur;
        lbl_80281408->ap50[nTrail][3] = nCur + 1;
    }
    lbl_80281408->an38[nTrail] = nIndex;
    lbl_80281408->an34[nTrail] = nVert;
    lbl_80281408->an44[nTrail] += 2;
    if (lbl_80281408->an44[nTrail] > 200) {
        lbl_80281408->an44[nTrail] = 200;
        nOldest = lbl_80281408->an3C[nTrail];
        lbl_80281408->an3C[nTrail] = nOldest + 2;
        if (lbl_80281408->an3C[nTrail] >= 200) {
            lbl_80281408->an3C[nTrail] = 0;
        }
        while (nOldest == lbl_80281408->ap50[nTrail][lbl_80281408->an40[nTrail]] ||
               nOldest == lbl_80281408->ap50[nTrail][lbl_80281408->an40[nTrail] + 1] ||
               nOldest + 1 == lbl_80281408->ap50[nTrail][lbl_80281408->an40[nTrail]] ||
               nOldest + 1 == lbl_80281408->ap50[nTrail][lbl_80281408->an40[nTrail] + 1]) {
            lbl_80281408->an40[nTrail] += 2;
            if (lbl_80281408->an40[nTrail] >= 600) {
                lbl_80281408->an40[nTrail] = 0;
            }
        }
    }
}

// Keep each view's emitter from fn_800A2FFC at the ball of the player that view follows, then
// draw the sand trail: the indices from n40 up to n38 in the ring of 600 (two draws when they
// wrap), once it has more than two vertices.
void fn_800A3A84(void) {
    DynRenderDrawIn aDraws[2];
    DynRenderFill fill;
    PsEmitter* pEmitter;
    s32 nFirst;
    s32 nEnd;

    pEmitter = lbl_80281408->ap74[0];
    if (pEmitter != NULL && (pEmitter->params.u58 & 0x20000)) {
        Vec_Copy(gPlayers[fn_8001707C(0)].ball.vPos, pEmitter->params.v80);
        Vec_Copy(gPlayers[fn_8001707C(0)].ball.vPos, lbl_80281408->ap74[0]->mtx[3]);
    }
    pEmitter = lbl_80281408->ap74[1];
    if (pEmitter != NULL && (pEmitter->params.u58 & 0x20000)) {
        Vec_Copy(gPlayers[fn_8001707C(1)].ball.vPos, pEmitter->params.v80);
        Vec_Copy(gPlayers[fn_8001707C(1)].ball.vPos, lbl_80281408->ap74[1]->mtx[3]);
    }
    fn_80014118(0x70);
    fn_800352BC();
    fn_80013CCC(fn_8001614C());
    fn_80013EEC(fn_8001614C());
    fn_80035240(0);
    fn_80016B9C();
    fn_80035138(0);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(3);
    fn_80035118(1, 1);
    fn_80012EF8();
    if (lbl_80281408->an44[0] > 2) {
        fn_8005CC64(lbl_80281408->pBank, lbl_80281408->pTex);
        fn_80012EF8();
        nFirst = lbl_80281408->an40[0];
        nEnd = lbl_80281408->an38[0];
        if (nFirst < nEnd) {
            fill.nCount = 1;
            aDraws[0].nPrim = 0;
            aDraws[0].nStart = nFirst;
            aDraws[0].nCount = nEnd - nFirst;
        } else {
            aDraws[0].nPrim = 0;
            fill.nCount = 2;
            aDraws[0].nStart = nFirst;
            aDraws[0].nCount = 600 - nFirst;
            aDraws[1].nPrim = 0;
            aDraws[1].nStart = 0;
            aDraws[1].nCount = nEnd;
        }
        fill.pDraws = aDraws;
        fill.nVerts = lbl_80281408->an44[0];
        fill.pIndices = lbl_80281408->ap50[0];
        fill.pPos = lbl_80281408->ap28[0];
        fill.pColour = lbl_80281408->ap30[0];
        fill.pTexCoord = lbl_80281408->ap2C[0];
        fn_80036100(&lbl_80281408->mesh, &fill, 1);
        fn_800360D4(&lbl_80281408->mesh);
    }
    fn_80012F50(1, 6, 0x80);
    fn_80012EF8();
}

// Start emitter 15 at pPos for nPlayer's view, drifting with a tenth of the wind.
void fn_800A3CB0(f32* pPos, int nPlayer) {
    f32 vWind[4];
    int nView = gPlayers[nPlayer].nView[0];

    Wind_Get(vWind);
    fn_8000AE28(vWind, 0.1f, vWind);
    Vec3Copy(pPos, lbl_8018CA98[15].v80);
    Vec3Copy(vWind, lbl_8018CA98[15].vA0);
    lbl_80281408->apEmitter[nView] = fn_80099758(&lbl_8018CA98[15]);
    if (lbl_80281408->apEmitter[nView] != NULL) {
        fn_80099EA4(lbl_80281408->apEmitter[nView]);
    }
}

// Move the emitter of nPlayer's view to pPos.
void fn_800A3D6C(f32* pPos, int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    if (lbl_80281408->apEmitter[nView] != NULL && (lbl_80281408->apEmitter[nView]->params.u58 & 0x20000)) {
        Vec_Copy(pPos, lbl_80281408->apEmitter[nView]->params.v80);
        Vec_Copy(pPos, lbl_80281408->apEmitter[nView]->mtx[3]);
    }
}

void fn_800A3DF4(int nPlayer) {
    PsEmitter* pEmitter = lbl_80281408->apEmitter[gPlayers[nPlayer].nView[0]];
    if (pEmitter != NULL && (pEmitter->params.u58 & 0x20000)) {
        pEmitter->n50 = 1000000;
    }
}
