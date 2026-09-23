// camera.h (our name): the views on screen and their cameras: the view structs, camera shots and
// sequences, a view's camera script, the camera tuning values, the golf cameras' (GoGolfCam.c)
// shared state, and the view and camera functions.

#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"

// A render camera's lens (our name; unsorted/cull.h called it CameraSub): what fn_80008370 returns,
// the pointer at the render camera's +0x10. Only the fields read so far; its size is unknown.
typedef struct CamLens {
    s32  nType;                 // 0x00  0: a perspective camera, else flat (LLObj_Gc.c)
    f32  v4[3];                 // 0x04  a position: the green zoom-to-aim camera copies it to View.v20
    u8   unk10[0x34 - 0x10];
    f32  v34[3];                // 0x34  a position: GameMode8 measures the ball's distance to it
    u8   unk40[0xB0 - 0x40];
    f32  fB0;                   // 0xB0  fn_8001EFFC; the zoom-to-aim camera divides its distance by it
    f32  fB4;                  // 0xB4  a flat camera's view width (guess)
    f32  fB8;                   // 0xB8  its view height (guess)
} CamLens;

// A camera shot (0xC0 bytes): a named script position the camera script moves to. The shots of a
// sequence are chained through p40.
typedef struct CamShot {
    char szName[0x20];          // 0x00
    f32  v20[4];                // 0x20  a position ([1]: the elevator camera adds the course's height)
    f32  v30[4];                // 0x30  a second position (the super zoom's target, the zoom-to-aim
                                //       camera's start)
    struct CamShot* p40;       // 0x40
    struct CamShot* p44;        // 0x44  in View.shot19C: the shot camera 13 goes back to
    f32  f48;                   // 0x48  how long the shot lasts
    f32  f4C;                   // 0x4C
    u32  u50;                   // 0x50  on the CrAP screen: bit n for CrAPGolfer.nC = n up to 32
    u32  u54;                   // 0x54  ... bit n - 32 above that (fn_8003D294)
    u8   unk58[0x60 - 0x58];
    f32  f60;                   // 0x60
    f32  f64;                   // 0x64
    f32  f68;                   // 0x68
    f32  f6C;                   // 0x6C
    f32  f70;                   // 0x70
    f32  f74;                   // 0x74
    f32  f78;                   // 0x78
    f32  f7C;                   // 0x7C
    f32  f80;                   // 0x80
    f32  f84;                   // 0x84
    u8   unk88[4];
    f32  f8C;                   // 0x8C
    u8   unk90[4];
    f32  f94;                   // 0x94
    f32  f98;                   // 0x98
    f32  f9C;                   // 0x9C
    u8   unkA0[0xA4 - 0xA0];
    s32  nA4;                   // 0xA4
    u8   bA8;                   // 0xA8
    u8   bA9;                   // 0xA9  another shot's p40 leads here (fn_80039C5C)
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

// A camera sequence (DynamicCam's, 0x50 bytes): the shots a camera plan steps through, and the
// conditions it is picked on. Its shot choices (CamChoice) are in dyncam.h.
typedef struct CamSequence {
    char szName[0x20];          // 0x00  "DEF..." for a default sequence (fn_8003CAFC)
    struct CamSequence* p20;   // 0x20  the sequence that follows (an index in the file)
    f32  f24;                   // 0x24  fn_8003D0BC: picked for values from this ...
    f32  f28;                   // 0x28  ... to this
    f32  f2C;                   // 0x2C  fn_8003CD6C: picked for values from this ...
    f32  f30;                   // 0x30  ... to this
    u8   unk34[0x38 - 0x34];
    f32  f38;                   // 0x38  its length
    s32  nChoices;              // 0x3C  how many shot choices p4C holds
    u32  uCourses;              // 0x40  one bit per course it is used on
    u8   b44;                   // 0x44  its kind
    u8   b45;                   // 0x45  the clubs it is for (fn_8003CBE8)
    u8   b46;                   // 0x46  6: the ball-flight camera keeps one for shot kind 5
    u8   b47;                   // 0x47  0 humans, 1 CPU players, 2 not in a replay, 3 in a replay,
                                //       4 in a replay or a CPU player (fn_8003CEEC); the swing camera
                                //       starts its shot with blend 5, time 0 when it is nonzero
    u8   b48;                   // 0x48  0 single-view play outside modes 9 and 11 and fn_800E39F0;
                                //       1 split screen or modes 9 and 11; 2 fn_800E39F0 (fn_8003D140)
    u8   b49;                   // 0x49  a bit mask
    u8   b4A;                   // 0x4A  a bit mask
    s8   n4B;                   // 0x4B  one bit per value of fn_800D2B08
    struct CamChoice* p4C;      // 0x4C  its shot choices (dyncam.h)
} CamSequence;
LAYOUT_ASSERT(CamSequence, 0x50);

// A view's camera script (0x40 bytes at View + 0x84).
typedef struct CamScript {
    f32  v0[4];                 // 0x00  camera 4 puts the ball here
    f32  v10[4];                // 0x10  and the pin here
    f32  a20[8];                // 0x20  cleared with the rest by fn_80062E40
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
    f32      f5C;               // 0x05C
    f32      f60;               // 0x060  camera 4: the camera's height over the ball once it is in place
    f32      f64;               // 0x064
    f32      fFade;             // 0x068  how far the view has faded (the ball-in-flight state waits for 0.5)
    u8       bFade;             // 0x06C  the fade is on
    u8       unk6D[3];
    s32      nCurCamera;        // 0x070
    CamSequence* p74;           // 0x074  the camera sequence the shots are picked from
    CamSequence* p78;           // 0x078  the sequence before the post-shot cameras (p74 saved)
    CamSequence* p7C;           // 0x07C  the swing camera's sequence, kept for the replay
    CamShot* p80;               // 0x080
    CamScript script;           // 0x084  the camera script the camera functions drive
    f32      vC4[4];            // 0x0C4
    f32      vD4[4];            // 0x0D4  the ball-flight camera: where the shot should land (the aim, at
                                //        the club's full distance)
    u8       unkE4[0xF4 - 0xE4];
    f32      vF4[4];            // 0x0F4  (0, 0, 0, 1) when the view is set up (fn_80062E40)
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
    CamShot* p13C;              // 0x13C  the shot before (GolfCamera_CutToGolferDoneAnimatingCam)
    s32      n140;              // 0x140
    s32      nCamera;           // 0x144
    s32      n148;              // 0x148  the shot kind asked for
    s32      n14C;              // 0x14C  the shot kind last started
    u8       b150;              // 0x150
    u8       unk151[2];
    u8       b153;              // 0x153
    s32      n154;              // 0x154
    u8       unk158[4];
    f32      f15C;              // 0x15C  camera 8: the ground height it follows
    u8       unk160[4];
    s32      n164;              // 0x164  a shot kind for fn_8003A950 (25 = none)
    f32      f168;              // 0x168
    u8       b16C;              // 0x16C
    u8       unk16D[0x174 - 0x16D];
    f32      f174;              // 0x174  } set together by fn_800642A4
    f32      f178;              // 0x178  }
    u8       unk17C[0x18C - 0x17C];
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
    u8       b274;              // 0x274  the view is set up (fn_80016D18) and not yet shut down
    u8       bFlagOut;          // 0x275  the flagstick is out (set while every player on this view is on the
                                //        green, the fringe or holed): no pin collisions
    u8       unk276[2];
    f32      f278;              // 0x278  } the render camera's screen rectangle, saved by fn_800170F4 in
    f32      f27C;              // 0x27C  } reverse order (f284 is its first value) and put back by
    f32      f280;              // 0x280  } fn_80017158
    f32      f284;              // 0x284  }
} ViewController;
LAYOUT_ASSERT(ViewController, 0x288);

// The camera tuning values (GoGolfCam.c); only the fields read so far.
typedef struct CamTuning {
    f32  f0;                    // 0x000  the zoom-to-aim camera's base speed
    f32  f4;                    // 0x004  the zoom-to-aim camera's distance back from the target
    f32  f8;                    // 0x008  the zoom-to-aim camera slows down over this last distance
    f32  fC;                    // 0x00C  ... closer in when the ball is within this of it plus f4
    f32  f10;                   // 0x010  ... as a share of the ball's distance
    f32  f14;                   // 0x014  the zoom-to-aim camera's height over View.f15C
    f32  f18;                   // 0x018  the zoom-to-aim camera's creep and aim-marker lag
    f32  f1C;                   // 0x01C  both zoom-to-aim cameras' slow motion at full speed
    f32  f20;                   // 0x020  the zoom-to-aim camera's slow motion before it sets off
    f32  f24;                   // 0x024  both: the camera has arrived within this of its goal
    f32  f28;                   // 0x028  the zoom-to-aim camera's aim-marker lag at the start
    f32  f2C;                   // 0x02C  ... once there, its height's share of the move a frame
    f32  f30;                   // 0x030  ... it slows down over this last distance
    f32  f34;                   // 0x034  ... its base speed
    f32  f38;                   // 0x038  ... its distance back on a putt
    f32  f3C;                   // 0x03C  ... View.shot19C.f74 falls by this each frame
    f32  f40;                   // 0x040  ... once there, its height moves by this a frame
    f32  f44;                   // 0x044  ... its height over the goal (for a lens of fB0 1)
    f32  f48;                   // 0x048  ... the aim marker's lag
    f32  f4C;                   // 0x04C  camera 5's height over the ball
    f32  f50;                   // 0x050  camera 5: the ball-to-pin distance of a full swing out
    f32  f54;                   // 0x054  camera 5: how long the swing out lasts
    f32  f58;                   // 0x058  how long camera 13's closing shot lasts
    f32  f5C;                   // 0x05C  how long the matrix camera's first shot lasts (twice as long for
                                //        swing camera kinds other than 1 and 6)
    f32  f60;                   // 0x060  camera 13's slow-motion step length (0: every frame)
    f32  f64;                   // 0x064
    f32  v68[4];                // 0x068  camera 13's fn_80038010 vector; [3] shrinks as the camera's time
                                //        runs
    f32  f78;                   // 0x078  ... over this many seconds
    f32  f7C;                   // 0x07C  camera 13's fallback shots' f78/f7C: from this value ...
    f32  f80;                   // 0x080  ... to this one
    f32  f84;                   // 0x084  how long the super zoom's first shot lasts
    f32  f88;                   // 0x088
    f32  f8C;                   // 0x08C
    f32  f90;                   // 0x090  fn_800638B8 switches to camera 2 while the ball is below this
                                //        height (and falling, not yet bounced)
    f32  f94;                   // 0x094  the elevator camera's first blend value
    f32  f98;                   // 0x098  camera 8: 1 - this is its height's share of the move a frame
    f32  f9C;                   // 0x09C  the swing camera: the least shot power for one (fn_800C6618)
    f32  fA0;                   // 0x0A0  ... above this power, the chance (percent) is fAC
    f32  fA4;                   // 0x0A4  ... above this one, fB0 (else fA8)
    f32  fA8;                   // 0x0A8
    f32  fAC;                   // 0x0AC
    f32  fB0;                   // 0x0B0
    f32  fB4;                   // 0x0B4  ... above this power with a short club, always kind 11
    f32  fB8;                   // 0x0B8  camera 15 waits this long on a ball near the green
    s32  nBeats;                // 0x0BC  the heartbeat camera's beats
    s32  nBeatFrames;           // 0x0C0
    f32  fC4;                   // 0x0C4
    f32  fC8;                   // 0x0C8
    f32  fCC;                   // 0x0CC
    u8   unkD0[0xDC - 0xD0];
    f32  fDC;                   // 0x0DC  the green zoom-to-aim camera's aim marker (CameraScript_LagAimMarker)
    u8   unkE0[0x168 - 0xE0];
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
    f32  f1D0;                  // 0x1D0  the steep-slope camera: height step per try (down going up, up going down)
    f32  f1D4;                  // 0x1D4  ... distance step back per try
    f32  f1D8;                  // 0x1D8  ... first distance back from the ball
    s32  n1DC;                  // 0x1DC  ... most tries
    f32  fSlopeUp;              // 0x1E0
    f32  fSlopeDown;            // 0x1E4
    f32  f1E8;                  // 0x1E8  the steep-slope camera's base: x offset from the ball
    f32  f1EC;                  // 0x1EC  ... height over the ball
    f32  f1F0;                  // 0x1F0  ... how far the camera may move per call
    f32  f1F4;                  // 0x1F4  ... how far the aim may move per call
    f32  fMaxPitchUp;           // 0x1F8  fn_800C4AB0: the steepest camera angle above the horizontal (degrees)
    f32  fMaxPitchDown;         // 0x1FC  and below it
    u8   unk200[0x20C - 0x200];
    f32  f20C;                  // 0x20C  camera 4: the most View.f54 grows to
    f32  f210;                  // 0x210  camera 4: how fast (per second) it moves in from its start
    f32  f214;                  // 0x214  camera 4: how fast View.f54 grows and shrinks
    f32  f218;                  // 0x218  camera 4: how fast (per second) buttons 0x31/0x32 turn it round
    f32  f21C;                  // 0x21C  camera 4: the angle of its swing between the ball and the pin
    f32  f220;                  // 0x220  camera 4: its ground clearance, and its least height over the ball
    f32  f224;                  // 0x224  camera 4: (f20C - 1) times this raises the aim each frame ...
    s32  n228;                  // 0x228  ... unless this is set: then the aim is at the camera's height
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
    u8      b5D;                // 0x05D  cleared by the swing camera
    u8      unk5E[2];
    s32     n60;                // 0x060  passed to fn_8006509C
    f32     f64;                // 0x064  camera 7's slow-motion rate while b5A is set
    f32     f68;                // 0x068
    CamShot shot6C;             // 0x06C  the tutorial wait's two hand-made shots (camera 18)
    CamShot shot12C;            // 0x12C
    s32     n1EC[5];            // 0x1EC  per player: the next swing camera kind (View.n260) to use, 1..11 in turn
} GolfCamState;

// The golfer shown on the menu screens (FEgolferanim.c): the create-a-player (CrAP) screen's
// state at lbl_80281EE0 (0x1E0 bytes, allocated by fn_8008AD80). The golfers are kept in a ring
// of CRAP_NUM_GOLFERS slots (the code is written for more than one; the game uses one).
#define CRAP_NUM_GOLFERS 1

typedef struct CrAPGolfer {
    struct CrAPGolfer* pPrev;   // 0x00  } the ring
    struct CrAPGolfer* pNext;   // 0x04  }
    struct Character* pChar;    // 0x08  the golfer being edited (character.h)
    s32  nC;                    // 0x0C  the golfer's id (-1: none)
    s32  n10;                   // 0x10
    s32  n14;                   // 0x14  nC once its stream was opened
    u8   b18;                   // 0x18  the camera script runs
    u8   b19;                   // 0x19
    u8   unk1A[2];
    s32  n1C;                   // 0x1C
} CrAPGolfer;

typedef struct CrAPState {
    s32  n0;                    // 0x000  0..4: picks the shot the CrAP camera frames (fn_800C39A8)
    s32  n4;                    // 0x004  the CrAP camera's kind (GolfCamera_SwitchCrAPCamera)
    s32  n8;                    // 0x008
    s32  nC;                    // 0x00C
    char sz10[0x10];            // 0x010  the animation fn_8008E468 started
    char sz20[0x10];            // 0x020
    char sz30[0x20];            // 0x030
    s32  n50;                   // 0x050
    char sz54[0x20];            // 0x054
    s32  n74;                   // 0x074
    u8   b78;                   // 0x078
    u8   unk79[3];
    f32  f7C;                   // 0x07C
    u8   b80;                   // 0x080
    u8   b81;                   // 0x081
    u8   b82;                   // 0x082
    u8   b83;                   // 0x083  set by a menu message (FE_MessageTable.c)
    u8   b84;                   // 0x084
    u8   b85;                   // 0x085
    u8   b86;                   // 0x086  set by a menu message; a change while n0 is 3 calls
                                //        fn_8008E354
    u8   b87;                   // 0x087
    u8   b88;                   // 0x088
    u8   b89;                   // 0x089
    u8   b8A;                   // 0x08A
    u8   unk8B;
    s32  n8C;                   // 0x08C  the golfer id whose stream was opened last (-1: none)
    u8   b90;                   // 0x090
    u8   b91;                   // 0x091
    u8   unk92[2];
    CrAPGolfer aGolfer[CRAP_NUM_GOLFERS];   // 0x094
    CrAPGolfer* pB4;            // 0x0B4  the golfer shown
    CrAPGolfer* pB8;            // 0x0B8  the golfer being loaded
    s32  nBC;                   // 0x0BC  which of pB4, its pNext or its pPrev pB8 is (0..2)
    f32  mC0[4][4];             // 0x0C0
    f32  v100[4];               // 0x100
    f32  v110[4];               // 0x110
    f32  v120[4];               // 0x120
    f32  v130[4];               // 0x130
    f32  f140;                  // 0x140
    f32  f144;                  // 0x144
    f32  f148;                  // 0x148
    f32  f14C;                  // 0x14C
    u8   unk150[0x18C - 0x150];
    u8   b18C;                  // 0x18C
    u8   unk18D[3];
    s32  n190;                  // 0x190  counts the golfers loaded
    s32  n194;                  // 0x194  } the next golfer to show: a column and row of
    s32  n198;                  // 0x198  } lbl_801899E0
    f32  f19C;                  // 0x19C
    f32  f1A0;                  // 0x1A0
    u8   unk1A4[0x1B0 - 0x1A4];
    u8   b1B0;                  // 0x1B0
    u8   unk1B1[3];
    s32  n1B4;                  // 0x1B4
    s32  n1B8;                  // 0x1B8
    s32  n1BC;                  // 0x1BC
    s32  n1C0;                  // 0x1C0
    s32  n1C4;                  // 0x1C4
    u8   b1C8;                  // 0x1C8
    u8   unk1C9[3];
    f32  f1CC;                  // 0x1CC
    s8   n1D0;                  // 0x1D0
    u8   b1D1;                  // 0x1D1
    u8   b1D2;                  // 0x1D2
    u8   unk1D3;
    s32  n1D4;                  // 0x1D4
    s32  n1D8;                  // 0x1D8
    u8   b1DC;                  // 0x1DC
    u8   unk1DD[3];
} CrAPState;
LAYOUT_ASSERT(CrAPState, 0x1E0);

extern CrAPState* lbl_80281EE0;

// ---- the views ------------------------------------------------------------------------------

// Points at the slot holding the current render camera (lbl_80281C90): fn_8001614C reads it,
// fn_80013D5C sets it.
extern void** lbl_80280DF0;

ViewController* fn_80016CFC(int nView);
void*  fn_80017004(int nView);          // the view's render camera
View*  fn_80017028(int nView);
void   fn_8001704C(int nView, int nPlayer);   // the player the view follows
int    fn_8001707C(int nView);          // the player the view follows (as set above)
u8     fn_800170A0(int nView);          // the view is in use
void   fn_800170C4(int nView, u8 b);    // sets ViewController.b274
f32*   fn_8001731C(View* pView);        // the camera's position (v0)
f32*   fn_80017314(View* pView);        // where it looks (v10)
u8     fn_800172C4(View* pView);        // the camera move has finished
f32*   fn_80012EF0(void* pCamera);      // a render camera's screen rectangle
f32    fn_80012ED0(f32* pRect);         // the rectangle's [3]: its height
f32    fn_80012ED8(f32* pRect);         // [2]: its width
f32    fn_80012EE0(f32* pRect);         // [1]: its top
f32    fn_80012EE8(f32* pRect);         // [0]: its left
void*  fn_8001614C(void);               // the current render camera
void   fn_80013CCC(void* pCamera);
void   fn_80013EEC(void* pCamera);
void   fn_80016B9C(void);
int    fn_80016D10(void);
void   fn_800171D8(f32* pRect, f32 x, f32 y, f32 w, f32 h);   // set a screen rectangle (fractions)
// A world position on screen (0..1 across and down; pZ, if not NULL, gets a third value). Returns
// 1, or 0 when one of its tests fails (not decompiled yet).
u8     fn_8006434C(void* pCamera, f32* pPos, f32* pX, f32* pY, f32* pZ);
void   fn_8006A8D4(void* pCamera, f32* pX, f32* pY);

// ---- camera shots and sequences (0x8003A7C8..) ----------------------------------------------

CamShot* fn_8003A7C8(int nPlayer, int nKind, CamShot* pShot);
CamShot* fn_8003A8C4(char* szName);     // the shot with this name (case ignored), or NULL
// A shot of kind nKind from the sequence, picked at random, and its blend values (each out
// pointer may be NULL).
CamShot* fn_8003A950(CamSequence* pSequence, int nKind, int* pA, f32* pF1, f32* pF2, int* pB, f32* pF3,
                     int nPlayer);
CamSequence* fn_8003BDBC(int nPlayer, int nLie, int nClass, int nKind, int a, f32 fDist);
CamSequence* DynamicCam_ChoosePreFlightSequence(int nPlayer, int nLie, int nKind);
u8       fn_8003C9D0(int nPlayer, int a, CamSequence** ppSeq, CamShot** ppShot);
u8       fn_8003D7A0(CamSequence* pSequence, int nPlayer);   // it suits the player's club and shot
u8     fn_8003DC78(CamShot* pShot);     // the shot's bAC is 1..6 or 7
// 0 when gSession.nGameType is 3, else fn_8001EDF4 of the player's golfer (Player.pChar) as a flag;
// the shot is not read.
u8     fn_800453C8(int nPlayer, CamShot* pShot);

// ---- the camera scripts (gocamscripts.c, 0x8003DCE8..) ----------------------------------------

void     fn_8003DCE8(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003E624(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003EA50(int nPlayer, void* pCam, void* pSub, void* pScript, CamShot* pShot, int a,
                     f32 fFrameTime);
void     fn_8003F2E0(void* pScript, f32 fTime);
void     CameraScript_RecordCurrentCam(CamShot* pShot, void* pCam, void* pSub, int nPlayer, void* pScript,
                                       int a);
void     CameraScript_InterpToNewScript(void* pScript, CamShot* pShot, int nPlayer, void* pCam, void* pSub,
                                        int nA, f32 f1, f32 f2, int nB, f32 f3);
u8       CameraScript_IsDefaultSwingCam(CamShot* pShot, int nPlayer, f32* pCam);
// Keep pNew above the ground (by fClearance); the out values are optional (NULL): two flags and a
// float.
u8       CamScript_KeepAboveGround(int nPlayer, f32* pNew, f32* pOld, int a, u8* pb1, f32* pf, u8* pb2,
                                   f32 fClearance);
u8       CameraScript_WillGolferBeOccludedInThisView(int nPlayer, CamShot* pShot, void* pScript);

// ---- camera script helpers (gocamscripts, 0x800C7480..) -------------------------------------

// Not decompiled yet. Both write a point into pOut: fn_800C7D14 from the direction between pA and
// pB (its y cleared unless b1, normalised unless b2), a distance f and an angle; fn_800C7E50 from
// three points and fT.
void   fn_800C7D14(f32* pA, f32* pB, u8 b1, u8 b2, f32* pOut, f32 f, f32 fAngle);
void   fn_800C7E50(f32* pA, f32* pB, f32* pC, int n, f32* pOut, f32 fT);

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
u8     fn_800642B0(void);               // fn_800C6CB0's answer (gomainloop tests it)
void   fn_80063F08(f32* pA, f32* pB, f32* pOut);   // the green zoom-to-aim camera: View.v20 as pA and pOut

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
// Every caller passes a sixth argument (0 or 1) that the camera does not read.
void   GolfCamera_SwitchCrAPCamera(View* pView, char* szName, int nShot, u8 bBlend, u8 bForce, int n6);
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

// ---- frame buffers (GoFrameBuf.c) -----------------------------------------------------------

// A frame buffer's size and scale (0x34 bytes; fn_8006E1C8 makes one). The last seven fields are
// worked out from the first six by fn_8006E150.
typedef struct GoFrameBuf {
    f32  f0;                    // 0x00  0 by default
    f32  f4;                    // 0x04  0 by default
    f32  fWidth;                // 0x08  512 by default
    f32  fHeight;               // 0x0C  448 by default
    f32  f10;                   // 0x10  a horizontal scale, 1 by default
    f32  f14;                   // 0x14  a vertical scale, 1 by default
    f32  f18;                   // 0x18  fWidth * f10
    f32  f1C;                   // 0x1C  fHeight * f14
    f32  f20;                   // 0x20  fWidth / f10
    f32  f24;                   // 0x24  fHeight / f14
    f32  f28;                   // 0x28  1 / f10
    f32  f2C;                   // 0x2C  1 / f14
    f32  fAspect;               // 0x30  fHeight / fWidth
} GoFrameBuf;
LAYOUT_ASSERT(GoFrameBuf, 0x34);

void        fn_8006E150(GoFrameBuf* pBuf);   // work out the derived fields
GoFrameBuf* fn_8006E1C8(void);               // a new frame buffer with the default size
void        fn_8006E214(GoFrameBuf* pBuf);   // free it
void        fn_8006E234(GoFrameBuf* pBuf);   // the default size: 512 x 448, scale 1
void        fn_8006E26C(GoFrameBuf* pBuf, f32 f0, f32 f4, f32 fWidth, f32 fHeight, f32 f10, f32 f14);

#endif
