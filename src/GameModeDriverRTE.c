// GameModeDriverRTE.c (TW06's GameModeDriverRTE): game mode 24, real-time events on the calendar
// ('RTEc'/'RTEs'/'RTEn' stream objects): 118 dated entries, each starting one of 111 challenges (the
// mode 5 code in GameMode5.c runs them) on its date, by the console's clock.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/rte.h"

// This file's globals, defined last address first (an object's .bss is laid out in reverse).
s32 lbl_80281680 = 4;                       // gSession.options.nC saved while an event runs
void (*lbl_8028235C)(void);                 // mode 5's pfnShutdown, called from ours (fn_800F0678)
void (*lbl_80282358)(void);                 // mode 5's pfnEndGame, called from ours (fn_800F0BBC)
s32 lbl_80282354;                           // the event's round
s32 lbl_80282350;                           // the event (gRTEs.aEvent index)
u8  lbl_8028234C;                           // 1 while an event runs
s32 lbl_80282348;                           // gSession.options.nWind saved while an event runs
RTEData gRTEs;

void GameModeDriverRTE_UnregisterStreamClients(void);
void GameModeDriverRTE_Locale_LoadRTEcFromStream(UStreamObject* pObject);
void GameModeDriverRTE_LoadRTEsFromStream(UStreamObject* pObject);
s32 fn_800F0E18(s32 i);
s32 fn_800F0E20(s32* pRound);
void fn_800F0E30(s32 nId, s32 nRound);
s32 fn_800F1008(s32 i);
s32 fn_800F102C(void);
void  GameModeDriverRTE_Locale_LoadRTEnFromStream(UStreamObject* pObject);
void  fn_800F0678(void);
void  fn_800F0BBC(void);
s32   fn_800F0820(void);
u8    GameModeDriverRTE_GetEventByDate(u16 nDate, s32* pId, s32* pRound);
u8    fn_800F0DB8(s32 nMonth, s32 nDay, s32 nYear, s32* pId, s32* pRound);
s32   fn_800F0F54(void);

// Mode 24 starts: match-play callbacks (GameModeMatch) around the event's own start and end.
void fn_800F0448(void) {
    gpGame->pfnInit = fn_800F0448;
    gpGame->pfnShutdown = fn_800F0678;
    gpGame->pfnSetupNextGolfer = fn_800E9F14;
    gpGame->pfnGetHonors = GameModeMatch_GetHonors;
    gpGame->pfnHoleFinished = GameModeMatch_HoleFinished;
    gpGame->pfnGameFinished = GameModeMatch_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeMatch_GoToPlayoff;
    gpGame->pfnEndHole = GameModeMatch_EndHole;
    gpGame->pfnEndGame = fn_800F0BBC;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

void GameModeDriverRTE_RegisterStreamClients(void) {
    UStream_RegisterHandler('RTEc', GameModeDriverRTE_Locale_LoadRTEcFromStream);
    UStream_RegisterHandler('RTEs', GameModeDriverRTE_LoadRTEsFromStream);
    UStream_RegisterHandler('RTEn', GameModeDriverRTE_Locale_LoadRTEnFromStream);
}

void GameModeDriverRTE_UnregisterStreamClients(void) {
    UStream_UnregisterHandler('RTEc');
    UStream_UnregisterHandler('RTEs');
    UStream_UnregisterHandler('RTEn');
}

void GameModeDriverRTE_Locale_LoadRTEcFromStream(UStreamObject* pObject) {
    // port: the 'RTEc' object is copied straight into gRTEs.aEvent (RTEvent[118]); it is big-endian
    // on disc, so a little-endian port converts it field by field here (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(gRTEs.aEvent), gRTEs.aEvent);
}

void GameModeDriverRTE_LoadRTEsFromStream(UStreamObject* pObject) {
    // port: the 'RTEs' object is copied straight into gRTEs.aChallenge (Challenge[111]); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(gRTEs.aChallenge), gRTEs.aChallenge);
}

// The 'RTEn' object: the names block is copied out.
void GameModeDriverRTE_Locale_LoadRTEnFromStream(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gRTEs.pNames = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gRTEs.pNames, pData, nSize);
        fn_80009E70(pObject);
    }
}

// The mode ends: the challenge's cleanup, and the options it changed come back.
void fn_800F0678(void) {
    if (lbl_8028235C) {
        lbl_8028235C();
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nC = lbl_80281680;
    gSession.options.nWind = lbl_80282348;
    lbl_8028234C = 0;
}

// Starts today's event: the options are saved (wind off), and
// its challenge runs in mode 5 with this file's start and end wrapped around it.
void GameModeDriverRTE_StartEvent(void) {
    lbl_80281680 = gSession.options.nC;
    lbl_80282348 = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
    lbl_8028234C = 1;
    if (gRTEs.aEvent[lbl_80282350].bOff == 0) {
        if (gRTEs.aEvent[lbl_80282350].nChallenge != 0) {
            gSession.nNumPlayers = 1;
            fn_800E0B38(5);
            fn_800EC544(gRTEs.aChallenge, 111);
            fn_800EAE38(gRTEs.aEvent[lbl_80282350].nChallenge - 1);
            fn_800EAF7C();
            lbl_8028235C = gpGame->pfnShutdown;
            lbl_80282358 = gpGame->pfnEndGame;
            gpGame->pfnShutdown = fn_800F0678;
            gpGame->pfnEndGame = fn_800F0BBC;
        }
    }
}

// The next challenge of the event starts.
void fn_800F07C8(void) {
    fn_800EAF7C();
    lbl_8028235C = gpGame->pfnShutdown;
    lbl_80282358 = gpGame->pfnEndGame;
    gpGame->pfnShutdown = fn_800F0678;
    gpGame->pfnEndGame = fn_800F0BBC;
}

u8 fn_800F0818(void) {
    return lbl_8028234C;
}

// How many events profile 0 has won.
s32 fn_800F0820(void) {
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

// The message after an event (before its award is marked won): the first win (0), or one for the
// event, else one of four at random.
void fn_800F08A8(void) {
    u8 bFirst = 0;
    u8 bSaid;
    if (!fn_800F0820()) {
        fn_800E4364(10, 0, 0, 0);
        bFirst = 1;
    }
    bSaid = 1;
    switch (lbl_80282350) {
    case 0x0:
        fn_800E4364(10, 0xD, 0, 0);
        break;
    case 0x2:
    case 0x6:
        fn_800E4364(10, 0xE, 0, 0);
        break;
    case 0x7:
        fn_800E4364(10, 0xF, 0, 0);
        break;
    case 0x12:
    case 0xE:
        fn_800E4364(10, 0x10, 0, 0);
        break;
    case 0x16:
        fn_800E4364(10, 0x11, 0, 0);
        break;
    case 0x1F:
        fn_800E4364(10, 0x12, 0, 0);
        break;
    case 0x41:
        fn_800E4364(10, 0x13, 0, 0);
        break;
    case 0x55:
        fn_800E4364(10, 0x14, 0, 0);
        break;
    case 0x5D:
        fn_800E4364(10, 0x15, 0, 0);
        break;
    case 0x6D:
        fn_800E4364(10, 0x16, 0, 0);
        break;
    case 0x6E:
        fn_800E4364(10, 0x17, 0, 0);
        break;
    case 0x6F:
        fn_800E4364(10, 0x18, 0, 0);
        break;
    case 0x56:
    case 0x17:
        fn_800E4364(10, 0x19, 0, 0);
        break;
    case 0x18:
    case 0x1E:
        fn_800E4364(10, 5, 0, 0);
        break;
    default:
        bSaid = 0;
        break;
    }
    if (!bFirst && !bSaid) {
        switch (Rand_Next(0) & 3) {
        case 0:
            fn_800E4364(10, 2, 0, 0);
            return;
        case 1:
            fn_800E4364(10, 3, 0, 0);
            return;
        case 2:
            fn_800E4364(10, 4, 0, 0);
            return;
        default:
            fn_800E4364(10, 7, 0, 0);
            break;
        }
    }
}

// The event is over: with a medal, its reward, the messages, and the event's flag in the profile.
void fn_800F0BBC(void) {
    s32 nReward;
    lbl_80282358();
    if (fn_800EC558() != 3) {
        nReward = gRTEs.aChallenge[gRTEs.aEvent[lbl_80282350].nChallenge - 1].aMedal[0].nReward;
        fn_800D3548(0, nReward, 0);
        fn_800E4364(0, 0x6F, nReward, 0);
        fn_800F08A8();
        fn_800D7770(0, &gpSaveData->aRTEAward[gRTEs.aEvent[lbl_80282350].nId]);
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

// The event held on a date (and which of its days).
u8 GameModeDriverRTE_GetEventByDate(u16 nDate, s32* pId, s32* pRound) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 i;
    s32 d;
    s32 nSeason;
    u8 bFound;
    fn_800D2714(&nDate, &nMonth, &nDay, &nYear);
    bFound = 0;
    nSeason = nYear - 2003;
    // EA bug: nSeason is not checked against the ten seasons (GameModeDriverRTE_GetNextEvent checks
    // it), so a date outside 2003..2012 reads past aDate.
    for (i = 0; i < 118; i++) {
        if (gRTEs.aEvent[i].aDate[nSeason] != 0) {
            d = nDate - gRTEs.aEvent[i].aDate[nSeason];
            if (d >= 0 && d < fn_800F0E18(i)) {
                *pId = i;
                bFound = 1;
                *pRound = d + 1;
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

u8 fn_800F0DB8(s32 nMonth, s32 nDay, s32 nYear, s32* pId, s32* pRound) {
    u16 nDate;
    fn_800D2678(&nDate, nMonth, nDay, nYear);
    return GameModeDriverRTE_GetEventByDate(nDate, pId, pRound);
}

// How many days event i lasts: always one.
s32 fn_800F0E18(s32 i) {
    return 1;
}

// The current event, and its day.
s32 fn_800F0E20(s32* pRound) {
    *pRound = lbl_80282354;
    return lbl_80282350;
}

// Event nId, on its day nRound, becomes the current one.
void fn_800F0E30(s32 nId, s32 nRound) {
    lbl_80282354 = nRound;
    lbl_80282350 = nId;
}

// Today's event becomes the current one.
s32 fn_800F0E3C(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nId;
    s32 nRound;
    GameModeDriverRTE_GetCurrentDate(&nMonth, &nDay, &nYear);
    if (fn_800F0DB8(nMonth, nDay, nYear, &nId, &nRound)) {
        fn_800F0E30(nId, nRound);
        return 1;
    }
    return 0;
}

RTEvent* GameModeDriverRTE_GetCalData(s32 i) {
    return &gRTEs.aEvent[i];
}

RTEvent* fn_800F0EB4(u16 nDate) {
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

// The reward for event i.
s32 fn_800F0F30(s32 i) {
    return gRTEs.aChallenge[gRTEs.aEvent[i].nChallenge - 1].aMedal[0].nReward;
}

// This year's season (0..9 from 2003), or 0.
s32 fn_800F0F54(void) {
    s32 nYear;
    s32 n;
    s32 bOk;
    s32 nSeason;
    fn_8011E020(&n, &n, &nYear, &n, &n, &n, &n);
    nSeason = nYear - 2003;
    bOk = 0;
    if (nSeason >= 0 && nSeason < 10) {
        bOk = 1;
    }
    return bOk ? nSeason : 0;
}

u16 fn_800F0FBC(s32 i) {
    RTEvent* p = GameModeDriverRTE_GetCalData(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aDate[fn_800F0F54()];
}

s32 fn_800F1008(s32 i) {
    return GameModeDriverRTE_GetCalData(i)->n14;
}

s32 fn_800F102C(void) {
    return 0;
}

// The next event from today (-1 if none this season).
s32 GameModeDriverRTE_GetNextEvent(void) {
    s32 nNext;
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u16 nToday;
    s32 nSeason;
    s32 nBest = -1;
    s32 i;
    s32 d;
    u8 bFound;
    GameModeDriverRTE_GetCurrentDate(&nMonth, &nDay, &nYear);
    nSeason = nYear - 2003;
    fn_800D2678(&nToday, nMonth, nDay, nYear);
    if (nSeason >= 0 && nSeason < 10) {
        bFound = 0;
        for (i = 0; i < 118; i++) {
            if (gRTEs.aEvent[i].aDate[nSeason] != 0) {
                d = gRTEs.aEvent[i].aDate[nSeason] - nToday;
                if (d >= 0 && (nBest == -1 || d < nBest)) {
                    nNext = i;
                    nBest = d;
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

// An event's n14, by its id (0 if none).
s32 fn_800F1154(s32 nId) {
    s32 i;
    for (i = 0; i < 118; i++) {
        if (nId == gRTEs.aEvent[i].nId) {
            return gRTEs.aEvent[i].n14;
        }
    }
    return 0;
}

// An event's name, by its id.
void fn_800F11A0(s32 nId, char* pDst) {
    s32 i;
    for (i = 0; i < 118; i++) {
        if (nId == gRTEs.aEvent[i].nId) {
            strcpy(pDst, gRTEs.pNames + gRTEs.aEvent[i].nName);
        }
    }
}

s32 fn_800F120C(s32 i) {
    return gRTEs.aEvent[i].nId;
}

// Whether a profile has done event i.
u8 GameModeDriverRTE_IsEventComplete(s32 nProfile, s32 i) {
    SaveProfile* p = &gpSaveData[nProfile];
    return p->aRTEAward[gRTEs.aEvent[i].nId].bWon;
}
