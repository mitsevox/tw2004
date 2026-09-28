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
u8   gHoleContestWon;              // a contest has a winner (fn_800DA6D0), or the hole in one was made

u8   fn_800D304C(int nHole);    // a flag of the hole's course data (byte 0x35): the drive can count
u8   fn_800D0D54(int nPlayer);  // the ball lies on a fairway, the green or in the cup

u8   fn_800D9E5C(void);
void fn_800D9F34(void);
void fn_800DA6D0(void);

// A new round: no contest holes until they are drawn.
void fn_800D9E14(void) {
    gHoleContestLongestDriveHole = -1;
    gHoleContestClosestToPinHole = -1;
    gHoleContestHoleInOneHole = -1;
    gHoleContestWon = 0;
    gHoleContestDecided = 0;
    if (fn_800D9E5C()) {
        fn_800D9F34();
    }
}

// Whether this round has hole contests: several players, a round of every hole, no mulligans,
// gSession.a8[0] clear, neither GameMode5 test (PlayNow_IsChallengeRunning, PlayNow_GetCalendarFlag) and game
// mode 0, 1 or 2.
u8 fn_800D9E5C(void) {
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

// Draws the contest holes at random among the round's 18.
void fn_800D9F34(void) {
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

// The longest drive is played on this hole. On the round's last hole of a GameMode5 challenge the
// contest is always on.
u8 fn_800DA174(void) {
    if (PlayNow_GetCalendarFlag() && Game_CurHoleIndex() == 17 && gpGame->bD4 == 0) {
        return 1;
    }
    return gHoleContestLongestDriveHole == Game_CurHoleIndex();
}

// Closest to the pin is played on this hole (always on the 17th of a GameMode5 challenge).
u8 fn_800DA1D4(void) {
    if (PlayNow_GetCalendarFlag() && Game_CurHoleIndex() == 16 && gpGame->bD4 == 0) {
        return 1;
    }
    return gHoleContestClosestToPinHole == Game_CurHoleIndex();
}

// The hole-in-one prize is on this hole.
u8 fn_800DA234(void) {
    return Game_CurHoleIndex() == gHoleContestHoleInOneHole;
}

// The player whose turn it is has not played a stroke on this hole yet: they are on the tee.
u8 fn_800DA264(void) {
    return gPlayers[lbl_80282278].nStrokes[Game_CurHoleIndex()] == 0;
}

// The contest on this hole is ready to be decided: every player has played their tee shot.
u8 fn_800DA2AC(void) {
    int i;
    u8 bDone;

    if (!fn_800DA174() && !fn_800DA1D4()) return 0;
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
// first, before fn_800DA36C's -1.0 (fn_800DADC0, its only user, comes last); its body is unknown,
// this one only reproduces the order.
static f32 GameHoleContests_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Clears every player's contest result and the result table (winner: nobody); on the round's first
// hole (GM_OnFirstSelectedHole) also draws the contest holes again (fn_800D9E14).
void fn_800DA36C(void) {
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
        fn_800D9E14();
    }
}

// After a player's shot: a tee shot on a contest hole enters the contest (the drive's length when it
// stays on the fairway or green; the distance from the pin in feet, or 0 holed, when it finds the
// green), and a hole in one on the prize hole wins $100,000. A mulligan's shot does not count.
void fn_800DA48C(int nPlayer) {
    CourseMoneyTracking money;
    s32 nIndex;

    if (fn_800DA174()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && fn_800D0D54(nPlayer) &&
            gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = fn_800D0550(nPlayer);
        }
        fn_800DA6D0();
    }
    if (fn_800DA1D4()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && gPlayers[nPlayer].ball.nLie == 9 &&
            gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = 3.0f * fn_800D0478(nPlayer);
        }
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 &&
            gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && gPlayers[nPlayer].bC2F == 0) {
            gHoleContestPlayerResult[nPlayer] = 0.0f;
        }
        fn_800DA6D0();
    }
    if (fn_800DA234()) {
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

// Ranks the players on the contest hole and fills in the result table: the longest drives first
// (or the shots closest to the pin), then the players with no result (distance -1). Each is named by
// a CPU golfer's nickname ("NA": none, then the last name) or the player's profile name ("User n"
// while no profile is loaded). The first is the winner. Not on the hole-in-one prize hole.
// fake match: the (u32) casts on the player index; with a signed index the compiler walks one
// pointer instead of keeping the array start and the offset apart (see GoTerrain fn_80032518).
void fn_800DA6D0(void) {
    s32 aRank[5];               // per player: the place in the table, -1 not placed yet
    char szName[32];            // the stack frame gives 32 bytes; the real size is not known
    int nRank;
    int nBest;
    f32 fBest;
    int i;
    int j;
    s32 nIndex;

    if (fn_800DA234()) return;
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
    if (fn_800DA174()) {
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
    if (fn_800DA1D4()) {
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

char* fn_800DAD1C(int nPlayer) {
    return gHoleContestPlaceName[nPlayer];
}

s32 fn_800DAD30(int nPlayer) {
    return gHoleContestPlaceDistance[nPlayer];
}

u8 fn_800DAD44(void) {
    return gHoleContestWon;
}

s32 fn_800DAD4C(void) {
    return gHoleContestEverWon;
}

// Pays the contest's winner.
void fn_800DAD54(void) {
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

// The winner's ball: 1 holed, 2 within a foot of the pin, else 0.
s32 fn_800DADC0(void) {
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
