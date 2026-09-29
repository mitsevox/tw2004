// sitdev.h (our name): the commentary scripts ("situation development", TW07's Golf\SitDev): the
// loaded script tables (SitDevFile.c), their run-time state (SitDev.c) and what the other SitDev
// files share.

#ifndef SITDEV_H
#define SITDEV_H

#include "engine.h"
#include "endian.h"
#include "ball.h"

#define SITDEV_NUM_VALUES 96

// An event queued for the scripts (SitDev_QueueEvent adds them, SitDev_ProcessEventQueue runs and
// clears them).
typedef struct SitDevEvent {
    s32   nPlayer;              // 0x00
    u8    nEvent;               // 0x04  event.c's event number
    u8    unk5[3];
} SitDevEvent;

// The block gpSitDevData points at (gSitDevData, 0x140 bytes).
typedef struct SitDevData {
    u16   aValue[SITDEV_NUM_VALUES];            // 0x000  set through _SetStateVecAndCondition
    u32   aSetBits[SITDEV_NUM_VALUES / 32];     // 0x0C0  bit n: aValue[n] has been set
    void* pCC;                                  // 0x0CC  SitDev_LoadScripts' argument; freed by
                                                //        SitDev_vCloseModule
    void* pD0;                                  // 0x0D0  freed by SitDev_vCloseModule when set
    u8*   pGroupFlags;                          // 0x0D4  a byte per situation group (nGroups): 1
                                                //        once the group has run; allocated by
                                                //        SitDev_LoadScripts, cleared by
                                                //        SitDev_ClearGroupFlags, freed by
                                                //        SitDev_vCloseModule
    u8    abPlayed[14];                         // 0x0D8  per kind of action: one has played already
    u8    unkE6[2];
    struct SitDevResponse* pE8;                 // 0x0E8  the last line played
                                                //        (SitDev_TriggerResponse kind 1)
    SitDevEvent aEvents[10];                    // 0x0EC  this frame's events, n13C of them
    s32   n13C;                                 // 0x13C  cleared with the block by SitDev_vInitModule
} SitDevData;
LAYOUT_ASSERT(SitDevData, 0x140);

extern SitDevData  gSitDevData;
extern SitDevData* gpSitDevData;    // 0x802811B8 (.sdata): &gSitDevData

// A halfword the loader rewrites (SitDev_SwapTables): on disc its two bit-fields are in the other bit
// order, so it reads the raw value and stores its low 11 bits and its top 5 bits back as fields.
typedef union SitDevBits {
    u16 uRaw;
    struct {
        u16 n11 : 11;           // bits 15..5
        u16 nFileIndex : 5;     // bits 4..0: the script file the entry comes from (TW07's
                                // iFileIndex)
    } s;
} SitDevBits;

// A situation: an entry of the scripts' first table (SitDevHeader.pSituations, 0x30 bytes; TW07's
// Situation), tried by SitDev_ProcessEventQueue. Its conditions (SitDev_ConditionsMatch): for each
// bit n set in auTests, in order, test k compares SitDevData.aValue[n] with aArg[k] by aOp[k].
typedef struct SitDevSituation {
    u8         nGroup;          // 0x00  its group, whose situations run once (its
                                //       SitDevData.pGroupFlags byte); 0: none
    u8         nEvent;          // 0x01  the queued event it waits for; 0: any
    SitDevBits b2;              // 0x02  b2.s.nFileIndex: its script file
    u32        auTests[SITDEV_NUM_VALUES / 32];   // 0x04
    u8         aOp[8];          // 0x10  0 always true, 1 value == argument, 2 !=, 3 value >
                                //       argument, 4 value < argument, 5 a bit in common
    u16        aArg[8];         // 0x18
    u16        aActions[4];     // 0x28  SitDevHeader.pActions entries to try
                                //       (SitDev_InvokeMultipleActions), 0xFFF0 ends
} SitDevSituation;

// An action: an entry of the scripts' second table (SitDevHeader.pActions, 0x68 bytes; TW07's
// Action): what a situation does.
typedef struct SitDevAction {
    u8         nKind;           // 0x00  its SitDevData.abPlayed byte; kinds 1 and 2 are commentary
    u8         nChance;         // 0x01  percent
    u8         unk2;
    u8         bSound;          // 0x03  nonzero: play a sound from aList
                                //       (SitDev_InvokeCommentaryBank), else run the responses it
                                //       lists (SitDev_InvokeAction)
    u16        aList[50];       // 0x04  a deck (SitDev_NumEntries..
                                //       SitDev_ChooseRandomResponseNoRepeat); 0xFFF0 ends
} SitDevAction;

// A response: an entry of the scripts' third table (SitDevHeader.pResponses, 8 bytes; TW07's
// Response): one thing to do.
typedef struct SitDevResponse {
    u8         nKind;           // 0x00  SitDev_TriggerResponse's switch; also its
                                //       SitDevData.abPlayed byte
    u8         unk1;
    SitDevBits b2;              // 0x02
    u32        n4;              // 0x04  its argument (a sound, a music, ...)
} SitDevResponse;

// The situation scripts' header (TW07's SitDevHeader; gpSitDevScripts): the block whose address is
// the first word of SitDev_LoadScripts' argument. SitDev_BindHeader turns the offsets at 0x14..0x20
// into pointers.
typedef struct SitDevHeader {
    u32              nSituations;   // 0x00  entries at pSituations
    u32              nActions;      // 0x04  entries at pActions
    u32              nResponses;    // 0x08  entries at pResponses
    u32              nNameWords;    // 0x0C  words at pNames (four per name)
    u32              nGroups;       // 0x10  situation groups: bytes in SitDevData.pGroupFlags
    SitDevSituation* pSituations;   // 0x14
    SitDevAction*    pActions;      // 0x18  (SitDev_InvokeMultipleActions)
    SitDevResponse*  pResponses;    // 0x1C
    u8*              pNames;        // 0x20  16-byte names (state value 86: an animation clip's)
} SitDevHeader;

extern SitDevHeader* gpSitDevScripts;  // 0x80282208 (.sbss), NULL until the scripts are loaded

// The byte-swap layouts of the header, a situation, an action and a response (ByteSwap_Records).
extern SwapField gSitDevHeaderSwap[9];
extern SwapField gSitDevSituationSwap[7];
extern SwapField gSitDevActionSwap[5];
extern SwapField gSitDevResponseSwap[4];

// A situation zone, from chunk 5 of the hole's data (SitDev_NetworkLoadCallback): an outline (with its
// net.nNumNodes nodes), and then the zone's bits (a u32 right after the last node).
typedef struct SitDevZone {
    TNetwork net;               // 0x0
} SitDevZone;

extern SitDevZone* gSitDevCommentaryZones[10];  // the hole's zones
extern s32 gSitDevNumCommentaryZones;           // how many

// The look-ahead ball's commentary (situation event 29, its first bounce; SitDev.c): a situation
// for it that tests value 64 sets both flags as it runs; SitDev_InvokeMultipleActions clears
// gSitDevPredictionPending after the run, and gSitDevPredictionVoiced too when nothing played.
extern u8 gSitDevPredictionVoiced;     // a line about where the look-ahead ball lands played
extern u32 gSitDevPredictedHitClass;   // the class of what the look-ahead ball hit (state value 64,
                                       // SitDevStateVector.c)
extern u8 gSitDevPredictionPending;    // such a situation is running

// Per value: nonzero when the scripts compare it as signed (SitDev_CompareConditions).
extern u8 lbl_80193188[88];

// Per game mode: the bit SitDev_TranslateGameMode returns for it, -1 for none.
extern s32 gSitDevGameModeBits[28];

void SitDev_ClearGroupFlags(void);         // SitDevTrigger.c: clear SitDevData.pGroupFlags
extern s32 gSitDevEmotionSet[5];           // per player; cleared by SitDev_ClearEmotionStates
extern s32 gSitDevPredictedEmotionSet[5];  // per player; 1: SitDev_PredictedEmotionAvailable is true

// Store uValue in pValues[nIndex] and set bit nIndex of pSetBits.
void _SetStateVecAndCondition(u16* pValues, int nIndex, u16 uValue, u32* pSetBits);

void SitDev_QueueEvent(int nPlayer, int a, u8 nEvent);    // event.c's handlers call it for most events

// SitDevStateVector.c
void SitDev_SetupStateVector(int nPlayer, u8 nKind);
u8   SitDev_ConditionsMatch(SitDevSituation* pEntry, SitDevData* pData, int nPlayer);
// SitDevTrigger.c
void SitDev_InvokeMultipleActions(SitDevSituation* pEntry, int nFile, int nPlayer, u8 nEvent);
// SitDevMisc.c
void SitDev_ClearCupBevelFlag(void);
void SitDev_ClearEmotionStates(void);
void SitDev_ClearBallThatWasHit(void);
void SitDev_SetBallHitTime(struct Ball* pBall);
// SitDevFile.c
void SitDev_LoadScripts(SitDevHeader** ppScripts);  // the 'sscr' stream handler
// SitDevCommentaryZones.c
void SitDev_NetworkLoadCallback(u8* pChunk);        // the course loader for chunk 5

#endif
