// dyncam.h (our name): the dynamic cameras' own types (GoDynamicCam.c): the tables of camera
// shots, sequences and shot choices loaded from the camera files. The shot and sequence types and
// the calls other files make are in camera.h.

#ifndef DYNCAM_H
#define DYNCAM_H

#include "camera.h"

// One shot a sequence can choose (0x48 bytes; CamSequence.p4C points at nChoices of them).
typedef struct CamChoice {
    f32  f0;                    // 0x00  the blend's first time (CameraScript_InterpToNewScript's f1)
    f32  f4;                    // 0x04  its second (f2)
    f32  f8;                    // 0x08  its third (f3)
    f32  fC;                    // 0x0C  kind 0x18: the ball-flight camera takes it once the flight has
                                //       run this far (fn_80044EA8)
    CamShot* p10;               // 0x10  the shot (an index in the file)
    u8   b14;                   // 0x14  the shot kind it is for (9: any kind but 23)
    u8   b15;                   // 0x15  the blend's first kind (nA)
    u8   b16;                   // 0x16  its second (nB)
    u8   b17;                    // 0x17  only for some golfers (fn_8003ABEC)
    u32  aNoHoles[12];          // 0x18  one bit per hole of every course (course * 18 + hole): not
                                //       used there
} CamChoice;
LAYOUT_ASSERT(CamChoice, 0x48);

// The dynamic cameras' tables (0x28 bytes, allocated by fn_80039FF8): the shots and sequences
// loaded so far, and the block the sequences' choices are handed out from.
typedef struct DynCamTables {
    CamShot*     pShots;        // 0x00
    CamSequence* pSequences;    // 0x04
    void*        p8;            // 0x08  0x28-byte records: a shot and four sequences each
    CamChoice*   pChoices;      // 0x0C
    s32          nShots;        // 0x10
    s32          nSequences;    // 0x14
    s32          n18;           // 0x18  how many records p8 holds
    s32          n1C;           // 0x1C  counts the sequence and shot loads, 1..2
    s32          nChoicesUsed;  // 0x20
    u8           unk24[4];
} DynCamTables;
LAYOUT_ASSERT(DynCamTables, 0x28);

extern DynCamTables* lbl_80281D88;
extern s32 lbl_80187988[20];            // fn_8003CB80's table
extern s32 lbl_801879D8[17];            // fn_8003CBD4's table

#endif
