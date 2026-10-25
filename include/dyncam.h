// dyncam.h: the dynamic cameras' own tables (GoDynamicCam.c), loaded from the camera files: the
// shots, the sequences with their shot choices, and the anim pairs. The shot and sequence types and
// the dynamic cameras' functions are in camera.h.

#ifndef DYNCAM_H
#define DYNCAM_H

#include "camera.h"

// A shot a sequence can choose for a camera event (0x48 bytes; CameraSequence_t.pChoices points at
// nChoices of them).
typedef struct CameraEventTrigger_t {
    f32  fInterpTime;           // 0x00  } CameraScript_InterpToNewScript's blend time and the
    f32  fMaxSpeed;             // 0x04  } camera's top speed on the way
    f32  fTimeTriggerTime;      // 0x08  the script time (fScriptTime) after which nTimeTrigger is asked for
    f32  fFlightShare;          // 0x0C  a choice for event 0x18: GolfCamera_ProcessBallFlightCamera takes
                                //       the one with the largest share below how far the ball's flight
                                //       has run (CamScript_EstimateBallFlightPercent)
    CamShot* pShot;             // 0x10  the shot it plays (an index in the file)
    u8   nEvent;                // 0x14  the camera event it answers (CameraEvent_e; CAMEVENT_ANY: any
                                //       event but CAMEVENT_PRESHOT_TIMED)
    u8   nInterpType;           // 0x15  how the script blends into the shot (CameraInterpType_e)
    u8   nTimeTrigger;          // 0x16  the event asked for after fTimeTriggerTime:
                                //       CAMEVENT_TIMED_FIRST..CAMEVENT_TIMED_LAST, and only one that a
                                //       choice of the same sequence answers; else CAMEVENT_NONE (set on load)
    u8   bModelLimited;         // 0x17  only for some golfer models (DynamicCam_CanUseScriptOnThisModel)
    u32  aNoHoles[12];          // 0x18  one bit per hole of every course (course * 18 + hole): set
                                //       where the choice is not used (DynamicCam_CanUseScriptOnThisHole)
} CameraEventTrigger_t;
LAYOUT_ASSERT(CameraEventTrigger_t, 0x48);

// What an anim pair gives when its pChanceSeq is not taken (CameraAnimPairs_t.nKind).
typedef enum CamAnimPairKind {
    CAMPAIR_SHOT = 13,          // its shot, pShot
    CAMPAIR_RANDOM_SEQUENCE = 14    // one of its pRandomSeq1..3 at random
} CamAnimPairKind;

// An anim pair (0x28 bytes; DynamicCamManager_t.pAnimPairs): a golfer animation's name and the shot
// or sequences shown for it (DynamicCamSearchForPairedSequence). The file holds an index for each
// pointer, negative for none.
typedef struct CameraAnimPairs_t {
    char szName[0x10];          // 0x00  the animation's name (matched with case ignored)
    CamShot* pShot;             // 0x10  CAMPAIR_SHOT: the pair gives this shot
    CameraSequence_t* pRandomSeq1;  // 0x14  } CAMPAIR_RANDOM_SEQUENCE: the pair gives one of these three
    CameraSequence_t* pRandomSeq2;  // 0x18  } at random (pRandomSeq3 when the one picked is missing)
    CameraSequence_t* pRandomSeq3;  // 0x1C  }
    CameraSequence_t* pChanceSeq;   // 0x20  tried first: given 39 times in 100 when it has shot choices
    u8   nKind;                 // 0x24  what it gives otherwise (CamAnimPairKind; any other: nothing)
    u8   unk25[3];
} CameraAnimPairs_t;
LAYOUT_ASSERT(CameraAnimPairs_t, 0x28);

// The dynamic cameras' tables (0x28 bytes, allocated by DynamicCam_Init): what the camera files
// brought, and the block the sequences' shot choices sit in.
typedef struct DynamicCamManager_t {
    CamShot*              pShots;       // 0x00  the shots ('CAMV')
    CameraSequence_t*     pSequences;   // 0x04  the sequences ('CAMS')
    CameraAnimPairs_t*    pAnimPairs;   // 0x08  the anim pairs ('CAMA')
    CameraEventTrigger_t* pChoices;     // 0x0C  every sequence's shot choices, in sequence order
    s32                   nShots;       // 0x10
    s32                   nSequences;   // 0x14
    s32                   nAnimPairs;   // 0x18
    s32                   nFilesLoaded; // 0x1C  counts the 'CAMS' and 'CAMV' loads: 1, 2, then 1 again
                                        //       (at 2 both files are in)
    s32                   nChoicesUsed; // 0x20  the choices of pChoices handed out to the sequences so far
    u8                    unk24[4];
} DynamicCamManager_t;
LAYOUT_ASSERT(DynamicCamManager_t, 0x28);

extern DynamicCamManager_t* gpDynCam;
extern CameraLie_e gBallLieToCamLies[17];   // DynamicCam_BallLieToCameraLie's table

#endif
