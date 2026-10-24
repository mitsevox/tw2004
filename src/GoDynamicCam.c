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
u8   DynamicCamSearchForPairedSequence(char* szName, CameraSequence_t** ppSeq, CamShot** ppShot);
void DynamicCam_Vec3Add(f32* pA, f32* pB, f32* pOut);         // a + b
void DynamicCam_Vec3Sub(f32* pA, f32* pB, f32* pOut);
u8   DynamicCam_MatchStateType(CameraSequence_t* pSequence, int nKind);
u8   DynamicCam_MatchScriptStateType(CamShot* pShot, int nKind);
u8   DynamicCam_MatchPlayer(CamShot* pShot);
void DynamicCam_GetLocation(int nKind, int nPlayer, f32* pOut, CameraScriptSettings_t* pScript,
                            CamShot* pShot, f32* pCam, f32* pSub);
void DynamicCam_AddOffset(f32* pPos, f32* pDir, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                          f32 fSide, f32 fY);
void DynamicCam_AddHeightOffset(f32* pPos, CameraScriptSettings_t* pScript, CamShot* pShot, int nPlayer,
                                f32 fY);
void DynamicCam_TrackOffset(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut,
                            f32* pCam, f32* pSub);
void DynamicCam_TrackBallVelocityLag(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut,
                                     f32* pCam, f32* pSub, f32 f);
void DynamicCam_TrackBallVelocityTight(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript,
                                       f32* pOut, f32* pCam, f32* pSub);
void DynamicCam_TrackFixed(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut, f32* pCam,
                           f32* pSub);
f32  DynamicCam_fAdjustMaxHeightByHole(f32 f);
void DynamicCam_ClampBallVelocity(f32* pDir, f32* pA, f32* pB);
u8   DynamicCam_IsDefualtSeq(CameraSequence_t* pSequence, int nKind);
s32  DynamicCam_MaterialToCameraLie(u32 n, int nPlayer);
s32  DynamicCam_BallLieToCameraLie(int n, int nPlayer);
u8   DynamicCam_MatchSeqClub(CameraSequence_t* pSequence, int nPlayer);
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
        Circular_MemFreeFunc(pStreamData);
        return;
    }
    srcPtr = pStreamData->pData;
    BYTESWAP_SWAPDATA(&srcPtr, (u8*)&numSeqsToLoad, sizeof(numSeqsToLoad), 4);
    srcPtr = pStreamData->pData + 4;
    BYTESWAP_SWAPDATA(&srcPtr, (u8*)&numTriggersToLoad, sizeof(numTriggersToLoad), 4);
    gpDynCam->pSequences =
        Circular_MemAllocFunc(numSeqsToLoad * sizeof(CameraSequence_t), MEMTYPE_LOW, 0,
                              "GoDynamicCam.c", 403);
    gpDynCam->pChoices =
        Circular_MemAllocFunc(numTriggersToLoad * sizeof(CameraEventTrigger_t), MEMTYPE_LOW, 0,
                              "GoDynamicCam.c", 404);
    gpDynCam->nSequences = 0;
    gpDynCam->nChoicesUsed = 0;
    srcPtr = pStreamData->pData + 8;
    DynamicCam_CopySequenceData(srcPtr, (u8*)gpDynCam->pSequences, numSeqsToLoad);
    DynamicCam_ParseCameraSeqs(numSeqsToLoad);
    DynamicCam_ParseNextSeqs();
    Circular_MemFreeFunc(pStreamData);
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
        Circular_MemFreeFunc(pStreamData);
        return;
    }
    gpDynCam->pShots = Circular_MemAllocFunc(pStreamData->uSize, MEMTYPE_LOW, 0, "GoDynamicCam.c", 454);
    DynamicCam_CopyScriptData(pStreamData->pData, gpDynCam->pShots, pStreamData->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViews(pStreamData->uSize);
    Circular_MemFreeFunc(pStreamData);
}

// The front end's 'CAMV' handler: as DynamicCam_LoadCAMVfromStream, but nFilesLoaded is left alone
// and the shots are set up by DynamicCam_ParseCameraViewsFE.
static void DynamicCam_LoadCAMVfromStreamFE(UStreamObject* pStreamData) {
    if (gpDynCam->pShots != NULL) {
        Circular_MemFreeFunc(pStreamData);
        return;
    }
    gpDynCam->pShots = Circular_MemAllocFunc(pStreamData->uSize, MEMTYPE_LOW, 0, "GoDynamicCam.c", 495);
    DynamicCam_CopyScriptData(pStreamData->pData, gpDynCam->pShots, pStreamData->uSize / sizeof(CamShot));
    DynamicCam_ParseCameraViewsFE(pStreamData->uSize);
    Circular_MemFreeFunc(pStreamData);
}

// The 'CAMA' handler, the anim pairs: copies them (DynamicCam_CopyAnimPairData) and turns their
// indexes into pointers (DynamicCam_ParseAnimPairs). Frees the object; a second file is dropped.
static void DynamicCam_LoadCAMAfromStream(UStreamObject* pStreamData) {
    if (gpDynCam->pAnimPairs != NULL) {
        Circular_MemFreeFunc(pStreamData);
        return;
    }
    gpDynCam->pAnimPairs = Circular_MemAllocFunc(pStreamData->uSize, MEMTYPE_LOW, 0, "GoDynamicCam.c", 536);
    DynamicCam_CopyAnimPairData(pStreamData->pData, gpDynCam->pAnimPairs,
                                pStreamData->uSize / sizeof(CameraAnimPairs_t));
    DynamicCam_ParseAnimPairs(pStreamData->uSize);
    Circular_MemFreeFunc(pStreamData);
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
    DynamicCamManager_t* pManager = Circular_MemAllocFunc(sizeof(DynamicCamManager_t), MEMTYPE_LOW, 0,
                                                          "GoDynamicCam.c", 938);

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
        Circular_MemFreeFunc(gpDynCam->pSequences);
        gpDynCam->pSequences = NULL;
    }
    if (gpDynCam->pShots != NULL) {
        Circular_MemFreeFunc(gpDynCam->pShots);
        gpDynCam->pShots = NULL;
    }
    if (gpDynCam->pAnimPairs != NULL) {
        Circular_MemFreeFunc(gpDynCam->pAnimPairs);
        gpDynCam->pAnimPairs = NULL;
    }
    if (gpDynCam->pChoices != NULL) {
        Circular_MemFreeFunc(gpDynCam->pChoices);
        gpDynCam->pChoices = NULL;
    }
    gpDynCam->nFilesLoaded = 0;
    Circular_MemFreeFunc(gpDynCam);
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

// Tracking modes 2, 3, 4 and 8: the camera fDist along the direction from the shot's first point
// (nFromPoint) to its second (nToPoint), level for modes 2 and 3, then fSideOffset sideways (both
// as DynamicCam_CalculateFlippedOffsets flips them), then its height from
// DynamicCam_AddHeightOffset. For mode 8 its level offset from pSub (its height becoming pSub's) is
// scaled from the tuning's fTrackOffsetStartScale down to fTrackOffsetEndScale as the share of the
// way to the pin CamScript_GetBallToPinPercent gives (never less than the most seen, kept in the
// script's fMaxBallToPin) goes from the tuning's fTrackOffsetStartShare to fTrackOffsetEndShare;
// when CameraScript_SnapToScript holds, fMaxBallToPin just takes the share. Waits, like
// DynamicCam_TrackPercent, for a ball update when a point is the ball.
void DynamicCam_TrackOffset(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut,
                            f32* pCam, f32* pSub) {
    f32 vFrom[4];
    f32 vTo[4];
    f32 vOff[4];
    f32 fSide;
    f32 fDist;
    f32 fY = pOut[1];
    f32 fFar;
    f32 fScale;

    if ((pShot->nFromPoint == CAMPOINT_BALL || pShot->nToPoint == CAMPOINT_BALL
         || pShot->nFromPoint == CAMPOINT_BALL_AHEAD || pShot->nToPoint == CAMPOINT_BALL_AHEAD)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return;
    }
    DynamicCam_GetLocation(pShot->nFromPoint, nPlayer, vFrom, pScript, pShot, pCam, pSub);
    DynamicCam_GetLocation(pShot->nToPoint, nPlayer, vTo, pScript, pShot, pCam, pSub);
    if (pShot->nTrackMode == CAMTRACK_OFFSET_FLAT || pShot->nTrackMode == CAMTRACK_OFFSET_FLAT_LAGGED) {
        DynamicCam_CalculateFlippedOffsets(pShot, nPlayer, &fSide, &fDist);
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 0, 0, pOut, fDist, fSide);
    } else {
        DynamicCam_CalculateFlippedOffsets(pShot, nPlayer, &fSide, &fDist);
        CamUtils_vGetPositionBetweenTwoPoints(vFrom, vTo, 1, 0, pOut, fDist, fSide);
    }
    if (pShot->nTrackMode == CAMTRACK_OFFSET_TO_PIN) {
        fFar = CamScript_GetBallToPinPercent(nPlayer, pScript);
        if (!CameraScript_SnapToScript(pScript, pShot)) {
            if (fFar > pScript->fMaxBallToPin) {
                pScript->fMaxBallToPin = fFar;
            }
            fFar = pScript->fMaxBallToPin;
            DynamicCam_Vec3Sub(pOut, pSub, vOff);
            vOff[1] = 0.0f;
            if (fFar > gpCamTuning->fTrackOffsetStartShare) {
                if (fFar > gpCamTuning->fTrackOffsetEndShare) {
                    fScale = gpCamTuning->fTrackOffsetEndScale;
                } else {
                    fScale = (fFar - gpCamTuning->fTrackOffsetStartShare)
                            / (gpCamTuning->fTrackOffsetEndShare - gpCamTuning->fTrackOffsetStartShare);
                    fScale = gpCamTuning->fTrackOffsetStartScale - fScale
                            * (gpCamTuning->fTrackOffsetStartScale - gpCamTuning->fTrackOffsetEndScale);
                }
                Vec3_Scale(fScale, vOff, vOff);
                DynamicCam_Vec3Add(vOff, pSub, pOut);
            }
        } else {
            pScript->fMaxBallToPin = fFar;
        }
    }
    DynamicCam_AddHeightOffset(pOut, pScript, pShot, nPlayer, fY);
}

// Tracking modes 5 and 6, the lagging ball-flight camera: its target is fDist along the ball's
// flight direction (level for mode 6, kept within the tuning's fMaxFlightTilt of level by
// DynamicCam_ClampBallVelocity) from the ball (DynamicCam_GetLocation's point 0), plus the tuning's
// fAboveMaxDistShare times (fMaxHeight - height) when the ball is above its most height over the
// script's ground fGroundHeight or fBelowMinDistShare times (height - fMinHeight) below its least (both
// shorten fDist), then fSideOffset sideways (DynamicCam_AddOffset); the heights are the current
// shot's while the next shot has the same mode. When CameraScript_SnapToScript holds the camera
// jumps there. Otherwise it closes the distance to the ball (the tuning's fTrackCloseRate) and
// turns round the ball towards the target (fTrackTurnRate) by a share per NTSC frame of the frame
// time f, scaled while the script's fScriptTime is under the tuning's fTrackEaseInTime (by
// ((fTrackEaseInTime - fCamTime) / fTrackEaseInTime) squared over fTrackCloseRate), times the
// script's f8C when its nInterpType is 4, less for a ball slower than the tuning's
// fTrackMinBallSpeed and while the script's second clock f88 is under fCutEaseInTime; while f88 is
// under the tuning's fCutHeightEaseTime only part of the target's height is taken (the share to the
// power fCutHeightEasePower).
void DynamicCam_TrackBallVelocityLag(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut,
                                     f32* pCam, f32* pSub, f32 f) {
    f32 fStep;
    f32 aFrom[4];
    f32 aBallDir[4];
    f32 aTarget[4];
    f32 aCurOff[4];
    f32 aCurDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aTgtOff[4];
    f32 aTgtDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aNew[4];
    f32 aNewDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    f32 aAxis[4];
    f32 aTurn[4];
    CamShot* pCur;
    f32 fHi;
    f32 fLo;
    f32 fHeight;
    f32 fDist;
    f32 fMove;
    f32 fTurn;
    f32 fEase;
    f32 fT;
    f32 fCurDist;
    f32 fAngle;
    f32 fFrames;
    f32 fY;
    f32 fGrow;
    f32 fRise;

    fY = pOut[1];
    DynamicCam_GetLocation(CAMPOINT_BALL, nPlayer, aFrom, pScript, pShot, pCam, pSub);
    Vec3Copy(gPlayers[nPlayer].ball.vVel, aBallDir);
    if (pShot->nTrackMode == CAMTRACK_BALL_LAG_FLAT) {
        aBallDir[1] = 0.0f;
    }
    if (aBallDir[0] != 0.0f || aBallDir[1] != 0.0f || aBallDir[2] != 0.0f) {
        LLMath_Normalize3(aBallDir, aBallDir);
    }
    DynamicCam_ClampBallVelocity(aBallDir, pOut, aFrom);
    if (pScript->pNextShot != NULL && pScript->pShot->nTrackMode == pScript->pNextShot->nTrackMode) {
        pCur = pScript->pShot;
        fHi = pCur->fMaxHeight;
        fLo = pCur->fMinHeight;
    } else {
        fHi = pShot->fMaxHeight;
        fLo = pShot->fMinHeight;
    }
    fHeight = gPlayers[nPlayer].ball.vPos[1] - pScript->fGroundHeight;
    if (fHeight > fHi) {
        fDist = gpCamTuning->fAboveMaxDistShare * (fHi - fHeight) + pShot->fDist;
    } else if (fHeight < fLo) {
        fDist = gpCamTuning->fBelowMinDistShare * (fHeight - fLo) + pShot->fDist;
    } else {
        fDist = pShot->fDist;
    }
    LLMath_AddScale3(aFrom, aBallDir, fDist, aTarget);
    DynamicCam_AddOffset(aTarget, aBallDir, pScript, pShot, nPlayer, pShot->fSideOffset, fY);
    if (CameraScript_SnapToScript(pScript, pShot)) {
        Vec3Copy(aTarget, pOut);
        return;
    }
    // port: NTSC rate
    fFrames = f / (1.0f / 59.94f);
    fMove = gpCamTuning->fTrackCloseRate * fFrames;
    fTurn = gpCamTuning->fTrackTurnRate * fFrames;
    if (pScript->fScriptTime < gpCamTuning->fTrackEaseInTime) {
        fT = (gpCamTuning->fTrackEaseInTime - pScript->fCamTime) / gpCamTuning->fTrackEaseInTime;
        fT *= fT;
        fEase = (1.0f / gpCamTuning->fTrackCloseRate) * fT;
        fMove *= fEase;
        fTurn *= fEase;
    }
    if (pScript->nInterpType == 4) {
        fMove *= pScript->f8C;
        fTurn *= pScript->f8C;
    }
    if (gPlayers[nPlayer].ball.fSpeed < gpCamTuning->fTrackMinBallSpeed) {
        fGrow = gPlayers[nPlayer].ball.fSpeed / gpCamTuning->fTrackMinBallSpeed;
        fGrow = fGrow * fGrow;
        fGrow = fGrow * fGrow;
        fMove *= fGrow;
        fTurn *= fGrow;
    }
    if (pScript->f88 < gpCamTuning->fCutEaseInTime) {
        fMove *= pScript->f88 / gpCamTuning->fCutEaseInTime;
        fTurn *= pScript->f88 / gpCamTuning->fCutEaseInTime;
    }
    if (pScript->f88 < gpCamTuning->fCutHeightEaseTime) {
        fRise = powf(pScript->f88 / gpCamTuning->fCutHeightEaseTime, gpCamTuning->fCutHeightEasePower);
        fRise *= fFrames;
        aTarget[1] = fRise * (aTarget[1] - pOut[1]) + pOut[1];
    }
    DynamicCam_Vec3Sub(pOut, aFrom, aCurOff);
    DynamicCam_Vec3Sub(aTarget, aFrom, aTgtOff);
    fCurDist = (f32)Math_Sqrt(LLMath_LengthSquared3(aCurOff));
    fStep = (f32)Math_Sqrt(LLMath_LengthSquared3(aTgtOff)) - fCurDist;
    fStep *= fMove;
    if (aCurOff[0] != 0.0f || aCurOff[1] != 0.0f || aCurOff[2] != 0.0f) {
        LLMath_Normalize3(aCurOff, aCurDir);
    } else {
        aCurDir[0] = 0.0f;
        aCurDir[1] = 0.0f;
        aCurDir[2] = 0.0f;
    }
    if (aTgtOff[0] != 0.0f || aTgtOff[1] != 0.0f || aTgtOff[2] != 0.0f) {
        LLMath_Normalize3(aTgtOff, aTgtDir);
    } else {
        aTgtDir[0] = 0.0f;
        aTgtDir[1] = 0.0f;
        aTgtDir[2] = 0.0f;
    }
    fAngle = acosf(Vec3_Dot(aCurDir, aTgtDir));
    fTurn = fAngle * fTurn;
    vec4flt_CrossProduct(aCurDir, aTgtDir, aAxis);
    if (aAxis[0] != 0.0f || aAxis[1] != 0.0f || aAxis[2] != 0.0f) {
        LLMath_Normalize3(aAxis, aAxis);
    }
    Vec3_Scale(fTurn, aAxis, aAxis);
    Quat_BuildFromVector(aAxis, aTurn);
    aCurDir[3] = 0.0f;
    Quat_RotateVector(aTurn, aCurDir, aNew);
    if (aNew[0] != 0.0f || aNew[1] != 0.0f || aNew[2] != 0.0f) {
        LLMath_Normalize3(aNew, aNewDir);
    } else {
        aNewDir[0] = 0.0f;
        aNewDir[1] = 0.0f;
        aNewDir[2] = 0.0f;
    }
    Vec3_Scale(fCurDist + fStep, aNewDir, aNew);
    DynamicCam_Vec3Add(aFrom, aNew, pOut);
}

// Tracking mode 7, the tight ball-flight camera: fDist along the ball's flight direction from the
// ball (DynamicCam_GetLocation's point 0), plus the tuning's fAboveMaxDistShare times (fMaxHeight -
// height) when the ball's height over the script's ground fGroundHeight is above the shot's most height
// fMaxHeight, or fBelowMinDistShare times (height - fMinHeight) when under its least height
// fMinHeight (the current shot's heights while the next shot has the same mode); both shorten
// fDist. Then fSideOffset sideways
// (DynamicCam_AddOffset). Nothing moves while the ball is slower than the tuning's
// fTrackMinBallSpeed.
void DynamicCam_TrackBallVelocityTight(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript,
                                       f32* pOut, f32* pCam, f32* pSub) {
    f32 vFrom[4];
    f32 vDir[4];
    f32 vPos[4];
    f32 fY = pOut[1];
    f32 fHeight;
    f32 fMax;
    f32 fMin;
    f32 fDist;

    DynamicCam_GetLocation(CAMPOINT_BALL, nPlayer, vFrom, pScript, pShot, pCam, pSub);
    Vec3Copy(gPlayers[nPlayer].ball.vVel, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        LLMath_Normalize3(vDir, vDir);
    }
    if (gPlayers[nPlayer].ball.fSpeed < gpCamTuning->fTrackMinBallSpeed) return;
    if (pScript->pNextShot != NULL && pScript->pShot->nTrackMode == pScript->pNextShot->nTrackMode) {
        fMax = pScript->pShot->fMaxHeight;
        fMin = pScript->pShot->fMinHeight;
    } else {
        fMax = pShot->fMaxHeight;
        fMin = pShot->fMinHeight;
    }
    fHeight = gPlayers[nPlayer].ball.vPos[1] - pScript->fGroundHeight;
    if (fHeight > fMax) {
        fDist = gpCamTuning->fAboveMaxDistShare * (fMax - fHeight) + pShot->fDist;
    } else if (fHeight < fMin) {
        fDist = gpCamTuning->fBelowMinDistShare * (fHeight - fMin) + pShot->fDist;
    } else {
        fDist = pShot->fDist;
    }
    LLMath_AddScale3(vFrom, vDir, fDist, vPos);
    DynamicCam_AddOffset(vPos, vDir, pScript, pShot, nPlayer, pShot->fSideOffset, fY);
    Vec3Copy(vPos, pOut);
}

// Tracking mode 9: the camera at the shot's first point (DynamicCam_GetLocation for nFromPoint).
void DynamicCam_TrackFixed(CamShot* pShot, int nPlayer, CameraScriptSettings_t* pScript, f32* pOut, f32* pCam,
                           f32* pSub) {
    f32 vPos[4];

    DynamicCam_GetLocation(pShot->nFromPoint, nPlayer, vPos, pScript, pShot, pCam, pSub);
    Vec3Copy(vPos, pOut);
}

// A tracking point of kind nKind for the shot, into pOut (the body parts are TW07's names for these
// bones): 0 the ball (DynamicCam_GetSmoothBallLocation), 1 Player.vBall, 2 between the golfer's
// feet (bones 0x39 and 0x47), 4 the waist (bone 1), 6 the head (bone 10), 8 the chest (bone 7), 9
// Player.vTarget2, 10 the pin, 11 the player's tee, 12 the script's v50, 16 the script's own camera
// point (set on the fairway by CamScript_GetCameraOnFairwayPos for its shot or
// CamScript_PutBackOnFairway for its next shot while CameraScript_SnapToScript holds, else the
// script's vCamPos), 17..19 the head, chest and waist moved one unit along their bone's third matrix
// row, 20 and 21 the shot's other point (the golfer's origin, bone 0, when that is 20 or 21 too)
// moved one unit along bone 0's first or third row, 24 the shot's v20, 25 (0, 0, 100). Any other
// kind leaves pOut as it was.
void DynamicCam_GetLocation(int nKind, int nPlayer, f32* pOut, CameraScriptSettings_t* pScript,
                            CamShot* pShot, f32* pCam, f32* pSub) {
    f32 vBone47[4];
    f32 vBone39[4];
    f32 vMid[4];
    f32 vBone10[4];
    f32 vBone1[4];
    f32 vBone7[4];
    f32 vDir7[4];
    f32 vDir10[4];
    f32 vDir1[4];
    f32 vFrom[4];
    f32 vDir[4];
    f32 (*pMatrix)[4];
    int nOther;
    int nPin;
    int nTee;
    CourseInfo* pCourse;

    if (nKind == pShot->nFromPoint) {
        nOther = pShot->nToPoint;
    } else {
        nOther = pShot->nFromPoint;
    }
    switch (nKind) {
    case CAMPOINT_BALL:
        DynamicCam_GetSmoothBallLocation(pScript, pShot, nPlayer, pOut, 0);
        break;
    case 2:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x39, vBone39);
        Character_GetBonePos(gPlayers[nPlayer].pChar, 0x47, vBone47);
        DynamicCam_Vec3Add(vBone39, vBone47, vMid);
        Vec3_Scale(0.5f, vMid, vMid);
        Vec3Copy(vMid, pOut);
        break;
    case 6:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 10, vBone10);
        Vec3Copy(vBone10, pOut);
        break;
    case 8:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 7, vBone7);
        Vec3Copy(vBone7, pOut);
        break;
    case 4:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        Vec3Copy(vBone1, pOut);
        break;
    case 1:
        Vec3Copy(gPlayers[nPlayer].vBall, pOut);
        break;
    case 9:
        Vec3Copy(gPlayers[nPlayer].vTarget2, pOut);
        break;
    case 12:
        Vec3Copy(pScript->v50, pOut);
        break;
    case 10:
        nPin = Game_CurrentPinSet();
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->pin[nPin].x, pOut);
        break;
    case 11:
        nTee = gSession.nTeeSet[nPlayer];
        pCourse = Ter_GetTGD();
        Vec3Copy(&pCourse->tee[nTee].x, pOut);
        break;
    case 24:
        Vec3Copy(pShot->v20, pOut);
        break;
    case 16:
        if (CameraScript_SnapToScript(pScript, pShot)) {
            if (pShot == pScript->pShot) {
                CamScript_GetCameraOnFairwayPos(pScript, pOut, pCam, nPlayer, pScript->pB4, pSub, NULL);
                Vec3Copy(pOut, pScript->vCamPos);
                pScript->bFairwayFix = 1;
            } else if (pShot == pScript->pNextShot) {
                CamScript_PutBackOnFairway(pScript, pOut, pCam, nPlayer, pScript->pB4, pSub);
                Vec3Copy(pOut, pScript->vNextCamPos);
            }
        } else if (pShot == pScript->pShot) {
            Vec3Copy(pScript->vCamPos, pOut);
        } else if (pShot == pScript->pNextShot) {
            // the script's vCamPos for the next shot too (vNextCamPos is where the fairway branch keeps it)
            Vec3Copy(pScript->vCamPos, pOut);
        }
        break;
    case 17:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 10, vBone10);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 10);
        if (pMatrix == NULL) {
            Vec3Copy(vBone10, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir10);
        if (0.0f != vDir10[0] || 0.0f != vDir10[1] || 0.0f != vDir10[2]) {
            LLMath_Normalize3(vDir10, vDir10);
        }
        DynamicCam_Vec3Add(vBone10, vDir10, pOut);
        break;
    case 18:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 7, vBone7);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 7);
        if (pMatrix == NULL) {
            Vec3Copy(vBone7, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir7);
        if (0.0f != vDir7[0] || 0.0f != vDir7[1] || 0.0f != vDir7[2]) {
            LLMath_Normalize3(vDir7, vDir7);
        }
        DynamicCam_Vec3Add(vBone7, vDir7, pOut);
        break;
    case 19:
        Character_GetBonePos(gPlayers[nPlayer].pChar, 1, vBone1);
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 1);
        if (pMatrix == NULL) {
            Vec3Copy(vBone1, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir1);
        if (0.0f != vDir1[0] || 0.0f != vDir1[1] || 0.0f != vDir1[2]) {
            LLMath_Normalize3(vDir1, vDir1);
        }
        DynamicCam_Vec3Add(vBone1, vDir1, pOut);
        break;
    case 20:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            Character_GetBonePos(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[0], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            LLMath_Normalize3(vDir, vDir);
        }
        DynamicCam_Vec3Add(vFrom, vDir, pOut);
        break;
    case 21:
        if (nOther != 20 && nOther != 21) {
            DynamicCam_GetLocation(nOther, nPlayer, vFrom, pScript, pShot, pCam, pSub);
        } else {
            Character_GetBonePos(gPlayers[nPlayer].pChar, 0, vFrom);
        }
        pMatrix = Character_GetBoneMatrix(gPlayers[nPlayer].pChar, 0);
        if (pMatrix == NULL) {
            Vec3Copy(vFrom, pOut);
            break;
        }
        Vec3Copy(pMatrix[2], vDir);
        if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
            LLMath_Normalize3(vDir, vDir);
        }
        DynamicCam_Vec3Add(vFrom, vDir, pOut);
        break;
    case 25:
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 100.0f;
        break;
    }
}

// Picks the camera sequence for state nKind of nPlayer's shot, from the first 50 sequences with
// shot choices whose state, club, shot (a: TW07's allowGenericShotTypes), player type, course, par,
// game mode, start lie (nLie, or the tee on the tee; DynamicCam_BallLieToCameraLie), end lie
// (nClass through DynamicCam_MaterialToCameraLie), pin height over the ball (f2C..f30) and shot
// distance fDist (f24..f28) all fit: one at random by their weights (f34). With none, the last
// default ("DEF") sequence for the state on this course, else the first default one for the state
// that has choices, else NULL. NULL without course data.
CameraSequence_t* DynamicCam_ChooseSequence(int nPlayer, int nLie, int nClass, int nKind, u8 a, f32 fDist) {
    int anPicked[DYNAMICCAM_MAX_POSSIBLE];
    CourseInfo* pCourse;
    CameraSequence_t* pSeq;
    int i;
    int nPicked;
    int nSum;
    int nDefault;
    int nTee;
    int nClassBit;
    int nPinSet;
    u32 uRand;
    int nRoll;
    f32 fHeight;
    f32 fTotal;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = DynamicCam_BallLieToCameraLie(nLie, nPlayer);
    }
    nClassBit = DynamicCam_MaterialToCameraLie(nClass, nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    for (i = 0; i < gpDynCam->nSequences; i++) {
        if (nPicked >= DYNAMICCAM_MAX_POSSIBLE) {
            break;
        }
        if (gpDynCam->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], nKind) &&
                DynamicCam_MatchCourse(&gpDynCam->pSequences[i])) {
                nDefault = i;
            } else if (DynamicCam_MatchStateType(&gpDynCam->pSequences[i], nKind) &&
                       DynamicCam_MatchSeqClub(&gpDynCam->pSequences[i], nPlayer) &&
                       DynamicCam_MatchSeqShot(&gpDynCam->pSequences[i], nPlayer, a) &&
                       DynamicCam_MatchPlayerType(&gpDynCam->pSequences[i], nPlayer) &&
                       DynamicCam_MatchCourse(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchPars(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uStartLies, nTee) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uEndLies, nClassBit) &&
                       DynamicCam_MatchHeightDiff(&gpDynCam->pSequences[i], nPlayer, fHeight) &&
                       DynamicCam_ModeType(&gpDynCam->pSequences[i])) {
                pSeq = &gpDynCam->pSequences[i];
                if (fDist >= pSeq->fMinDist && fDist <= pSeq->fMaxDist && pSeq->nChoices > 0) {
                    anPicked[nPicked] = i;
                    nPicked++;
                }
            }
        }
    }
    if (nPicked == 0) {
        if (nDefault >= 0) {
            return &gpDynCam->pSequences[nDefault];
        }
        for (i = 0; i < gpDynCam->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], nKind) &&
                gpDynCam->pSequences[i].nChoices > 0) {
                return &gpDynCam->pSequences[i];
            }
        }
        return NULL;
    }
    uRand = Misc_RandFunc(1);
    fTotal = 0.0f;
    for (i = 0; i < nPicked; i++) {
        fTotal += gpDynCam->pSequences[anPicked[i]].f34;
    }
    nRoll = uRand % (int)(1000.0f * fTotal);
    for (i = 0; i < nPicked; i++) {
        if ((f32)(nRoll - nSum) < 1000.0f * gpDynCam->pSequences[anPicked[i]].f34) {
            return &gpDynCam->pSequences[anPicked[i]];
        }
        nSum += (int)(1000.0f * gpDynCam->pSequences[anPicked[i]].f34);
    }
    return &gpDynCam->pSequences[anPicked[0]];
}

// DynamicCam_ChooseSequence's pick for the pre-flight cameras: state nKind (3 becomes 28 when an
// object or hazard is within the tuning's fBallObstructionDist of the ball, and 3 again for the
// last fallback), the ball's distance to the pin (GameAnalysis_GetCurrentDistanceToPin) within
// f24..f28 (DynamicCam_MatchDistToPin) in place of a shot distance, no end lie, generic shot types
// allowed.
CameraSequence_t* DynamicCam_ChoosePreFlightSequence(int nPlayer, int nLie, int nKind) {
    int anPicked[DYNAMICCAM_MAX_POSSIBLE];
    CourseInfo* pCourse;
    int i;
    int nPicked;
    int nSum;
    int nDefault;
    int nTee;
    int nPinSet;
    u32 uRand;
    int nRoll;
    f32 fPinDist;
    f32 fHeight;
    f32 fTotal;

    nPicked = 0;
    nSum = 0;
    nDefault = -1;
    if (Player_OnTee(nPlayer)) {
        nTee = 1;
    } else {
        nTee = DynamicCam_BallLieToCameraLie(nLie, nPlayer);
    }
    fPinDist = GameAnalysis_GetCurrentDistanceToPin(nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    if (pCourse == NULL) {
        return NULL;
    }
    fHeight = pCourse->pin[nPinSet].y - gPlayers[nPlayer].vBall[1];
    if (nKind == CAMSTATE_SWING
        && Ter_CheckObjectAndHazardObstruction(gPlayers[nPlayer].ball.vPos,
                                               gpCamTuning->fBallObstructionDist, 1, 0, 0.0f, 1,
                                               gpCamTuning->fObstructionMaxSlope)) {
        nKind = CAMSTATE_SWING_OBSTRUCTED;
    }
    for (i = 0; i < gpDynCam->nSequences; i++) {
        if (nPicked >= DYNAMICCAM_MAX_POSSIBLE) {
            break;
        }
        if (gpDynCam->pSequences[i].nChoices > 0) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], nKind) &&
                DynamicCam_MatchCourse(&gpDynCam->pSequences[i])) {
                nDefault = i;
            } else if (DynamicCam_MatchStateType(&gpDynCam->pSequences[i], nKind) &&
                       DynamicCam_MatchSeqClub(&gpDynCam->pSequences[i], nPlayer) &&
                       DynamicCam_MatchSeqShot(&gpDynCam->pSequences[i], nPlayer, 1) &&
                       DynamicCam_MatchPlayerType(&gpDynCam->pSequences[i], nPlayer) &&
                       DynamicCam_MatchDistToPin(&gpDynCam->pSequences[i], nPlayer, fPinDist) &&
                       DynamicCam_MatchCourse(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchPars(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchHeightDiff(&gpDynCam->pSequences[i], nPlayer, fHeight) &&
                       DynamicCam_ModeType(&gpDynCam->pSequences[i]) &&
                       DynamicCam_MatchLies(gpDynCam->pSequences[i].uStartLies, nTee)) {
                anPicked[nPicked] = i;
                nPicked++;
            }
        }
    }
    if (nPicked == 0) {
        if (nDefault >= 0) {
            return &gpDynCam->pSequences[nDefault];
        }
        if (nKind == CAMSTATE_SWING_OBSTRUCTED) {
            nKind = CAMSTATE_SWING;
        }
        for (i = 0; i < gpDynCam->nSequences; i++) {
            if (DynamicCam_IsDefualtSeq(&gpDynCam->pSequences[i], nKind) &&
                gpDynCam->pSequences[i].nChoices > 0) {
                return &gpDynCam->pSequences[i];
            }
        }
        return NULL;
    }
    uRand = Misc_RandFunc(1);
    fTotal = 0.0f;
    for (i = 0; i < nPicked; i++) {
        fTotal += gpDynCam->pSequences[anPicked[i]].f34;
    }
    nRoll = uRand % (int)(1000.0f * fTotal);
    for (i = 0; i < nPicked; i++) {
        if ((f32)(nRoll - nSum) < 1000.0f * gpDynCam->pSequences[anPicked[i]].f34) {
            return &gpDynCam->pSequences[anPicked[i]];
        }
        nSum += (int)(1000.0f * gpDynCam->pSequences[anPicked[i]].f34);
    }
    return &gpDynCam->pSequences[anPicked[0]];
}

// The anim pair named szName (an animation's name, case ignored) gives a sequence (*ppSeq) or a
// shot (*ppShot), returning 1: its pChanceSeq 39 times in 100 when that has shot choices, else by
// its kind: 14 one of pRandomSeq1, pRandomSeq2 and pRandomSeq3 at random (pRandomSeq3 when the
// pick is missing), 13 its shot. 0 when no pair gives one.
u8 DynamicCamSearchForPairedSequence(char* szName, CameraSequence_t** ppSeq, CamShot** ppShot) {
    int i;
    u32 nPick;

    for (i = 0; i < gpDynCam->nAnimPairs; i++) {
        if (stricmp(gpDynCam->pAnimPairs[i].szName, szName) != 0) {
            continue;
        }
        if (gpDynCam->pAnimPairs[i].pChanceSeq != NULL && gpDynCam->pAnimPairs[i].pChanceSeq->nChoices > 0 &&
            Misc_RandFunc(1) % 100 > 60) {
            *ppSeq = gpDynCam->pAnimPairs[i].pChanceSeq;
            return 1;
        }
        if (gpDynCam->pAnimPairs[i].nKind == 14) {
            nPick = Misc_RandFunc(1) % 3;
            if (nPick == 0 && gpDynCam->pAnimPairs[i].pRandomSeq1 != NULL) {
                *ppSeq = gpDynCam->pAnimPairs[i].pRandomSeq1;
                return 1;
            }
            if (nPick == 1 && gpDynCam->pAnimPairs[i].pRandomSeq2 != NULL) {
                *ppSeq = gpDynCam->pAnimPairs[i].pRandomSeq2;
                return 1;
            }
            if (gpDynCam->pAnimPairs[i].pRandomSeq3 != NULL) {
                *ppSeq = gpDynCam->pAnimPairs[i].pRandomSeq3;
                return 1;
            }
        } else if (gpDynCam->pAnimPairs[i].nKind == 13) {
            *ppShot = gpDynCam->pAnimPairs[i].pShot;
            return 1;
        }
    }
    return 0;
}

// The sequence or shot paired with the golfer's animation (DynamicCamSearchForPairedSequence): with
// b (TW07's checkReaction) the clip in Character.pReactionClip when there is one, else the clip it
// is playing. While the GameBreaker letterbox is up (GameEffects_IsLetterboxOn) the name with "LB"
// in front is tried first. 1 when one is found; *ppSeq and *ppShot are cleared first, and a NULL out pointer
// gives 0.
u8 DynamicCam_ChoosePairedSequenceOrCamera(int nPlayer, u8 b, CameraSequence_t** ppSeq, CamShot** ppShot) {
    char szName[0x18];          // the frame allows 12 to 24 bytes; the true size is unknown
    char* pName = NULL;

    if (ppSeq == NULL || ppShot == NULL) {
        return 0;
    }
    *ppSeq = NULL;
    *ppShot = NULL;
    if (b && gPlayers[nPlayer].pChar->pReactionClip != NULL) {
        pName = gPlayers[nPlayer].pChar->pReactionClip->name;
    }
    if (pName == NULL && gPlayers[nPlayer].pChar->pCurClip != NULL) {
        pName = gPlayers[nPlayer].pChar->pCurClip->name;
    }
    if (pName != NULL) {
        if (GameEffects_IsLetterboxOn()) {
            sprintf(szName, "LB%s", pName);
            if (DynamicCamSearchForPairedSequence(szName, ppSeq, ppShot) == 1) {
                return 1;
            }
        }
        if (DynamicCamSearchForPairedSequence(pName, ppSeq, ppShot) == 1) {
            return 1;
        }
    }
    return 0;
}

// A default sequence for state nKind: one of that state (DynamicCam_MatchStateType) whose name
// starts with "DEF". 0 for NULL.
u8 DynamicCam_IsDefualtSeq(CameraSequence_t* pSequence, int nKind) {
    if (pSequence == NULL) return 0;
    if (!DynamicCam_MatchStateType(pSequence, nKind)) return 0;
    if (pSequence->szName[0] != 'D') return 0;
    if (pSequence->szName[1] != 'E') return 0;
    if (pSequence->szName[2] != 'F') return 0;
    return 1;
}

// The camera lie for each end lie (DynamicCam_MaterialToCameraLie; entry 1 is decided there by the
// ball's lie) and for each ball lie (DynamicCam_BallLieToCameraLie). A sequence's uStartLies and uEndLies
// hold one bit per camera lie.
s32 gMaterialToCamLies[20] = {0, 1, 2, 6, 2, 3, 4, 5, 3, 3, 3, 3, 6, 3, 3, 3, 5, 3, 6, 7};
s32 gBallLieToCamLies[17] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 6, 2, 3, 6, 5, 5, 5, 7};

// The camera lie for end lie n (0..19), from gMaterialToCamLies; 0 for any other n. Lie 1 depends
// on the ball: 1 when it lies on lie 0, else 2.
s32 DynamicCam_MaterialToCameraLie(u32 n, int nPlayer) {
    s32 nRet;

    if (n > 19) return 0;
    if (n == 1) {
        nRet = 2;
        if (gPlayers[nPlayer].ball.nLie == 0) {
            nRet = 1;
        }
        return nRet;
    }
    return gMaterialToCamLies[n];
}

// The camera lie for ball lie n, from gBallLieToCamLies (17 entries; n is not checked). nPlayer is
// not used.
s32 DynamicCam_BallLieToCameraLie(int n, int nPlayer) {
    return gBallLieToCamLies[n];
}

// The sequence suits the player's club: b45 picks the clubs (0 any, 1 woods, 2 5..9 irons,
// 3 1..5 irons, 4 wedges, 5 putter, 6 woods and irons, 7 woods and 1..5 irons, 8 all but the
// putter, 9 5 iron to the wedges, 10 irons and wedges).
u8 DynamicCam_MatchSeqClub(CameraSequence_t* pSequence, int nPlayer) {
    int nClub = gPlayers[nPlayer].nClub;

    switch (pSequence->b45) {
    case 0:
        return 1;
    case 1:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_7WOOD_e) return 1;
        return 0;
    case 2:
        if (nClub >= CLUB_5IRON_e && nClub <= CLUB_9IRON_e) return 1;
        return 0;
    case 3:
        if (nClub >= CLUB_1IRON_e && nClub <= CLUB_5IRON_e) return 1;
        return 0;
    case 4:
        if (nClub >= CLUB_PITCHINGWEDGE_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 5:
        if (nClub >= CLUB_PUTTER_e && nClub <= CLUB_PUTTER_e) return 1;
        return 0;
    case 6:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_9IRON_e) return 1;
        return 0;
    case 8:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 9:
        if (nClub >= CLUB_5IRON_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 10:
        if (nClub >= CLUB_1IRON_e && nClub <= CLUB_HIGHLOBWEDGE_e) return 1;
        return 0;
    case 7:
        if (nClub >= CLUB_DRIVER1_e && nClub <= CLUB_5IRON_e) return 1;
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
// them changes sign: the side (-fSideOffset), or the distance (-fDist) for a shot with a point of kind 21
// (DynamicCam_GetLocation).
void DynamicCam_CalculateFlippedOffsets(CamShot* pShot, int nPlayer, f32* pA, f32* pB) {
    if (pShot != NULL) {
        if (CameraScript_FlipCameraForLefty(nPlayer, pShot)) {
            if (pShot->nFromPoint == 21 || pShot->nToPoint == 21) {
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

    if (nKind == 1 || (u8)(nKind - 2) <= 4U || nKind == 7) {
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
