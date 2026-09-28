// GameModeDriverRTE.c (TW06's GameModeDriverRTE class; TW07's GameModeDriver_RealTimeEvents.cpp):
// game mode 24, the real-time events. A calendar of 118 dated entries ('RTEc'), each playing one of
// 111 challenges ('RTEs'; the mode 5 code in GameMode5.c runs them) on its date by the console's
// clock, with their names and descriptions in 'RTEn'. Also the queries the calendar, its day panels
// and the trophy room use (GM_RealtimeMode_*, TW07's names), and the purse and award a medal wins.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/rte.h"

// This file's globals, defined last address first (an object's .bss is laid out in reverse).
s32 gRTESavedOptionC = 4;               // gSession.options.nWeather saved while an event runs
void (*gRTEChallengeShutdown)(void);    // mode 5's pfnShutdown, called from GameModeDriverRTE_Shutdown
void (*gRTEChallengeEndGame)(void);     // mode 5's pfnEndGame, called from GameModeDriverRTE_EndGame
s32 gRTESelectedDay;                    // the selected event's day (1-based)
s32 gRTESelectedEvent;                  // the selected event (gRTEs.aEvent index)
u8  gRTEEventRunning;                   // 1 while an event runs (GM_Currently_RealtimeMode)
s32 gRTESavedWind;                      // gSession.options.nWind saved while an event runs
RTEData gRTEs;

void GameModeDriverRTE_UnregisterStreamClients(void);
void GameModeDriverRTE_Locale_LoadRTEcFromStream(UStreamObject* pObject);
void GameModeDriverRTE_LoadRTEsFromStream(UStreamObject* pObject);
s32 GameModeDriverRTE_GetEventDays(s32 i);
s32 GM_RealtimeMode_GetSelectedEvent(s32* pRound);
void GM_RealtimeMode_SelectEvent(s32 nId, s32 nRound);

void  GameModeDriverRTE_Locale_LoadRTEnFromStream(UStreamObject* pObject);
void  GameModeDriverRTE_Shutdown(void);
void  GameModeDriverRTE_EndGame(void);
s32   GM_RealtimeMode_GetNEventsWon(void);
u8    GameModeDriverRTE_GetEventByDateMDY(s32 nMonth, s32 nDay, s32 nYear, s32* pId, s32* pRound);
s32   GameModeDriverRTE_GetYearIndex(void);

// Game mode 24 (real-time events; TW06 GM_Realtime_mode) starts, from GameRound's mode switch:
// match play's golfer order, honors, hole, game-over and playoff rules (GameModeMatch) with this
// file's GameModeDriverRTE_Shutdown and GameModeDriverRTE_EndGame; no mulligans, nC and n10 1, nDC
// 0, single view. The event itself is played later as a mode 5 challenge
// (GameModeDriverRTE_StartEvent).
void GameModeDriverRTE_Init(void) {
    gpGame->pfnInit = GameModeDriverRTE_Init;
    gpGame->pfnShutdown = GameModeDriverRTE_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeMatch_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeMatch_GetHonors;
    gpGame->pfnHoleFinished = GameModeMatch_HoleFinished;
    gpGame->pfnGameFinished = GameModeMatch_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeMatch_GoToPlayoff;
    gpGame->pfnEndHole = GameModeMatch_EndHole;
    gpGame->pfnEndGame = GameModeDriverRTE_EndGame;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

void GameModeDriverRTE_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('RTEc', GameModeDriverRTE_Locale_LoadRTEcFromStream);
    Stream_RegisterLoadChunkCallback('RTEs', GameModeDriverRTE_LoadRTEsFromStream);
    Stream_RegisterLoadChunkCallback('RTEn', GameModeDriverRTE_Locale_LoadRTEnFromStream);
}

void GameModeDriverRTE_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('RTEc');
    Stream_UnregisterLoadChunkCallback('RTEs');
    Stream_UnregisterLoadChunkCallback('RTEn');
}

// The 'RTEc' stream object: the 118 calendar entries, copied into gRTEs.aEvent.
void GameModeDriverRTE_Locale_LoadRTEcFromStream(UStreamObject* pObject) {
    // port: the 'RTEc' object is copied straight into gRTEs.aEvent (RTEvent[118]); it is big-endian
    //       on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gRTEs.aEvent), gRTEs.aEvent);
}

// The 'RTEs' stream object: the events' 111 challenges (TW06 scenarios), copied into
// gRTEs.aChallenge.
void GameModeDriverRTE_LoadRTEsFromStream(UStreamObject* pObject) {
    // port: the 'RTEs' object is copied straight into gRTEs.aChallenge (Challenge[111]); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gRTEs.aChallenge), gRTEs.aChallenge);
}

// The 'RTEn' stream object: the events' names and descriptions (the string table RTEvent.nName and
// nDesc index), copied into a new 16-byte aligned block at gRTEs.pNames; the stream's copy is
// freed. An empty object leaves pNames as it was.
void GameModeDriverRTE_Locale_LoadRTEnFromStream(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gRTEs.pNames = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gRTEs.pNames, pData, nSize);
        StaticMem_Free(pObject);
    }
}

// Mode 24's shutdown (also wrapped around mode 5's while an event's challenge runs): mode 5's own
// shutdown when one was saved, gpGame nC and n10 back to 1, the options nWeather and wind that
// GameModeDriverRTE_StartEvent changed put back, and the event is no longer running
// (GM_Currently_RealtimeMode).
void GameModeDriverRTE_Shutdown(void) {
    if (gRTEChallengeShutdown) {
        gRTEChallengeShutdown();
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nWeather = gRTESavedOptionC;
    gSession.options.nWind = gRTESavedWind;
    gRTEEventRunning = 0;
}

// Starts the selected event (GM_RealtimeMode_SelectEvent): the options nWeather and wind are saved
// and set to 4 and 0 (calm), and the event counts as running. If the entry is not switched off
// (bOff) and has a challenge, one player plays it in mode 5: the RTE challenge table
// (gRTEs.aChallenge) goes in, challenge nChallenge - 1 is selected and started, and mode 5's
// shutdown and end game are saved and replaced by GameModeDriverRTE_Shutdown and
// GameModeDriverRTE_EndGame. Called from the menu (FE_MessageTable.c) and on a restart (GameMode5).
void GameModeDriverRTE_StartEvent(void) {
    gRTESavedOptionC = gSession.options.nWeather;
    gRTESavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
    gSession.options.nWind = 0;
    gRTEEventRunning = 1;
    if (gRTEs.aEvent[gRTESelectedEvent].bOff == 0) {
        if (gRTEs.aEvent[gRTESelectedEvent].nChallenge != 0) {
            gSession.nNumPlayers = 1;
            GM_SetModeType(5);
            PlayNow_SetChallengeList(gRTEs.aChallenge, 111);
            PlayNow_SelectChallenge(gRTEs.aEvent[gRTESelectedEvent].nChallenge - 1);
            PlayNow_StartChallenge();
            gRTEChallengeShutdown = gpGame->pfnShutdown;
            gRTEChallengeEndGame = gpGame->pfnEndGame;
            gpGame->pfnShutdown = GameModeDriverRTE_Shutdown;
            gpGame->pfnEndGame = GameModeDriverRTE_EndGame;
        }
    }
}

// The next challenge of the event's group starts (GameMode5, after the previous one, in mode 24):
// mode 5's challenge start, then its shutdown and end game are saved and replaced by
// GameModeDriverRTE_Shutdown and GameModeDriverRTE_EndGame, as GameModeDriverRTE_StartEvent does.
void GameModeDriverRTE_StartNextChallenge(void) {
    PlayNow_StartChallenge();
    gRTEChallengeShutdown = gpGame->pfnShutdown;
    gRTEChallengeEndGame = gpGame->pfnEndGame;
    gpGame->pfnShutdown = GameModeDriverRTE_Shutdown;
    gpGame->pfnEndGame = GameModeDriverRTE_EndGame;
}

// 1 while a real-time event is being played (from GameModeDriverRTE_StartEvent to
// GameModeDriverRTE_Shutdown).
u8 GM_Currently_RealtimeMode(void) {
    return gRTEEventRunning;
}

// How many of the 75 real-time event awards profile 0 has won (SaveProfile.aRTEAward); 0 before the
// first win (GameModeDriverRTE_QueueWinMessages).
s32 GM_RealtimeMode_GetNEventsWon(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SaveProfile* p = &gpSaveData[nPlayer];
    s32 n = 0;
    s32 i;
    for (i = 0; i < 75; i++) {
        if (p->aRTEAward[i].bWon == 1) {
            n++;
        }
    }
    return n;
}

// The HUD messages after an event is won (GUI_QueueMessage queue 10), queued before its award is
// marked won: message 0 for the player's first win (no award won yet); a message of its own for
// some events (by the selected event's index: 0xD..0x19, or 5); when neither applies, one of 2, 3,
// 4 or 7 at random.
void GameModeDriverRTE_QueueWinMessages(void) {
    u8 bFirst = 0;
    u8 bSaid;
    if (!GM_RealtimeMode_GetNEventsWon()) {
        GUI_QueueMessage(10, 0, 0, 0);
        bFirst = 1;
    }
    bSaid = 1;
    switch (gRTESelectedEvent) {
    case 0x0:
        GUI_QueueMessage(10, 0xD, 0, 0);
        break;
    case 0x2:
    case 0x6:
        GUI_QueueMessage(10, 0xE, 0, 0);
        break;
    case 0x7:
        GUI_QueueMessage(10, 0xF, 0, 0);
        break;
    case 0x12:
    case 0xE:
        GUI_QueueMessage(10, 0x10, 0, 0);
        break;
    case 0x16:
        GUI_QueueMessage(10, 0x11, 0, 0);
        break;
    case 0x1F:
        GUI_QueueMessage(10, 0x12, 0, 0);
        break;
    case 0x41:
        GUI_QueueMessage(10, 0x13, 0, 0);
        break;
    case 0x55:
        GUI_QueueMessage(10, 0x14, 0, 0);
        break;
    case 0x5D:
        GUI_QueueMessage(10, 0x15, 0, 0);
        break;
    case 0x6D:
        GUI_QueueMessage(10, 0x16, 0, 0);
        break;
    case 0x6E:
        GUI_QueueMessage(10, 0x17, 0, 0);
        break;
    case 0x6F:
        GUI_QueueMessage(10, 0x18, 0, 0);
        break;
    case 0x56:
    case 0x17:
        GUI_QueueMessage(10, 0x19, 0, 0);
        break;
    case 0x18:
    case 0x1E:
        GUI_QueueMessage(10, 5, 0, 0);
        break;
    default:
        bSaid = 0;
        break;
    }
    if (!bFirst && !bSaid) {
        switch (Misc_RandFunc(0) & 3) {
        case 0:
            GUI_QueueMessage(10, 2, 0, 0);
            return;
        case 1:
            GUI_QueueMessage(10, 3, 0, 0);
            return;
        case 2:
            GUI_QueueMessage(10, 4, 0, 0);
            return;
        default:
            GUI_QueueMessage(10, 7, 0, 0);
            break;
        }
    }
}

// The event's end of game: mode 5's own end game first (called unchecked: it is saved by
// GameModeDriverRTE_StartEvent), then with any medal (PlayNow_GetMedal below 3) player 0 is paid the
// challenge's best-medal reward (GameModeDriverRTE_GetPurse's amount, whatever the medal), the
// money message (queue 0, 0x6F) and GameModeDriverRTE_QueueWinMessages go up, and profile 0's award
// for the event is marked won with today's date (GM_Earnings_GiveAwardToUser).
void GameModeDriverRTE_EndGame(void) {
    s32 nReward;
    gRTEChallengeEndGame();
    if (PlayNow_GetMedal() != 3) {
        nReward = gRTEs.aChallenge[gRTEs.aEvent[gRTESelectedEvent].nChallenge - 1].aMedal[0].nReward;
        GM_Earnings_AwardMoney(0, nReward, 0);
        GUI_QueueMessage(0, 0x6F, nReward, 0);
        GameModeDriverRTE_QueueWinMessages();
        GM_Earnings_GiveAwardToUser(0, &gpSaveData->aRTEAward[gRTEs.aEvent[gRTESelectedEvent].nId]);
    }
}

// Today's date from the clock.
void GameModeDriverRTE_GetCurrentDate(s32* pMonth, s32* pDay, s32* pYear) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;
    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    *pMonth = nMonth;
    *pDay = nDay;
    *pYear = nYear;
}

// Finds the event held on day number nDate (CalDate): the first entry whose start date for that
// year is set and within GameModeDriverRTE_GetEventDays of it. Found: *pId is its index in
// gRTEs.aEvent, *pRound its day (1-based), and 1 is returned; else -1, 0 and 0.
u8 GameModeDriverRTE_GetEventByDate(u16 nDate, s32* pId, s32* pRound) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 i;
    s32 nDelta;
    s32 nSeason;
    u8 bFound;
    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    bFound = 0;
    nSeason = nYear - 2003;
    // EA bug: nSeason is not checked against the ten seasons (GameModeDriverRTE_GetNextEvent checks
    // it), so a date outside 2003..2012 reads past aDate.
    for (i = 0; i < 118; i++) {
        if (gRTEs.aEvent[i].aDate[nSeason] != 0) {
            nDelta = nDate - gRTEs.aEvent[i].aDate[nSeason];
            if (nDelta >= 0 && nDelta < GameModeDriverRTE_GetEventDays(i)) {
                *pId = i;
                bFound = 1;
                *pRound = nDelta + 1;
                break;
            }
        }
    }
    if (!bFound) {
        *pId = -1;
        *pRound = 0;
    }
    return bFound;
}

// GameModeDriverRTE_GetEventByDate for a month, day and year.
u8 GameModeDriverRTE_GetEventByDateMDY(s32 nMonth, s32 nDay, s32 nYear, s32* pId, s32* pRound) {
    u16 nDate;
    CalDate_SetMDY(&nDate, nMonth, nDay, nYear);
    return GameModeDriverRTE_GetEventByDate(nDate, pId, pRound);
}

// How many days event i lasts: always one.
s32 GameModeDriverRTE_GetEventDays(s32 i) {
    return 1;
}

// The selected event (an index into gRTEs.aEvent, set by GM_RealtimeMode_SelectEvent); its day goes
// in *pRound.
s32 GM_RealtimeMode_GetSelectedEvent(s32* pRound) {
    *pRound = gRTESelectedDay;
    return gRTESelectedEvent;
}

// Event nId, on its day nRound, becomes the selected one (GameModeDriverRTE_StartEvent plays it).
void GM_RealtimeMode_SelectEvent(s32 nId, s32 nRound) {
    gRTESelectedDay = nRound;
    gRTESelectedEvent = nId;
}

// Today's event (by the clock) becomes the selected one; 1 if there is one, else 0 and the
// selection stays as it was.
s32 GM_RealtimeMode_SelectEventToday(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nId;
    s32 nRound;
    GameModeDriverRTE_GetCurrentDate(&nMonth, &nDay, &nYear);
    if (GameModeDriverRTE_GetEventByDateMDY(nMonth, nDay, nYear, &nId, &nRound)) {
        GM_RealtimeMode_SelectEvent(nId, nRound);
        return 1;
    }
    return 0;
}

RTEvent* GameModeDriverRTE_GetCalData(s32 i) {
    return &gRTEs.aEvent[i];
}

// The calendar entry of the event held on day number nDate, or NULL when none; the real-time
// driver's entry in the calendar's per-driver table (GameModeDriver.c).
RTEvent* GM_RealtimeMode_GetEventInfoByDate(u16 nDate) {
    s32 nId;
    s32 nRound;
    if (GameModeDriverRTE_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverRTE_GetCalData(nId);
    }
    return 0;
}

char* GameModeDriverRTE_GetName(s32 i) {
    return gRTEs.pNames + gRTEs.aEvent[i].nName;
}

char* GameModeDriverRTE_GetDescription(s32 i) {
    return gRTEs.pNames + gRTEs.aEvent[i].nDesc;
}

// Event i's purse: the best-medal reward of its challenge (Challenge.aMedal[0].nReward), which
// GameModeDriverRTE_EndGame pays for any medal.
s32 GameModeDriverRTE_GetPurse(s32 i) {
    return gRTEs.aChallenge[gRTEs.aEvent[i].nChallenge - 1].aMedal[0].nReward;
}

// The clock's year as an index into RTEvent.aDate: 0 for 2003 up to 9 for 2012, and 0 for any year
// outside that range.
s32 GameModeDriverRTE_GetYearIndex(void) {
    s32 nYear;
    s32 nUnused;
    s32 bOk;
    s32 nSeason;
    fn_8011E020(&nUnused, &nUnused, &nYear, &nUnused, &nUnused, &nUnused, &nUnused);
    nSeason = nYear - 2003;
    bOk = 0;
    if (nSeason >= 0 && nSeason < 10) {
        bOk = 1;
    }
    return bOk ? nSeason : 0;
}

// Event i's start date (day number) in the clock's year (GameModeDriverRTE_GetYearIndex), 0 when it
// is not held that year; 0xFFFF for no entry, which GameModeDriverRTE_GetCalData never gives.
u16 GM_RealtimeMode_GetStartDate(s32 i) {
    RTEvent* p = GameModeDriverRTE_GetCalData(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aDate[GameModeDriverRTE_GetYearIndex()];
}

// Event i's icon (RTEvent.n14), shown in its day cell on the calendar (GameModeDriver.c).
s32 GameModeDriverRTE_UI_GetEventIconIndexOnCal(s32 i) {
    return GameModeDriverRTE_GetCalData(i)->n14;
}

// Always 0 in this build, so the calendar shows today's event with the not-yet-played panel (4) and
// never the completed one (5).
u8 GM_RealtimeMode_TodaysEventCompleted(void) {
    return 0;
}

// The index of the event starting soonest from today (today included) in the clock's year, by the
// entries' start dates; -1 when none is left or the year is outside 2003..2012.
s32 GameModeDriverRTE_GetNextEvent(void) {
    s32 nNext;
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u16 nToday;
    s32 nSeason;
    s32 nBestDelta = -1;
    s32 i;
    s32 nDelta;
    u8 bFound;
    GameModeDriverRTE_GetCurrentDate(&nMonth, &nDay, &nYear);
    nSeason = nYear - 2003;
    CalDate_SetMDY(&nToday, nMonth, nDay, nYear);
    if (nSeason >= 0 && nSeason < 10) {
        bFound = 0;
        for (i = 0; i < 118; i++) {
            if (gRTEs.aEvent[i].aDate[nSeason] != 0) {
                nDelta = gRTEs.aEvent[i].aDate[nSeason] - nToday;
                if (nDelta >= 0 && (nBestDelta == -1 || nDelta < nBestDelta)) {
                    nNext = i;
                    nBestDelta = nDelta;
                    bFound = 1;
                }
            }
        }
        if (!bFound) {
            nNext = -1;
        }
    } else {
        nNext = -1;
    }
    return nNext;
}

// The icon (RTEvent.n14) of the first event whose award id is nId, 0 if none; the trophy room shows
// it for the award (EventInfo.c, GameMode22.c).
s32 GM_RealtimeMode_GetIconIDByTrophyGroup(s32 nId) {
    s32 i;
    for (i = 0; i < 118; i++) {
        if (nId == gRTEs.aEvent[i].nId) {
            return gRTEs.aEvent[i].n14;
        }
    }
    return 0;
}

// Copies into pDst the name of the event whose award id is nId (of every such event, so the last
// one wins); pDst is left alone when there is none.
void GM_RealtimeMode_GetNameByTrophyGroup(s32 nId, char* pDst) {
    s32 i;
    for (i = 0; i < 118; i++) {
        if (nId == gRTEs.aEvent[i].nId) {
            strcpy(pDst, gRTEs.pNames + gRTEs.aEvent[i].nName);
        }
    }
}

// Event i's award id: its slot in SaveProfile.aRTEAward, and the unlock value (lock mode 0x11) of
// the items winning it gives (EventInfo.c).
s32 GM_RealtimeMode_GetTrophyID(s32 i) {
    return gRTEs.aEvent[i].nId;
}

// Whether profile nProfile has won event i's award (the day panel's COMPLETE / INCOMPLETE status).
u8 GameModeDriverRTE_IsEventComplete(s32 nProfile, s32 i) {
    SaveProfile* p = &gpSaveData[nProfile];
    return p->aRTEAward[gRTEs.aEvent[i].nId].bWon;
}
