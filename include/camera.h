// camera.h (our name): the views on screen and their cameras: the view structs, camera shots and
// sequences, a view's camera script, the camera tuning values, the golf cameras' (GoGolfCam.c)
// shared state, and the view and camera functions.

#ifndef CAMERA_H
#define CAMERA_H

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
LAYOUT_ASSERT(CamShot, 0xC0);

// A camera sequence (DynamicCam's): the shots a camera plan steps through.
typedef struct CamSequence {
    u8   unk0[0x20];
    struct CamSequence* p20;    // 0x20  the sequence that follows
    u8   unk24[0x38 - 0x24];
    f32  f38;                   // 0x38  its length
    u8   unk3C[0x44 - 0x3C];
    u8   b44;                   // 0x44  its kind
} CamSequence;

// A view's camera script (0x40 bytes at View + 0x84).
typedef struct CamScript {
    f32  v0[4];                 // 0x00  camera 4 puts the ball here
    f32  v10[4];                // 0x10  and the pin here
    u8   unk20[0x20];
} CamScript;

// A view's camera controller (View_SetCamera is TW06's CameraController_SetCameraMode): the
// camera mode, its shots and script. It sits at +4 in a ViewController; only the fields read so far.
typedef struct View {
    f32      v0[4];             // 0x000  what fn_8001731C returns: the camera's position (inferred)
    f32      v10[4];            // 0x010  what fn_80017314 returns: where it looks (the pin, for camera 5)
    f32      v20[4];            // 0x020
    f32      v30[4];            // 0x030
    f32      v40[4];            // 0x040
    f32      f50;               // 0x050
    f32      f54;               // 0x054
    f32      f58;               // 0x058
    u8       unk5C[0x68 - 0x5C];
    f32      fFade;             // 0x068  how far the view has faded (the ball-in-flight state waits for 0.5)
    u8       bFade;             // 0x06C  the fade is on
    u8       unk6D[3];
    s32      nCurCamera;        // 0x070
    CamSequence* p74;           // 0x074  the camera sequence the shots are picked from
    void*    p78;               // 0x078
    u8       unk7C[4];
    CamShot* p80;               // 0x080
    CamScript script;           // 0x084  the camera script the camera functions drive
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
    CamShot* p138;              // 0x138  where SwitchCrAPCamera records the current camera
    u8       unk13C[4];
    s32      n140;              // 0x140
    s32      nCamera;           // 0x144
    s32      n148;              // 0x148  the shot kind asked for
    s32      n14C;              // 0x14C  the shot kind last started
    u8       b150;              // 0x150
    u8       unk151[2];
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
    u8       unk26B;
} View;
LAYOUT_ASSERT(View, 0x26C);

// One of the four views on screen (lbl_801B8BA8, 0x288 bytes each; Player.nView[], TW06's
// viewControllerID, index them): the render camera, the view's camera controller, the player it
// follows.
typedef struct ViewController {
    void*    pCamera;           // 0x000  the render camera (fn_80017004)
    View     view;              // 0x004  (fn_80017028)
    s32      nPlayer;           // 0x270  set by fn_8001704C
    u8       unk274;
    u8       bFlagOut;          // 0x275  the flagstick is out (set while every player on this view is on the
                                //        green, the fringe or holed): no pin collisions
    u8       unk276[0x288 - 0x276];
} ViewController;
LAYOUT_ASSERT(ViewController, 0x288);

// The camera tuning values (GoGolfCam.c); only the fields read so far.
typedef struct CamTuning {
    u8   unk0[0x4C];
    f32  f4C;                   // 0x04C  camera 5's height over the ball
    f32  f50;                   // 0x050  camera 5: the ball-to-pin distance of a full swing out
    f32  f54;                   // 0x054  camera 5: how long the swing out lasts
    f32  f58;                   // 0x058  how long camera 13's closing shot lasts
    u8   unk5C[4];
    f32  f60;                   // 0x060  camera 13's slow-motion step length (0: every frame)
    f32  f64;                   // 0x064
    f32  v68[4];                // 0x068  camera 13's fn_80038010 vector; [3] shrinks as the camera's time
                                //        runs
    f32  f78;                   // 0x078  ... over this many seconds
    f32  f7C;                   // 0x07C  camera 13's fallback shots' f78/f7C: from this value ...
    f32  f80;                   // 0x080  ... to this one
    u8   unk84[4];
    f32  f88;                   // 0x088
    f32  f8C;                   // 0x08C
    u8   unk90[4];
    f32  f94;                   // 0x094  the elevator camera's first blend value
    u8   unk98[0xB8 - 0x98];
    f32  fB8;                   // 0x0B8  camera 15 waits this long on a ball near the green
    s32  nBeats;                // 0x0BC  the heartbeat camera's beats
    s32  nBeatFrames;           // 0x0C0
    f32  fC4;                   // 0x0C4
    f32  fC8;                   // 0x0C8
    f32  fCC;                   // 0x0CC
    u8   unkD0[0x168 - 0xD0];
    f32  f168;                  // 0x168  the ground clearance for CamScript_KeepAboveGround
    f32  f16C;                  // 0x16C  the obstruction radius around the ball for the pre-shot routine
    f32  f170;                  // 0x170  a blend for fn_80063B98 / fn_80063BF4
    u8   unk174[0x178 - 0x174];
    f32  f178;                  // 0x178
    f32  v17C[4];               // 0x17C
    u8   unk18C[0x1C0 - 0x18C];
    s32  n1C0;                  // 0x1C0  nonzero enables camera 19
    u8   unk1C4[0x1C8 - 0x1C4];
    s32  bCheckSlope;           // 0x1C8  fn_800C4650 tests the slope to the target (fn_800C4520)
    s32  bCheckTerrain;         // 0x1CC  and the ground in between (fn_800C4604)
    u8   unk1D0[0x1E0 - 0x1D0];
    f32  fSlopeUp;              // 0x1E0
    f32  fSlopeDown;            // 0x1E4
} CamTuning;

extern CamTuning* lbl_80281F78;         // EA's file list has GoCamTuningVars

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

extern GolfCamState* lbl_80282220;

// The create-a-player (CrAP) screen's state at lbl_80281EE0; only what the CrAP camera reads.
typedef struct CrAPGolfer {
    u8   unk0[8];
    struct Character* pChar;    // 0x08  the golfer being edited (character.h)
    s32  nC;                    // 0x0C
    u8   unk10[8];
    u8   b18;                   // 0x18  the camera script runs
} CrAPGolfer;
typedef struct CrAPState {
    s32  n0;                    // 0x00  0..4: picks the shot the CrAP camera frames (fn_800C39A8)
    u8   unk4[0xB4 - 0x4];
    CrAPGolfer* pB4;            // 0xB4
} CrAPState;

extern CrAPState* lbl_80281EE0;

// ---- the views ------------------------------------------------------------------------------

ViewController* fn_80016CFC(int nView);
void*  fn_80017004(int nView);          // the view's render camera
View*  fn_80017028(int nView);
void   fn_8001704C(int nView, int nPlayer);   // the player the view follows
void   fn_800170C4(int nView, int a);
f32*   fn_8001731C(View* pView);        // the camera's position (v0)
f32*   fn_80017314(View* pView);        // where it looks (v10)
u8     fn_800172C4(View* pView);        // the camera move has finished
f32*   fn_80012EF0(void* pCamera);      // a render camera's screen rectangle
void*  fn_8001614C(void);               // the current render camera
void   fn_80013CCC(void* pCamera);
void   fn_80013EEC(void* pCamera);
void   fn_80016B9C(void);
int    fn_80016D10(void);
void   fn_800171D8(f32* pRect, f32 x, f32 y, f32 w, f32 h);   // set a screen rectangle (fractions)
void   fn_8006434C(void* pCamera, f32* pPos, f32* pX, f32* pY, int a);   // a world position on screen (0..1)
void   fn_8006A8D4(void* pCamera, f32* pX, f32* pY);

// ---- camera shots and sequences (0x8003A7C8..) ----------------------------------------------

// A shot of kind nKind from the sequence, picked at random, and its blend values (each out
// pointer may be NULL).
CamShot* fn_8003A950(CamSequence* pSequence, int nKind, int* pA, f32* pF1, f32* pF2, int* pB, f32* pF3,
                     int nPlayer);
CamSequence* fn_8003BDBC(int nPlayer, int nLie, int nClass, int nKind, int a, f32 fDist);
u8     fn_8003DC78(CamShot* pShot);     // the shot's bAC is 1..6 or 7

// ---- the camera controller (0x80062F38..) ---------------------------------------------------

void   View_SetCamera(View* pView, int nCamera, int nPlayer, int nView);
void   fn_80062F1C(View* pView);
void   fn_80063B98(View* pView, f32 f, f32* pVec);
void   fn_80063BF4(View* pView, f32 f, f32* pVec);
u8     fn_80063C50(View* pView);
u8     fn_80063C7C(View* pView);
u8     fn_80063C90(View* pView);        // the camera is still moving
void   fn_80063CBC(View* pView, f32* pVec);   // nCamera 3, the vector into vC4
void   fn_80063CF0(View* pView, int nCamera, int nPlayer);
void   fn_800642D0(View* pView, int nPlayer);

// ---- the golf cameras (GoGolfCam.c) ---------------------------------------------------------

void   fn_800BD894(void);
void   fn_800BDA04(void);
void   fn_800C1790(View* pView, int nPlayer);
u8     fn_800C441C(View* pView, int nPlayer);
u8     fn_800C44A8(View* pView, int nPlayer);
u8     fn_800C44CC(View* pView, int nPlayer);
u8     fn_800C44E0(View* pView, int nPlayer);
int    fn_800C4518(View* pView);
u8     fn_800C4650(View* pView, int nPlayer);
void   fn_800C4E80(View* pView, int nPlayer);
void   fn_800C5CEC(View* pView, int nPlayer);
u8     fn_800C5FE4(View* pView, int nPlayer);
void   fn_800C6010(View* pView, int nPlayer);
void   fn_800C60E8(View* pView, int nPlayer);
void   GolfCamera_CutToGolferDoneAnimatingCam(View* pView, int nPlayer);
u8     fn_800C6604(View* pView);
void   fn_800C6618(View* pView, int nPlayer);
int    fn_800C6B38(View* pView);
f32    fn_800C6B7C(View* pView);        // the slow-motion rate for the swing camera kind
void   fn_800C6C8C(void);
u8     fn_800C6CB0(void);
u8     fn_800C6CCC(void);
u8     fn_800C6D28(void);
u8     fn_800C6D64(void);
u8     fn_800C6D80(void);
u8     fn_800C6D9C(void);
void   fn_800C6DE4(void);
void   fn_800C6DFC(void);
void   fn_800C6E14(void);
void   fn_800C6E2C(void);
u8     fn_800C6E88(View* pView, int nPlayer);
u8     fn_800C6F7C(View* pView, int nPlayer, f32 f);
void   fn_800C7080(View* pView);
void   fn_800C70F8(View* pView, int a);
u8     fn_800C7100(View* pView);
int    fn_800C7138(View* pView);
void   fn_800C7140(int a);
u8     fn_800C714C(void);
void   fn_800C7158(View* pView, int a);
u8     fn_800C7160(View* pView);
void   fn_800C7168(View* pView, int a);
u8     fn_800C7170(View* pView);
void   fn_800C7178(View* pView, int nPlayer);
u8     fn_800C71A4(View* pView, int nPlayer);
u8     fn_800C7340(View* pView, int nPlayer);

#endif
