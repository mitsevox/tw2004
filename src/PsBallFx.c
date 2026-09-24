// PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the ball's
// particle effects, one emitter per view (PsBallFxState in include/psmgr.h). Mostly not yet
// decompiled.

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
void fn_800A34C0(int n, Ball* pBall, f32* pDir);          // not yet decompiled

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
    lbl_80281408->p2C = fn_80009B34(0x640, 2, 16, "PsBallFx.c", 1435);
    lbl_80281408->p30 = fn_80009B34(0x320, 2, 16, "PsBallFx.c", 1440);
    lbl_80281408->p28 = fn_80009B34(0x960, 2, 16, "PsBallFx.c", 1445);
    lbl_80281408->p50 = fn_80009B34(0x4B0, 2, 16, "PsBallFx.c", 1450);
    for (i = 0; i < 50; i++) {
        lbl_80281408->p2C[i * 8 + 0] = fZero;
        lbl_80281408->p2C[i * 8 + 1] = fZero;
        lbl_80281408->p2C[i * 8 + 2] = fOne;
        lbl_80281408->p2C[i * 8 + 3] = fZero;
        lbl_80281408->p2C[i * 8 + 4] = fZero;
        lbl_80281408->p2C[i * 8 + 5] = fOne;
        lbl_80281408->p2C[i * 8 + 6] = fOne;
        lbl_80281408->p2C[i * 8 + 7] = fOne;
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
    fn_80009E70(lbl_80281408->p28);
    fn_80009E70(lbl_80281408->p2C);
    fn_80009E70(lbl_80281408->p30);
    fn_80009E70(lbl_80281408->p50);
}

// Clear the effects' state and give emitters 0, 6 and 14 the current course's vectors.
void fn_800A2E68(void) {
    int i;

    lbl_80281408->n3C = 0;
    lbl_80281408->n40 = 0;
    lbl_80281408->n34 = 0;
    lbl_80281408->n38 = 0;
    lbl_80281408->n44 = 0;
    for (i = 0; i < 8; i++) {
        lbl_80281408->a54[i] = 0.0f;
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
                pDef->f40 = -fn_8000AD78(-gPlayers[nPlayer].ball.vVel[0], gPlayers[nPlayer].ball.vVel[2]);
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

// The swing effect of the surface under the ball (unless the club is 25), and on some lies and
// surfaces (a club above 8, shot kind 1 or 4) the player's 'TEO ' objects at the ball.
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
    if (lbl_80281408->n44 > 2) {
        fn_8005CC64(lbl_80281408->pBank, lbl_80281408->pTex);
        fn_80012EF8();
        nFirst = lbl_80281408->n40;
        nEnd = lbl_80281408->n38;
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
        fill.nVerts = lbl_80281408->n44;
        fill.pIndices = lbl_80281408->p50;
        fill.pPos = lbl_80281408->p28;
        fill.pColour = lbl_80281408->p30;
        fill.pTexCoord = lbl_80281408->p2C;
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
