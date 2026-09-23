// GoGolfCam.c (EA's name, from its asserts): the golf cameras (TW06's GolfCamera_*). Each camera
// mode has an init, called by View_SetCamera, and a per-frame process, called by
// CameraController_Idle; both drive the view's camera script. The file runs 0x800BD894-0x800C7480;
// this unit is its middle, camera 7's process to camera 24's init.

#include "golfer.h"

// A camera shot (0xC0 bytes) as fn_8006509C returns them; the shots are chained through p40.
typedef struct CamShot {
    u8   unk0[0x40];
    struct CamShot* p40;        // 0x40
    u8   unk44[4];
    f32  f48;                   // 0x48  how long the shot lasts
    u8   unk4C[0xA4 - 0x4C];
    s32  nA4;                   // 0xA4
    u8   unkA8[3];
    u8   bAB;                   // 0xAB
    u8   unkAC[0xC0 - 0xAC];
} CamShot;

// The golf cameras' shared state.
typedef struct GolfCamState {
    u8      unk0[0x56];
    u8      b56;                // 0x56
    u8      unk57[0x5B - 0x57];
    u8      b5B;                // 0x5B  set by the shutter camera
    u8      unk5C[4];
    s32     n60;                // 0x60  passed to fn_8006509C with the state
    u8      unk64[4];
    f32     f68;                // 0x68
    CamShot shot6C;             // 0x06C
    CamShot shot12C;            // 0x12C
} GolfCamState;

// The camera tuning values.
typedef struct CamTuning {
    u8   unk0[0x178];
    f32  f178;                  // 0x178
    f32  v17C[4];               // 0x17C
} CamTuning;

// A view (one per split-screen half); only what this file touches. The fields Swing.c also uses
// keep its names.
typedef struct View {
    u8       unk0[0x84];
    u8       a84[0x40];         // 0x084  the camera script the camera functions drive
    f32      vC4[4];            // 0x0C4
    u8       unkD4[0x104 - 0xD4];
    f32      fCamTime;          // 0x104  time on this camera
    u8       unk108[0x110 - 0x108];
    f32      f110;              // 0x110
    f32      f114;              // 0x114
    f32      f118;              // 0x118
    u8       unk11C[0x124 - 0x11C];
    f32      f124;              // 0x124
    f32      f128;              // 0x128
    u8       unk12C[4];
    CamShot* p130;              // 0x130  the current shot
    CamShot* p134;              // 0x134  the next one
    u8       unk138[0x140 - 0x138];
    s32      n140;              // 0x140
    s32      nCamera;           // 0x144
    u8       unk148[0x153 - 0x148];
    u8       b153;              // 0x153
    s32      n154;              // 0x154
    u8       unk158[0x18C - 0x158];
    f32      f18C;              // 0x18C
    f32      f190;              // 0x190
    s32      n194;              // 0x194
    s32      n198;              // 0x198
    u8       a19C[0x260 - 0x19C];   // 0x19C
    s32      n260;              // 0x260
} View;

extern GolfCamState* lbl_80282220;
extern CamTuning*    lbl_80281F78;

void*    fn_8001731C(View* pView);
void*    fn_80017314(View* pView);
void     fn_8003DCE8(int nPlayer, void* pCam, void* pSub, void* pScript, void* p19C, int a, f32 fFrameTime);
void     fn_8003EA50(int nPlayer, void* pCam, void* pSub, void* pScript, void* p19C, int a, f32 fFrameTime);
void*    fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot);
void     CameraScript_InterpToNewScript(void* pScript, void* pNew, int nPlayer, void* pCam, void* pSub,
                                        int a, int b, f32 f1, f32 f2, f32 f3);
CamShot* fn_8006509C(s32 n, GolfCamState* pState);
void     fn_8006351C(View* pView, int nPlayer, int nCamera);
void     fn_800B3550(int a, View* pView, int nPlayer);
void     fn_800C5D64(View* pView, void* pCam, void* pSub, int nPlayer, GolfCamState* pState);
void     GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate);

// Camera 7 (the knee cam): only the script's per-frame update.
void fn_800C0804(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, pView->a19C, 0, gSession.fFrameTime);
}

// Camera 10: start on the first shot fn_8006509C gives.
void fn_800C0880(View* pView, int nPlayer) {
    CamShot* pShot;
    GolfCamState* pState;
    s32 n;
    pState = lbl_80282220;
    n = pState->n60;        // fake match: read before the store below, in the original's order
    pState->f68 = 0.0f;
    pShot = fn_8006509C(n, pState);
    if (pShot != NULL) {
        pView->p130 = pShot;
        pView->p134 = pShot->p40;
        pView->f110 = pView->p130->f48;
        pView->n140 = pView->p130->bAB;
        pView->fCamTime = 0.0f;
        pView->f128 = 0.0f;
        pView->f124 = 0.0f;
    } else {
        pView->p130 = NULL;
        pView->fCamTime = 0.0f;
    }
    pView->b153 = 0;
}

// Camera 10: when the next shot is the last of its group (no successor, or one with another nA4)
// and less than lbl_80281F78->f178 of this shot is left, start the move to the tuning's position
// (nCamera 1, f118 the time left).
void fn_800C0914(View* pView, int nPlayer) {
    f32 v[4];
    void* pCam;
    void* pSub;
    f32 fLead;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fLead = lbl_80281F78->f178;
    Vec_Copy(lbl_80281F78->v17C, v);
    fn_8003EA50(nPlayer, pCam, pSub, pView->a84, pView->a19C, 0, gSession.fFrameTime);
    if (pView->p134 != NULL && (pView->p134->p40 == NULL || pView->p134->nA4 != pView->p134->p40->nA4)
        && pView->nCamera == 0 && pView->f110 - pView->fCamTime < fLead) {
        pView->nCamera = 1;
        Vec_Copy(v, pView->vC4);
        pView->f114 = 0.0f;
        pView->f118 = pView->f110 - pView->fCamTime;
    }
}

// Camera 13.
void fn_800C14B0(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    GolfCamState* pState;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->n194 = 0;
    pView->n198 = 0;
    pView->fCamTime = 0.0f;
    pState = lbl_80282220;
    pState->f68 = 0.0f;
    fn_800C5D64(pView, pCam, pSub, nPlayer, pState);
}

// Camera 20.
void fn_800C1670(View* pView, int nPlayer) {
    if (pView->n260 == 9) {
        fn_800B3550(1, pView, nPlayer);
    } else {
        fn_800B3550(0, pView, nPlayer);
    }
    lbl_80282220->b56 = 1;
}

// Camera 22, the shutter camera: script 0x3E of the current shot (13 when it has none), with the
// game in super slow motion.
void GolfCamera_InitShutterCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    void* pScript;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pScript = fn_8003A7C8(nPlayer, 0x3E, pView->p130);
    if (pScript == NULL) {
        pScript = fn_8003A7C8(nPlayer, 0xD, pView->p130);
    }
    if (pScript != NULL) {
        CameraScript_InterpToNewScript(pView->a84, pScript, nPlayer, pCam, pSub, 5, 0x19, 0.0f, 100.0f, 0.0f);
    }
    pView->n194 = 0;
    pView->f18C = 0.0f;
    pView->f190 = 0.0f;
    lbl_80282220->b5B = 1;
    pView->f190 = 0.0f;
    GameEffects_SetSuperSlowMo(1, nPlayer, 1.0f);
}

// Camera 18 (the tutorial wait): with no shot to go to, fall back on the state's own two shots.
void fn_800C37FC(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, pView->a19C, 0, gSession.fFrameTime);
    if (pView->p134 == NULL) {
        pView->p130 = &lbl_80282220->shot6C;
        pView->p134 = &lbl_80282220->shot12C;
        pView->n140 = 7;
        pView->f110 = 40.0f;
        pView->n154 = 1;
        pView->fCamTime = 0.00001f;
    }
}

// Camera 24.
void fn_800C3EB8(View* pView, int nPlayer) {
    fn_8006351C(pView, nPlayer, 10);
}
