// GoGolfCam.c (EA's name, from its asserts): the golf cameras (TW06's GolfCamera_*). Each camera
// mode has an init, called by View_SetCamera, and a per-frame process, called by
// CameraController_Idle; both drive the view's camera script. The shared camera state
// (lbl_80282220) is allocated here, with a per-course elevator camera height.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// A camera shot (0xC0 bytes): a named script position the camera script moves to. The shots of a
// sequence are chained through p40.
typedef struct CamShot {
    char szName[0x24];          // 0x00
    f32  f24;                   // 0x24  height; the elevator camera adds the course's own
    u8   unk28[0x40 - 0x28];
    struct CamShot* p40;        // 0x40
    u8   unk44[4];
    f32  f48;                   // 0x48  how long the shot lasts
    f32  f4C;                   // 0x4C
    u8   unk50[0x60 - 0x50];
    f32  f60;                   // 0x60
    f32  f64;                   // 0x64
    f32  f68;                   // 0x68
    f32  f6C;                   // 0x6C
    f32  f70;                   // 0x70
    f32  f74;                   // 0x74
    f32  f78;                   // 0x78
    f32  f7C;                   // 0x7C
    u8   unk80[4];
    f32  f84;                   // 0x84
    u8   unk88[0xA4 - 0x88];
    s32  nA4;                   // 0xA4
    u8   bA8;                   // 0xA8
    u8   unkA9;
    u8   bAA;                   // 0xAA
    u8   bAB;                   // 0xAB
    u8   bAC;                   // 0xAC
    u8   unkAD[2];
    u8   bAF;                   // 0xAF
    u8   bB0;                   // 0xB0
    u8   bB1;                   // 0xB1
    u8   bB2;                   // 0xB2
    u8   unkB3[0xC0 - 0xB3];
} CamShot;

// The golf cameras' shared state (0x200 bytes, allocated by fn_800BD894).
typedef struct GolfCamState {
    f32     fElevatorHeight[21];    // 0x000  per course, added to the elevator shot's height
    u8      b54;                // 0x054
    u8      b55;                // 0x055
    u8      b56;                // 0x056
    u8      b57;                // 0x057
    u8      b58;                // 0x058
    u8      b59;                // 0x059
    u8      b5A;                // 0x05A
    u8      b5B;                // 0x05B  set by the shutter camera
    u8      b5C;                // 0x05C
    u8      unk5D[3];
    s32     n60;                // 0x060  passed to fn_8006509C
    u8      unk64[4];
    f32     f68;                // 0x068
    CamShot shot6C;             // 0x06C
    CamShot shot12C;            // 0x12C
    s32     n1EC[5];            // 0x1EC
} GolfCamState;

// A view (one per split-screen half); only what this file touches. The fields Swing.c also uses
// keep its names.
typedef struct View {
    f32      v0[4];             // 0x000  what fn_8001731C returns: the camera's position (inferred)
    f32      v10[4];            // 0x010  what fn_80017314 returns: where it looks (the pin, for camera 5)
    u8       unk20[0x74 - 0x20];
    void*    p74;               // 0x074  the camera sequence the shots are picked from
    void*    p78;               // 0x078
    u8       unk7C[0x84 - 0x7C];
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
    CamShot  shot19C;           // 0x19C  a shot built by hand (the knee, steep-slope and elevator cameras)
    s32      nSavedCamera;      // 0x25C
    s32      n260;              // 0x260  set by the swing camera and the game modes
    u8       unk264[4];
    u8       b268;              // 0x268
    u8       b269;              // 0x269
    u8       b26A;              // 0x26A
} View;

extern GolfCamState* lbl_80282220;

void*    fn_80008370(void* pCamera);
void     fn_80045470(void* pLens, f32 fFov);
void*    fn_80012EF0(void* p);
void     fn_800171D8(void* pRect, f32 x, f32 y, f32 w, f32 h);
void     fn_800352BC(void);
void     fn_80035240(int a);
void*    fn_8001614C(void);
void     fn_80013CCC(void* pCamera);
void     fn_80013EEC(void* pCamera);
void     fn_80016B9C(void);
void     fn_80012EF8(void);
void*    fn_8001731C(View* pView);
void*    fn_80017314(View* pView);
void     Vec3Copy(f32* pSrc, f32* pDst);
void     CameraScript_RecordCurrentCam(CamShot* pShot, void* pCam, void* pSub, int nPlayer, void* pScript,
                                       int a);
void     fn_8003F2E0(void* pScript, f32 fTime);
void     fn_8003DCE8(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003EA50(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
CamShot* fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot);
CamShot* fn_8003A950(void* pSequence, int nKind, f32* pa, f32* pb, f32* pc, f32* pd, f32* pe, int nPlayer);
u8       fn_8003DC78(CamShot* pShot);
void     CameraScript_InterpToNewScript(void* pScript, CamShot* pShot, int nPlayer, void* pCam, void* pSub,
                                        int a, int b, f32 f1, f32 f2, f32 f3);
CamShot* fn_8006509C(s32 n);
void     fn_8006351C(View* pView, int nPlayer, int nCamera);
f32      fn_80072CB8(void* p);
void     fn_800B3550(int a, View* pView, int nPlayer);
u8       fn_800B4908(void);
void     GolfCamera_ComputeSteepSlopeCamVectors(View* pView, int nPlayer);
void     fn_800C5D64(View* pView, void* pCam, void* pSub, int nPlayer);
void     fn_800C6DE4(void);
void     fn_800C6DFC(void);
void     fn_800C6E14(void);
void     fn_800C6E2C(void);
void     GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate);

// Allocate the shared camera state: every flag off, each course's elevator height 10.
void fn_800BD894(void) {
    int i;
    lbl_80282220 = fn_80009B34(0x200, 2, 0, "GoGolfCam.c", 164);
    lbl_80282220->b54 = 0;
    lbl_80282220->b55 = 0;
    lbl_80282220->b56 = 0;
    lbl_80282220->b57 = 0;
    lbl_80282220->b58 = 0;
    lbl_80282220->b59 = 0;
    lbl_80282220->b5A = 0;
    lbl_80282220->b5B = 0;
    lbl_80282220->n60 = 0;
    lbl_80282220->b5C = 0;
    lbl_80282220->f68 = 0.0f;
    for (i = 0; i < 5; i++) {
        lbl_80282220->n1EC[i] = 1;
    }
    for (i = 0; i < 21; i++) {
        lbl_80282220->fElevatorHeight[i] = 10.0f;
    }
}

void fn_800BDA04(void) {
    fn_80009E70(lbl_80282220);
    lbl_80282220 = NULL;
}

// Camera 0: only the script's per-frame update.
void fn_800BDBA4(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

void GolfCamera_InitSteepSlopeCamera(View* pView, int nPlayer) {
    if (pView->p130 != NULL) {
        GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
        pView->p130 = NULL;
        CameraScript_RecordCurrentCam(&pView->shot19C, pView->v0, pView->v10, nPlayer, pView->a84, 0);
        strcpy(pView->shot19C.szName, "STEEPSLOPE CAM");
    }
}

void GolfCamera_ProcessSteepSlopeCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    GolfCamera_ComputeSteepSlopeCamVectors(pView, nPlayer);
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 3, the elevator camera: the current view raised by the course's elevator height.
void GolfCamera_InitElevatorCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->p130 != NULL) {
        CameraScript_RecordCurrentCam(&pView->shot19C, pCam, pSub, nPlayer, pView->a84, 0);
        strcpy(pView->shot19C.szName, "ELEVATOR CAM");
        pView->shot19C.f24 += lbl_80282220->fElevatorHeight[Game_GetCourse()];
        pView->shot19C.bAC = 9;
        CameraScript_InterpToNewScript(pView->a84, &pView->shot19C, nPlayer, pCam, pSub, 1, 0x19,
                                       lbl_80281F78->f94, 100.0f, 0.0f);
    }
}

// Camera 3 (the elevator camera).
void fn_800BF094(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 8: a 60-degree lens, no script.
void fn_800BF110(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView0)), DEG(60.0f));
    pView->n194 = 0;
    pView->p74 = NULL;
    pView->p130 = NULL;
}

// Camera 9: the same as camera 8.
void fn_800BF5E4(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView0)), DEG(60.0f));
    pView->n194 = 0;
    pView->p130 = NULL;
    pView->p74 = NULL;
}

// Camera 5: look at the pin through a 30-degree lens.
void fn_800C0364(View* pView, int nPlayer) {
    void* pSub;
    CourseInfo* pCourse;
    fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        int nHole = Game_CurrentHole();
        Vec3Copy(&pCourse->pin[nHole].x, pSub);
        pView->p130 = NULL;
        pView->p134 = NULL;
        fn_80045470(fn_80008370(fn_80017004(gPlayers[nPlayer].nView0)), DEG(30.0f));
        pView->fCamTime = 0.0f;
    }
}

// Camera 6: script 0x3A.
void fn_800C0624(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pShot = fn_8003A7C8(nPlayer, 0x3A, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0x19, 0.0f, 100.0f, 0.0f);
    }
}

// Camera 6.
void fn_800C06C8(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 7 (the knee cam): a shot built by hand from the current one, lasting 0.55 s.
void fn_800C0744(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    pView->shot19C.p40 = NULL;
    pView->shot19C.f60 = 0.0f;
    pView->shot19C.f64 = 0.0f;
    pView->shot19C.f68 = 0.5f;
    pView->shot19C.f6C = 0.5f;
    pView->shot19C.f70 = 0.0f;
    pView->shot19C.f74 = 0.0f;
    pView->shot19C.f78 = pView->p130->f78;
    pView->shot19C.f7C = pView->shot19C.f78;
    pView->shot19C.bAA = pView->p130->bAA;
    pView->shot19C.f84 = 0.0f;
    pView->shot19C.bA8 = 1;
    pView->shot19C.bAB = 1;
    pView->shot19C.bAC = 9;
    pView->shot19C.bAF = 1;
    pView->shot19C.bB0 = 9;
    pView->shot19C.bB1 = 2;
    pView->shot19C.bB2 = 0;
    pView->p134 = &pView->shot19C;
    pView->fCamTime = 0.0f;
    pView->n140 = 1;
    pView->f110 = 0.55f;
}

// Camera 7 (the knee cam).
void fn_800C0804(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 10: start on the first shot fn_8006509C gives.
void fn_800C0880(View* pView, int nPlayer) {
    CamShot* pShot;
    s32 n;
    n = lbl_80282220->n60;  // fake match: read before the store below, in the original's order
    lbl_80282220->f68 = 0.0f;
    pShot = fn_8006509C(n);
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
    fn_8003EA50(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
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
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->n194 = 0;
    pView->n198 = 0;
    pView->fCamTime = 0.0f;
    lbl_80282220->f68 = 0.0f;
    fn_800C5D64(pView, pCam, pSub, nPlayer);
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

// Undo what camera 20 set up (b56): the view's rectangle back to 0,0-1,1 and the render state reset.
void fn_800C1790(View* pView, int nPlayer) {
    int nView = gPlayers[nPlayer].nView0;
    if (lbl_80282220->b56) {
        fn_800171D8(fn_80012EF0(fn_80017004(nView)), 0.0f, 0.0f, 1.0f, 1.0f);
        fn_800352BC();
        fn_80035240(0);
        fn_80013CCC(fn_8001614C());
        fn_80013EEC(fn_8001614C());
        fn_80016B9C();
        fn_80012EF8();
        lbl_80282220->b56 = 0;
        lbl_80282220->b57 = 0;
    }
}

// Camera 22, the shutter camera: script 0x3E of the current shot (13 when it has none), with the
// game in super slow motion.
void GolfCamera_InitShutterCamera(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pShot = fn_8003A7C8(nPlayer, 0x3E, pView->p130);
    if (pShot == NULL) {
        pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
    }
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0x19, 0.0f, 100.0f, 0.0f);
    }
    pView->n194 = 0;
    pView->f18C = 0.0f;
    pView->f190 = 0.0f;
    lbl_80282220->b5B = 1;
    pView->f190 = 0.0f;
    GameEffects_SetSuperSlowMo(1, nPlayer, 1.0f);
}

// Camera 17: the script runs on at 1/59.94 s a frame.
void fn_800C34F8(View* pView, int nPlayer) {
    fn_8003F2E0(pView->a84, 0.016683351f);
    pView->f114 += 0.016683351f;
}

// Camera 18 (the tutorial wait): with no shot to go to, fall back on the state's own two shots.
void fn_800C37FC(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
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

u8 fn_800C43C0(View* pView, int nPlayer) {
    fn_8001731C(pView);
    if (pView->n260 == 1) {
        return 1;
    }
    if (pView->n260 == 6) {
        return 1;
    }
    return pView->n260 == 10;
}

u8 fn_800C44A8(View* pView, int nPlayer) {
    if (pView->n260 == 4) {
        return 1;
    }
    return pView->n260 == 9;
}

u8 fn_800C44CC(View* pView, int nPlayer) {
    return pView->n260 == 7;
}

u8 fn_800C44E0(View* pView, int nPlayer) {
    return pView->n260 == 3;
}

u8 fn_800C44F4(View* pView, int nPlayer) {
    if (pView->n260 == 13) {
        return 1;
    }
    return pView->n260 == 14;
}

int fn_800C4518(View* pView) {
    return pView->n194;
}

void fn_800C5CEC(View* pView, int nPlayer) {
    void* pCam;
    void* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->n194++;
    pView->fCamTime = 0.0f;
    fn_800C5D64(pView, pCam, pSub, nPlayer);
}

u8 fn_800C5FE4(View* pView, int nPlayer) {
    u8 bOn = 0;
    if (pView->n260 == 11) {
        bOn = 1;
    }
    if (bOn) {
        pView->n198 = 1;
    }
    return bOn;
}

void fn_800C60E8(View* pView, int nPlayer) {
    pView->shot19C.f60 -= 0.1f;
    if (pView->shot19C.f60 < 0.0f) {
        pView->shot19C.f60 = 0.0f;
    }
}

u8 fn_800C6604(View* pView) {
    return pView->n198 > 0;
}

int fn_800C6B38(View* pView) {
    switch (pView->n260) {
    case 2:
    case 8:
    case 11:
        return 2;
    case 5:
        return 1;
    case 3:
    case 4:
    case 7:
    case 9:
    case 15:
    case 16:
        return 0;
    }
    return 0;
}

void fn_800C6C8C(void) {
    fn_800C6DE4();
    fn_800C6DFC();
}

u8 fn_800C6CB0(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b56;
}

u8 fn_800C6CCC(void) {
    int bOn;
    if (lbl_80282220 == NULL) {
        return 0;
    }
    bOn = 0;
    if (lbl_80282220->b56 && fn_800B4908()) {
        bOn = 1;
    }
    return bOn;
}

u8 fn_800C6D28(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b54 || lbl_80282220->b55;
}

u8 fn_800C6D64(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b58;
}

u8 fn_800C6D80(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    return lbl_80282220->b59;
}

u8 fn_800C6D9C(void) {
    if (lbl_80282220 == NULL) {
        return 0;
    }
    if (lbl_80282220->b54 || lbl_80282220->b58 || lbl_80282220->b55) {
        return 1;
    }
    return 0;
}

void fn_800C6DE4(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b54 = 0;
    }
}

void fn_800C6DFC(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b58 = 0;
    }
}

void fn_800C6E14(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b59 = 0;
    }
}

void fn_800C6E2C(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b5A = 0;
    }
}

u8 fn_800C6E44(View* pView) {
    if (pView->n194 == 1 && pView->p134 == NULL
        && (pView->p130 == NULL || pView->p130->f4C < pView->fCamTime)) {
        return 1;
    }
    return 0;
}

u8 fn_800C6E88(View* pView, int nPlayer) {
    if (pView->b268 == 1) {
        return 0;
    }
    if (fn_8003A950(pView->p74, 0x17, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return pView->n194 < 1;
    }
    return 0;
}

void fn_800C7080(View* pView) {
    pView->n198 = 1;
}

u8 fn_800C708C(View* pView) {
    if (pView->p134 != NULL) {
        return fn_8003DC78(pView->p134) != 0;
    }
    if (pView->p130 != NULL) {
        return fn_8003DC78(pView->p130) != 0;
    }
    return 0;
}

void fn_800C70F8(View* pView, int a) {
    pView->b268 = a;
}

// The camera has settled: no shot, or more than 5 seconds on this one.
u8 fn_800C7100(View* pView) {
    if (pView->p130 == NULL || pView->p134 == NULL || pView->fCamTime > 5.0f) {
        return 1;
    }
    return 0;
}

int fn_800C7138(View* pView) {
    return pView->n260;
}

void fn_800C7140(int a) {
    lbl_80282220->b55 = a;
}

u8 fn_800C714C(void) {
    return lbl_80282220->b55;
}

void fn_800C7158(View* pView, int a) {
    pView->b269 = a;
}

u8 fn_800C7160(View* pView) {
    return pView->b269;
}

void fn_800C7168(View* pView, int a) {
    pView->b26A = a;
}

u8 fn_800C7170(View* pView) {
    return pView->b26A;
}

void fn_800C7178(View* pView, int nPlayer) {
    fn_800C1790(pView, nPlayer);
    fn_800C6E14();
    fn_800C6DE4();
    fn_800C6E2C();
}

u8 fn_800C72DC(View* pView) {
    return pView->n198 != 0;
}

void fn_800C72F0(void) {
    if (lbl_80282220 != NULL) {
        lbl_80282220->b54 = 0;
        lbl_80282220->b55 = 0;
        lbl_80282220->b56 = 0;
        lbl_80282220->b57 = 0;
        lbl_80282220->b58 = 0;
        lbl_80282220->b59 = 0;
        lbl_80282220->b5A = 0;
        lbl_80282220->b5B = 0;
    }
}

// The time left on the current shot (0 when there is no next one).
f32 fn_800C7394(View* pView) {
    if (pView->p134 == NULL) {
        return 0.0f;
    }
    return pView->f110 - pView->fCamTime;
}

f32 fn_800C741C(u8* p) {
    if (p == NULL) {
        return 0.0f;
    }
    return fn_80072CB8(p + 0x40C);
}
