// Earnings.c (EA's name, from the TW2003 source tree: Golf/GameMode/Earnings.c): the money and
// goals kept in the save profiles. It loads the prize table (stream 'ERN '), rounds payouts to $25,
// pays the tournament and match prizes, scales a payout by the bonus, course, tee, pin and TOUR
// card multipliers, checks the goals that unlock things, and keeps the saved replays. The
// GM_Earnings_ functions of TW06 come in the same order.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"
#include "frontend/fe.h"
#include "core/easb.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

// The working tables and their saved copies, ten entries each.
extern s32 lbl_80200010[10];
extern s32 lbl_80200038[10];
extern s32 lbl_80200060[10];
extern s32 lbl_80200088[10];
extern s32 lbl_802000B0[10];
extern s32 lbl_802000D8[10];
extern s32 lbl_80200100[10];
extern s32 lbl_80200128[10];
extern s32 lbl_80200150[10];
extern s32 lbl_80200178[10];
extern s32 lbl_802001A0[10];
extern s32 lbl_802001C8[10];
extern s32 lbl_802001F0[10];
extern s32 lbl_80200218[10];
extern s32 lbl_80200240[10];
extern s32 lbl_80200268[10];
extern s32 lbl_80200290[10];
extern s32 lbl_802002B8[10];
extern s32 lbl_80200358[10];
extern s32 lbl_80200380[10];
extern s32 lbl_802003A8[10];
extern s32 lbl_802003D0[10];
extern s32 lbl_802003F8[10];
extern s32 lbl_80200420[10];
extern CourseMoneyTracking lbl_801FFAE8[10];
extern CourseMoneyTracking lbl_801FFD90[10];     // the breakdown of each lbl_80200150 payout
extern s32 lbl_80282248;
extern s32 lbl_8028224C;
extern s32 lbl_80282250;
extern s32 lbl_80282254;
extern s32 lbl_80191A08[39];
extern f32 lbl_80191AA4[70];


void  fn_800D344C(UStreamObject* pObject);
int   GameMode4_GetNumEventsWon(void);
int   fn_801021FC(void);                                // GameMode4: the current ladder event
f32   fn_800D04AC(int nPlayer);                         // HoleScore.c
u32   fn_800D0BAC(int nPlayer);                         // the class of the ground the shot left
u8    fn_800D0BF8(int nPlayer, u8 bUnder, u8 bAnyLie);
u8    fn_800D0D54(int nPlayer);
int   fn_800D0DC8(int nPlayer, int nToPar);
int   fn_800D0E74(int nPlayer);
int   fn_800D0F04(int nPlayer, int nToPar);
int   fn_800D1330(int nPlayer);
int   fn_800D3208(void);                                // CourseData.c
u8    fn_800D61E4(int nPlayer, u8 bCheck);
u8    fn_800D68CC(int nPlayer, u8 bCheck);
u8    fn_800D69B8(int nPlayer, u8 bCheck);
s32   fn_80126FA0(void);                                // GameMode22.c
s32   fn_80127098(s32 n);

int   fn_800D3A20(int nProfile, u8 bMessage);
int   fn_800D3CF8(int nRating);
u8    fn_800D4010(int nId);
u8    fn_800D4EF8(u32 uMask, int nBit);
f32   fn_800D6EEC(void);
u8    fn_800D748C(int nPlayer);
u8    fn_800D76AC(int nPlayer, int nAward);
int   fn_800D7DA0(int nPlayer, int bSave, u8 bCountStroke, u8 bAll);
s32   fn_800D9E00(s32 i);

// Put the working tables back to their saved copies.
void fn_800D3244(void) {
    lbl_80282248 = lbl_80282250;
    lbl_8028224C = lbl_80282254;
    memcpy(lbl_802001C8, lbl_80200420, sizeof(lbl_802001C8));
    memcpy(lbl_802001A0, lbl_802003F8, sizeof(lbl_802001A0));
    memcpy(lbl_80200178, lbl_802003D0, sizeof(lbl_80200178));
    memcpy(lbl_80200150, lbl_802003A8, sizeof(lbl_80200150));
    memcpy(lbl_80200128, lbl_80200380, sizeof(lbl_80200128));
    memcpy(lbl_80200100, lbl_80200358, sizeof(lbl_80200100));
    memcpy(lbl_802000D8, lbl_802002B8, sizeof(lbl_802000D8));
    memcpy(lbl_802000B0, lbl_80200290, sizeof(lbl_802000B0));
    memcpy(lbl_80200088, lbl_80200268, sizeof(lbl_80200088));
    memcpy(lbl_80200060, lbl_80200240, sizeof(lbl_80200060));
    memcpy(lbl_80200038, lbl_80200218, sizeof(lbl_80200038));
    memcpy(lbl_80200010, lbl_802001F0, sizeof(lbl_80200010));
    memcpy(lbl_801FFD90, lbl_801FFAE8, sizeof(lbl_801FFD90));
}

// Every payout is a multiple of $25. TW06: roundToNearest25 (by position).
s32 fn_800D33A8(s32 nMoney) {
    return (s32)((12.5f + (f32)nMoney) / 25.0f) * 25;
}

// TW06: GM_Earnings_FreeStreamMemory (by position).
void fn_800D33F0(void) {
}

// The prize table comes from the stream. TW06: EarningsInfo_RegisterStreamClients (by position).
void fn_800D33F4(void) {
    UStream_RegisterHandler('ERN ', fn_800D344C);
}

// TW06: EarningsInfo_UnRegisterStreamClients (by position).
void fn_800D3424(void) {
    UStream_UnregisterHandler('ERN ');
}

void fn_800D344C(UStreamObject* pObject) {
    // port: the 'ERN ' object is copied straight into the prize table (EarningsTable); it is
    // big-endian on disc, so a little-endian port converts it field by field here
    // (docs/format-byteorder.md)
    fn_8000E790(pObject, sizeof(lbl_80200538), &lbl_80200538);
}

// With a row of lbl_80191AA4, n scaled down by how far it is
// into nTotal (against the row 0 share), rounded to $10; row 0 leaves n as it is.
// TW06: GM_Earnings_TournamentPayout (by position).
s32 fn_800D3478(int nTotal, int n, int nRow) {
    f32 f;
    s32 nRounded;
    s32 nRet;

    if (nRow == 0) {
        nRet = n;
    } else {
        f = 0.1f * (((1.0f - (f32)n / (f32)nTotal) / (1.0f - lbl_80191AA4[0])) *
                    ((f32)nTotal * lbl_80191AA4[nRow]));
        if (f > 0.0f) {
            nRounded = (s32)(0.5f + f);
        } else {
            nRounded = -(s32)(0.5f - f);
        }
        nRet = nRounded * 10;
    }
    return nRet;
}

// Pay a human player: the money goes into the round's breakdown (field by field from pMoney, or
// all as the payout) and into the profile's money, and the courses it now buys are unlocked.
void fn_800D3548(int nPlayer, int nMoney, CourseMoneyTracking* pMoney) {
    int nProfile;

    if (nPlayer >= 5 || nPlayer == 4) return;
    if (Player_IsCPU(nPlayer)) return;
    if (fn_800E177C() != 0) return;
    nProfile = gPlayers[nPlayer].nIndex;
    if (nProfile >= 5 || nProfile == 4) return;
    if (gpSaveData[nProfile].bActive != 1) return;
    if (pMoney != NULL) {
        gPlayers[nPlayer].money.nBase += pMoney->nBase;
        gPlayers[nPlayer].money.n24 += pMoney->n24;
        gPlayers[nPlayer].money.n0 += pMoney->n0;
        gPlayers[nPlayer].money.nCourse += pMoney->nCourse;
        gPlayers[nPlayer].money.n2C += pMoney->n2C;
        gPlayers[nPlayer].money.nTee += pMoney->nTee;
        gPlayers[nPlayer].money.nTourCard += pMoney->nTourCard;
        gPlayers[nPlayer].money.n3C += pMoney->n3C;
        gPlayers[nPlayer].money.n38 += pMoney->n38;
    } else {
        gPlayers[nPlayer].money.n24 += nMoney;
    }
    gpSaveData[nProfile].n64 += nMoney;
    gpSaveData[nProfile].n6C += nMoney;
    gpSaveData[nProfile].b70 = 1;
    fn_800D3A20(nProfile, 1);
}

// Beating a CPU golfer pays by their earnings rating: a base
// prize and so much a stroke of the margin (at most 5). *pPrize gets the base.
// TW06: GM_Earnings_GetStrokeWinnings (by position).
int fn_800D36E0(int nWinner, int nLoser, int nMargin, int* pPrize) {
    int nRating;

    if (fn_800E177C() != 0) return 0;
    if (Player_IsCPU(nWinner) || !Player_IsCPU(nLoser)) return 0;
    nRating = fn_800D3C7C(nLoser);
    if (nMargin > 5) {
        nMargin = 5;
    }
    if (pPrize != NULL) {
        *pPrize = lbl_80200538.aStrokePrize[nRating].nBase;
    }
    return lbl_80200538.aStrokePrize[nRating].nBase + lbl_80200538.aStrokePrize[nRating].nPerStroke * nMargin;
}

// The same for a team (0: players 0 and 1, 1: players 2 and
// 3) beating a CPU team: the average of what the two losers would pay.
// TW06: GM_Earnings_GetStrokeWinningsTeam (by position).
int fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize) {
    int nFirst;
    int nSecond;
    int nRating1;
    int nRating2;
    int nBase1;
    int nBase2;
    int nTotal;

    if (fn_800E177C() != 0) return 0;
    if (Team_IsAllCPU(nWinner) || !Team_IsAllCPU(nLoser)) return 0;
    if (nLoser == 0) {
        nFirst = 0;
        nSecond = 1;
    } else {
        nFirst = 2;
        nSecond = 3;
    }
    nRating1 = fn_800D3C7C(nFirst);
    nRating2 = fn_800D3C7C(nSecond);
    if (nMargin > 5) {
        nMargin = 5;
    }
    nBase1 = lbl_80200538.aStrokePrize[nRating1].nBase;
    nBase2 = lbl_80200538.aStrokePrize[nRating2].nBase;
    if (pPrize != NULL) {
        *pPrize = (nBase1 + nBase2) / 2;
    }
    nTotal = lbl_80200538.aStrokePrize[nRating1].nBase +
             lbl_80200538.aStrokePrize[nRating1].nPerStroke * nMargin;
    nTotal += lbl_80200538.aStrokePrize[nRating2].nBase +
              lbl_80200538.aStrokePrize[nRating2].nPerStroke * nMargin;
    return nTotal / 2;
}

// A ladder event won (GameMode4): the event's prize and so much a hole of the margin (at most 5),
// and its EA Sports Bio accomplishment is posted. *pPrize gets the prize.
int fn_800D38F0(int nWinner, int nLoser, int nMargin, s32* pPrize) {
    int nEvent;
    int nMoney;

    if (fn_800E177C() != 0) return 0;
    nEvent = fn_801021FC();
    if (nMargin > 5) {
        nMargin = 5;
    }
    if (pPrize != NULL) {
        *pPrize = lbl_80200538.aLadderPrize[nEvent].nBase;
    }
    nMoney = lbl_80200538.aLadderPrize[nEvent].nBase + lbl_80200538.aLadderPrize[nEvent].nPerHole * nMargin;
    if (lbl_80200538.aLadderPrize[nEvent].nBio != -1) {
        EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aLadderPrize[nEvent].nBio].szName,
                                 lbl_80200538.aBio[lbl_80200538.aLadderPrize[nEvent].nBio].nValue);
    }
    return nMoney;
}

// Pay a player twice nMoney, booked in the breakdown's n24 and n3C.
void fn_800D39B4(int nPlayer, int nMoney) {
    CourseMoneyTracking money;
    s32 nPaid;

    if (fn_800E177C() == 0) {
        nPaid = nMoney * 2;
        fn_80005AE8(&money, 0, sizeof(money));
        money.n24 = nPaid;
        money.n3C = nPaid;
        fn_800D3548(nPlayer, nPaid, &money);
    }
}

// The courses a profile's money has bought: each course whose price the money has reached is
// unlocked, with its EA Sports Bio accomplishment; courses 21 and 22 get a message of their own.
// With bMessage, a message for each other course unlocked. Returns how many those were.
int fn_800D3A20(int nProfile, u8 bMessage) {
    int i;
    int n;

    n = 0;
    for (i = 0; i < 21; i++) {
        if (gpSaveData[nProfile].n64 >= lbl_80200538.aCoursePrice[i].nPrice && !fn_800583FC(nProfile, i)) {
            fn_800583B0(nProfile, i);
            if (i != 4) {
                // EA bug: the list holds 10, and up to 20 courses could be bought at once
                lbl_801FFD68[n] = i;
                n++;
                if (lbl_80200538.aCoursePrice[i].nBio != -1) {
                    EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aCoursePrice[i].nBio].szName,
                                             lbl_80200538.aBio[lbl_80200538.aCoursePrice[i].nBio].nValue);
                }
            }
        }
    }
    if (gpSaveData[nProfile].n64 >= lbl_80200538.aCoursePrice[21].nPrice && !fn_800584B4(nProfile)) {
        fn_80058494(nProfile);
        fn_800E4364(3, 7, 2, nProfile);
        if (lbl_80200538.aCoursePrice[21].nBio != -1) {
            EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aCoursePrice[21].nBio].szName,
                                     lbl_80200538.aBio[lbl_80200538.aCoursePrice[21].nBio].nValue);
        }
    }
    if (gpSaveData[nProfile].n64 >= lbl_80200538.aCoursePrice[23].nPrice && !fn_8005846C(nProfile)) {
        fn_8005844C(nProfile);
        fn_800E4364(3, 7, 3, nProfile);
        if (lbl_80200538.aCoursePrice[23].nBio != -1) {
            EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aCoursePrice[23].nBio].szName,
                                     lbl_80200538.aBio[lbl_80200538.aCoursePrice[23].nBio].nValue);
        }
    }
    if (bMessage) {
        for (i = 0; i < n; i++) {
            // fake match: the cast gives the original's copy of nProfile for this loop
            fn_800E4364(3, lbl_801FFD68[i], 0, (u32)nProfile);
        }
    }
    return n;
}

// The best earnings rating among the players. TW06: GM_GetHighestRatedGolfer (by position).
int fn_800D3C1C(void) {
    int i;
    int nBest;
    int nRating;

    nBest = 0;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        nRating = fn_800D3C7C(i);
        if (nRating > nBest) {
            nBest = nRating;
        }
    }
    return nBest;
}

// A CPU plays at its golfer's rating; a human's comes from the profile.
// TW06: GM_Earnings_RateGolfer (by position).
int fn_800D3C7C(int nPlayer) {
    int nRating;

    nRating = fn_800584DC(gPlayers[nPlayer].nIndex);
    if (Player_IsCPU(nPlayer)) {
        return gPlayers[nPlayer].golfer.nEarningsRating;
    }
    return fn_800D3CF8(nRating);
}

// Ratings stop at 25.
int fn_800D3CF8(int nRating) {
    int n;

    n = 25;
    if (nRating <= 25) {
        n = nRating;
    }
    return n;
}

// The earnings rating of golfer nGolfer of gGolferTable; the created golfers share one.
int fn_800D3D10(int nGolfer) {
    int nRating;

    nRating = GameMode4_GetNumEventsWon();
    if (nGolfer >= FIRST_CREATED_GOLFER) {
        return fn_800D3CF8(nRating);
    }
    return gGolferTable[nGolfer].nEarningsRating;
}

// What a skin on hole nHole (0..17) is worth. TW06: GM_Earnings_GetSkinsHoleValue (by position).
s32 fn_800D3D64(int nRating, int nHole) {
    if (nHole < 6) return lbl_80200538.aSkins[nRating].aValue[0];
    if (nHole < 12) return lbl_80200538.aSkins[nRating].aValue[1];
    if (nHole < 17) return lbl_80200538.aSkins[nRating].aValue[2];
    return lbl_80200538.aSkins[nRating].aValue[3];
}

// After a shot that stayed in bounds (GM_PlayerTookShot), for a human player with a profile: the
// shot is checked (fn_800D782C, fn_800D477C) and what it earned is paid out from the working
// tables, each with its message: the lbl_80200498 entries of kind 2 or 4, the shot's bonuses with
// their breakdowns, and the awards won with their money (booked as bonuses, money.n8).
void fn_800D3DDC(int nPlayer) {
    int nProfile;
    int i;
    int nKind;

    if (Game_GetMode() == 10) return;
    if (fn_800E177C() != 0) return;
    gpGame->pfn244(nPlayer);
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    fn_800D782C(nPlayer, &gPlayers[nPlayer].ball, 1, 0, 0);
    for (i = 0; i < lbl_80282258; i++) {
        nKind = lbl_80200498[i];
        if (nKind == 2 || nKind == 4) {
            fn_800E4364(1, lbl_80200510[i], nKind, nProfile);
        }
    }
    fn_800D477C(nPlayer, &gPlayers[nPlayer].ball, 0);
    fn_800D3244();
    for (i = 0; i < lbl_8028224C; i++) {
        if (lbl_80200150[i] != 0) {
            fn_800E4364(0, lbl_802001C8[i], lbl_80200150[i], nProfile);
            fn_800D3548(nPlayer, lbl_80200150[i], &lbl_801FFD90[i]);
        }
    }
    for (i = 0; i < lbl_80282248; i++) {
        if (fn_800D750C(nPlayer, lbl_802000D8[i])) {
            if (fn_800D4010(lbl_802000D8[i])) {
                fn_800E4364(6, fn_800D9E00(lbl_802000D8[i]), 0, nProfile);
            } else {
                fn_800E4364(2, fn_800D9E00(lbl_802000D8[i]), lbl_80200060[i], nProfile);
            }
            fn_800D3548(nPlayer, lbl_80200060[i], NULL);
            gPlayers[nPlayer].money.n8 += lbl_80200060[i];
        }
    }
}

// Whether an id is one of 23..38.
u8 fn_800D4010(int nId) {
    int b;

    b = 0;
    if (nId >= 23 && nId <= 38) {
        b = 1;
    }
    return b;
}

// After a putt, for a human player with a profile: the putt record check (fn_800D7B1C) with its
// messages, then two rounds of payouts from the working tables as fn_800D3DDC pays them: the
// putt's (fn_800D4F14), then the hole's (fn_800D588C; none in a playoff).
void fn_800D4030(int nPlayer) {
    int nProfile;
    int nKind;
    int i;

    if (Game_GetMode() == 10) return;
    if (fn_800E177C() != 0) return;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    fn_800D7B1C(nPlayer, &gPlayers[nPlayer].ball, 1, 0, 0);
    for (i = 0; i < lbl_80282258; i++) {
        nKind = lbl_80200470[i];
        if (nKind == 2 || nKind == 4) {
            fn_800E4364(1, lbl_802004E8[i], nKind, nProfile);
        }
    }
    fn_800D4F14(nPlayer, 0);
    fn_800D3244();
    for (i = 0; i < lbl_8028224C; i++) {
        if (lbl_80200128[i] != 0) {
            fn_800E4364(0, lbl_802001A0[i], lbl_80200128[i], nProfile);
            fn_800D3548(nPlayer, lbl_80200128[i], &lbl_801FFD90[i]);
        }
    }
    for (i = 0; i < lbl_80282248; i++) {
        if (fn_800D750C(nPlayer, lbl_802000B0[i])) {
            if (fn_800D4010(lbl_802000B0[i])) {
                fn_800E4364(6, fn_800D9E00(lbl_802000B0[i]), 0, nProfile);
            } else {
                fn_800E4364(2, fn_800D9E00(lbl_802000B0[i]), lbl_80200038[i], nProfile);
            }
            fn_800D3548(nPlayer, lbl_80200038[i], NULL);
            gPlayers[nPlayer].money.n8 += lbl_80200038[i];
        }
    }
    if (!gpGame->bD4) {
        fn_800D588C(nPlayer, 0, 0);
    } else {
        lbl_80282254 = 0;
        lbl_80282250 = 0;
    }
    fn_800D3244();
    for (i = 0; i < lbl_8028224C; i++) {
        if (lbl_80200100[i] != 0) {
            fn_800E4364(0, lbl_80200178[i], lbl_80200100[i], nProfile);
            fn_800D3548(nPlayer, lbl_80200100[i], &lbl_801FFD90[i]);
        }
    }
    for (i = 0; i < lbl_80282248; i++) {
        if (fn_800D750C(nPlayer, lbl_80200268[i])) {
            if (fn_800D4010(lbl_80200088[i])) {
                fn_800E4364(6, fn_800D9E00(lbl_80200088[i]), 0, nProfile);
            } else {
                fn_800E4364(2, fn_800D9E00(lbl_80200088[i]), lbl_80200010[i], nProfile);
            }
            fn_800D3548(nPlayer, lbl_80200010[i], NULL);
            gPlayers[nPlayer].money.n8 += lbl_80200010[i];
        }
    }
}

// At the end of a hole, for a human player with a profile: what the hole earned is paid out from
// the working tables, with its messages (as fn_800D3DDC does after a shot), and the TOUR card
// level rises with the profile's completion score: level 2 from 7.5, 3 from 15, 4 from 30, 5 from
// 60, 6 at 100. bRoundOver skips the hole check (fn_800D7DA0) and its messages.
void fn_800D439C(int nPlayer, u8 bRoundOver) {
    int nProfile;
    int nLevel;
    int nKind;
    int i;
    f32 fProgress;

    nLevel = 0;
    if (fn_800E3A54()) return;
    if (fn_800E177C() != 0) return;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!bRoundOver) {
        fn_800D7DA0(nPlayer, 1, 0, 0);
        for (i = 0; i < lbl_80282258; i++) {
            nKind = lbl_80200448[i];
            if (nKind == 2 || nKind == 4) {
                fn_800E4364(1, lbl_802004C0[i], nKind, nProfile);
            }
        }
    }
    fn_800D588C(nPlayer, 0, bRoundOver);
    fn_800D3244();
    for (i = 0; i < lbl_8028224C; i++) {
        if (lbl_80200358[i] != 0) {
            fn_800E4364(0, lbl_802003D0[i], lbl_80200358[i], nProfile);
            fn_800D3548(nPlayer, lbl_80200358[i], &lbl_801FFD90[i]);
        }
    }
    for (i = 0; i < lbl_80282248; i++) {
        if (fn_800D750C(nPlayer, lbl_80200268[i])) {
            if (fn_800D4010(lbl_80200088[i])) {
                fn_800E4364(6, fn_800D9E00(lbl_80200088[i]), 0, nProfile);
            } else {
                fn_800E4364(2, fn_800D9E00(lbl_80200088[i]), lbl_80200010[i], nProfile);
            }
            fn_800D3548(nPlayer, lbl_802001F0[i], NULL);
            gPlayers[nPlayer].money.n8 += lbl_802001F0[i];
        }
    }
    fProgress = GM_GetGameProgress(&gpSaveData[nProfile]);
    if (fProgress >= 100.0f) {
        nLevel = 6;
    } else if (fProgress >= 60.0f) {
        nLevel = 5;
    } else if (fProgress >= 30.0f) {
        nLevel = 4;
    } else if (fProgress >= 15.0f) {
        nLevel = 3;
    } else if (fProgress >= 7.5f) {
        nLevel = 2;
    }
    if (gpSaveData[nProfile].nTourCardLevel < nLevel) {
        gpSaveData[nProfile].nTourCardLevel = nLevel;
        fn_800E4364(0, gpSaveData[nProfile].nTourCardLevel + 99, 0, nProfile);
    }
}

s32 fn_800D4694(u32 n) {
    if (n == 1) return 0;
    if (n == 2) return 1;
    if (n == 5) return 2;
    if (n == 6) return 3;
    if (n == 3) return 4;
    return 6;
}

s32 fn_800D46E8(int n) {
    int r;

    if (n == 0) return 0;
    if (n == 1 || n == 2 || n == 10) return 1;
    if (n == 3 || n == 4 || n == 5 || n == 14 || n == 15 || n == 17) return 2;
    if (n == 6 || n == 7 || n == 8) return 3;
    if (n == 9) return 4;
    r = 6;                      // fake match: a plain "return 6" makes the last test branch-free
    if (n == 12) {
        r = 5;
    }
    return r;
}

// After a shot: check the shot goals and fill the working tables with what they give, awards
// (lbl_80282250 of them) and money prizes (lbl_80282254), as fn_800D4F14 does for putts. With
// pBall the check runs on that ball in place of the player's own. With bPreview the shot is not
// counted yet (one stroke fewer), and no EA Sports Bio accomplishment is posted; without a ball
// too, the tests on the ball are skipped.
void fn_800D477C(int nPlayer, Ball* pBall, u8 bPreview) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    Ball saved;
    int i;
    int nSlot;
    u8 bReplace;
    u8 bLost;
    int j;
    s32 nValue;
    u8 bHole;
    u8 bTee;
    u8 bNoBall;
    int nAdj;
    int nHoles;

    lbl_80282254 = 0;
    lbl_80282250 = 0;
    if (gSession.uFlags & 0x4000) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!fn_800D748C(nPlayer)) return;
    if (fn_80100294()) return;
    if (fn_800E177C() != 0) return;

    if (pBall != NULL) {
        Mem_cpy(&saved, &gPlayers[nPlayer].ball, sizeof(Ball));
        Mem_cpy(&gPlayers[nPlayer].ball, pBall, sizeof(Ball));
    }
    nAdj = bPreview ? -1 : 0;
    if (bPreview) {
        if (pBall == NULL) {
            bNoBall = 1;
        } else {
            bNoBall = 0;
        }
    } else {
        bNoBall = 0;
    }

    for (i = 0; i < NUM_SHOT_GOALS; i++) {
        if (!lbl_80200538.aShotGoal[i].bEnabled) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uModes, Game_GetMode())) continue;
        if (fn_800EC550() && !fn_801025F4() && !fn_800D4EF8(lbl_80200538.aShotGoal[i].uModes, 5)) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uPars, 0) && fn_800D2B08() == 3) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uPars, 1) && fn_800D2B08() == 4) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uPars, 2) && fn_800D2B08() == 5) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uLies, fn_800D4694(fn_800D0BAC(nPlayer)))) continue;
        if (lbl_80200538.aShotGoal[i].f0C > fn_800D04AC(nPlayer)) continue;
        if (!bNoBall && !fn_800D4EF8(lbl_80200538.aShotGoal[i].uBallLies,
                                     fn_800D46E8(gPlayers[nPlayer].ball.nLie))) continue;
        if (!bNoBall && lbl_80200538.aShotGoal[i].f14 > fn_800D0550(nPlayer)) continue;
        if (!bNoBall && lbl_80200538.aShotGoal[i].f18 != 0.0f &&
            lbl_80200538.aShotGoal[i].f18 < fn_800D0478(nPlayer)) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uShotKinds, gPlayers[nPlayer].nShotKind)) continue;
        if (!fn_800D4EF8(lbl_80200538.aShotGoal[i].uClubs, gPlayers[nPlayer].nClub)) continue;
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 0) && !fn_800D0BF8(nPlayer, 1, bPreview)) continue;
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 1) && !fn_800D0BF8(nPlayer, 0, bPreview)) continue;
        if (!bNoBall && fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 2) && !fn_800D0D54(nPlayer)) continue;
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 3) && gPlayers[nPlayer].b312) continue;
        if (!bNoBall && fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 4) && !gPlayers[nPlayer].b30C) continue;
        if (!bNoBall && fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 5) && !gPlayers[nPlayer].b30D) continue;
        // Flag 6: the hole's first stroke.
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 6) &&
            nAdj + 1 != gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]) continue;
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 7) && fn_800D1170(nPlayer, 0) < nAdj + 18) continue;
        if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uFlags, 8)) {
            nHoles = fn_800D3208();
            if (nHoles < 10) continue;
            if (nHoles + nAdj > fn_800D0FBC(nPlayer)) continue;
        }
        if (lbl_80200538.aShotGoal[i].nAward >= 23 && lbl_80200538.aShotGoal[i].nAward <= 38 &&
            (bPreview || !fn_800D9998(nPlayer, lbl_80200538.aShotGoal[i].nAward))) continue;
        if (lbl_80200538.aShotGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (lbl_80200538.aShotGoal[i].nAward != 39) {
            if (!fn_800D76AC(nPlayer, lbl_80200538.aShotGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282250;
            if (lbl_80200538.aShotGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282250; j++) {
                    if (lbl_80200538.aShotGoal[i].nId == aAwardIds[j]) {
                        if (lbl_80200538.aShotGoal[i].nValue > lbl_80200240[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            lbl_802002B8[nSlot] = lbl_80200538.aShotGoal[i].nAward;
            lbl_80200240[nSlot] = lbl_80200538.aShotGoal[i].nValue;
            aAwardIds[nSlot] = lbl_80200538.aShotGoal[i].nId;
            if (!bReplace) {
                lbl_80282250++;
            }
            if (!bPreview && lbl_80200538.aShotGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aShotGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aShotGoal[i].nBio].nValue);
            }
        } else {
            nValue = lbl_80200538.aShotGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282254;
            if (lbl_80200538.aShotGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282254; j++) {
                    if (lbl_80200538.aShotGoal[i].nId == aPrizeIds[j]) {
                        if (lbl_80200538.aShotGoal[i].nValue > lbl_80200330[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            aPrizeIds[nSlot] = lbl_80200538.aShotGoal[i].nId;
            lbl_80200330[nSlot] = nValue;
            bHole = fn_800D4EF8(lbl_80200538.aShotGoal[i].uMults, 2);
            bTee = fn_800D4EF8(lbl_80200538.aShotGoal[i].uMults, 1);
            lbl_802003A8[nSlot] = fn_800D6A70(lbl_80200330[nSlot], nPlayer,
                                              fn_800D4EF8(lbl_80200538.aShotGoal[i].uMults, 0), bTee, bHole,
                                              &lbl_801FFAE8[nSlot]);
            if (fn_800D4EF8(lbl_80200538.aShotGoal[i].uMults, 3)) {
                lbl_802003A8[nSlot] = fn_800D7220(lbl_802003A8[nSlot], nPlayer, &lbl_801FFAE8[nSlot]);
            }
            lbl_80200420[nSlot] = lbl_80200538.aShotGoal[i].n2A;
            if (!bReplace) {
                lbl_80282254++;
            }
            if (!bPreview && lbl_80200538.aShotGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aShotGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aShotGoal[i].nBio].nValue);
            }
        }
    }

    if (pBall != NULL) {
        Mem_cpy(&gPlayers[nPlayer].ball, &saved, sizeof(Ball));
    }
}

// Whether bit nBit of uMask is set.
u8 fn_800D4EF8(u32 uMask, int nBit) {
    return (uMask & (1 << nBit)) != 0;
}

// After a putt: check the putt goals and fill the working tables with what they give, awards
// (lbl_80282250 of them) and money prizes (lbl_80282254). Of goals with the same id only the one
// with the biggest nValue is kept. The holes still to come count 999 strokes and putts meanwhile.
// With bPreview the hole counts one more stroke and putt (the ball dropping now), a few tests are
// skipped and no EA Sports Bio accomplishment is posted.
void fn_800D4F14(int nPlayer, u8 bPreview) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    int i;
    int nSlot;
    u8 bReplace;
    u8 bLost;
    int j;
    s32 nValue;
    u8 bHole;
    u8 bTee;

    lbl_80282254 = 0;
    lbl_80282250 = 0;
    if (gSession.uFlags & 0x4000) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!fn_800D748C(nPlayer)) return;
    if (fn_800EC550() && !fn_801025F4()) return;
    if (fn_800E177C() != 0) return;

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 999;
        gPlayers[nPlayer].nPutts[i] = 999;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]++;
    }

    for (i = 0; i < NUM_PUTT_GOALS; i++) {
        if (!lbl_80200538.aPuttGoal[i].bEnabled) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uModes, Game_GetMode())) continue;
        if (fn_800EC550() && !fn_801025F4() && !fn_800D4EF8(lbl_80200538.aPuttGoal[i].uModes, 5)) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uPars, 0) && fn_800D2B08() == 3) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uPars, 1) && fn_800D2B08() == 4) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uPars, 2) && fn_800D2B08() == 5) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uLies, fn_800D4694(fn_800D0BAC(nPlayer)))) continue;
        if (lbl_80200538.aPuttGoal[i].f0C > fn_800D04AC(nPlayer)) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uShotKinds, gPlayers[nPlayer].nShotKind)) continue;
        if (!fn_800D4EF8(lbl_80200538.aPuttGoal[i].uClubs, gPlayers[nPlayer].nClub)) continue;
        if (fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 0) && !fn_800D0BF8(nPlayer, 1, 0)) continue;
        if (fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 1) && !fn_800D0BF8(nPlayer, 0, 0)) continue;
        if (!bPreview && fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 2) && !fn_800D0D54(nPlayer)) continue;
        if (fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 4) &&
            !gPlayers[nPlayer].b310 && !gPlayers[nPlayer].b311) continue;
        if (!bPreview && fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 5) &&
            !gPlayers[nPlayer].b30D) continue;
        if (lbl_80200538.aPuttGoal[i].nMaxPutts != 0 &&
            lbl_80200538.aPuttGoal[i].nMaxPutts < gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]) continue;
        // The score on the hole: 2 triple bogey or better, 3 double bogey, 4 bogey, 5 par or better,
        // 6 birdie, 7 eagle and 8 albatross (not with a hole in one), 9 a hole in one.
        if (lbl_80200538.aPuttGoal[i].nScore == 2 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > fn_800D2B08() + 3) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 3 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != fn_800D2B08() + 2) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 4 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != fn_800D2B08() + 1) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 5 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > fn_800D2B08()) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 6 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != fn_800D2B08() - 1) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 7 &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 ||
             gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != fn_800D2B08() - 2)) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 8 &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 ||
             gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != fn_800D2B08() - 3)) continue;
        if (lbl_80200538.aPuttGoal[i].nScore == 9 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > 1) continue;
        // Flag 6: a hole in one, and the round's second (fn_800D0DC8 below -3 counts holes in one).
        if (fn_800D4EF8(lbl_80200538.aPuttGoal[i].uFlags, 6) &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > 1 || fn_800D0DC8(nPlayer, -5) < 2)) continue;
        if (lbl_80200538.aPuttGoal[i].nAward >= 23 && lbl_80200538.aPuttGoal[i].nAward <= 38 &&
            (bPreview || !fn_800D9998(nPlayer, lbl_80200538.aPuttGoal[i].nAward))) continue;
        if (lbl_80200538.aPuttGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (lbl_80200538.aPuttGoal[i].nAward != 39) {
            if (!fn_800D76AC(nPlayer, lbl_80200538.aPuttGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282250;
            if (lbl_80200538.aPuttGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282250; j++) {
                    if (lbl_80200538.aPuttGoal[i].nId == aAwardIds[j]) {
                        if (lbl_80200538.aPuttGoal[i].nValue > lbl_80200218[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            lbl_80200290[nSlot] = lbl_80200538.aPuttGoal[i].nAward;
            lbl_80200218[nSlot] = lbl_80200538.aPuttGoal[i].nValue;
            aAwardIds[nSlot] = lbl_80200538.aPuttGoal[i].nId;
            if (!bReplace) {
                lbl_80282250++;
            }
            if (!bPreview && lbl_80200538.aPuttGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aPuttGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aPuttGoal[i].nBio].nValue);
            }
        } else {
            nValue = lbl_80200538.aPuttGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282254;
            if (lbl_80200538.aPuttGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282254; j++) {
                    if (lbl_80200538.aPuttGoal[i].nId == aPrizeIds[j]) {
                        if (lbl_80200538.aPuttGoal[i].nValue > lbl_80200308[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            aPrizeIds[nSlot] = lbl_80200538.aPuttGoal[i].nId;
            lbl_80200308[nSlot] = nValue;
            bHole = fn_800D4EF8(lbl_80200538.aPuttGoal[i].uMults, 2);
            bTee = fn_800D4EF8(lbl_80200538.aPuttGoal[i].uMults, 1);
            lbl_80200380[nSlot] = fn_800D6A70(lbl_80200308[nSlot], nPlayer,
                                              fn_800D4EF8(lbl_80200538.aPuttGoal[i].uMults, 0), bTee, bHole,
                                              &lbl_801FFAE8[nSlot]);
            if (fn_800D4EF8(lbl_80200538.aPuttGoal[i].uMults, 3)) {
                lbl_80200380[nSlot] = fn_800D7220(lbl_80200380[nSlot], nPlayer, &lbl_801FFAE8[nSlot]);
            }
            lbl_802003F8[nSlot] = lbl_80200538.aPuttGoal[i].n1D;
            if (!bReplace) {
                lbl_80282254++;
            }
            if (!bPreview && lbl_80200538.aPuttGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aPuttGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aPuttGoal[i].nBio].nValue);
            }
        }
    }

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 0;
        gPlayers[nPlayer].nPutts[i] = 0;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]--;
    }
}

// After a hole (bRoundOver: after the round), as fn_800D4F14 does for the putt goals: check the
// hole goals against the round so far and fill the working tables (lbl_80200268 awards,
// lbl_802002E0 money prizes).
void fn_800D588C(int nPlayer, u8 bPreview, u8 bRoundOver) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    int i;
    int nSlot;
    u8 bReplace;
    u8 bLost;
    int j;
    s32 nValue;
    u8 bHole;
    u8 bTee;
    int nHoles;
    int nNeed;
    u8 bMore;

    lbl_80282254 = 0;
    lbl_80282250 = 0;
    if (gSession.uFlags & 0x4000) return;
    if (!fn_800D748C(nPlayer)) return;
    if (fn_800E177C() != 0) return;

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 999;
        gPlayers[nPlayer].nPutts[i] = 999;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]++;
    }
    nNeed = 0;                  // fake match: the flag is worked out in an int first
    if (!bRoundOver && (!fn_800E1BBC() || Game_CurHoleIndex() != 17)) {
        nNeed = 1;
    }
    bMore = nNeed;

    for (i = 0; i < NUM_HOLE_GOALS; i++) {
        if (lbl_80200538.aHoleGoal[i].bEachHole && bRoundOver) continue;
        if (!lbl_80200538.aHoleGoal[i].bEachHole && !bRoundOver) continue;
        if (!lbl_80200538.aHoleGoal[i].bEnabled) continue;
        if (!fn_800D4EF8(lbl_80200538.aHoleGoal[i].uModes, Game_GetMode())) continue;
        if (fn_800EC550() && !fn_801025F4() && !fn_800D4EF8(lbl_80200538.aHoleGoal[i].uModes, 5)) continue;
        if (!lbl_80200538.aHoleGoal[i].b19 && bMore) continue;
        if (lbl_80200538.aHoleGoal[i].aToPar[0] != 0 &&
            lbl_80200538.aHoleGoal[i].aToPar[0] > fn_800D0DC8(nPlayer, 0)) continue;
        if (lbl_80200538.aHoleGoal[i].aToPar[1] != 0 &&
            lbl_80200538.aHoleGoal[i].aToPar[1] > fn_800D0DC8(nPlayer, -1)) continue;
        if (lbl_80200538.aHoleGoal[i].aToPar[2] != 0 &&
            lbl_80200538.aHoleGoal[i].aToPar[2] > fn_800D0DC8(nPlayer, -2)) continue;
        if (lbl_80200538.aHoleGoal[i].aToPar[3] != 0 &&
            lbl_80200538.aHoleGoal[i].aToPar[3] > fn_800D0DC8(nPlayer, -3)) continue;
        if (lbl_80200538.aHoleGoal[i].aToPar[4] != 0 &&
            lbl_80200538.aHoleGoal[i].aToPar[4] > fn_800D0DC8(nPlayer, -5)) continue;
        if (lbl_80200538.aHoleGoal[i].aRun[0] != 0 &&
            lbl_80200538.aHoleGoal[i].aRun[0] > fn_800D0F04(nPlayer, 0)) continue;
        if (lbl_80200538.aHoleGoal[i].aRun[1] != 0 &&
            lbl_80200538.aHoleGoal[i].aRun[1] > fn_800D0F04(nPlayer, -1)) continue;
        if (lbl_80200538.aHoleGoal[i].aRun[2] != 0 &&
            lbl_80200538.aHoleGoal[i].aRun[2] > fn_800D0F04(nPlayer, -2)) continue;
        if (lbl_80200538.aHoleGoal[i].aRun[3] != 0 &&
            lbl_80200538.aHoleGoal[i].aRun[3] > fn_800D0F04(nPlayer, -3)) continue;
        if (lbl_80200538.aHoleGoal[i].aRun[4] != 0 &&
            lbl_80200538.aHoleGoal[i].aRun[4] > fn_800D0F04(nPlayer, -5)) continue;
        if (lbl_80200538.aHoleGoal[i].n13 != 0) {
            nNeed = lbl_80200538.aHoleGoal[i].n13;
            nHoles = fn_800D3208();
            if (nHoles < 10) continue;
            if (nNeed > nHoles) {
                nNeed = nHoles;
            }
            if (nNeed > fn_800D0FBC(nPlayer)) continue;
        }
        if (lbl_80200538.aHoleGoal[i].n14 != 0 &&
            lbl_80200538.aHoleGoal[i].n14 > fn_800D1170(nPlayer, 0)) continue;
        if (lbl_80200538.aHoleGoal[i].n15 != 0 &&
            lbl_80200538.aHoleGoal[i].n15 > fn_800D10B0(nPlayer)) continue;
        if (lbl_80200538.aHoleGoal[i].n16 != 0 &&
            lbl_80200538.aHoleGoal[i].n16 > fn_800D1250(nPlayer)) continue;
        if (lbl_80200538.aHoleGoal[i].n17 != 0 &&
            lbl_80200538.aHoleGoal[i].n17 > fn_800D1330(nPlayer)) continue;
        if (lbl_80200538.aHoleGoal[i].nMaxStrokes != 0 &&
            lbl_80200538.aHoleGoal[i].nMaxStrokes < fn_800E17AC(nPlayer)) continue;
        if (lbl_80200538.aHoleGoal[i].nKind != 0) {
            if (lbl_80200538.aHoleGoal[i].nKind == 1 && !fn_800D61E4(nPlayer, bPreview)) continue;
            if (lbl_80200538.aHoleGoal[i].nKind == 2 && !fn_800D68CC(nPlayer, bPreview)) continue;
            if (lbl_80200538.aHoleGoal[i].nKind == 3) continue;
            if (lbl_80200538.aHoleGoal[i].nKind == 4 && !fn_800D69B8(nPlayer, bPreview)) continue;
            if (lbl_80200538.aHoleGoal[i].nKind == 5 && fn_800D0E74(nPlayer) != 0) continue;
            if (lbl_80200538.aHoleGoal[i].nKind == 6 && fn_800D2FB4(0) <= fn_800E17AC(nPlayer)) continue;
        }
        if (lbl_80200538.aHoleGoal[i].nAward >= 23 && lbl_80200538.aHoleGoal[i].nAward <= 38 &&
            (bPreview || !fn_800D9998(nPlayer, lbl_80200538.aHoleGoal[i].nAward))) continue;
        if (lbl_80200538.aHoleGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (lbl_80200538.aHoleGoal[i].nAward != 39) {
            if (!fn_800D76AC(nPlayer, lbl_80200538.aHoleGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282250;
            if (lbl_80200538.aHoleGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282250; j++) {
                    if (lbl_80200538.aHoleGoal[i].nId == aAwardIds[j]) {
                        if (lbl_80200538.aHoleGoal[i].nValue > lbl_802001F0[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            lbl_80200268[nSlot] = lbl_80200538.aHoleGoal[i].nAward;
            lbl_802001F0[nSlot] = lbl_80200538.aHoleGoal[i].nValue;
            aAwardIds[nSlot] = lbl_80200538.aHoleGoal[i].nId;
            if (!bReplace) {
                lbl_80282250++;
            }
            if (!bPreview && lbl_80200538.aHoleGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aHoleGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aHoleGoal[i].nBio].nValue);
            }
        } else {
            nValue = lbl_80200538.aHoleGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = lbl_80282254;
            if (lbl_80200538.aHoleGoal[i].nId != 0) {
                for (j = 0; j < lbl_80282254; j++) {
                    if (lbl_80200538.aHoleGoal[i].nId == aPrizeIds[j]) {
                        if (lbl_80200538.aHoleGoal[i].nValue > lbl_802002E0[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            aPrizeIds[nSlot] = lbl_80200538.aHoleGoal[i].nId;
            lbl_802002E0[nSlot] = nValue;
            bHole = fn_800D4EF8(lbl_80200538.aHoleGoal[i].uMults, 2);
            bTee = fn_800D4EF8(lbl_80200538.aHoleGoal[i].uMults, 1);
            lbl_80200358[nSlot] = fn_800D6A70(lbl_802002E0[nSlot], nPlayer,
                                              fn_800D4EF8(lbl_80200538.aHoleGoal[i].uMults, 0), bTee, bHole,
                                              &lbl_801FFAE8[nSlot]);
            if (fn_800D4EF8(lbl_80200538.aHoleGoal[i].uMults, 3)) {
                lbl_80200358[nSlot] = fn_800D7220(lbl_80200358[nSlot], nPlayer, &lbl_801FFAE8[nSlot]);
            }
            lbl_802003D0[nSlot] = lbl_80200538.aHoleGoal[i].n25;
            if (!bReplace) {
                lbl_80282254++;
            }
            if (!bPreview && lbl_80200538.aHoleGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(lbl_80200538.aBio[lbl_80200538.aHoleGoal[i].nBio].szName,
                                         lbl_80200538.aBio[lbl_80200538.aHoleGoal[i].nBio].nValue);
            }
        }
    }

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 0;
        gPlayers[nPlayer].nPutts[i] = 0;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]--;
    }
}

// The marked holes (fn_800E1CE8's items, kind 0). Without bCheck: whether the profile has all 71.
// With it: whether this par 5 was played in eagle or better with 70 held and this hole the one
// missing.
u8 fn_800D61E4(int nPlayer, u8 bCheck) {
    int nProfile;
    u8 bAll;
    int i;
    int nHave;
    int nItem;

    nProfile = gPlayers[nPlayer].nIndex;
    if (!gpSaveData[nProfile].bActive) return 0;
    if (!bCheck) {
        bAll = 1;
        for (i = 0; i < 71; i++) {
            if (!fn_800588F4(&gpSaveData[nProfile], 0, i)) {
                bAll = 0;
            }
        }
        return bAll;
    }
    if (fn_800D2ABC(Game_GetCourse(), fn_80015464()) == 5 &&
        gPlayers[nPlayer].nStrokes[fn_80015464()] <= fn_800D2ABC(gpGame->nCurCourse, fn_80015464()) - 2) {
        // EA bug: from here the profile is picked by nPlayer, not nProfile as above.
        nHave = 0;
        for (i = 0; i < 71; i++) {
            if (fn_800588F4(&gpSaveData[nPlayer], 0, i)) {
                nHave++;
            }
        }
        if (nHave == 70) {
            nItem = -1;
            switch (Game_GetCourse()) {
            case 0:
                switch (fn_80015464() + 1) {
                case 2: nItem = 0; break;
                case 6: nItem = 1; break;
                case 14: nItem = 2; break;
                case 18: nItem = 3; break;
                }
                break;
            case 1:
                switch (fn_80015464() + 1) {
                case 2: nItem = 4; break;
                case 4: nItem = 5; break;
                case 10: nItem = 6; break;
                case 15: nItem = 7; break;
                }
                break;
            case 2:
                switch (fn_80015464() + 1) {
                case 2: nItem = 8; break;
                case 9: nItem = 9; break;
                case 11: nItem = 10; break;
                case 16: nItem = 11; break;
                }
                break;
            case 3:
                switch (fn_80015464() + 1) {
                case 2: nItem = 12; break;
                case 9: nItem = 13; break;
                case 13: nItem = 14; break;
                case 18: nItem = 15; break;
                }
                break;
            case 6:
                switch (fn_80015464() + 1) {
                case 6: nItem = 16; break;
                case 15: nItem = 17; break;
                case 17: nItem = 18; break;
                }
                break;
            case 12:
                switch (fn_80015464() + 1) {
                case 2: nItem = 19; break;
                case 6: nItem = 20; break;
                case 11: nItem = 21; break;
                case 18: nItem = 22; break;
                }
                break;
            case 9:
                switch (fn_80015464() + 1) {
                case 4: nItem = 23; break;
                case 6: nItem = 24; break;
                case 12: nItem = 25; break;
                case 16: nItem = 26; break;
                }
                break;
            // EA left out course 10 (items 27..30), which fn_800E1CE8 has.
            case 11:
                switch (fn_80015464() + 1) {
                case 4: nItem = 31; break;
                case 9: nItem = 32; break;
                case 10: nItem = 33; break;
                case 12: nItem = 34; break;
                case 18: nItem = 35; break;
                }
                break;
            case 13:
                switch (fn_80015464() + 1) {
                case 3: nItem = 36; break;
                case 13: nItem = 37; break;
                case 15: nItem = 38; break;
                }
                break;
            case 15:
                switch (fn_80015464() + 1) {
                case 5: nItem = 39; break;
                case 14: nItem = 40; break;
                }
                break;
            case 14:
                switch (fn_80015464() + 1) {
                case 6: nItem = 41; break;
                case 9: nItem = 42; break;
                case 13: nItem = 43; break;
                case 18: nItem = 44; break;
                }
                break;
            case 16:
                switch (fn_80015464() + 1) {
                case 2: nItem = 45; break;
                case 6: nItem = 46; break;
                case 11: nItem = 47; break;
                }
                break;
            case 17:
                switch (fn_80015464() + 1) {
                case 1: nItem = 48; break;
                case 6: nItem = 49; break;
                case 7: nItem = 50; break;
                case 11: nItem = 51; break;
                case 15: nItem = 52; break;
                }
                break;
            case 18:
                switch (fn_80015464() + 1) {
                case 5: nItem = 53; break;
                case 8: nItem = 54; break;
                case 13: nItem = 55; break;
                case 18: nItem = 56; break;
                }
                break;
            case 19:
                switch (fn_80015464() + 1) {
                case 5: nItem = 57; break;
                case 9: nItem = 58; break;
                case 15: nItem = 59; break;
                case 18: nItem = 60; break;
                }
                break;
            case 20:
                switch (fn_80015464() + 1) {
                case 4: nItem = 61; break;
                case 10: nItem = 62; break;
                case 16: nItem = 63; break;
                }
                break;
            case 5:
                switch (fn_80015464() + 1) {
                case 4: nItem = 64; break;
                case 7: nItem = 65; break;
                case 13: nItem = 66; break;
                }
                break;
            case 8:
                switch (fn_80015464() + 1) {
                case 4: nItem = 67; break;
                case 6: nItem = 68; break;
                case 12: nItem = 69; break;
                case 16: nItem = 70; break;
                }
                break;
            case 4:
                switch (fn_80015464() + 1) {
                case 1: nItem = 71; break;
                case 7: nItem = 72; break;
                case 10: nItem = 73; break;
                case 18: nItem = 74; break;
                }
                break;
            }
            if (nItem >= 0 && !fn_800588F4(&gpSaveData[nPlayer], 0, nItem)) return 1;
        }
    }
    return 0;
}

// Without bCheck: whether the profile has won all 31 PGA TOUR tournaments. With it: whether this is
// game mode 23 with 30 won and fn_800CF450 agrees.
u8 fn_800D68CC(int nPlayer, u8 bCheck) {
    SaveProfile* pProfile;
    int i;
    int n;
    u8 bAll;

    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive == 0) return 0;
    pProfile = &gpSaveData[gPlayers[nPlayer].nIndex];
    if (!bCheck) {
        bAll = 1;
        for (i = 0; i < 31; i++) {
            if (!pProfile->aC8[i].award.bWon) {
                bAll = 0;
            }
        }
        return bAll;
    }
    n = 0;
    for (i = 0; i < 31; i++) {
        if (pProfile->aC8[i].award.bWon) {
            n++;
        }
    }
    if (Game_GetMode() == 23 && n == 30 && fn_800CF450(nPlayer)) return 1;
    return 0;
}

// Without bCheck: whether the profile has won any PGA TOUR tournament. With it: whether this is
// game mode 23 and fn_800CF450 agrees.
u8 fn_800D69B8(int nPlayer, u8 bCheck) {
    SaveProfile* pProfile;
    int i;
    u8 bAny;

    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive == 0) return 0;
    pProfile = &gpSaveData[gPlayers[nPlayer].nIndex];
    if (!bCheck) {
        bAny = 0;
        for (i = 0; i < 31; i++) {
            if (pProfile->aC8[i].award.bWon) {
                bAny = 1;
            }
        }
        return bAny;
    }
    if (Game_GetMode() == 23 && fn_800CF450(nPlayer)) return 1;
    return 0;
}

// The points, rounded to $25, earn a bonus on top for the
// course, the tees played and the hole's gpGame->nPinSet value (each flag switches one on).
// Each part is rounded to $25 by itself; the total is at least 0.
// TW06: GM_Earnings_ComputeBonusModifiers (by position).
s32 fn_800D6A70(s32 nPoints, int nPlayer, u8 bCourse, u8 bTee, u8 bHole,
                                       CourseMoneyTracking* pMoney) {
    f32 fCourseBonus;           // fake match: whole dollars kept as floats and added as floats, as
    f32 fTeeBonus;              // the original does (s32 locals with float casts: 87%)
    f32 fHoleBonus;
    f32 fCourse;
    f32 fTee;
    f32 fHole;
    s32 nBase;
    s32 nTotal;

    if (fn_800E177C() != 0) return 0;
    fTee = 1.0f;
    fHole = fTee;
    fCourse = fn_800D6EEC();
    switch (gSession.nTeeSet[nPlayer]) {
    case 0:
        fTee = (f32)lbl_80200538.aMult[EARN_MULT_TEE +2] / 100.0f;
        break;
    case 1:
        fTee = (f32)lbl_80200538.aMult[EARN_MULT_TEE +1] / 100.0f;
        break;
    case 2:
        fTee = (f32)lbl_80200538.aMult[EARN_MULT_TEE +0] / 100.0f;
        break;
    case 3:
        fTee = 1.0f;
        break;
    }
    switch (gpGame->nPinSet[Game_CurHoleIndex()]) {
    case 0:
        fHole = (f32)lbl_80200538.aMult[EARN_MULT_PINSET +0] / 100.0f;
        break;
    case 1:
        fHole = (f32)lbl_80200538.aMult[EARN_MULT_PINSET +1] / 100.0f;
        break;
    case 2:
        fHole = (f32)lbl_80200538.aMult[EARN_MULT_PINSET +2] / 100.0f;
        break;
    case 3:
        fHole = (f32)lbl_80200538.aMult[EARN_MULT_PINSET +3] / 100.0f;
        break;
    }
    nBase = fn_800D33A8(nPoints);
    if (bCourse) {
        fCourse = (f32)nBase * fCourse - (f32)nBase;
    } else {
        fCourse = 0.0f;
    }
    if (bTee) {
        fTee = (f32)nBase * fTee - (f32)nBase;
    } else {
        fTee = 0.0f;
    }
    if (bHole) {
        fHole = (f32)nBase * fHole - (f32)nBase;
    } else {
        fHole = 0.0f;
    }
    fCourseBonus = fn_800D33A8((s32)fCourse);
    fTeeBonus = fn_800D33A8((s32)fTee);
    fHoleBonus = fn_800D33A8((s32)fHole);
    nTotal = (s32)((f32)nBase + (fHoleBonus + (fCourseBonus + fTeeBonus)));
    if (nTotal < 0) {
        nTotal = 0;
    }
    nTotal = (s32)((12.5f + (f32)nTotal) / 25.0f) * 25;
    if (pMoney != NULL) {
        pMoney->n24 = nTotal;
        pMoney->nBase = nBase;
        pMoney->n0 = nTotal;
        pMoney->nCourse = (s32)fCourseBonus;
        pMoney->n2C = (s32)fHoleBonus;
        pMoney->nTee = (s32)fTeeBonus;
    }
    return nTotal;
}

// The current course's payout multiplier (1 for a course the table does not list); course 7 has
// three, one per fn_80015464 value.
f32 fn_800D6EEC(void) {
    f32 fMult;

    fMult = 1.0f;
    switch (gpGame->nCurCourse) {
    case 0:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +0];
        break;
    case 2:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +1];
        break;
    case 1:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +2];
        break;
    case 6:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +3];
        break;
    case 10:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +4];
        break;
    case 11:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +5];
        break;
    case 13:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +6];
        break;
    case 15:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +7];
        break;
    case 14:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +8];
        break;
    case 3:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +9];
        break;
    case 12:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +10];
        break;
    case 9:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +11];
        break;
    case 22:
        fMult = lbl_80200538.aMult[EARN_MULT_COURSE +12];
        break;
    case 7:
        if (fn_80015464() == 0) {
            fMult = lbl_80200538.aMult[EARN_MULT_COURSE +13];
        }
        if (fn_80015464() == 1) {
            fMult = lbl_80200538.aMult[EARN_MULT_COURSE +14];
        }
        if (fn_80015464() == 2) {
            fMult = lbl_80200538.aMult[EARN_MULT_COURSE +15];
        }
        break;
    }
    return fMult;
}

// The TOUR card level raises the payout; the extra
// goes in the breakdown. Nothing is paid when fn_800E177C says so.
// TW06: GM_Earnings_ComputeTOURCardModifiers (by position).
int fn_800D7220(int nReward, int nPlayer, CourseMoneyTracking* pMoney) {
    f32 fMult;
    s32 nTotal;

    if (fn_800E177C() != 0) return 0;
    fMult = 1.0f;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 0) {
        switch (gpSaveData[gPlayers[nPlayer].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +0] / 100.0f;
            break;
        case 2:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +1] / 100.0f;
            break;
        case 3:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +2] / 100.0f;
            break;
        case 4:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +3] / 100.0f;
            break;
        case 5:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +4] / 100.0f;
            break;
        case 6:
            fMult = (f32)lbl_80200538.aMult[EARN_MULT_TOUR +5] / 100.0f;
            break;
        }
    }
    nTotal = (s32)((f32)nReward * fMult);
    nTotal = (s32)((12.5f + (f32)nTotal) / 25.0f) * 25;
    if (pMoney != NULL) {
        pMoney->n24 = nTotal;
        pMoney->n0 = nTotal;
        pMoney->nTourCard = nTotal - nReward;
    }
    return nTotal;
}

// Whether a human player's profile can earn awards.
u8 fn_800D748C(int nPlayer) {
    if (Player_IsCPU(nPlayer)) return 0;
    if (fn_800E177C() != 0) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    return 1;
}

// Give a player award nAward if they do not have it yet. Five awards also keep the shot's replay.
u8 fn_800D750C(int nPlayer, int nAward) {
    int nProfile;
    int nSlot;

    if (fn_800E177C() != 0) return 0;
    if (fn_800D76AC(nPlayer, nAward)) {
        nProfile = gPlayers[nPlayer].nIndex;
        if (gpSaveData[nProfile].bActive != 1) return 0;
        fn_800D7770(nPlayer, &gpSaveData[nProfile].aAward[nAward]);
        gpSaveData[nProfile].b70 = 1;
        nSlot = 5;
        if (nAward == 0) {
            nSlot = 0;
        } else if (nAward == 6) {
            nSlot = 1;
        } else if (nAward == 9) {
            nSlot = 2;
        } else if (nAward == 3) {
            nSlot = 3;
        } else if (nAward == 13) {
            nSlot = 4;
        }
        if (nSlot != 5 && gReplayData.bF10) {
            Mem_cpy(gpSaveData[nProfile].aReplay[nSlot], &gReplayData, sizeof(gpSaveData->aReplay[0]));
        }
        return 1;
    }
    return 0;
}

s32 fn_800D7660(int nPlayer, Ball* pBall, u8 b) {
    fn_800D477C(nPlayer, pBall, b);
    return fn_800D9954();
}

s32 fn_800D7684(int nPlayer, Ball* pBall, u8 b) {
    fn_800D4F14(nPlayer, b);
    return fn_800D9954();
}

// Whether a human player can still win award nAward (0..38).
u8 fn_800D76AC(int nPlayer, int nAward) {
    if (fn_800E177C() != 0) return 0;
    if (nAward == 39) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    return gpSaveData[gPlayers[nPlayer].nIndex].aAward[nAward].bWon != 1;
}

// Mark an award won, with today's date; 1 if it was not won before.
u8 fn_800D7770(int nPlayer, Award* pAward) {
    if (fn_800E177C() != 0) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    if (pAward->bWon) return 0;
    pAward->bWon = 1;
    pAward->nDate = fn_800D2994();
    return 1;
}

// The record checks after a shot share this start: nothing in some modes and states, or for a
// CPU player or one without a profile; the record holder's name is the profile's, or "User <n>"
// when the front end has no profile loaded in that slot.

// Whether a drive (from the tee of a par 4 or 5, off the fairway class 1 surface, in bounds) sets a
// record (kind 1) of its distance. With bAll only a new best (fn_800D8750 gives 2 or 4) counts,
// else any place; a hit goes into lbl_80200498/lbl_80200510. bCountStroke counts the shot on the
// hole while it is checked. Returns how many hit.
int fn_800D782C(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    int nPar;
    u32 nClass;
    int nResult;

    lbl_80282258 = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12 || fn_800E39F0() || Game_GetMode() == 22 || Game_GetMode() == 26) return 0;
    if (gpGame->b137) return 0;
    if (fn_800E177C() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (lbl_801D7148.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = fn_80009680(fDx * fDx + fDz * fDz);
    if (pBall->pCourse == NULL) {
        if (bCountStroke) {
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
        }
        return 0;
    }
    nPar = fn_800D2B08();
    nClass = gSurfaceTypes[pBall->nStartSurface].nClass;
    if (fn_800D8DB4(1) && (nPar == 4 || nPar == 5) && nClass == 1 && !fn_800E2B40(nPlayer, pBall)) {
        nResult = fn_800D8750(1, (s32)fDist, a, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200498[lbl_80282258] = nResult;
            lbl_80200510[lbl_80282258] = 1;
            lbl_80282258++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return lbl_80282258;
}

// The same for a putt (the putter, club 25): record kind 2, its distance in feet; a hit goes into
// lbl_80200470/lbl_802004E8.
int fn_800D7B1C(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    int nResult;

    lbl_80282258 = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12 || fn_800E39F0() || Game_GetMode() == 22 || Game_GetMode() == 26) return 0;
    if (gpGame->b137) return 0;
    if (fn_800E177C() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (lbl_801D7148.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = fn_80009680(fDx * fDx + fDz * fDz);
    if (fn_800D8DB4(2) && gPlayers[nPlayer].nClub == 25) {
        nResult = fn_800D8750(2, (s32)(3.0f * fDist), a, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200470[lbl_80282258] = nResult;
            lbl_802004E8[lbl_80282258] = 2;
            lbl_80282258++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return lbl_80282258;
}

// After a hole, the round records as fn_800D782C checks the drive: mode 22's (kind 9), the
// fn_800E39F0 rounds' (kind 8), or the score (0), fn_800D1170's (3) and fn_800D0FBC's (5) counts,
// the holes under par (7) and at two under or better (6) and the putts (4). bCountStroke counts
// the hole one stroke more meanwhile; the results go to lbl_80200448/lbl_802004C0 as there.
int fn_800D7DA0(int nPlayer, int bSave, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    int nResult;
    int nValue;
    int i;
    int nEagles;
    int nPutts;

    lbl_80282258 = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12) return 0;
    if (fn_800E177C() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (lbl_801D7148.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (Game_GetMode() == 22) {
        if (fn_800D8DB4(9)) {
            nResult = fn_800D8750(9, gPlayers[nPlayer].nEBC, bSave, szName, nPlayer);
            if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
                lbl_80200448[lbl_80282258] = nResult;
                lbl_802004C0[lbl_80282258] = 9;
                lbl_80282258++;
            }
        }
        return lbl_80282258;
    }
    if (fn_800E39F0()) {
        if (fn_800D8DB4(8)) {
            nResult = fn_800D8750(8, gPlayers[nPlayer].nDD8, bSave, szName, nPlayer);
            if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
                lbl_80200448[lbl_80282258] = nResult;
                lbl_802004C0[lbl_80282258] = 8;
                lbl_80282258++;
            }
        }
        return lbl_80282258;
    }
    if (gpGame->b137) return 0;
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    nValue = fn_800E17AC(nPlayer);
    if (fn_800D8DB4(0)) {
        nResult = fn_800D8750(0, nValue, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 0;
            lbl_80282258++;
        }
    }
    if (fn_800D8DB4(3)) {
        nResult = fn_800D8750(3, fn_800D1170(nPlayer, 0), bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 3;
            lbl_80282258++;
        }
    }
    if (fn_800D8DB4(5)) {
        nResult = fn_800D8750(5, fn_800D0FBC(nPlayer), bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 5;
            lbl_80282258++;
        }
    }
    nValue = 0;
    for (i = 0; i < 18; i++) {
        if (gPlayers[nPlayer].nStrokes[i] <= fn_800D2AD8(i) - 1) {
            nValue++;
        }
    }
    if (fn_800D8DB4(7)) {
        nResult = fn_800D8750(7, nValue, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 7;
            lbl_80282258++;
        }
    }
    nEagles = 0;
    for (i = 0; i < 18; i++) {
        if (gPlayers[nPlayer].nStrokes[i] < fn_800D2AD8(i) - 1) {
            nEagles++;
        }
    }
    if (fn_800D8DB4(6)) {
        nResult = fn_800D8750(6, nEagles, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 6;
            lbl_80282258++;
        }
    }
    nPutts = 0;
    for (i = 0; i < 18; i++) {
        nPutts += gPlayers[nPlayer].nPutts[i];
    }
    if (fn_800D8DB4(4)) {
        nResult = fn_800D8750(4, nPutts, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            lbl_80200448[lbl_80282258] = nResult;
            lbl_802004C0[lbl_80282258] = 4;
            lbl_80282258++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return lbl_80282258;
}

// Whether nValue and szName are among the top five of course k's record i. Never on a round whose
// holes are not one course's 1..18.
u8 fn_800D8458(int i, int nValue, const char* szName, int k) {
    RecordEntry* pRec;
    int j;

    if (gpGame->b136) return 0;
    for (j = 0; j < 5; j++) {
        pRec = &gSession.aCourseRecord[k].aRecord[i][j];
        if (pRec->nValue == nValue && strcmp(pRec->szName, szName) == 0) {
            return 1;
        }
    }
    // EA bug: the same five entries are searched again
    for (j = 0; j < 5; j++) {
        pRec = &gSession.aCourseRecord[k].aRecord[i][j];
        if (pRec->nValue == nValue && strcmp(pRec->szName, szName) == 0) {
            return 1;
        }
    }
    return 0;
}

// Whether nValue and szName are among the top five of all-time record recB[k][i].
u8 fn_800D853C(int i, int nValue, const char* szName, int k) {
    RecordEntry* pRec;
    int j;

    for (j = 0; j < 5; j++) {
        pRec = &gSession.recB[k][i][j];
        if (pRec->nValue == nValue && strcmp(pRec->szName, szName) == 0) {
            return 1;
        }
    }
    return 0;
}

// The same for recC[k][i].
u8 fn_800D85DC(int i, int nValue, const char* szName, int k) {
    RecordEntry* pRec;
    int j;

    for (j = 0; j < 5; j++) {
        pRec = &gSession.recC[k][i][j];
        if (pRec->nValue == nValue && strcmp(pRec->szName, szName) == 0) {
            return 1;
        }
    }
    return 0;
}

// Whether nValue breaks record kind nKind's nRecord: kinds 0 and 4 go low, 1..9 high.
u8 fn_800D867C(int nKind, int nValue, int nRecord) {
    switch (nKind) {
    case 0:
    case 4:
        return nValue <= nRecord;
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        return nRecord <= nValue;
    }
    return 0;
}

s32 fn_800D86DC(s32 n) {
    switch (n) {
    case 16:
        return 0;
    case 17:
        return 1;
    case 13:
        return 2;
    default:
        return 3;
    }
}

s32 fn_800D8720(s32 n) {
    switch (n) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

// Offer nValue and szName to record kind nKind: kinds 0..7 to the course's records and the
// all-time recA, kind 8 to recB (by game mode), the rest to recC. Returns 1 for a place in the
// course (or recB/recC) top five, 2 for its best, 3 and 4 the same for recA. With bSave the entry
// is written in (the ones below move down) and the player's profile gets b70.
// EA passes szName to sprintf as the format. The callers pass bSave unmasked and this function
// tests its low byte (the (u8) casts): EA's definition took a u8 behind an int prototype.
int fn_800D8750(int nKind, int nValue, int bSave, const char* szName, int nPlayer) {
    int nResult;
    int nPos;
    int j;
    int n;
    RecordEntry* pFrom;
    RecordEntry* pTo;

    nResult = 0;
    if (gpGame->b136) return 0;
    if (nKind < 8) {
        if (fn_800D867C(nKind, nValue, gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][4].nValue)) {
            nResult = 1;
            nPos = 4;
            for (j = 3; j >= 0; j--) {
                if (fn_800D867C(nKind, nValue,
                                gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][j].nValue)) {
                    nPos = j;
                }
            }
            if (nPos == 0) {
                nResult = 2;
            }
            if ((u8)bSave) {
                for (j = 4; j > nPos; j--) {
                    pFrom = &gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][j - 1];
                    pTo = &gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][j];
                    pTo->nValue = pFrom->nValue;
                    sprintf(pTo->szName, pFrom->szName);
                }
                pTo = &gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][nPos];
                pTo->nValue = nValue;
                sprintf(pTo->szName, szName);
            }
        }
        if (fn_800D867C(nKind, nValue, gSession.recA[nKind][4].nValue)) {
            nResult = 3;
            nPos = 4;
            for (j = 3; j >= 0; j--) {
                if (fn_800D867C(nKind, nValue, gSession.recA[nKind][j].nValue)) {
                    nPos = j;
                }
            }
            if (nPos == 0) {
                nResult = 4;
            }
            if ((u8)bSave) {
                for (j = 4; j > nPos; j--) {
                    pFrom = &gSession.recA[nKind][j - 1];
                    pTo = &gSession.recA[nKind][j];
                    pTo->nValue = pFrom->nValue;
                    sprintf(pTo->szName, pFrom->szName);
                }
                pTo = &gSession.recA[nKind][nPos];
                pTo->nValue = nValue;
                sprintf(pTo->szName, szName);
            }
        }
    } else if (nKind == 8) {
        n = fn_800D86DC(Game_GetMode());
        if (n != 3) {
            if (fn_800D867C(nKind, nValue, gSession.recB[fn_80015464()][n][4].nValue)) {
                nResult = 1;
                nPos = 4;
                for (j = 3; j >= 0; j--) {
                    if (fn_800D867C(nKind, nValue, gSession.recB[fn_80015464()][n][j].nValue)) {
                        nPos = j;
                    }
                }
                if (nPos == 0) {
                    nResult = 2;
                }
                if ((u8)bSave) {
                    for (j = 4; j > nPos; j--) {
                        pFrom = &gSession.recB[fn_80015464()][n][j - 1];
                        pTo = &gSession.recB[fn_80015464()][n][j];
                        pTo->nValue = pFrom->nValue;
                        sprintf(pTo->szName, pFrom->szName);
                    }
                    pTo = &gSession.recB[fn_80015464()][n][nPos];
                    pTo->nValue = nValue;
                    sprintf(pTo->szName, szName);
                }
            }
        }
    } else {
        n = fn_800D8720(fn_80126FA0());
        if (n != 2) {
            if (fn_800D867C(nKind, nValue, gSession.recC[fn_80127098(fn_80015464())][n][4].nValue)) {
                nResult = 1;
                nPos = 4;
                for (j = 3; j >= 0; j--) {
                    if (fn_800D867C(nKind, nValue, gSession.recC[fn_80127098(fn_80015464())][n][j].nValue)) {
                        nPos = j;
                    }
                }
                if (nPos == 0) {
                    nResult = 2;
                }
                if ((u8)bSave) {
                    for (j = 4; j > nPos; j--) {
                        pFrom = &gSession.recC[fn_80127098(fn_80015464())][n][j - 1];
                        pTo = &gSession.recC[fn_80127098(fn_80015464())][n][j];
                        pTo->nValue = pFrom->nValue;
                        sprintf(pTo->szName, pFrom->szName);
                    }
                    pTo = &gSession.recC[fn_80127098(fn_80015464())][n][nPos];
                    pTo->nValue = nValue;
                    sprintf(pTo->szName, szName);
                }
            }
        }
    }
    if (nPlayer != 5 && nResult != 0 && (u8)bSave) {
        if (gpSaveData[gPlayers[nPlayer].nIndex].bActive) {
            gpSaveData[gPlayers[nPlayer].nIndex].b70 = 1;
        }
    }
    return nResult;
}

// Clear the player's flags b30C..b30F (Swing.c calls it).
void fn_800D8D10(int nPlayer) {
    gPlayers[nPlayer].b30C = 0;
    gPlayers[nPlayer].b30D = 0;
    gPlayers[nPlayer].b30F = 0;
    gPlayers[nPlayer].b30E = 0;
}

// Clear the flags fn_800D9350 sets.
void fn_800D8D38(int nPlayer) {
    gPlayers[nPlayer].b310 = 0;
    gPlayers[nPlayer].b311 = 0;
    gPlayers[nPlayer].b312 = 0;
}

// Clear the player's money breakdown for the round (GameRound.c, as a round is set up).
void fn_800D8D5C(int nPlayer) {
    gPlayers[nPlayer].money.n0 = 0;
    gPlayers[nPlayer].money.n4 = 0;
    gPlayers[nPlayer].money.n8 = 0;
    gPlayers[nPlayer].money.nC = 0;
    gPlayers[nPlayer].money.n10 = 0;
    gPlayers[nPlayer].money.n14 = 0;
    gPlayers[nPlayer].money.n18 = 0;
    gPlayers[nPlayer].money.n1C = 0;
    gPlayers[nPlayer].money.nBase = 0;
    gPlayers[nPlayer].money.nCourse = 0;
    gPlayers[nPlayer].money.n2C = 0;
    gPlayers[nPlayer].money.nTee = 0;
    gPlayers[nPlayer].money.nTourCard = 0;
    gPlayers[nPlayer].money.n38 = 0;
    gPlayers[nPlayer].money.n3C = 0;
    gPlayers[nPlayer].money.n24 = 0;
}

// Whether goals of kind nKind count now: never in modes 9 and 11 or when fn_800E177C says so;
// kinds 0 and 3..7 need fn_800EC550 off, a round of every hole and fn_8008AB40 off, kinds 1 and 2
// only fn_800EC550 off, kind 8 fn_800E39F0, kind 9 game mode 22.
u8 fn_800D8DB4(int nKind) {
    if (Game_GetMode() == 9 || Game_GetMode() == 11) return 0;
    if (fn_800E177C() != 0) return 0;
    switch (nKind) {
    case 0:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 1:
        if (fn_800EC550()) return 0;
        return 1;
    case 2:
        if (fn_800EC550()) return 0;
        return 1;
    case 3:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 4:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 5:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 6:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 7:
        if (fn_800EC550() || !fn_800E1BBC() || fn_8008AB40()) return 0;
        return 1;
    case 8:
        if (fn_800EC550() || !fn_800E39F0()) return 0;
        return 1;
    case 9:
        if (fn_800EC550() || Game_GetMode() != 22) return 0;
        return 1;
    }
    return 1;
}

// After a shot that stayed in bounds (GM_PlayerTookShot), when gpGame->b27B allows it: the shot's
// statistics. A drive (the first stroke of a par 4 or 5, off class-1 ground) can be the round's
// and the profile's longest and is counted in the profile; the first stroke's distance goes in
// nC24; b2E4 marks a first stroke on a par 4 or 5 that finished on the fairway, the green or in the
// hole, b2F6 a ball on the green or in the hole in par - 2 strokes or fewer. A putt (club 25) that
// fn_800E2DB4 accepts can be the longest, in feet.
void fn_800D8FE4(int nPlayer) {
    u32 nClass;
    Ball* pBall;
    int nPar;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    int nLie;
    int nStrokes;
    int nProfile;
    SurfaceType* pSurface;
    int nPutt;

    if (!gpGame->b27B) return;
    if (fn_800E177C() != 0) return;
    pBall = &gPlayers[nPlayer].ball;
    nPar = fn_800D2B08();
    nClass = gSurfaceTypes[pBall->nStartSurface].nClass;
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = fn_80009680(fDx * fDx + fDz * fDz);
    nLie = pBall->nLie;
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    pSurface = Ter_GetSupportingWorldMaterial(pBall->pCourse, gPlayers[nPlayer].vBall);
    nProfile = gPlayers[nPlayer].nIndex;
    if ((nPar == 4 || nPar == 5) && nStrokes == 1 && nClass == 1) {
        if (fDist > gPlayers[nPlayer].n2DC) {
            gPlayers[nPlayer].n2DC = fDist;
        }
        if (gpSaveData[nProfile].bActive && fDist > gpSaveData[nProfile].nA0) {
            gpSaveData[nProfile].nA0 = fDist;
            gpSaveData[nProfile].b70 = 1;
        }
    }
    if (nStrokes == 1) {
        gPlayers[nPlayer].nC24 = fDist;
    }
    if ((nPar == 4 || nPar == 5) && nStrokes == 1 &&
        (nLie == LIE_FAIRWAY_e || nLie == LIE_GREEN_e || nLie == LIE_INCUP_e)) {
        gPlayers[nPlayer].b2E4[Game_CurHoleIndex()] = 1;
    }
    if (pSurface != NULL && pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
        nStrokes <= nPar - 2) {
        gPlayers[nPlayer].b2F6[Game_CurHoleIndex()] = 1;
    }
    if (gpSaveData[nProfile].bActive && (nPar == 4 || nPar == 5) && nStrokes == 1 && nClass == 1) {
        gpSaveData[nProfile].n88++;
        gpSaveData[nProfile].n8C += (s32)fDist;
        gpSaveData[nProfile].b70 = 1;
    }
    if (fn_800E2DB4(nPlayer) && gPlayers[nPlayer].nClub == 25) {
        nPutt = 3.0f * gPlayers[nPlayer].fA64;
        if (nPutt > gPlayers[nPlayer].n2E0) {
            gPlayers[nPlayer].n2E0 = nPutt;
        }
        if (gpSaveData[nProfile].bActive && nPutt > gpSaveData[nProfile].nA4) {
            gpSaveData[nProfile].nA4 = nPutt;
        }
    }
}

// After every shot (GM_PlayerTookShot calls it last), when the mode allows no mulligans: carry the
// flags over, and set bit 0 of n308 when the hole's first stroke on a par 4 or 5 finished on the
// green or in the hole.
void fn_800D9350(int nPlayer) {
    int nLie;
    int nStrokes;
    int nPar;

    if (fn_800E177C() == 0) {
        nLie = gPlayers[nPlayer].ball.nLie;
        if (gPlayers[nPlayer].b30F) {
            gPlayers[nPlayer].b310 = 1;
        }
        if (gPlayers[nPlayer].b30E) {
            gPlayers[nPlayer].b311 = 1;
            gPlayers[nPlayer].n308 |= 2;
        }
        if (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) {
            gPlayers[nPlayer].b312 = 1;
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
        nPar = fn_800D2B08();
        if (nStrokes == 1 && nPar >= 4 && gPlayers[nPlayer].b312) {
            gPlayers[nPlayer].n308 |= 1;
        }
    }
}

// The player has finished the current hole: mark it, and add the hole to the statistics of the
// player's profile (putts, the per-hole flags b2E4 and b2F6, the score against par), recording the
// date of an eagle or better on a par 5 when the hole is a marked one.
void fn_800D9458(int nPlayer) {
    int nProfile;
    int nPar;
    int nDiff;
    int nDate;
    int nMarked;
    f32 dx;
    f32 dz;
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;

    gpGame->b16C[nPlayer][Game_CurHoleIndex()] = 1;
    if (gpGame->b27C) {
        switch (fn_800EC550()) {    // fake match: as in fn_800D9834
        case 0:
            break;
        default:
            return;
        }
        if (fn_800E177C() == 0) {
            // How far the ball ended from vBall; the result is not used.
            dx = gPlayers[nPlayer].ball.vPos[0] - gPlayers[nPlayer].vBall[0];
            dz = gPlayers[nPlayer].ball.vPos[2] - gPlayers[nPlayer].vBall[2];
            fn_80009680(dx * dx + dz * dz);
            nProfile = gPlayers[nPlayer].nIndex;
            nPar = fn_800D2B08();
            if (gpSaveData[nProfile].bActive) {
                gpSaveData[nProfile].b70 = 1;
            } else {
                return;
            }
            if (nPar == 4 || nPar == 5) {
                if (gPlayers[nPlayer].b2E4[Game_CurHoleIndex()]) {
                    gpSaveData[nProfile].n90++;
                    gpSaveData[nProfile].n94++;
                } else {
                    gpSaveData[nProfile].n90++;
                }
            }
            if (gPlayers[nPlayer].b2F6[Game_CurHoleIndex()]) {
                gpSaveData[nProfile].n98++;
                gpSaveData[nProfile].n9C++;
            } else {
                gpSaveData[nProfile].n98++;
            }
            if (gPlayers[nPlayer].nPutts[Game_CurHoleIndex()] < 10) {
                gpSaveData[nProfile].n80++;
                gpSaveData[nProfile].n84 += gPlayers[nPlayer].nPutts[Game_CurHoleIndex()];
            }
            nDiff = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] - fn_800D2B08();
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1) {
                gpSaveData[nProfile].nAC++;
            } else if (nDiff == -3) {
                gpSaveData[nProfile].nB0++;
            } else if (nDiff == -2) {
                gpSaveData[nProfile].nB4++;
            } else if (nDiff == -1) {
                gpSaveData[nProfile].nB8++;
            } else if (nDiff == 0) {
                gpSaveData[nProfile].nBC++;
            } else if (nDiff == 1) {
                gpSaveData[nProfile].nC0++;
            } else if (nDiff > 1) {
                gpSaveData[nProfile].nC4++;
            }
            if (gpSaveData[nProfile].bActive && nPar == 5 &&
                gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] <= 3) {
                nMarked = fn_800E1CE8(gpGame->nCurCourse, fn_80015464());
                fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
                nDate = fn_80078604(nMonth, nDay, nYear);
                if (nMarked != -1) {
                    fn_8005897C(&gpSaveData[nProfile], 0, nMarked, 1);
                    fn_8005897C(&gpSaveData[nProfile], 1, nMarked, nDate);
                }
            }
        }
    }
}

// At the end of a round: count it in the profile (a full round; in stroke play also its strokes and
// the best score).
void fn_800D9834(int nPlayer) {
    int nProfile;
    int nStrokes;

    if (gpGame->b27D) {
        switch (fn_800EC550()) {    // fake match: a plain "if (...) return" folds the branch over a branch
        case 0:
            break;
        default:
            return;
        }
        if (fn_800E177C() == 0) {
            nProfile = gPlayers[nPlayer].nIndex;
            if (gpSaveData[nProfile].bActive) {
                if (fn_800E1BBC()) {
                    gpSaveData[nProfile].n7C++;
                    if (gpGame->n4 == 0) {
                        nStrokes = fn_800E17AC(nPlayer);
                        gpSaveData[nProfile].n74++;
                        gpSaveData[nProfile].n78 += nStrokes;
                        if (gpSaveData[nProfile].nA8 == 0) {
                            gpSaveData[nProfile].nA8 = nStrokes;
                        } else if (nStrokes < gpSaveData[nProfile].nA8) {
                            gpSaveData[nProfile].nA8 = nStrokes;
                        }
                    }
                }
                gpSaveData[nProfile].b70 = 1;
            }
        }
    }
}

s32 fn_800D9954(void) {
    return lbl_80282250;
}

s32 fn_800D995C(s32 i) {
    return lbl_802002B8[i];
}

s32 fn_800D9970(s32 i) {
    return lbl_80200290[i];
}

s32 fn_800D9984(s32 i) {
    return lbl_80200268[i];
}

// Whether the profile has earned award nAward (23..38, the PGA TOUR and career awards); others
// give 0. Most need the season's last round and at least 15 tournaments started.
u8 fn_800D9998(int nPlayer, int nAward) {
    SaveProfile* pProfile;
    u8 bSeasonEnd;
    u8 bFullSeason;
    int i;
    int nLeads;

    if (fn_800E177C() != 0) return 0;
    pProfile = &gpSaveData[nPlayer];
    if (!pProfile->bActive) return 0;
    if ((pProfile->tour.nEvent == -1 || GameModeDriverPGATour_GetNextEvent() == -1) &&
        pProfile->tour.nRound + 1 == GameModeDriverPGATour_GetRounds(pProfile->tour.nEvent)) {
        bSeasonEnd = 1;
    } else {
        bSeasonEnd = 0;
    }
    bFullSeason = pProfile->tour.aStats[PGA_USER_GOLFER].nEvents >= 15;

    switch (nAward) {
    case 23:
        return pProfile->nAC > 18;
    case 24:
        // Top 25 in every tournament played.
        if (bSeasonEnd && bFullSeason) {
            for (i = 0; i < fn_800EF834(); i++) {
                if (pProfile->tour.aEvent[i].nUserRankType == 0) continue;
                if (pProfile->tour.aEvent[i].nUserRankType == 2 &&
                    pProfile->tour.aEvent[i].nUserRank <= 25) continue;
                return 0;
            }
            return 1;
        }
        return 0;
    case 25:
        // Leads the tour in 15 statistics.
        if (bSeasonEnd && bFullSeason) {
            nLeads = 0;
            for (i = 0; i < 28; i++) {
                if (fn_80118F60(nPlayer, PGA_USER_GOLFER, i)) {
                    nLeads++;
                }
            }
            if (nLeads >= 15) return 1;
        }
        return 0;
    case 26:
        if (bSeasonEnd && bFullSeason && fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR3BIRDS) &&
            fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR4BIRDS) &&
            fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR5BIRDS)) {
            return 1;
        }
        return 0;
    case 27:
        if (bSeasonEnd && bFullSeason &&
            GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, PGA_USER_GOLFER,
                                                   GM_PGA_STAT_BIRDIESPERROUND) > 4.25f) {
            return 1;
        }
        return 0;
    case 28:
        if (fn_800ED6F0() && fn_800EC558() == 0) return 1;
        return 0;
    case 29:
        return fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_CAREER_WINNINGS) != 0;
    case 30:
        // Under par in every tournament played.
        if (bSeasonEnd && bFullSeason) {
            for (i = 0; i < fn_800EF834(); i++) {
                if (pProfile->tour.aEvent[i].nUserRankType == 0) continue;
                if (pProfile->tour.aEvent[i].nUserRankType == 2 &&
                    pProfile->tour.aEvent[i].nUserScore < pProfile->tour.aEvent[i].nEventPar) continue;
                return 0;
            }
            return 1;
        }
        return 0;
    case 31:
        return pProfile->tour.n4E96 > 11;
    case 32:
        return fn_800E17AC(nPlayer) < 59;
    case 33:
        if (bSeasonEnd && bFullSeason &&
            GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SCORING) < 68.17f) {
            return 1;
        }
        return 0;
    case 34:
        if (pProfile->tour.n4E94 > 100 &&
            (f32)pProfile->tour.aStats[PGA_USER_GOLFER].nCareerWins / (f32)pProfile->tour.n4E94 > 0.28f) {
            return 1;
        }
        return 0;
    case 35:
        return pProfile->tour.n4E98 > 66;
    case 36:
        return pProfile->tour.n4E9A > 18;
    case 37:
        return pProfile->tour.aStats[PGA_USER_GOLFER].nSeasonWins > 9;
    case 38:
        // Tiger Woods's season record, $9,188,321 in 2000.
        return pProfile->tour.aStats[PGA_USER_GOLFER].nSeasonWinnings > 9188321;
    }
    return 0;
}

s32 fn_800D9E00(s32 i) {
    return lbl_80191A08[i];
}
