// GoDynamicCam.c: the dynamic cameras. Loads the camera files (the 'CAMS' sequences, 'CAMV' shots and
// 'CAMA' anim pairs), picks the sequence and the shot for each moment of a golf shot, and places the
// camera each frame by the shot's tracking mode, following the ball or the golfer.

#include "golfer.h"
#include "game.h"
#include "dyncam.h"
#include "charstate.h"
#include "frontend/fe.h"
#include "endian.h"

// The most shots or sequences one pick chooses among.
#define DYNAMICCAM_MAX_POSSIBLE 50

DynamicCamManager_t* gpDynCam;    // the loaded shots, sequences, anim pairs and choices (DynamicCam_Init)

static void DynamicCam_LoadFilesFromDisk(void);
static void DynamicCam_LoadFilesFromDiskFE(void);
static void DynamicCam_LoadCAMSfromStream(UStreamObject* pStreamData);
static void DynamicCam_LoadCAMVfromStream(UStreamObject* pStreamData);
static void DynamicCam_LoadCAMVfromStreamFE(UStreamObject* pStreamData);
static void DynamicCam_LoadCAMAfromStream(UStreamObject* pStreamData);
static void DynamicCam_CopySequenceData(u8* src, u8* dest, int numSeqs);
static void DynamicCam_CopyScriptData(u8* src, CamShot* dest, u32 numScripts);
static void DynamicCam_CopyAnimPairData(u8* src, CameraAnimPairs_t* dest, u32 numPairs);
static void DynamicCam_ParseCameraViews(int size);
static void DynamicCam_ParseCameraViewsFE(int size);
static void DynamicCam_ParseCameraSeqs(int numSeqs);
static void DynamicCam_ParseNextSeqs(void);
static void DynamicCam_ParseAnimPairs(int size);
static u8   DynamicCam_CanUseScriptOnThisHole(CameraEventTrigger_t* trigger);
static u8   DynamicCam_CanUseScriptOnThisModel(CameraEventTrigger_t* trigger, int playerID);
static void DynamicCam_TrackPercent(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                    f32* position, f32* lookAt, f32* origCamPos);
static void DynamicCam_TrackOffset(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                   f32* position, f32* lookAt, f32* origCamPos);
static void DynamicCam_TrackBallVelocityLag(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                            f32* position, f32* lookAt, f32* origCamPos, f32 timeStep);
static void DynamicCam_TrackBallVelocityTight(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                              f32* position, f32* lookAt, f32* origCamPos);
static void DynamicCam_TrackFixed(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                  f32* position, f32* lookAt, f32* origCamPos);
static void DynamicCam_GetLocation(CameraTracking_e tracking, int ePlayerID, f32* position,
                                   CameraScriptSettings_t* settings, CamShot* link, f32* lookAt,
                                   f32* origCamPos);
static u8   DynamicCamSearchForPairedSequence(char* animName, CameraSequence_t** pSequence, CamShot** pLink);
static u8   DynamicCam_IsDefualtSeq(CameraSequence_t* seq, CameraShotState_e state);
static CameraLie_e DynamicCam_MaterialToCameraLie(u32 lieID, int playerID);
static CameraLie_e DynamicCam_BallLieToCameraLie(Lie_t lie, int playerID);
static u8   DynamicCam_MatchSeqClub(CameraSequence_t* seq, int ePlayerID);
void DynamicCam_Vec3Add(f32* pA, f32* pB, f32* pOut);         // a + b
void DynamicCam_Vec3Sub(f32* pA, f32* pB, f32* pOut);
u8   DynamicCam_MatchStateType(CameraSequence_t* pSequence, int nKind);
u8   DynamicCam_MatchScriptStateType(CamShot* pShot, int nKind);
u8   DynamicCam_MatchPlayer(CamShot* pShot);
void DynamicCam_AddOffset(f32* pPos, f32* pDir, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                          f32 fSide, f32 fY);
void DynamicCam_AddHeightOffset(f32* pPos, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                                f32 fY);
f32  DynamicCam_fAdjustMaxHeightByHole(f32 f);
void DynamicCam_ClampBallVelocity(f32* pDir, f32* pA, f32* pB);
u8   DynamicCam_MatchHeightDiff(CameraSequence_t* pSequence, int nPlayer, f32 f);
u8   DynamicCam_MatchSeqShot(CameraSequence_t* pSequence, int nPlayer, u8 b);
u8   DynamicCam_MatchPlayerType(CameraSequence_t* pSequence, int nPlayer);
u8   DynamicCam_MatchCourse(CameraSequence_t* pSequence);
u8   DynamicCam_MatchPars(CameraSequence_t* pSequence);
u8   DynamicCam_MatchLies(int nMask, int nBit);
u8   DynamicCam_ModeType(CameraSequence_t* pSequence);
u8   DynamicCam_MatchDistToPin(CameraSequence_t* pSequence, int nPlayer, f32 f);
void DynamicCam_CalculateFlippedOffsets(CamShot* pShot, int nPlayer, f32* pA, f32* pB);

// Has the round's stream (StreamManagerIngame_RegisterStreamClients) hand this file its camera
// files: 'CAMS' the sequences, 'CAMV' the shots, 'CAMA' the anim pairs.
void DynamicCam_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CAMS', DynamicCam_LoadCAMSfromStream);
    Stream_RegisterLoadChunkCallback('CAMV', DynamicCam_LoadCAMVfromStream);
    Stream_RegisterLoadChunkCallback('CAMA', DynamicCam_LoadCAMAfromStream);
}

// Takes the three handlers back, then calls DynamicCam_LoadFilesFromDisk, which does nothing.
void DynamicCam_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CAMS');
    Stream_UnregisterLoadChunkCallback('CAMV');
    Stream_UnregisterLoadChunkCallback('CAMA');
    DynamicCam_LoadFilesFromDisk();
}

// The front end's stream (StreamManagerFrontend_RegisterStreamClients) brings only the shots,
// 'CAMV' (DynamicCam_LoadCAMVfromStreamFE).
void DynamicCam_RegisterStreamClientsFE(void) {
    Stream_RegisterLoadChunkCallback('CAMV', DynamicCam_LoadCAMVfromStreamFE);
}

// Takes the front end's handler back, then calls DynamicCam_LoadFilesFromDiskFE, which does nothing.
void DynamicCam_UnRegisterStreamClientsFE(void) {
    Stream_UnregisterLoadChunkCallback('CAMV');
    DynamicCam_LoadFilesFromDiskFE();
}

static void DynamicCam_LoadFilesFromDisk(void) {
}

static void DynamicCam_LoadFilesFromDiskFE(void) {
}

// The 'CAMS' handler, the sequence file: two counts (the sequences, then the shot choices of all of
// them), then the sequences, each followed by its choices (DynamicCam_CopySequenceData). The
// sequences' follow-ons become pointers (DynamicCam_ParseNextSeqs); the choices' shots do once the
// shot file is in too (DynamicCam_ParseCameraSeqs). Each 'CAMS' or 'CAMV' load counts nFilesLoaded
// on. Frees the object; a second sequence file is dropped.
static void DynamicCam_LoadCAMSfromStream(UStreamObject* pStreamData) {
    s32 numSeqsToLoad;
    s32 numTriggersToLoad;
    u8* srcPtr;

    gpDynCam->nFilesLoaded++;
    if (gpDynCam->nFilesLoaded > 2) {
        gpDynCam->nFilesLoaded = 1;
    }
    if (gpDynCam->pSequences != NULL) {
        StaticMem_Free(pStreamData);
        return;
    }
    srcPtr = pStreamData->pData;
    BYTESWAP_SWAPDATA(&srcPtr, (u8*)&numSeqsToLoad, sizeof(numSeqsToLoad), 4);
    srcPtr = pStreamData->pData + 4;
    BYTESWAP_SWAPDATA(&srcPtr, (u8*)&numTriggersToLoad, sizeof(numTriggersToLoad), 4);
    gpDynCam->pSequences =
        StaticMem_Alloc(numSeqsToLoad * sizeof(CameraSequence_t), 2, 0, "GoDynamicCam.c", 403);
    gpDynCam->pChoices =
        StaticMem_Alloc(numTriggersToLoad * sizeof(CameraEventTrigger_t), 2, 0, "GoDynamicCam.c", 404);
    gpDynCam->nSequences = 0;
    gpDynCam->nChoicesUsed = 0;
    srcPtr = pStreamData->pData + 8;
    DynamicCam_CopySequenceData(srcPtr, (u8*)gpDynCam->pSequences, numSeqsToLoad);
    DynamicCam_ParseCameraSeqs(numSeqsToLoad);
    DynamicCam_ParseNextSeqs();
    StaticMem_Free(pStreamData);
}

// The round's 'CAMV' handler, the shot file: copies the shots (DynamicCam_CopyScriptData) and sets
// them up (DynamicCam_ParseCameraViews). Counts nFilesLoaded on, as 'CAMS' does. Frees the object;
// a second shot file is dropped.
static void DynamicCam_LoadCAMVfromStream(UStreamObject* pStreamData) {
    gpDynCam->nFilesLoaded++;
    if (gpDynCam->nFilesLoaded > 2) {
        gpDynCam->nFilesLoaded = 1;
    }
    if (gpDynCam->pShots != NULL) {
        StaticMem_Free(pStreamData);
        return;
    }
    gpDynCam->pShots = StaticMem_Alloc(pStreamData->uSize, 2, 0, "GoDynamicCam.c", 454);
    DynamicCam_CopyScriptData(pStreamData->pData, gpDynCam->pShots, pStreamData->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViews(pStreamData->uSize);
    StaticMem_Free(pStreamData);
}

// The front end's 'CAMV' handler: as DynamicCam_LoadCAMVfromStream, but nFilesLoaded is left alone
// and the shots are set up by DynamicCam_ParseCameraViewsFE.
static void DynamicCam_LoadCAMVfromStreamFE(UStreamObject* pStreamData) {
    if (gpDynCam->pShots != NULL) {
        StaticMem_Free(pStreamData);
        return;
    }
    gpDynCam->pShots = StaticMem_Alloc(pStreamData->uSize, 2, 0, "GoDynamicCam.c", 495);
    DynamicCam_CopyScriptData(pStreamData->pData, gpDynCam->pShots, pStreamData->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViewsFE(pStreamData->uSize);
    StaticMem_Free(pStreamData);
}

// The 'CAMA' handler, the anim pairs: copies them (DynamicCam_CopyAnimPairData) and turns their
// indexes into pointers (DynamicCam_ParseAnimPairs). Frees the object; a second file is dropped.
static void DynamicCam_LoadCAMAfromStream(UStreamObject* pStreamData) {
    if (gpDynCam->pAnimPairs != NULL) {
        StaticMem_Free(pStreamData);
        return;
    }
    gpDynCam->pAnimPairs = StaticMem_Alloc(pStreamData->uSize, 2, 0, "GoDynamicCam.c", 536);
    DynamicCam_CopyAnimPairData(pStreamData->pData, gpDynCam->pAnimPairs,
                                pStreamData->uSize / sizeof(CameraAnimPairs_t));
    DynamicCam_ParseAnimPairs(pStreamData->uSize);
    StaticMem_Free(pStreamData);
}

// Copies numSeqs sequences from the file (little-endian) into dest, swapping each value's bytes.
// Each is followed in the file by its shot choices, which go into the next free choices of
// gpDynCam->pChoices; the sequence's pChoices points at the first of them. The file keeps a word
// where pChoices goes.
static void DynamicCam_CopySequenceData(u8* src, u8* dest, int numSeqs) {
    SwapField seqElmDef[] = {
        { 32, 1 },                                          // szName
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
    };
    SwapField triggerElmDef[] = {
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
        { 48, 4 },                                          // aNoHoles
    };
    CameraEventTrigger_t* triggerDest;
    int i;

    for (i = 0; i < numSeqs; i++) {
        ByteSwap_Records((void**)&src, (void**)&dest, seqElmDef, sizeof(seqElmDef) / sizeof(seqElmDef[0]),
                         1);
        // dest is now at the sequence's pChoices
        *(CameraEventTrigger_t**)dest = &gpDynCam->pChoices[gpDynCam->nChoicesUsed];
        src += sizeof(CameraEventTrigger_t*);
        dest += sizeof(CameraEventTrigger_t*);
        triggerDest = &gpDynCam->pChoices[gpDynCam->nChoicesUsed];
        ByteSwap_Records((void**)&src, (void**)&triggerDest, triggerElmDef,
                         sizeof(triggerElmDef) / sizeof(triggerElmDef[0]), gpDynCam->pSequences[i].nChoices);
        gpDynCam->nChoicesUsed += gpDynCam->pSequences[i].nChoices;
    }
    gpDynCam->nSequences = numSeqs;
}

// Copies numScripts shots from the file (little-endian) into dest, swapping each value's bytes.
static void DynamicCam_CopyScriptData(u8* src, CamShot* dest, u32 numScripts) {
    SwapField viewElmDef[] = {
        { 32, 1 },                                          // szName
        { 16, 4 }, { 16, 4 },                               // v20, v30
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 },
        { 1, 1 }, { 1, 1 },
        { 13, 1 },
    };

    ByteSwap_Records((void**)&src, (void**)&dest, viewElmDef, sizeof(viewElmDef) / sizeof(viewElmDef[0]),
                     numScripts);
}

// Copies numPairs anim pairs from the file (little-endian) into dest, swapping each value's bytes.
static void DynamicCam_CopyAnimPairData(u8* src, CameraAnimPairs_t* dest, u32 numPairs) {
    SwapField pairElmDef[] = {
        { 16, 1 },
        { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
        { 1, 1 },
        { 3, 1 },
    };

    ByteSwap_Records((void**)&src, (void**)&dest, pairElmDef, sizeof(pairElmDef) / sizeof(pairElmDef[0]),
                     numPairs);
}

static f32 DynamicCam_OrderConstants(f32 x) {   // fake match: never called, pools 1.0f before 0.0f
    return x + 1.0f;
}

// Sets up size bytes of shots just loaded in a round: as DynamicCam_ParseCameraViewsFE, but each
// shot's fMinHeight is kept at least the tuning's fGroundClearance, its fMaxHeight at least
// fMinHeight, and its fBlur within 0..0.49. When the sequence file is in too (nFilesLoaded is 2),
// the sequences' choices get their shots (DynamicCam_ParseCameraSeqs).
static void DynamicCam_ParseCameraViews(int size) {
    int i;

    gpDynCam->nShots = 0;
    gpDynCam->nShots = size / sizeof(CamShot);
    for (i = 0; i < gpDynCam->nShots; i++) {
        // the file holds the index of the shot that follows; its own index for none
        if (i == (s32)gpDynCam->pShots[i].pNext) {
            gpDynCam->pShots[i].pNext = NULL;
        } else {
            gpDynCam->pShots[i].pNext = &gpDynCam->pShots[(s32)gpDynCam->pShots[i].pNext];
            gpDynCam->pShots[i].pNext->bHasPrev = 1;
        }
        if (gpDynCam->pShots[i].fMinHeight < gpCamTuning->fGroundClearance) {
            gpDynCam->pShots[i].fMinHeight = gpCamTuning->fGroundClearance;
        }
        if (gpDynCam->pShots[i].fMaxHeight < gpDynCam->pShots[i].fMinHeight) {
            gpDynCam->pShots[i].fMaxHeight = gpDynCam->pShots[i].fMinHeight;
        }
        gpDynCam->pShots[i].fBlur = (gpDynCam->pShots[i].fBlur < 0.0f) ? 0.0f
            : ((gpDynCam->pShots[i].fBlur > 0.49f) ? 0.49f : gpDynCam->pShots[i].fBlur);
        gpDynCam->pShots[i].fFovEnd = gpDynCam->pShots[i].fFovStart;
    }
    if (gpDynCam->nFilesLoaded == 2) {
        DynamicCam_ParseCameraSeqs(gpDynCam->nSequences);
    }
}

// Sets up size bytes of shots just loaded: each shot's pNext becomes a pointer to the shot that
// follows (NULL for none), and that shot is marked bHasPrev; fMaxHeight is set to fMinHeight and
// fFovEnd to fFovStart.
static void DynamicCam_ParseCameraViewsFE(int size) {
    int i;

    gpDynCam->nShots = 0;
    gpDynCam->nShots = size / sizeof(CamShot);
    for (i = 0; i < gpDynCam->nShots; i++) {
        // the file holds the index of the shot that follows; its own index for none
        if (i == (s32)gpDynCam->pShots[i].pNext) {
            gpDynCam->pShots[i].pNext = NULL;
        } else {
            gpDynCam->pShots[i].pNext = &gpDynCam->pShots[(s32)gpDynCam->pShots[i].pNext];
            gpDynCam->pShots[i].pNext->bHasPrev = 1;
        }
        gpDynCam->pShots[i].fMaxHeight = gpDynCam->pShots[i].fMinHeight;
        gpDynCam->pShots[i].fFovEnd = gpDynCam->pShots[i].fFovStart;
    }
}

// Keeps the sequence count and, once both the sequence and the shot file are in (nFilesLoaded is
// 2), sets up every sequence's shot choices: the shot index becomes a pointer, and the time trigger
// becomes CAMEVENT_NONE unless it is one of CAMEVENT_TIMED_FIRST..CAMEVENT_TIMED_LAST that some
// choice of the same sequence answers.
static void DynamicCam_ParseCameraSeqs(int numSeqs) {
    int i;
    int j;
    int k;
    u8 matchFound;

    gpDynCam->nSequences = numSeqs;
    if (gpDynCam->nFilesLoaded != 2) {
        return;
    }
    for (i = 0; i < gpDynCam->nSequences; i++) {
        for (j = 0; j < gpDynCam->pSequences[i].nChoices; j++) {
            // the file holds the shot's index
            gpDynCam->pSequences[i].pChoices[j].pShot =
                &gpDynCam->pShots[(s32)gpDynCam->pSequences[i].pChoices[j].pShot];
            if (gpDynCam->pSequences[i].pChoices[j].nTimeTrigger < CAMEVENT_TIMED_FIRST
                || gpDynCam->pSequences[i].pChoices[j].nTimeTrigger > CAMEVENT_TIMED_LAST) {
                gpDynCam->pSequences[i].pChoices[j].nTimeTrigger = CAMEVENT_NONE;
            }
            if (gpDynCam->pSequences[i].pChoices[j].nTimeTrigger >= CAMEVENT_TIMED_FIRST
                && gpDynCam->pSequences[i].pChoices[j].nTimeTrigger <= CAMEVENT_TIMED_LAST) {
                matchFound = 0;
                for (k = 0; k < gpDynCam->pSequences[i].nChoices; k++) {
                    if (gpDynCam->pSequences[i].pChoices[j].nTimeTrigger
                        == gpDynCam->pSequences[i].pChoices[k].nEvent) {
                        matchFound = 1;
                    }
                }
                if (!matchFound) {
                    gpDynCam->pSequences[i].pChoices[j].nTimeTrigger = CAMEVENT_NONE;
                }
            }
        }
    }
}

// Turns each sequence's follow-on (pNext, an index in the file) into a pointer; a sequence whose
// follow-on has no shot choices follows itself.
static void DynamicCam_ParseNextSeqs(void) {
    int i;

    for (i = 0; i < gpDynCam->nSequences; i++) {
        gpDynCam->pSequences[i].pNext = &gpDynCam->pSequences[(s32)gpDynCam->pSequences[i].pNext];
        if (gpDynCam->pSequences[i].pNext->nChoices <= 0) {
            gpDynCam->pSequences[i].pNext = &gpDynCam->pSequences[i];
        }
    }
}

// Sets up size bytes of anim pairs just loaded: each shot and sequence index becomes a pointer into
// the loaded tables, NULL for a negative index.
static void DynamicCam_ParseAnimPairs(int size) {
    int i;

    gpDynCam->nAnimPairs = size / sizeof(CameraAnimPairs_t);
    for (i = 0; i < gpDynCam->nAnimPairs; i++) {
        if ((s32)gpDynCam->pAnimPairs[i].pShot >= 0) {
            gpDynCam->pAnimPairs[i].pShot = &gpDynCam->pShots[(s32)gpDynCam->pAnimPairs[i].pShot];
        } else {
            gpDynCam->pAnimPairs[i].pShot = NULL;
        }
        if ((s32)gpDynCam->pAnimPairs[i].pRandomSeq1 >= 0) {
            gpDynCam->pAnimPairs[i].pRandomSeq1 =
                &gpDynCam->pSequences[(s32)gpDynCam->pAnimPairs[i].pRandomSeq1];
        } else {
            gpDynCam->pAnimPairs[i].pRandomSeq1 = NULL;
        }
        if ((s32)gpDynCam->pAnimPairs[i].pRandomSeq2 >= 0) {
            gpDynCam->pAnimPairs[i].pRandomSeq2 =
                &gpDynCam->pSequences[(s32)gpDynCam->pAnimPairs[i].pRandomSeq2];
        } else {
            gpDynCam->pAnimPairs[i].pRandomSeq2 = NULL;
        }
        if ((s32)gpDynCam->pAnimPairs[i].pRandomSeq3 >= 0) {
            gpDynCam->pAnimPairs[i].pRandomSeq3 =
                &gpDynCam->pSequences[(s32)gpDynCam->pAnimPairs[i].pRandomSeq3];
        } else {
            gpDynCam->pAnimPairs[i].pRandomSeq3 = NULL;
        }
        if ((s32)gpDynCam->pAnimPairs[i].pChanceSeq >= 0) {
            gpDynCam->pAnimPairs[i].pChanceSeq =
                &gpDynCam->pSequences[(s32)gpDynCam->pAnimPairs[i].pChanceSeq];
        } else {
            gpDynCam->pAnimPairs[i].pChanceSeq = NULL;
        }
    }
}

// Allocates the dynamic cameras' tables (gpDynCam), empty. Called as the front end and as a round
// start (GO_vInitFE, GO_vInitIG).
void DynamicCam_Init(void) {
    DynamicCamManager_t* pManager = StaticMem_Alloc(sizeof(DynamicCamManager_t), 2, 0, "GoDynamicCam.c", 938);

    gpDynCam = pManager;
    pManager->pSequences = NULL;
    gpDynCam->pShots = NULL;
    gpDynCam->pAnimPairs = NULL;
    gpDynCam->pChoices = NULL;
    gpDynCam->nShots = 0;
    gpDynCam->nSequences = 0;
    gpDynCam->nAnimPairs = 0;
    gpDynCam->nChoicesUsed = 0;
}

// Frees what the camera files brought and the tables themselves, after clearing the counts and
// nFilesLoaded.
void DynamicCam_DeInit(void) {
    gpDynCam->nShots = 0;
    gpDynCam->nSequences = 0;
    gpDynCam->nAnimPairs = 0;
    gpDynCam->nChoicesUsed = 0;
    if (gpDynCam->pSequences != NULL) {
        StaticMem_Free(gpDynCam->pSequences);
        gpDynCam->pSequences = NULL;
    }
    if (gpDynCam->pShots != NULL) {
        StaticMem_Free(gpDynCam->pShots);
        gpDynCam->pShots = NULL;
    }
    if (gpDynCam->pAnimPairs != NULL) {
        StaticMem_Free(gpDynCam->pAnimPairs);
        gpDynCam->pAnimPairs = NULL;
    }
    if (gpDynCam->pChoices != NULL) {
        StaticMem_Free(gpDynCam->pChoices);
        gpDynCam->pChoices = NULL;
    }
    gpDynCam->nFilesLoaded = 0;
    StaticMem_Free(gpDynCam);
}

// Places a dynamic shot's camera at position for this frame, by the shot's tracking mode
// (CamTrackMode; timeStep only for the lagging ball-flight camera), then keeps it between the
// shot's least and most height over the ground:
// - The ground: on a shot's first frame (CameraScript_SnapToScript) it is measured under the camera
//   and kept in settings->fGroundHeight (the course's floor when there is none); on course 12's hole
//   10, within 40 of the player's tee, the lowest ground there (Ter_GetLowestGroundHeight), else
//   CamScript_GuessBestPlayableHeight, for a ball-flight shot that is neither a swing camera nor
//   looks at the golfer at least the lower of the pin and the tee. The next shot and later frames
//   take fGroundHeight. Without course data it is 0, or in the front end (game type 3) the shot's
//   fMinHeight, the camera put there.
// - The heights: fMinHeight and fMaxHeight (the latter by DynamicCam_fAdjustMaxHeightByHole); a
//   swing camera's or one looking at the golfer are over the ball's spot (Player.vBall) instead,
//   and at least the tuning's fGroundClearance.
// - Too low, the ball-flight cameras (CAMTRACK_BALL_LAG..CAMTRACK_BALL_TIGHT) rise by the tuning's
//   fRiseStep a frame when they were too low already, never above the least height nor below
//   fGroundClearance; the others go straight to the least height. Above the most height less the
//   tuning's fHeightCeilingEase (over fGroundHeight) the camera is eased so that it nears the most
//   height without passing it.
// A CAMTRACK_OFFSET_FLAT_LAGGED camera then trails its place, except on a shot's first frame: it
// stays no more than half its fDist behind, and closes a smaller gap more slowly.
void DynamicCam_ProcessScript(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                              f32* position, f32* lookAt, f32* origCamPos, f32 timeStep) {
    f32 origPosition[4];
    f32 origToNewPos[4];
    f32 vTeeOffset[4];
    f32 origToNewPosNormalized[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    CourseInfo* pTerrainGameData;
    int nPinSet;
    f32 fActualGroundHeight;
    f32 minTerrainHeight;
    f32 minHeight;
    f32 maxHeight;
    f32 heightDiff;
    f32 fTop;
    f32 fEase;
    f32 fAbove;
    f32 fGroundHeight;
    f32 origToNewPosLength;
    f32 maxLag;
    f32 scaleAmount;
    f32 fOver;

    nPinSet = Game_CurrentPinSet();
    Vec3Copy(position, origPosition);
    switch (link->nTrackMode) {
    case CAMTRACK_PERCENT_FLAT:
    case CAMTRACK_PERCENT:
        DynamicCam_TrackPercent(link, ePlayerID, settings, position, lookAt, origCamPos);
        break;
    case CAMTRACK_OFFSET_FLAT:
    case CAMTRACK_OFFSET_FLAT_LAGGED:
    case CAMTRACK_OFFSET:
    case CAMTRACK_OFFSET_TO_PIN:
        DynamicCam_TrackOffset(link, ePlayerID, settings, position, lookAt, origCamPos);
        break;
    case CAMTRACK_BALL_LAG:
    case CAMTRACK_BALL_LAG_FLAT:
        DynamicCam_TrackBallVelocityLag(link, ePlayerID, settings, position, lookAt, origCamPos, timeStep);
        break;
    case CAMTRACK_BALL_TIGHT:
        DynamicCam_TrackBallVelocityTight(link, ePlayerID, settings, position, lookAt, origCamPos);
        break;
    case CAMTRACK_FIXED:
        DynamicCam_TrackFixed(link, ePlayerID, settings, position, lookAt, origCamPos);
        break;
    }
    pTerrainGameData = Ter_GetTGD();
    if (pTerrainGameData != NULL) {
        if (CameraScript_SnapToScript(settings, link) && link != settings->pNextShot) {
            if (Game_GetCourse() == 12 && Game_GetCurHoleNum() == 10) {
                DynamicCam_Vec3Sub(&pTerrainGameData->tee[gSession.nTeeSet[ePlayerID]].x, position,
                                   vTeeOffset);
                vTeeOffset[1] = 0.0f;
                if ((f32)Math_Sqrt(LLMath_LengthSquared3(vTeeOffset)) < 40.0f) {
                    fActualGroundHeight = Ter_GetLowestGroundHeight(pTerrainGameData, position);
                } else {
                    fActualGroundHeight = CamScript_GuessBestPlayableHeight(position, NULL);
                    if (link->nStateType == CAMSTATE_BALL_FLIGHT && !DynamicCam_bIsSwingCamera(link)
                        && !CamScript_DoesScriptTrackGolfer(link)) {
                        minTerrainHeight = (pTerrainGameData->pin[nPinSet].y
                                            <= pTerrainGameData->tee[gSession.nTeeSet[ePlayerID]].y)
                                               ? pTerrainGameData->pin[nPinSet].y
                                               : pTerrainGameData->tee[gSession.nTeeSet[ePlayerID]].y;
                        fActualGroundHeight =
                            (minTerrainHeight
                             <= fActualGroundHeight) ? fActualGroundHeight : minTerrainHeight;
                    }
                }
                settings->fGroundHeight = fActualGroundHeight;
                if (settings->fGroundHeight < -60000.0f) {
                    settings->fGroundHeight = Ter_GetTGD()->fFloor;
                }
            } else {
                fActualGroundHeight = CamScript_GuessBestPlayableHeight(position, NULL);
                if (link->nStateType == CAMSTATE_BALL_FLIGHT && !DynamicCam_bIsSwingCamera(link)
                    && !CamScript_DoesScriptTrackGolfer(link)) {
                    minTerrainHeight = (pTerrainGameData->pin[nPinSet].y
                                        <= pTerrainGameData->tee[gSession.nTeeSet[ePlayerID]].y)
                                           ? pTerrainGameData->pin[nPinSet].y
                                           : pTerrainGameData->tee[gSession.nTeeSet[ePlayerID]].y;
                    fActualGroundHeight =
                        (minTerrainHeight <= fActualGroundHeight) ? fActualGroundHeight : minTerrainHeight;
                }
                settings->fGroundHeight = fActualGroundHeight;
                if (settings->fGroundHeight < -60000.0f) {
                    settings->fGroundHeight = Ter_GetTGD()->fFloor;
                }
            }
        } else {
            fActualGroundHeight = settings->fGroundHeight;
        }
    } else if (gSession.nGameType == 3) {
        fActualGroundHeight = link->fMinHeight;
        position[1] = fActualGroundHeight;
    } else {
        fActualGroundHeight = 0.0f;
    }
    if (fActualGroundHeight < -60000.0f) {
        fActualGroundHeight = Ter_GetLowestGroundHeight(pTerrainGameData, position);
    }
    if (DynamicCam_bIsSwingCamera(link) || CamScript_DoesScriptTrackGolfer(link)) {
        heightDiff = gPlayers[ePlayerID].vBall[1] - fActualGroundHeight;
        minHeight = link->fMinHeight + heightDiff;
        maxHeight = link->fMaxHeight + heightDiff;
        if (minHeight < gpCamTuning->fGroundClearance) {
            minHeight = gpCamTuning->fGroundClearance;
        }
        if (maxHeight < gpCamTuning->fGroundClearance) {
            maxHeight = gpCamTuning->fGroundClearance;
        }
    } else {
        minHeight = link->fMinHeight;
        maxHeight = DynamicCam_fAdjustMaxHeightByHole(link->fMaxHeight);
    }
    if (position[1] - fActualGroundHeight < minHeight) {
        if (link->nTrackMode == CAMTRACK_BALL_LAG || link->nTrackMode == CAMTRACK_BALL_LAG_FLAT
            || link->nTrackMode == CAMTRACK_BALL_TIGHT) {
            if (!(origPosition[1] - fActualGroundHeight >= minHeight)) {
                position[1] += gpCamTuning->fRiseStep;
                if (position[1] - fActualGroundHeight > minHeight) {
                    position[1] = fActualGroundHeight + minHeight;
                }
            }
            if (position[1] - fActualGroundHeight < gpCamTuning->fGroundClearance) {
                position[1] = fActualGroundHeight + gpCamTuning->fGroundClearance;
            }
        } else {
            position[1] = fActualGroundHeight + minHeight;
        }
    } else {
        fGroundHeight = settings->fGroundHeight;
        fEase = gpCamTuning->fHeightCeilingEase;
        fAbove = position[1] - fGroundHeight;
        fTop = maxHeight - fEase;
        if (fAbove > fTop) {
            fOver = fAbove - fTop;
            fOver = fEase * (1.0f - fEase / (fOver + fEase));
            position[1] = fOver + (fGroundHeight + fTop);
            if (position[1] - fActualGroundHeight < minHeight) {
                position[1] = fActualGroundHeight + minHeight;
            }
        }
    }
    if (link->nTrackMode == CAMTRACK_OFFSET_FLAT_LAGGED && !CameraScript_SnapToScript(settings, link)
        && (settings->pNextShot != link || settings->fCamTime > 0.0f)) {
        DynamicCam_Vec3Sub(position, origPosition, origToNewPos);
        origToNewPosLength = (f32)Math_Sqrt(LLMath_LengthSquared3(origToNewPos));
        if (origToNewPos[0] != 0.0f || origToNewPos[1] != 0.0f || origToNewPos[2] != 0.0f) {
            LLMath_Normalize3(origToNewPos, origToNewPosNormalized);
        } else {
            origToNewPosNormalized[0] = 0.0f;
            origToNewPosNormalized[1] = 0.0f;
            origToNewPosNormalized[2] = 0.0f;
        }
        maxLag = fabsf(link->fDist) / 2.0f;
        if (origToNewPosLength > maxLag) {
            scaleAmount = origToNewPosLength - maxLag;
        } else {
            scaleAmount = origToNewPosLength * (1.0f - (maxLag - origToNewPosLength) / maxLag);
            scaleAmount = scaleAmount * scaleAmount;
        }
        Vec3_Scale(scaleAmount, origToNewPosNormalized, origToNewPos);
        DynamicCam_Vec3Add(origPosition, origToNewPos, position);
    }
}

// The shot is a swing camera's: one of the pre-shot, swing, replay swing, obstructed, alternate and
// matrix swing cameras' shots or the comic panels'. DynamicCam_ProcessScript keeps such a camera's
// heights over the ball's spot instead of the ground; CamScript_RunScript keeps it off the flagstick
// check.
u8 DynamicCam_bIsSwingCamera(CamShot* camLink) {
    u8 nState = camLink->nStateType;

    if (nState == CAMSTATE_SWING || nState == CAMSTATE_PRE_SHOT
        || (nState >= CAMSTATE_ALT_SWING_1 && nState <= CAMSTATE_ALT_SWING_5)
        || (nState >= CAMSTATE_COMIC_3SCREEN && nState <= CAMSTATE_COMIC_9SCREEN + 2)
        || nState == CAMSTATE_REPLAY_SWING || nState == CAMSTATE_SWING_OBSTRUCTED
        || nState == CAMSTATE_MATRIX_SWING) {
        return 1;
    }
    return 0;
}

// A random shot for the state (DynamicCam_MatchScriptStateType), from the first
// DYNAMICCAM_MAX_POSSIBLE that DynamicCam_MatchPlayer allows, that no other shot leads to
// (bHasPrev) and that are not dontPick; NULL when there is none. ePlayerID is not used.
CamShot* DynamicCam_ChooseScript(int ePlayerID, int state, CamShot* dontPick) {
    int possibleScripts[DYNAMICCAM_MAX_POSSIBLE];
    int* pNextPossible = possibleScripts;
    int scriptsFound = 0;
    int i;

    for (i = 0; i < gpDynCam->nShots; i++) {
        if (scriptsFound >= DYNAMICCAM_MAX_POSSIBLE) break;
        if (DynamicCam_MatchScriptStateType(&gpDynCam->pShots[i], state)
            && DynamicCam_MatchPlayer(&gpDynCam->pShots[i])
            && !gpDynCam->pShots[i].bHasPrev && &gpDynCam->pShots[i] != dontPick) {
            *pNextPossible++ = i;
            scriptsFound++;
        }
    }
    if (scriptsFound == 0) return NULL;
    i = Misc_RandFunc(1) % scriptsFound;
    return &gpDynCam->pShots[possibleScripts[i]];
}

// The shot with this name (case ignored), or NULL.
CamShot* DynamicCam_ChooseScriptByName(char* camName) {
    int i;

    for (i = 0; i < gpDynCam->nShots; i++) {
        if (stricmp(camName, gpDynCam->pShots[i].szName) == 0) {
            return &gpDynCam->pShots[i];
        }
    }
    return NULL;
}

// A shot for the event from the sequence's choices: one at random of the first
// DYNAMICCAM_MAX_POSSIBLE that answer it (a CAMEVENT_ANY choice answers every event but
// CAMEVENT_PRESHOT_TIMED) and may be used on this hole (DynamicCam_CanUseScriptOnThisHole) and for
// this golfer (DynamicCam_CanUseScriptOnThisModel); with none, of the first that answer it at all.
// The choice's blend goes to each out pointer that is not NULL: interp its nInterpType, time its
// fInterpTime, speed its fMaxSpeed, timeTrigger its nTimeTrigger, timeTriggerTime its
// fTimeTriggerTime. NULL for a NULL sequence or when no choice answers the event.
CamShot* DynamicCam_ChooseScriptInSequence(CameraSequence_t* sequence, CameraEvent_e event, int* interp,
                                           f32* time, f32* speed, int* timeTrigger, f32* timeTriggerTime,
                                           int playerID) {
    int possibleScripts[DYNAMICCAM_MAX_POSSIBLE];
    int i;
    int scriptsFound = 0;
    u32 randomNum;

    if (sequence == NULL) return NULL;
    for (i = 0; i < sequence->nChoices; i++) {
        if (scriptsFound >= DYNAMICCAM_MAX_POSSIBLE) break;
        if ((event == sequence->pChoices[i].nEvent
             || (event != CAMEVENT_PRESHOT_TIMED && sequence->pChoices[i].nEvent == CAMEVENT_ANY))
            && DynamicCam_CanUseScriptOnThisHole(&sequence->pChoices[i])
            && DynamicCam_CanUseScriptOnThisModel(&sequence->pChoices[i], playerID)) {
            possibleScripts[scriptsFound] = i;
            scriptsFound++;
        }
    }
    if (scriptsFound == 0) {
        for (i = 0; i < sequence->nChoices; i++) {
            if (scriptsFound >= DYNAMICCAM_MAX_POSSIBLE) break;
            if (event == sequence->pChoices[i].nEvent
                || (event != CAMEVENT_PRESHOT_TIMED && sequence->pChoices[i].nEvent == CAMEVENT_ANY)) {
                possibleScripts[scriptsFound] = i;
                scriptsFound++;
            }
        }
    }
    if (scriptsFound == 0) return NULL;
    randomNum = Misc_RandFunc(1) % scriptsFound;
    if (interp != NULL) {
        *interp = sequence->pChoices[possibleScripts[randomNum]].nInterpType;
    }
    if (time != NULL) {
        *time = sequence->pChoices[possibleScripts[randomNum]].fInterpTime;
    }
    if (speed != NULL) {
        *speed = sequence->pChoices[possibleScripts[randomNum]].fMaxSpeed;
    }
    if (timeTrigger != NULL) {
        *timeTrigger = sequence->pChoices[possibleScripts[randomNum]].nTimeTrigger;
    }
    if (timeTriggerTime != NULL) {
        *timeTriggerTime = sequence->pChoices[possibleScripts[randomNum]].fTimeTriggerTime;
    }
    return sequence->pChoices[possibleScripts[randomNum]].pShot;
}

// The choice may be used on the current hole: its aNoHoles bit for it (Game_GetCourse() * 18 +
// Game_GetCurHoleNum()) is clear.
static u8 DynamicCam_CanUseScriptOnThisHole(CameraEventTrigger_t* trigger) {
    if (BitArray_TestBit(trigger->aNoHoles, Game_GetCourse() * 18 + Game_GetCurHoleNum())) return 0;
    return 1;
}

// The choice suits the player's golfer: a bModelLimited choice is only for golfer models
// (golfer.nModelID) 0, 1, 8, 10, 12..15 and 17.
static u8 DynamicCam_CanUseScriptOnThisModel(CameraEventTrigger_t* trigger, int playerID) {
    int nModel = gPlayers[playerID].golfer.nModelID;

    if (trigger->bModelLimited) {
        if (nModel == 0 || nModel == 1 || nModel == 8 || nModel == 10 || nModel == 12 || nModel == 13
            || nModel == 14 || nModel == 15 || nModel == 17) {
            return 1;
        }
        return 0;
    }
    return 1;
}

// CAMTRACK_PERCENT_FLAT and CAMTRACK_PERCENT: the camera at share fDist of the way from the shot's
// first point (DynamicCam_GetLocation for nFromPoint) to its second (nToPoint), the way taken level
// for CAMTRACK_PERCENT_FLAT, then fSideOffset to the side (the other way where
// CameraScript_FlipCameraForLefty says so), then its height by DynamicCam_AddHeightOffset (from the
// height it had). A shot with a ball point (CAMPOINT_BALL, CAMPOINT_BALL_AHEAD) leaves the camera
// where it is until a frame in which the ball updates (GameEffects_BallUpdatesThisFrame).
static void DynamicCam_TrackPercent(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                    f32* position, f32* lookAt, f32* origCamPos) {
    f32 object1Location[4];
    f32 object2Location[4];
    f32 origHeight = position[1];

    if ((link->nFromPoint == CAMPOINT_BALL || link->nToPoint == CAMPOINT_BALL
         || link->nFromPoint == CAMPOINT_BALL_AHEAD || link->nToPoint == CAMPOINT_BALL_AHEAD)
        && GameEffects_BallUpdatesThisFrame(ePlayerID) < 1) {
        return;
    }
    DynamicCam_GetLocation(link->nFromPoint, ePlayerID, object1Location, settings, link, lookAt, origCamPos);
    DynamicCam_GetLocation(link->nToPoint, ePlayerID, object2Location, settings, link, lookAt, origCamPos);
    if (link->nTrackMode == CAMTRACK_PERCENT_FLAT) {
        if (CameraScript_FlipCameraForLefty(ePlayerID, link)) {
            CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 0, 1, position,
                                                  link->fDist,
                                                  -link->fSideOffset);
        } else {
            CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 0, 1, position,
                                                  link->fDist,
                                                  link->fSideOffset);
        }
    } else if (CameraScript_FlipCameraForLefty(ePlayerID, link)) {
        CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 1, 1, position, link->fDist,
                                              -link->fSideOffset);
    } else {
        CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 1, 1, position, link->fDist,
                                              link->fSideOffset);
    }
    DynamicCam_AddHeightOffset(position, settings, link, ePlayerID, origHeight);
}

// CAMTRACK_OFFSET_FLAT, CAMTRACK_OFFSET_FLAT_LAGGED, CAMTRACK_OFFSET and CAMTRACK_OFFSET_TO_PIN: the
// camera refOffset along the direction from the shot's first point (DynamicCam_GetLocation for
// nFromPoint) to its second (nToPoint), taken level for the first two, and sideOffset to the side (the
// shot's fDist and fSideOffset, as DynamicCam_CalculateFlippedOffsets flips them for a lefty), then
// its height by DynamicCam_AddHeightOffset (from the height it had). For CAMTRACK_OFFSET_TO_PIN, once
// the share of the way to the pin (CamScript_GetBallToPinPercent, never less than the most seen, kept
// in the settings' fMaxBallToPin) passes the tuning's fTrackOffsetStartShare, the camera's level move
// from origCamPos (its place before this frame) is scaled from the tuning's fTrackOffsetStartScale
// down to fTrackOffsetEndScale at fTrackOffsetEndShare, and its height becomes origCamPos's; on a
// shot's first frame (CameraScript_SnapToScript) fMaxBallToPin just takes the share. Waits, like
// DynamicCam_TrackPercent, for a frame in which the ball updates when a point is the ball.
static void DynamicCam_TrackOffset(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                   f32* position, f32* lookAt, f32* origCamPos) {
    f32 object1Location[4];
    f32 object2Location[4];
    f32 curToNewPosition[4];
    f32 sideOffset;
    f32 refOffset;
    f32 origHeight = position[1];
    f32 ballToPinPercent;
    f32 scaleAmount;

    if ((link->nFromPoint == CAMPOINT_BALL || link->nToPoint == CAMPOINT_BALL
         || link->nFromPoint == CAMPOINT_BALL_AHEAD || link->nToPoint == CAMPOINT_BALL_AHEAD)
        && GameEffects_BallUpdatesThisFrame(ePlayerID) < 1) {
        return;
    }
    DynamicCam_GetLocation(link->nFromPoint, ePlayerID, object1Location, settings, link, lookAt, origCamPos);
    DynamicCam_GetLocation(link->nToPoint, ePlayerID, object2Location, settings, link, lookAt, origCamPos);
    if (link->nTrackMode == CAMTRACK_OFFSET_FLAT || link->nTrackMode == CAMTRACK_OFFSET_FLAT_LAGGED) {
        DynamicCam_CalculateFlippedOffsets(link, ePlayerID, &sideOffset, &refOffset);
        CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 0, 0, position, refOffset,
                                              sideOffset);
    } else {
        DynamicCam_CalculateFlippedOffsets(link, ePlayerID, &sideOffset, &refOffset);
        CamUtils_vGetPositionBetweenTwoPoints(object1Location, object2Location, 1, 0, position, refOffset,
                                              sideOffset);
    }
    if (link->nTrackMode == CAMTRACK_OFFSET_TO_PIN) {
        ballToPinPercent = CamScript_GetBallToPinPercent(ePlayerID, settings);
        if (!CameraScript_SnapToScript(settings, link)) {
            if (ballToPinPercent > settings->fMaxBallToPin) {
                settings->fMaxBallToPin = ballToPinPercent;
            }
            ballToPinPercent = settings->fMaxBallToPin;
            DynamicCam_Vec3Sub(position, origCamPos, curToNewPosition);
            curToNewPosition[1] = 0.0f;
            if (ballToPinPercent > gpCamTuning->fTrackOffsetStartShare) {
                if (ballToPinPercent > gpCamTuning->fTrackOffsetEndShare) {
                    scaleAmount = gpCamTuning->fTrackOffsetEndScale;
                } else {
                    scaleAmount = (ballToPinPercent - gpCamTuning->fTrackOffsetStartShare)
                            / (gpCamTuning->fTrackOffsetEndShare - gpCamTuning->fTrackOffsetStartShare);
                    scaleAmount = gpCamTuning->fTrackOffsetStartScale - scaleAmount
                            * (gpCamTuning->fTrackOffsetStartScale - gpCamTuning->fTrackOffsetEndScale);
                }
                Vec3_Scale(scaleAmount, curToNewPosition, curToNewPosition);
                DynamicCam_Vec3Add(curToNewPosition, origCamPos, position);
            }
        } else {
            settings->fMaxBallToPin = ballToPinPercent;
        }
    }
    DynamicCam_AddHeightOffset(position, settings, link, ePlayerID, origHeight);
}

// CAMTRACK_BALL_LAG and CAMTRACK_BALL_LAG_FLAT, the lagging ball-flight camera. Its target is offset
// along the ball's flight direction (taken level for CAMTRACK_BALL_LAG_FLAT, kept within the tuning's
// fMaxFlightTilt of level by DynamicCam_ClampBallVelocity) from the ball (CAMPOINT_BALL), then the
// shot's fSideOffset to the side (DynamicCam_AddOffset). offset is the shot's fDist, less the
// tuning's fAboveMaxDistShare times how far the ball is above the shot's fMaxHeight over the
// settings' fGroundHeight, or fBelowMinDistShare times how far below fMinHeight (the settings'
// current shot's heights while the next shot has the same tracking mode). On a shot's first frame
// (CameraScript_SnapToScript) the camera jumps to the target. Otherwise it closes on the target's
// distance from the ball by the tuning's fTrackCloseRate and turns round the ball towards it by
// fTrackTurnRate, shares a frame (timeStep in NTSC frames), each scaled: while the settings'
// fScriptTime is under the tuning's fTrackEaseInTime by ((fTrackEaseInTime - fCamTime) /
// fTrackEaseInTime) squared over fTrackCloseRate; by the settings' fInterpTime when its nInterpType is
// 4; by (the ball's speed / the tuning's fTrackMinBallSpeed) to the fourth for a slower ball; and
// while the settings' fCutEaseTime is under the tuning's fCutEaseInTime by fCutEaseTime /
// fCutEaseInTime. While fCutEaseTime is under the tuning's fCutHeightEaseTime the target's height is
// taken from the camera's only (fCutEaseTime / fCutHeightEaseTime) to the power fCutHeightEasePower,
// times the frames, of the way.
static void DynamicCam_TrackBallVelocityLag(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                            f32* position, f32* lookAt, f32* origCamPos, f32 timeStep) {
    f32 diffLength;
    f32 ballLocation[4];
    f32 ballVelocity[4];
    f32 newPosition[4];
    f32 ballToOldPos[4];
    f32 ballToOldPosNorm[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 ballToNewPos[4];
    f32 ballToNewPosNorm[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 ballToRotPos[4];
    f32 ballToRotPosNorm[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 rotAxis[4];
    f32 rotationQuat[4];
    CamShot* pCurShot;
    f32 maxHeight;
    f32 minHeight;
    f32 smoothedBallAltitude;
    f32 offset;
    f32 translationScaleAmount;
    f32 rotationScaleAmount;
    f32 easeLagAmount;
    f32 fEaseShare;
    f32 oldDistance;
    f32 angleDiffInRad;
    f32 fFrames;
    f32 origHeight;
    f32 ballSpeedFactor;
    f32 fHeightShare;

    origHeight = position[1];
    DynamicCam_GetLocation(CAMPOINT_BALL, ePlayerID, ballLocation, settings, link, lookAt, origCamPos);
    Vec3Copy(gPlayers[ePlayerID].ball.vVel, ballVelocity);
    if (link->nTrackMode == CAMTRACK_BALL_LAG_FLAT) {
        ballVelocity[1] = 0.0f;
    }
    if (ballVelocity[0] != 0.0f || ballVelocity[1] != 0.0f || ballVelocity[2] != 0.0f) {
        LLMath_Normalize3(ballVelocity, ballVelocity);
    }
    DynamicCam_ClampBallVelocity(ballVelocity, position, ballLocation);
    if (settings->pNextShot != NULL && settings->pShot->nTrackMode == settings->pNextShot->nTrackMode) {
        pCurShot = settings->pShot;
        maxHeight = pCurShot->fMaxHeight;
        minHeight = pCurShot->fMinHeight;
    } else {
        maxHeight = link->fMaxHeight;
        minHeight = link->fMinHeight;
    }
    smoothedBallAltitude = gPlayers[ePlayerID].ball.vPos[1] - settings->fGroundHeight;
    if (smoothedBallAltitude > maxHeight) {
        offset = gpCamTuning->fAboveMaxDistShare * (maxHeight - smoothedBallAltitude) + link->fDist;
    } else if (smoothedBallAltitude < minHeight) {
        offset = gpCamTuning->fBelowMinDistShare * (smoothedBallAltitude - minHeight) + link->fDist;
    } else {
        offset = link->fDist;
    }
    LLMath_AddScale3(ballLocation, ballVelocity, offset, newPosition);
    DynamicCam_AddOffset(newPosition, ballVelocity, settings, link, ePlayerID, link->fSideOffset, origHeight);
    if (CameraScript_SnapToScript(settings, link)) {
        Vec3Copy(newPosition, position);
        return;
    }
    fFrames = timeStep / FRAME_TIME;
    translationScaleAmount = gpCamTuning->fTrackCloseRate * fFrames;
    rotationScaleAmount = gpCamTuning->fTrackTurnRate * fFrames;
    if (settings->fScriptTime < gpCamTuning->fTrackEaseInTime) {
        fEaseShare = (gpCamTuning->fTrackEaseInTime - settings->fCamTime) / gpCamTuning->fTrackEaseInTime;
        fEaseShare *= fEaseShare;
        easeLagAmount = (1.0f / gpCamTuning->fTrackCloseRate) * fEaseShare;
        translationScaleAmount *= easeLagAmount;
        rotationScaleAmount *= easeLagAmount;
    }
    if (settings->nInterpType == 4) {
        translationScaleAmount *= settings->fInterpTime;
        rotationScaleAmount *= settings->fInterpTime;
    }
    if (gPlayers[ePlayerID].ball.fSpeed < gpCamTuning->fTrackMinBallSpeed) {
        ballSpeedFactor = gPlayers[ePlayerID].ball.fSpeed / gpCamTuning->fTrackMinBallSpeed;
        ballSpeedFactor = ballSpeedFactor * ballSpeedFactor;
        ballSpeedFactor = ballSpeedFactor * ballSpeedFactor;
        translationScaleAmount *= ballSpeedFactor;
        rotationScaleAmount *= ballSpeedFactor;
    }
    if (settings->fCutEaseTime < gpCamTuning->fCutEaseInTime) {
        translationScaleAmount *= settings->fCutEaseTime / gpCamTuning->fCutEaseInTime;
        rotationScaleAmount *= settings->fCutEaseTime / gpCamTuning->fCutEaseInTime;
    }
    if (settings->fCutEaseTime < gpCamTuning->fCutHeightEaseTime) {
        fHeightShare = powf(settings->fCutEaseTime / gpCamTuning->fCutHeightEaseTime,
                            gpCamTuning->fCutHeightEasePower);
        fHeightShare *= fFrames;
        newPosition[1] = fHeightShare * (newPosition[1] - position[1]) + position[1];
    }
    DynamicCam_Vec3Sub(position, ballLocation, ballToOldPos);
    DynamicCam_Vec3Sub(newPosition, ballLocation, ballToNewPos);
    oldDistance = (f32)Math_Sqrt(LLMath_LengthSquared3(ballToOldPos));
    diffLength = (f32)Math_Sqrt(LLMath_LengthSquared3(ballToNewPos)) - oldDistance;
    diffLength *= translationScaleAmount;
    if (ballToOldPos[0] != 0.0f || ballToOldPos[1] != 0.0f || ballToOldPos[2] != 0.0f) {
        LLMath_Normalize3(ballToOldPos, ballToOldPosNorm);
    } else {
        ballToOldPosNorm[0] = 0.0f;
        ballToOldPosNorm[1] = 0.0f;
        ballToOldPosNorm[2] = 0.0f;
    }
    if (ballToNewPos[0] != 0.0f || ballToNewPos[1] != 0.0f || ballToNewPos[2] != 0.0f) {
        LLMath_Normalize3(ballToNewPos, ballToNewPosNorm);
    } else {
        ballToNewPosNorm[0] = 0.0f;
        ballToNewPosNorm[1] = 0.0f;
        ballToNewPosNorm[2] = 0.0f;
    }
    angleDiffInRad = acosf(Vec3_Dot(ballToOldPosNorm, ballToNewPosNorm));
    rotationScaleAmount = angleDiffInRad * rotationScaleAmount;
    vec4flt_CrossProduct(ballToOldPosNorm, ballToNewPosNorm, rotAxis);
    if (rotAxis[0] != 0.0f || rotAxis[1] != 0.0f || rotAxis[2] != 0.0f) {
        LLMath_Normalize3(rotAxis, rotAxis);
    }
    Vec3_Scale(rotationScaleAmount, rotAxis, rotAxis);
    Quat_BuildFromVector(rotAxis, rotationQuat);
    ballToOldPosNorm[3] = 0.0f;
    Quat_RotateVector(rotationQuat, ballToOldPosNorm, ballToRotPos);
    if (ballToRotPos[0] != 0.0f || ballToRotPos[1] != 0.0f || ballToRotPos[2] != 0.0f) {
        LLMath_Normalize3(ballToRotPos, ballToRotPosNorm);
    } else {
        ballToRotPosNorm[0] = 0.0f;
        ballToRotPosNorm[1] = 0.0f;
        ballToRotPosNorm[2] = 0.0f;
    }
    Vec3_Scale(oldDistance + diffLength, ballToRotPosNorm, ballToRotPos);
    DynamicCam_Vec3Add(ballLocation, ballToRotPos, position);
}

// CAMTRACK_BALL_TIGHT, the tight ball-flight camera: straight to the target
// DynamicCam_TrackBallVelocityLag works out, with the flight direction as it is (neither taken level
// nor kept near it). It stays where it is while the ball is slower than the tuning's
// fTrackMinBallSpeed.
static void DynamicCam_TrackBallVelocityTight(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                              f32* position, f32* lookAt, f32* origCamPos) {
    f32 ballLocation[4];
    f32 ballVelocity[4];
    f32 newPosition[4];
    f32 origHeight = position[1];
    f32 smoothedBallAltitude;
    f32 maxHeight;
    f32 minHeight;
    f32 offset;

    DynamicCam_GetLocation(CAMPOINT_BALL, ePlayerID, ballLocation, settings, link, lookAt, origCamPos);
    Vec3Copy(gPlayers[ePlayerID].ball.vVel, ballVelocity);
    if (0.0f != ballVelocity[0] || 0.0f != ballVelocity[1] || 0.0f != ballVelocity[2]) {
        LLMath_Normalize3(ballVelocity, ballVelocity);
    }
    if (gPlayers[ePlayerID].ball.fSpeed < gpCamTuning->fTrackMinBallSpeed) return;
    if (settings->pNextShot != NULL && settings->pShot->nTrackMode == settings->pNextShot->nTrackMode) {
        maxHeight = settings->pShot->fMaxHeight;
        minHeight = settings->pShot->fMinHeight;
    } else {
        maxHeight = link->fMaxHeight;
        minHeight = link->fMinHeight;
    }
    smoothedBallAltitude = gPlayers[ePlayerID].ball.vPos[1] - settings->fGroundHeight;
    if (smoothedBallAltitude > maxHeight) {
        offset = gpCamTuning->fAboveMaxDistShare * (maxHeight - smoothedBallAltitude) + link->fDist;
    } else if (smoothedBallAltitude < minHeight) {
        offset = gpCamTuning->fBelowMinDistShare * (smoothedBallAltitude - minHeight) + link->fDist;
    } else {
        offset = link->fDist;
    }
    LLMath_AddScale3(ballLocation, ballVelocity, offset, newPosition);
    DynamicCam_AddOffset(newPosition, ballVelocity, settings, link, ePlayerID, link->fSideOffset, origHeight);
    Vec3Copy(newPosition, position);
}

// CAMTRACK_FIXED: the camera at the shot's first point (DynamicCam_GetLocation for nFromPoint).
static void DynamicCam_TrackFixed(CamShot* link, int ePlayerID, CameraScriptSettings_t* settings,
                                  f32* position, f32* lookAt, f32* origCamPos) {
    f32 objectPosition[4];

    DynamicCam_GetLocation(link->nFromPoint, ePlayerID, objectPosition, settings, link, lookAt, origCamPos);
    Vec3Copy(objectPosition, position);
}

// The point of kind tracking for the shot link (CameraTracking_e) into position. A point moved
// along a bone's matrix row stays where it is when the bone has no matrix. CAMPOINT_FAIRWAY_CAM
// is, on the shot's first frame (CameraScript_SnapToScript), a spot on the fairway: for the settings'
// current shot from CamScript_GetCameraOnFairwayPos (kept in vCamPos, and bFairwayFix set), for its
// next shot from CamScript_PutBackOnFairway (kept in vNextCamPos); on later frames it is vCamPos for
// both. A kind with no case here leaves position as it was.
static void DynamicCam_GetLocation(CameraTracking_e tracking, int ePlayerID, f32* position,
                                   CameraScriptSettings_t* settings, CamShot* link, f32* lookAt,
                                   f32* origCamPos) {
    f32 leftFootPos[4];
    f32 rightFootPos[4];
    f32 footCenterPos[4];
    f32 headPos[4];
    f32 waistPos[4];
    f32 chestPos[4];
    f32 chestDir[4];
    f32 headDir[4];
    f32 waistDir[4];
    f32 originPos[4];
    f32 originDir[4];
    f32 (*pBoneMat)[4];
    CameraTracking_e otherTracking;
    int nPin;
    int nTee;
    CourseInfo* pCourse;

    if (tracking == link->nFromPoint) {
        otherTracking = link->nToPoint;
    } else {
        otherTracking = link->nFromPoint;
    }
    switch (tracking) {
    case CAMPOINT_BALL:
        DynamicCam_GetSmoothBallLocation(settings, link, ePlayerID, position, 0);
        break;
    case CAMPOINT_GOLFER_FEET:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 0x39, rightFootPos);
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 0x47, leftFootPos);
        DynamicCam_Vec3Add(rightFootPos, leftFootPos, footCenterPos);
        Vec3_Scale(0.5f, footCenterPos, footCenterPos);
        Vec3Copy(footCenterPos, position);
        break;
    case CAMPOINT_GOLFER_HEAD:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 10, headPos);
        Vec3Copy(headPos, position);
        break;
    case CAMPOINT_GOLFER_CHEST:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 7, chestPos);
        Vec3Copy(chestPos, position);
        break;
    case CAMPOINT_GOLFER_WAIST:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 1, waistPos);
        Vec3Copy(waistPos, position);
        break;
    case CAMPOINT_BALL_SPOT:
        Vec3Copy(gPlayers[ePlayerID].vBall, position);
        break;
    case CAMPOINT_AIM:
        Vec3Copy(gPlayers[ePlayerID].vTarget2, position);
        break;
    case CAMPOINT_LANDING:
        Vec3Copy(settings->v50, position);
        break;
    case CAMPOINT_PIN:
        nPin = Game_CurrentPinSet();
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->pin[nPin].x, position);
        break;
    case CAMPOINT_TEE:
        nTee = gSession.nTeeSet[ePlayerID];
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->tee[nTee].x, position);
        break;
    case CAMPOINT_SHOT_POINT:
        Vec3Copy(link->v20, position);
        break;
    case CAMPOINT_FAIRWAY_CAM:
        if (CameraScript_SnapToScript(settings, link)) {
            if (link == settings->pShot) {
                CamScript_GetCameraOnFairwayPos(settings, position, lookAt, ePlayerID, settings->pB4,
                                                origCamPos, NULL);
                Vec3Copy(position, settings->vCamPos);
                settings->bFairwayFix = 1;
            } else if (link == settings->pNextShot) {
                CamScript_PutBackOnFairway(settings, position, lookAt, ePlayerID, settings->pB4, origCamPos);
                Vec3Copy(position, settings->vNextCamPos);
            }
        } else if (link == settings->pShot) {
            Vec3Copy(settings->vCamPos, position);
        } else if (link == settings->pNextShot) {
            Vec3Copy(settings->vCamPos, position);
        }
        break;
    case CAMPOINT_GOLFER_HEAD_DIR:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 10, headPos);
        pBoneMat = Character_GetBoneMatrix(gPlayers[ePlayerID].pChar, 10);
        if (pBoneMat == NULL) {
            Vec3Copy(headPos, position);
            break;
        }
        Vec3Copy(pBoneMat[2], headDir);
        if (0.0f != headDir[0] || 0.0f != headDir[1] || 0.0f != headDir[2]) {
            LLMath_Normalize3(headDir, headDir);
        }
        DynamicCam_Vec3Add(headPos, headDir, position);
        break;
    case CAMPOINT_GOLFER_CHEST_DIR:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 7, chestPos);
        pBoneMat = Character_GetBoneMatrix(gPlayers[ePlayerID].pChar, 7);
        if (pBoneMat == NULL) {
            Vec3Copy(chestPos, position);
            break;
        }
        Vec3Copy(pBoneMat[2], chestDir);
        if (0.0f != chestDir[0] || 0.0f != chestDir[1] || 0.0f != chestDir[2]) {
            LLMath_Normalize3(chestDir, chestDir);
        }
        DynamicCam_Vec3Add(chestPos, chestDir, position);
        break;
    case CAMPOINT_GOLFER_WAIST_DIR:
        Character_GetBonePos(gPlayers[ePlayerID].pChar, 1, waistPos);
        pBoneMat = Character_GetBoneMatrix(gPlayers[ePlayerID].pChar, 1);
        if (pBoneMat == NULL) {
            Vec3Copy(waistPos, position);
            break;
        }
        Vec3Copy(pBoneMat[2], waistDir);
        if (0.0f != waistDir[0] || 0.0f != waistDir[1] || 0.0f != waistDir[2]) {
            LLMath_Normalize3(waistDir, waistDir);
        }
        DynamicCam_Vec3Add(waistPos, waistDir, position);
        break;
    case CAMPOINT_GOLFER_AXIS_X:
        if (otherTracking != CAMPOINT_GOLFER_AXIS_X && otherTracking != CAMPOINT_GOLFER_AXIS_Z) {
            DynamicCam_GetLocation(otherTracking, ePlayerID, originPos, settings, link, lookAt, origCamPos);
        } else {
            Character_GetBonePos(gPlayers[ePlayerID].pChar, 0, originPos);
        }
        pBoneMat = Character_GetBoneMatrix(gPlayers[ePlayerID].pChar, 0);
        if (pBoneMat == NULL) {
            Vec3Copy(originPos, position);
            break;
        }
        Vec3Copy(pBoneMat[0], originDir);
        if (0.0f != originDir[0] || 0.0f != originDir[1] || 0.0f != originDir[2]) {
            LLMath_Normalize3(originDir, originDir);
        }
        DynamicCam_Vec3Add(originPos, originDir, position);
        break;
    case CAMPOINT_GOLFER_AXIS_Z:
        if (otherTracking != CAMPOINT_GOLFER_AXIS_X && otherTracking != CAMPOINT_GOLFER_AXIS_Z) {
            DynamicCam_GetLocation(otherTracking, ePlayerID, originPos, settings, link, lookAt, origCamPos);
        } else {
            Character_GetBonePos(gPlayers[ePlayerID].pChar, 0, originPos);
        }
        pBoneMat = Character_GetBoneMatrix(gPlayers[ePlayerID].pChar, 0);
        if (pBoneMat == NULL) {
            Vec3Copy(originPos, position);
            break;
        }
        Vec3Copy(pBoneMat[2], originDir);
        if (0.0f != originDir[0] || 0.0f != originDir[1] || 0.0f != originDir[2]) {
            LLMath_Normalize3(originDir, originDir);
        }
        DynamicCam_Vec3Add(originPos, originDir, position);
        break;
    case CAMPOINT_WORLD_Z:
        position[0] = 0.0f;
        position[1] = 0.0f;
        position[2] = 100.0f;
        break;
    }
}

// Picks the camera sequence for state of ePlayerID's shot: one at random by their weights (fWeight)
// of the first DYNAMICCAM_MAX_POSSIBLE sequences with shot choices whose state, club, shot type
// (generic ones only with allowGenericShotTypes), player type, course, par, start lie (the ball's lie
// startLie, CAMLIE_TEE on the tee; DynamicCam_BallLieToCameraLie), end lie (surface class endLie;
// DynamicCam_MaterialToCameraLie), pin height over the ball (fMinPinHeight..fMaxPinHeight), game mode
// and shot distance shotDist (fMinDist..fMaxDist) all fit. With none, the last default ("DEF")
// sequence for the state on this course, else the first default one for the state that has choices,
// else NULL. NULL without course data.
CameraSequence_t* DynamicCam_ChooseSequence(int ePlayerID, int startLie, int endLie, int state,
                                            u8 allowGenericShotTypes, f32 shotDist) {
    int possibleScripts[DYNAMICCAM_MAX_POSSIBLE];
    CourseInfo* pCourse;
    CameraSequence_t* pSeq;
    int i;
    int scriptsFound;
    int currentScript;
    int defaultSeq;
    int cameraStartLie;
    int cameraEndLie;
    int nPinSet;
    u32 randomNum;
    int randomScript;
    f32 heightDiff;
    f32 possiblePercent;

    scriptsFound = 0;
    currentScript = 0;
    defaultSeq = -1;
    if (Player_OnTee(ePlayerID)) {
        cameraStartLie = CAMLIE_TEE;
    } else {
        cameraStartLie = DynamicCam_BallLieToCameraLie(startLie, ePlayerID);
    }
    cameraEndLie = DynamicCam_MaterialToCameraLie(endLie, ePlayerID);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    heightDiff = pCourse->pin[nPinSet].y - gPlayers[ePlayerID].vBall[1];
    for (i = 0; i < gpDynCam->nSequences; i++) {
        if (scriptsFound >= DYNAMICCAM_MAX_POSSIBLE) {
            break;
        }
        if (gpDynCam->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], state) &&
                DynamicCam_MatchCourse(&gpDynCam->pSequences[i])) {
                defaultSeq = i;
            } else if (DynamicCam_MatchStateType(&gpDynCam->pSequences[i], state) &&
                       DynamicCam_MatchSeqClub(&gpDynCam->pSequences[i], ePlayerID) &&
                       DynamicCam_MatchSeqShot(&gpDynCam->pSequences[i], ePlayerID, allowGenericShotTypes) &&
                       DynamicCam_MatchPlayerType(&gpDynCam->pSequences[i], ePlayerID) &&
                       DynamicCam_MatchCourse(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchPars(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uStartLies, cameraStartLie) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uEndLies, cameraEndLie) &&
                       DynamicCam_MatchHeightDiff(&gpDynCam->pSequences[i], ePlayerID, heightDiff) &&
                       DynamicCam_ModeType(&gpDynCam->pSequences[i])) {
                pSeq = &gpDynCam->pSequences[i];
                if (shotDist >= pSeq->fMinDist && shotDist <= pSeq->fMaxDist && pSeq->nChoices > 0) {
                    possibleScripts[scriptsFound] = i;
                    scriptsFound++;
                }
            }
        }
    }
    if (scriptsFound == 0) {
        if (defaultSeq >= 0) {
            return &gpDynCam->pSequences[defaultSeq];
        }
        for (i = 0; i < gpDynCam->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], state) &&
                gpDynCam->pSequences[i].nChoices > 0) {
                return &gpDynCam->pSequences[i];
            }
        }
        return NULL;
    }
    randomNum = Misc_RandFunc(1);
    possiblePercent = 0.0f;
    for (i = 0; i < scriptsFound; i++) {
        possiblePercent += gpDynCam->pSequences[possibleScripts[i]].fWeight;
    }
    randomScript = randomNum % (int)(1000.0f * possiblePercent);
    for (i = 0; i < scriptsFound; i++) {
        if ((f32)(randomScript - currentScript)
            < 1000.0f * gpDynCam->pSequences[possibleScripts[i]].fWeight) {
            return &gpDynCam->pSequences[possibleScripts[i]];
        }
        currentScript += (int)(1000.0f * gpDynCam->pSequences[possibleScripts[i]].fWeight);
    }
    return &gpDynCam->pSequences[possibleScripts[0]];
}

// DynamicCam_ChooseSequence's pick for the pre-flight cameras: the ball's distance to the pin
// (GameAnalysis_GetCurrentDistanceToPin, DynamicCam_MatchDistToPin) stands for the shot distance,
// there is no end lie, and generic shot types are allowed. A CAMSTATE_SWING pick is made for
// CAMSTATE_SWING_OBSTRUCTED instead when an object or hazard is within the tuning's
// fBallObstructionDist of the ball (Ter_CheckObjectAndHazardObstruction); the last fallback, the
// first default sequence with choices, then looks for CAMSTATE_SWING again.
CameraSequence_t* DynamicCam_ChoosePreFlightSequence(int ePlayerID, int startLie, int state) {
    int possibleScripts[DYNAMICCAM_MAX_POSSIBLE];
    CourseInfo* pCourse;
    int i;
    int scriptsFound;
    int currentScript;
    int defaultSeq;
    int cameraStartLie;
    int nPinSet;
    u32 randomNum;
    int randomScript;
    f32 distToPin;
    f32 heightDiff;
    f32 possiblePercent;

    scriptsFound = 0;
    currentScript = 0;
    defaultSeq = -1;
    if (Player_OnTee(ePlayerID)) {
        cameraStartLie = CAMLIE_TEE;
    } else {
        cameraStartLie = DynamicCam_BallLieToCameraLie(startLie, ePlayerID);
    }
    distToPin = GameAnalysis_GetCurrentDistanceToPin(ePlayerID);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    heightDiff = pCourse->pin[nPinSet].y - gPlayers[ePlayerID].vBall[1];
    if (state == CAMSTATE_SWING
        && Ter_CheckObjectAndHazardObstruction(gPlayers[ePlayerID].ball.vPos,
                                               gpCamTuning->fBallObstructionDist, 1, 0, 0.0f, 1,
                                               gpCamTuning->fObstructionMaxSlope)) {
        state = CAMSTATE_SWING_OBSTRUCTED;
    }
    for (i = 0; i < gpDynCam->nSequences; i++) {
        if (scriptsFound >= DYNAMICCAM_MAX_POSSIBLE) {
            break;
        }
        if (gpDynCam->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], state) &&
                DynamicCam_MatchCourse(&gpDynCam->pSequences[i])) {
                defaultSeq = i;
            } else if (DynamicCam_MatchStateType(&gpDynCam->pSequences[i], state) &&
                       DynamicCam_MatchSeqClub(&gpDynCam->pSequences[i], ePlayerID) &&
                       DynamicCam_MatchSeqShot(&gpDynCam->pSequences[i], ePlayerID, 1) &&
                       DynamicCam_MatchPlayerType(&gpDynCam->pSequences[i], ePlayerID) &&
                       DynamicCam_MatchDistToPin(&gpDynCam->pSequences[i], ePlayerID, distToPin) &&
                       DynamicCam_MatchCourse(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchPars(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchHeightDiff(&gpDynCam->pSequences[i], ePlayerID, heightDiff) &&
                       DynamicCam_ModeType(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uStartLies, cameraStartLie)) {
                possibleScripts[scriptsFound] = i;
                scriptsFound++;
            }
        }
    }
    if (scriptsFound == 0) {
        if (defaultSeq >= 0) {
            return &gpDynCam->pSequences[defaultSeq];
        }
        if (state == CAMSTATE_SWING_OBSTRUCTED) {
            state = CAMSTATE_SWING;
        }
        for (i = 0; i < gpDynCam->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], state) &&
                gpDynCam->pSequences[i].nChoices > 0) {
                return &gpDynCam->pSequences[i];
            }
        }
        return NULL;
    }
    randomNum = Misc_RandFunc(1);
    possiblePercent = 0.0f;
    for (i = 0; i < scriptsFound; i++) {
        possiblePercent += gpDynCam->pSequences[possibleScripts[i]].fWeight;
    }
    randomScript = randomNum % (int)(1000.0f * possiblePercent);
    for (i = 0; i < scriptsFound; i++) {
        if ((f32)(randomScript - currentScript)
            < 1000.0f * gpDynCam->pSequences[possibleScripts[i]].fWeight) {
            return &gpDynCam->pSequences[possibleScripts[i]];
        }
        currentScript += (int)(1000.0f * gpDynCam->pSequences[possibleScripts[i]].fWeight);
    }
    return &gpDynCam->pSequences[possibleScripts[0]];
}

// The anim pair named animName (case ignored) gives a sequence (*pSequence) or a shot (*pLink),
// returning 1: its pChanceSeq 39 times in 100 when that has shot choices, else by its kind: a
// CAMPAIR_RANDOM_SEQUENCE pair one of pRandomSeq1, pRandomSeq2 and pRandomSeq3 at random
// (pRandomSeq3 when the one picked is missing; the search goes on when that is missing too), a
// CAMPAIR_SHOT pair its shot. 0 when no pair gives one.
static u8 DynamicCamSearchForPairedSequence(char* animName, CameraSequence_t** pSequence, CamShot** pLink) {
    int i;
    u32 randomval;

    for (i = 0; i < gpDynCam->nAnimPairs; i++) {
        if (stricmp(gpDynCam->pAnimPairs[i].szName, animName) != 0) {
            continue;
        }
        if (gpDynCam->pAnimPairs[i].pChanceSeq != NULL && gpDynCam->pAnimPairs[i].pChanceSeq->nChoices > 0 &&
            Misc_RandFunc(1) % 100 > 60) {
            *pSequence = gpDynCam->pAnimPairs[i].pChanceSeq;
            return 1;
        }
        if (gpDynCam->pAnimPairs[i].nKind == CAMPAIR_RANDOM_SEQUENCE) {
            randomval = Misc_RandFunc(1) % 3;
            if (randomval == 0 && gpDynCam->pAnimPairs[i].pRandomSeq1 != NULL) {
                *pSequence = gpDynCam->pAnimPairs[i].pRandomSeq1;
                return 1;
            }
            if (randomval == 1 && gpDynCam->pAnimPairs[i].pRandomSeq2 != NULL) {
                *pSequence = gpDynCam->pAnimPairs[i].pRandomSeq2;
                return 1;
            }
            if (gpDynCam->pAnimPairs[i].pRandomSeq3 != NULL) {
                *pSequence = gpDynCam->pAnimPairs[i].pRandomSeq3;
                return 1;
            }
        } else if (gpDynCam->pAnimPairs[i].nKind == CAMPAIR_SHOT) {
            *pLink = gpDynCam->pAnimPairs[i].pShot;
            return 1;
        }
    }
    return 0;
}

// The sequence or shot paired with the golfer's animation (DynamicCamSearchForPairedSequence): with
// checkReaction the clip in Character.pReactionClip when there is one, else the clip he is playing
// (pCurClip). While the GameBreaker letterbox is up (GameEffects_IsLetterboxOn) the name with "LB" in
// front is tried first. 1 when one is found; *pSequence and *pLink are cleared first, and a NULL out
// pointer gives 0.
u8 DynamicCam_ChoosePairedSequenceOrCamera(int ePlayerID, u8 checkReaction, CameraSequence_t** pSequence,
                                           CamShot** pLink) {
    char copyAnimName[0x18];
    char* animName = NULL;

    if (pSequence == NULL || pLink == NULL) {
        return 0;
    }
    *pSequence = NULL;
    *pLink = NULL;
    if (checkReaction && gPlayers[ePlayerID].pChar->pReactionClip != NULL) {
        animName = gPlayers[ePlayerID].pChar->pReactionClip->name;
    }
    if (animName == NULL && gPlayers[ePlayerID].pChar->pCurClip != NULL) {
        animName = gPlayers[ePlayerID].pChar->pCurClip->name;
    }
    if (animName != NULL) {
        if (GameEffects_IsLetterboxOn()) {
            sprintf(copyAnimName, "LB%s", animName);
            if (DynamicCamSearchForPairedSequence(copyAnimName, pSequence, pLink) == 1) {
                return 1;
            }
        }
        if (DynamicCamSearchForPairedSequence(animName, pSequence, pLink) == 1) {
            return 1;
        }
    }
    return 0;
}

// A default sequence for state: one of that state (DynamicCam_MatchStateType) whose name starts with
// "DEF". 0 for NULL.
static u8 DynamicCam_IsDefualtSeq(CameraSequence_t* seq, CameraShotState_e state) {
    if (seq == NULL) return 0;
    if (!DynamicCam_MatchStateType(seq, state)) return 0;
    if (seq->szName[0] != 'D') return 0;
    if (seq->szName[1] != 'E') return 0;
    if (seq->szName[2] != 'F') return 0;
    return 1;
}

// The camera lie for surface class lieID, CAMLIE_NONE past 19: class 1 is the tee under a ball on
// the tee (LIE_TEE_e), else the fairway.
static CameraLie_e DynamicCam_MaterialToCameraLie(u32 lieID, int playerID) {
    static CameraLie_e sMaterialToCamLies[20] = {
        CAMLIE_NONE, CAMLIE_TEE, CAMLIE_FAIRWAY, CAMLIE_GREEN, CAMLIE_FAIRWAY, CAMLIE_ROUGH,
        CAMLIE_SAND, CAMLIE_WATER, CAMLIE_ROUGH, CAMLIE_ROUGH, CAMLIE_ROUGH, CAMLIE_ROUGH,
        CAMLIE_GREEN, CAMLIE_ROUGH, CAMLIE_ROUGH, CAMLIE_ROUGH, CAMLIE_WATER, CAMLIE_ROUGH,
        CAMLIE_GREEN, CAMLIE_OUT_OF_BOUNDS,
    };
    CameraLie_e nRet;

    if (lieID > 19) return CAMLIE_NONE;
    if (lieID == 1) {
        nRet = CAMLIE_FAIRWAY;
        if (gPlayers[playerID].ball.nLie == LIE_TEE_e) {
            nRet = CAMLIE_TEE;
        }
        return nRet;
    }
    return sMaterialToCamLies[lieID];
}

// The camera lie for each ball lie up to LIE_OUT_OF_BOUNDS_e (DynamicCam_BallLieToCameraLie).
CameraLie_e gBallLieToCamLies[17] = {
    CAMLIE_TEE, CAMLIE_FAIRWAY, CAMLIE_FAIRWAY, CAMLIE_ROUGH, CAMLIE_ROUGH, CAMLIE_ROUGH,
    CAMLIE_SAND, CAMLIE_SAND, CAMLIE_SAND, CAMLIE_GREEN, CAMLIE_FAIRWAY, CAMLIE_ROUGH,
    CAMLIE_GREEN, CAMLIE_WATER, CAMLIE_WATER, CAMLIE_WATER, CAMLIE_OUT_OF_BOUNDS,
};

// The camera lie for the ball's lie, from gBallLieToCamLies. lie is not checked: LIE_MISC_e (17),
// which Physics_SetLie gives the surface classes it does not list, reads one past the table.
// playerID is not used.
static CameraLie_e DynamicCam_BallLieToCameraLie(Lie_t lie, int playerID) {
    return gBallLieToCamLies[lie];
}

// The sequence suits the player's club (its nClubType). The woods are the drivers and the 3, 5 and
// 7 woods, the wedges the pitching wedge to the high lob wedge.
static u8 DynamicCam_MatchSeqClub(CameraSequence_t* seq, int ePlayerID) {
    Club_t club = gPlayers[ePlayerID].nClub;

    switch (seq->nClubType) {
    case CAMCLUB_ANY:
        return 1;
    case CAMCLUB_WOODS:
        if (club >= CLUB_DRIVER1_e && club <= CLUB_7WOOD_e) return 1;
        return 0;
    case CAMCLUB_IRONS_5_TO_9:
        if (club >= CLUB_5IRON_e && club <= CLUB_9IRON_e) return 1;
        return 0;
    case CAMCLUB_IRONS_1_TO_5:
        if (club >= CLUB_1IRON_e && club <= CLUB_5IRON_e) return 1;
        return 0;
    case CAMCLUB_WEDGES:
        if (club >= CLUB_PITCHINGWEDGE_e && club <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case CAMCLUB_PUTTER:
        if (club >= CLUB_PUTTER_e && club <= CLUB_PUTTER_e) return 1;
        return 0;
    case CAMCLUB_WOODS_AND_IRONS:
        if (club >= CLUB_DRIVER1_e && club <= CLUB_9IRON_e) return 1;
        return 0;
    case CAMCLUB_ALL_BUT_PUTTER:
        if (club >= CLUB_DRIVER1_e && club <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case CAMCLUB_5_IRON_TO_WEDGES:
        if (club >= CLUB_5IRON_e && club <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case CAMCLUB_IRONS_AND_WEDGES:
        if (club >= CLUB_1IRON_e && club <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case CAMCLUB_WOODS_TO_5_IRON:
        if (club >= CLUB_DRIVER1_e && club <= CLUB_5IRON_e) return 1;
        return 0;
    default:
        return 0;
    }
}

// The pin's height over the ball (f) is within the sequence's
// fMinPinHeight..fMaxPinHeight. nPlayer is not used.
u8 DynamicCam_MatchHeightDiff(CameraSequence_t* pSequence, int nPlayer, f32 f) {
    if (f <= pSequence->fMaxPinHeight && f >= pSequence->fMinPinHeight) {
        return 1;
    }
    return 0;
}

// The sequence suits the player's shot type (Player.nShotKind) by its nShotType: 2..8 types 1..7, 10 type
// 0, 11 type 1, 12 any type but 0 and 1. The generic entries need b (TW07's allowGenericShotTypes):
// 0 any shot, 1 a ball on lie 0, 13 a ball on any other lie. Any other nShotType: no.
u8 DynamicCam_MatchSeqShot(CameraSequence_t* pSequence, int nPlayer, u8 b) {
    int nKind = gPlayers[nPlayer].nShotKind;

    switch (pSequence->nShotType) {
    case 0:
        return b != 0;
    case 1:
        if (gPlayers[nPlayer].ball.nLie == 0) {
            return b != 0;
        }
        return 0;
    case 2:
        return nKind == 1;
    case 3:
        return nKind == 2;
    case 4:
        return nKind == 3;
    case 5:
        return nKind == 4;
    case 6:
        return nKind == 5;
    case 7:
        return nKind == 6;
    case 8:
        return nKind == 7;
    case 10:
        return nKind == 0;
    case 11:
        return nKind == 1;
    case 12:
        if (nKind != 1 && nKind != 0) return 1;
        return 0;
    case 13:
        if (gPlayers[nPlayer].ball.nLie == 0) return 0;
        return b != 0;
    default:
        return 0;
    }
}

// The sequence suits who is playing (nPlayerType: see CameraSequence_t).
u8 DynamicCam_MatchPlayerType(CameraSequence_t* pSequence, int nPlayer) {
    switch (pSequence->nPlayerType) {
    case 0:
        if (!gSession.bReplay && !Player_IsCPU(nPlayer)) return 1;
        return 0;
    case 1:
        if (!gSession.bReplay && Player_IsCPU(nPlayer)) return 1;
        return 0;
    case 2:
        if (gSession.bReplay) return 0;
        return 1;
    case 3:
        return gSession.bReplay != 0;
    case 4:
        if (gSession.bReplay || Player_IsCPU(nPlayer)) return 1;
        return 0;
    default:
        return 0;
    }
}

// The sequence is used on the current course.
u8 DynamicCam_MatchCourse(CameraSequence_t* pSequence) {
    return (pSequence->uCourses & (1 << Game_GetCourse())) != 0;
}

// The sequence is used on a hole of the current one's par: its n4B bit for GM_GetCurrentHolePar is
// set.
u8 DynamicCam_MatchPars(CameraSequence_t* pSequence) {
    return (pSequence->n4B & (1 << GM_GetCurrentHolePar())) != 0;
}

// Camera lie nBit is in the lie mask nMask (a sequence's uStartLies or uEndLies).
u8 DynamicCam_MatchLies(int nMask, int nBit) {
    return (nMask & (1 << nBit)) != 0;
}

// The ball's distance to the pin (f) is within the sequence's f24..f28. nPlayer is not used.
u8 DynamicCam_MatchDistToPin(CameraSequence_t* pSequence, int nPlayer, f32 f) {
    if (f >= pSequence->fMinDist && f <= pSequence->fMaxDist) {
        return 1;
    }
    return 0;
}

// The sequence is for state nKind (its nStateType): state 14 takes any sequence, state 10 any of states
// 6..9.
u8 DynamicCam_MatchStateType(CameraSequence_t* pSequence, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pSequence->nStateType) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pSequence->nStateType == nKind;
}

// The sequence suits the game mode and screen (nModeType: see CameraSequence_t).
u8 DynamicCam_ModeType(CameraSequence_t* pSequence) {
    int nMode = Game_GetMode();

    if (pSequence == NULL) return 0;
    if (pSequence->nModeType == 0) {
        if (gSession.nSplitScreen) return 0;
        if (nMode == 9) return 0;
        if (nMode == 11) return 0;
        if (GM_Currently_SkillZoneMode()) return 0;
        return 1;
    }
    if (pSequence->nModeType == 1) {
        if (gSession.nSplitScreen) return 1;
        if (nMode == 9) return 1;
        return nMode == 11;
    }
    if (pSequence->nModeType == 2) {
        return GM_Currently_SkillZoneMode() != 0;
    }
    return 0;
}

// The shot is for state nKind (its nStateType): state 14 takes any shot, state 10 any of states 6..9.
u8 DynamicCam_MatchScriptStateType(CamShot* pShot, int nKind) {
    if (nKind == 14) return 1;
    if (nKind == 10) {
        switch (pShot->nStateType) {
        case 6:
        case 7:
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return pShot->nStateType == nKind;
}

// The shot may be used for the golfer shown: always outside game type 3 (the CrAP screen); there
// only when the CrAP state's golfer (pB4) has a character and the shot's u50/u54 bit for its
// nGolferId is set.
u8 DynamicCam_MatchPlayer(CamShot* pShot) {
    CrAPGolfer* pGolfer;
    int n;

    if (gSession.nGameType != 3) return 1;
    pGolfer = gpCrAPState->pB4;
    if (pGolfer != NULL && pGolfer->pChar != NULL) {
        n = pGolfer->nGolferId;
        if (n <= 32) {
            // EA bug: n == 32 shifts by 32 (undefined in C; the PowerPC gives 0)
            return (pShot->u.bits.u50 & (1 << n)) != 0;
        }
        return (pShot->u.bits.u54 & (1 << (n - 32))) != 0;
    }
    return 0;
}

// Moves pPos fSide sideways, square to pDir on the level, then hands it on to DynamicCam_AddHeightOffset.
void DynamicCam_AddOffset(f32* pPos, f32* pDir, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                          f32 fSide, f32 fY) {
    f32 vDir[4];

    Vec3Copy(pDir, vDir);
    vDir[1] = 0.0f;
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        LLMath_Normalize3(vDir, vDir);
    }
    pPos[0] += fSide * -vDir[2];
    pPos[2] += fSide * vDir[0];
    DynamicCam_AddHeightOffset(pPos, pScript, pShot, nPlayer, fY);
}

// Sets the camera's height pPos[1] by the shot's nHeightRef (0: left alone): 1 between the golfer's
// feet (bones 0x39 and 0x47), 2 the waist (bone 1), 3 the head (bone 0xA), 4 fY (the height
// before), 5 and 7 the ball (DynamicCam_GetSmoothBallLocation), 6 the pin; then the shot's
// fHeightOffset above that. Kind 7 follows the ball smoothly: when CameraScript_SnapToScript holds
// it takes that height and the script's f104 the shot's fMinHeight; otherwise, once per ball update
// this frame (none: it stays at fY), it steps from the script's v70 towards the ball and takes a
// share of the way from the last height: the tuning's fRisingHeightShare while the ball rises (f104
// keeping its height over the script's ground fGroundHeight), and on the way down
// fFallingHeightShare near the ground (under the shot's fMinHeight, at least 0.15) blending to
// fRisingHeightShare at the height f104. Early on (the script's fScriptTime under the tuning's
// fScriptEaseInTime) the share is larger, all of the way at the start.
void DynamicCam_AddHeightOffset(f32* pPos, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                                f32 fY) {
    f32 vBone47[4];
    f32 vBone39[4];
    f32 vMid[4];
    f32 vBone1[4];
    f32 vBoneA[4];
    f32 vPoint[4];
    f32 vStep[4];
    f32 vOff[4];
    int nSteps;
    int nPin;
    CourseInfo* pCourse;
    int i;
    f32 fLow;
    f32 fHeight;
    f32 fEase;
    f64 dSmooth;

    nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (pShot->nHeightRef == CAMHEIGHT_NONE) {
        return;
    }
    switch (pShot->nHeightRef) {
    case 1:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x39, vBone39);
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x47, vBone47);
        DynamicCam_Vec3Add(vBone39, vBone47, vMid);
        Vec3_Scale(0.5f, vMid, vMid);
        pPos[1] = vMid[1];
        break;
    case 2:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        pPos[1] = vBone1[1];
        break;
    case 3:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0xA, vBoneA);
        pPos[1] = vBoneA[1];
        break;
    case 4:
        pPos[1] = fY;
        break;
    case 5:
    case 7:
        DynamicCam_GetSmoothBallLocation(pScript, pShot, nPlayer, vPoint, 0);
        pPos[1] = vPoint[1];
        break;
    case 6:
        nPin = Game_CurrentPinSet();
        pCourse = Ter_GetTGD();
        if (pCourse != NULL) {
            pPos[1] = pCourse->pin[nPin].y;
        }
        break;
    }
    pPos[1] += pShot->fHeightOffset;
    if (pShot->nHeightRef != 7) {
        return;
    }
    if (CameraScript_SnapToScript(pScript, pShot)) {
        pScript->f104 = pShot->fMinHeight;
        return;
    }
    if (nSteps == 0) {
        pPos[1] = fY;
        return;
    }
    for (i = 0; i < nSteps; i++) {
        DynamicCam_Vec3Sub(vPoint, pScript->v70, vOff);
        Vec3_Scale((f32)(i + 1) / (f32)nSteps, vOff, vOff);
        DynamicCam_Vec3Add(pScript->v70, vOff, vStep);
        pPos[1] = vStep[1];
        pPos[1] += pShot->fHeightOffset;
        fLow = pShot->fMinHeight;
        if (fLow <= 0.15f) {
            fLow = 0.15f;
        }
        if (!gPlayers[nPlayer].ball.bHitTopArc) {
            fEase = gpCamTuning->fRisingHeightShare;
            pScript->f104 = pPos[1] - pScript->fGroundHeight;
        } else {
            fHeight = pPos[1] - pScript->fGroundHeight;
            if (fHeight <= fLow || pScript->f104 < fLow) {
                fEase = gpCamTuning->fFallingHeightShare;
            } else if (fHeight > pScript->f104) {
                fEase = gpCamTuning->fRisingHeightShare;
            } else {
                fEase = (gpCamTuning->fRisingHeightShare - gpCamTuning->fFallingHeightShare)
                        * ((fHeight - fLow) / pScript->f104) +
                        gpCamTuning->fFallingHeightShare;
            }
        }
        if (pScript->fScriptTime < gpCamTuning->fScriptEaseInTime) {
            dSmooth = Math_Sqrt((f32)Math_Sqrt(pScript->fScriptTime / gpCamTuning->fScriptEaseInTime));
            fEase = 1.0f - (f32)dSmooth * (1.0f - fEase);
        }
        pPos[1] = fEase * (pPos[1] - fY) + fY;
        fY = pPos[1];
    }
}

// The sequence may serve as the ball-flight sequence: it suits the player's club and shot type
// (generic shot types allowed). 0 for NULL. GoGolfCam.c tests the current sequence's follow-on with
// it.
u8 DynamicCam_IsValidFlightSequence(CameraSequence_t* pSequence, int nPlayer) {
    if (pSequence == NULL) return 0;
    if (DynamicCam_MatchSeqClub(pSequence, nPlayer) && DynamicCam_MatchSeqShot(pSequence, nPlayer, 1)) {
        return 1;
    }
    return 0;
}

// Keeps the ball's flight direction pDir (normalised by the caller) within the tuning's
// fMaxFlightTilt radians of level: with no level part at all it becomes the direction from pA (the
// camera) to pB (the ball), normalised; tilted further than that, it is turned back to that tilt.
void DynamicCam_ClampBallVelocity(f32* pDir, f32* pA, f32* pB) {
    f32 vLevel[4];
    f32 vAxis[4];
    f32 qTurn[4];

    if (0.0f == pDir[0] && 0.0f == pDir[2]) {
        DynamicCam_Vec3Sub(pB, pA, pDir);
        if (0.0f != pDir[0] || 0.0f != pDir[1] || 0.0f != pDir[2]) {
            LLMath_Normalize3(pDir, pDir);
        }
        return;
    }
    Vec3Copy(pDir, vLevel);
    vLevel[1] = 0.0f;
    if (0.0f != vLevel[0] || 0.0f != vLevel[1] || 0.0f != vLevel[2]) {
        LLMath_Normalize3(vLevel, vLevel);
    }
    if (fabsf(acosf(Vec3_Dot(vLevel, pDir))) > gpCamTuning->fMaxFlightTilt) {
        vec4flt_CrossProduct(pDir, vLevel, vAxis);
        if (0.0f != vAxis[0] || 0.0f != vAxis[1] || 0.0f != vAxis[2]) {
            LLMath_Normalize3(vAxis, vAxis);
        }
        Vec3_Scale(gpCamTuning->fMaxFlightTilt, vAxis, vAxis);
        Quat_BuildFromVector(vAxis, qTurn);
        vLevel[3] = 0.0f;
        Quat_RotateVector(qTurn, vLevel, pDir);
        if (0.0f != pDir[0] || 0.0f != pDir[1] || 0.0f != pDir[2]) {
            LLMath_Normalize3(pDir, pDir);
        }
    }
}

// The ball's position into pOut; but when the ball is in the cup (lie 12) or on surface 0x62 or
// 0x69, more than 0.005 below the pin and within 2 of the script's v70, v70 instead. With bKeep,
// v70 takes the ball's position whenever it is not used.
void DynamicCam_GetSmoothBallLocation(CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer, f32* pOut,
                                      u8 bKeep) {
    u8 bNear = 0;
    CourseInfo* pCourse = Ter_GetTGD();
    int nPin = Game_CurrentPinSet();

    if ((gPlayers[nPlayer].ball.nSurface == 0x62 || gPlayers[nPlayer].ball.nSurface == 0x69
         || gPlayers[nPlayer].ball.nLie == 12)
        && pCourse != NULL) {
        if (pCourse->pin[nPin].y - gPlayers[nPlayer].ball.vPos[1] > 0.005f
            && LLMath_DistanceBetween3(pScript->v70, gPlayers[nPlayer].ball.vPos) < 2.0f) {
            bNear = 1;
        }
    }
    if (bNear) {
        Vec3Copy(pScript->v70, pOut);
    } else {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pOut);
    }
    if (bKeep && !bNear) {
        Vec3Copy(gPlayers[nPlayer].ball.vPos, pScript->v70);
    }
}

// The shot's side offset fSideOffset into *pA and its distance fDist into *pB (each when not NULL; nothing
// for a NULL shot). When CameraScript_FlipCameraForLefty holds for the player and the shot one of
// them changes sign: the side (-fSideOffset), or the distance (-fDist) for a shot with a
// CAMPOINT_GOLFER_AXIS_Z point.
void DynamicCam_CalculateFlippedOffsets(CamShot* pShot, int nPlayer, f32* pA, f32* pB) {
    if (pShot != NULL) {
        if (CameraScript_FlipCameraForLefty(nPlayer, pShot)) {
            if (pShot->nFromPoint == CAMPOINT_GOLFER_AXIS_Z || pShot->nToPoint == CAMPOINT_GOLFER_AXIS_Z) {
                if (pA != NULL) {
                    *pA = pShot->fSideOffset;
                }
                if (pB != NULL) {
                    *pB = -pShot->fDist;
                }
            } else {
                if (pA != NULL) {
                    *pA = -pShot->fSideOffset;
                }
                if (pB != NULL) {
                    *pB = pShot->fDist;
                }
            }
        } else {
            if (pA != NULL) {
                *pA = pShot->fSideOffset;
            }
            if (pB != NULL) {
                *pB = pShot->fDist;
            }
        }
    }
}

// The shot's most height f, times 10 on three holes: course 9's hole 12 and course 3's holes 13 and
// 15 (Game_GetCurHoleNum).
f32 DynamicCam_fAdjustMaxHeightByHole(f32 f) {
    if (Game_GetCourse() == 9 && Game_GetCurHoleNum() == 12) {
        return 10.0f * f;
    }
    if (Game_GetCourse() == 3 && (Game_GetCurHoleNum() == 13 || Game_GetCurHoleNum() == 15)) {
        return 10.0f * f;
    }
    return f;
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void DynamicCam_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
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
#else
// port: untested, the plain-C version for compilers without paired singles.
void DynamicCam_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void DynamicCam_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
#else
// port: untested, the plain-C version for compilers without paired singles.
void DynamicCam_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// The shot looks at the golfer: its look-at kind (nLookAtKind) is 1..7. TW07 has it as an inline in
// GoCamScripts.h; this build keeps one copy here, which gocamscripts.c, GoGolfCam.c and
// GameEffects.c call too.
u8 CamScript_DoesScriptTrackGolfer(CamShot* pShot) {
    u8 nKind = pShot->nLookAtKind;

    if (nKind == CAMPOINT_BALL_SPOT
        || (u8)(nKind - CAMPOINT_GOLFER_FEET) <= CAMPOINT_GOLFER_HEAD - CAMPOINT_GOLFER_FEET
        || nKind == CAMPOINT_GOLFER_HEAD_LAGGED) {
        return 1;
    }
    return 0;
}

// The GameBreaker letterbox is up: a GameBreaker is on (bGameBreaker) and it is a predicted one
// (nGBType) or its b19 is 1. TW07 has it as an inline in GameEffects.h; this build keeps one copy
// here, which GameUICommands.c calls too.
u8 GameEffects_IsLetterboxOn(void) {
    int bResult = 0;

    if (gGameEffects.bGameBreaker && (gGameEffects.nGBType != 0 || gGameEffects.b19 == 1)) {
        bResult = 1;
    }
    return bResult;
}
