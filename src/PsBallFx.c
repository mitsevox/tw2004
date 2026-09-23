// PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the ball's
// particle effects, one emitter per view (PsBallFxState in include/psmgr.h). Mostly not yet
// decompiled.

#include "psmgr.h"
#include "golfer.h"

void fn_800360A0(void* pMesh);     // Skin.c

void fn_800A2E14(void) {
    fn_800360A0(lbl_80281408->mesh);
    fn_80009E70(lbl_80281408->p28);
    fn_80009E70(lbl_80281408->p2C);
    fn_80009E70(lbl_80281408->p30);
    fn_80009E70(lbl_80281408->p50);
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
