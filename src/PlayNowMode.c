// PlayNowMode.c (EA's name: the source paths Golf\GameMode\PlayNowMode.c in TW2003 and TW2005):
// game mode 5, the Play Now challenges. Its data is the 'PLY ' stream
// object, EA's DATA\PLAYNOW_GC.BIN: 83 challenges of 0x80 bytes (Challenge,
// include/game/modes/challenge.h), with their names and descriptions in 'PLYs' (PLAYNOW.STR) and
// their ball spots in the course's objects. The challenges come in 29 groups, one per entry of the
// Play Now menu ("Lucky 7", "2 Down Comeback"); a group is played as one or more challenges in a
// row, each a round set up in another game mode (stroke, match, skins, speed golf, a target game)
// with a target score, and it earns one of three medals, the best kept per group in the save
// profile, with money and trophy balls. Mode 5 runs the played mode with its own callbacks wrapped
// around that mode's. The ladder (LadderedMode.c) plays some of these challenges as events, and the
// real-time events (mode 24, GameModeDriverRTE.c) play their own list of 111 through this code.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/challenge.h"
#include "game/earnings.h"

s32 gPlayNowSavedOptionC = 4;           // gSession.options.nWeather saved while a challenge runs
Challenge* gChallengeList = gPlayNowChallenges; // the list being played (mode 24 swaps in its own)
s32 gNumChallenges = 83;                // its length

Challenge  gPlayNowChallenges[83];      // mode 5's own challenges ('PLY ')
ChallengeSpot gPlayNowBallSpots[83];    // where each places the ball (course objects of type 10)

// The played mode's own callbacks, kept by PlayNow_StartChallenge while mode 5's stand in for them.
void (*gPlayNowModeShutdown)(void);                             // pfnShutdown (called once)
void (*gPlayNowModeEndGame)(void);                              // pfnEndGame
void (*gPlayNowModeHoleStart)(void);                            // pfnLoadHole
u8  (*gPlayNowModeGameFinished)(u8 bCheck);                     // pfnGameFinished
u8 (*gPlayNowModeHoleFinished)(int nPlayer, u8 bCheck);         // pfnHoleFinished
void (*gPlayNowModeHoleOver)(int nPlayer);                      // pfnEndTurnEndHoleNotGame
u8 gPlayNowIntroPending;                // the group's intro message is still to be shown
char* gPlayNowText;                     // the 'PLYs' text block (group names and descriptions)
// The group's totals over the challenges played so far (PlayNow_GetMedal judges them).
s32 gPlayNowGroupStrokes;               // strokes
s32 gPlayNowGroupPar;                   // par of the holes played
s32 gPlayNowGroupHoles;                 // holes played
s32 gPlayNowGroupTime;                  // Player.n290 summed: speed golf's hole times
u8 gPlayNowRestarting;                  // a restart is pending (PlayNow_Restart .. PlayNow_HoleOver)
u8 gPlayNowCalendarFlag;                // set by the calendar screen (PlayNow_GetCalendarFlag)
u8 gPlayNowChallengeRunning;            // PlayNow_IsChallengeRunning
s32 gPlayNowSelectedChallenge;          // the challenge a restart goes back to
s32 gCurChallenge;                      // the current challenge, an index into gChallengeList
s32 gPlayNowSavedWind;                  // gSession.options.nWind saved while a challenge runs

void PlayNow_SelectGroup(int nGroup);
s32 PlayNow_GetNumGroups(void);
void PlayNow_UnregisterStreamClients(void);
void PlayNow_LoadPLYFromStream(UStreamObject* pObject);

int   PlayNow_GetChallengeTarget(int i);
void  Character_PreHoleInit(void);
void  PlayNow_ApplyChallengeSetup(void);
void  PlayNow_Shutdown(void);
void  PlayNow_HoleStart(void);
void  PlayNow_EndGame(void);
void  PlayNow_HoleOver(int nPlayer);
void  PlayNow_LoadPLYsFromStream(UStreamObject* pObject);
int   PlayNow_GetGroupTarget(int iUnused);
void  PlayNow_QueueMedalMessage(int nMedal);
u8    PlayNow_HasAllMedals(int nProfile);
u8    PlayNow_GameFinished(u8 bCheck);
int   PlayNow_CountGroupChallenges(int nGroup);
u8 PlayNow_IsSpeedGolf(void);
u8 PlayNow_HoleFinished(int nPlayer, u8 bCheck);

// Game mode 5 (the Play Now challenges) starts, from GM_SetModeType: its own shutdown, hole-start,
// end-game, hole-finished and hole-over callbacks go in, gpGame nC and n10 become 1, no restart is
// pending, and the challenge list goes back to the mode's own 83 (gPlayNowChallenges; mode 24 swaps
// in its own with PlayNow_SetChallengeList).
void PlayNow_Init(void) {
    gpGame->pfnInit = PlayNow_Init;
    gpGame->pfnShutdown = PlayNow_Shutdown;
    gpGame->pfnLoadHole = PlayNow_HoleStart;
    gpGame->pfnEndGame = PlayNow_EndGame;
    gpGame->pfnHoleFinished = PlayNow_HoleFinished;
    gpGame->pfnEndTurnEndHoleNotGame = PlayNow_HoleOver;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gPlayNowRestarting = 0;
    gChallengeList = gPlayNowChallenges;
    gNumChallenges = 83;
}

// Mode 5's shutdown (pfnShutdown): the played mode's own shutdown runs once (then it is forgotten),
// gpGame nC and n10 go back to 1, the options nWeather and wind that PlayNow_StartChallenge saved
// are put back, and no challenge is running any more (PlayNow_IsChallengeRunning).
void PlayNow_Shutdown(void) {
    if (gPlayNowModeShutdown) {
        gPlayNowModeShutdown();
        gPlayNowModeShutdown = NULL;
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nWeather = gPlayNowSavedOptionC;
    gSession.options.nWind = gPlayNowSavedWind;
    gPlayNowChallengeRunning = 0;
}

// Mode 5's part of a round's teardown (GM_DeInitModule, after the mode's shutdown); empty in this
// build.
void PlayNow_DeInit(void) {
}

// A course object of type 10, from the 'Cact' handler (Kernel_DownloadActors): the spot where challenge
// nChallenge (1-based; 84 and up are ignored) places the ball is copied into gPlayNowBallSpots, and
// the object is freed.
void PlayNow_LoadBallSpot(void* pObj) {
    // port: the course object is big-endian and read in place through ChallengeSpotRecord; a
    //       little-endian port converts v[] (three floats) here
    ChallengeSpotRecord* pRecord = *(ChallengeSpotRecord**)pObj;
    int i = pRecord->nChallenge - 1;
    if (i < 83) {
        gPlayNowBallSpots[i].fX = pRecord->v[0];
        gPlayNowBallSpots[i].fY = pRecord->v[1];
        gPlayNowBallSpots[i].fZ = pRecord->v[2];
    }
    StaticMem_Free(pObj);
}

// Selects challenge nChallenge (a 0-based index into gChallengeList) as the current one and as the
// one a restart goes back to (PlayNow_Restart). The menu (FE_MessageTable.c), the ladder
// (GameMode4_StartEvent) and the real-time events (GameModeDriverRTE_StartEvent) call it.
void PlayNow_SelectChallenge(s32 nChallenge) {
    gCurChallenge = nChallenge;
    gPlayNowSelectedChallenge = nChallenge;
}

// Selects the first challenge of group nGroup (challenge 0 when no challenge has that group) as
// current and as the restart point, as PlayNow_SelectChallenge does. A group is one entry of the
// Play Now menu, played as one or more challenges in a row; the menu's FE message passes its
// 1-based choice minus one.
void PlayNow_SelectGroup(int nGroup) {
    int i = PlayNow_GetGroupFirstChallenge(nGroup);
    gCurChallenge = i;
    gPlayNowSelectedChallenge = i;
}

// The number of challenge groups the menu lists: 29, as many as SaveProfile.aMedal has entries.
s32 PlayNow_GetNumGroups(void) {
    return 29;
}

void PlayNow_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('PLY ', PlayNow_LoadPLYFromStream);
    Stream_RegisterLoadChunkCallback('PLYs', PlayNow_LoadPLYsFromStream);
}

void PlayNow_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('PLY ');
    Stream_UnregisterLoadChunkCallback('PLYs');
}

// The 'PLY ' stream object (EA's DATA\PLAYNOW_GC.BIN): the 83 challenges, copied into
// gPlayNowChallenges.
void PlayNow_LoadPLYFromStream(UStreamObject* pObject) {
    // port: the 'PLY ' object is copied straight into the challenges (Challenge[83]); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/game/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gPlayNowChallenges), gPlayNowChallenges);
}

// The 'PLYs' stream object (EA's PLAYNOW.STR): the groups' names and descriptions, the text block
// Challenge.nGroupName and nGroupDesc are offsets into, copied into a new 16-byte aligned block at
// gPlayNowText; the stream's copy is freed. An empty object leaves gPlayNowText as it was.
void PlayNow_LoadPLYsFromStream(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gPlayNowText = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gPlayNowText, pData, nSize);
        StaticMem_Free(pObject);
    }
}

// The current challenge (gCurChallenge) starts. The options nWeather and wind are saved
// (gPlayNowSavedOptionC, gPlayNowSavedWind), the new-game data cleared, player 0's active profile
// gets bChanged set, and the challenge's game mode is set (GM_SetModeType fills that mode's
// callbacks); the challenge now counts as running. Then its course, hole set and hole (1-based; a
// one-hole challenge selects it alone; not set for the par-5/4/3 and type-7 hole sets), the holes
// before it deselected, its tee set for everyone, its pin set (nPins - 1; nPins 0 gives nPinSet -1
// and pin set 0), and player 0 plus up to three CPU opponents (one playing player 0's golfer in the
// same look gets the next of four looks). The holes before the challenge hole get player 0's scores
// by nTargetKind: 0 none; 1 a one-hole challenge carries nTargetBase as strokes, else pars nudged
// at random to total nTargetBase; 2 pars nudged at random to nTargetBase over par (a one-hole
// challenge carries it, counting one hole); 3/4/5 birdie/par/bogey on each; 7 with match scoring
// (gpGame->nScoringType 1) holes won at random until the margin is nTargetBase (negative: the
// opponent's), with stroke scoring everyone on par and player 0 nudged to nTargetBase over. The
// challenge hole starts at a score by nHoleKind: 0 none, 1 nHoleExtra, 2 par plus nHoleExtra, 3/4/5
// birdie/par/bogey. Options nWeather becomes 3 when bForceWeather is set, else 0; no mulligans; the
// challenge's wind. The first challenge of its group starts the group totals (gPlayNowGroupStrokes,
// Par, Holes, Time) and flags the group's intro (gPlayNowIntroPending); a later one adds to them.
// Last, the played mode's shutdown, end game, hole start, game finished, hole finished and
// hole-over callbacks are kept (gPlayNowMode*) and mode 5's go in their place.
void PlayNow_StartChallenge(void) {
    int h;
    int nSum;
    int nSum0;
    int nDiff;
    int i;
    int nStrokes;
    int nPar;
    int nHoles;
    u8 bFound;
    gPlayNowSavedOptionC = gSession.options.nWeather;
    gPlayNowSavedWind = gSession.options.nWind;
    GM_ClearDataForNewGame();
    if (gpSaveData[gPlayers[0].nIndex].bActive) {
        gpSaveData[gPlayers[0].nIndex].bChanged = 1;
    }
    GM_SetModeType(gChallengeList[gCurChallenge].nMode);
    gPlayNowChallengeRunning = 1;
    GM_SetCurrentCourse(gChallengeList[gCurChallenge].nCourse);
    GM_SelectHoleSet(gChallengeList[gCurChallenge].nType);
    if (gChallengeList[gCurChallenge].nType == 0) {
        GM_SelectSingleHole(gChallengeList[gCurChallenge].nHole - 1);
    }
    if (gChallengeList[gCurChallenge].nType != 4 && gChallengeList[gCurChallenge].nType != 5 &&
        gChallengeList[gCurChallenge].nType != 6 && gChallengeList[gCurChallenge].nType != 7) {
        GM_SetCurrentHole(gChallengeList[gCurChallenge].nHole - 1);
    }
    for (h = 0; h < gChallengeList[gCurChallenge].nHole - 1; h++) {
        gpGame->bHoleSelected[h] = 0;
    }
    gSession.nTeeSet[0] = gChallengeList[gCurChallenge].nTeeSet;
    if (gChallengeList[gCurChallenge].nPins) {
        gSession.nPinSet = gChallengeList[gCurChallenge].nPins - 1;
        for (h = 0; h < 18; h++) {
            gpGame->nPinSet[h] = gSession.nPinSet;
        }
    } else {
        gSession.nPinSet = -1;
        for (h = 0; h < 18; h++) {
            gpGame->nPinSet[h] = 0;
        }
    }
    gNumPlayersSetUp = 1;
    Session_SetNumPlayers(gChallengeList[gCurChallenge].nOpponents + 1);
    if (gChallengeList[gCurChallenge].nOpponents > 0) {
        Session_SetGolfer(gChallengeList[gCurChallenge].aOpponent[0], 1);
        gSession.nController[1] = CONTROLLER_CPU;
        gSession.nTeeSet[1] = gChallengeList[gCurChallenge].nTeeSet;
        gNumPlayersSetUp = 2;
        if (gSession.nGolfer[0] == gSession.nGolfer[1] &&
            gSession.aProfile[0].nShirt == gSession.aProfile[1].nShirt) {
            gSession.aProfile[1].nShirt++;
            if (gSession.aProfile[1].nShirt >= 4) {
                gSession.aProfile[1].nShirt = 0;
            }
        }
    }
    if (gChallengeList[gCurChallenge].nOpponents > 1) {
        Session_SetGolfer(gChallengeList[gCurChallenge].aOpponent[1], 2);
        gSession.nController[2] = CONTROLLER_CPU;
        gSession.nTeeSet[2] = gChallengeList[gCurChallenge].nTeeSet;
        gNumPlayersSetUp = 3;
        if (gSession.nGolfer[0] == gSession.nGolfer[2] &&
            gSession.aProfile[0].nShirt == gSession.aProfile[2].nShirt) {
            gSession.aProfile[2].nShirt++;
            if (gSession.aProfile[2].nShirt >= 4) {
                gSession.aProfile[2].nShirt = 0;
            }
        }
    }
    if (gChallengeList[gCurChallenge].nOpponents > 2) {
        Session_SetGolfer(gChallengeList[gCurChallenge].aOpponent[2], 3);
        gSession.nController[3] = CONTROLLER_CPU;
        gSession.nTeeSet[3] = gChallengeList[gCurChallenge].nTeeSet;
        gNumPlayersSetUp = 4;
        if (gSession.nGolfer[0] == gSession.nGolfer[3] &&
            gSession.aProfile[0].nShirt == gSession.aProfile[3].nShirt) {
            gSession.aProfile[3].nShirt++;
            if (gSession.aProfile[3].nShirt >= 4) {
                gSession.aProfile[3].nShirt = 0;
            }
        }
    }
    nStrokes = 0;
    nPar = 0;
    nHoles = 0;
    switch (gChallengeList[gCurChallenge].nTargetKind) {
    case 0:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = 0;
        }
        break;
    case 1:
        if (gChallengeList[gCurChallenge].nType == 0) {
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = 0;
            }
            nPar = 0;
            nHoles = 0;
            nStrokes = gChallengeList[gCurChallenge].nTargetBase;
        } else {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h);
                nSum0 += gPlayers[0].nStrokes[h];
            }
            nDiff = gChallengeList[gCurChallenge].nTargetBase - nSum0;
            while (nDiff != 0) {
                h = Misc_RandFunc(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[h] > GM_GetHoleIndexPar(h) - 1) {
                        gPlayers[0].nStrokes[h]--;
                    }
                } else if (gPlayers[0].nStrokes[h] < GM_GetHoleIndexPar(h) + 1) {
                    gPlayers[0].nStrokes[h]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = gChallengeList[gCurChallenge].nTargetBase - nSum;
            }
        }
        break;
    case 2:
        if (gChallengeList[gCurChallenge].nType == 0) {
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = 0;
            }
            nPar = 0;
            nHoles = 1;
            nStrokes = gChallengeList[gCurChallenge].nTargetBase;
        } else {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h);
                nSum0 += gPlayers[0].nStrokes[h];
            }
            nDiff = gChallengeList[gCurChallenge].nTargetBase;
            while (nDiff != 0) {
                h = Misc_RandFunc(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[h] > GM_GetHoleIndexPar(h) - 1) {
                        gPlayers[0].nStrokes[h]--;
                    }
                } else if (gPlayers[0].nStrokes[h] < GM_GetHoleIndexPar(h) + 1) {
                    gPlayers[0].nStrokes[h]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = gChallengeList[gCurChallenge].nTargetBase - (nSum - nSum0);
            }
        }
        break;
    case 3:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h) - 1;
        }
        break;
    case 4:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h);
        }
        break;
    case 5:
        for (h = 0; h < Game_CurHoleIndex(); h++) {
            gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h) + 1;
        }
        break;
    case 7:
        if (gpGame->nScoringType == 1) {
            nDiff = gChallengeList[gCurChallenge].nTargetBase;
            while (nDiff != 0) {
                h = Misc_RandFunc(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nModePoints[h] == 0 && gPlayers[1].nModePoints[h] == 0) {
                        nDiff++;
                        gPlayers[1].nModePoints[h] = 1;
                        gPlayers[1].nHolesWon++;
                    }
                } else if (gPlayers[0].nModePoints[h] == 0 && gPlayers[1].nModePoints[h] == 0) {
                    nDiff--;
                    gPlayers[0].nModePoints[h] = 1;
                    gPlayers[0].nHolesWon++;
                }
            }
        }
        if (gpGame->nScoringType == 0) {
            nSum0 = 0;
            for (h = 0; h < Game_CurHoleIndex(); h++) {
                gPlayers[0].nStrokes[h] = GM_GetHoleIndexPar(h);
                nSum0 += gPlayers[0].nStrokes[h];
                for (i = 1; i < gNumPlayersSetUp; i++) {
                    gPlayers[(u32)i].nStrokes[h] = GM_GetHoleIndexPar(h);
                }
            }
            nDiff = gChallengeList[gCurChallenge].nTargetBase;
            while (nDiff != 0) {
                i = Misc_RandFunc(0) % (Game_CurHoleIndex() + 1);
                if (nDiff < 0) {
                    if (gPlayers[0].nStrokes[i] > GM_GetHoleIndexPar(i) - 1) {
                        gPlayers[0].nStrokes[i]--;
                    }
                } else if (gPlayers[0].nStrokes[i] < GM_GetHoleIndexPar(i) + 1) {
                    gPlayers[0].nStrokes[i]++;
                }
                nSum = 0;
                for (h = 0; h < Game_CurHoleIndex(); h++) {
                    nSum += gPlayers[0].nStrokes[h];
                }
                nDiff = gChallengeList[gCurChallenge].nTargetBase - (nSum - nSum0);
            }
        }
        break;
    }
    switch (gChallengeList[gCurChallenge].nHoleKind) {
    case 0:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = 0;
        break;
    case 1:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = gChallengeList[gCurChallenge].nHoleExtra;
        break;
    case 2:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = gChallengeList[gCurChallenge].nHoleExtra
                + GM_GetCurrentHolePar();
        break;
    case 3:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = GM_GetCurrentHolePar() - 1;
        break;
    case 4:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = GM_GetCurrentHolePar();
        break;
    case 5:
        gPlayers[0].nStrokes[Game_CurHoleIndex()] = GM_GetCurrentHolePar() + 1;
        break;
    }
    if (gChallengeList[gCurChallenge].bForceWeather) {
        gSession.options.nWeather = 3;
    } else {
        gSession.options.nWeather = 0;
    }
    gpGame->nMulligans = 0;
    gSession.options.nWind = gChallengeList[gCurChallenge].nWind;
    bFound = 0;
    for (i = gCurChallenge - 1; i >= 0; i--) {
        if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup) {
            bFound = 1;
        }
    }
    if (!bFound) {
        gPlayNowGroupStrokes = nStrokes;
        gPlayNowGroupPar = nPar;
        gPlayNowGroupHoles = nHoles;
        gPlayNowGroupTime = 0;
        gPlayNowIntroPending = 1;
    } else {
        gPlayNowGroupStrokes += nStrokes;
        gPlayNowGroupPar += nPar;
        gPlayNowGroupHoles += nHoles;
    }
    gPlayNowModeShutdown = gpGame->pfnShutdown;
    gPlayNowModeEndGame = gpGame->pfnEndGame;
    gPlayNowModeHoleStart = gpGame->pfnLoadHole;
    gPlayNowModeGameFinished = gpGame->pfnGameFinished;
    gPlayNowModeHoleFinished = gpGame->pfnHoleFinished;
    gPlayNowModeHoleOver = gpGame->pfnEndTurnEndHoleNotGame;
    gpGame->pfnShutdown = PlayNow_Shutdown;
    gpGame->pfnEndGame = PlayNow_EndGame;
    gpGame->pfnLoadHole = PlayNow_HoleStart;
    gpGame->pfnGameFinished = PlayNow_GameFinished;
    gpGame->pfnHoleFinished = PlayNow_HoleFinished;
    gpGame->pfnEndTurnEndHoleNotGame = PlayNow_HoleOver;
}

// Mode 5's hole start (pfnLoadHole, from GM_InitForHole): the between-holes scorecard is turned on
// (b275), the played mode's own hole start runs, then the challenge's ball spot, bag and weather go
// in (PlayNow_ApplyChallengeSetup).
void PlayNow_HoleStart(void) {
    gpGame->b275 = 1;
    gPlayNowModeHoleStart();
    PlayNow_ApplyChallengeSetup();
}

// Mode 5's game-over test (pfnGameFinished; bCheck 1 only asks). Never over while a restart is
// pending. When the played mode says its game is over, the round's selected holes are added to the
// group totals (strokes, par, hole count, time n290), the hole set goes back to 0, and if a later
// challenge in the list has the same group, the game is not over: with bCheck that is all,
// otherwise that challenge becomes current and starts (in a real-time event as mode 24 through
// GameModeDriverRTE_StartNextChallenge, else as mode 5 through PlayNow_StartChallenge) and b134 is
// set. 1 only after the group's last challenge. The totals are added even when bCheck only asks.
u8 PlayNow_GameFinished(u8 bCheck) {
    int h;
    int nStrokes;
    int i;
    if (gPlayNowRestarting) {
        return 0;
    }
    if (gPlayNowModeGameFinished(bCheck)) {
        nStrokes = 0;
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nStrokes += gPlayers[0].nStrokes[h];
                gPlayNowGroupPar += GM_GetHoleIndexPar(h);
                gPlayNowGroupHoles++;
                gPlayNowGroupTime += gPlayers[0].n290[h];
            }
        }
        gPlayNowGroupStrokes += nStrokes;
        GM_SelectHoleSet(0);
        for (i = gCurChallenge + 1; i < gNumChallenges; i++) {
            if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup) {
                if (bCheck) {
                    return 0;
                }
                gCurChallenge = i;
                if (GM_Currently_RealtimeMode()) {
                    GM_SetModeType(24);
                    GameModeDriverRTE_StartNextChallenge();
                } else {
                    GM_SetModeType(5);
                    PlayNow_StartChallenge();
                }
                gpGame->b134 = 1;
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// The current challenge's hole setup (PlayNow_HoleStart): with bPlaceBall, player 0's ball starts
// at the challenge's spot (gPlayNowBallSpots), dropped to the ground; a nonzero nClubBits replaces
// player 0's bag (uBagMask): bag bit 25 always, challenge bit 0 gives bag bit 0, bits 1..14 bag
// bits 6..19, bit 15 bag bit 21 and bit 16 bag bit 23; with bForceWeather the weather amount
// fWeatherAmount is forced (PlayNow_ForceWeather).
void PlayNow_ApplyChallengeSetup(void) {
    f32 v[4];
    if (gChallengeList[gCurChallenge].bPlaceBall) {
        v[0] = gPlayNowBallSpots[gCurChallenge].fX;
        v[1] = gPlayNowBallSpots[gCurChallenge].fY;
        v[2] = gPlayNowBallSpots[gCurChallenge].fZ;
        v[3] = 1.0f;
        Physics_InitBall(&gPlayers[0].ball, v, 0);
        Physics_DropBall(&gPlayers[0].ball, v);
        LLMath_CopyVec(v, gPlayers[0].vBall);
    }
    if (gChallengeList[gCurChallenge].nClubBits) {
        gPlayers[0].golfer.uBagMask = 0x2000000;
        if (gChallengeList[gCurChallenge].nClubBits & 1) {
            gPlayers[0].golfer.uBagMask |= 0x1;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 2) {
            gPlayers[0].golfer.uBagMask |= 0x40;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 4) {
            gPlayers[0].golfer.uBagMask |= 0x80;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 8) {
            gPlayers[0].golfer.uBagMask |= 0x100;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x10) {
            gPlayers[0].golfer.uBagMask |= 0x200;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x20) {
            gPlayers[0].golfer.uBagMask |= 0x400;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x40) {
            gPlayers[0].golfer.uBagMask |= 0x800;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x80) {
            gPlayers[0].golfer.uBagMask |= 0x1000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x100) {
            gPlayers[0].golfer.uBagMask |= 0x2000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x200) {
            gPlayers[0].golfer.uBagMask |= 0x4000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x400) {
            gPlayers[0].golfer.uBagMask |= 0x8000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x800) {
            gPlayers[0].golfer.uBagMask |= 0x10000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x1000) {
            gPlayers[0].golfer.uBagMask |= 0x20000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x2000) {
            gPlayers[0].golfer.uBagMask |= 0x40000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x4000) {
            gPlayers[0].golfer.uBagMask |= 0x80000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x8000) {
            gPlayers[0].golfer.uBagMask |= 0x200000;
        }
        if (gChallengeList[gCurChallenge].nClubBits & 0x10000) {
            gPlayers[0].golfer.uBagMask |= 0x800000;
        }
    }
    if (gChallengeList[gCurChallenge].bForceWeather) {
        PlayNow_ForceWeather(gChallengeList[gCurChallenge].fWeatherAmount);
    }
}

// Per medal (0..2): three messages to pick from.
s32 gPlayNowMedalMessages[3][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
};

// Queues a message for medal nMedal (0 best .. 2): one of its three ids in gPlayNowMedalMessages,
// picked at random, as GUI message kind 7.
void PlayNow_QueueMedalMessage(int nMedal) {
    s32 nMsg = gPlayNowMedalMessages[nMedal][Misc_RandFunc(0) % 3];
    GUI_QueueMessage(7, nMsg, 0, 0);
}

// Mode 5's end of game (pfnEndGame): the played mode's own end game runs first. Then, outside a
// real-time event and a ladder event, when a medal is earned (PlayNow_GetMedal, not 3): it is saved
// with today's date if it beats the profile's best for the group, and the medal's message is
// queued; for an active profile the medal's reward goes through
// GM_Earnings_ComputeTOURCardModifiers, its payout message (0x6F, 0x70, 0x71 by medal) is queued
// when it is not 0, and it is paid. Then two trophy balls, each paid (message, money, and counted
// in money.n8) only when newly given: 0x1C when the calendar flag is set and award 0x1C is earned
// (a best medal, Earnings.c), and 0xC once the profile has a medal in every group
// (PlayNow_HasAllMedals).
void PlayNow_EndGame(void) {
    s32 aOut[18];               // the payout's breakdown, handed on as a CourseMoneyTracking
    int nReward;
    int nMedal;
    int nProfile;
    int nMoney;
    gPlayNowModeEndGame();
    if (!GM_Currently_RealtimeMode()) {
        nMedal = PlayNow_GetMedal();
        if (!GameMode4_IsEventRunning() && nMedal != 3) {
            if (nMedal < gpSaveData[gPlayers[0].nIndex].aMedal[PlayNow_GetCurrentGroup()]) {
                gpSaveData[gPlayers[0].nIndex].aMedal[PlayNow_GetCurrentGroup()] = nMedal;
                gpSaveData[gPlayers[0].nIndex].aMedalDate[PlayNow_GetCurrentGroup()] = CalDate_GetToday();
            }
            switch (nMedal) {
            case 0:
                nReward = gChallengeList[gCurChallenge].aMedal[0].nReward;
                break;
            case 1:
                nReward = gChallengeList[gCurChallenge].aMedal[1].nReward;
                break;
            case 2:
                nReward = gChallengeList[gCurChallenge].aMedal[2].nReward;
                break;
            }
            PlayNow_QueueMedalMessage(nMedal);
            nProfile = gPlayers[0].nIndex;
            if (gpSaveData[nProfile].bActive) {
                nMoney = GM_Earnings_ComputeTOURCardModifiers(nReward, 0, (CourseMoneyTracking*)aOut);
                if (nMoney) {
                    switch (nMedal) {
                    case 0:
                        GUI_QueueMessage(0, 0x6F, nMoney, nProfile);
                        break;
                    case 1:
                        GUI_QueueMessage(0, 0x70, nMoney, nProfile);
                        break;
                    case 2:
                        GUI_QueueMessage(0, 0x71, nMoney, nProfile);
                        break;
                    }
                }
                GM_Earnings_AwardMoney(0, nMoney, (CourseMoneyTracking*)aOut);
                if (PlayNow_GetCalendarFlag() && Earnings_IsTourAwardEarned(0, 0x1C)
                    && GM_Earnings_AwardTrophyBall(0, 0x1C)) {
                    GUI_QueueMessage(6, 0x1C, gEarningsTable.nA24, nProfile);
                    GM_Earnings_AwardMoney(0, gEarningsTable.nA24, 0);
                    gPlayers[0].money.n8 += gEarningsTable.nA24;
                }
                if (PlayNow_HasAllMedals(nProfile) && GM_Earnings_AwardTrophyBall(0, 0xC)) {
                    GUI_QueueMessage(2, 0xC, gEarningsTable.n9E4, nProfile);
                    GM_Earnings_AwardMoney(0, gEarningsTable.n9E4, 0);
                    gPlayers[0].money.n8 += gEarningsTable.n9E4;
                }
            }
        }
    }
}

// Whether profile nProfile has a medal in every one of the 29 challenge groups (no aMedal is 3) and
// a TOUR card (level 1 or more).
u8 PlayNow_HasAllMedals(int nProfile) {
    SaveProfile* p = &gpSaveData[nProfile];
    int i;
    if (p->nTourCardLevel < 1) {
        return 0;
    }
    for (i = 0; i < 29; i++) {
        if (p->aMedal[i] == 3) {
            return 0;
        }
    }
    return 1;
}

// Plays challenges from another list of nCount: mode 24 passes its 111 (gRTEs.aChallenge);
// PlayNow_Init puts the mode's own 83 back.
void PlayNow_SetChallengeList(Challenge* pList, s32 nCount) {
    gChallengeList = pList;
    gNumChallenges = nCount;
}

// Whether one of the mode's challenges is being played (set when it starts, cleared when it ends).
u8 PlayNow_IsChallengeRunning(void) {
    return gPlayNowChallengeRunning;
}

// The medal the current challenge has earned: the first of 0 (best), 1, 2 whose rule
// (aMedal[m].nRule; 0 never passes) holds against its mark, else 3 (none). With nScoring 0 the
// group's totals so far are judged: 1 strokes plus nTargetBase at most the mark; 2 strokes over par
// at most the mark; 3/4/5 birdie, par or bogey golf over the holes played; 6 the total time (n290)
// under the mark; 7 in match scoring (gpGame->nScoringType 1) a margin of holes won of at least the
// mark (with the calendar flag a playoff gives 1 if player 0 has won more holes, else 2; without it
// a playoff counts only for mark 0 and a lead), in stroke scoring player 1's strokes minus player
// 0's at most the mark; 8 more skins than every opponent and at most the mark in playoff holes
// (nPlayoffHoles); 9 in a skill zone game at least the mark in points (nSkillZonePoints). With
// nScoring 1 this hole alone: 1 strokes at most the mark, 2 over par at most the mark, 3/4/5
// birdie, par or bogey or better, 6 as above, 7 a playoff player 0 leads gives 2, else the margin,
// 9 as above.
int PlayNow_GetMedal(void) {
    int m;
    int nRule;
    int nMark;
    int nStrokes;
    int nTime;
    int nSum;
    int nPar;
    int nHoles;
    u8 bBest;
    int i;
    int nPlayoff;
    for (m = 0; m < 3; m++) {
        switch (m) {
        case 0:
            nRule = gChallengeList[gCurChallenge].aMedal[0].nRule;
            nMark = gChallengeList[gCurChallenge].aMedal[0].nMark;
            break;
        case 1:
            nRule = gChallengeList[gCurChallenge].aMedal[1].nRule;
            nMark = gChallengeList[gCurChallenge].aMedal[1].nMark;
            break;
        case 2:
            nRule = gChallengeList[gCurChallenge].aMedal[2].nRule;
            nMark = gChallengeList[gCurChallenge].aMedal[2].nMark;
            break;
        }
        switch (gChallengeList[gCurChallenge].nScoring) {
        case 0:
            nStrokes = gPlayNowGroupStrokes;
            nPar = gPlayNowGroupPar;
            nHoles = gPlayNowGroupHoles;
            nTime = gPlayNowGroupTime;
            switch (nRule) {
            case 1:
                if (nStrokes + gChallengeList[gCurChallenge].nTargetBase <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - nHoles) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + nHoles) {
                    return m;
                }
                break;
            case 6:
                if (nTime < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->nScoringType == 1) {
                    if (PlayNow_GetCalendarFlag()) {
                        if (gpGame->bInPlayoff) {
                            nPlayoff = 2;
                            if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                                nPlayoff = 1;
                            }
                            return nPlayoff;
                        }
                        if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                            return m;
                        }
                    } else if (gpGame->bInPlayoff) {
                        if (nMark == 0 && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                            return m;
                        }
                    } else if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                        return m;
                    }
                }
                if (gpGame->nScoringType == 0) {
                    if (gSession.nNumPlayers > 1) {
                        nSum = 0;
                        for (i = 0; i < 18; i++) {
                            if (gpGame->bHoleSelected[i]) {
                                nSum += gPlayers[1].nStrokes[i];
                            }
                        }
                    }
                    // EA bug: with one player nSum is never set, so this compares whatever the
                    // register holds.
                    if (nSum - nStrokes <= nMark) {
                        return m;
                    }
                }
                break;
            case 8:
                bBest = 1;
                for (i = 1; i < gNumPlayersSetUp; i++) {
                    if (gPlayers[0].nSkinsTotal <= PLAYER(i)->nSkinsTotal) {
                        bBest = 0;
                    }
                }
                if (bBest && gpGame->nPlayoffHoles <= nMark) {
                    return m;
                }
                break;
            case 9:
                if (GM_Currently_SkillZoneMode() && gPlayers[0].nSkillZonePoints >= nMark) {
                    return m;
                }
                break;
            }
            break;
        case 1:
            nStrokes = gPlayers[0].nStrokes[Game_CurHoleIndex()];
            nPar = GM_GetCurrentHolePar();
            switch (nRule) {
            case 1:
                if (nStrokes <= nMark) {
                    return m;
                }
                break;
            case 2:
                if (nStrokes - nPar <= nMark) {
                    return m;
                }
                break;
            case 3:
                if (nStrokes <= nPar - 1) {
                    return m;
                }
                break;
            case 4:
                if (nStrokes <= nPar) {
                    return m;
                }
                break;
            case 5:
                if (nStrokes <= nPar + 1) {
                    return m;
                }
                break;
            case 6:
                // EA bug: nTime is only set when the round's totals are scored (nScoring 0), so
                // this compares whatever the register holds.
                if (nTime < nMark) {
                    return m;
                }
                break;
            case 7:
                if (gpGame->bInPlayoff && gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
                    return 2;
                }
                if (gPlayers[0].nHolesWon - gPlayers[1].nHolesWon >= nMark) {
                    return m;
                }
                break;
            case 9:
                if (GM_Currently_SkillZoneMode() && gPlayers[0].nSkillZonePoints >= nMark) {
                    return m;
                }
                break;
            }
            break;
        }
    }
    return 3;
}

// Whether the game mode is 8, speed golf (a UI command asks).
u8 PlayNow_IsSpeedGolf(void) {
    return Game_GetMode() == 8;
}

// The mark for medal k as the UI shows it, counted from the lowest (k 0 is aMedal[2], 2 the best,
// aMedal[0]): the group's last challenge's mark minus the group's target (PlayNow_GetGroupTarget;
// taken as 0 for target kind 1, and for kind 7 in stroke play, mode 0). -1 when the current
// challenge has no such medal (its rule 0); 0 for any other k.
s32 PlayNow_GetMedalMark(int k) {
    int i;
    int nLast = gCurChallenge;
    int nTarget;
    for (i = 0; i < gNumChallenges; i++) {
        if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup && i > nLast) {
            nLast = i;
        }
    }
    nTarget = PlayNow_GetGroupTarget(gCurChallenge);
    if ((gChallengeList[gCurChallenge].nTargetKind == 7 && Game_GetMode() == 0) ||
        gChallengeList[gCurChallenge].nTargetKind == 1) {
        nTarget = 0;
    }
    switch (k) {
    case 0:
        if (gChallengeList[gCurChallenge].aMedal[2].nRule == 0) {
            return -1;
        } else {
            return gChallengeList[nLast].aMedal[2].nMark - nTarget;
        }
    case 1:
        if (gChallengeList[gCurChallenge].aMedal[1].nRule == 0) {
            return -1;
        } else {
            return gChallengeList[nLast].aMedal[1].nMark - nTarget;
        }
    case 2:
        if (gChallengeList[gCurChallenge].aMedal[0].nRule == 0) {
            return -1;
        } else {
            return gChallengeList[nLast].aMedal[0].nMark - nTarget;
        }
    default:
        return 0;
    }
}

// The pause menu opens during a challenge (GUI_OpenPauseMenu): in speed golf (mode 8), game message
// 18 for player 0 (PlayNow_SendMessage18).
void PlayNow_OnPause(void) {
    if (Game_GetMode() == 8) {
        PlayNow_SendMessage18(0);
    }
}

// The number shown against the challenge's target. In match play (mode 1) the margin of holes won
// (0 in a playoff); speed golf (8) the total time (n290) of the selected holes; skins (2) the
// playoff holes (nPlayoffHoles); a skill zone game the points (nSkillZonePoints). Otherwise the
// group's strokes so far (not for target kind 1) plus this round's (in stroke play, mode 0, with
// target kind 7 the holes before the current one, and the current one once the scorecard is up;
// else every selected hole, or all 18 for kind 1), minus the group's targets up to this challenge
// (PlayNow_GetChallengeTarget; 0 for kind 1).
int PlayNow_GetScoreToTarget(void) {
    int nTarget;
    int nScore;
    int i;
    int k;
    int h;
    if (Game_GetMode() == 1) {
        if (gpGame->bInPlayoff) {
            return 0;
        }
        return gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
    }
    if (Game_GetMode() == 8) {
        nScore = 0;
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nScore += gPlayers[0].n290[h];
            }
        }
        return nScore;
    }
    i = Game_GetMode();     // i is the mode here, a challenge index below
    if (i == 2) {
        return gpGame->nPlayoffHoles;
    }
    if (GM_Currently_SkillZoneMode()) {
        return gPlayers[0].nSkillZonePoints;
    }
    i = 0;
    nTarget = 0;
    for (; i <= gCurChallenge; i++) {
        if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup) {
            nTarget += PlayNow_GetChallengeTarget(i);
        }
    }
    k = 0;
    nScore = gChallengeList[gCurChallenge].nTargetKind != 1 ? gPlayNowGroupStrokes : 0;
    if (Game_GetMode() == 0 && gChallengeList[gCurChallenge].nTargetKind == 7) {
        if (GUI_ScoreCardUp()) {
            k = 1;
        }
        for (h = 0; h < k + Game_CurHoleIndex(); h++) {
            if (gpGame->bHoleSelected[h]) {
                nScore += gPlayers[0].nStrokes[h];
            }
        }
    } else {
        for (h = 0; h < 18; h++) {
            if (gpGame->bHoleSelected[h] || gChallengeList[gCurChallenge].nTargetKind == 1) {
                nScore += gPlayers[0].nStrokes[h];
            }
        }
    }
    if (gChallengeList[gCurChallenge].nTargetKind == 1) {
        nTarget = 0;
    }
    return nScore - nTarget;
}

// The total of PlayNow_GetChallengeTarget over every challenge in the current challenge's group.
// The argument is not read (PlayNow_GetMedalMark passes the current index).
int PlayNow_GetGroupTarget(int iUnused) {
    int n = 0;
    int i;
    for (i = 0; i < gNumChallenges; i++) {
        if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup) {
            n += PlayNow_GetChallengeTarget(i);
        }
    }
    return n;
}

// Challenge i's target score, in stroke play (game mode 0) only, else 0. For the holes before the
// challenge hole, by nTargetKind: 1 nTargetBase; 2 their par plus nTargetBase; 3/4/5 birdie, par or
// bogey on each; 7 player 1's strokes on the selected holes played so far (the current one too once
// the scorecard is up). The challenge hole adds, by nHoleKind: 1 nHoleExtra, 2 its par plus
// nHoleExtra, 3/4/5 birdie, par or bogey.
int PlayNow_GetChallengeTarget(int i) {
    int nTarget = 0;
    int nHole = gChallengeList[i].nHole - 1;
    int k;               // a loop counter, and in case 7 the current hole once it is over
    int h;
    h = Game_GetMode();     // h is the mode here, a hole number below
    if (h == 0) {
        switch (gChallengeList[i].nTargetKind) {
        case 1:
            nTarget = gChallengeList[i].nTargetBase;
            break;
        case 2:
            for (k = 0; k < nHole; k++) {
                nTarget += GM_GetHolePar(gChallengeList[i].nCourse, k);
            }
            nTarget += gChallengeList[i].nTargetBase;
            break;
        case 3:
            for (h = 0; h < nHole; h++) {
                nTarget += GM_GetHolePar(gChallengeList[i].nCourse, h) - 1;
            }
            break;
        case 4:
            for (h = 0; h < nHole; h++) {
                nTarget += GM_GetHolePar(gChallengeList[i].nCourse, h);
            }
            break;
        case 5:
            for (h = 0; h < nHole; h++) {
                nTarget += GM_GetHolePar(gChallengeList[i].nCourse, h) + 1;
            }
            break;
        case 7:
            k = 0;
            if (GUI_ScoreCardUp()) {
                k = 1;
            }
            for (h = 0; h < k + Game_CurHoleIndex(); h++) {
                if (gpGame->bHoleSelected[h]) {
                    nTarget += gPlayers[1].nStrokes[h];
                }
            }
            break;
        }
        switch (gChallengeList[i].nHoleKind) {
        case 1:
            nTarget += gChallengeList[i].nHoleExtra;
            break;
        case 2:
            nTarget += GM_GetHolePar(gChallengeList[i].nCourse, nHole) + gChallengeList[i].nHoleExtra;
            break;
        case 3:
            nTarget += GM_GetHolePar(gChallengeList[i].nCourse, nHole) - 1;
            break;
        case 4:
            nTarget += GM_GetHolePar(gChallengeList[i].nCourse, nHole);
            break;
        case 5:
            nTarget += GM_GetHolePar(gChallengeList[i].nCourse, nHole) + 1;
            break;
        }
    }
    return nTarget;
}

// Group nGroup's name: the text at its first challenge's nGroupName in the 'PLYs' block. The first
// challenge is looked up in the list being played (PlayNow_GetGroupFirstChallenge), but its
// nGroupName is read from the mode's own 83 (gPlayNowChallenges); the menu and the in-round screens
// show it (a real-time event shows GameModeDriverRTE_GetName instead).
char* PlayNow_GetGroupName(int nGroup) {
    int i = PlayNow_GetGroupFirstChallenge(nGroup);
    // EA bug: PlayNow_GetGroupFirstChallenge returns 0, never -1, for a group it does not find, so
    // this test never passes and an unknown group gets challenge 0's line.
    if (i == -1) {
        return 0;
    }
    return gPlayNowText + gPlayNowChallenges[i].nGroupName;
}

// Group nGroup's description: the text at its first challenge's nGroupDesc in the 'PLYs' block, as
// PlayNow_GetGroupName reads the name.
char* PlayNow_GetGroupDescription(int nGroup) {
    int i = PlayNow_GetGroupFirstChallenge(nGroup);
    // EA bug: never -1, as above.
    if (i == -1) {
        return 0;
    }
    return gPlayNowText + gPlayNowChallenges[i].nGroupDesc;
}

// The holes left in the challenge group: the selected holes after the current one, plus, when the
// group has more than one challenge, the holes of each challenge after the current one (from the
// current one itself once PlayNow_HoleFinished says the hole is over) by nType: one, 18, a nine, or
// the par 5s, 4s or 3s of its course (type 7 none). 0 while the end-of-round screen is up
// (GUI_IsEndGameUiShowing).
int PlayNow_GetHolesLeft(void) {
    int h;
    int i;
    int n = 0;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            n++;
        }
    }
    i = !PlayNow_HoleFinished(0, 1);
    if (PlayNow_CountGroupChallenges(gChallengeList[gCurChallenge].nGroup) > 1) {
        for (i = gCurChallenge + i; i < gNumChallenges; i++) {
            if (gChallengeList[i].nGroup == gChallengeList[gCurChallenge].nGroup) {
                switch (gChallengeList[i].nType) {
                case 0:
                    n += 1;
                    break;
                case 1:
                    n += 18;
                    break;
                case 2:
                    n += 9;
                    break;
                case 3:
                    n += 9;
                    break;
                case 4:
                    for (h = 0; h < 18; h++) {
                        if (GM_GetHolePar(gChallengeList[i].nCourse, h) == 5) {
                            n++;
                        }
                    }
                    break;
                case 5:
                    for (h = 0; h < 18; h++) {
                        if (GM_GetHolePar(gChallengeList[i].nCourse, h) == 4) {
                            n++;
                        }
                    }
                    break;
                case 6:
                    for (h = 0; h < 18; h++) {
                        if (GM_GetHolePar(gChallengeList[i].nCourse, h) == 3) {
                            n++;
                        }
                    }
                    break;
                case 7:
                    break;
                }
            }
        }
    }
    if (GUI_IsEndGameUiShowing()) {
        return 0;
    }
    // fake match: the binary calls GUI_ScoreCardUp and branches on its result, but both paths return
    // n (a bare call without the test loses the compare: 99.2%).
    if (GUI_ScoreCardUp()) {
        return n;
    }
    return n;
}

// How many challenges of the list being played belong to group nGroup.
int PlayNow_CountGroupChallenges(int nGroup) {
    int n = 0;
    int i;
    for (i = 0; i < gNumChallenges; i++) {
        if (nGroup == gChallengeList[i].nGroup) {
            n++;
        }
    }
    return n;
}

// Whether the challenge group's intro is still to be shown: set when the group's first challenge
// starts (PlayNow_StartChallenge); at the first swing (STATEFUNC_SwingInit) a human player gets the
// group's message and PlayNow_ClearIntroPending clears it.
u8 PlayNow_IsIntroPending(void) {
    return gPlayNowIntroPending;
}

void PlayNow_ClearIntroPending(void) {
    gPlayNowIntroPending = 0;
}

// The challenge restarts (the restart command, after GM_RestartHole): a restart is flagged
// (gPlayNowRestarting: the hole counts as finished and the game as not, until PlayNow_HoleOver),
// b134 is set, the current challenge goes back to the selected one (the group's first), the
// golfer's turn is ended, the mode's hole-restart hook runs, UI message flag 2 is set, the new-game
// data is cleared and the challenge starts again (in a real-time event through
// GameModeDriverRTE_StartEvent, else PlayNow_StartChallenge); then Character_PreHoleInit.
void PlayNow_Restart(void) {
    gPlayNowRestarting = 1;
    gpGame->b134 = 1;
    gCurChallenge = gPlayNowSelectedChallenge;
    GM_EndOfGolferTurn(0);
    gpGame->pfnRestartHole();
    GameMsg_SetPending(2);
    GM_ClearDataForNewGame();
    if (GM_Currently_RealtimeMode()) {
        GameModeDriverRTE_StartEvent();
    } else {
        PlayNow_StartChallenge();
    }
    Character_PreHoleInit();
}

// Mode 5's hole-finished test (pfnHoleFinished; bCheck 1 only asks): always over while a restart is
// pending (PlayNow_Restart), otherwise the played mode's own test.
u8 PlayNow_HoleFinished(int nPlayer, u8 bCheck) {
    if (gPlayNowRestarting) {
        return 1;
    }
    return gPlayNowModeHoleFinished(nPlayer, bCheck);
}

// Mode 5's hook for a hole that is over while the game goes on (pfnEndTurnEndHoleNotGame): after a
// restart the end-of-hole screen is flagged pending (GUI_SetEndOfHolePending) and the restart flag
// and b275 (the between-holes scorecard) are cleared; otherwise the played mode's own hook runs.
void PlayNow_HoleOver(int nPlayer) {
    if (gPlayNowRestarting) {
        GUI_SetEndOfHolePending();
        gPlayNowRestarting = 0;
        gpGame->b275 = 0;
        return;
    }
    gPlayNowModeHoleOver(nPlayer);
}

// Challenge i's three medal rewards, the lowest medal's (aMedal[2]) first and the best's
// (aMedal[0]) last.
void PlayNow_GetRewards(int i, s32* pThird, s32* pSecond, s32* pBest) {
    *pThird = gChallengeList[i].aMedal[2].nReward;
    *pSecond = gChallengeList[i].aMedal[1].nReward;
    *pBest = gChallengeList[i].aMedal[0].nReward;
}

s32 PlayNow_GetNumOpponents(int i) {
    return gChallengeList[i].nOpponents;
}

// The golfer of challenge i's CPU opponent k (0, 1; any other k gives the third).
s32 PlayNow_GetOpponent(int i, int k) {
    if (k == 0) {
        return gChallengeList[i].aOpponent[0];
    }
    if (k == 1) {
        return gChallengeList[i].aOpponent[1];
    }
    return gChallengeList[i].aOpponent[2];
}

// Sets the flag the calendar screen controls (FE message 694, CalendarScreen.c; see
// PlayNow_GetCalendarFlag for what it changes).
void PlayNow_SetCalendarFlag(u8 bOn) {
    gPlayNowCalendarFlag = bOn;
}

// The flag the calendar screen sets (PlayNow_SetCalendarFlag; FE message 695 reads it). While it is
// set the loading screen uses file 2 (LoadData.c), the hole contests change (GameHoleContests.c:
// the round counts as having none, but the long drive and closest to the pin are on holes 18 and 17
// outside a playoff), a match-play challenge's playoff decides medal 1 or 2 (PlayNow_GetMedal), and
// a best medal earns award 0x1C, a trophy ball PlayNow_EndGame pays.
u8 PlayNow_GetCalendarFlag(void) {
    return gPlayNowCalendarFlag;
}

// Forces the next weather pick (fn_8006F650, at the start of a hole) to its effect bit 1 at amount
// fAmount (kept to 0.1..1 when applied), the effect the game option nWeather 3 picks with a random
// amount. A challenge with bForceWeather passes its fWeatherAmount (PlayNow_ApplyChallengeSetup); a
// replay passes its saved amount (GameModeReplay.c).
void PlayNow_ForceWeather(f32 fAmount) {
    lbl_802811F0->b1C = 1;
    lbl_802811F0->f18 = fAmount;
}

// Sends game message 18 with player nPlayer. The timed modes send it when a player's turn or time
// ends (GameMode8.c, GameMode_SkillZoneTimed.c), and PlayNow_OnPause when a speed golf challenge is paused.
void PlayNow_SendMessage18(s32 nPlayer) {
    GameMsg_SendInt(18, nPlayer);
}
