// goballfx.c (TW06's golf/hi-rendering/goballfx.c; BFX_vInit is paired with TW06's): the scene's
// light sets and the ball marker (include/lighting.h). It keeps four sets of five lights (four point
// lights and a directional one), fills the current set from the hole's lights (fn_800935CC) or from
// a caller's settings (fn_80093854), and draws a small marker quad on the ground under the ball.

#include "lighting.h"
#include "camera.h"
#include "ball.h"

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);
void fn_800360D4(u8* pMesh);

// GoTerrainCollision.c
f32 fn_8004D80C(CourseInfo* pCourse, f32* pPos);

void BFX_vInit(void) {
    s32 desc[2];
    fn_800102DC(fn_8000BEE4("marker"), &lbl_80281F44, &lbl_80281F48);
    desc[0] = 0x10;
    desc[1] = 4;
    fn_80036054(lbl_801D94B0, 0, desc);
}

void fn_80093AD4(void) {
    lbl_80281F40 = 1;
}

// Draws the player's ball marker: a quad 0.04 across in the player's colour, just above the ground
// under the ball; nothing where there is no ground.
void BFX_vRender(Ball* pBall, int nPlayer) {
    s16 aIndex[4] = {0, 1, 2, 3};
    TrailMeshDesc desc;
    f32* pPos;
    f32 fGround;
    f32 fSize;
    fGround = fn_8004D80C(fn_8000C594(), pBall->vPos);
    if (-65536.125f != fGround) {
        fn_8005CC64(lbl_80281F44, lbl_80281F48);
        fn_80016B9C();
        fn_80035118(4, 5);
        // fake match: the row offset as a signed 64-bit product (&lbl_801D95C8[nPlayer]); its dead
        // high word joins the first scheduling pass and is gone after register allocation.
        // port: the product is truncated back to a 32-bit pointer offset.
        pPos = (f32*)((u8*)lbl_801D95C8 + (s64)nPlayer * 48);
        fSize = 0.02f;
        pPos[0] = pBall->vPos[0] - fSize;
        pPos[1] = 0.01f + fGround;
        pPos[2] = pBall->vPos[2] - fSize;
        pPos[3] = pBall->vPos[0] - fSize;
        pPos[4] = 0.01f + fGround;
        pPos[5] = pBall->vPos[2] + fSize;
        pPos[6] = pBall->vPos[0] + fSize;
        pPos[7] = 0.01f + fGround;
        pPos[8] = pBall->vPos[2] - fSize;
        pPos[9] = pBall->vPos[0] + fSize;
        pPos[10] = 0.01f + fGround;
        pPos[11] = pBall->vPos[2] + fSize;
        lbl_801D9578[nPlayer][0] = lbl_80189CB0[nPlayer][0];
        lbl_801D9578[nPlayer][1] = lbl_80189CB0[nPlayer][1];
        lbl_801D9578[nPlayer][2] = lbl_80189CB0[nPlayer][2];
        lbl_801D9578[nPlayer][3] = lbl_80189CB0[nPlayer][3];
        lbl_801D9578[nPlayer][4] = lbl_80189CB0[nPlayer][0];
        lbl_801D9578[nPlayer][5] = lbl_80189CB0[nPlayer][1];
        lbl_801D9578[nPlayer][6] = lbl_80189CB0[nPlayer][2];
        lbl_801D9578[nPlayer][7] = lbl_80189CB0[nPlayer][3];
        lbl_801D9578[nPlayer][8] = lbl_80189CB0[nPlayer][0];
        lbl_801D9578[nPlayer][9] = lbl_80189CB0[nPlayer][1];
        lbl_801D9578[nPlayer][10] = lbl_80189CB0[nPlayer][2];
        lbl_801D9578[nPlayer][11] = lbl_80189CB0[nPlayer][3];
        lbl_801D9578[nPlayer][12] = lbl_80189CB0[nPlayer][0];
        lbl_801D9578[nPlayer][13] = lbl_80189CB0[nPlayer][1];
        lbl_801D9578[nPlayer][14] = lbl_80189CB0[nPlayer][2];
        lbl_801D9578[nPlayer][15] = lbl_80189CB0[nPlayer][3];
        lbl_801D94D8[nPlayer][0] = 0.0f;
        lbl_801D94D8[nPlayer][1] = 0.0f;
        lbl_801D94D8[nPlayer][2] = 0.0f;
        lbl_801D94D8[nPlayer][3] = 1.0f;
        lbl_801D94D8[nPlayer][4] = 1.0f;
        lbl_801D94D8[nPlayer][5] = 0.0f;
        lbl_801D94D8[nPlayer][6] = 1.0f;
        lbl_801D94D8[nPlayer][7] = 1.0f;
        fn_80014118(0x70);
        fn_80035138(0);
        fn_80012EF8();
        desc.n0 = 4;
        desc.nVerts = 4;
        desc.pDraw = NULL;
        desc.pIndices = aIndex;
        desc.pPos = pPos;
        desc.pColour = lbl_801D9578[nPlayer];
        desc.pUV = lbl_801D94D8[nPlayer];
        fn_80036100((ShaderObject*)lbl_801D94B0, &desc, lbl_80281F40);
        fn_800360D4(lbl_801D94B0);
        lbl_80281F40 = 0;
    }
}

void fn_80093D14(void) {
    fn_800360A0(lbl_801D94B0);
}
