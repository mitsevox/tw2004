// GameHoleContests.c (our name): the hole contests of a multiplayer round in game mode 0 (stroke
// play), 1 or 2. At the start of the round up to three holes are drawn: a par 4 or 5 for the
// longest drive (gHoleContestLongestDriveHole), a par 3 for closest to the pin (gHoleContestClosestToPinHole) and, one round in
// five, another par 3 with a $100,000 prize for a hole in one (gHoleContestHoleInOneHole). Each player's result
// on the contest hole is kept by player (gHoleContestPlayerResult distances), ranked into a result table
// (gHoleContestPlaceName names, gHoleContestPlaceDistance distances), and the winner (gHoleContestWinner) is paid $2,500.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "frontend/fe.h"

s32  gHoleContestLongestDriveHole = -1;         // the longest-drive hole, -1 none
s32  gHoleContestClosestToPinHole = -1;         // the closest-to-the-pin hole, -1 none
s32  gHoleContestHoleInOneHole = -1;         // the hole-in-one prize hole, -1 none
// .bss/.sbss: defined in reverse address order (CodeWarrior lays them out last-defined-first).
f32  gHoleContestPlayerResult[5];           // per player: the drive's length or the distance from the pin
s32  gHoleContestPlaceDistance[5];           // by place: the distance, -1 no result
char gHoleContestPlaceName[5][14];       // by place: the name shown with the result
s32  gHoleContestEverWon;
s32  gHoleContestWinner;              // the contest's winner, 5 = nobody
u8   gHoleContestDecided;              // the contest on this hole is decided
u8   gHoleContestWon;              // a contest has a winner (HoleContest_RankResults), or the hole in one was made

u8   fn_800D304C(int nHole);    // a flag of the hole's course data (byte 0x35): the drive can count
u8   fn_800D0D54(int nPlayer);  // the ball lies on a fairway, the green or in the cup

u8   HoleContest_RoundHasContests(void);
void HoleContest_DrawHoles(void);
void HoleContest_RankResults(void);

// A new round of hole contests (HoleContest_InitForHole on the round's first hole): no contest
// holes and nothing won yet; when the round has contests (HoleContest_RoundHasContests) the holes
// are drawn (HoleContest_DrawHoles).
void HoleContest_NewRound(void) {
    gHoleContestLongestDriveHole = -1;
    gHoleContestClosestToPinHole = -1;
    gHoleContestHoleInOneHole = -1;
    gHoleContestWon = 0;
    gHoleContestDecided = 0;
    if (HoleContest_RoundHasContests()) {
        HoleContest_DrawHoles();
    }
}

// Whether this round has hole contests: no mulligans, more than one player, a full round of golf,
// gSession.a8[0] clear, no Play Now challenge running (PlayNow_IsChallengeRunning) or calendar flag
// (PlayNow_GetCalendarFlag), and game mode 0 (stroke play), 1 (match play) or 2 (skins).
u8 HoleContest_RoundHasContests(void) {
    if (gpGame->nMulligans != 0) return 0;
    if (gSession.nNumPlayers == 1) return 0;
    if (!GM_FullRoundOfGolf()) return 0;
    if (gSession.a8[0] != 0) return 0;
    if (PlayNow_IsChallengeRunning()) return 0;
    if (PlayNow_GetCalendarFlag()) return 0;
    if (Game_GetMode() == 0 || Game_GetMode() == 1 || Game_GetMode() == 2) {
        return 1;
    }
    return 0;
}

// Draws the contest holes at random among the round's 18 (Misc_RandFunc until one fits): the
// longest drive on a par 4 or 5 whose course data allows it (fn_800D304C, byte 0x35); closest to
// the pin on a par 3; and one round in five (20 in 100) the hole-in-one prize on another par 3. A
// contest with no hole that fits gets -1.
void HoleContest_DrawHoles(void) {
    u8 bFound;
    int i;

    bFound = 0;
    for (i = 0; i < 18; i++) {
        if (Course_GetHolePar(i) > 3 && fn_800D304C(i)) {
            bFound = 1;
        }
    }
    if (bFound) {
        gHoleContestLongestDriveHole = Misc_RandFunc(0) % 18;
        while (Course_GetHolePar(gHoleContestLongestDriveHole) == 3 || !fn_800D304C(gHoleContestLongestDriveHole)) {
            gHoleContestLongestDriveHole = Misc_RandFunc(0) % 18;
        }
    } else {
        gHoleContestLongestDriveHole = -1;
    }

    bFound = 0;
    for (i = 0; i < 18; i++) {
        if (Course_GetHolePar(i) == 3) {
            bFound = 1;
        }
    }
    if (bFound) {
        gHoleContestClosestToPinHole = Misc_RandFunc(0) % 18;
        while (Course_GetHolePar(gHoleContestClosestToPinHole) > 3) {
            gHoleContestClosestToPinHole = Misc_RandFunc(0) % 18;
        }
    } else {
        gHoleContestClosestToPinHole = -1;
    }

    if ((int)(Misc_RandFunc(0) % 100) < 20) {
        bFound = 0;
        for (i = 0; i < 18; i++) {
            if (Course_GetHolePar(i) == 3 && i != gHoleContestClosestToPinHole) {
                bFound = 1;
            }
        }
        if (bFound) {
            gHoleContestHoleInOneHole = Misc_RandFunc(0) % 18;
            while (Course_GetHolePar(gHoleContestHoleInOneHole) > 3 || gHoleContestHoleInOneHole == gHoleContestClosestToPinHole) {
                gHoleContestHoleInOneHole = Misc_RandFunc(0) % 18;
            }
        } else {
            gHoleContestHoleInOneHole = -1;
        }
    }
}

// Whether the current hole is the longest-drive hole. While the calendar flag is set
// (PlayNow_GetCalendarFlag) the 18th hole (index 17) always is, outside a playoff (gpGame->bD4
// clear).
u8 HoleContest_IsLongestDriveHole(void) {
    if (PlayNow_GetCalendarFlag() && Game_CurHoleIndex() == 17 && gpGame->bD4 == 0) {
        return 1;
    }
    return gHoleContestLongestDriveHole == Game_CurHoleIndex();
}

// Whether the current hole is the closest-to-the-pin hole. While the calendar flag is set
// (PlayNow_GetCalendarFlag) the 17th hole (index 16) always is, outside a playoff (gpGame->bD4
// clear).
u8 HoleContest_IsClosestToPinHole(void) {
    if (PlayNow_GetCalendarFlag() && Game_CurHoleIndex() == 16 && gpGame->bD4 == 0) {
        return 1;
    }
    return gHoleContestClosestToPinHole == Game_CurHoleIndex();
}

// Whether the hole-in-one prize ($100,000) is on the current hole.
u8 HoleContest_IsHoleInOneHole(void) {
    return Game_CurHoleIndex() == gHoleContestHoleInOneHole;
}

// Whether the player whose turn it is (lbl_80282278) has not played a stroke on the current hole
// yet, so is on the tee (STATEFUNC_SwingInit shows a contest's intro only then).
u8 HoleContest_IsCurrentPlayerOnTee(void) {
    return gPlayers[lbl_80282278].nStrokes[Game_CurHoleIndex()] == 0;
}

// Whether the contest on the current hole can be decided: a longest-drive or closest-to-the-pin
// hole, not decided yet (gHoleContestDecided), and every player has played his tee shot.
// GM_PlayerTookShot then pays the winner (HoleContest_PayWinner) and shows the result.
u8 HoleContest_IsReadyToDecide(void) {
    int i;
    u8 bDone;

    if (!HoleContest_IsLongestDriveHole() && !HoleContest_IsClosestToPinHole()) return 0;
    if (gHoleContestDecided) return 0;
    bDone = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] == 0) {
            bDone = 0;
        }
    }
    return bDone;
}

// fake match: stands in for a function the original linker stripped. The file's pool has 1.0
// first, before HoleContest_InitForHole's -1.0 (HoleContest_GetWinnerShotKind, its only user, comes
// last); its body is unknown, this one only reproduces the order.
static f32 GameHoleContests_StrippedFn(f32 x) {
    return x + 1.0f;
}

// A new hole (GM_InitForHole): every player's contest result cleared (-1.0), the result table
// emptied, not decided and no winner (5); on the round's first hole (GM_OnFirstSelectedHole) also a
// new round of contests (HoleContest_NewRound).
void HoleContest_InitForHole(void) {
    int i;
    int n;
    int j;

    n = gNumPlayersSetUp;
    i = 0;
    while (n-- > 0) {
        gHoleContestPlayerResult[i++] = -1.0f;
    }
    for (j = 0; j < 5; j++) {
        strcpy(gHoleContestPlaceName[j], "");
        gHoleContestPlaceDistance[j] = 0;
    }
    gHoleContestDecided = 0;
    gHoleContestWinner = 5;
    if (GM_OnFirstSelectedHole()) {
        HoleContest_NewRound();
    }
}

// After a player's shot (GM_PlayerTookShot). On the longest-drive hole a first stroke that
// fn_800D0D54 accepts (from fairway-class ground to the fairway, the green or the cup) records its
// length (fn_800D0550); on the closest-to-the-pin hole a first stroke onto the green records its
// distance from the pin in feet (3 x fn_800D0478), holed 0; both then re-rank
// (HoleContest_RankResults). On the hole-in-one prize hole a holed first stroke wins $100,000
// (message 0x74 for an active profile) and marks a contest won. A mulligan's shot (bC2F) does not
// count.
void HoleContest_PlayerTookShot(int nPlayer) {
    CourseMoneyTracking money;
    s32 nIndex;

    if (HoleContest_IsLongestDriveHole()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && fn_800D0D54(nPlayer) &&
            gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = fn_800D0550(nPlayer);
        }
        HoleContest_RankResults();
    }
    if (HoleContest_IsClosestToPinHole()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && gPlayers[nPlayer].ball.nLie == 9 &&
            gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = 3.0f * fn_800D0478(nPlayer);
        }
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 &&
            gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = 0.0f;
        }
        HoleContest_RankResults();
    }
    if (HoleContest_IsHoleInOneHole()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && GM_CheckForBallInHole(nPlayer) &&
            gPlayers[nPlayer].bC2F == 0) {
            gHoleContestWon = 1;
            gHoleContestEverWon = 1;
            Mem_set(&money, 0, sizeof(money));
            money.n24 = 100000;
            money.n0 = 100000;
            money.n38 = 100000;
            GM_Earnings_AwardMoney(nPlayer, 100000, &money);
            nIndex = gPlayers[nPlayer].nIndex;
            if (gpSaveData[nIndex].bActive) {
                GUI_QueueMessage(0, 0x74, 100000, nIndex);
            }
        }
    }
}

// Ranks the players on the contest hole into the result table (gHoleContestPlaceName,
// gHoleContestPlaceDistance): the longest drives first (or the shots closest to the pin), then the
// players with no result (distance -1). Each is named by a CPU golfer's nickname (the last name
// when the nickname is "NA") or the player's profile name ("User n" while no profile is loaded).
// The first place is the winner (gHoleContestWinner) and marks a contest won. Nothing on the
// hole-in-one prize hole.
// fake match: the (u32) casts on the player index; with a signed index the compiler walks one
// pointer instead of keeping the array start and the offset apart (see GoTerrain fn_80032518).
void HoleContest_RankResults(void) {
    s32 aRank[5];               // per player: the place in the table, -1 not placed yet
    char szName[32];            // the stack frame gives 32 bytes; the real size is not known
    int nRank;
    int nBest;
    f32 fBest;
    int i;
    int j;
    s32 nIndex;

    if (HoleContest_IsHoleInOneHole()) return;
    gHoleContestWinner = 5;
    nRank = 0;
    aRank[0] = -1;
    aRank[1] = -1;
    aRank[2] = -1;
    aRank[3] = -1;
    aRank[4] = -1;
    for (j = 0; j < 5; j++) {
        strcpy(gHoleContestPlaceName[j], "");
        gHoleContestPlaceDistance[j] = 0;
    }
    if (HoleContest_IsLongestDriveHole()) {
        for (j = 0; j < gNumPlayersSetUp; j++) {
            fBest = 0.0f;
            nBest = 5;
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (aRank[(u32)i] == -1 && gHoleContestPlayerResult[(u32)i] != -1.0f && gHoleContestPlayerResult[(u32)i] > fBest) {
                    nBest = i;
                    fBest = gHoleContestPlayerResult[(u32)i];
                }
            }
            if (nBest != 5) {
                if (Player_IsCPU(nBest)) {
                    if (strcmp(gPlayers[nBest].golfer.szNick, "NA") != 0) {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[nBest].golfer.szNick);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[nBest].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[nBest].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", nBest + 1);
                        strcpy(gHoleContestPlaceName[nRank], szName);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gpSaveData[nIndex].szName);
                    }
                }
                aRank[nBest] = nRank;
                gHoleContestPlaceDistance[nRank] = fBest;
                if (nRank == 0) {
                    gHoleContestWinner = nBest;
                }
                nRank++;
            }
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (gHoleContestPlayerResult[(u32)i] == -1.0f) {
                if (Player_IsCPU(i)) {
                    if (strcmp(gPlayers[(u32)i].golfer.szNick, "NA") != 0) {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[(u32)i].golfer.szNick);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[(u32)i].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[(u32)i].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", i + 1);
                        strcpy(gHoleContestPlaceName[nRank], szName);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gpSaveData[nIndex].szName);
                    }
                }
                gHoleContestPlaceDistance[nRank] = -1;
                aRank[(u32)i] = nRank;
                nRank++;
            }
        }
    }
    if (HoleContest_IsClosestToPinHole()) {
        for (j = 0; j < gNumPlayersSetUp; j++) {
            fBest = 9999.0f;
            nBest = 5;
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (aRank[(u32)i] == -1 && gHoleContestPlayerResult[(u32)i] != -1.0f && gHoleContestPlayerResult[(u32)i] < fBest) {
                    nBest = i;
                    fBest = gHoleContestPlayerResult[(u32)i];
                }
            }
            if (nBest != 5) {
                if (Player_IsCPU(nBest)) {
                    if (strcmp(gPlayers[nBest].golfer.szNick, "NA") != 0) {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[nBest].golfer.szNick);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[nBest].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[nBest].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", nBest + 1);
                        strcpy(gHoleContestPlaceName[nRank], szName);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gpSaveData[nIndex].szName);
                    }
                }
                aRank[nBest] = nRank;
                gHoleContestPlaceDistance[nRank] = fBest;
                if (nRank == 0) {
                    gHoleContestWinner = nBest;
                }
                nRank++;
            }
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (gHoleContestPlayerResult[(u32)i] == -1.0f) {
                if (Player_IsCPU(i)) {
                    if (strcmp(gPlayers[(u32)i].golfer.szNick, "NA") != 0) {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[(u32)i].golfer.szNick);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gPlayers[(u32)i].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[(u32)i].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", i + 1);
                        strcpy(gHoleContestPlaceName[nRank], szName);
                    } else {
                        strcpy(gHoleContestPlaceName[nRank], gpSaveData[nIndex].szName);
                    }
                }
                gHoleContestPlaceDistance[nRank] = -1;
                nRank++;
            }
        }
    }
    if (gHoleContestWinner != 5) {
        gHoleContestWon = 1;
        gHoleContestEverWon = 1;
    }
}

// The name at place nPlace (0-based) of the contest's result table, for the UI's record list
// (GameUICommands.c fn_8008886C, record kind 3).
char* HoleContest_GetPlaceName(int nPlayer) {
    return gHoleContestPlaceName[nPlayer];
}

// The result at place nPlace (0-based) of the contest's result table: the drive's length or the
// distance from the pin in feet, -1 none (GameUICommands.c fn_80088AD4, record kind 3).
s32 HoleContest_GetPlaceDistance(int nPlayer) {
    return gHoleContestPlaceDistance[nPlayer];
}

// Whether a hole contest was won this round (a winner ranked, or the hole in one made); FE message
// fn_80089590 case 0 asks.
u8 HoleContest_IsWonThisRound(void) {
    return gHoleContestWon;
}

// Whether a hole contest has been won since the game started: gHoleContestEverWon is set with
// gHoleContestWon and never cleared (FE message fn_80089590 case 1).
s32 HoleContest_WasEverWon(void) {
    return gHoleContestEverWon;
}

// Decides the contest on the current hole (GM_PlayerTookShot, once HoleContest_IsReadyToDecide):
// marked decided, and its winner, if any while a contest is won this round, is paid $2,500.
void HoleContest_PayWinner(void) {
    CourseMoneyTracking money;

    gHoleContestDecided = 1;
    if (gHoleContestWinner == 5) return;
    if (!gHoleContestWon) return;
    Mem_set(&money, 0, sizeof(money));
    money.n24 = 2500;
    money.n0 = 2500;
    money.n38 = 2500;
    GM_Earnings_AwardMoney(gHoleContestWinner, 2500, &money);
}

// How close the contest winner's ball is, while a contest is won this round: 1 in the cup
// (bPlanReady clear), 2 within a foot of the pin, else 0 (FE message fn_80089B8C).
s32 HoleContest_GetWinnerShotKind(void) {
    if (gHoleContestWon) {
        if (gPlayers[gHoleContestWinner].ball.nLie == LIE_INCUP_e && gPlayers[gHoleContestWinner].bPlanReady == 0) {
            return 1;
        }
        // fake match: the original loads gHoleContestWinner again for the call; the volatile read does that
        if (3.0f * fn_800D0478(*(volatile s32*)&gHoleContestWinner) < 1.0f) {
            return 2;
        }
    }
    return 0;
}
