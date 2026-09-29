// SitDev.c (EA's name: TW07's Golf\SitDev\SitDev.c has these functions in this order): the core
// of the commentary scripts ("situation development"). The state block every SitDev file reaches
// through gpSitDevData, set up at round start and freed at round end, the loaders of a hole's
// commentary zones and scripts, and the queue of events the situation scripts (SitDevFile.c loads
// them) react to: event.c's handlers queue each moment of a shot (SitDev_QueueEvent), and
// SitDev_ProcessEventQueue runs every script whose event came up, then empties the queue. It ends
// with _SetStateVecAndCondition, which stores each state value the scripts test (TW07 has it as an
// inline in SitDevStateVector.h), and a vector subtract.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "game.h"
#include "sitdev.h"
#include "terrain.h"
#include "core/easb.h"

SitDevData gSitDevData;                     // the commentary scripts' state (SitDev.c's)
SitDevData* gpSitDevData = &gSitDevData;    // every SitDev file reaches it through this


void SitDev_Vec3Sub(f32* pA, f32* pB, f32* pOut);

// .sbss, defined in reverse address order
u8 gSitDevBreaklinePassedCupQueued;    // situation event 26 (the putt's break line passed the cup)
                                       // has been queued since the last event 2 or 3
u8 gSitDevPredictionPending;    // set as a situation for event 29 (the look-ahead ball's first
                                // bounce) that tests value 64 runs; SitDev_InvokeMultipleActions
                                // clears it after the run
u8 gSitDevPredictionVoiced;     // such a situation's actions played something: a line about where
                                // the look-ahead ball lands. State value 87 is it and a lie class
                                // other than gSitDevPredictedHitClass. Cleared at every shot set-up

// Round start (GO_vInitIG): clears the commentary scripts' state block (SitDevData: no line played,
// no events queued), registers the loader for a hole's commentary zones (course chunk 5,
// SitDev_NetworkLoadCallback) and stops watching any ball (SitDev_ClearBallThatWasHit).
void SitDev_vInitModule(void) {
    Mem_set(gpSitDevData, 0, sizeof(SitDevData));
    gpSitDevData->pE8 = NULL;
    gpSitDevData->n13C = 0;
    Network_RegisterLoadNetworkCallback(5, SitDev_NetworkLoadCallback);
    SitDev_ClearBallThatWasHit();
}

// Round end (fn_8006CDC4): frees the commentary scripts' buffers (SitDevData pD0 when set, pCC,
// pGroupFlags) and forgets the loaded scripts (gpSitDevScripts).
void SitDev_vCloseModule(void) {
    if (gpSitDevData->pD0 != NULL) {
        StaticMem_Free(gpSitDevData->pD0);
    }
    StaticMem_Free(gpSitDevData->pCC);
    StaticMem_Free(gpSitDevData->pGroupFlags);
    gpSitDevScripts = NULL;
}

// Before a hole loads (fn_8006F4F0): no commentary zones yet (the count
// SitDev_NetworkLoadCallback adds to).
void SitDev_vInitBeforeHole(void) {
    gSitDevNumCommentaryZones = 0;
}

// Registers SitDev_LoadScripts as the loader of the hole stream's 'sscr' chunks (the commentary
// scripts); streammanagerhole.c calls it.
void SitDev_vRegisterStreamClients(void) {
    // port: SitDevFile.c defines the handler with the object's first word (the scripts) as its
    //       parameter; UStream calls it with the object. Same address on the GameCube.
    Stream_RegisterLoadChunkCallback('sscr', (void (*)(UStreamObject*))SitDev_LoadScripts);
}

// Unregisters the loader of the hole stream's 'sscr' chunks that SitDev_vRegisterStreamClients set
// up; streammanagerhole.c calls it.
void SitDev_vUnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('sscr');
}

// Queues situation event nEvent for nPlayer (0xFF means player 0; a negative player queues
// nothing); event.c's handlers call it, and a is not read. Some events are filtered: 30 (the
// flyover) is dropped in custom, random, dream and regional rounds; 27 (a tree hit) once the ball
// has collided; 26 (the putt's break line passed the cup) after the first since the last event 2 or
// 3. Event 3 (the shot set-up) also clears the prediction flag (gSitDevPredictionVoiced), the
// group flags (SitDev_ClearGroupFlags), the cup bevel flag and the watched ball, then goes on as
// event 2 (a turn begins: 26 may come again), then as event 25 (just before the swing): the emotion
// states are cleared, and the event is dropped when an event 8, 9, 10 or 11 is queued already. The
// queue's ten slots are not checked. The first event of a frame fills in the state values for its
// player (SitDev_SetupStateVector); for events 20 and 29 (the look-ahead ball worked out, its first
// bounce) values 18 and 19 then become the level distance, in inches, from the shot's start to the
// look-ahead ball.
void SitDev_QueueEvent(int nPlayer, int a, u8 nEvent) {
    int nWho;
    SitDevEvent* pEvent;
    int i;
    f32 vDiff[3];
    int nInches;

    if (nPlayer == 0xFF) {
        nWho = 0;
    } else if (nPlayer >= 0) {
        nWho = nPlayer;
    } else {
        return;
    }
    if (nEvent == 30) {
        if (gpGame->bCustomRound || gpGame->bRandom18 || gpGame->bDream18 || gpGame->nRegionalRound) {
            return;
        }
    }
    switch (nEvent) {
    case 27:
        if (gPlayers[nPlayer].ball.nCollideCount > 0) {
            return;
        }
        break;
    case 26:
        if (gSitDevBreaklinePassedCupQueued) {
            return;
        }
        gSitDevBreaklinePassedCupQueued = 1;
        break;
    case 3:
        gSitDevPredictionVoiced = 0;
        SitDev_ClearGroupFlags();
        SitDev_ClearCupBevelFlag();
        SitDev_ClearBallThatWasHit();
        SitDev_ClearEmotionStates();
        // fall through
    case 2:
        gSitDevBreaklinePassedCupQueued = 0;
        // fall through
    case 25:
        SitDev_ClearEmotionStates();
        for (i = 0; i < gpSitDevData->n13C; i++) {
            if (gpSitDevData->aEvents[i].nEvent == 8 || gpSitDevData->aEvents[i].nEvent == 9 ||
                gpSitDevData->aEvents[i].nEvent == 10 || gpSitDevData->aEvents[i].nEvent == 11) {
                return;
            }
        }
        break;
    }
    pEvent = &gpSitDevData->aEvents[gpSitDevData->n13C];
    pEvent->nPlayer = nWho;
    pEvent->nEvent = nEvent;
    gpSitDevData->n13C++;
    if (gpSitDevData->n13C == 1) {
        SitDev_SetupStateVector(nWho, nEvent);
        if (nEvent == 20 || nEvent == 29) {
            SitDev_Vec3Sub(gPlayers[nPlayer].ballBefore.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
            vDiff[1] = 0.0f;
            nInches = 36.0f * (f32)Math_Sqrt(Vec3_LengthSqClamped(vDiff));
            _SetStateVecAndCondition(gpSitDevData->aValue, 18, nInches, gpSitDevData->aSetBits);
            _SetStateVecAndCondition(gpSitDevData->aValue, 19, nInches, gpSitDevData->aSetBits);
        }
    }
}

// Each frame of play (gomainloop.c, after SitDev_ThrowBallHitDelayedEvent): runs the scripts for
// the events queued this frame, then empties the queue. For each situation (pSituations), in order:
// it needs a queued event of its kind (any, for kind 0) whose player is the first queued event's;
// then state value 0 is set again to the hole number (not for script file 22), and if its
// conditions hold (SitDev_ConditionsMatch) and its group (nGroup; 0 for none) has not fired, the
// group is marked and its actions run (SitDev_InvokeMultipleActions) for that player. A situation
// for event 29 that tests value 64 (what the look-ahead ball hit) sets the prediction flags
// (gSitDevPredictionVoiced, gSitDevPredictionPending) first. Afterwards, when an event 33 (the shot
// is over) was seen and the scripts set no emotion for its player (gSitDevEmotionSet), the player's
// shot outcome is recorded as 5 (fn_8006AAB4).
void SitDev_ProcessEventQueue(void) {
    int i;
    int j;
    SitDevSituation* pEntry;
    int nEmotionPlayer;
    SitDevEvent* pEvent;
    SitDevData* pData;
    u8 bFound;
    u8 nEvent;

    nEmotionPlayer = 5;
    if (gpSitDevData->n13C == 0) {
        return;
    }
    pEntry = gpSitDevScripts->pSituations;
    // fake match: a signed compare here, an unsigned one in SitDevFile.c's SitDev_SwapTables
    for (i = 0; i < (int)gpSitDevScripts->nSituations; i++, pEntry++) {
        pData = gpSitDevData;
        bFound = 0;
        for (j = 0; j < pData->n13C; j++) {
            pEvent = &pData->aEvents[j];
            if (pEvent->nEvent == 33) {
                nEmotionPlayer = pEvent->nPlayer;
            }
            if ((pEntry->nEvent == pEvent->nEvent || pEntry->nEvent == 0) &&
                pEvent->nPlayer == pData->aEvents[0].nPlayer) {
                bFound = 1;
                break;
            }
        }
        if (bFound) {
            if (pEntry->b2.s.nFileIndex != 22) {
                _SetStateVecAndCondition(gpSitDevData->aValue, 0, Game_CurHoleIndex() + 1, pData->aSetBits);
            }
            if (SitDev_ConditionsMatch(pEntry, gpSitDevData, pEvent->nPlayer) &&
                (pEntry->nGroup == 0 || !gpSitDevData->pGroupFlags[pEntry->nGroup])) {
                gpSitDevData->pGroupFlags[pEntry->nGroup] = 1;
                nEvent = gpSitDevData->aEvents[j].nEvent;
                if (nEvent == 29 && (pEntry->auTests[2] & 1)) {
                    gSitDevPredictionVoiced = 1;
                    gSitDevPredictionPending = 1;
                }
                SitDev_InvokeMultipleActions(pEntry, pEntry->b2.s.nFileIndex,
                                             gpSitDevData->aEvents[j].nPlayer, nEvent);
            }
        }
    }
    if (nEmotionPlayer != 5 && gSitDevEmotionSet[nEmotionPlayer] == 0) {
        fn_8006AAB4(nEmotionPlayer, 5);
    }
    gpSitDevData->n13C = 0;
}

// Stores state value nIndex (pValues[nIndex] = uValue) and sets its bit in pSetBits, the bits
// SitDev_SetupStateVector clears.
void _SetStateVecAndCondition(u16* pValues, int nIndex, u16 uValue, u32* pSetBits) {
    pValues[nIndex] = (int)uValue;  // fake match: the no-op widening only moves the store in the schedule
    pSetBits[nIndex / 32] |= 1 << (nIndex % 32);
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void SitDev_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void SitDev_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
