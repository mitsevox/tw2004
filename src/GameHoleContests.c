// GameHoleContests.c (our name): the hole contests of a multiplayer round in game mode 0 (stroke
// play), 1 or 2. At the start of the round up to three holes are drawn: a par 4 or 5 for the
// longest drive (lbl_80281568), a par 3 for closest to the pin (lbl_8028156C) and, one round in
// five, another par 3 with a $100,000 prize for a hole in one (lbl_80281570). Each player's result
// on the contest hole is kept by player (lbl_80202884 distances), ranked into a result table
// (lbl_80202828 names, lbl_80202870 distances), and the winner (lbl_80282264) is paid $2,500.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "frontend/fe.h"

s32  lbl_80281568 = -1;         // the longest-drive hole, -1 none
s32  lbl_8028156C = -1;         // the closest-to-the-pin hole, -1 none
s32  lbl_80281570 = -1;         // the hole-in-one prize hole, -1 none
// .bss/.sbss: defined in reverse address order (CodeWarrior lays them out last-defined-first).
f32  lbl_80202884[5];           // per player: the drive's length or the distance from the pin
s32  lbl_80202870[5];           // by place: the distance, -1 no result
char lbl_80202828[5][14];       // by place: the name shown with the result
s32  lbl_80282268;
s32  lbl_80282264;              // the contest's winner, 5 = nobody
u8   lbl_80282261;              // the contest on this hole is decided
u8   lbl_80282260;              // a contest has a winner (fn_800DA6D0), or the hole in one was made

u8   fn_800D304C(int nHole);    // a flag of the hole's course data (byte 0x35): the drive can count
u8   fn_800D0D54(int nPlayer);  // the ball lies on a fairway, the green or in the cup

u8   fn_800D9E5C(void);
void fn_800D9F34(void);
void fn_800DA6D0(void);

// A new round: no contest holes until they are drawn.
void fn_800D9E14(void) {
    lbl_80281568 = -1;
    lbl_8028156C = -1;
    lbl_80281570 = -1;
    lbl_80282260 = 0;
    lbl_80282261 = 0;
    if (fn_800D9E5C()) {
        fn_800D9F34();
    }
}

// Whether this round has hole contests: several players, a round of every hole, no mulligans,
// gSession.a8[0] clear, neither GameMode5 test (fn_800EC550, fn_800ED6F0) and game mode 0, 1 or 2.
u8 fn_800D9E5C(void) {
    if (gpGame->nMulligans != 0) return 0;
    if (gSession.nNumPlayers == 1) return 0;
    if (!fn_800E1BBC()) return 0;
    if (gSession.a8[0] != 0) return 0;
    if (fn_800EC550()) return 0;
    if (fn_800ED6F0()) return 0;
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
        if (fn_800D2AD8(i) > 3 && fn_800D304C(i)) {
            bFound = 1;
        }
    }
    if (bFound) {
        lbl_80281568 = Misc_RandFunc(0) % 18;
        while (fn_800D2AD8(lbl_80281568) == 3 || !fn_800D304C(lbl_80281568)) {
            lbl_80281568 = Misc_RandFunc(0) % 18;
        }
    } else {
        lbl_80281568 = -1;
    }

    bFound = 0;
    for (i = 0; i < 18; i++) {
        if (fn_800D2AD8(i) == 3) {
            bFound = 1;
        }
    }
    if (bFound) {
        lbl_8028156C = Misc_RandFunc(0) % 18;
        while (fn_800D2AD8(lbl_8028156C) > 3) {
            lbl_8028156C = Misc_RandFunc(0) % 18;
        }
    } else {
        lbl_8028156C = -1;
    }

    if ((int)(Misc_RandFunc(0) % 100) < 20) {
        bFound = 0;
        for (i = 0; i < 18; i++) {
            if (fn_800D2AD8(i) == 3 && i != lbl_8028156C) {
                bFound = 1;
            }
        }
        if (bFound) {
            lbl_80281570 = Misc_RandFunc(0) % 18;
            while (fn_800D2AD8(lbl_80281570) > 3 || lbl_80281570 == lbl_8028156C) {
                lbl_80281570 = Misc_RandFunc(0) % 18;
            }
        } else {
            lbl_80281570 = -1;
        }
    }
}

// The longest drive is played on this hole. On the round's last hole of a GameMode5 challenge the
// contest is always on.
u8 fn_800DA174(void) {
    if (fn_800ED6F0() && Game_CurHoleIndex() == 17 && gpGame->bD4 == 0) {
        return 1;
    }
    return lbl_80281568 == Game_CurHoleIndex();
}

// Closest to the pin is played on this hole (always on the 17th of a GameMode5 challenge).
u8 fn_800DA1D4(void) {
    if (fn_800ED6F0() && Game_CurHoleIndex() == 16 && gpGame->bD4 == 0) {
        return 1;
    }
    return lbl_8028156C == Game_CurHoleIndex();
}

// The hole-in-one prize is on this hole.
u8 fn_800DA234(void) {
    return Game_CurHoleIndex() == lbl_80281570;
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
    if (lbl_80282261) return 0;
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
// hole (fn_800E1734) also draws the contest holes again (fn_800D9E14).
void fn_800DA36C(void) {
    int i;
    int n;
    int j;

    n = gNumPlayersSetUp;
    i = 0;
    while (n-- > 0) {
        lbl_80202884[i++] = -1.0f;
    }
    for (j = 0; j < 5; j++) {
        strcpy(lbl_80202828[j], "");
        lbl_80202870[j] = 0;
    }
    lbl_80282261 = 0;
    lbl_80282264 = 5;
    if (fn_800E1734()) {
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
            lbl_80202884[nPlayer] = fn_800D0550(nPlayer);
        }
        fn_800DA6D0();
    }
    if (fn_800DA1D4()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && gPlayers[nPlayer].ball.nLie == 9 &&
            gPlayers[nPlayer].bC2F == 0) {
            lbl_80202884[nPlayer] = 3.0f * fn_800D0478(nPlayer);
        }
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 &&
            gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && gPlayers[nPlayer].bC2F == 0) {
            lbl_80202884[nPlayer] = 0.0f;
        }
        fn_800DA6D0();
    }
    if (fn_800DA234()) {
        if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1 && fn_800E2DB4(nPlayer) &&
            gPlayers[nPlayer].bC2F == 0) {
            lbl_80282260 = 1;
            lbl_80282268 = 1;
            fn_80005AE8(&money, 0, sizeof(money));
            money.n24 = 100000;
            money.n0 = 100000;
            money.n38 = 100000;
            fn_800D3548(nPlayer, 100000, &money);
            nIndex = gPlayers[nPlayer].nIndex;
            if (gpSaveData[nIndex].bActive) {
                fn_800E4364(0, 0x74, 100000, nIndex);
            }
        }
    }
}

// Ranks the players on the contest hole and fills in the result table: the longest drives first
// (or the shots closest to the pin), then the players with no result (distance -1). Each is named by
// a CPU golfer's nickname ("NA": none, then the last name) or the player's profile name ("User n"
// while no profile is loaded). The first is the winner. Not on the hole-in-one prize hole.
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
    lbl_80282264 = 5;
    nRank = 0;
    aRank[0] = -1;
    aRank[1] = -1;
    aRank[2] = -1;
    aRank[3] = -1;
    aRank[4] = -1;
    for (j = 0; j < 5; j++) {
        strcpy(lbl_80202828[j], "");
        lbl_80202870[j] = 0;
    }
    if (fn_800DA174()) {
        for (j = 0; j < gNumPlayersSetUp; j++) {
            fBest = 0.0f;
            nBest = 5;
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (aRank[(u32)i] == -1 && lbl_80202884[(u32)i] != -1.0f && lbl_80202884[(u32)i] > fBest) {
                    nBest = i;
                    fBest = lbl_80202884[(u32)i];
                }
            }
            if (nBest != 5) {
                if (Player_IsCPU(nBest)) {
                    if (strcmp(gPlayers[nBest].golfer.szNick, "NA") != 0) {
                        strcpy(lbl_80202828[nRank], gPlayers[nBest].golfer.szNick);
                    } else {
                        strcpy(lbl_80202828[nRank], gPlayers[nBest].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[nBest].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", nBest + 1);
                        strcpy(lbl_80202828[nRank], szName);
                    } else {
                        strcpy(lbl_80202828[nRank], gpSaveData[nIndex].szName);
                    }
                }
                aRank[nBest] = nRank;
                lbl_80202870[nRank] = fBest;
                if (nRank == 0) {
                    lbl_80282264 = nBest;
                }
                nRank++;
            }
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (lbl_80202884[(u32)i] == -1.0f) {
                if (Player_IsCPU(i)) {
                    if (strcmp(gPlayers[(u32)i].golfer.szNick, "NA") != 0) {
                        strcpy(lbl_80202828[nRank], gPlayers[(u32)i].golfer.szNick);
                    } else {
                        strcpy(lbl_80202828[nRank], gPlayers[(u32)i].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[(u32)i].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", i + 1);
                        strcpy(lbl_80202828[nRank], szName);
                    } else {
                        strcpy(lbl_80202828[nRank], gpSaveData[nIndex].szName);
                    }
                }
                lbl_80202870[nRank] = -1;
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
                if (aRank[(u32)i] == -1 && lbl_80202884[(u32)i] != -1.0f && lbl_80202884[(u32)i] < fBest) {
                    nBest = i;
                    fBest = lbl_80202884[(u32)i];
                }
            }
            if (nBest != 5) {
                if (Player_IsCPU(nBest)) {
                    if (strcmp(gPlayers[nBest].golfer.szNick, "NA") != 0) {
                        strcpy(lbl_80202828[nRank], gPlayers[nBest].golfer.szNick);
                    } else {
                        strcpy(lbl_80202828[nRank], gPlayers[nBest].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[nBest].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", nBest + 1);
                        strcpy(lbl_80202828[nRank], szName);
                    } else {
                        strcpy(lbl_80202828[nRank], gpSaveData[nIndex].szName);
                    }
                }
                aRank[nBest] = nRank;
                lbl_80202870[nRank] = fBest;
                if (nRank == 0) {
                    lbl_80282264 = nBest;
                }
                nRank++;
            }
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (lbl_80202884[(u32)i] == -1.0f) {
                if (Player_IsCPU(i)) {
                    if (strcmp(gPlayers[(u32)i].golfer.szNick, "NA") != 0) {
                        strcpy(lbl_80202828[nRank], gPlayers[(u32)i].golfer.szNick);
                    } else {
                        strcpy(lbl_80202828[nRank], gPlayers[(u32)i].golfer.szLast);
                    }
                } else {
                    nIndex = gPlayers[(u32)i].nIndex;
                    if (lbl_801D7148.aLoaded[nIndex] == 0) {
                        sprintf(szName, "User %d", i + 1);
                        strcpy(lbl_80202828[nRank], szName);
                    } else {
                        strcpy(lbl_80202828[nRank], gpSaveData[nIndex].szName);
                    }
                }
                lbl_80202870[nRank] = -1;
                nRank++;
            }
        }
    }
    if (lbl_80282264 != 5) {
        lbl_80282260 = 1;
        lbl_80282268 = 1;
    }
}

char* fn_800DAD1C(int nPlayer) {
    return lbl_80202828[nPlayer];
}

s32 fn_800DAD30(int nPlayer) {
    return lbl_80202870[nPlayer];
}

u8 fn_800DAD44(void) {
    return lbl_80282260;
}

s32 fn_800DAD4C(void) {
    return lbl_80282268;
}

// Pays the contest's winner.
void fn_800DAD54(void) {
    CourseMoneyTracking money;

    lbl_80282261 = 1;
    if (lbl_80282264 == 5) return;
    if (!lbl_80282260) return;
    fn_80005AE8(&money, 0, sizeof(money));
    money.n24 = 2500;
    money.n0 = 2500;
    money.n38 = 2500;
    fn_800D3548(lbl_80282264, 2500, &money);
}

// The winner's ball: 1 holed, 2 within a foot of the pin, else 0.
s32 fn_800DADC0(void) {
    if (lbl_80282260) {
        if (gPlayers[lbl_80282264].ball.nLie == LIE_INCUP_e && gPlayers[lbl_80282264].bPlanReady == 0) {
            return 1;
        }
        // fake match: the original loads lbl_80282264 again for the call; the volatile read does that
        if (3.0f * fn_800D0478(*(volatile s32*)&lbl_80282264) < 1.0f) {
            return 2;
        }
    }
    return 0;
}
