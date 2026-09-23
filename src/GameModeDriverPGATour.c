// GameModeDriverPGATour.c (TW06's GameModeDriverPGATour): game mode 23, a PGA Tour season of 31
// tournaments (gPgaData, loaded from the 'PGA' stream objects), with the player's results kept in
// the save profile (TourSeason): "Did Not Play", "Cut", a finishing place, "Tied (%d players)".

#include "golfer.h"
#include "game.h"
#include "engine.h"

// One tournament of the season (0x64 bytes). TW06: GM_PgaTour_EventSlot_t, which has the name
// index first and the champion's name and score together further on.
typedef struct Tournament {
    s32  nName;                 // 0x00  offset into the names block. TW06: nameIdx
    s32  nTourEvent;            // 0x04  1-based entry in aTourEvent (0 = one round)
    u8   unk8[8];
    s32  n10;                   // 0x10
    char szChampName[0x10];     // 0x14  the champion before the season is played. TW06: champName
    s32  nChampScore;           // 0x24  TW06: champScore
    s16  aPrize[10][2];         // 0x28  per bracket (fn_800EF0E0): first prize and purse, in thousands
    u16  aStartDate[10];        // 0x50  per season (fn_800EFB88). TW06: startDate
} Tournament;

// One round of a tournament's format (0xC bytes).
typedef struct TourRound {
    s32  nCourse;               // 0x0
    u8   unk4[8];
} TourRound;

// A tournament's format (0x54 bytes). TW06: Tournament_events_t, which starts with nRounds too.
typedef struct TourEvent {
    s32  nRounds;               // 0x00
    TourRound aRound[6];        // 0x04
    u8   unk4C[0x54 - 0x4C];
} TourEvent;

typedef struct Triple {
    s32  n0;                    // 0x0
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} Triple;

// The tour's data, loaded from the 'PGA' stream objects. TW06: PGA_Master (GameModeDriverPGATour::m_PgaData).
typedef struct PgaData {
    Tournament aTournament[31]; // 0x0000  'PGAc'
    TourEvent  aTourEvent[31];  // 0x0C1C  'PGAt'
    u8         unk1648[0x6FC8 - 0x1648];
    Triple     aTriple[11];     // 0x6FC8  'PGAp'
    char*      pNames;          // 0x704C  'PGAn'. TW06: pStrTable
    u8         unk7050[4];
} PgaData;
extern PgaData gPgaData;

typedef struct Pga80205F30 {
    u8   b0;                    // 0x0
    u8   unk1[3];
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} Pga80205F30;
extern Pga80205F30 lbl_80205F30;
Pga80205F30* fn_800EE8B8(void);

// A tournament of the season in a save profile (TW06: PGATourSeason_EventData, the same layout).
typedef struct SeasonEvent {
    char szChampName[0x10];     // 0x00  the tournament's champion. TW06: champName
    s32  nChampScore;           // 0x10  TW06: champScore
    u16  nEventPar;             // 0x14  TW06: eventPar
    u16  nUserBracket;          // 0x16  the player's bracket when it was played. TW06: userBracket
    s32  nUserScore;            // 0x18  TW06: userScore
    s32  nUserRank;             // 0x1C  the player's finishing place. TW06: userRank
    s32  nUserRankType;         // 0x20  0 did not play, 1 missed the cut, 2 placed. TW06: eUserRankType
} SeasonEvent;

// The tour season in a save profile (TW06: PGATourSeason_t, which has 29 tournaments).
typedef struct TourSeason {
    s32  nSeason;               // 0x00  0 = 2004. TW06: season
    s32  nEvent;                // 0x04  the current tournament. TW06: eventID
    s32  nRound;                // 0x08  its round. TW06: round
    SeasonEvent aEvent[31];     // 0x0C
} TourSeason;

// A save profile (0x10600 bytes; the other files see gpSaveData as bytes).
typedef struct Profile {
    u8         unk0[0xB634];
    TourSeason tour;            // 0xB634
    u8         unkBA9C[0x104C8 - 0xBA9C];
    u16        n104C8;          // 0x104C8  counts the tournaments started
    u8         unk104CA[0x10600 - 0x104CA];
} Profile;
extern Profile* gpSaveData;

void fn_800EDEE8(void);
void fn_800EDF34(UStreamObject* pObject);
void fn_800EDF60(UStreamObject* pObject);
void fn_800EDF90(UStreamObject* pObject);
void fn_800EE064(void);
extern u8 lbl_8028233C;
s32 fn_800EE8B0(void);
void fn_80119934(int a);
void fn_800EF294(void);
s32 fn_800EF834(void);
s32 fn_800EFB88(void);
s32 fn_800EFBAC(void);
char* fn_800EFDFC(s32 i);
Tournament* fn_800EFA70(s32 i);
s32 fn_800EFE3C(s32 i);
char* fn_800EFE60(s32 i);
void fn_800F009C(void);
u8 fn_8011908C(s32, s32);
s32 fn_8011937C(s32, s32, u8);
void fn_800F018C(void);

extern s32 lbl_80281670;
extern s32 lbl_80282338;
s32  fn_801190D8(s32 a, s32 n);
s32  fn_800EFBD0(s32 i);
void fn_800D27CC(u16* pDate, s32 nDays);
void fn_800907AC(s32 nMoney, char* pDst);
void fn_80117C50(s32 a, s32 b);
void fn_800EF130(s32 a, u32 b);
void fn_800EEB94(s32 a);
void fn_80117860(TourSeason* pTour);
s32  fn_8008AC00(void);
void fn_8011A720(s32 a, s32 nHole);
void fn_8011A5F8(s32 a);
u8   fn_800EF720(s32 nPlayer);
s32  fn_8011A684(s32 a);
u8   fn_8011A6F4(s32 a, s32 b);
extern u8  gNumPlayersSetUp;
extern s32 lbl_80282340;
s32  fn_80119A04(s32 a, s32 b);
u8   fn_801197A4(s32 nPlayer, s32 b);
u8   fn_80117DE0(void);
void fn_80117B58(s32 a);
s32  fn_801191D0(s32 a, s32 b, s32 c);
void fn_800EEA3C(s32 nPlayer);
void fn_80117DF0(s32 nPlayer);
void fn_80117AF8(s32 nPlayer);
void fn_800EED0C(s32 nPlayer);
void fn_8011A538(s32 nPlayer);
void fn_80117D80(s32 nPlayer);
u8   fn_800EF83C(u16 nDate, s32* pId, s32* pRound);
s32  fn_8011A7C8(s32 nPlayer, s32 nHole);
s32  fn_80119588(s32 nPlayer, s32 a);
s32  fn_800E1904(s32 nPlayer, s32 a);
s32   fn_80118684(s32 a);
char* fn_80118E30(s32 a, s32 b);
s32   fn_80119118(s32 a, s32 b);
s32   fn_801197CC(s32 a, s32 b);
void  fn_800EDFC0(UStreamObject* pObject);
s32  fn_800F02A8(void);
s32  fn_800EFA9C(s32 i);

void fn_800EDE78(void) {
}

// TW06: GameModeDriverPGATour::RegisterStreamClients.
void fn_800EDE7C(void) {
    UStream_RegisterHandler('PGAc', fn_800EDF34);
    UStream_RegisterHandler('PGAt', fn_800EDF60);
    UStream_RegisterHandler('PGAp', fn_800EDF90);
    UStream_RegisterHandler('PGAn', fn_800EDFC0);
}

// TW06: GameModeDriverPGATour::UnregisterStreamClients.
void fn_800EDEE8(void) {
    UStream_UnregisterHandler('PGAc');
    UStream_UnregisterHandler('PGAt');
    UStream_UnregisterHandler('PGAp');
    UStream_UnregisterHandler('PGAn');
}

// TW06: GameModeDriverPGATour::LoadPGAcFromStream.
void fn_800EDF34(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(gPgaData.aTournament), gPgaData.aTournament);
}

// TW06: GameModeDriverPGATour::LoadPGAtFromStream.
void fn_800EDF60(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(gPgaData.aTourEvent), gPgaData.aTourEvent);
}

// TW06: GameModeDriverPGATour::LoadPGApFromStream.
void fn_800EDF90(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(gPgaData.aTriple), gPgaData.aTriple);
}

// TW06: GameModeDriverPGATour::Locale_PgaTourMode_LoadPGAnFromStream. The 'PGAn' object: the names
// block is copied out.
void fn_800EDFC0(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gPgaData.pNames = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gPgaData.pNames, pData, nSize);
        fn_80009E70(pObject);
    }
}

// The mode ends: one player back, and the options it changed come back.
void fn_800EE02C(void) {
    gpGame->nC = 1;
    gpGame->n10 = 1;
    SESSION_OPTIONS->unkC = lbl_80281670;
    SESSION_OPTIONS->nWind = lbl_80282338;
    lbl_8028233C = 0;
}

// The current tournament's number of rounds goes into the game state.
void fn_800EE064(void) {
    s32 nTourEvent = gPgaData.aTournament[gpSaveData->tour.nEvent].nTourEvent - 1;
    gpGame->nE0 = gPgaData.aTourEvent[nTourEvent].nRounds;
}

u8 fn_800EE470(void) {
    return lbl_8028233C;
}

// A round of the current tournament is over for profile 0: after the second round of a
// tournament of four or more the cut is checked, the round's score is kept, and after the last
// round the tournament ends.
void fn_800EE478(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 0) {
        gpSaveData[nPlayer].n104C8++;
    }
    if (fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent) >= 4 && gpSaveData[nPlayer].tour.nRound == 1) {
        fn_80117B58(0);
        if (fn_801197A4(0, 0)) {
            gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent].nUserRankType = 1;
        }
    }
    gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent].nUserScore = fn_801191D0(0, 0, 1);
    if (gpSaveData[nPlayer].tour.nRound + 1 >= fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent)) {
        fn_800EEA3C(0);
    }
}

// Whether the player trails the leader by more than one stroke.
u8 fn_800EE5B4(s32 nPlayer) {
    int bBehind;
    if (gpGame->bD4) {
        return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <
               fn_8011A7C8(nPlayer, Game_CurHoleIndex());
    }
    bBehind = 0;
    if (fn_800E1904(nPlayer, 0) >= fn_80119588(nPlayer, 1)) {
        if (fn_800E1904(nPlayer, 1) + 1 < fn_80119588(nPlayer, 1)) {
            bBehind = 1;
        }
    }
    return bBehind;
}

// TW06: GameModeDriverPGATour::IsPuttForWin. In a playoff, a putt for the lead; otherwise on the
// last round, a putt that would put the player ahead.
u8 fn_800EE6A0(s32 nPlayer) {
    s32 nRounds;
    int bWin;
    if (gpGame->bD4) {
        return fn_800EE5B4(nPlayer);
    }
    nRounds = fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent);
    bWin = 0;
    if (fn_8008AC00() == 1 && gpSaveData[nPlayer].tour.nRound + 1 >= nRounds) {
        if (fn_800E1904(nPlayer, 1) + 1 < fn_80119588(nPlayer, 1)) {
            bWin = 1;
        }
    }
    return bWin;
}

// TW06: GameModeDriverPGATour::GetCurrentLead. Strokes behind the leader (in a playoff, on this hole).
s32 fn_800EE778(s32 nPlayer) {
    if (gpGame->bD4) {
        return fn_8011A7C8(nPlayer, Game_CurHoleIndex()) - gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    }
    return fn_80119588(nPlayer, 1) - fn_800E1904(nPlayer, 0);
}

// TW06: GameModeDriverPGATour::GetPotentialLead.
s32 fn_800EE810(s32 nPlayer) {
    if (gpGame->bD4) {
        return fn_8011A7C8(nPlayer, Game_CurHoleIndex()) -
               (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1);
    }
    return fn_80119588(nPlayer, 1) - (fn_800E1904(nPlayer, 1) + 1);
}

s32 fn_800EE8B0(void) {
    return 3;
}

Pga80205F30* fn_800EE8B8(void) {
    return &lbl_80205F30;
}

// A round is over. The round count goes up and a player who missed the cut is out; after the last
// round the tournament ends.
void fn_800EEF88(s32 nPlayer) {
    if (fn_80117DE0()) {
        fn_80117DF0(nPlayer);
        if (gpSaveData[nPlayer].tour.nRound == 0) {
            fn_80117AF8(nPlayer);
        }
    } else {
        fn_800EED0C(nPlayer);
        fn_8011A538(nPlayer);
        fn_80117D80(nPlayer);
        fn_80117DF0(nPlayer);
        gpSaveData[nPlayer].tour.nRound++;
        if (fn_801197A4(nPlayer, 0)) {
            fn_800EF130(nPlayer, 0);
        }
        if (gpSaveData[nPlayer].tour.nRound >= fn_800EFA9C(gpSaveData[nPlayer].tour.nEvent)) {
            fn_800EEB94(nPlayer);
        }
    }
}

void fn_800EF094(s32 a, s32 n) {
    lbl_80205F30.b0 = 1;
    lbl_80205F30.n4 = fn_801190D8(a, 0);
    lbl_80205F30.n8 = n;
}

// The player's bracket, 0..9: tournaments won x 10 / 31 (profile 0's awards; nPlayer is not read).
s32 fn_800EF0E0(PlayerNumber_t nPlayer) {
    s32 n = fn_800F02A8() * 10 / 31;
    return n > 9 ? 9 : n;
}

void fn_800EF294(void) {
    fn_80119934(0);
}

// Whether the round is over for the player: no selected hole is left and it was the last round (in
// a playoff, after every hole).
u8 fn_800EF64C(s32 nPlayer) {
    s32 i;
    if (gpGame->bD4) {
        fn_8011A720(0, Game_CurHoleIndex());
        return fn_800EF720(nPlayer) == 0;
    }
    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return 0;
        }
    }
    if (gpGame->nDC + 1 >= gpGame->nE0) {
        fn_8011A5F8(0);
        return fn_800EF720(nPlayer) == 0;
    }
    return 1;
}

// A tie for the lead after the last round goes to a playoff: the scores are cleared and the playoff
// holes (16..18 of the course, looping) are set up. nPlayer is not read.
u8 fn_800EF720(s32 nPlayer) {
    u8 bPlayoff = 0;
    s32 i;
    int h;
    if (fn_8011A684(0) > 1 && fn_8011A6F4(0, 0)) {
        bPlayoff = 1;
    }
    if (bPlayoff) {
        gpGame->bD5 = 1;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                PLAYER(i)->nStrokes[h] = 0;
                PLAYER(i)->nModePoints[h] = 0;
            }
        }
        lbl_80282340++;
        if (lbl_80282340 > 17) {
            lbl_80282340 = 15;
        }
        fn_800E1260(0);
        fn_800E1404(lbl_80282340);
        gpGame->bD4 = 1;
        fn_800E45C0();
    }
    return bPlayoff;
}

s32 fn_800EF834(void) {
    return 31;
}

// TW06: GameModeDriverPGATour::GetEventByDate.
// Which tournament (and which of its rounds) is played on a date: each tournament starts on a
// date per season (aStartDate, seasons from 2004).
u8 fn_800EF83C(u16 nDate, s32* pId, s32* pRound) {
    s32 nMonth;
    s32 nDay;
    s32 i;
    s32 d;
    s32 nYear;
    s32 nSeason;
    u8 bFound;
    fn_800D2714(&nDate, &nMonth, &nDay, &nYear);
    bFound = 0;
    nSeason = nYear - 2004;
    if (nSeason >= 0 && nSeason < 10) {
        for (i = 0; i < 31; i++) {
            d = nDate - gPgaData.aTournament[i].aStartDate[nSeason];
            if (d >= 0 && d < fn_800EFA9C(i)) {
                *pId = i;
                bFound = 1;
                *pRound = d;
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

// TW06: GameModeDriverPGATour::GetSelectedEvent. The tournament profile 0 is on, and its round.
s32 fn_800EF908(s32* pRound) {
    PlayerNumber_t nPlayer = PLR_1_e;
    *pRound = gpSaveData[nPlayer].tour.nRound;
    return gpSaveData[nPlayer].tour.nEvent;
}

// TW06: GameModeDriverPGATour::GetNextEvent. The tournament after the current one.
s32 fn_800EF940(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return fn_800EFBD0(gpSaveData[nPlayer].tour.nEvent + 1);
}

// TW06: GameModeDriverPGATour::GetFinalEventOfSeason. The last tournament there is.
s32 fn_800EF984(void) {
    s32 nLast = 0;
    s32 i = fn_800EFBD0(1);
    while (i != -1) {
        nLast = i;
        i = fn_800EFBD0(i + 1);
    }
    return nLast;
}

// Skips ahead to tournament nEvent: a tournament under way is abandoned, the ones before are
// played out.
void fn_800EF9D0(s32 nEvent) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (nEvent != gpSaveData[nPlayer].tour.nEvent && gpSaveData[nPlayer].tour.nRound > 0) {
        fn_80117C50(0, 0);
    }
    while (gpSaveData[nPlayer].tour.nEvent < nEvent) {
        fn_800EF130(0, 0);
        fn_800EEB94(0);
    }
}

// Tournament i (0..30), or none.
Tournament* fn_800EFA70(s32 i) {
    if (i != -1 && i < 31) {
        return &gPgaData.aTournament[i];
    }
    return 0;
}

// TW06: GameModeDriverPGATour::GetRounds. Tournament i's number of rounds (from its format; 1 without one).
s32 fn_800EFA9C(s32 i) {
    if (gPgaData.aTournament[i].nTourEvent) {
        return gPgaData.aTourEvent[gPgaData.aTournament[i].nTourEvent - 1].nRounds;
    }
    return 1;
}

// The next season: 0 after the tenth, else 1 and the season starts at its first tournament.
s32 fn_800EFAD0(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    gpSaveData[nPlayer].tour.nSeason++;
    if (gpSaveData[nPlayer].tour.nSeason >= 10) {
        gpSaveData[nPlayer].tour.nSeason = 10;
        gpSaveData[nPlayer].tour.nEvent = 0;
        return 0;
    }
    gpSaveData[nPlayer].tour.nEvent = fn_800EFBD0(0);
    fn_80117860(&gpSaveData[nPlayer].tour);
    return 1;
}

// TW06: GameModeDriverPGATour::GetCurrentSeason. Profile 0's season, 0 = 2004.
s32 fn_800EFB88(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nSeason;
}

s32 fn_800EFBAC(void) {
    return fn_800EFB88() + 2004;
}

// TW06: GameModeDriverPGATour::GetEventOnOrAfter. The first tournament from i on that is held this
// season, or -1.
s32 fn_800EFBD0(s32 i) {
    PlayerNumber_t nPlayer = PLR_1_e;
    s32 nEvent;
    u8 bFound = 0;
    while (!bFound) {
        Tournament* p = fn_800EFA70(i);
        if (p != NULL) {
            if (p->aStartDate[gpSaveData[nPlayer].tour.nSeason] != 0) {
                nEvent = i;
                bFound = 1;
            }
        } else {
            nEvent = -1;
            bFound = 1;
        }
        i++;
    }
    return nEvent;
}

// The tournament being played on a date.
Tournament* fn_800EFC80(u16 nDate) {
    s32 nId;
    s32 nRound;
    if (fn_800EF83C(nDate, &nId, &nRound)) {
        return fn_800EFA70(nId);
    }
    return 0;
}

// Tournament i's first prize in bracket k. TW06: GameModeDriverPGATour::ComputeFirstPrizeForBracket
// (by shape).
s32 fn_800EFCC0(s32 i, s32 k) {
    Tournament* p = fn_800EFA70(i);
    return p->aPrize[k][0] * 1000;
}

// Tournament i's purse in bracket k. TW06: GameModeDriverPGATour::ComputePurseForBracket (by shape).
s32 fn_800EFCFC(s32 i, s32 k) {
    Tournament* p = fn_800EFA70(i);
    return p->aPrize[k][1] * 1000;
}

u16 fn_800EFD38(s32 i) {
    Tournament* p = fn_800EFA70(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aStartDate[fn_800EFB88()];
}

// TW06: GameModeDriverPGATour::GetEndDate. The last day of tournament i this season.
u16 fn_800EFD84(s32 i) {
    u16 nDate;
    Tournament* p = fn_800EFA70(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    nDate = p->aStartDate[fn_800EFB88()];
    fn_800D27CC(&nDate, fn_800EFA9C(i) - 1);
    return nDate;
}

// TW06: GameModeDriverPGATour::GetName.
char* fn_800EFDFC(s32 i) {
    return gPgaData.pNames + gPgaData.aTournament[i].nName;
}

// TW06: GameModeDriverPGATour::GetCurrentEventID. Profile 0's current tournament.
s32 fn_800EFE18(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nEvent;
}

s32 fn_800EFE3C(s32 i) {
    return fn_800EFA70(i)->n10;
}

// TW06: GameModeDriverPGATour::GetInitialChampName.
char* fn_800EFE60(s32 i) {
    return gPgaData.aTournament[i].szChampName;
}

// TW06: GameModeDriverPGATour::GetInitialChampScore.
s32 fn_800EFE78(s32 i) {
    return gPgaData.aTournament[i].nChampScore;
}

// TW06: GameModeDriverPGATour::GetCourses. The course of each round of a tournament; returns the
// number of rounds.
s32 fn_800EFE90(Tournament* p, s32* pCourses) {
    TourEvent* pEvent = &gPgaData.aTourEvent[p->nTourEvent - 1];
    s32 nRounds;
    s32 i;
    TourRound* pRound;
    if (pEvent != NULL) {
        nRounds = pEvent->nRounds;
        pRound = pEvent->aRound;
        for (i = 0; i < nRounds; i++) {
            pCourses[i] = pRound[i].nCourse;
        }
        return nRounds;
    }
    return 0;
}

// TW06: GameModeDriverPGATour::GetWinnerEarningsString. Tournament i's first prize as text: in the
// player's bracket when it was played, else in the current one.
void fn_800EFF7C(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < fn_800EFE18()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = fn_800EF0E0(nPlayer);
    }
    fn_800907AC(fn_800EFCC0(i, nBracket), pDst);
}

// TW06: GameModeDriverPGATour::GetCurrentEventLeader. The leader's name, or "Tied (%d players)".
void fn_800F0010(char* pDst) {
    s32 n = fn_80118684(0);
    if (n > 1) {
        sprintf(pDst, "Tied (%d players)", n);
    } else {
        s32 nLeader = fn_801197CC(0, 0);
        s32 nGolfer = fn_80119118(0, nLeader);
        strcpy(pDst, fn_80118E30(0, nGolfer));
    }
}

void fn_800F009C(void) {
    s32 nLeader = fn_801197CC(0, 0);
    fn_8011937C(0, nLeader, fn_8011908C(0, nLeader) == 0);
}

// TW06: GameModeDriverPGATour::GetPurseString. The same for the purse.
void fn_800F00F8(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < fn_800EFE18()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = fn_800EF0E0(nPlayer);
    }
    fn_800907AC(fn_800EFCFC(i, nBracket), pDst);
}

void fn_800F018C(void) {
    fn_8011937C(0, 0, fn_8011908C(0, 0) == 0);
}

// TW06: GameModeDriverPGATour::GetUserFinishString. A tournament's result for the season screen:
// "Did Not Play", "Cut", or the place.
void fn_800F01CC(s32 i, char* pDst) {
    switch (gpSaveData->tour.aEvent[i].nUserRankType) {
    case 0:
        strcpy(pDst, "Did Not Play");
        return;
    case 1:
        strcpy(pDst, "Cut");
        return;
    case 2:
        sprintf(pDst, "%d", gpSaveData->tour.aEvent[i].nUserRank);
        return;
    }
}

// TW06: GameModeDriverPGATour::GetChamp.
void fn_800F0258(s32 i, char* pDst) {
    strcpy(pDst, gpSaveData->tour.aEvent[i].szChampName);
}

// TW06: GameModeDriverPGATour::GetChampScore.
s32 fn_800F0290(s32 i) {
    return gpSaveData->tour.aEvent[i].nChampScore;
}

// How many tournaments are won (flag 1 in the profile's list at +0xC8, 8 bytes each).
s32 fn_800F02A8(void) {
    s32 n = 0;
    s32 i;
    for (i = 0; i < 31; i++) {
        if (((u8*)gpSaveData)[0xC8 + i * 8] == 1) {
            n++;
        }
    }
    return n;
}

s32 fn_800F02D4(s32 i) {
    return gPgaData.aTriple[i].n0;
}

s32 fn_800F02EC(s32 i) {
    return gPgaData.aTriple[i].n4;
}

s32 fn_800F0304(s32 i) {
    return gPgaData.aTriple[i].n8;
}

// The message after a round, if there is one: after the second round whether the player made the
// cut, and in a playoff the score to beat.
s32 fn_800F031C(char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 1 && fn_80119A04(0, 0) == 18) {
        if (fn_801197A4(0, 0)) {
            strcpy(pDst, "TOURNAMENT CUT\n\nYou did not place in the top 70 after two\n"
                         "rounds. You have been cut from the tournament.");
            return 1;
        }
        strcpy(pDst, "TOURNAMENT CUT\n\nCongratulations! You placed in the top 70\n"
                     "after two rounds. You made the cut!");
        return 1;
    }
    if (gpGame->bD4 && fn_8011A6F4(0, 0) && fn_8011A684(0) > 1) {
        sprintf(pDst, "TOURNAMENT PLAYOFF\n\nYou're tied for first place. You must beat\n"
                      "your opponent's score of %d on the playoff\nhole to win.",
                fn_8011A7C8(0, lbl_80282340));
        return 1;
    }
    return 0;
}

s32 fn_800F0428(s32 nPlayer) {
    return gpSaveData[nPlayer].tour.nEvent;
}
