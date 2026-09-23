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
    struct CamShot* p44;        // 0x44  in View.shot19C: the shot camera 13 goes back to
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
    u8   bAD;                   // 0xAD
    u8   unkAE;
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
    f32     f64;                // 0x064  camera 7's slow-motion rate while b5A is set
    f32     f68;                // 0x068
    CamShot shot6C;             // 0x06C
    CamShot shot12C;            // 0x12C
    s32     n1EC[5];            // 0x1EC
} GolfCamState;

// The golfer's animation object (gPlayers[].nShotHandle); Swing.c has more of it.
typedef struct ShotObj {
    u8   unk0[0x17C];
    f32  fAnimTime;             // 0x17C
} ShotObj;

// The camera tuning values.
typedef struct CamTuning {
    u8   unk0[0x68];
    f32  v68[4];                // 0x68  camera 13's fn_80038010 vector; [3] shrinks as the camera's time runs
    f32  f78;                   // 0x78  ... over this many seconds
    u8   unk7C[0x94 - 0x7C];
    f32  f94;                   // 0x94  the elevator camera's first blend value
    u8   unk98[0x170 - 0x98];
    f32  f170;                  // 0x170
    u8   unk174[4];
    f32  f178;                  // 0x178
    f32  v17C[4];               // 0x17C
    u8   unk18C[0x1C8 - 0x18C];
    s32  bCheckSlope;           // 0x1C8  fn_800C4650 tests the slope to the target (fn_800C4520)
    s32  bCheckTerrain;         // 0x1CC  and the ground in between (fn_800C4604)
    u8   unk1D0[0x1E0 - 0x1D0];
    f32  fSlopeUp;              // 0x1E0
    f32  fSlopeDown;            // 0x1E4
} CamTuning;

// A view (one per split-screen half); only what this file touches. The fields Swing.c also uses
// keep its names.
typedef struct View {
    f32      v0[4];             // 0x000  what fn_8001731C returns: the camera's position (inferred)
    f32      v10[4];            // 0x010  what fn_80017314 returns: where it looks (the pin, for camera 5)
    f32      v20[4];            // 0x020
    u8       unk30[0x70 - 0x30];
    s32      nCurCamera;        // 0x070
    void*    p74;               // 0x074  the camera sequence the shots are picked from
    void*    p78;               // 0x078
    u8       unk7C[4];
    CamShot* p80;               // 0x080
    u8       a84[0x40];         // 0x084  the camera script the camera functions drive
    f32      vC4[4];            // 0x0C4
    u8       unkD4[0x104 - 0xD4];
    f32      fCamTime;          // 0x104  time on this camera
    u8       unk108[0x110 - 0x108];
    f32      f110;              // 0x110
    f32      f114;              // 0x114
    f32      f118;              // 0x118
    f32      f11C;              // 0x11C
    u8       unk120[0x124 - 0x120];
    f32      f124;              // 0x124
    f32      f128;              // 0x128
    u8       unk12C[4];
    CamShot* p130;              // 0x130  the current shot
    CamShot* p134;              // 0x134  the next one
    u8       unk138[0x140 - 0x138];
    s32      n140;              // 0x140
    s32      nCamera;           // 0x144
    s32      n148;              // 0x148  the shot kind asked for
    s32      n14C;              // 0x14C  the shot kind last started
    u8       unk150[0x153 - 0x150];
    u8       b153;              // 0x153
    s32      n154;              // 0x154
    u8       unk158[0x164 - 0x158];
    s32      n164;              // 0x164  a shot kind for fn_8003A950 (25 = none)
    u8       unk168[0x18C - 0x168];
    f32      f18C;              // 0x18C
    f32      f190;              // 0x190
    s32      n194;              // 0x194
    s32      n198;              // 0x198
    CamShot  shot19C;           // 0x19C  a shot built by hand (the knee, steep-slope and elevator cameras)
    s32      nSavedCamera;      // 0x25C
    s32      n260;              // 0x260  set by the swing camera and the game modes
    s32      n264;              // 0x264  which of the shots 0x1D..0x21 fn_800C4E80 tries next
    u8       b268;              // 0x268
    u8       b269;              // 0x269
    u8       b26A;              // 0x26A
} View;

// The game-breaker part of the effects state (GameEffects.c has the whole struct).
typedef struct GameEffects {
    u8   unk0[0x12];
    u8   bGameBreaker;          // 0x12  the letterbox is up
    u8   unk13;
    s32  nGBType;               // 0x14  0 scripted, 1 predicted
} GameEffects;

extern GameEffects   lbl_80202898;
extern GolfCamState* lbl_80282220;
extern CamTuning*    lbl_80281F78;

void*    fn_80017004(int nView);
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
f32*     fn_8001731C(View* pView);   // the view's camera position
f32*     fn_80017314(View* pView);   // where it looks
void     Vec3Copy(f32* pSrc, f32* pDst);
void     CameraScript_RecordCurrentCam(CamShot* pShot, void* pCam, void* pSub, int nPlayer, void* pScript,
                                       int a);
void     fn_8003F2E0(void* pScript, f32 fTime);
f32      (*fn_8001ED08(int nHandle, int nBone))[4];
void     fn_8001EF34(f32* pIn, f32 f, f32* pOut);           // scale a vector
void     fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // a + f x b
u8       fn_800635D0(int nPlayer);
u8       fn_800B36F4(View* pView, int nPlayer, f32 fFrameTime);
double   fabsf(double x);
void     fn_800C73B8(f32* pA, f32* pB, f32* pOut);
void     fn_800C73DC(f32* pA, f32* pB, f32* pOut);
void     fn_800C7400(f32* pA, f32* pOut);
void     GolfCamera_ProcessBallFlightCamera(View* pView, int nPlayer);
void     fn_80063CF0(void* pView, int nCamera, int nPlayer);
u8       fn_800C7160(View* pView);
void     fn_800C6110(View* pView, int nPlayer, int a);
void     GolfCamera_CutToGolferDoneAnimatingCam(View* pView, int nPlayer);
int      fn_80062C10(int nHandle);
int      fn_80062C1C(int nHandle);
void     fn_8003DCE8(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003EA50(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
CamShot* fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot);
CamShot* fn_8003A950(void* pSequence, int nKind, int* pA, f32* pF1, f32* pF2, int* pB, f32* pF3, int nPlayer);
u8       fn_8003DC78(CamShot* pShot);
void     CameraScript_InterpToNewScript(void* pScript, CamShot* pShot, int nPlayer, void* pCam, void* pSub,
                                        int nA, f32 f1, f32 f2, int nB, f32 f3);
CamShot* fn_8006509C(s32 n);
void     fn_8006351C(View* pView, int nPlayer, int nCamera);
f32      fn_80072CB8(void* p);
void     fn_800B3550(int a, View* pView, int nPlayer);
u8       fn_800B4908(void);
void     GolfCamera_ComputeSteepSlopeCamVectors(View* pView, int nPlayer);
void     fn_800C5D64(View* pView, f32* pCam, f32* pSub, int nPlayer);
void     fn_800C6DE4(void);
void     fn_800C6DFC(void);
void     fn_800C6E14(void);
void     fn_800C6E2C(void);
void     GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate);
u8       fn_80063C7C(void* pView);
f32      fn_8005CB78(int nHandle, unsigned long long uEvent);   // an animation event's time
u8       CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot, void* pScript);
f32      fn_8000C5FC(f32* pA, f32* pB);                         // dot product
u8       fn_800C708C(View* pView);
f32      fn_80062C28(int nHandle);            // the animation time left
int      fn_80016D10(void);
void     fn_80038010(u8 a, int n, f32* pVec);
CamShot* fn_800C4DF8(int nFirst, int nPlayer);
void     fn_800C5EC0(View* pView, f32* pCam, f32* pSub, int nPlayer);
u8       fn_800C6D80(void);
u8       Ter_CheckForGroundCollision(void* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, void* pA,
                                     void* pB);

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
        CameraScript_InterpToNewScript(pView->a84, &pView->shot19C, nPlayer, pCam, pSub, 1, lbl_80281F78->f94,
                                       100.0f, 0x19, 0.0f);
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
        CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19, 0.0f);
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

// Camera 11's process: start the shot kind asked for (n148) once it changes, but not before the
// swing animation has a second left, unless the camera has already cut (n194).
void fn_800C0C0C(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    u8 bStart;
    f32 f1;
    f32 f2;
    int nB;
    f32 f3;
    int nA;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    nB = 0x19;
    bStart = 1;
    f1 = 0.0f;
    f2 = 0.0f;
    f3 = 0.0f;
    nA = 5;
    if (pView->n148 != pView->n14C) {
        if (pView->n148 == 0x17) {
            pView->n194 = 1;
            if (pView->nCamera != 0) {
                pView->f114 = 0.0f;
                pView->nCamera = 2;
            }
        }
        if (pView->n194 < 1 && fn_80062C28(gPlayers[nPlayer].nShotHandle) < 1.0f) {
            bStart = 0;
        }
        pView->n14C = pView->n148;
        if (bStart) {
            pShot = fn_8003A950(pView->p74, pView->n148, &nA, &f1, &f2, &nB, &f3, nPlayer);
            if (pShot != NULL) {
                CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, nA, f1, f2, nB, f3);
            }
        }
    }
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
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

// Camera 13's process.
void fn_800C1530(View* pView, int nPlayer) {
    f32 v[4];
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    s32 n140;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (fn_800C6D80()) {
        fn_800C5EC0(pView, pCam, pSub, nPlayer);
    }
    Vec_Copy(lbl_80281F78->v68, v);
    if (!fn_800C6D80() && pView->fCamTime < lbl_80281F78->f78) {
        v[3] *= 1.0f - pView->fCamTime / lbl_80281F78->f78;
        fn_80038010(1, fn_80016D10(), v);
    }
    pShot = pView->p130;
    n140 = pView->n140;
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
    if (fn_800C6D80() && pShot != pView->p130) {
        if (pShot != NULL && pShot->p40 == NULL) {
            pView->n140 = 5;
            pView->f110 = 0.0f;
            pView->p134 = NULL;
        } else {
            pView->p134 = pShot;
            pView->n140 = n140;
        }
    }
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

// Camera 20: once fn_800B36F4 is done, back to the full view and on to the flight camera (14).
void fn_800C16C4(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    int nView;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    nView = gPlayers[nPlayer].nView0;
    if (fn_800B36F4(pView, nPlayer, gSession.fFrameTime)) {
        fn_800C1790(pView, nPlayer);
        View_SetCamera(fn_80017028(nView), 14, nPlayer, nView);
        GolfCamera_ProcessBallFlightCamera(pView, nPlayer);
    } else {
        fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
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
        CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19, 0.0f);
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

// Camera 16, the in-the-hole camera.
void GolfCamera_InitInHoleCamera(View* pView, int nPlayer) {
    fn_8001731C(pView);
    fn_80017314(pView);
    GOLFERSTATE_GetCurrentState(nPlayer);
    pView->p78 = pView->p74;
    pView->n194 = 0;
    pView->n194 = 1;
    fn_80063CF0(pView, 8, nPlayer);
    if (gPlayers[nPlayer].bPlanReady == 1) {
        if (fn_800C7160(pView)) {
            fn_800C6110(pView, nPlayer, 1);
            pView->n198 = 0;
        } else if (pView->fCamTime > 2.0f) {
            GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        } else {
            pView->n194 = 0;
        }
    } else {
        if (fn_800C7160(pView) || (s8)GOLFERSTATE_GetCurrentState(nPlayer) == GS_REMOVE_BALL) {
            fn_800C6110(pView, nPlayer, 1);
        }
        pView->n198 = 0;
    }
}

// Camera 16: once the golfer's animation 9 or 12 is under way, cut to the golfer (once).
void GolfCamera_ProcessInHoleCamera(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->n194 != 1) {
        GolfCamera_InitInHoleCamera(pView, nPlayer);
    }
    if ((fn_80062C1C(gPlayers[nPlayer].nShotHandle) || fn_80062C10(gPlayers[nPlayer].nShotHandle))
        && (fn_80095780(gPlayers[nPlayer].nShotHandle) == 9
            || fn_80095780(gPlayers[nPlayer].nShotHandle) == 12)
        && (pView->p130 == NULL || pView->p130->bAA) && pView->n198 < 1) {
        GolfCamera_CutToGolferDoneAnimatingCam(pView, nPlayer);
        pView->n198 = 1;
    }
    fn_8003DCE8(nPlayer, pCam, pSub, pView->a84, &pView->shot19C, 0, gSession.fFrameTime);
}

// Camera 17: move to a fixed spot unless the camera is already moving there.
void fn_800C3478(View* pView, int nPlayer) {
    f32 v[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if (fn_80063C7C(pView) || fn_80063C90(pView)) {
        fn_80063B98(pView, lbl_80281F78->f170, v);
    }
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

// Camera 23: from 1.5 up over the ball, looking along -x, script 0x23.
void fn_800C38BC(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    pView->p130 = NULL;
    pCam[0] = gPlayers[nPlayer].fBallX;
    pCam[2] = gPlayers[nPlayer].fBallZ;
    pCam[1] = 1.5f;
    pSub[0] = pCam[0] - 1.0f;
    pSub[1] = pCam[1];
    pSub[2] = pCam[2];
    pShot = fn_8003A7C8(0, 0x23, NULL);
    if (pShot != NULL) {
        CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19, 0.0f);
        pView->p80 = pShot;
        pView->n194 = -1;
        pView->n198 = 0x23;
    }
}

// Camera 24.
void fn_800C3EB8(View* pView, int nPlayer) {
    fn_8006351C(pView, nPlayer, 10);
}

// Camera 24: ride on the golfer's bone 10, looking forward along it.
void fn_800C3EDC(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    f32 (*m)[4];
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    m = fn_8001ED08(gPlayers[nPlayer].nShotHandle, 10);
    Vec_Copy(m[3], pSub);
    fn_8000C5D4(pSub, m[1], 0.1f, pSub);
    fn_8001EF34(m[2], 0.5f, pCam);
    fn_800C73B8(m[3], pCam, pCam);
    fn_8000C5D4(pCam, m[1], 0.1f, pCam);
    pCam[3] = 1.0f;
    fn_800C7400(m[0], pView->v20);
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

u8 fn_800C441C(View* pView, int nPlayer) {
    fn_8001731C(pView);
    if (pView->n260 == 2) {
        return 1;
    }
    if (pView->n260 == 5) {
        return 1;
    }
    if (pView->n260 == 8) {
        return 1;
    }
    if (pView->n260 == 11) {
        return 1;
    }
    if (pView->n260 == 15) {
        return 1;
    }
    return pView->n260 == 16;
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

// Is the target too steep from the ball: |dy / dx| at least fUp going up, fDown going down.
u8 fn_800C4520(View* pView, int nPlayer, f32 fUp, f32 fDown) {
    f32 dx = gPlayers[nPlayer].fTargetX - *(f32*)(gPlayers[nPlayer].ball + 0);
    f32 dy = gPlayers[nPlayer].fTargetY - *(f32*)(gPlayers[nPlayer].ball + 4);
    f32 fSlope;
    if (fabsf(dx) < 1e-6f) {
        return 0;
    }
    fSlope = fabsf(dy / dx);
    if (dy > 0.0f) {
        if (fSlope < fUp) {
            return 0;
        }
    } else {
        if (fSlope < fDown) {
            return 0;
        }
    }
    return 1;
}

// Is anything in the way between the view and the player's target?
u8 fn_800C4604(View* pView, int nPlayer) {
    f32 vHit[4];
    f32 vNormal[4];
    u32 uA;
    u32 uB;
    return Ter_CheckForGroundCollision(gPlayers[nPlayer].pBallCourse, pView->v0, &gPlayers[nPlayer].fTargetX,
                                       vHit, vNormal, &uA, &uB);
}

// Should the view be moved: never on a putt; else by the tuning's slope and terrain tests, and when
// fn_800635D0 fails.
u8 fn_800C4650(View* pView, int nPlayer) {
    u8 bMove;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER) {
        return 0;
    }
    bMove = 0;
    if (lbl_80281F78->bCheckSlope) {
        bMove = fn_800C4520(pView, nPlayer, lbl_80281F78->fSlopeUp, lbl_80281F78->fSlopeDown);
        if (!bMove) {
            return 0;
        }
    }
    if (!bMove && lbl_80281F78->bCheckTerrain) {
        bMove = fn_800C4604(pView, nPlayer);
    }
    if (!bMove) {
        bMove = fn_800635D0(nPlayer) == 0;
    }
    return bMove;
}

// Move pOut from pFrom towards pTo by at most fMax; nonzero if it had to stop short.
int fn_800C4D2C(f32* pFrom, f32* pTo, f32* pOut, f32 fMax) {
    int bClamped = 0;
    f32 v[4];
    f32 vStep[4];
    f32 fDist;
    fn_800C73DC(pTo, pFrom, v);
    fDist = fn_80009680(fn_80009744(v));
    if (fDist > fMax && fDist > 1e-6f) {
        fn_8001EF34(v, fMax / fDist, vStep);
        fn_800C73B8(pFrom, vStep, pOut);
        bClamped = 1;
    } else {
        Vec3Copy(pTo, pOut);
    }
    return bClamped;
}

// The first of shots 0x1C + n .. 0x21 the player's camera plan has (none in modes 6, 7 and 8).
CamShot* fn_800C4DF8(int nFirst, int nPlayer) {
    int i;
    CamShot* pShot;
    if (Game_GetMode() != 6 && Game_GetMode() != 7 && Game_GetMode() != 8) {
        for (i = nFirst; i <= 5; i++) {
            pShot = fn_8003A7C8(nPlayer, i + 0x1C, NULL);
            if (pShot != NULL) {
                return pShot;
            }
        }
    }
    return NULL;
}

// Cycle through shots 1..5 (fn_800C4DF8, skipping 3) when there is no next shot or it is of kind
// 6, 8, 9 or 10; with none, go back to the saved shot p80.
void fn_800C4E80(View* pView, int nPlayer) {
    f32* pCam;
    f32* pSub;
    CamShot* pShot;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->p134 == NULL || pView->n140 == 6 || pView->n140 == 9 || pView->n140 == 8
        || pView->n140 == 10) {
        pView->n264++;
        if (pView->n264 > 5) {
            pView->n264 = 1;
        }
        pShot = fn_800C4DF8(pView->n264, nPlayer);
        if (pShot != NULL && pView->n264 != 3) {
            CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, pShot->bAB, pShot->f48,
                                           1000.0f, 0x19, 0.0f);
        } else if (pView->p80 != NULL && pView->p80 != pView->p130) {
            if (pView->p130 != NULL) {
                CameraScript_InterpToNewScript(pView->a84, pView->p80, nPlayer, pCam, pSub, pView->p130->bAB,
                                               pView->p130->f48, 1000.0f, 0x19, 0.0f);
            } else {
                CameraScript_InterpToNewScript(pView->a84, pView->p80, nPlayer, pCam, pSub, 5, 0.0f, 1000.0f,
                                               0x19, 0.0f);
            }
        }
    }
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

// Camera 13's process: a new angle from the current shot each time (script 13), up to
// fn_800C6B38's count, then back to the saved shot. Kinds 15 and 16 use script 0x22 instead.
// The callers pass the view's position and aim, but it fetches them again.
void fn_800C5D64(View* pView, f32* pViewCam, f32* pViewSub, int nPlayer) {
    CamShot* pShot;
    f32* pCam;
    f32* pSub;
    pCam = fn_8001731C(pView);
    pSub = fn_80017314(pView);
    if (pView->n260 == 15 || pView->n260 == 16) {
        pShot = fn_8003A7C8(nPlayer, 0x22, NULL);
        if (pShot != NULL) {
            CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                           0.0f);
            pView->f110 = 1.0f;
        }
        pView->f190 = 0.0f;
        lbl_80282220->b59 = 1;
    } else {
        if (pView->n194 == 0) {
            pView->shot19C.p44 = pView->p130;
        }
        if (pView->n194 >= fn_800C6B38(pView)) {
            pView->p130 = pView->shot19C.p44;
            pView->p134 = NULL;
            pView->fCamTime = 0.0f;
        } else {
            pShot = fn_8003A7C8(nPlayer, 0xD, pView->p130);
            if (pShot != NULL && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, pView->a84)) {
                CameraScript_InterpToNewScript(pView->a84, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                               0.0f);
            }
        }
    }
}

// The swing's progress as 0..1 in fCamTime: start to top of the backswing (f190 0), then top to
// the end (f190 1).
void fn_800C5EC0(View* pView, f32* pCam, f32* pSub, int nPlayer) {
    f32 fEnd;
    f32 fStart;
    f32 fTop;
    f32 fTime;
    f32 t;
    fEnd = fn_8005CB78(gPlayers[nPlayer].nShotHandle, 2);
    fStart = fn_8005CB78(gPlayers[nPlayer].nShotHandle, 0);
    fTop = fn_8005CB78(gPlayers[nPlayer].nShotHandle, 1);
    fTime = ((ShotObj*)gPlayers[nPlayer].nShotHandle)->fAnimTime;
    if (fTime < fTop) {
        t = (fTime - fStart) / (fTop - fStart);
        pView->fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
        pView->f190 = 0.0f;
    } else {
        pView->f190 = 1.0f;
        t = (fTime - fTop) / (fEnd - fTop);
        pView->fCamTime = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    }
}

// Step shot19C.f60 up by 0.1 a frame, but keep it 3 short of the ground distance from the ball to the
// aim point.
void fn_800C6010(View* pView, int nPlayer) {
    f32 v[4];
    fn_800C73DC(gPlayers[nPlayer].vTarget2, &gPlayers[nPlayer].fBallX, v);
    v[1] = 0.0f;
    if (pView->shot19C.f60 + 0.1f < (f32)fn_80009680(fn_80009744(v)) - 3.0f) {
        pView->shot19C.f60 += 0.1f;
    } else if (pView->shot19C.f60 > (f32)fn_80009680(fn_80009744(v)) - 3.0f) {
        pView->shot19C.f60 = (f32)fn_80009680(fn_80009744(v)) - 3.0f;
        if (pView->shot19C.f60 < 0.0f) {
            pView->shot19C.f60 = 0.0f;
        }
    }
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

// The slow-motion rate for the swing camera kind.
f32 fn_800C6B7C(View* pView) {
    switch (pView->n260) {
    case 2:
        return 0.5f;
    case 8:
        if (fn_800C4518(pView) >= 2) {
            return 0.2f;
        }
        return 0.5f;
    case 11:
        if (fn_800C4518(pView) >= 2) {
            return 0.65f;
        }
        return 0.5f;
    case 5:
        if (fn_800C4518(pView) >= 1) {
            return 0.2f;
        }
        return 0.5f;
    case 15:
        if (pView->f190 > 0.0f) {
            return 1.5f;
        }
        return 0.2f;
    case 16:
        if (pView->f190 > 0.0f) {
            return 0.2f;
        }
        return 1.5f;
    case 4:
    case 9:
        return 1.0f;
    case 7:
        if (lbl_80282220->b5A) {
            return lbl_80282220->f64;
        }
        return 1.0f;
    case 3:
        return 1.0f;
    }
    return 1.0f;
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

u8 fn_800C6F14(View* pView, int nPlayer) {
    if (pView->n164 != 0x19 && pView->p74 != NULL
        && fn_8003A950(pView->p74, pView->n164, NULL, NULL, NULL, NULL, NULL, nPlayer) != NULL) {
        return 1;
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

// Is the ball behind the camera (on the far side from where it looks)? Only once the shot has
// settled, and not for shot kind 3.
u8 fn_800C71A4(View* pView, int nPlayer) {
    f32 vLook[4];
    f32 vBall[4];
    if (pView->p130 == NULL) {
        return 0;
    }
    if (pView->p130->bAD == 3) {
        return 0;
    }
    if (pView->f11C > 5.0f) {
        return 0;
    }
    fn_800C73DC(pView->v10, pView->v0, vLook);
    fn_800C73DC((f32*)gPlayers[nPlayer].ball, pView->v0, vBall);
    vLook[1] = 0.0f;
    vBall[1] = 0.0f;
    if (vLook[0] != 0.0f || vLook[1] != 0.0f || vLook[2] != 0.0f) {
        fn_800BAF04(vLook, vLook);
    }
    if (vBall[0] != 0.0f || vBall[1] != 0.0f || vBall[2] != 0.0f) {
        fn_800BAF04(vBall, vBall);
    }
    if (fn_8000C5FC(vLook, vBall) > 0.0f) {
        return 0;
    }
    return fn_800C708C(pView);
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

// Cameras 1 and 2 are done once f18C reaches 1; the others at once.
u8 fn_800C7340(View* pView, int nPlayer) {
    if (pView->nCurCamera == 1) {
        return pView->f18C >= 1.0f;
    }
    if (pView->nCurCamera == 2) {
        return pView->f18C >= 1.0f;
    }
    return 1;
}

// The time left on the current shot (0 when there is no next one).
f32 fn_800C7394(View* pView) {
    if (pView->p134 == NULL) {
        return 0.0f;
    }
    return pView->f110 - pView->fCamTime;
}

// Adds two vectors (three floats) into pOut.
asm void fn_800C73B8(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}

// Subtracts pB from pA (three floats) into pOut.
asm void fn_800C73DC(register f32* pA, register f32* pB, register f32* pOut) {
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

// Negates a vector (three floats) into pOut.
asm void fn_800C7400(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}

f32 fn_800C741C(u8* p) {
    if (p == NULL) {
        return 0.0f;
    }
    return fn_80072CB8(p + 0x40C);
}

// A predicted game breaker is on.
u8 fn_800C7450(void) {
    int bOn = 0;
    if (lbl_80202898.bGameBreaker && lbl_80202898.nGBType == 1) {
        bOn = 1;
    }
    return bOn;
}
