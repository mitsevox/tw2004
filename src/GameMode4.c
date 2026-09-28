// GameMode4.c (our name): game mode 4, the matches of the 25-event ladder loaded from the 'TCM '
// stream (gLadderEvents, 0x44 bytes an event; names from 'TCMS'). An event is a one-on-one match
// against a pro (mode 4, match play with GameModeMatch's callbacks) or a mode 5 challenge (neither
// is set up for an event with n1C set; GameModeSkins_EndGame also scores the current event).
// Winning one sets its flag in the save profile, pays its prize and unlocks the pro and a reward.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

// One event of the ladder.
typedef struct LadderEvent {
    s32 n0;                     // 0x00
    s32 nGolfer;                // 0x04  the opponent (34 = none)
    s32 nCourse;                // 0x08
    s32 nTeeSet;                // 0x0C
    s32 nPins;                  // 0x10  the pin set plus 1 (0 = leave it)
    s32 nHoles;                 // 0x14  the hole-selection preset
    s32 nChallenge;             // 0x18  a mode 5 challenge plus 1 (0 = a match)
    s32 n1C;                    // 0x1C  nonzero: not played here
    s32 nReward;                // 0x20  the reward unlocked plus 1 (0 = none)
    s32 aNeeded[6];             // 0x24  events that must be won first, plus 1 (0 = none)
    s32 nName;                  // 0x3C  the name's offset in the 'TCMS' text
    s32 n40;                    // 0x40
} LadderEvent;
LadderEvent gLadderEvents[25];

// The 'TCMS' text: the events' names.
typedef struct LadderNames {
    char* pText;
    u32   uSize;
} LadderNames;

s32 gLadderSavedWeather = 4;                    // the options' unkC, saved while a match is played
void (*gLadderChallengeShutdown)(void);          // the challenge's own end-of-mode callback
s32 gLadderEventBonus;                    // money to add to the course tracking when the event ends
s32 gLadderOpponent;                    // the event's opponent
s32 gLadderReward;                    // the event's reward plus 1
LadderNames gLadderNames;
s32 gLadderCurrentEvent;                    // the current event
u8  gLadderEventRunning;                    // a ladder event is being played
s32 gLadderSavedWind;                    // the wind option, saved

void  GM_Earnings_AwardDoubleMoney(int nPlayer, int nMoney);

void GameMode4_Shutdown(void);
u8   GameMode4_HasWonEvent(int nProfile, int nEvent);
int  GameMode4_GetEventHoles(int nEvent);
int  GameMode4_GetEventKind(int nEvent);
int  GameMode4_GetCurrentEvent(void);
u8   GameMode4_IsEventOpen(int nProfile, int nEvent);
void GameMode4_LoadTCMFromStream(UStreamObject* pObject);
void GameMode4_LoadTCMSFromStream(UStreamObject* pObject);
void GameMode4_EndGame(void);
void GameMode4_QueueRegionFinalMessage(void);
void GameMode4_WinEvent(void);

// Mode 4's setup (GM_SetModeType): GameModeMatch's match-play callbacks with this file's own
// shutdown and end of game; no mulligans, no split screen. Unlike GameModeMatch_Init it leaves
// bAIConcedes as it was and sets nC and n10 to 1 (GameMode4_StartEvent sets 2 for a match).
void GameMode4_Init(void) {
    gpGame->pfnInit = GameMode4_Init;
    gpGame->pfnShutdown = GameMode4_Shutdown;
    gpGame->pfnSetupNextGolfer = fn_800E9F14;
    gpGame->pfnGetHonors = GameModeMatch_GetHonors;
    gpGame->pfnHoleFinished = GameModeMatch_HoleFinished;
    gpGame->pfnGameFinished = GameModeMatch_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeMatch_GoToPlayoff;
    gpGame->pfnEndHole = GameModeMatch_EndHole;
    gpGame->pfnEndGame = GameMode4_EndGame;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// Empty in this build; the front end's shutdown (fn_8006CB2C, gomainloop.c) calls it among the
// other systems' close calls.
void GameMode4_CloseFE(void) {
}

// How many of the 25 ladder events player 0's profile has won (fn_800584DC counts its aLadderAward
// flags). The money rating (GM_GetGolferMoneyRating) and two front-end messages read it.
int GameMode4_GetNumEventsWon(void) {
    return fn_800584DC(gPlayers[0].nIndex);
}

// Event nEvent's opponent, a golfer id (34 = none: GameMode4_WinEvent then unlocks nobody).
int GameMode4_GetEventOpponent(int nEvent) {
    return gLadderEvents[nEvent].nGolfer;
}

int GameMode4_GetEventCourse(int nEvent) {
    return gLadderEvents[nEvent].nCourse;
}

// Event nEvent's hole set, as GM_SelectHoleSet takes it: 1 all 18 holes, 2 the front nine, 3 the
// back nine (gLadderHoleSetNames; 0 is "None").
int GameMode4_GetEventHoles(int nEvent) {
    return gLadderEvents[nEvent].nHoles;
}

int GameMode4_GetCurrentEventHoles(void) {
    return GameMode4_GetEventHoles(GameMode4_GetCurrentEvent());
}

// The current event's kind (GameMode4_GetEventKind: 0 not played here, 1 challenge, 2 region or
// World final, 3 match).
int GameMode4_GetCurrentEventKind(void) {
    return GameMode4_GetEventKind(GameMode4_GetCurrentEvent());
}

// The kind of event nEvent, tested in this order: 0 when its n1C is set (not played here), 1 a Play
// Now challenge (nChallenge set), 2 a region final (events 3, 7, 11, 15, 19 and 23) or the World
// final (24), 3 any other match.
int GameMode4_GetEventKind(int nEvent) {
    if (gLadderEvents[nEvent].n1C != 0) {
        return 0;
    }
    if (gLadderEvents[nEvent].nChallenge != 0) {
        return 1;
    }
    if (nEvent == 3 || nEvent == 7 || nEvent == 11 || nEvent == 15 || nEvent == 19 || nEvent == 23 ||
        nEvent == 24) {
        return 2;
    }
    return 3;
}

// The current ladder event, 0..24 (gLadderCurrentEvent, set by GameMode4_SelectEvent).
int GameMode4_GetCurrentEvent(void) {
    return gLadderCurrentEvent;
}

// Whether profile nProfile has won event nEvent: its aLadderAward entry's bWon, which
// GameMode4_WinEvent sets through GM_Earnings_GiveAwardToUser.
u8 GameMode4_HasWonEvent(int nProfile, int nEvent) {
    return gpSaveData[nProfile].aLadderAward[nEvent].bWon;
}

// Whether profile nProfile may play event nEvent: it has won every event in the event's aNeeded
// list (up to six, each stored plus 1; 0 is an empty entry).
u8 GameMode4_IsEventOpen(int nProfile, int nEvent) {
    int i;
    u8 bOpen = 1;
    for (i = 0; i < 6; i++) {
        if (gLadderEvents[nEvent].aNeeded[i] != 0 &&
            !GameMode4_HasWonEvent(nProfile, gLadderEvents[nEvent].aNeeded[i] - 1)) {
            bOpen = 0;
            break;
        }
    }
    return bOpen;
}

// Makes nEvent the current event (gLadderCurrentEvent) when profile nProfile may play it
// (GameMode4_IsEventOpen). Returns 1 if it did; 0 leaves the current event as it was.
u8 GameMode4_SelectEvent(int nProfile, int nEvent) {
    u8 bOk = 0;
    if (GameMode4_IsEventOpen(nProfile, nEvent)) {
        gLadderCurrentEvent = nEvent;
        bOk = 1;
    }
    return bOk;
}

// Sets the bonus a won event pays (gLadderEventBonus): GameMode4_WinEvent pays twice the amount
// (GM_Earnings_AwardDoubleMoney) when it is not 0. Front-end message 66 sets it; nothing else
// writes it.
void GameMode4_SetEventBonus(s32 n) {
    gLadderEventBonus = n;
}

// Registers the ladder's stream chunks: 'TCM ' (the 25 events) and 'TCMS' (their names).
void GameMode4_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('TCM ', GameMode4_LoadTCMFromStream);
    Stream_RegisterLoadChunkCallback('TCMS', GameMode4_LoadTCMSFromStream);
}

// Unregisters the 'TCM ' chunk only; 'TCMS' stays registered.
void GameMode4_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('TCM ');
}

// The 'TCM ' chunk: the 25 events, copied over gLadderEvents.
void GameMode4_LoadTCMFromStream(UStreamObject* pObject) {
    // port: the 'TCM ' object is copied straight into the ladder events (LadderEvent[25]); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gLadderEvents), gLadderEvents);
}

// The 'TCMS' chunk: the events' names, copied into a new 16-byte-aligned block (fn_800951A0) that
// gLadderNames keeps with its size. A NULL object is ignored.
void GameMode4_LoadTCMSFromStream(UStreamObject* pObject) {
    if (pObject) {
        gLadderNames.uSize = pObject->uSize;
        gLadderNames.pText = fn_800951A0(gLadderNames.uSize, 0x10, 1);
        memcpy(gLadderNames.pText, pObject->pData, gLadderNames.uSize);
    }
}

// Mode 4's shutdown (pfnShutdown). For a challenge event it takes the place of mode 5's shutdown,
// which it calls first (gLadderChallengeShutdown). Then nC and n10 go back to 1, the weather and
// wind options saved by GameMode4_StartEvent are restored, and no ladder event is running any more.
void GameMode4_Shutdown(void) {
    if (gLadderChallengeShutdown) {
        gLadderChallengeShutdown();
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nWeather = gLadderSavedWeather;
    gSession.options.nWind = gLadderSavedWind;
    gLadderEventRunning = 0;
}

// Sets the session up for the current event (front-end message 46, LadderMenu_StartEvent). The
// weather and wind options are saved and forced clear and calm (4 and 0), a ladder event is marked
// running, and the event's opponent and reward are kept for GameMode4_WinEvent. A challenge event
// switches to mode 5 with one player and starts Play Now challenge nChallenge - 1, its shutdown
// chained through GameMode4_Shutdown; a match has two players, player 1 the event's opponent under
// CPU control, on the event's course, hole set and tee set, and pin set nPins - 1 when nPins is not
// 0. An event with n1C set gets neither.
void GameMode4_StartEvent(void) {
    int nEvent;
    int nPins;
    gLadderSavedWeather = gSession.options.nWeather;
    gLadderSavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
    gSession.options.nWind = 0;
    gLadderEventRunning = 1;
    nEvent = GameMode4_GetCurrentEvent();
    gLadderOpponent = gLadderEvents[nEvent].nGolfer;
    gLadderReward = gLadderEvents[nEvent].nReward;
    if (gLadderEvents[nEvent].n1C == 0) {
        if (gLadderEvents[nEvent].nChallenge != 0) {
            gSession.nNumPlayers = 1;
            GM_SetModeType(5);
            PlayNow_SelectChallenge(gLadderEvents[nEvent].nChallenge - 1);
            PlayNow_StartChallenge();
            gLadderChallengeShutdown = gpGame->pfnShutdown;
            gpGame->pfnShutdown = GameMode4_Shutdown;
        } else {
            gLadderChallengeShutdown = NULL;
            gpGame->nC = 2;
            gpGame->n10 = 2;
            Session_SetNumPlayers(2);
            Session_SetGolfer(gLadderEvents[nEvent].nGolfer, 1);
            gSession.nController[1] = CONTROLLER_CPU;
            GM_SetCurrentCourse(gLadderEvents[nEvent].nCourse);
            GM_SelectHoleSet(gLadderEvents[nEvent].nHoles);
            nPins = gLadderEvents[nEvent].nPins;
            gSession.nTeeSet[0] = gLadderEvents[nEvent].nTeeSet;
            gSession.nTeeSet[1] = gLadderEvents[nEvent].nTeeSet;
            if (nPins != 0) {
                gSession.nPinSet = nPins - 1;
            }
        }
    }
}

// Whether a ladder event is being played, from GameMode4_StartEvent to GameMode4_Shutdown. Skins
// and Play Now scoring, the earnings goals and the front end's game setup check it.
u8 GameMode4_IsEventRunning(void) {
    return gLadderEventRunning;
}

// Mode 4's end of game (pfnEndGame) for a match. When player 0 won more holes than player 1 and has
// an active profile: the game counts as won for the EA Sports Bio, the prize message (0x6E with the
// event's base prize) is queued, GM_Earnings_GetLadderWinnings's amount (the base prize plus so
// much a hole of the margin, at most 5 holes; 0 with mulligans) is paid and booked in money.nC and
// money.n10, and GameMode4_WinEvent scores the event. A loss changes nothing.
void GameMode4_EndGame(void) {
    int nMargin;
    int nMoney;
    int nProfile;
    int nEvent;
    s32 nPrize;
    if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
        nMargin = gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
        if (nMargin > 5) {
            nMargin = 5;
        }
        nMoney = GM_Earnings_GetLadderWinnings(0, 1, nMargin, &nPrize);
        nProfile = gPlayers[0].nIndex;
        if (gpSaveData[nProfile].bActive) {
            EASBio_SetCurrentGameWon(1);
            GUI_QueueMessage(0, 0x6E, nPrize, nProfile);
            GM_Earnings_AwardMoney(0, nMoney, NULL);
            nEvent = GameMode4_GetCurrentEvent();
            gPlayers[0].money.nC += gEarningsTable.aLadderPrize[nEvent].nBase;
            gPlayers[0].money.n10 += gEarningsTable.aLadderPrize[nEvent].nPerHole * nMargin;
            GameMode4_WinEvent();
        }
    }
}

// Player 0 won a ladder event played as Skins (GameModeSkins_EndGame): the event's base prize
// (GM_Earnings_GetLadderWinnings with no margin; 0 with mulligans), when it is not 0 and player 0's
// profile is active, is announced (message 0x6E), paid and booked in money.nC. Then
// GameMode4_WinEvent scores the event.
void GameMode4_WinSkinsEvent(void) {
    s32 nPrize;
    int nMoney = GM_Earnings_GetLadderWinnings(0, 1, 0, &nPrize);
    if (nMoney != 0) {
        int nIndex = gPlayers[0].nIndex;
        if (gpSaveData[nIndex].bActive) {
            GUI_QueueMessage(0, 0x6E, nPrize, nIndex);
            GM_Earnings_AwardMoney(0, nMoney, NULL);
            gPlayers[0].money.nC += nMoney;
        }
    }
    GameMode4_WinEvent();
}

// Queues the ladder message for a won region final (display queue 11, shown by
// GUI_ShowLadderMessage): kind 4 for event 3, 7 for event 7, 3 for event 11, 8 for event 15, 5 for
// event 19 and 13 for event 23. Any other event queues nothing.
void GameMode4_QueueRegionFinalMessage(void) {
    switch (GameMode4_GetCurrentEvent()) {
    case 11:
        GUI_QueueMessage(11, 3, 0, 0);
        break;
    case 3:
        GUI_QueueMessage(11, 4, 0, 0);
        break;
    case 19:
        GUI_QueueMessage(11, 5, 0, 0);
        break;
    case 7:
        GUI_QueueMessage(11, 7, 0, 0);
        break;
    case 15:
        GUI_QueueMessage(11, 8, 0, 0);
        break;
    case 23:
        GUI_QueueMessage(11, 13, 0, 0);
        break;
    }
}

// Scores the current event as won for player 0's profile, when it is active: the region-final
// message (GameMode4_QueueRegionFinalMessage); the event's award marked won with today's date; the
// opponent unlocked, with a message (queue 4), unless it is 34 or already available; the reward
// nReward - 1 unlocked, with a message (queue 3, 0x16); the bonus (gLadderEventBonus) paid twice.
// For a final (kind 2): the World final (24) queues message 0x1A (queue 5) and, when trophy ball 15
// is newly given, pays gEarningsTable.nLadderDone with its message and books it in money.n8; a
// region final queues message 20 + nEvent / 4 (queue 5).
void GameMode4_WinEvent(void) {
    int nProfile = gPlayers[0].nIndex;
    int nEvent;
    u8 bLast;
    if (gpSaveData[nProfile].bActive) {
        nEvent = GameMode4_GetCurrentEvent();
        GameMode4_QueueRegionFinalMessage();
        // EA bug: the profile number goes in as the player number, so GM_Earnings_GiveAwardToUser checks
        // gPlayers[nProfile] (player 0 only while player 0 plays profile 0).
        GM_Earnings_GiveAwardToUser(nProfile, &gpSaveData[nProfile].aLadderAward[nEvent]);
        if (gLadderOpponent != 34 && !fn_8005832C(nProfile, gLadderOpponent)) {
            fn_80058278(nProfile, gLadderOpponent);
            GUI_QueueMessage(4, gLadderOpponent, 0, nProfile);
        }
        if (gLadderReward != 0) {
            fn_80058428(nProfile, gLadderReward - 1);
            GUI_QueueMessage(3, 0x16, gLadderReward, nProfile);
        }
        if (gLadderEventBonus != 0) {
            GM_Earnings_AwardDoubleMoney(0, gLadderEventBonus);
        }
        if (GameMode4_GetCurrentEventKind() == 2) {
            bLast = 0;
            if (nEvent >= 24) {
                bLast = 1;
            }
            if (bLast == 1) {
                GUI_QueueMessage(5, 0x1A, 0, nProfile);
                if (GM_Earnings_AwardTrophyBall(0, 15)) {
                    GUI_QueueMessage(2, 15, gEarningsTable.nLadderDone, nProfile);
                    GM_Earnings_AwardMoney(0, gEarningsTable.nLadderDone, NULL);
                    gPlayers[0].money.n8 += gEarningsTable.nLadderDone;
                }
            } else if ((nEvent + 1) % 4 == 0) {
                GUI_QueueMessage(5, nEvent / 4 + 20, 0, nProfile);
            }
        }
    }
}

// Event nEvent's first word (LadderEvent.n0). Only front-end message 579
// (LadderMenu_GetNodeEventN0) reads it; what it holds is not known.
int GameMode4_GetEventN0(int nEvent) {
    return gLadderEvents[nEvent].n0;
}

// Copies event nEvent's name (its nName offset into the 'TCMS' text, gLadderNames) to szOut; szOut
// is left as it was for an event outside 0..24.
void GameMode4_GetEventName(int nEvent, char* szOut) {
    if (nEvent < 0 || nEvent >= 25) return;
    strcpy(szOut, gLadderNames.pText + gLadderEvents[nEvent].nName);
}

// Event nEvent's tour stop number, which the map's panel prints as "<region> / Tour Stop <n>"
// (LadderMenu_GetEventText).
int GameMode4_GetEventTourStop(int nEvent) {
    return gLadderEvents[nEvent].n40;
}

// Empty in this build; called last when a game started from the menus is set up (fn_80079AD4,
// FE_Manager.c), right after Gaud_ExitFE.
void GameMode4_ExitFE(void) {
}
