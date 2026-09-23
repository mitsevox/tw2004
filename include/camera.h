// camera.h (our name): the golf cameras (GoGolfCam.c) and what they drive: camera shots and
// sequences, a view's camera script, the cameras' shared state.

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
typedef struct CrAPModel {
    u8   unk0[0x34];
    s32  n34;                   // 0x34  1: the shot names get an 'f' in front
} CrAPModel;
typedef struct CrAPGolfer {
    u8   unk0[8];
    CrAPModel* p8;              // 0x08
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

#endif
