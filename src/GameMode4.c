// GameMode4.c (our name): game mode 4, the matches of the 25-event ladder loaded from the 'TCM '
// stream (lbl_802124B8, 0x44 bytes an event; names from 'TCMS'). An event is a one-on-one match
// against a pro (mode 4, match play with GameModeMatch's callbacks) or a mode 5 challenge. Winning
// one sets its flag in the save profile, pays its prize and unlocks the pro and a reward.

#include "golfer.h"
#include "game.h"
#include "engine.h"

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
extern LadderEvent lbl_802124B8[25];

// The 'TCMS' text: the events' names.
typedef struct LadderNames {
    char* pText;
    u32   uSize;
} LadderNames;
extern LadderNames lbl_8028243C;

// The prize table (stream 'ERN '); only what this file reads.
typedef struct MatchPrize {
    s32 nBase;                  // for the win
    s32 nPerHole;               // and for each hole of the margin, up to 5
    s32 n8;
} MatchPrize;
typedef struct PrizeTable {
    u8         unk0[0x5E4];
    MatchPrize aEvent[25];      // 0x5E4  per ladder event
    u8         unk710[0x9F0 - 0x710];
    s32        nLadderDone;     // 0x9F0  paid when the last event is won
} PrizeTable;
extern PrizeTable lbl_80200538;

// A save profile (0x10600 bytes); only what this file reads.
typedef struct LadderSave {
    u8  bActive;                // 0x000  the profile is in use
    u8  unk1[0x338 - 0x1];
    u8  aWon[25][4];            // 0x338  per event: [0] set once it is won
    u8  unk39C[0x10600 - 0x39C];
} LadderSave;
extern LadderSave* gpSaveData;

extern s32 lbl_802816E0;                    // the options' unkC, saved while a match is played
extern s32 lbl_80282430;                    // the wind option, saved
extern u8  lbl_80282434;                    // a ladder event is being played
extern s32 lbl_80282438;                    // the current event
extern s32 lbl_80282444;                    // the event's reward plus 1
extern s32 lbl_80282448;                    // the event's opponent
extern s32 lbl_8028244C;                    // money to add to the course tracking when the event ends
extern void (*lbl_80282450)(void);          // the challenge's own end-of-mode callback

int   fn_800584DC(int nProfile);            // the profile's earnings rating
int   fn_800D38F0(int nPlayer, int a, int nMargin, s32* pPrize);
void  fn_800D39B4(int nPlayer, int nMoney);
void  fn_800D7770(int nPlayer, u8* pFlag);
u8    fn_800D750C(int nPlayer, int nAward);
void  fn_80058278(int nProfile, int nGolfer);
u8    fn_8005832C(int nProfile, int nGolfer);  // the golfer is unlocked for the profile
void  fn_80058428(int nProfile, int nReward);
void* memcpy(void* pDst, const void* pSrc, u32 uLen);

void fn_80102404(void);
u8   fn_80102204(int nProfile, int nEvent);
int  fn_8010211C(int nEvent);
int  fn_8010217C(int nEvent);
int  fn_801021FC(void);
u8   fn_80102228(int nProfile, int nEvent);
void fn_8010237C(UStreamObject* pObject);
void fn_801023A8(UStreamObject* pObject);
void fn_801025FC(void);
void fn_801027A4(void);
void fn_80102874(void);

// Mode 4 starts: a match against the event's pro, with GameModeMatch's rules.
void fn_80101FEC(void) {
    gpGame->pfn1C8 = fn_80101FEC;
    gpGame->pfn1CC = fn_80102404;
    gpGame->pfn1D0 = fn_800E9F14;
    gpGame->pfn1D4 = fn_800EA084;
    gpGame->pfn1D8 = (u8 (*)(int, int))fn_800EA278;
    gpGame->pfn1DC = (u8 (*)(int))fn_800EA548;
    gpGame->pfn1E0 = (s32 (*)(void))fn_800EA758;
    gpGame->pfn1E8 = fn_800EAA40;
    gpGame->pfn1F4 = fn_801025FC;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

void fn_801020BC(void) {
}

// The earnings rating of player 0's profile.
int fn_801020C0(void) {
    return fn_800584DC(gPlayers[0].nIndex);
}

int fn_801020EC(int nEvent) {
    return lbl_802124B8[nEvent].nGolfer;
}

int fn_80102104(int nEvent) {
    return lbl_802124B8[nEvent].nCourse;
}

int fn_8010211C(int nEvent) {
    return lbl_802124B8[nEvent].nHoles;
}

int fn_80102134(void) {
    return fn_8010211C(fn_801021FC());
}

int fn_80102158(void) {
    return fn_8010217C(fn_801021FC());
}

// The kind of event: 0 not played here, 1 a challenge, 2 a milestone match (every fourth, and the
// last two), 3 a match.
int fn_8010217C(int nEvent) {
    if (lbl_802124B8[nEvent].n1C != 0) {
        return 0;
    }
    if (lbl_802124B8[nEvent].nChallenge != 0) {
        return 1;
    }
    if (nEvent == 3 || nEvent == 7 || nEvent == 11 || nEvent == 15 || nEvent == 19 || nEvent == 23 ||
        nEvent == 24) {
        return 2;
    }
    return 3;
}

int fn_801021FC(void) {
    return lbl_80282438;
}

// Has the profile won the event?
u8 fn_80102204(int nProfile, int nEvent) {
    return gpSaveData[nProfile].aWon[nEvent][0];
}

// Has the profile won every event this one needs?
u8 fn_80102228(int nProfile, int nEvent) {
    int i;
    u8 bOpen = 1;
    for (i = 0; i < 6; i++) {
        if (lbl_802124B8[nEvent].aNeeded[i] != 0 &&
            !fn_80102204(nProfile, lbl_802124B8[nEvent].aNeeded[i] - 1)) {
            bOpen = 0;
            break;
        }
    }
    return bOpen;
}

// Makes the event current if the profile may play it.
u8 fn_801022BC(int nProfile, int nEvent) {
    u8 bOk = 0;
    if (fn_80102228(nProfile, nEvent)) {
        lbl_80282438 = nEvent;
        bOk = 1;
    }
    return bOk;
}

void fn_80102308(s32 n) {
    lbl_8028244C = n;
}

void fn_80102310(void) {
    UStream_RegisterHandler('TCM ', fn_8010237C);
    UStream_RegisterHandler('TCMS', fn_801023A8);
}

void fn_80102354(void) {
    UStream_UnregisterHandler('TCM ');
}

void fn_8010237C(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_802124B8), lbl_802124B8);
}

void fn_801023A8(UStreamObject* pObject) {
    if (pObject) {
        lbl_8028243C.uSize = pObject->uSize;
        lbl_8028243C.pText = fn_800951A0(lbl_8028243C.uSize, 0x10, 1);
        memcpy(lbl_8028243C.pText, pObject->pData, lbl_8028243C.uSize);
    }
}

// The mode ends: the challenge's own callback first, then the saved options go back.
void fn_80102404(void) {
    if (lbl_80282450) {
        lbl_80282450();
    }
    gpGame->nC = 1;
    gpGame->n10 = 1;
    SESSION_OPTIONS->unkC = lbl_802816E0;
    SESSION_OPTIONS->nWind = lbl_80282430;
    lbl_80282434 = 0;
}

// Starts the current event: a challenge through mode 5, or a two-player match against its pro.
void fn_80102468(void) {
    int nEvent;
    int nPins;
    lbl_802816E0 = SESSION_OPTIONS->unkC;
    lbl_80282430 = SESSION_OPTIONS->nWind;
    SESSION_OPTIONS->unkC = 4;
    SESSION_OPTIONS->nWind = 0;
    lbl_80282434 = 1;
    nEvent = fn_801021FC();
    lbl_80282448 = lbl_802124B8[nEvent].nGolfer;
    lbl_80282444 = lbl_802124B8[nEvent].nReward;
    if (lbl_802124B8[nEvent].n1C == 0) {
        if (lbl_802124B8[nEvent].nChallenge != 0) {
            gSession.nNumPlayers = 1;
            fn_800E0B38(5);
            fn_800EAE38(lbl_802124B8[nEvent].nChallenge - 1);
            fn_800EAF7C();
            lbl_80282450 = gpGame->pfn1CC;
            gpGame->pfn1CC = fn_80102404;
        } else {
            lbl_80282450 = NULL;
            gpGame->nC = 2;
            gpGame->n10 = 2;
            Session_SetNumPlayers(2);
            Session_SetGolfer(lbl_802124B8[nEvent].nGolfer, 1);
            gSession.nController[1] = CONTROLLER_CPU;
            fn_800E14E0(lbl_802124B8[nEvent].nCourse);
            fn_800E1260(lbl_802124B8[nEvent].nHoles);
            nPins = lbl_802124B8[nEvent].nPins;
            gSession.nTeeSet[0] = lbl_802124B8[nEvent].nTeeSet;
            gSession.nTeeSet[1] = lbl_802124B8[nEvent].nTeeSet;
            if (nPins != 0) {
                gSession.nPinSet = nPins - 1;
            }
        }
    }
}

// Is a ladder event being played?
u8 fn_801025F4(void) {
    return lbl_80282434;
}

// EndGame: a win pays the match prize plus the event's own prize, then the event is scored.
void fn_801025FC(void) {
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
        nMoney = fn_800D38F0(0, 1, nMargin, &nPrize);
        nProfile = gPlayers[0].nIndex;
        if (gpSaveData[nProfile].bActive) {
            fn_80125910(1);
            fn_800E4364(0, 0x6E, nPrize, nProfile);
            fn_800D3548(0, nMoney, NULL);
            nEvent = fn_801021FC();
            gPlayers[0].n320 += lbl_80200538.aEvent[nEvent].nBase;
            gPlayers[0].n324 += lbl_80200538.aEvent[nEvent].nPerHole * nMargin;
            fn_80102874();
        }
    }
}

// A challenge event ended: its money, then the event is scored.
void fn_80102704(void) {
    s32 nPrize;
    int nMoney = fn_800D38F0(0, 1, 0, &nPrize);
    if (nMoney != 0 && gpSaveData[gPlayers[0].nIndex].bActive) {
        fn_800E4364(0, 0x6E, nPrize, gPlayers[0].nIndex);
        fn_800D3548(0, nMoney, NULL);
        gPlayers[0].n320 += nMoney;
    }
    fn_80102874();
}

// The message for a milestone event.
void fn_801027A4(void) {
    switch (fn_801021FC()) {
    case 11:
        fn_800E4364(11, 3, 0, 0);
        break;
    case 3:
        fn_800E4364(11, 4, 0, 0);
        break;
    case 19:
        fn_800E4364(11, 5, 0, 0);
        break;
    case 7:
        fn_800E4364(11, 7, 0, 0);
        break;
    case 15:
        fn_800E4364(11, 8, 0, 0);
        break;
    case 23:
        fn_800E4364(11, 13, 0, 0);
        break;
    }
}

// The event is won: its flag, the pro and the reward unlocked, and a message every fourth event
// (the last one also pays the ladder's prize).
void fn_80102874(void) {
    int nProfile = gPlayers[0].nIndex;
    int nEvent;
    u8 bLast;
    if (gpSaveData[nProfile].bActive) {
        nEvent = fn_801021FC();
        fn_801027A4();
        fn_800D7770(nProfile, gpSaveData[nProfile].aWon[nEvent]);
        if (lbl_80282448 != 34 && !fn_8005832C(nProfile, lbl_80282448)) {
            fn_80058278(nProfile, lbl_80282448);
            fn_800E4364(4, lbl_80282448, 0, nProfile);
        }
        if (lbl_80282444 != 0) {
            fn_80058428(nProfile, lbl_80282444 - 1);
            fn_800E4364(3, 0x16, lbl_80282444, nProfile);
        }
        if (lbl_8028244C != 0) {
            fn_800D39B4(0, lbl_8028244C);
        }
        if (fn_80102158() == 2) {
            bLast = 0;
            if (nEvent >= 24) {
                bLast = 1;
            }
            if (bLast == 1) {
                fn_800E4364(5, 0x1A, 0, nProfile);
                if (fn_800D750C(0, 15)) {
                    fn_800E4364(2, 15, lbl_80200538.nLadderDone, nProfile);
                    fn_800D3548(0, lbl_80200538.nLadderDone, NULL);
                    gPlayers[0].n31C += lbl_80200538.nLadderDone;
                }
            } else if ((nEvent + 1) % 4 == 0) {
                fn_800E4364(5, nEvent / 4 + 20, 0, nProfile);
            }
        }
    }
}

int fn_80102A44(int nEvent) {
    return lbl_802124B8[nEvent].n0;
}

// Copies the event's name.
void fn_80102A58(int nEvent, char* szOut) {
    if (nEvent < 0 || nEvent >= 25) return;
    strcpy(szOut, lbl_8028243C.pText + lbl_802124B8[nEvent].nName);
}

int fn_80102AAC(int nEvent) {
    return lbl_802124B8[nEvent].n40;
}

void fn_80102AC4(void) {
}
