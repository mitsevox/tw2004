// GameModeDriverRTE.c (TW06's GameModeDriverRTE): game mode 24, real-time events on the calendar
// ('RTEc'/'RTEs'/'RTEn' stream objects): 118 dated entries, each starting one of 111 challenges (the
// mode 5 code in GameMode5.c runs them) on its date, by the console's clock.

#include "golfer.h"
#include "game.h"
#include "engine.h"

void fn_800F0570(void);
void fn_800F05B0(UStreamObject* pObject);
void fn_800F05DC(UStreamObject* pObject);
extern u8 lbl_8028234C;
s32 fn_800F0E18(void);
extern s32 lbl_80282350;
extern s32 lbl_80282354;
s32 fn_800F0E20(u8* p0);
void fn_800F0E30(s32 p0, s32 p1);
s32 fn_800F1008(s32 i);
s32 fn_800F102C(void);

// An award in a save profile (TW06: AwardInfoBase).
typedef struct AwardInfo {
    u8   bWon;                  // 0x0  TW06: bWon
    u8   unk1;
    u16  nDateWon;              // 0x2  TW06: dateWon
} AwardInfo;

// The start of a save profile as this file reads it (the profiles are 0x10600 bytes apart).
typedef struct RTESave {
    u8        unk0[0x20C];
    AwardInfo aAward[75];       // 0x20C  per event id. TW06: rteEventAwardInfo (RealTimeEventUserData)
} RTESave;

// One calendar event (0x30 bytes). TW06: CalendarEntry_t, which also starts with the name and
// description and ends with the start dates.
typedef struct RTEvent {
    s32 nName;                  // 0x00  offset into the names block. TW06: nameIdx
    s32 nDesc;                  // 0x04  the same for its description. TW06: descIdx
    s32 bOff;                   // 0x08  nonzero: not playable
    s32 nChallenge;             // 0x0C  1-based, in aChallenge
    s32 nId;                    // 0x10  the event's id (its flag in the save profile)
    s32 n14;                    // 0x14
    u8  unk18[4];
    u16 aDate[10];              // 0x1C  the start date per season (from 2003; 0 = not held). TW06: startDate
} RTEvent;

// A challenge as this file reads it: GameMode5.c's Challenge (0x80 bytes), which runs it.
typedef struct RTEChallenge {
    u8  unk0[0x64];
    s32 n64;                    // 0x64  the best medal's reward
    u8  unk68[0x80 - 0x68];
} RTEChallenge;

// The calendar's data, loaded from the 'RTE' stream objects. TW06: RTEvents
// (GameModeDriverRTE::m_RTEs), which holds pointers to the same three blocks.
typedef struct RTEData {
    RTEvent      aEvent[118];       // 0x0000  'RTEc'. TW06: pCalendar
    RTEChallenge aChallenge[111];   // 0x1620  'RTEs'. TW06: pScenarios
    char*        pNames;            // 0x4DA0  'RTEn'. TW06: pStrTable
    u8           unk4DA4[4];
} RTEData;
extern RTEData gRTEs;

void  GM_vCloseModuleONCE(void);
void  fn_800EAA40(void);
void  fn_800E0B38(int nMode);
void  fn_800EC544(RTEChallenge* pList, s32 n);   // Challenge* in GameMode5.c
void  fn_800EAE38(s32 i);
void  fn_800EAF7C(void);
int   fn_800EC558(void);
void  fn_800D7770(int nPlayer, u8* pFlag);
void  fn_8011E020(s32* pYear, s32* pMonth, s32* pDay, s32* a, s32* b, s32* c, s32* d);
void  fn_800D2714(u16* pDate, s32* pDay, s32* pMonth, s32* pYear);
void  fn_800D2678(u16* pDate, s32 nYear, s32 nMonth, s32 nDay);
extern void (*lbl_8028235C)(void);
extern void (*lbl_80282358)(void);
extern s32 lbl_80281680;
extern s32 lbl_80282348;
extern u8* gpSaveData;
void  fn_800F05DC(UStreamObject* pObject);
void  fn_800F060C(UStreamObject* pObject);
void  fn_800F0678(void);
void  fn_800F0BBC(void);
s32   fn_800F0820(void);
u8    fn_800F0CEC(u16 nDate, s32* pId, s32* pRound);
u8    fn_800F0DB8(s32 nYear, s32 nMonth, s32 nDay, s32* pId, s32* pRound);
s32   fn_800F0F54(void);

// Mode 24 starts: match-play callbacks (GameModeMatch) around the event's own start and end.
void fn_800F0448(void) {
    gpGame->pfn1C8 = fn_800F0448;
    gpGame->pfn1CC = fn_800F0678;
    gpGame->pfn1D0 = fn_800E9F14;
    gpGame->pfn1D4 = fn_800EA084;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800EA278;
    gpGame->pfn1DC = (u8 (*)(int))fn_800EA548;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800EA758;
    gpGame->pfn1E8 = fn_800EAA40;
    gpGame->pfn1F4 = fn_800F0BBC;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// TW06: GameModeDriverRTE::RegisterStreamClients.
void fn_800F0518(void) {
    UStream_RegisterHandler('RTEc', fn_800F05B0);
    UStream_RegisterHandler('RTEs', fn_800F05DC);
    UStream_RegisterHandler('RTEn', fn_800F060C);
}

// TW06: GameModeDriverRTE::UnregisterStreamClients.
void fn_800F0570(void) {
    UStream_UnregisterHandler('RTEc');
    UStream_UnregisterHandler('RTEs');
    UStream_UnregisterHandler('RTEn');
}

// TW06: GameModeDriverRTE::Locale_LoadRTEcFromStream.
void fn_800F05B0(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(gRTEs.aEvent), gRTEs.aEvent);
}

// TW06: GameModeDriverRTE::LoadRTEsFromStream.
void fn_800F05DC(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(gRTEs.aChallenge), gRTEs.aChallenge);
}

// TW06: GameModeDriverRTE::Locale_LoadRTEnFromStream. The 'RTEn' object: the names block is copied out.
void fn_800F060C(UStreamObject* pObject) {
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
    SESSION_OPTIONS->unkC = lbl_80281680;
    SESSION_OPTIONS->nWind = lbl_80282348;
    lbl_8028234C = 0;
}

// TW06: GameModeDriverRTE::StartEvent. Starts today's event: the options are saved (wind off), and
// its challenge runs in mode 5 with this file's start and end wrapped around it.
void fn_800F06DC(void) {
    lbl_8028234C = 1;
    lbl_80281680 = SESSION_OPTIONS->unkC;
    lbl_80282348 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
    if (gRTEs.aEvent[lbl_80282350].bOff == 0) {
        if (gRTEs.aEvent[lbl_80282350].nChallenge != 0) {
            gSession.nNumPlayers = 1;
            fn_800E0B38(5);
            fn_800EC544(gRTEs.aChallenge, 111);
            fn_800EAE38(gRTEs.aEvent[lbl_80282350].nChallenge - 1);
            fn_800EAF7C();
            lbl_8028235C = gpGame->pfn1CC;
            lbl_80282358 = gpGame->pfn1F4;
            gpGame->pfn1CC = fn_800F0678;
            gpGame->pfn1F4 = fn_800F0BBC;
        }
    }
}

// The next challenge of the event starts.
void fn_800F07C8(void) {
    fn_800EAF7C();
    lbl_8028235C = gpGame->pfn1CC;
    lbl_80282358 = gpGame->pfn1F4;
    gpGame->pfn1CC = fn_800F0678;
    gpGame->pfn1F4 = fn_800F0BBC;
}

u8 fn_800F0818(void) {
    return lbl_8028234C;
}

// How many events profile 0 has won.
s32 fn_800F0820(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    RTESave* p = (RTESave*)(gpSaveData + nPlayer * 0x10600);
    s32 n = 0;
    s32 i;
    for (i = 0; i < 75; i++) {
        if (p->aAward[i].bWon == 1) {
            n++;
        }
    }
    return n;
}

// The message after an event: every event done (0), or one for the event, else one of four at
// random.
void fn_800F08A8(void) {
    u8 bAll = 0;
    u8 bSaid;
    if (!fn_800F0820()) {
        fn_800E4364(10, 0, 0, 0);
        bAll = 1;
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
    if (!bAll && !bSaid) {
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
        nReward = gRTEs.aChallenge[gRTEs.aEvent[lbl_80282350].nChallenge - 1].n64;
        fn_800D3548(0, nReward, 0);
        fn_800E4364(0, 0x6F, nReward, 0);
        fn_800F08A8();
        fn_800D7770(0, gpSaveData + gRTEs.aEvent[lbl_80282350].nId * 4 + 0x20C);
    }
}

// TW06: GameModeDriverRTE::GetCurrentDate. Today's date from the clock.
void fn_800F0C74(s32* pYear, s32* pMonth, s32* pDay) {
    s32 nYear;
    s32 nMonth;
    s32 nDay;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    fn_8011E020(&nYear, &nMonth, &nDay, &a, &b, &c, &d);
    *pYear = nYear;
    *pMonth = nMonth;
    *pDay = nDay;
}

// TW06: GameModeDriverRTE::GetEventByDate. The event held on a date (and which of its days).
u8 fn_800F0CEC(u16 nDate, s32* pId, s32* pRound) {
    s32 nDay;
    s32 nMonth;
    s32 nYear;
    u8 bFound;
    s32 i;
    s32 d;
    fn_800D2714(&nDate, &nDay, &nMonth, &nYear);
    bFound = 0;
    for (i = 0; i < 118; i++) {
        if (gRTEs.aEvent[i].aDate[nYear - 2003] != 0) {
            d = nDate - gRTEs.aEvent[i].aDate[nYear - 2003];
            if (d >= 0 && d < fn_800F0E18()) {
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

u8 fn_800F0DB8(s32 nYear, s32 nMonth, s32 nDay, s32* pId, s32* pRound) {
    u16 nDate;
    fn_800D2678(&nDate, nYear, nMonth, nDay);
    return fn_800F0CEC(nDate, pId, pRound);
}

s32 fn_800F0E18(void) {
    return 1;
}

s32 fn_800F0E20(u8* p0) {
    *(s32*)p0 = lbl_80282354;
    return lbl_80282350;
}

void fn_800F0E30(s32 p0, s32 p1) {
    lbl_80282354 = p1;
    lbl_80282350 = p0;
}

// Today's event becomes the current one.
s32 fn_800F0E3C(void) {
    s32 nYear;
    s32 nMonth;
    s32 nDay;
    s32 nId;
    s32 nRound;
    fn_800F0C74(&nYear, &nMonth, &nDay);
    if (fn_800F0DB8(nYear, nMonth, nDay, &nId, &nRound)) {
        fn_800F0E30(nId, nRound);
        return 1;
    }
    return 0;
}

// TW06: GameModeDriverRTE::GetCalData.
RTEvent* fn_800F0EA0(s32 i) {
    return &gRTEs.aEvent[i];
}

RTEvent* fn_800F0EB4(u16 nDate) {
    s32 nId;
    s32 nRound;
    if (fn_800F0CEC(nDate, &nId, &nRound)) {
        return fn_800F0EA0(nId);
    }
    return 0;
}

// TW06: GameModeDriverRTE::GetName.
char* fn_800F0EF4(s32 i) {
    return gRTEs.pNames + gRTEs.aEvent[i].nName;
}

// TW06: GameModeDriverRTE::GetDescription.
char* fn_800F0F10(s32 i) {
    return gRTEs.pNames + gRTEs.aEvent[i].nDesc;
}

// The reward for event i.
s32 fn_800F0F30(s32 i) {
    return gRTEs.aChallenge[gRTEs.aEvent[i].nChallenge - 1].n64;
}

// This year's season (0..9 from 2003), or 0.
s32 fn_800F0F54(void) {
    s32 nYear;
    s32 n;
    s32 bOk;
    s32 nSeason;
    fn_8011E020(&n, &n, &nYear, &n, &n, &n, &n);
    bOk = 0;
    nSeason = nYear - 2003;
    if (nSeason >= 0 && nSeason < 10) {
        bOk = 1;
    }
    return bOk ? nSeason : 0;
}

u16 fn_800F0FBC(s32 i) {
    RTEvent* p = fn_800F0EA0(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aDate[fn_800F0F54()];
}

s32 fn_800F1008(s32 i) {
    return fn_800F0EA0(i)->n14;
}

s32 fn_800F102C(void) {
    return 0;
}

// TW06: GameModeDriverRTE::GetNextEvent. The next event from today (-1 if none this season).
s32 fn_800F1034(void) {
    s32 nBest = -1;
    s32 nYear;
    s32 nMonth;
    s32 nDay;
    u16 nToday;
    s32 nSeason;
    s32 nNext;
    u8 bFound;
    s32 i;
    s32 d;
    fn_800F0C74(&nYear, &nMonth, &nDay);
    nSeason = nDay - 2003;
    fn_800D2678(&nToday, nYear, nMonth, nDay);
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

// TW06: GameModeDriverRTE::IsEventComplete. Whether a profile has done event i.
u8 fn_800F1224(s32 nProfile, s32 i) {
    return gpSaveData[nProfile * 0x10600 + 0x20C + gRTEs.aEvent[i].nId * 4];
}
