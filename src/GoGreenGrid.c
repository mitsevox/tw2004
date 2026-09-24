// GoGreenGrid.c (EA's name, from its asserts; TW06): the grid drawn over the putting green. It is
// laid out from the target back towards the ball, the ground height under each point is sampled a
// few points a frame, and the lines are built into a mesh per view.

#include "game_types.h"
#include "game.h"
#include "golfer.h"
#include "ball.h"
#include "camera.h"
#include "physics.h"
#include "greengrid.h"

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);

void fn_8009CB78(f32* pA, f32* pB, f32* pOut);
u8   fn_8009BD24(int nPlayer);
u8   fn_8009BD94(int nPlayer);

GreenGrid lbl_801E3068;
GreenGrid* lbl_802813C0 = &lbl_801E3068;
TexBank*  lbl_80281FAC;         // the "gridpt" texture's bank
TexEntry* lbl_80281FA8;         // and the texture

void GR_vInit(void) {
    s32 desc[2];
    int i;
    int nViews;
    int nRows;
    if (gSession.nSplitScreen) {
        nViews = 2;
        desc[0] = 0x80;
        nRows = 8;
    } else {
        nViews = 1;
        desc[0] = 0x100;
        nRows = 16;
    }
    desc[1] = 1;
    for (i = 0; i < nViews; i++) {
        fn_80036054(lbl_802813C0->aMesh[i], 0x13, desc);
        lbl_802813C0->apVert[i] = fn_80009B34(desc[0] * 12, 2, 16, "GoGreenGrid.c", 94);
        lbl_802813C0->apUV[i] = fn_80009B34(desc[0] * 8, 2, 16, "GoGreenGrid.c", 99);
        lbl_802813C0->apColor[i] = fn_80009B34(desc[0] * 4, 2, 16, "GoGreenGrid.c", 104);
        lbl_802813C0->apIndex[i] = fn_80009B34(desc[0] * 2, 2, 16, "GoGreenGrid.c", 109);
        lbl_802813C0->apHeight[i] = fn_80009B34(nRows * 16, 2, 16, "GoGreenGrid.c", 114);
        lbl_802813C0->aTarget[i][0] = 0.0f;
        lbl_802813C0->aTarget[i][1] = 0.0f;
        lbl_802813C0->aTarget[i][2] = 0.0f;
        lbl_802813C0->aTarget[i][3] = 1.0f;
        lbl_802813C0->anRows[i] = nRows;
    }
    lbl_802813C0->fCellW = 1.0f;
    lbl_802813C0->fCellD = 1.0f;
    lbl_802813C0->fFC = 1.0f;
    lbl_802813C0->f100 = 0.125f;
    lbl_802813C0->b104 = 1;
    lbl_802813C0->anColor[0] = 0x80;
    lbl_802813C0->anColor[1] = 0x80;
    lbl_802813C0->anColor[2] = 0x40;
    lbl_802813C0->anColor[3] = 0x41;
    lbl_802813C0->nCols = 4;
    fn_800102DC(fn_8000BEE4("gridpt"), &lbl_80281FAC, &lbl_80281FA8);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8009B894(void);

void fn_8009B894(void) {
}

// ---- end of sweep code ----

void fn_8009B898(void) {
    int nViews = gSession.nSplitScreen ? 2 : 1;
    int i;
    for (i = 0; i < nViews; i++) {
        fn_800360A0(lbl_802813C0->aMesh[i]);
        fn_80009E70(lbl_802813C0->apVert[i]);
        fn_80009E70(lbl_802813C0->apUV[i]);
        fn_80009E70(lbl_802813C0->apColor[i]);
        fn_80009E70(lbl_802813C0->apIndex[i]);
        fn_80009E70(lbl_802813C0->apHeight[i]);
    }
}

// Whether the grid shows for the player: never with fn_800E39F0; with the putter when
// options.b84 is set; otherwise when the target is on the kinds of ground fn_8009BD94 lists.
u8 fn_8009BD24(int nPlayer) {
    if (fn_800E39F0()) {
        return 0;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        return gSession.options.b84;
    }
    return fn_8009BD94(nPlayer);
}

u8 fn_8009BD94(int nPlayer) {
    int bOn = 0;
    int nSurface;
    nSurface = gPlayers[nPlayer].nSurface;
    // EA bug: only the first test checks for no surface (-1); the others read the row before
    // gSurfaceTypes.
    if ((nSurface >= 0 && gSurfaceTypes[nSurface].nClass == 3) || gSurfaceTypes[nSurface].nClass == 4
        || gSurfaceTypes[nSurface].nClass == 12 || gSurfaceTypes[nSurface].nClass == 18) {
        bOn = 1;
    }
    return bOn;
}

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_8009CB78(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8009CB78(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif
