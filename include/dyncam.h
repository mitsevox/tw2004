// dyncam.h (our name): the dynamic cameras' own types (GoDynamicCam.c): the tables of camera
// shots, sequences and shot choices loaded from the camera files. The shot and sequence types and
// the calls other files make are in camera.h.

#ifndef DYNCAM_H
#define DYNCAM_H

#include "camera.h"

// One shot a sequence can choose (0x48 bytes; TW07's CameraEventTrigger_t; CamSequence.pChoices
// points at nChoices of them).
typedef struct CamChoice {
    f32  fInterpTime;           // 0x00  } CameraScript_InterpToNewScript's blend time and the
    f32  fMaxSpeed;             // 0x04  } camera's top speed on the way
    f32  fTimeTriggerTime;      // 0x08  when nTimeTrigger fires
    f32  fC;                    // 0x0C  event 0x18: the ball-flight camera takes it once the flight has
                                //       run this far (CamScript_EstimateBallFlightPercent)
    CamShot* p10;               // 0x10  the shot (an index in the file)
    u8   nEvent;                // 0x14  the camera event it is for (9: any event but 23)
    u8   nInterpType;           // 0x15  how the script blends into the shot (CamShot.nBlendKind)
    u8   nTimeTrigger;          // 0x16  the camera event started at fTimeTriggerTime (13..22; else
                                //       25, none, on load)
    u8   b17;                    // 0x17  only for some golfers (DynamicCam_CanUseScriptOnThisModel)
    u32  aNoHoles[12];          // 0x18  one bit per hole of every course (course * 18 + hole): not
                                //       used there
} CamChoice;
LAYOUT_ASSERT(CamChoice, 0x48);

// An anim pair (0x28 bytes; TW07's CameraAnimPairs_t; DynCamTables.pSets): a golfer animation's
// name and a shot and four sequences, each an index in the file, -1 for none.
typedef struct DynCamSet {
    char szName[0x10];          // 0x00  DynamicCamSearchForPairedSequence finds a set by it (case ignored)
    CamShot* pShot;             // 0x10  kind 13: the set gives this shot
    CamSequence* p14;           // 0x14  } kind 14: the set gives one of these three at random
    CamSequence* p18;           // 0x18  }
    CamSequence* p1C;           // 0x1C  }
    CamSequence* p20;           // 0x20  taken first, 39 times in 100, when it has shot choices
    u8   nKind;                 // 0x24  13 or 14
    u8   unk25[3];
} DynCamSet;
LAYOUT_ASSERT(DynCamSet, 0x28);

// The dynamic cameras' tables (0x28 bytes, allocated by DynamicCam_Init): the shots and sequences
// loaded so far, and the block the sequences' choices are handed out from.
typedef struct DynCamTables {
    CamShot*     pShots;        // 0x00
    CamSequence* pSequences;    // 0x04
    DynCamSet*   pSets;         // 0x08
    CamChoice*   pChoices;      // 0x0C
    s32          nShots;        // 0x10
    s32          nSequences;    // 0x14
    s32          nSets;         // 0x18
    s32          n1C;           // 0x1C  counts the sequence and shot loads, 1..2
    s32          nChoicesUsed;  // 0x20
    u8           unk24[4];
} DynCamTables;
LAYOUT_ASSERT(DynCamTables, 0x28);

extern DynCamTables* gpDynCam;
extern s32 gMaterialToCamLies[20];            // DynamicCam_MaterialToCameraLie's table
extern s32 gBallLieToCamLies[17];            // DynamicCam_BallLieToCameraLie's table

// GoDynamicCam.c: register and unregister the camera files' stream handlers.
void DynamicCam_RegisterStreamClients(void);
void DynamicCam_UnRegisterStreamClients(void);
void DynamicCam_RegisterStreamClientsFE(void);
void DynamicCam_UnRegisterStreamClientsFE(void);

#endif
