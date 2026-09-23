// Earnings.c (TW06's earnings.c): the money and goals kept in the save profiles. It loads the
// prize table (stream 'ERN '), rounds payouts to $25, pays the tournament and match prizes, scales
// a payout by the bonus, course, tee, pin and TOUR card multipliers, checks the goals that unlock
// things, and keeps the saved replays. The GM_Earnings_ functions of TW06 come in the same order.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// The prize table (stream 'ERN ', 0x22F0 bytes); multipliers are percentages (100 = x1).
typedef struct StrokePrize {
    s32  nBase;                 // the prize for a win
    s32  nPerStroke;            // and for each stroke of the margin, up to 5
} StrokePrize;
typedef struct SkinsValue {
    s32  aValue[4];             // holes 1..6, 7..12, 13..17, 18
    s32  n10;
} SkinsValue;
typedef struct EarningsTable {
    u8   unk0[0x1D4];
    StrokePrize aStrokePrize[26];   // 0x1D4  per earnings rating of the beaten golfer
    SkinsValue aSkins[26];      // 0x2A4  a skin's value, per the best earnings rating in the game
    u8   unk4AC[0x980 - 0x4AC];
    s32  aTeePct[3];            // 0x980  the tee multiplier, as [2 - nTeeSet] (tee set 3 pays as 1)
    s32  a98C[4];               // 0x98C  the multiplier for the hole's gpGame->holeOrder value 0..3
    s32  aTourPct[6];           // 0x99C  the TOUR card multiplier per level 1..6 (level 0 pays as 1)
    u8   unk9B4[0x22F0 - 0x9B4];
} EarningsTable;
extern EarningsTable lbl_80200538;

// A save profile (0x10600 bytes each); only what this file reads.
typedef struct Profile {
    u8   b0;                    // 0x00000  payouts are scaled (and awards given, at 1) only when set
    u8   unk1[0x5000 - 0x1];
    s32  n5000;                 // 0x05000  the TOUR card level, 0..6
    u8   unk5004[0x10600 - 0x5004];
} Profile;
extern Profile* gpSaveData;

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
extern u8  lbl_801FFAE8[0x280];
extern u8  lbl_801FFD90[0x280];
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)
extern s32 lbl_80282248;
extern s32 lbl_8028224C;
extern s32 lbl_80282250;
extern s32 lbl_80282254;
extern s32 lbl_80191A08[39];

void* memcpy(void* pDst, const void* pSrc, u32 uLen);

void  fn_800D344C(UStreamObject* pObject);
int   fn_800584DC(int nProfile);
int   fn_801020C0(void);

int   fn_800D3CF8(int nRating);
s32   fn_800D477C(int nPlayer, u8* pBall, u8 b);
void  fn_800D4F14(int nPlayer, u8 b);
f32   fn_800D6EEC(void);
s32   fn_800D9954(void);

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

// TW06: roundToNearest25. Every payout is a multiple of $25.
s32 fn_800D33A8(s32 nMoney) {
    return (s32)((12.5f + (f32)nMoney) / 25.0f) * 25;
}

// TW06: GM_Earnings_FreeStreamMemory.
void fn_800D33F0(void) {
}

// TW06: GM_Earnings_RegisterStreamClients. The prize table comes from the stream.
void fn_800D33F4(void) {
    UStream_RegisterHandler('ERN ', fn_800D344C);
}

// TW06: GM_Earnings_UnRegisterStreamClients.
void fn_800D3424(void) {
    UStream_UnregisterHandler('ERN ');
}

void fn_800D344C(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_80200538), &lbl_80200538);
}

// TW06: GM_Earnings_GetStrokeWinnings. Beating a CPU golfer pays by their earnings rating: a base
// prize and so much a stroke of the margin (at most 5). *pPrize gets the base.
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

// TW06: GM_Earnings_GetStrokeWinningsTeam. The same for a team (0: players 0 and 1, 1: players 2 and
// 3) beating a CPU team: the average of what the two losers would pay.
// 99.2%: only the order of the four table loads in the return differs. Tried: all 24 orders of the
// flat sum, the grouped forms, an inline per-golfer helper; the nBase locals took it from 82% to 98%.
int fn_800D37BC(int nWinner, int nLoser, int nMargin, int* pPrize) {
    int nFirst;
    int nSecond;
    int nRating1;
    int nRating2;
    int nBase1;
    int nBase2;

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
    return (lbl_80200538.aStrokePrize[nRating2].nBase +
            (lbl_80200538.aStrokePrize[nRating2].nPerStroke * nMargin +
             (lbl_80200538.aStrokePrize[nRating1].nBase +
              lbl_80200538.aStrokePrize[nRating1].nPerStroke * nMargin))) / 2;
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

// TW06: GM_GetHighestRatedGolfer. The best earnings rating among the players.
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

// TW06: GM_Earnings_RateGolfer. A CPU plays at its golfer's rating; a human's comes from the profile.
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

    nRating = fn_801020C0();
    if (nGolfer >= FIRST_CREATED_GOLFER) {
        return fn_800D3CF8(nRating);
    }
    return gGolferTable[nGolfer].nEarningsRating;
}

// TW06: GM_Earnings_GetSkinsHoleValue. What a skin on hole nHole (0..17) is worth.
s32 fn_800D3D64(int nRating, int nHole) {
    if (nHole < 6) return lbl_80200538.aSkins[nRating].aValue[0];
    if (nHole < 12) return lbl_80200538.aSkins[nRating].aValue[1];
    if (nHole < 17) return lbl_80200538.aSkins[nRating].aValue[2];
    return lbl_80200538.aSkins[nRating].aValue[3];
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

// Whether bit nBit of uMask is set.
u8 fn_800D4EF8(u32 uMask, int nBit) {
    return (uMask & (1 << nBit)) != 0;
}

// TW06: GM_Earnings_ComputeBonusModifiers. The points, rounded to $25, earn a bonus on top for the
// course, the tees played and the hole's gpGame->holeOrder value (each flag switches one on).
// Each part is rounded to $25 by itself; the total is at least 0.
s32 fn_800D6A70(s32 nPoints, int nPlayer, u8 bCourse, u8 bTee, u8 bHole, CourseMoneyTracking* pMoney) {
    f32 fCourseBonus;
    f32 fTeeBonus;
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
        fTee = (f32)lbl_80200538.aTeePct[2] / 100.0f;
        break;
    case 1:
        fTee = (f32)lbl_80200538.aTeePct[1] / 100.0f;
        break;
    case 2:
        fTee = (f32)lbl_80200538.aTeePct[0] / 100.0f;
        break;
    case 3:
        fTee = 1.0f;
        break;
    }
    switch (gpGame->holeOrder[Game_CurHoleIndex()]) {
    case 0:
        fHole = (f32)lbl_80200538.a98C[0] / 100.0f;
        break;
    case 1:
        fHole = (f32)lbl_80200538.a98C[1] / 100.0f;
        break;
    case 2:
        fHole = (f32)lbl_80200538.a98C[2] / 100.0f;
        break;
    case 3:
        fHole = (f32)lbl_80200538.a98C[3] / 100.0f;
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

// TW06: GM_Earnings_ComputeTOURCardModifiers. The TOUR card level raises the payout; the extra
// goes in the breakdown. Nothing is paid when fn_800E177C says so.
int fn_800D7220(int nReward, int nPlayer, CourseMoneyTracking* pMoney) {
    f32 fMult;
    s32 nTotal;

    if (fn_800E177C() != 0) return 0;
    fMult = 1.0f;
    if (gpSaveData[gPlayers[nPlayer].nIndex].b0 != 0) {
        switch (gpSaveData[gPlayers[nPlayer].nIndex].n5000) {
        case 0:
        case 1:
            fMult = (f32)lbl_80200538.aTourPct[0] / 100.0f;
            break;
        case 2:
            fMult = (f32)lbl_80200538.aTourPct[1] / 100.0f;
            break;
        case 3:
            fMult = (f32)lbl_80200538.aTourPct[2] / 100.0f;
            break;
        case 4:
            fMult = (f32)lbl_80200538.aTourPct[3] / 100.0f;
            break;
        case 5:
            fMult = (f32)lbl_80200538.aTourPct[4] / 100.0f;
            break;
        case 6:
            fMult = (f32)lbl_80200538.aTourPct[5] / 100.0f;
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

s32 fn_800D7660(int nPlayer, u8* pBall, u8 b) {
    fn_800D477C(nPlayer, pBall, b);
    return fn_800D9954();
}

s32 fn_800D7684(int nPlayer, int a, u8 b) {
    fn_800D4F14(nPlayer, b);
    return fn_800D9954();
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

s32 fn_800D9E00(s32 i) {
    return lbl_80191A08[i];
}
