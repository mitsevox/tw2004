// GoObjShadow.c (our name): shadows drawn with the "shadow" texture into five mesh objects
// (lbl_801D96B8); fn_80093DB8 is called from GoDynObj.c. Its data starts on its own 8-byte
// boundaries (.sdata 0x80281390, .sdata2 0x80283C68).

#include "engine.h"
#include "golfer.h"
#include "ball.h"
#include "camera.h"
#include "terrain.h"

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);
void fn_800360D4(u8* pMesh);

// GoTerrainCollision.c
f32 fn_8004D80C(CourseInfo* pCourse, f32* pPos);

#define NUM_SHADOW_MESHES 5

// The uninitialised globals are defined last-first: CodeWarrior lays them out in reverse.
f32 lbl_801D97B0[12];           // the shadow quad's four corners
u8  lbl_801D97A0[4][4];         // its vertex colours
f32 lbl_801D9780[8];            // its texture coordinates
u8 lbl_801D96B8[NUM_SHADOW_MESHES][0x28];  // mesh objects, as Swing.c keeps its trail meshes
s32 lbl_80281390 = 4;           // vertices to colour
TexEntry* lbl_80281F54;         // the "shadow" texture
TexBank*  lbl_80281F50;         // and its bank

void fn_80093D3C(void) {
    s32 desc[2];
    int i;
    fn_800102DC(fn_8000BEE4("shadow"), &lbl_80281F50, &lbl_80281F54);
    desc[0] = 4;
    desc[1] = 1;
    for (i = 0; i < NUM_SHADOW_MESHES; i++) {
        fn_80036054(lbl_801D96B8[i], 0, desc);
    }
}

// Defined after fn_80093D3C: its "shadow" string sits before them in .sdata.
f32 lbl_8028139C = 0.07f;       // half the quad's size
f32 lbl_802813A0 = 0.01f;       // how far the shadow is pushed from the light point

// Draws the player's ball shadow: a grey quad 0.14 across on the ground under the ball, pushed
// 0.01 away from the session's light point (f5B3C, f5B44); nothing where there is no ground.
void fn_80093DB8(Ball* pBall, int nPlayer) {
    s16 aIndex[4] = {0, 1, 2, 3};
    f32 vAway[4];
    TrailMeshDesc desc;
    f32 fGround;
    int i;
    fGround = fn_8004D80C(fn_8000C594(), pBall->vPos);
    if (-65536.125f != fGround) {
        fn_8005CC64(lbl_80281F50, lbl_80281F54);
        fn_80012F34(0);
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(3);
        fn_80035118(4, 5);
        fn_80016B9C();
        fn_80014118(0x70);
        fn_80035138(0);
        fn_80012EF8();
        vAway[0] = pBall->vPos[0] - gSession.f5B3C;
        vAway[1] = 0.0f;
        vAway[2] = pBall->vPos[2] - gSession.f5B44;
        vAway[3] = 1.0f;
        fn_800BAF04(vAway, vAway);
        fn_8001EF34(vAway, lbl_802813A0, vAway);
        lbl_801D97B0[0] = vAway[0] + (pBall->vPos[0] - lbl_8028139C);
        lbl_801D97B0[1] = 0.005f + fGround;
        lbl_801D97B0[2] = vAway[2] + (pBall->vPos[2] - lbl_8028139C);
        lbl_801D97B0[3] = vAway[0] + (pBall->vPos[0] - lbl_8028139C);
        lbl_801D97B0[4] = 0.005f + fGround;
        lbl_801D97B0[5] = vAway[2] + (pBall->vPos[2] + lbl_8028139C);
        lbl_801D97B0[6] = vAway[0] + (pBall->vPos[0] + lbl_8028139C);
        lbl_801D97B0[7] = 0.005f + fGround;
        lbl_801D97B0[8] = vAway[2] + (pBall->vPos[2] - lbl_8028139C);
        lbl_801D97B0[9] = vAway[0] + (pBall->vPos[0] + lbl_8028139C);
        lbl_801D97B0[10] = 0.005f + fGround;
        lbl_801D97B0[11] = vAway[2] + (pBall->vPos[2] + lbl_8028139C);
        for (i = 0; lbl_80281390 > i; i++) {
            lbl_801D97A0[i][0] = 0x80;
            lbl_801D97A0[i][1] = 0x80;
            lbl_801D97A0[i][2] = 0x80;
            lbl_801D97A0[i][3] = 0x80;
        }
        lbl_801D9780[0] = 0.0f;
        lbl_801D9780[1] = 0.0f;
        lbl_801D9780[2] = 0.0f;
        lbl_801D9780[3] = 1.0f;
        lbl_801D9780[4] = 1.0f;
        lbl_801D9780[5] = 0.0f;
        lbl_801D9780[6] = 1.0f;
        lbl_801D9780[7] = 1.0f;
        desc.n0 = 4;
        desc.nVerts = 4;
        desc.pDraw = NULL;
        desc.pIndices = aIndex;
        desc.pPos = lbl_801D97B0;
        desc.pColour = lbl_801D97A0[0];
        desc.pUV = lbl_801D9780;
        fn_80036100((ShaderObject*)lbl_801D96B8[nPlayer], &desc, 1);
        fn_800360D4(lbl_801D96B8[nPlayer]);
        fn_80012F34(1);
        fn_80012F50(1, 6, 0x80);
        fn_80012F18(3);
        fn_80012EF8();
    }
}

void fn_800940FC(void) {
    int i;
    for (i = 0; i < NUM_SHADOW_MESHES; i++) {
        fn_800360A0(lbl_801D96B8[i]);
    }
}
