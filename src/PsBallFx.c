// PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the ball's
// particle effects, one emitter per view (PsBallFxState in include/psmgr.h). Mostly not yet
// decompiled.

#include "psmgr.h"
#include "game.h"
#include "golfer.h"

void fn_800360A0(void* pMesh);     // Skin.c

void fn_800A2E14(void) {
    fn_800360A0(lbl_80281408->mesh);
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
    PsEmitterDef* pDef;
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
    PsEmitterDef* pDef;
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
    if (lbl_80281408->apEmitter[nView] != NULL && (lbl_80281408->apEmitter[nView]->uB8 & 0x20000)) {
        Vec_Copy(pPos, lbl_80281408->apEmitter[nView]->vE0);
        Vec_Copy(pPos, lbl_80281408->apEmitter[nView]->v30);
    }
}

void fn_800A3DF4(int nPlayer) {
    PsEmitter* pEmitter = lbl_80281408->apEmitter[gPlayers[nPlayer].nView[0]];
    if (pEmitter != NULL && (pEmitter->uB8 & 0x20000)) {
        pEmitter->n50 = 1000000;
    }
}
