// Earnings.c (EA's name, from the TW2003 source tree: Golf/GameMode/Earnings.c): the money,
// awards, records and statistics kept in the save profiles. It loads the prize table (stream
// 'ERN ', gEarningsTable), rounds payouts to $25, pays the tournament and match prizes, scales a
// payout by the course, tee, pin set and TOUR card multipliers, checks the shot, putt and hole
// goals that pay prizes and give awards (trophy balls, some with the shot's saved replay), offers
// a player's results to the high-score records (TW07 moved those checks to HighScoreRecords.cpp)
// and adds each shot, hole and round to the profile's statistics. The GM_Earnings_ functions of
// TW06 and TW07 come in the same order.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"
#include "frontend/fe.h"
#include "core/easb.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

// .bss, reverse address order (the ones not declared here are in game/earnings.h)
EarningsTable gEarningsTable;
// What the end-of-shot, end-of-hole and end-of-round record checks listed (gNumRecordHits of
// them): the record kind and the place HighScoreRecords_CheckRecord gave it (1..4).
s32 gShotRecordKinds[10];
s32 gPuttRecordKinds[10];
s32 gRoundRecordKinds[10];
s32 gShotRecordResults[10];
s32 gPuttRecordResults[10];
s32 gRoundRecordResults[10];
// The working lists the shot, putt and hole goal checks fill, ten entries each: per money prize
// (gNumPrizes) its message, its payout after the multipliers and its base before them; per award
// (gNumAwards) its id and the money paid with it. Then the gPay copies GM_Earnings_CopyGoalResults
// makes of them, which the payouts read.
s32 gShotPrizeMsgs[10];
s32 gPuttPrizeMsgs[10];
s32 gHolePrizeMsgs[10];
s32 gShotPrizes[10];
s32 gPuttPrizes[10];
s32 gHolePrizes[10];
s32 gShotPrizeBases[10];
s32 gPuttPrizeBases[10];
s32 gHolePrizeBases[10];
s32 gShotAwards[10];
s32 gPuttAwards[10];
s32 gHoleAwards[10];
s32 gShotAwardMoney[10];
s32 gPuttAwardMoney[10];
s32 gHoleAwardMoney[10];
s32 gPayShotPrizeMsgs[10];
s32 gPayPuttPrizeMsgs[10];
s32 gPayHolePrizeMsgs[10];
s32 gPayShotPrizes[10];
s32 gPayPuttPrizes[10];
s32 gPayHolePrizes[10];
s32 gPayShotAwards[10];
s32 gPayPuttAwards[10];
s32 gPayHoleAwards[10];
s32 gPayShotAwardMoney[10];
s32 gPayPuttAwardMoney[10];
s32 gPayHoleAwardMoney[10];
CourseMoneyTracking gPayPrizeBreakdowns[10];     // the breakdown of each gPay...Prizes payout
s32 gUnlockedCourses[10];       // the courses GM_Earnings_CheckUnlockCourses unlocked, for their messages
CourseMoneyTracking gPrizeBreakdowns[10];        // the breakdown of each prize the goal checks list

// .sbss, reverse address order (gNumRecordHits is in game/earnings.h)
s32 gNumRecordHits;
s32 gNumPrizes;
s32 gNumAwards;
s32 gPayNumPrizes;
s32 gPayNumAwards;

// Per award 0..38, its message index (Earnings_GetAwardMessageId).
s32 gAwardMessageIds[39] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 0,
    10, 3, 11, 12, 5, 13, 6, 14, 4, 15, 8, 16,
    9, 17, 2,
};

// Per finishing place 1..70, its share of a tournament's purse (GM_Earnings_TournamentPayout).
f32 gTournamentPayoutShares[70] = {
    0.18f, 0.108f, 0.068f, 0.048f, 0.04f, 0.036f, 0.0335f, 0.031f, 0.029f, 0.027f,
    0.025f, 0.023f, 0.021f, 0.019f, 0.018f, 0.017f, 0.016f, 0.015f, 0.014f, 0.013f,
    0.012f, 0.0112f, 0.0104f, 0.0096f, 0.0088f, 0.008f, 0.0077f, 0.0074f, 0.0071f, 0.0068f,
    0.0065f, 0.0062f, 0.0059f, 0.00565f, 0.0054f, 0.00515f, 0.0049f, 0.0047f, 0.0045f, 0.0043f,
    0.0041f, 0.0039f, 0.0037f, 0.0035f, 0.0033f, 0.0031f, 0.0029f, 0.00274f, 0.0026f, 0.00252f,
    0.00246f, 0.0024f, 0.00236f, 0.00232f, 0.0023f, 0.00228f, 0.00226f, 0.00224f, 0.00222f, 0.0022f,
    0.00218f, 0.00216f, 0.00214f, 0.00212f, 0.0021f, 0.00208f, 0.00206f, 0.00204f, 0.00202f, 0.002f,
};


void  EarningsInfo_LoadERNFromStream(UStreamObject* pObject);
int   GameMode4_GetNumEventsWon(void);
int   GameMode4_GetCurrentEvent(void);                                // GameMode4: the current ladder event
f32   fn_800D04AC(int nPlayer);                         // HoleScore.c
u32   fn_800D0BAC(int nPlayer);                         // the class of the ground the shot left
u8    fn_800D0BF8(int nPlayer, u8 bUnder, u8 bAnyLie);
u8    fn_800D0D54(int nPlayer);
int   fn_800D0DC8(int nPlayer, int nToPar);
int   fn_800D0E74(int nPlayer);
int   fn_800D0F04(int nPlayer, int nToPar);
int   fn_800D1330(int nPlayer);
int   GM_CurrentCourseTotalPar4andPar5Holes(void);                                // CourseData.c
u8    GM_Earnings_CheckEagleEveryPar5(int nPlayer, u8 bPreview);
u8    GM_Earnings_CheckWinAllTournaments(int nPlayer, u8 bPreview);
u8    GM_Earnings_CheckFirstTournamentWin(int nPlayer, u8 bPreview);
s32   GameMode22_GetVariant(void);                                // GameMode22.c
s32   GameMode22_GetHoleRecordIndex(s32 n);

int   GM_Earnings_CheckUnlockCourses(int nProfile, u8 bMessage);
int   GM_Earnings_CapRating(int nRating);
u8    GM_Earnings_IsTourAward(int nId);
u8    Earnings_TestBit(u32 uMask, int nBit);
f32   GM_Earnings_GetCourseModifier(void);
u8    GM_Earnings_AwardShotBonusToUser(int nPlayer);
u8    GM_Earnings_AwardThisTrophyBallToUser(int nPlayer, int nAward);
int   HighScoreRecords_GetEndOfGameRecord(int nPlayer, int bSave, u8 bCountStroke, u8 bAll);
s32   Earnings_GetAwardMessageId(s32 i);

// Copies what the last goal check found into the second set of tables, which the payers read: the
// working tables GM_Earnings_CheckShotGoals, GM_Earnings_CheckPuttGoals and
// GM_Earnings_CheckHoleGoals fill (the money prizes after the multipliers, with their message ids
// and breakdowns; the awards, with their values) and the two counts (lbl_80282254 prizes,
// lbl_80282250 awards). GM_Earnings_PayShotGoals, GM_Earnings_PayHoledGoals and
// GM_Earnings_PayRoundGoals call it after each check, then pay from the copies.
void GM_Earnings_CopyGoalResults(void) {
    gPayNumPrizes = gNumPrizes;
    gPayNumAwards = gNumAwards;
    memcpy(gPayShotPrizeMsgs, gShotPrizeMsgs, sizeof(gPayShotPrizeMsgs));
    memcpy(gPayPuttPrizeMsgs, gPuttPrizeMsgs, sizeof(gPayPuttPrizeMsgs));
    memcpy(gPayHolePrizeMsgs, gHolePrizeMsgs, sizeof(gPayHolePrizeMsgs));
    memcpy(gPayShotPrizes, gShotPrizes, sizeof(gPayShotPrizes));
    memcpy(gPayPuttPrizes, gPuttPrizes, sizeof(gPayPuttPrizes));
    memcpy(gPayHolePrizes, gHolePrizes, sizeof(gPayHolePrizes));
    memcpy(gPayShotAwards, gShotAwards, sizeof(gPayShotAwards));
    memcpy(gPayPuttAwards, gPuttAwards, sizeof(gPayPuttAwards));
    memcpy(gPayHoleAwards, gHoleAwards, sizeof(gPayHoleAwards));
    memcpy(gPayShotAwardMoney, gShotAwardMoney, sizeof(gPayShotAwardMoney));
    memcpy(gPayPuttAwardMoney, gPuttAwardMoney, sizeof(gPayPuttAwardMoney));
    memcpy(gPayHoleAwardMoney, gHoleAwardMoney, sizeof(gPayHoleAwardMoney));
    memcpy(gPayPrizeBreakdowns, gPrizeBreakdowns, sizeof(gPayPrizeBreakdowns));
}

// fake match: stands in for a function EA's linker stripped; it puts 1.0f first in .sdata2
static f32 Earnings_StrippedFn(f32 x) {
    return x + 1.0f;
}

// nMoney rounded to the nearest $25. For a negative amount the cast truncates toward zero, so the
// result can be $25 higher than the nearest. TW06: roundToNearest25 (by position); TW07's
// roundToNearest25Or5 has the same place.
s32 roundToNearest25(s32 nMoney) {
    return (s32)((12.5f + (f32)nMoney) / 25.0f) * 25;
}

// Empty in this build: GM_DeInitModule calls it when a round is torn down. TW06:
// GM_Earnings_FreeStreamMemory (by position).
void GM_Earnings_FreeStreamMemory(void) {
}

// Registers EarningsInfo_LoadERNFromStream as the loader of stream chunk 'ERN ' (the prize table);
// the hole stream manager (fn_80014864) calls it. TW06: EarningsInfo::RegisterStreamClients (by
// position).
void EarningsInfo_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('ERN ', EarningsInfo_LoadERNFromStream);
}

// Removes the 'ERN ' chunk loader again; the hole stream manager (fn_800148A8) calls it. TW06:
// EarningsInfo::UnRegisterStreamClients (by position).
void EarningsInfo_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('ERN ');
}

// The 'ERN ' chunk loader: copies the object into the prize table lbl_80200538 (EarningsTable,
// 0x22F0 bytes). TW07: EarningsInfo::LoadERNFromStream.
void EarningsInfo_LoadERNFromStream(UStreamObject* pObject) {
    // port: the 'ERN ' object is copied straight into the prize table (EarningsTable); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gEarningsTable), &gEarningsTable);
}

// fake match: stands in for a function EA's linker stripped; it puts 0.5f before 0.1f in .sdata2
static f32 Earnings_StrippedFn2(f32 x) {
    return x + 0.5f;
}

// A PGA TOUR tournament's prize for finishing place nRow (0 the winner;
// GM_PgaTourSim_DistributeWinnings pays places 0..69 and splits ties). The winner gets n, the first
// prize, as it is. Any other place gets its share of the purse nTotal (lbl_80191AA4[nRow]: 18% for
// first down to 0.2% for 70th) times (1 - n / nTotal) / (1 - 18%), so the other places share what
// the given first prize leaves as the table shares it; rounded to the nearest $10. TW06:
// GM_Earnings_TournamentPayout (by position); TW07's arguments are (purse, firstPrize, place).
s32 GM_Earnings_TournamentPayout(int nTotal, int n, int nRow) {
    f32 f;
    s32 nRounded;
    s32 nRet;

    if (nRow == 0) {
        nRet = n;
    } else {
        f = 0.1f * (((1.0f - (f32)n / (f32)nTotal) / (1.0f - gTournamentPayoutShares[0])) *
                    ((f32)nTotal * gTournamentPayoutShares[nRow]));
        if (f > 0.0f) {
            nRounded = (s32)(0.5f + f);
        } else {
            nRounded = -(s32)(0.5f - f);
        }
        nRet = nRounded * 10;
    }
    return nRet;
}

// Pays nMoney to player nPlayer (0..3) when the player is human with an active profile and
// mulligans are off (with mulligans nothing is earned). pMoney, when given, is the payout's
// breakdown and is added to the player's round totals (Player.money) field by field; without it the
// whole amount goes into money.n24. The profile's money (SaveProfile.nTotalCash and nCurrentCash)
// grows by nMoney, bChanged is set, and GM_Earnings_CheckUnlockCourses unlocks, with their
// messages, the courses the money now buys.
void GM_Earnings_AwardMoney(int nPlayer, int nMoney, CourseMoneyTracking* pMoney) {
    int nProfile;

    if (nPlayer >= 5 || nPlayer == 4) return;
    if (Player_IsCPU(nPlayer)) return;
    if (Game_GetMulliganRule() != 0) return;
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
    gpSaveData[nProfile].nTotalCash += nMoney;
    gpSaveData[nProfile].nCurrentCash += nMoney;
    gpSaveData[nProfile].bChanged = 1;
    GM_Earnings_CheckUnlockCourses(nProfile, 1);
}

// What a human beating a CPU golfer by nMargin strokes earns: by the loser's earnings rating
// (GM_Earnings_RateGolfer), the base prize plus so much a stroke of the margin (at most 5). *pPrize
// (may be NULL) gets the base. 0 with mulligans, a CPU winner or a human loser. TW06:
// GM_Earnings_GetStrokeWinnings (by position).
int GM_Earnings_GetStrokeWinnings(int nWinner, int nLoser, int nMargin, int* pPrize) {
    int nRating;

    if (Game_GetMulliganRule() != 0) return 0;
    if (Player_IsCPU(nWinner) || !Player_IsCPU(nLoser)) return 0;
    nRating = GM_Earnings_RateGolfer(nLoser);
    if (nMargin > 5) {
        nMargin = 5;
    }
    if (pPrize != NULL) {
        *pPrize = gEarningsTable.aStrokePrize[nRating].nBase;
    }
    return gEarningsTable.aStrokePrize[nRating].nBase + gEarningsTable.aStrokePrize[nRating].nPerStroke
            * nMargin;
}

// The same for teams (team 0: players 0 and 1, team 1: players 2 and 3): what a team with a human
// beating an all-CPU team by nMargin strokes earns, the average of what its two golfers would pay
// (GM_Earnings_GetStrokeWinnings' formula each). *pPrize (may be NULL) gets the average base prize.
// 0 with mulligans. TW06: GM_Earnings_GetStrokeWinningsTeam (by position).
int GM_Earnings_GetStrokeWinningsTeam(int nWinner, int nLoser, int nMargin, int* pPrize) {
    int nFirst;
    int nSecond;
    int nRating1;
    int nRating2;
    int nBase1;
    int nBase2;
    int nTotal;

    if (Game_GetMulliganRule() != 0) return 0;
    if (Team_IsAllCPU(nWinner) || !Team_IsAllCPU(nLoser)) return 0;
    if (nLoser == 0) {
        nFirst = 0;
        nSecond = 1;
    } else {
        nFirst = 2;
        nSecond = 3;
    }
    nRating1 = GM_Earnings_RateGolfer(nFirst);
    nRating2 = GM_Earnings_RateGolfer(nSecond);
    if (nMargin > 5) {
        nMargin = 5;
    }
    nBase1 = gEarningsTable.aStrokePrize[nRating1].nBase;
    nBase2 = gEarningsTable.aStrokePrize[nRating2].nBase;
    if (pPrize != NULL) {
        *pPrize = (nBase1 + nBase2) / 2;
    }
    nTotal = gEarningsTable.aStrokePrize[nRating1].nBase +
             gEarningsTable.aStrokePrize[nRating1].nPerStroke * nMargin;
    nTotal += gEarningsTable.aStrokePrize[nRating2].nBase +
              gEarningsTable.aStrokePrize[nRating2].nPerStroke * nMargin;
    return nTotal / 2;
}

// What winning the current ladder event (GameMode4) by nMargin holes earns: the event's prize plus
// so much a hole of the margin (at most 5); its EA Sports Bio accomplishment, if any, is posted.
// *pPrize (may be NULL) gets the event's prize. nWinner and nLoser are not read. 0 with mulligans.
int GM_Earnings_GetLadderWinnings(int nWinner, int nLoser, int nMargin, s32* pPrize) {
    int nEvent;
    int nMoney;

    if (Game_GetMulliganRule() != 0) return 0;
    nEvent = GameMode4_GetCurrentEvent();
    if (nMargin > 5) {
        nMargin = 5;
    }
    if (pPrize != NULL) {
        *pPrize = gEarningsTable.aLadderPrize[nEvent].nBase;
    }
    nMoney = gEarningsTable.aLadderPrize[nEvent].nBase + gEarningsTable.aLadderPrize[nEvent].nPerHole
            * nMargin;
    if (gEarningsTable.aLadderPrize[nEvent].nBio != -1) {
        EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aLadderPrize[nEvent].nBio].szName,
                                 gEarningsTable.aBio[gEarningsTable.aLadderPrize[nEvent].nBio].nValue);
    }
    return nMoney;
}

// Pays player nPlayer twice nMoney, booked in the breakdown's n24 and n3C (nothing with mulligans).
// Its one caller is GameMode4_WinEvent, with the amount a ladder menu message stored
// (gLadderEventBonus).
void GM_Earnings_AwardDoubleMoney(int nPlayer, int nMoney) {
    CourseMoneyTracking money;
    s32 nPaid;

    if (Game_GetMulliganRule() == 0) {
        nPaid = nMoney * 2;
        Mem_set(&money, 0, sizeof(money));
        money.n24 = nPaid;
        money.n3C = nPaid;
        GM_Earnings_AwardMoney(nPlayer, nPaid, &money);
    }
}

// Unlocks what profile nProfile's money (SaveProfile.nTotalCash) now buys, from the prize table's
// aCoursePrice: each course 0..20 is unlocked and, except course 4, listed, with its EA Sports Bio
// accomplishment when it has one. Price 21 unlocks course slot 22 and price 23 slot 21, each with a
// message of its own (3, 7, 2 and 3, 7, 3). With bMessage a message is queued for each listed
// course. Returns how many were listed.
int GM_Earnings_CheckUnlockCourses(int nProfile, u8 bMessage) {
    int i;
    int n;

    n = 0;
    for (i = 0; i < 21; i++) {
        if (gpSaveData[nProfile].nTotalCash >= gEarningsTable.aCoursePrice[i].nPrice
            && !UserInfo_IsCourseUnlocked(nProfile, i)) {
            UserInfo_UnlockCourse(nProfile, i);
            if (i != 4) {
                // EA bug: the list holds 10, and up to 20 courses could be bought at once
                gUnlockedCourses[n] = i;
                n++;
                if (gEarningsTable.aCoursePrice[i].nBio != -1) {
                    EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aCoursePrice[i].nBio].szName,
                                             gEarningsTable.aBio[gEarningsTable.aCoursePrice[i].nBio].nValue);
                }
            }
        }
    }
    if (gpSaveData[nProfile].nTotalCash >= gEarningsTable.aCoursePrice[21].nPrice
        && !UserInfo_IsCourseSlot22Unlocked(nProfile)) {
        UserInfo_UnlockCourseSlot22(nProfile);
        GUI_QueueMessage(3, 7, 2, nProfile);
        if (gEarningsTable.aCoursePrice[21].nBio != -1) {
            EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aCoursePrice[21].nBio].szName,
                                     gEarningsTable.aBio[gEarningsTable.aCoursePrice[21].nBio].nValue);
        }
    }
    if (gpSaveData[nProfile].nTotalCash >= gEarningsTable.aCoursePrice[23].nPrice
        && !UserInfo_IsCourseSlot21Unlocked(nProfile)) {
        UserInfo_UnlockCourseSlot21(nProfile);
        GUI_QueueMessage(3, 7, 3, nProfile);
        if (gEarningsTable.aCoursePrice[23].nBio != -1) {
            EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aCoursePrice[23].nBio].szName,
                                     gEarningsTable.aBio[gEarningsTable.aCoursePrice[23].nBio].nValue);
        }
    }
    if (bMessage) {
        for (i = 0; i < n; i++) {
            // fake match: the cast gives the original's copy of nProfile for this loop
            GUI_QueueMessage(3, gUnlockedCourses[i], 0, (u32)nProfile);
        }
    }
    return n;
}

// The best earnings rating among the players. TW06: GM_GetHighestRatedGolfer (by position).
int GM_GetHighestRatedGolfer(void) {
    int i;
    int nBest;
    int nRating;

    nBest = 0;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        nRating = GM_Earnings_RateGolfer(i);
        if (nRating > nBest) {
            nBest = nRating;
        }
    }
    return nBest;
}

// A player's earnings rating (0..25), which picks the prize rows: a CPU plays at its golfer's
// rating; a human's is the number of ladder events their profile has won, at most 25. TW06:
// GM_Earnings_RateGolfer (by position).
int GM_Earnings_RateGolfer(int nPlayer) {
    int nRating;

    nRating = UserInfo_GetNumLadderEventsWon(gPlayers[nPlayer].nIndex);
    if (Player_IsCPU(nPlayer)) {
        return gPlayers[nPlayer].golfer.nEarningsRating;
    }
    return GM_Earnings_CapRating(nRating);
}

// nRating, at most 25 (the last row of the rating tables, NUM_EARNINGS_RATINGS - 1). Its callers
// pass a count of ladder wins, which cannot pass 25 (there are 25 events).
int GM_Earnings_CapRating(int nRating) {
    int n;

    n = 25;
    if (nRating <= 25) {
        n = nRating;
    }
    return n;
}

// The earnings rating of golfer nGolfer (gGolferTable's nEarningsRating). The created golfers (from
// FIRST_CREATED_GOLFER) get player 0's profile's ladder wins, at most 25, as a human does in
// GM_Earnings_RateGolfer. The front end's message table reads prize rows with it. TW07:
// GM_GetGolferMoneyRating.
int GM_GetGolferMoneyRating(int nGolfer) {
    int nRating;

    nRating = GameMode4_GetNumEventsWon();
    if (nGolfer >= FIRST_CREATED_GOLFER) {
        return GM_Earnings_CapRating(nRating);
    }
    return gGolferTable[nGolfer].nEarningsRating;
}

// What a skin is worth on round hole nHole (0..17) at earnings rating nRating (the Skins mode
// passes the best among the players, GM_GetHighestRatedGolfer): holes 1..6, 7..12, 13..17 and the
// 18th each have their own value in the rating's aSkins row. TW06: GM_Earnings_GetSkinsHoleValue
// (by position).
s32 GM_Earnings_GetSkinsHoleValue(int nRating, int nHole) {
    if (nHole < 6) return gEarningsTable.aSkins[nRating].aValue[0];
    if (nHole < 12) return gEarningsTable.aSkins[nRating].aValue[1];
    if (nHole < 17) return gEarningsTable.aSkins[nRating].aValue[2];
    return gEarningsTable.aSkins[nRating].aValue[3];
}

// After a shot that stayed in bounds (GM_PlayerTookShot); nothing in game mode 10 or with
// mulligans. The mode's pfnCheckShotAwards is told first; then, for a human player with an active
// profile, the shot records are checked (HighScoreRecords_GetEndOfShotRecord), with a message for
// each new best (kind 2 or 4 in lbl_80200498, ids in lbl_80200510), and the shot goals
// (GM_Earnings_CheckShotGoals). What they found is paid from the copies GM_Earnings_CopyGoalResults
// makes: each money prize with its message and breakdown, and each award (trophy ball) the player
// now gets (GM_Earnings_AwardTrophyBall) with its message (kind 6 for the PGA TOUR awards 23..38,
// else kind 2 with the money; message numbers from Earnings_GetAwardMessageId) and its money, also
// booked as bonuses (money.n8).
void GM_Earnings_PayShotGoals(int nPlayer) {
    int nProfile;
    int i;
    int nKind;

    if (Game_GetMode() == 10) return;
    if (Game_GetMulliganRule() != 0) return;
    gpGame->pfnCheckShotAwards(nPlayer);
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    HighScoreRecords_GetEndOfShotRecord(nPlayer, &gPlayers[nPlayer].ball, 1, 0, 0);
    for (i = 0; i < gNumRecordHits; i++) {
        nKind = gShotRecordResults[i];
        if (nKind == 2 || nKind == 4) {
            GUI_QueueMessage(1, gShotRecordKinds[i], nKind, nProfile);
        }
    }
    GM_Earnings_CheckShotGoals(nPlayer, &gPlayers[nPlayer].ball, 0);
    GM_Earnings_CopyGoalResults();
    for (i = 0; i < gPayNumPrizes; i++) {
        if (gPayShotPrizes[i] != 0) {
            GUI_QueueMessage(0, gPayShotPrizeMsgs[i], gPayShotPrizes[i], nProfile);
            GM_Earnings_AwardMoney(nPlayer, gPayShotPrizes[i], &gPayPrizeBreakdowns[i]);
        }
    }
    for (i = 0; i < gPayNumAwards; i++) {
        if (GM_Earnings_AwardTrophyBall(nPlayer, gPayShotAwards[i])) {
            if (GM_Earnings_IsTourAward(gPayShotAwards[i])) {
                GUI_QueueMessage(6, Earnings_GetAwardMessageId(gPayShotAwards[i]), 0, nProfile);
            } else {
                GUI_QueueMessage(2, Earnings_GetAwardMessageId(gPayShotAwards[i]), gPayShotAwardMoney[i],
                                 nProfile);
            }
            GM_Earnings_AwardMoney(nPlayer, gPayShotAwardMoney[i], NULL);
            gPlayers[nPlayer].money.n8 += gPayShotAwardMoney[i];
        }
    }
}

// Whether award nId is one of 23..38, the PGA TOUR and career awards (Earnings_IsTourAwardEarned
// decides them). The payers give these a message of their own kind (6), with no money in it.
u8 GM_Earnings_IsTourAward(int nId) {
    int b;

    b = 0;
    if (nId >= 23 && nId <= 38) {
        b = 1;
    }
    return b;
}

// After the ball is holed (GM_PlayerTookShot, when GM_CheckForBallInHole says so), for a human
// player with an active profile; nothing in game mode 10 or with mulligans. The putt records are
// checked (HighScoreRecords_GetEndOfHoleRecord), with a message for each new best (kind 2 or 4 in
// lbl_80200470, ids in lbl_802004E8). Then two rounds of payouts, made as GM_Earnings_PayShotGoals
// makes them: the putt goals' (GM_Earnings_CheckPuttGoals), then those of the hole goals checked
// after each hole (GM_Earnings_CheckHoleGoals; none in a playoff, gpGame->bInPlayoff).
void GM_Earnings_PayHoledGoals(int nPlayer) {
    int nProfile;
    int nKind;
    int i;

    if (Game_GetMode() == 10) return;
    if (Game_GetMulliganRule() != 0) return;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    HighScoreRecords_GetEndOfHoleRecord(nPlayer, &gPlayers[nPlayer].ball, 1, 0, 0);
    for (i = 0; i < gNumRecordHits; i++) {
        nKind = gPuttRecordResults[i];
        if (nKind == 2 || nKind == 4) {
            GUI_QueueMessage(1, gPuttRecordKinds[i], nKind, nProfile);
        }
    }
    GM_Earnings_CheckPuttGoals(nPlayer, 0);
    GM_Earnings_CopyGoalResults();
    for (i = 0; i < gPayNumPrizes; i++) {
        if (gPayPuttPrizes[i] != 0) {
            GUI_QueueMessage(0, gPayPuttPrizeMsgs[i], gPayPuttPrizes[i], nProfile);
            GM_Earnings_AwardMoney(nPlayer, gPayPuttPrizes[i], &gPayPrizeBreakdowns[i]);
        }
    }
    for (i = 0; i < gPayNumAwards; i++) {
        if (GM_Earnings_AwardTrophyBall(nPlayer, gPayPuttAwards[i])) {
            if (GM_Earnings_IsTourAward(gPayPuttAwards[i])) {
                GUI_QueueMessage(6, Earnings_GetAwardMessageId(gPayPuttAwards[i]), 0, nProfile);
            } else {
                GUI_QueueMessage(2, Earnings_GetAwardMessageId(gPayPuttAwards[i]), gPayPuttAwardMoney[i],
                                 nProfile);
            }
            GM_Earnings_AwardMoney(nPlayer, gPayPuttAwardMoney[i], NULL);
            gPlayers[nPlayer].money.n8 += gPayPuttAwardMoney[i];
        }
    }
    if (!gpGame->bInPlayoff) {
        GM_Earnings_CheckHoleGoals(nPlayer, 0, 0);
    } else {
        gNumPrizes = 0;
        gNumAwards = 0;
    }
    GM_Earnings_CopyGoalResults();
    for (i = 0; i < gPayNumPrizes; i++) {
        if (gPayHolePrizes[i] != 0) {
            GUI_QueueMessage(0, gPayHolePrizeMsgs[i], gPayHolePrizes[i], nProfile);
            GM_Earnings_AwardMoney(nPlayer, gPayHolePrizes[i], &gPayPrizeBreakdowns[i]);
        }
    }
    for (i = 0; i < gPayNumAwards; i++) {
        if (GM_Earnings_AwardTrophyBall(nPlayer, gHoleAwards[i])) {
            if (GM_Earnings_IsTourAward(gPayHoleAwards[i])) {
                GUI_QueueMessage(6, Earnings_GetAwardMessageId(gPayHoleAwards[i]), 0, nProfile);
            } else {
                GUI_QueueMessage(2, Earnings_GetAwardMessageId(gPayHoleAwards[i]), gPayHoleAwardMoney[i],
                                 nProfile);
            }
            GM_Earnings_AwardMoney(nPlayer, gPayHoleAwardMoney[i], NULL);
            gPlayers[nPlayer].money.n8 += gPayHoleAwardMoney[i];
        }
    }
}

// At the end of a round, for a human player with an active profile; nothing in speed golf
// (GM_IsSpeedGolfMode) or with mulligans. GameManager calls it after the round's last hole outside
// a playoff (bRoundOver 0) and, with gpGame->b273, when the game is over (bRoundOver 1). Without
// bRoundOver the round records are checked first (HighScoreRecords_GetEndOfGameRecord), with a
// message for each new best (kind 2 or 4 in lbl_80200448, ids in lbl_802004C0). Then the hole goals
// (GM_Earnings_CheckHoleGoals: with bRoundOver those kept for the end of the round) are paid as
// GM_Earnings_PayShotGoals pays, and the TOUR card level rises with the profile's completion score
// (GM_GetGameProgress): level 2 from 7.5, 3 from 15, 4 from 30, 5 from 60, 6 at 100, with message
// 99 + level.
void GM_Earnings_PayRoundGoals(int nPlayer, u8 bRoundOver) {
    int nProfile;
    int nLevel;
    int nKind;
    int i;
    f32 fProgress;

    nLevel = 0;
    if (GM_IsSpeedGolfMode()) return;
    if (Game_GetMulliganRule() != 0) return;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!bRoundOver) {
        HighScoreRecords_GetEndOfGameRecord(nPlayer, 1, 0, 0);
        for (i = 0; i < gNumRecordHits; i++) {
            nKind = gRoundRecordResults[i];
            if (nKind == 2 || nKind == 4) {
                GUI_QueueMessage(1, gRoundRecordKinds[i], nKind, nProfile);
            }
        }
    }
    GM_Earnings_CheckHoleGoals(nPlayer, 0, bRoundOver);
    GM_Earnings_CopyGoalResults();
    for (i = 0; i < gPayNumPrizes; i++) {
        if (gHolePrizes[i] != 0) {
            GUI_QueueMessage(0, gHolePrizeMsgs[i], gHolePrizes[i], nProfile);
            GM_Earnings_AwardMoney(nPlayer, gHolePrizes[i], &gPayPrizeBreakdowns[i]);
        }
    }
    for (i = 0; i < gPayNumAwards; i++) {
        if (GM_Earnings_AwardTrophyBall(nPlayer, gHoleAwards[i])) {
            if (GM_Earnings_IsTourAward(gPayHoleAwards[i])) {
                GUI_QueueMessage(6, Earnings_GetAwardMessageId(gPayHoleAwards[i]), 0, nProfile);
            } else {
                GUI_QueueMessage(2, Earnings_GetAwardMessageId(gPayHoleAwards[i]), gPayHoleAwardMoney[i],
                                 nProfile);
            }
            GM_Earnings_AwardMoney(nPlayer, gHoleAwardMoney[i], NULL);
            gPlayers[nPlayer].money.n8 += gHoleAwardMoney[i];
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
        GUI_QueueMessage(0, gpSaveData[nProfile].nTourCardLevel + 99, 0, nProfile);
    }
}

// The bit of ShotGoal.uLies and PuttGoal.uLies for a surface class (SurfaceType.nClass; the goal
// checkers pass fn_800D0BAC, the surface the shot left): class 1 bit 0, 2 (fairway) bit 1, 5
// (rough) bit 2, 6 (sand) bit 3, 3 (green) bit 4; any other class bit 6.
s32 Earnings_GetSurfaceClassBit(u32 n) {
    if (n == 1) return 0;
    if (n == 2) return 1;
    if (n == 5) return 2;
    if (n == 6) return 3;
    if (n == 3) return 4;
    return 6;
}

// The bit of ShotGoal.uBallLies for the ball's lie (Lie_t): 0 the tee; 1 fairway (fairway, tight
// fairway, fringe); 2 rough (the three roughs, ice, snow, misc); 3 sand; 4 green; 5 in the cup; 6
// anything else (cart path, water, out of bounds).
s32 Earnings_GetLieBit(int n) {
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

// Checks the shot goals (the prize table's aShotGoal) against the shot just played and fills the
// working tables with what they give: awards (trophy balls, GM_Earnings_AwardThisTrophyBallToUser;
// lbl_80282250 of them, in lbl_802002B8 with their values in lbl_80200240) and money prizes
// (lbl_80282254 of them: lbl_80200330 as found, lbl_802003A8 after
// GM_Earnings_ComputeBonusModifiers and, with uMults bit 3, GM_Earnings_ComputeTOURCardModifiers;
// message ids lbl_80200420, breakdowns lbl_801FFAE8). Of goals with the same nonzero id only the
// one with the biggest nValue is kept. Nothing is found with the session's debug flag 0x4000, for a
// player who cannot earn (GM_Earnings_AwardShotBonusToUser), in a lesson or with mulligans; during
// a challenge that is not a ladder event only goals with mode bit 5 count. With pBall the check
// runs on that ball in place of the player's own. With bPreview (a what-if from HoleScore or
// Earnings_CheckShotAwards) the shot is not counted yet (one stroke fewer), the PGA TOUR awards
// (23..38) are left out and no EA Sports Bio accomplishment is posted; with bPreview and no ball
// the tests on the ball are skipped too.
void GM_Earnings_CheckShotGoals(int nPlayer, Ball* pBall, u8 bPreview) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    Ball saved;
    int i;
    u8 bReplace;
    u8 bLost;
    int j;
    int nSlot;
    s32 nValue;
    s32 nAdj;
    u8 bNoBall;
    int nHoles;

    gNumPrizes = 0;
    gNumAwards = 0;
    if (gSession.uFlags & 0x4000) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!GM_Earnings_AwardShotBonusToUser(nPlayer)) return;
    if (Lessons_IsRunning()) return;
    if (Game_GetMulliganRule() != 0) return;

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
        if (!gEarningsTable.aShotGoal[i].bEnabled) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uModes, Game_GetMode())) continue;
        if (PlayNow_IsChallengeRunning() && !GameMode4_IsEventRunning()
            && !Earnings_TestBit(gEarningsTable.aShotGoal[i].uModes, 5)) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uPars, 0) && GM_GetCurrentHolePar() == 3) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uPars, 1) && GM_GetCurrentHolePar() == 4) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uPars, 2) && GM_GetCurrentHolePar() == 5) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uLies,
                              Earnings_GetSurfaceClassBit(fn_800D0BAC(nPlayer)))) continue;
        if (gEarningsTable.aShotGoal[i].f0C > fn_800D04AC(nPlayer)) continue;
        if (!bNoBall && !Earnings_TestBit(gEarningsTable.aShotGoal[i].uBallLies,
                                     Earnings_GetLieBit(gPlayers[nPlayer].ball.nLie))) continue;
        if (!bNoBall && gEarningsTable.aShotGoal[i].f14 > fn_800D0550(nPlayer)) continue;
        if (!bNoBall && gEarningsTable.aShotGoal[i].f18 &&
            gEarningsTable.aShotGoal[i].f18 < fn_800D0478(nPlayer)) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uShotKinds, gPlayers[nPlayer].nShotKind)) continue;
        if (!Earnings_TestBit(gEarningsTable.aShotGoal[i].uClubs, gPlayers[nPlayer].nClub)) continue;
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 0)
            && !fn_800D0BF8(nPlayer, 1, bPreview)) continue;
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 1)
            && !fn_800D0BF8(nPlayer, 0, bPreview)) continue;
        if (!bNoBall && Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 2)
            && !fn_800D0D54(nPlayer)) continue;
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 3) && gPlayers[nPlayer].b312) continue;
        if (!bNoBall && Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 4)
            && !gPlayers[nPlayer].bHitObject) continue;
        if (!bNoBall && Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 5)
            && !gPlayers[nPlayer].bHitPin) continue;
        // Flag 6: the hole's first stroke.
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 6) &&
            nAdj + 1 != gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]) continue;
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 7) && fn_800D1170(nPlayer, 0) < nAdj
            + 18) continue;
        if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uFlags, 8)) {
            nHoles = GM_CurrentCourseTotalPar4andPar5Holes();
            if (nHoles < 10) continue;
            if (nHoles + nAdj > fn_800D0FBC(nPlayer)) continue;
        }
        if (gEarningsTable.aShotGoal[i].nAward >= 23 && gEarningsTable.aShotGoal[i].nAward <= 38 &&
            (bPreview || !Earnings_IsTourAwardEarned(nPlayer, gEarningsTable.aShotGoal[i].nAward))) continue;
        if (gEarningsTable.aShotGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (gEarningsTable.aShotGoal[i].nAward != 39) {
            int nSlot;  // fake match: shadows the function-level nSlot (register order; TW07 keeps
                        // one function-level index per table, this one awaits its own name)

            if (!GM_Earnings_AwardThisTrophyBallToUser(nPlayer, gEarningsTable.aShotGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumAwards;
            if (gEarningsTable.aShotGoal[i].nId != 0) {
                for (j = 0; j < gNumAwards; j++) {
                    if (gEarningsTable.aShotGoal[i].nId == aAwardIds[j]) {
                        if (gEarningsTable.aShotGoal[i].nValue > gShotAwardMoney[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gShotAwards[nSlot] = gEarningsTable.aShotGoal[i].nAward;
            gShotAwardMoney[nSlot] = gEarningsTable.aShotGoal[i].nValue;
            aAwardIds[nSlot] = gEarningsTable.aShotGoal[i].nId;
            if (!bReplace) {
                gNumAwards++;
            }
            if (!bPreview && gEarningsTable.aShotGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aShotGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aShotGoal[i].nBio].nValue);
            }
        } else {
            nValue = gEarningsTable.aShotGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumPrizes;
            if (gEarningsTable.aShotGoal[i].nId != 0) {
                for (j = 0; j < gNumPrizes; j++) {
                    if (gEarningsTable.aShotGoal[i].nId == aPrizeIds[j]) {
                        if (gEarningsTable.aShotGoal[i].nValue > gShotPrizeBases[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gShotPrizeBases[nSlot] = nValue;
            aPrizeIds[nSlot] = gEarningsTable.aShotGoal[i].nId;
            gShotPrizes[nSlot] = GM_Earnings_ComputeBonusModifiers(gShotPrizeBases[nSlot], nPlayer,
                                              Earnings_TestBit(gEarningsTable.aShotGoal[i].uMults, 0),
                                              Earnings_TestBit(gEarningsTable.aShotGoal[i].uMults, 1),
                                              Earnings_TestBit(gEarningsTable.aShotGoal[i].uMults, 2),
                                              &gPrizeBreakdowns[nSlot]);
            if (Earnings_TestBit(gEarningsTable.aShotGoal[i].uMults, 3)) {
                gShotPrizes[nSlot] = GM_Earnings_ComputeTOURCardModifiers(gShotPrizes[nSlot], nPlayer,
                        &gPrizeBreakdowns[nSlot]);
            }
            gShotPrizeMsgs[nSlot] = gEarningsTable.aShotGoal[i].n2A;
            if (!bReplace) {
                gNumPrizes++;
            }
            if (!bPreview && gEarningsTable.aShotGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aShotGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aShotGoal[i].nBio].nValue);
            }
        }
    }

    if (pBall != NULL) {
        Mem_cpy(&gPlayers[nPlayer].ball, &saved, sizeof(Ball));
    }
}

// Whether bit nBit of uMask is set.
u8 Earnings_TestBit(u32 uMask, int nBit) {
    return (uMask & (1 << nBit)) != 0;
}

// Checks the putt goals (aPuttGoal) when the ball drops: the score on the hole, the putts and the
// last shot's tests. It fills the putt tables as GM_Earnings_CheckShotGoals fills the shot tables
// (awards lbl_80200290 with their values in lbl_80200218; prizes lbl_80200308 as found,
// lbl_80200380 after the multipliers, message ids lbl_802003F8), keeping of goals with the same
// nonzero id only the biggest nValue. Nothing is found with the session's debug flag 0x4000, for a
// player who cannot earn (GM_Earnings_AwardShotBonusToUser), during a challenge that is not a
// ladder event, or with mulligans. While it runs the holes still to come count 999 strokes and
// putts (0 afterwards). With bPreview (a what-if from HoleScore or Earnings_CheckPuttAwards) the
// hole counts one more stroke and putt (the ball dropping now), flag tests 2 and 5 are skipped, the
// PGA TOUR awards (23..38) are left out and no EA Sports Bio accomplishment is posted.
void GM_Earnings_CheckPuttGoals(int nPlayer, u8 bPreview) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    int i;
    u8 bReplace;
    u8 bLost;
    int j;
    s32 nValue;

    gNumPrizes = 0;
    gNumAwards = 0;
    if (gSession.uFlags & 0x4000) return;
    if (Player_IsCPU(nPlayer)) return;
    if (!GM_Earnings_AwardShotBonusToUser(nPlayer)) return;
    if (PlayNow_IsChallengeRunning() && !GameMode4_IsEventRunning()) return;
    if (Game_GetMulliganRule() != 0) return;

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 999;
        gPlayers[nPlayer].nPutts[i] = 999;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]++;
    }

    for (i = 0; i < NUM_PUTT_GOALS; i++) {
        if (!gEarningsTable.aPuttGoal[i].bEnabled) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uModes, Game_GetMode())) continue;
        if (PlayNow_IsChallengeRunning() && !GameMode4_IsEventRunning()
            && !Earnings_TestBit(gEarningsTable.aPuttGoal[i].uModes, 5)) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uPars, 0) && GM_GetCurrentHolePar() == 3) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uPars, 1) && GM_GetCurrentHolePar() == 4) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uPars, 2) && GM_GetCurrentHolePar() == 5) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uLies,
                              Earnings_GetSurfaceClassBit(fn_800D0BAC(nPlayer)))) continue;
        if (gEarningsTable.aPuttGoal[i].f0C > fn_800D04AC(nPlayer)) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uShotKinds, gPlayers[nPlayer].nShotKind)) continue;
        if (!Earnings_TestBit(gEarningsTable.aPuttGoal[i].uClubs, gPlayers[nPlayer].nClub)) continue;
        if (Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 0) && !fn_800D0BF8(nPlayer, 1, 0)) continue;
        if (Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 1) && !fn_800D0BF8(nPlayer, 0, 0)) continue;
        if (!bPreview && Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 2)
            && !fn_800D0D54(nPlayer)) continue;
        if (Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 4) &&
            !gPlayers[nPlayer].bBunkerThisHole && !gPlayers[nPlayer].b311) continue;
        if (!bPreview && Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 5) &&
            !gPlayers[nPlayer].bHitPin) continue;
        if (gEarningsTable.aPuttGoal[i].nMaxPutts != 0 &&
            gEarningsTable.aPuttGoal[i].nMaxPutts < gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]) continue;
        // The score on the hole: 2 triple bogey or better, 3 double bogey, 4 bogey, 5 par or better,
        // 6 birdie, 7 eagle and 8 albatross (not with a hole in one), 9 a hole in one.
        if (gEarningsTable.aPuttGoal[i].nScore == 2 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > GM_GetCurrentHolePar() + 3) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 3 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != GM_GetCurrentHolePar() + 2) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 4 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != GM_GetCurrentHolePar() + 1) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 5 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > GM_GetCurrentHolePar()) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 6 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != GM_GetCurrentHolePar() - 1) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 7 &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 ||
             gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != GM_GetCurrentHolePar() - 2)) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 8 &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 ||
             gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != GM_GetCurrentHolePar() - 3)) continue;
        if (gEarningsTable.aPuttGoal[i].nScore == 9 &&
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > 1) continue;
        // Flag 6: a hole in one, and the round's second (fn_800D0DC8 below -3 counts holes in one).
        if (Earnings_TestBit(gEarningsTable.aPuttGoal[i].uFlags, 6) &&
            (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] > 1 || fn_800D0DC8(nPlayer, -5) < 2)) continue;
        if (gEarningsTable.aPuttGoal[i].nAward >= 23 && gEarningsTable.aPuttGoal[i].nAward <= 38 &&
            (bPreview || !Earnings_IsTourAwardEarned(nPlayer, gEarningsTable.aPuttGoal[i].nAward))) continue;
        if (gEarningsTable.aPuttGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (gEarningsTable.aPuttGoal[i].nAward != 39) {
            int nSlot;

            if (!GM_Earnings_AwardThisTrophyBallToUser(nPlayer, gEarningsTable.aPuttGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumAwards;
            if (gEarningsTable.aPuttGoal[i].nId != 0) {
                for (j = 0; j < gNumAwards; j++) {
                    if (gEarningsTable.aPuttGoal[i].nId == aAwardIds[j]) {
                        if (gEarningsTable.aPuttGoal[i].nValue > gPuttAwardMoney[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gPuttAwards[nSlot] = gEarningsTable.aPuttGoal[i].nAward;
            gPuttAwardMoney[nSlot] = gEarningsTable.aPuttGoal[i].nValue;
            aAwardIds[nSlot] = gEarningsTable.aPuttGoal[i].nId;
            if (!bReplace) {
                gNumAwards++;
            }
            if (!bPreview && gEarningsTable.aPuttGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aPuttGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aPuttGoal[i].nBio].nValue);
            }
        } else {
            int nSlot;

            nValue = gEarningsTable.aPuttGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumPrizes;
            if (gEarningsTable.aPuttGoal[i].nId != 0) {
                for (j = 0; j < gNumPrizes; j++) {
                    if (gEarningsTable.aPuttGoal[i].nId == aPrizeIds[j]) {
                        if (gEarningsTable.aPuttGoal[i].nValue > gPuttPrizeBases[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gPuttPrizeBases[nSlot] = gEarningsTable.aPuttGoal[i].nValue;
            aPrizeIds[nSlot] = gEarningsTable.aPuttGoal[i].nId;
            gPuttPrizes[nSlot] = GM_Earnings_ComputeBonusModifiers(gPuttPrizeBases[nSlot], nPlayer,
                                              Earnings_TestBit(gEarningsTable.aPuttGoal[i].uMults, 0),
                                              Earnings_TestBit(gEarningsTable.aPuttGoal[i].uMults, 1),
                                              Earnings_TestBit(gEarningsTable.aPuttGoal[i].uMults, 2),
                                              &gPrizeBreakdowns[nSlot]);
            if (Earnings_TestBit(gEarningsTable.aPuttGoal[i].uMults, 3)) {
                gPuttPrizes[nSlot] = GM_Earnings_ComputeTOURCardModifiers(gPuttPrizes[nSlot], nPlayer,
                        &gPrizeBreakdowns[nSlot]);
            }
            gPuttPrizeMsgs[nSlot] = gEarningsTable.aPuttGoal[i].n1D;
            if (!bReplace) {
                gNumPrizes++;
            }
            if (!bPreview && gEarningsTable.aPuttGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aPuttGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aPuttGoal[i].nBio].nValue);
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

// Checks the hole goals (aHoleGoal) against the round so far and fills the hole tables as
// GM_Earnings_CheckPuttGoals does (awards lbl_80200268 with their values in lbl_802001F0; prizes
// lbl_802002E0 as found, lbl_80200358 after the multipliers, message ids lbl_802003D0). Without
// bRoundOver (after a hole) only the goals marked bEachHole count, and those without b19 only on
// the 18th hole of a full round; with bRoundOver (the game over) only the others. Nothing is found
// with the session's debug flag 0x4000, for a player who cannot earn
// (GM_Earnings_AwardShotBonusToUser) or with mulligans. While it runs the holes still to come count
// 999 strokes and putts (0 afterwards). With bPreview (a what-if from HoleScore) this hole counts
// one more stroke and putt, the whole-round tests (GM_Earnings_CheckEagleEveryPar5 and the others)
// predict, the PGA TOUR awards (23..38) are left out and no EA Sports Bio accomplishment is posted.
void GM_Earnings_CheckHoleGoals(int nPlayer, u8 bPreview, u8 bRoundOver) {
    s32 aPrizeIds[10];
    s32 aAwardIds[10];
    int i;
    u8 bReplace;
    u8 bLost;
    int j;
    s32 nValue;
    int nHoles;
    int nNeed;
    u8 bMore;

    gNumPrizes = 0;
    gNumAwards = 0;
    if (gSession.uFlags & 0x4000) return;
    if (!GM_Earnings_AwardShotBonusToUser(nPlayer)) return;
    if (Game_GetMulliganRule() != 0) return;

    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        gPlayers[nPlayer].nStrokes[i] = 999;
        gPlayers[nPlayer].nPutts[i] = 999;
    }
    if (bPreview) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
        gPlayers[nPlayer].nPutts[Game_CurHoleIndex()]++;
    }
    nNeed = 0;                  // fake match: the flag is worked out in an int first
    if (!bRoundOver && (!GM_FullRoundOfGolf() || Game_CurHoleIndex() != 17)) {
        nNeed = 1;
    }
    bMore = nNeed;

    for (i = 0; i < NUM_HOLE_GOALS; i++) {
        if (gEarningsTable.aHoleGoal[i].bEachHole && bRoundOver) continue;
        if (!gEarningsTable.aHoleGoal[i].bEachHole && !bRoundOver) continue;
        if (!gEarningsTable.aHoleGoal[i].bEnabled) continue;
        if (!Earnings_TestBit(gEarningsTable.aHoleGoal[i].uModes, Game_GetMode())) continue;
        if (PlayNow_IsChallengeRunning() && !GameMode4_IsEventRunning()
            && !Earnings_TestBit(gEarningsTable.aHoleGoal[i].uModes, 5)) continue;
        if (!gEarningsTable.aHoleGoal[i].b19 && bMore) continue;
        if (gEarningsTable.aHoleGoal[i].aToPar[0] != 0 &&
            gEarningsTable.aHoleGoal[i].aToPar[0] > fn_800D0DC8(nPlayer, 0)) continue;
        if (gEarningsTable.aHoleGoal[i].aToPar[1] != 0 &&
            gEarningsTable.aHoleGoal[i].aToPar[1] > fn_800D0DC8(nPlayer, -1)) continue;
        if (gEarningsTable.aHoleGoal[i].aToPar[2] != 0 &&
            gEarningsTable.aHoleGoal[i].aToPar[2] > fn_800D0DC8(nPlayer, -2)) continue;
        if (gEarningsTable.aHoleGoal[i].aToPar[3] != 0 &&
            gEarningsTable.aHoleGoal[i].aToPar[3] > fn_800D0DC8(nPlayer, -3)) continue;
        if (gEarningsTable.aHoleGoal[i].aToPar[4] != 0 &&
            gEarningsTable.aHoleGoal[i].aToPar[4] > fn_800D0DC8(nPlayer, -5)) continue;
        if (gEarningsTable.aHoleGoal[i].aRun[0] != 0 &&
            gEarningsTable.aHoleGoal[i].aRun[0] > fn_800D0F04(nPlayer, 0)) continue;
        if (gEarningsTable.aHoleGoal[i].aRun[1] != 0 &&
            gEarningsTable.aHoleGoal[i].aRun[1] > fn_800D0F04(nPlayer, -1)) continue;
        if (gEarningsTable.aHoleGoal[i].aRun[2] != 0 &&
            gEarningsTable.aHoleGoal[i].aRun[2] > fn_800D0F04(nPlayer, -2)) continue;
        if (gEarningsTable.aHoleGoal[i].aRun[3] != 0 &&
            gEarningsTable.aHoleGoal[i].aRun[3] > fn_800D0F04(nPlayer, -3)) continue;
        if (gEarningsTable.aHoleGoal[i].aRun[4] != 0 &&
            gEarningsTable.aHoleGoal[i].aRun[4] > fn_800D0F04(nPlayer, -5)) continue;
        if (gEarningsTable.aHoleGoal[i].n13 != 0) {
            nNeed = gEarningsTable.aHoleGoal[i].n13;
            nHoles = GM_CurrentCourseTotalPar4andPar5Holes();
            if (nHoles < 10) continue;
            if (nNeed > nHoles) {
                nNeed = nHoles;
            }
            if (nNeed > fn_800D0FBC(nPlayer)) continue;
        }
        if (gEarningsTable.aHoleGoal[i].n14 != 0 &&
            gEarningsTable.aHoleGoal[i].n14 > fn_800D1170(nPlayer, 0)) continue;
        if (gEarningsTable.aHoleGoal[i].n15 != 0 &&
            gEarningsTable.aHoleGoal[i].n15 > fn_800D10B0(nPlayer)) continue;
        if (gEarningsTable.aHoleGoal[i].n16 != 0 &&
            gEarningsTable.aHoleGoal[i].n16 > fn_800D1250(nPlayer)) continue;
        if (gEarningsTable.aHoleGoal[i].n17 != 0 &&
            gEarningsTable.aHoleGoal[i].n17 > fn_800D1330(nPlayer)) continue;
        if (gEarningsTable.aHoleGoal[i].nMaxStrokes != 0 &&
            gEarningsTable.aHoleGoal[i].nMaxStrokes < GM_GetPlayerRoundStrokes(nPlayer)) continue;
        if (gEarningsTable.aHoleGoal[i].nKind != 0) {
            if (gEarningsTable.aHoleGoal[i].nKind == 1
                && !GM_Earnings_CheckEagleEveryPar5(nPlayer, bPreview)) continue;
            if (gEarningsTable.aHoleGoal[i].nKind == 2
                && !GM_Earnings_CheckWinAllTournaments(nPlayer, bPreview)) continue;
            if (gEarningsTable.aHoleGoal[i].nKind == 3) continue;
            if (gEarningsTable.aHoleGoal[i].nKind == 4
                && !GM_Earnings_CheckFirstTournamentWin(nPlayer, bPreview)) continue;
            if (gEarningsTable.aHoleGoal[i].nKind == 5 && fn_800D0E74(nPlayer) != 0) continue;
            if (gEarningsTable.aHoleGoal[i].nKind == 6 && GM_GetCurrentCourseTotalPar(0)
                <= GM_GetPlayerRoundStrokes(nPlayer)) continue;
        }
        if (gEarningsTable.aHoleGoal[i].nAward >= 23 && gEarningsTable.aHoleGoal[i].nAward <= 38 &&
            (bPreview || !Earnings_IsTourAwardEarned(nPlayer, gEarningsTable.aHoleGoal[i].nAward))) continue;
        if (gEarningsTable.aHoleGoal[i].nAward == 22 &&
            GM_GetGameProgress(&gpSaveData[nPlayer]) < 100.0f) continue;

        if (gEarningsTable.aHoleGoal[i].nAward != 39) {
            int nSlot;

            if (!GM_Earnings_AwardThisTrophyBallToUser(nPlayer, gEarningsTable.aHoleGoal[i].nAward)) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumAwards;
            if (gEarningsTable.aHoleGoal[i].nId != 0) {
                for (j = 0; j < gNumAwards; j++) {
                    if (gEarningsTable.aHoleGoal[i].nId == aAwardIds[j]) {
                        if (gEarningsTable.aHoleGoal[i].nValue > gHoleAwardMoney[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gHoleAwards[nSlot] = gEarningsTable.aHoleGoal[i].nAward;
            gHoleAwardMoney[nSlot] = gEarningsTable.aHoleGoal[i].nValue;
            aAwardIds[nSlot] = gEarningsTable.aHoleGoal[i].nId;
            if (!bReplace) {
                gNumAwards++;
            }
            if (!bPreview && gEarningsTable.aHoleGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aHoleGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aHoleGoal[i].nBio].nValue);
            }
        } else {
            int nSlot;

            nValue = gEarningsTable.aHoleGoal[i].nValue;
            if (nValue == 0) continue;
            bReplace = 0;
            bLost = 0;
            nSlot = gNumPrizes;
            if (gEarningsTable.aHoleGoal[i].nId != 0) {
                for (j = 0; j < gNumPrizes; j++) {
                    if (gEarningsTable.aHoleGoal[i].nId == aPrizeIds[j]) {
                        if (gEarningsTable.aHoleGoal[i].nValue > gHolePrizeBases[j]) {
                            nSlot = j;
                            bReplace = 1;
                        } else {
                            bLost = 1;
                        }
                    }
                }
            }
            if (bLost) continue;
            gHolePrizeBases[nSlot] = nValue;
            aPrizeIds[nSlot] = gEarningsTable.aHoleGoal[i].nId;
            gHolePrizes[nSlot] = GM_Earnings_ComputeBonusModifiers(gHolePrizeBases[nSlot], nPlayer,
                                              Earnings_TestBit(gEarningsTable.aHoleGoal[i].uMults, 0),
                                              Earnings_TestBit(gEarningsTable.aHoleGoal[i].uMults, 1),
                                              Earnings_TestBit(gEarningsTable.aHoleGoal[i].uMults, 2),
                                              &gPrizeBreakdowns[nSlot]);
            if (Earnings_TestBit(gEarningsTable.aHoleGoal[i].uMults, 3)) {
                gHolePrizes[nSlot] = GM_Earnings_ComputeTOURCardModifiers(gHolePrizes[nSlot], nPlayer,
                        &gPrizeBreakdowns[nSlot]);
            }
            gHolePrizeMsgs[nSlot] = gEarningsTable.aHoleGoal[i].n25;
            if (!bReplace) {
                gNumPrizes++;
            }
            if (!bPreview && gEarningsTable.aHoleGoal[i].nBio != -1) {
                EASBio_SetAccomplishment(gEarningsTable.aBio[gEarningsTable.aHoleGoal[i].nBio].szName,
                                         gEarningsTable.aBio[gEarningsTable.aHoleGoal[i].nBio].nValue);
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

// Whole-round test 1 of the hole goals (HoleGoal.nKind): the par-5 eagles
// (GM_ConvertCourseAndHoleToPar5EagleIndex's items, the profile's kind-0 records). Without
// bPreview: whether the profile has eagled every one of items 0..70. With it (predicting the hole
// now being finished): whether this hole is a par 5 played in eagle or better, the profile holds 70
// of items 0..70 and this hole's item is one it does not hold. The strokes are read at
// Game_GetCurHoleNum (the hole's number on its course), and course 10 (items 27..30) is missing
// from the table below. TW07: GM_Earnings_CheckEagleEveryPar5.
u8 GM_Earnings_CheckEagleEveryPar5(int nPlayer, u8 bPreview) {
    int nProfile;
    u8 bAll;
    int i;
    int nHave;
    int nItem;

    nProfile = gPlayers[nPlayer].nIndex;
    if (!gpSaveData[nProfile].bActive) return 0;
    if (!bPreview) {
        bAll = 1;
        for (i = 0; i < 71; i++) {
            if (!UserInfo_GetPar5EagleStat(&gpSaveData[nProfile], 0, i)) {
                bAll = 0;
            }
        }
        return bAll;
    }
    if (GM_GetHolePar(Game_GetCourse(), Game_GetCurHoleNum()) == 5 &&
        gPlayers[nPlayer].nStrokes[Game_GetCurHoleNum()]
                <= GM_GetHolePar(gpGame->nCurCourse, Game_GetCurHoleNum()) - 2) {
        // EA bug: from here the profile is picked by nPlayer, not nProfile as above.
        nHave = 0;
        for (i = 0; i < 71; i++) {
            if (UserInfo_GetPar5EagleStat(&gpSaveData[nPlayer], 0, i)) {
                nHave++;
            }
        }
        if (nHave == 70) {
            nItem = -1;
            switch (Game_GetCourse()) {
            case 0:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 0; break;
                case 6: nItem = 1; break;
                case 14: nItem = 2; break;
                case 18: nItem = 3; break;
                }
                break;
            case 1:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 4; break;
                case 4: nItem = 5; break;
                case 10: nItem = 6; break;
                case 15: nItem = 7; break;
                }
                break;
            case 2:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 8; break;
                case 9: nItem = 9; break;
                case 11: nItem = 10; break;
                case 16: nItem = 11; break;
                }
                break;
            case 3:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 12; break;
                case 9: nItem = 13; break;
                case 13: nItem = 14; break;
                case 18: nItem = 15; break;
                }
                break;
            case 6:
                switch (Game_GetCurHoleNum() + 1) {
                case 6: nItem = 16; break;
                case 15: nItem = 17; break;
                case 17: nItem = 18; break;
                }
                break;
            case 12:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 19; break;
                case 6: nItem = 20; break;
                case 11: nItem = 21; break;
                case 18: nItem = 22; break;
                }
                break;
            case 9:
                switch (Game_GetCurHoleNum() + 1) {
                case 4: nItem = 23; break;
                case 6: nItem = 24; break;
                case 12: nItem = 25; break;
                case 16: nItem = 26; break;
                }
                break;
            // EA left out course 10 (items 27..30), which GM_ConvertCourseAndHoleToPar5EagleIndex has.
            case 11:
                switch (Game_GetCurHoleNum() + 1) {
                case 4: nItem = 31; break;
                case 9: nItem = 32; break;
                case 10: nItem = 33; break;
                case 12: nItem = 34; break;
                case 18: nItem = 35; break;
                }
                break;
            case 13:
                switch (Game_GetCurHoleNum() + 1) {
                case 3: nItem = 36; break;
                case 13: nItem = 37; break;
                case 15: nItem = 38; break;
                }
                break;
            case 15:
                switch (Game_GetCurHoleNum() + 1) {
                case 5: nItem = 39; break;
                case 14: nItem = 40; break;
                }
                break;
            case 14:
                switch (Game_GetCurHoleNum() + 1) {
                case 6: nItem = 41; break;
                case 9: nItem = 42; break;
                case 13: nItem = 43; break;
                case 18: nItem = 44; break;
                }
                break;
            case 16:
                switch (Game_GetCurHoleNum() + 1) {
                case 2: nItem = 45; break;
                case 6: nItem = 46; break;
                case 11: nItem = 47; break;
                }
                break;
            case 17:
                switch (Game_GetCurHoleNum() + 1) {
                case 1: nItem = 48; break;
                case 6: nItem = 49; break;
                case 7: nItem = 50; break;
                case 11: nItem = 51; break;
                case 15: nItem = 52; break;
                }
                break;
            case 18:
                switch (Game_GetCurHoleNum() + 1) {
                case 5: nItem = 53; break;
                case 8: nItem = 54; break;
                case 13: nItem = 55; break;
                case 18: nItem = 56; break;
                }
                break;
            case 19:
                switch (Game_GetCurHoleNum() + 1) {
                case 5: nItem = 57; break;
                case 9: nItem = 58; break;
                case 15: nItem = 59; break;
                case 18: nItem = 60; break;
                }
                break;
            case 20:
                switch (Game_GetCurHoleNum() + 1) {
                case 4: nItem = 61; break;
                case 10: nItem = 62; break;
                case 16: nItem = 63; break;
                }
                break;
            case 5:
                switch (Game_GetCurHoleNum() + 1) {
                case 4: nItem = 64; break;
                case 7: nItem = 65; break;
                case 13: nItem = 66; break;
                }
                break;
            case 8:
                switch (Game_GetCurHoleNum() + 1) {
                case 4: nItem = 67; break;
                case 6: nItem = 68; break;
                case 12: nItem = 69; break;
                case 16: nItem = 70; break;
                }
                break;
            case 4:
                switch (Game_GetCurHoleNum() + 1) {
                case 1: nItem = 71; break;
                case 7: nItem = 72; break;
                case 10: nItem = 73; break;
                case 18: nItem = 74; break;
                }
                break;
            }
            if (nItem >= 0 && !UserInfo_GetPar5EagleStat(&gpSaveData[nPlayer], 0, nItem)) return 1;
        }
    }
    return 0;
}

// Whole-round test 2 of the hole goals: without bPreview, whether the profile has won all 31 PGA
// TOUR tournaments; with it, whether this is the PGA TOUR (game mode 23) with 30 won and holing
// this ball would win (fn_800CF450). TW07: GM_Earnings_CheckWinAllTournaments.
u8 GM_Earnings_CheckWinAllTournaments(int nPlayer, u8 bPreview) {
    SaveProfile* pProfile;
    int i;
    int n;
    u8 bAll;
    PlayerNumber_t nProfile;

    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    pProfile = &gpSaveData[nProfile];
    if (!bPreview) {
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

// Whole-round test 4 of the hole goals: without bPreview, whether the profile has won any PGA TOUR
// tournament; with it, whether this is the PGA TOUR (game mode 23) and holing this ball would win
// (fn_800CF450). TW07: GM_Earnings_CheckFirstTournamentWin.
u8 GM_Earnings_CheckFirstTournamentWin(int nPlayer, u8 bPreview) {
    SaveProfile* pProfile;
    int i;
    u8 bAny;
    PlayerNumber_t nProfile;

    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    pProfile = &gpSaveData[nProfile];
    if (!bPreview) {
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

// Scales a money prize: nPoints rounded to $25, plus a bonus for each multiplier its flags switch
// on: bCourse the course's (GM_Earnings_GetCourseModifier), bTee the tees the player plays
// (gSession.nTeeSet 0..2 reads aMult[EARN_MULT_TEE + 2 - set], set 3 counts as x1), bHole the hole's pin set
// (gpGame->nPinSet, aMult[EARN_MULT_PINSET + set]); the table's multipliers are percentages. Each
// bonus (the base times the multiplier, less the base) is rounded to $25 by itself; the total is at
// least 0 and rounded to $25 again. pMoney, when given, gets the breakdown: the total (n0 and n24),
// the base, and the course, pin (n2C) and tee bonuses. 0 with mulligans. TW06:
// GM_Earnings_ComputeBonusModifiers (by position); TW07 calls the flags course, tee and pin.
s32 GM_Earnings_ComputeBonusModifiers(s32 nPoints, int nPlayer, u8 bCourse, u8 bTee, u8 bHole,
                                       CourseMoneyTracking* pMoney) {
    f32 fCourseBonus;           // fake match: whole dollars kept as floats and added as floats, as
    f32 fTeeBonus;              // the original does (s32 locals with float casts: 87%)
    f32 fHoleBonus;
    f32 fCourse;
    f32 fTee;
    f32 fHole;
    s32 nBase;
    s32 nTotal;

    if (Game_GetMulliganRule() != 0) return 0;
    fTee = 1.0f;
    fHole = fTee;
    fCourse = GM_Earnings_GetCourseModifier();
    switch (gSession.nTeeSet[nPlayer]) {
    case 0:
        fTee = (f32)gEarningsTable.aMult[EARN_MULT_TEE +2] / 100.0f;
        break;
    case 1:
        fTee = (f32)gEarningsTable.aMult[EARN_MULT_TEE +1] / 100.0f;
        break;
    case 2:
        fTee = (f32)gEarningsTable.aMult[EARN_MULT_TEE +0] / 100.0f;
        break;
    case 3:
        fTee = 1.0f;
        break;
    }
    switch (gpGame->nPinSet[Game_CurHoleIndex()]) {
    case 0:
        fHole = (f32)gEarningsTable.aMult[EARN_MULT_PINSET +0] / 100.0f;
        break;
    case 1:
        fHole = (f32)gEarningsTable.aMult[EARN_MULT_PINSET +1] / 100.0f;
        break;
    case 2:
        fHole = (f32)gEarningsTable.aMult[EARN_MULT_PINSET +2] / 100.0f;
        break;
    case 3:
        fHole = (f32)gEarningsTable.aMult[EARN_MULT_PINSET +3] / 100.0f;
        break;
    }
    nBase = roundToNearest25(nPoints);
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
    fCourseBonus = roundToNearest25((s32)fCourse);
    fTeeBonus = roundToNearest25((s32)fTee);
    fHoleBonus = roundToNearest25((s32)fHole);
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

// The current course's payout multiplier from the prize table (EARN_MULT_COURSE), a whole number
// (x1..x4); 1 for a course the table does not list. Course 7 has three, picked by
// Game_GetCurHoleNum (0..2). GM_Earnings_ComputeBonusModifiers and a menu message (GameUICommands.c
// GM_vGetWrapupData) read it.
f32 GM_Earnings_GetCourseModifier(void) {
    f32 fMult;

    fMult = 1.0f;
    switch (gpGame->nCurCourse) {
    case 0:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +0];
        break;
    case 2:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +1];
        break;
    case 1:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +2];
        break;
    case 6:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +3];
        break;
    case 10:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +4];
        break;
    case 11:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +5];
        break;
    case 13:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +6];
        break;
    case 15:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +7];
        break;
    case 14:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +8];
        break;
    case 3:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +9];
        break;
    case 12:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +10];
        break;
    case 9:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +11];
        break;
    case 22:
        fMult = gEarningsTable.aMult[EARN_MULT_COURSE +12];
        break;
    case 7:
        if (Game_GetCurHoleNum() == 0) {
            fMult = gEarningsTable.aMult[EARN_MULT_COURSE +13];
        }
        if (Game_GetCurHoleNum() == 1) {
            fMult = gEarningsTable.aMult[EARN_MULT_COURSE +14];
        }
        if (Game_GetCurHoleNum() == 2) {
            fMult = gEarningsTable.aMult[EARN_MULT_COURSE +15];
        }
        break;
    }
    return fMult;
}

// nReward scaled by the TOUR card percentage of the player's profile level (EARN_MULT_TOUR: levels
// 0 and 1 take the first; no active profile pays x1), rounded to $25. With pMoney the total goes in
// its n0 and n24 and the extra in nTourCard. 0 with mulligans on (Game_GetMulliganRule). TW06:
// GM_Earnings_ComputeTOURCardModifiers (by position).
int GM_Earnings_ComputeTOURCardModifiers(int nReward, int nPlayer, CourseMoneyTracking* pMoney) {
    f32 fMult;
    s32 nTotal;

    if (Game_GetMulliganRule() != 0) return 0;
    fMult = 1.0f;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 0) {
        switch (gpSaveData[gPlayers[nPlayer].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +0] / 100.0f;
            break;
        case 2:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +1] / 100.0f;
            break;
        case 3:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +2] / 100.0f;
            break;
        case 4:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +3] / 100.0f;
            break;
        case 5:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +4] / 100.0f;
            break;
        case 6:
            fMult = (f32)gEarningsTable.aMult[EARN_MULT_TOUR +5] / 100.0f;
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

// Whether the player can earn goal bonuses: a human player with an active profile, with mulligans
// off. The shot, putt and hole goal checks (GM_Earnings_CheckShotGoals, GM_Earnings_CheckPuttGoals,
// GM_Earnings_CheckHoleGoals) stop without it.
u8 GM_Earnings_AwardShotBonusToUser(int nPlayer) {
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMulliganRule() != 0) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    return 1;
}

// Give the player award nAward (a trophy ball) if they can still win it
// (GM_Earnings_AwardThisTrophyBallToUser): it is marked won with today's date and the profile
// flagged as changed (bChanged). Awards 0, 6, 9, 3 and 13 also keep the shot's replay (gReplayData,
// when one was recorded: bF10) in the profile's aReplay slots 0..4. Returns 1 when it was given; 0
// with mulligans on or without an active profile.
u8 GM_Earnings_AwardTrophyBall(int nPlayer, int nAward) {
    PlayerNumber_t nProfile;
    int nSlot;

    if (Game_GetMulliganRule() != 0) return 0;
    if (GM_Earnings_AwardThisTrophyBallToUser(nPlayer, nAward)) {
        nProfile = gPlayers[nPlayer].nIndex;
        if (gpSaveData[nProfile].bActive != 1) return 0;
        GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nProfile].aAward[nAward]);
        gpSaveData[nProfile].bChanged = 1;
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
            Mem_cpy(&gpSaveData[nProfile].aReplay[nSlot], &gReplayData, sizeof(gpSaveData->aReplay[0]));
        }
        return 1;
    }
    return 0;
}

// Run the shot goal check (GM_Earnings_CheckShotGoals) on pBall, bPreview passed on, and return how
// many awards it listed (Earnings_GetNumAwards). HoleScore and GameEffects ask it whether a shot
// earns a trophy ball.
s32 Earnings_CheckShotAwards(int nPlayer, Ball* pBall, u8 b) {
    GM_Earnings_CheckShotGoals(nPlayer, pBall, b);
    return Earnings_GetNumAwards();
}

// The same for the putt goals (GM_Earnings_CheckPuttGoals), which read the player's own ball: pBall
// is not used.
s32 Earnings_CheckPuttAwards(int nPlayer, Ball* pBall, u8 b) {
    GM_Earnings_CheckPuttGoals(nPlayer, b);
    return Earnings_GetNumAwards();
}

// Whether the player can still win award nAward (0..38): a human player with an active profile who
// has not won it; never with mulligans on, nor for 39 (the goal tables' mark for a money prize).
u8 GM_Earnings_AwardThisTrophyBallToUser(int nPlayer, int nAward) {
    if (Game_GetMulliganRule() != 0) return 0;
    if (nAward == 39) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    return gpSaveData[gPlayers[nPlayer].nIndex].aAward[nAward].bWon != 1;
}

// Mark pAward won with today's date (CalDate_GetToday). Returns 1 when it was not won before; 0,
// with nothing changed, for a CPU player, a player without an active profile or with mulligans on.
u8 GM_Earnings_GiveAwardToUser(int nPlayer, Award* pAward) {
    if (Game_GetMulliganRule() != 0) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (gpSaveData[gPlayers[nPlayer].nIndex].bActive != 1) return 0;
    if (pAward->bWon) return 0;
    pAward->bWon = 1;
    pAward->nDate = CalDate_GetToday();
    return 1;
}

// The three record checks below (TW07's HighScoreRecords::GetEndOfShotRecord, GetEndOfHoleRecord,
// GetEndOfGameRecord) share this start: nothing in some modes and states, or for a CPU player or
// one without a profile; the record holder's name is the profile's, or "User <n>" when the front
// end has no profile loaded in that slot.

// The end-of-shot record check: a shot on a par 4 or 5 that left class-1 ground and stayed in
// bounds is offered to record kind 1, its length in yards, while
// HighScoreRecords_CheckRecordGameSetting(1) allows it. a (TW07: setrecord) writes a place in. With
// bAll (TW07: firstPlaceOnly) only a new best (HighScoreRecords_CheckRecord gives 2 or 4) is
// listed, else any place; a hit goes into gShotRecordResults (the result) and gShotRecordKinds (kind 1).
// bCountStroke (TW07: predicted) counts the shot on the hole while it checks. Returns how many were
// listed (gNumRecordHits): 0 in game modes 12, 22 and 26 and the skill-zone modes, in "Random 18",
// with mulligans on, with gSession.uFlags 0x4000, for a CPU player or one without a profile, or for
// a ball off the course (no pCourse).
int HighScoreRecords_GetEndOfShotRecord(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    int nPar;
    u32 nClass;
    int nResult;

    gNumRecordHits = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12 || GM_Currently_SkillZoneMode() || Game_GetMode() == 22 || Game_GetMode()
        == 26) return 0;
    if (gpGame->bRandom18) return 0;
    if (Game_GetMulliganRule() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (gFEState.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = Math_Sqrt(fDx * fDx + fDz * fDz);
    if (pBall->pCourse == NULL) {
        if (bCountStroke) {
            gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
        }
        return 0;
    }
    nPar = GM_GetCurrentHolePar();
    nClass = gSurfaceTypes[pBall->nStartSurface].nClass;
    if (HighScoreRecords_CheckRecordGameSetting(1) && (nPar == 4 || nPar == 5) && nClass == 1
        && !GM_IsBallOOB(nPlayer, pBall)) {
        nResult = HighScoreRecords_CheckRecord(1, (s32)fDist, a, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gShotRecordResults[gNumRecordHits] = nResult;
            gShotRecordKinds[gNumRecordHits] = 1;
            gNumRecordHits++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return gNumRecordHits;
}

// The end-of-hole record check, made when the ball drops: a putt (the putter, club 25) is offered
// to record kind 2, its length in feet (3 x the yards from where it was struck), while
// HighScoreRecords_CheckRecordGameSetting(2) allows it. Parameters, exclusions and result as
// HighScoreRecords_GetEndOfShotRecord (without its par, ground and bounds tests); a hit goes into
// gPuttRecordResults and gPuttRecordKinds (kind 2).
int HighScoreRecords_GetEndOfHoleRecord(int nPlayer, Ball* pBall, int a, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    f32 fDist;
    f32 fDx;
    f32 fDz;
    int nResult;

    gNumRecordHits = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12 || GM_Currently_SkillZoneMode() || Game_GetMode() == 22 || Game_GetMode()
        == 26) return 0;
    if (gpGame->bRandom18) return 0;
    if (Game_GetMulliganRule() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (gFEState.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = Math_Sqrt(fDx * fDx + fDz * fDz);
    if (HighScoreRecords_CheckRecordGameSetting(2) && gPlayers[nPlayer].nClub == 25) {
        nResult = HighScoreRecords_CheckRecord(2, (s32)(3.0f * fDist), a, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gPuttRecordResults[gNumRecordHits] = nResult;
            gPuttRecordKinds[gNumRecordHits] = 2;
            gNumRecordHits++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return gNumRecordHits;
}

// The end-of-round record checks (after the last hole, GM_Earnings_PayRoundGoals). In game mode 22 (the
// long-drive contest) only record kind 9 (Player.nDriveScore); in the skill-zone modes only kind 8
// (Player.nSkillZonePoints). Otherwise, outside "Random 18": the round's strokes (kind 0), its
// greens in regulation (3, fn_800D1170), fairways hit (5, fn_800D0FBC), birdies or better (7),
// eagles or better (6) and putts (4). Each kind only while HighScoreRecords_CheckRecordGameSetting
// allows it. bSave writes a place in; with bAll only a new best (2 or 4) is listed, else any place;
// a hit goes into gRoundRecordResults (the result) and gRoundRecordKinds (the kind). bCountStroke
// counts the hole one stroke more while it checks. Returns how many were listed (gNumRecordHits): 0
// in game mode 12, with mulligans on, with gSession.uFlags 0x4000, for a CPU player or one without
// a profile.
int HighScoreRecords_GetEndOfGameRecord(int nPlayer, int bSave, u8 bCountStroke, u8 bAll) {
    char szName[32];
    int nProfile;
    int nResult;
    int nValue;
    int i;
    int nEagles;
    int nPutts;

    gNumRecordHits = 0;
    if (gSession.uFlags & 0x4000) return 0;
    if (Player_IsCPU(nPlayer)) return 0;
    if (Game_GetMode() == 12) return 0;
    if (Game_GetMulliganRule() != 0) return 0;
    nProfile = gPlayers[nPlayer].nIndex;
    if (gpSaveData[nProfile].bActive == 0) return 0;
    if (gFEState.aLoaded[nProfile] == 0) {
        sprintf(szName, "User %d", nProfile + 1);
    } else {
        strcpy(szName, gpSaveData[nProfile].szName);
    }
    if (Game_GetMode() == 22) {
        if (HighScoreRecords_CheckRecordGameSetting(9)) {
            nResult = HighScoreRecords_CheckRecord(9, gPlayers[nPlayer].nDriveScore, bSave, szName, nPlayer);
            if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
                gRoundRecordResults[gNumRecordHits] = nResult;
                gRoundRecordKinds[gNumRecordHits] = 9;
                gNumRecordHits++;
            }
        }
        return gNumRecordHits;
    }
    if (GM_Currently_SkillZoneMode()) {
        if (HighScoreRecords_CheckRecordGameSetting(8)) {
            nResult = HighScoreRecords_CheckRecord(8, gPlayers[nPlayer].nSkillZonePoints, bSave, szName,
                                                   nPlayer);
            if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
                gRoundRecordResults[gNumRecordHits] = nResult;
                gRoundRecordKinds[gNumRecordHits] = 8;
                gNumRecordHits++;
            }
        }
        return gNumRecordHits;
    }
    if (gpGame->bRandom18) return 0;
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    }
    nValue = GM_GetPlayerRoundStrokes(nPlayer);
    if (HighScoreRecords_CheckRecordGameSetting(0)) {
        nResult = HighScoreRecords_CheckRecord(0, nValue, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 0;
            gNumRecordHits++;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(3)) {
        nResult = HighScoreRecords_CheckRecord(3, fn_800D1170(nPlayer, 0), bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 3;
            gNumRecordHits++;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(5)) {
        nResult = HighScoreRecords_CheckRecord(5, fn_800D0FBC(nPlayer), bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 5;
            gNumRecordHits++;
        }
    }
    nValue = 0;
    for (i = 0; i < 18; i++) {
        if (gPlayers[nPlayer].nStrokes[i] <= GM_GetHoleIndexPar(i) - 1) {
            nValue++;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(7)) {
        nResult = HighScoreRecords_CheckRecord(7, nValue, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 7;
            gNumRecordHits++;
        }
    }
    nEagles = 0;
    for (i = 0; i < 18; i++) {
        if (gPlayers[nPlayer].nStrokes[i] < GM_GetHoleIndexPar(i) - 1) {
            nEagles++;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(6)) {
        nResult = HighScoreRecords_CheckRecord(6, nEagles, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 6;
            gNumRecordHits++;
        }
    }
    nPutts = 0;
    for (i = 0; i < 18; i++) {
        nPutts += gPlayers[nPlayer].nPutts[i];
    }
    if (HighScoreRecords_CheckRecordGameSetting(4)) {
        nResult = HighScoreRecords_CheckRecord(4, nPutts, bSave, szName, nPlayer);
        if ((bAll && (nResult == 2 || nResult == 4)) || (!bAll && nResult != 0)) {
            gRoundRecordResults[gNumRecordHits] = nResult;
            gRoundRecordKinds[gNumRecordHits] = 4;
            gNumRecordHits++;
        }
    }
    if (bCountStroke) {
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]--;
    }
    return gNumRecordHits;
}

// Whether nValue and szName are already among the top five of course k's record kind i
// (MC_MergeRecords asks before merging a memory card's records in). Never for a round whose holes
// are not one course's 1..18 (gpGame->bCustomRound).
u8 HighScoreRecords_RecordExist(int i, int nValue, const char* szName, int k) {
    RecordEntry* pRec;
    int j;

    if (gpGame->bCustomRound) return 0;
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

// The same for the skill-zone records: type i of recB[k] (the indexes HighScoreRecords_CheckRecord
// gives kind 8).
u8 HighScoreRecords_SkillZoneRecordExist(int i, int nValue, const char* szName, int k) {
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

// The same for the long-drive contest's records: type i of recC[k] (the indexes
// HighScoreRecords_CheckRecord gives kind 9).
u8 HighScoreRecords_LongDriveRecordExist(int i, int nValue, const char* szName, int k) {
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

// Whether nValue equals or beats nRecord for record kind nKind: kinds 0 (the round's strokes) and 4
// (its putts) go low, the others (1..3, 5..9) high; 0 for any other kind.
u8 HighScoreRecords_IsEqualOrBetter(int nKind, int nValue, int nRecord) {
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

// The skill-zone record table (recB's second index) of game mode n: 16 gives 0, 17 1, 13 2; any
// other mode 3 (none).
s32 HighScoreRecords_GetSkillZoneRecordType(s32 n) {
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

// The long-drive record table (recC's second index) of the long-drive contest's setting n
// (GameMode22 GameMode22_GetVariant): 0 and 1 as they are, anything else 2 (none).
s32 Earnings_GetLongDriveRecordType(s32 n) {
    switch (n) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

// Offer nValue and szName to record kind nKind: kinds 0..7 go to the current course's records
// (gSession.aCourseRecord) and then the all-time ones (recA), kind 8 to the skill-zone table of the
// game mode (recB, by Game_GetCurHoleNum), kind 9 and up to the long-drive table (recC). Returns 1
// for a place in the course's (or recB's, recC's) top five, 2 for its best, 3 and 4 the same for
// recA (tested last, so it wins); 0 for none, and always 0 for a round whose holes are not one
// course's 1..18 (gpGame->bCustomRound). With bSave the entry is written in (the ones below move
// down) and the player's profile is flagged as changed (bChanged); nPlayer 5 (MC.c) is no player.
// EA passes szName to sprintf as the format. The callers pass bSave unmasked and this function
// tests its low byte (the (u8) casts): EA's definition took a u8 (TW07: bool setrecord) behind an
// int prototype.
int HighScoreRecords_CheckRecord(int nKind, int nValue, int bSave, const char* szName, int nPlayer) {
    int nResult;
    int nPos;
    int n;
    RecordEntry* pFrom;
    RecordEntry* pTo;
    RecordEntry* pLast;
    RecordEntry* pRec;
    int j;
    PlayerNumber_t nProfile;

    nResult = 0;
    if (gpGame->bCustomRound) return 0;
    if (nKind < 8) {
        pLast = &gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][4];
        if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pLast->nValue)) {
            nResult = 1;
            nPos = 4;
            for (j = 3; j >= 0; j--) {
                pRec = &gSession.aCourseRecord[Game_GetCourse()].aRecord[nKind][j];
                if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pRec->nValue)) {
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
        pLast = &gSession.recA[nKind][4];
        if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pLast->nValue)) {
            nResult = 3;
            nPos = 4;
            for (j = 3; j >= 0; j--) {
                pRec = &gSession.recA[nKind][j];
                if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pRec->nValue)) {
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
        n = HighScoreRecords_GetSkillZoneRecordType(Game_GetMode());
        if (n != 3) {
            pLast = &gSession.recB[Game_GetCurHoleNum()][n][4];
            if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pLast->nValue)) {
                nResult = 1;
                nPos = 4;
                for (j = 3; j >= 0; j--) {
                    pRec = &gSession.recB[Game_GetCurHoleNum()][n][j];
                    if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pRec->nValue)) {
                        nPos = j;
                    }
                }
                if (nPos == 0) {
                    nResult = 2;
                }
                if ((u8)bSave) {
                    for (j = 4; j > nPos; j--) {
                        pFrom = &gSession.recB[Game_GetCurHoleNum()][n][j - 1];
                        pTo = &gSession.recB[Game_GetCurHoleNum()][n][j];
                        pTo->nValue = pFrom->nValue;
                        sprintf(pTo->szName, pFrom->szName);
                    }
                    pTo = &gSession.recB[Game_GetCurHoleNum()][n][nPos];
                    pTo->nValue = nValue;
                    sprintf(pTo->szName, szName);
                }
            }
        }
    } else {
        n = Earnings_GetLongDriveRecordType(GameMode22_GetVariant());
        if (n != 2) {
            pLast = &gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][n][4];
            if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pLast->nValue)) {
                nResult = 1;
                nPos = 4;
                for (j = 3; j >= 0; j--) {
                    pRec = &gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][n][j];
                    if (HighScoreRecords_IsEqualOrBetter(nKind, nValue, pRec->nValue)) {
                        nPos = j;
                    }
                }
                if (nPos == 0) {
                    nResult = 2;
                }
                if ((u8)bSave) {
                    for (j = 4; j > nPos; j--) {
                        pFrom = &gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][n][j - 1];
                        pTo = &gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][n][j];
                        pTo->nValue = pFrom->nValue;
                        sprintf(pTo->szName, pFrom->szName);
                    }
                    pTo = &gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][n][nPos];
                    pTo->nValue = nValue;
                    sprintf(pTo->szName, szName);
                }
            }
        }
    }
    if (nPlayer != 5) {
        nProfile = gPlayers[nPlayer].nIndex;
        if (nResult != 0 && (u8)bSave) {
            if (gpSaveData[nProfile].bActive) {
                gpSaveData[nProfile].bChanged = 1;
            }
        }
    }
    return nResult;
}

// Clear the player's per-shot bonus flags (bHitObject..bBunkerThisShot), before each shot (the
// place-ball and pre-shot states).
void GM_ClearShotBonusStats(int nPlayer) {
    gPlayers[nPlayer].bHitObject = 0;
    gPlayers[nPlayer].bHitPin = 0;
    gPlayers[nPlayer].bBunkerThisShot = 0;
    gPlayers[nPlayer].b30E = 0;
}

// Clear the flags GM_RecordBonusShotStats keeps over a hole (bBunkerThisHole..b312), when a hole
// starts (GM_InitForHole).
void GM_ClearHoleBonusStats(int nPlayer) {
    gPlayers[nPlayer].bBunkerThisHole = 0;
    gPlayers[nPlayer].b311 = 0;
    gPlayers[nPlayer].b312 = 0;
}

// Clear the player's money breakdown (Player.money) for a new game (GM_ClearDataForNewGame).
void GM_ClearGameBonusStats(int nPlayer) {
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

// Whether records of kind nKind count in this game: never in game modes 9 and 11 or with mulligans
// on. Kinds 0..9 also need no challenge running (PlayNow_IsChallengeRunning): the round's kinds (0,
// 3..7) a full round (GM_FullRoundOfGolf) of stroke scoring (GM_GetScoringType 0), the drive and the putt
// (1, 2) nothing more, kind 8 a skill-zone mode, kind 9 game mode 22 (the long-drive contest). Any
// other kind counts.
u8 HighScoreRecords_CheckRecordGameSetting(int nKind) {
    if (Game_GetMode() == 9 || Game_GetMode() == 11) return 0;
    if (Game_GetMulliganRule() != 0) return 0;
    switch (nKind) {
    case 0:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 1:
        if (PlayNow_IsChallengeRunning()) return 0;
        return 1;
    case 2:
        if (PlayNow_IsChallengeRunning()) return 0;
        return 1;
    case 3:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 4:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 5:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 6:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 7:
        if (PlayNow_IsChallengeRunning() || !GM_FullRoundOfGolf() || GM_GetScoringType()) return 0;
        return 1;
    case 8:
        if (PlayNow_IsChallengeRunning() || !GM_Currently_SkillZoneMode()) return 0;
        return 1;
    case 9:
        if (PlayNow_IsChallengeRunning() || Game_GetMode() != 22) return 0;
        return 1;
    }
    return 1;
}

// After a shot that stayed in bounds (GM_PlayerTookShot), when gpGame->b27B allows it and mulligans
// are off: the shot's statistics. A drive (the first stroke of a par 4 or 5, off class-1 ground)
// can be the round's longest (Player.nLongestDrive) and the profile's (nLongestDrive), in yards,
// and is counted in the profile (nDrives drives, nDriveDistance their yards). The first stroke's
// length goes in nC24; bFairwayHit marks a fairway hit (that first stroke on a par 4 or 5 finished
// on the fairway, the green or in the hole), bGreenInReg a green in regulation (on the green or in
// the hole, not on class-3 ground, in par - 2 strokes or fewer). A holed putt (club 25) can be the
// round's and the profile's longest (both nLongestPutt): 3 x Player.fA64, in feet.
void GM_RecordIndividualShotStats(int nPlayer) {
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
    if (Game_GetMulliganRule() != 0) return;
    pBall = &gPlayers[nPlayer].ball;
    nPar = GM_GetCurrentHolePar();
    nClass = gSurfaceTypes[pBall->nStartSurface].nClass;
    fDx = pBall->vPos[0] - gPlayers[nPlayer].vBall[0];
    fDz = pBall->vPos[2] - gPlayers[nPlayer].vBall[2];
    fDist = Math_Sqrt(fDx * fDx + fDz * fDz);
    nLie = pBall->nLie;
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    pSurface = Ter_GetSupportingWorldMaterial(pBall->pCourse, gPlayers[nPlayer].vBall);
    nProfile = gPlayers[nPlayer].nIndex;
    if ((nPar == 4 || nPar == 5) && nStrokes == 1 && nClass == 1) {
        if (fDist > gPlayers[nPlayer].nLongestDrive) {
            gPlayers[nPlayer].nLongestDrive = fDist;
        }
        if (gpSaveData[nProfile].bActive && fDist > gpSaveData[nProfile].nLongestDrive) {
            gpSaveData[nProfile].nLongestDrive = fDist;
            gpSaveData[nProfile].bChanged = 1;
        }
    }
    if (nStrokes == 1) {
        gPlayers[nPlayer].nC24 = fDist;
    }
    if ((nPar == 4 || nPar == 5) && nStrokes == 1 &&
        (nLie == LIE_FAIRWAY_e || nLie == LIE_GREEN_e || nLie == LIE_INCUP_e)) {
        gPlayers[nPlayer].bFairwayHit[Game_CurHoleIndex()] = 1;
    }
    if (pSurface != NULL && pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
        nStrokes <= nPar - 2) {
        gPlayers[nPlayer].bGreenInReg[Game_CurHoleIndex()] = 1;
    }
    if (gpSaveData[nProfile].bActive && (nPar == 4 || nPar == 5) && nStrokes == 1 && nClass == 1) {
        gpSaveData[nProfile].nDrives++;
        gpSaveData[nProfile].nDriveDistance += (s32)fDist;
        gpSaveData[nProfile].bChanged = 1;
    }
    if (GM_CheckForBallInHole(nPlayer) && gPlayers[nPlayer].nClub == 25) {
        nPutt = 3.0f * gPlayers[nPlayer].fA64;
        if (nPutt > gPlayers[nPlayer].nLongestPutt) {
            gPlayers[nPlayer].nLongestPutt = nPutt;
        }
        if (gpSaveData[nProfile].bActive && nPutt > gpSaveData[nProfile].nLongestPutt) {
            gpSaveData[nProfile].nLongestPutt = nPutt;
        }
    }
}

// After every shot (GM_PlayerTookShot calls it last), unless mulligans are on: the shot's bonus
// flags carry into the hole's. bBunkerThisShot sets bBunkerThisHole; b30E sets b311 and bit 1 of
// n308; a ball on the green or in the hole sets b312; bit 0 of n308 is set when that happened on
// the hole's first stroke of a par 4 or more (the green driven).
void GM_RecordBonusShotStats(int nPlayer) {
    int nLie;
    int nStrokes;
    int nPar;

    if (Game_GetMulliganRule() == 0) {
        nLie = gPlayers[nPlayer].ball.nLie;
        if (gPlayers[nPlayer].bBunkerThisShot) {
            gPlayers[nPlayer].bBunkerThisHole = 1;
        }
        if (gPlayers[nPlayer].b30E) {
            gPlayers[nPlayer].b311 = 1;
            gPlayers[nPlayer].n308 |= 2;
        }
        if (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) {
            gPlayers[nPlayer].b312 = 1;
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
        nPar = GM_GetCurrentHolePar();
        if (nStrokes == 1 && nPar >= 4 && gPlayers[nPlayer].b312) {
            gPlayers[nPlayer].n308 |= 1;
        }
    }
}

// The player's hole is over (holed, or picked up at the stroke limit; GM_PlayerTookShot): it is
// marked in gpGame->b16C and, when gpGame->b27C allows it, no challenge runs and mulligans are off,
// added to the profile's statistics: fairways (par 4 and 5 holes nFairways, hit nFairwaysHit:
// bFairwayHit), greens in regulation (nHoles, hit nGreensHit: bGreenInReg), putts (holes with fewer
// than 10: nPuttHoles, their putts nPutts) and the score against par (nHolesInOne hole in one,
// nAlbatrosses albatross, nEagles eagle, nBirdies birdie, nPars par, nBogeys bogey, nDoubleBogeys
// worse). An eagle or better on a par 5 is marked, with the date, in the profile's par-5 table
// (a5004, a504C). The profile is flagged as changed (bChanged).
void GM_RecordIndividualHoleStats(int nPlayer) {
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
        switch (PlayNow_IsChallengeRunning()) {    // fake match: as in GM_RecordIndividualRoundStats
        case 0:
            break;
        default:
            return;
        }
        if (Game_GetMulliganRule() == 0) {
            f32* pPos = gPlayers[nPlayer].ball.vPos;

            // How far the ball ended from vBall; the result is not used.
            dx = pPos[0] - gPlayers[nPlayer].vBall[0];
            dz = pPos[2] - gPlayers[nPlayer].vBall[2];
            Math_Sqrt(dx * dx + dz * dz);
            nProfile = gPlayers[nPlayer].nIndex;
            nPar = GM_GetCurrentHolePar();
            if (gpSaveData[nProfile].bActive) {
                gpSaveData[nProfile].bChanged = 1;
            } else {
                return;
            }
            if (nPar == 4 || nPar == 5) {
                if (gPlayers[nPlayer].bFairwayHit[Game_CurHoleIndex()]) {
                    gpSaveData[nProfile].nFairways++;
                    gpSaveData[nProfile].nFairwaysHit++;
                } else {
                    gpSaveData[nProfile].nFairways++;
                }
            }
            if (gPlayers[nPlayer].bGreenInReg[Game_CurHoleIndex()]) {
                gpSaveData[nProfile].nHoles++;
                gpSaveData[nProfile].nGreensHit++;
            } else {
                gpSaveData[nProfile].nHoles++;
            }
            if (gPlayers[nPlayer].nPutts[Game_CurHoleIndex()] < 10) {
                gpSaveData[nProfile].nPuttHoles++;
                gpSaveData[nProfile].nPutts += gPlayers[nPlayer].nPutts[Game_CurHoleIndex()];
            }
            nDiff = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] - GM_GetCurrentHolePar();
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1) {
                gpSaveData[nProfile].nHolesInOne++;
            } else if (nDiff == -3) {
                gpSaveData[nProfile].nAlbatrosses++;
            } else if (nDiff == -2) {
                gpSaveData[nProfile].nEagles++;
            } else if (nDiff == -1) {
                gpSaveData[nProfile].nBirdies++;
            } else if (nDiff == 0) {
                gpSaveData[nProfile].nPars++;
            } else if (nDiff == 1) {
                gpSaveData[nProfile].nBogeys++;
            } else if (nDiff > 1) {
                gpSaveData[nProfile].nDoubleBogeys++;
            }
            if (gpSaveData[nProfile].bActive && nPar == 5 &&
                gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] <= 3) {
                nMarked = GM_ConvertCourseAndHoleToPar5EagleIndex(gpGame->nCurCourse, Game_GetCurHoleNum());
                fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
                nDate = FE_DateToInt(nMonth, nDay, nYear);
                if (nMarked != -1) {
                    UserInfo_SetPar5EagleStat(&gpSaveData[nProfile], 0, nMarked, 1);
                    UserInfo_SetPar5EagleStat(&gpSaveData[nProfile], 1, nMarked, nDate);
                }
            }
        }
    }
}

// At the end of the round (its last hole, outside a playoff; GM_EndOfGolferTurn_HoleFinished), when
// gpGame->b27D allows it, no challenge runs and mulligans are off: a full round is counted in the
// profile (nRounds), and in stroke play (gpGame->nScoringType 0) also in nStrokeRounds, with its
// strokes added to nStrokeRoundStrokes and kept as the best round (nBestRound) when lower. The
// profile is flagged as changed (bChanged).
void GM_RecordIndividualRoundStats(int nPlayer) {
    PlayerNumber_t nProfile;
    int nStrokes;

    if (gpGame->b27D) {
        // fake match: a plain "if (...) return" folds the branch over a branch
        switch (PlayNow_IsChallengeRunning()) {
        case 0:
            break;
        default:
            return;
        }
        if (Game_GetMulliganRule() == 0) {
            nProfile = gPlayers[nPlayer].nIndex;
            if (gpSaveData[nProfile].bActive) {
                if (GM_FullRoundOfGolf()) {
                    gpSaveData[nProfile].nRounds++;
                    if (gpGame->nScoringType == 0) {
                        nStrokes = GM_GetPlayerRoundStrokes(nPlayer);
                        gpSaveData[nProfile].nStrokeRounds++;
                        gpSaveData[nProfile].nStrokeRoundStrokes += nStrokes;
                        if (gpSaveData[nProfile].nBestRound == 0) {
                            gpSaveData[nProfile].nBestRound = nStrokes;
                        } else if (nStrokes < gpSaveData[nProfile].nBestRound) {
                            gpSaveData[nProfile].nBestRound = nStrokes;
                        }
                    }
                }
                gpSaveData[nProfile].bChanged = 1;
            }
        }
    }
}

// How many awards the last shot, putt or hole goal check listed (gNumAwards).
s32 Earnings_GetNumAwards(void) {
    return gNumAwards;
}

// Award i of those the shot goal check listed (gShotAwards).
s32 Earnings_GetShotAwardId(s32 i) {
    return gShotAwards[i];
}

// Award i of those the putt goal check listed (gPuttAwards).
s32 Earnings_GetPuttAwardId(s32 i) {
    return gPuttAwards[i];
}

// Award i of those the hole goal check listed (gHoleAwards).
s32 Earnings_GetHoleAwardId(s32 i) {
    return gHoleAwards[i];
}

// Whether the player has earned PGA TOUR award nAward (23..38; any other gives 0), with mulligans
// off and an active profile. nPlayer indexes gpSaveData directly (not through Player.nIndex) and is
// passed on as the player too. The season awards need the season over (the last round of the last
// event) and 15 or more events played: 24 a top 25 in every event played, 25 leading 15 of the 28
// tour statistics, 26 leading the par 3, 4 and 5 birdie statistics, 27 over 4.25 birdies a round,
// 30 under par in every event played, 33 a scoring average under 68.17. The others at any time: 23
// more than 18 holes in one (SaveProfile.nHolesInOne), 28 GameMode5's PlayNow_GetCalendarFlag with
// the best medal (PlayNow_GetMedal 0), 29 leading the career money list, 31 tour.nWinStreak over
// 11, 32 a round under 59 strokes, 34 more than 100 events (nEventsStarted) with over 28% won, 35
// nParRoundStreak over 66, 36 nMajorWins over 18, 37 ten or more wins in a season, 38 more season
// winnings than Tiger Woods's $9,188,321 of 2000.
u8 Earnings_IsTourAwardEarned(int nPlayer, int nAward) {
    SaveProfile* pProfile;
    u8 bSeasonEnd;
    u8 bFullSeason;
    int i;
    int nLeads;
    PgaStatCounts* pStats;
    TourSeason* pTour;

    if (Game_GetMulliganRule() != 0) return 0;
    pProfile = &gpSaveData[nPlayer];
    if (!pProfile->bActive) return 0;
    pStats = &pProfile->tour.aStats[PGA_USER_GOLFER];
    pTour = &pProfile->tour;
    if ((pProfile->tour.nEvent == -1 || GameModeDriverPGATour_GetNextEvent() == -1) &&
        pProfile->tour.nRound + 1 == GameModeDriverPGATour_GetRounds(pProfile->tour.nEvent)) {
        bSeasonEnd = 1;
    } else {
        bSeasonEnd = 0;
    }
    // fake match: the (int) changes nothing in C, but gives EA's signed compare (srawi; subfc; adde)
    bFullSeason = (int)pStats->nEvents >= 15;

    switch (nAward) {
    case 23:
        return pProfile->nHolesInOne > 18;
    case 24:
        // Top 25 in every tournament played.
        if (bSeasonEnd && bFullSeason) {
            for (i = 0; i < GM_PgaTourMode_GetNEvents(); i++) {
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
                if (GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, i)) {
                    nLeads++;
                }
            }
            if (nLeads >= 15) return 1;
        }
        return 0;
    case 26:
        if (bSeasonEnd && bFullSeason && GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR3BIRDS) &&
            GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR4BIRDS) &&
            GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PAR5BIRDS)) {
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
        if (PlayNow_GetCalendarFlag() && PlayNow_GetMedal() == 0) return 1;
        return 0;
    case 29:
        return GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_CAREER_WINNINGS) != 0;
    case 30:
        // Under par in every tournament played.
        if (bSeasonEnd && bFullSeason) {
            for (i = 0; i < GM_PgaTourMode_GetNEvents(); i++) {
                if (pProfile->tour.aEvent[i].nUserRankType == 0) continue;
                if (pProfile->tour.aEvent[i].nUserRankType == 2 &&
                    pProfile->tour.aEvent[i].nUserScore < pProfile->tour.aEvent[i].nEventPar) continue;
                return 0;
            }
            return 1;
        }
        return 0;
    case 31:
        return pTour->nWinStreak > 11;
    case 32:
        return GM_GetPlayerRoundStrokes(nPlayer) < 59;
    case 33:
        if (bSeasonEnd && bFullSeason &&
            GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SCORING) < 68.17f) {
            return 1;
        }
        return 0;
    case 34:
        if (pTour->nEventsStarted > 100 && (f32)pStats->nCareerWins / (f32)pTour->nEventsStarted > 0.28f) {
            return 1;
        }
        return 0;
    case 35:
        return pTour->nParRoundStreak > 66;
    case 36:
        return pTour->nMajorWins > 18;
    case 37:
        return pStats->nSeasonWins > 9;
    case 38:
        // Tiger Woods's season record, $9,188,321 in 2000.
        return pStats->nSeasonWinnings > 9188321;
    }
    return 0;
}

// The message index of award i (gAwardMessageIds): awards 0..22 are their own; the PGA TOUR awards
// 23..38 index the tour awards' message list (GUI_QueueMessage kind 6).
s32 Earnings_GetAwardMessageId(s32 i) {
    return gAwardMessageIds[i];
}
