// GameModeBestBall.c (TW06's GameModeBestBall): game mode 19, two-against-two stroke play where each
// team counts its better ball on every hole. Team 0 is players 0 and 1, team 1 players 2 and 3.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

u8   fn_800E82AC(int nTeam);
int  fn_800E83A8(int nPlayer);
void fn_800E83F8(void);
s32  fn_800E84B0(int nPlayer);
int  fn_800E8848(int nPlayer);
u8   fn_800E8858(int nPlayer, u8 bCheck);
u8   fn_800E88A8(u8 bCheck);
u8   fn_800E8904(u8 bCheck);
void fn_800E890C(void);
void fn_800E8A68(void);

// TW06: GameModeBestBall::Init. Four players, stroke play, one mulligan each.
void fn_800E81C4(void) {
    gpGame->pfnInit = fn_800E81C4;
    gpGame->pfnSetupNextGolfer = fn_800E83F8;
    gpGame->pfnGetHonors = fn_800E84B0;
    gpGame->pfnHoleFinished = fn_800E8858;
    gpGame->pfnGameFinished = fn_800E88A8;
    gpGame->pfnGoToPlayoff = fn_800E8904;
    gpGame->pfnEndHole = fn_800E890C;
    gpGame->pfnEndGame = fn_800E8A68;
    gpGame->n4 = 0;
    gpGame->nMulligans = 2;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
}

// TW06: GameModeBestBall::TeamDone. A partner has holed out and the other can no longer beat that
// score.
u8 fn_800E82AC(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int bDone;
    int b;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    bDone = 0;
    if ((Player_IsHoled(a) && gPlayers[a].nStrokes[nHole] <= gPlayers[b].nStrokes[nHole] + 1) ||
        (Player_IsHoled(b) && gPlayers[b].nStrokes[nHole] <= gPlayers[a].nStrokes[nHole] + 1)) {
        bDone = 1;
    }
    return bDone;
}

// TW06: GameModeBestBall::GetPartner (found by the sweep).
int fn_800E83A8(int nPlayer) {
    switch (nPlayer) {
    case 0:
        return 1;
    case 1:
        return 0;
    case 2:
        return 3;
    case 3:
        return 2;
    default:
        return 5;
    }
}

// The hole starts: in split screen everyone plays at once; otherwise the first golfer gets ready and
// the others wait.
void fn_800E83F8(void) {
    int i;
    if (gSession.nSplitScreen == 1) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        }
        return;
    }
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        } else {
            GOLFERSTATE_Set(GS_WAIT, i);
        }
    }
}

// TW06: GameModeBestBall::GetHonors. Who plays next after nPlayer (5 = nobody): on the tee the team
// with the better score on the last decided hole, and within a team the better score; otherwise the
// player farthest from the pin (off the green first) whose team is still playing.
s32 fn_800E84B0(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};  // the tee order before anyone has a lower team score
    s32* pOrder;        // fake match: the within-team compares read aOrder through a pointer
    int a;
    int b;
    int w;
    int t;
    int i;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    int h;
    int nLead;
    f32 dx;
    f32 dz;
    f32 d;
    nLead = 0;
    pOrder = aOrder;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            a = gPlayers[1].nStrokes[h];
            if (gPlayers[0].nStrokes[h] <= a) {
                a = gPlayers[0].nStrokes[h];
            }
            b = gPlayers[3].nStrokes[h];
            if (gPlayers[2].nStrokes[h] <= b) {
                b = gPlayers[2].nStrokes[h];
            }
            if (a < b) {
                w = 0;
            } else if (b < a) {
                w = 1;
            } else {
                w = nLead;
            }
            if (w != nLead) {
                nLead = w;
                t = aOrder[0];
                aOrder[0] = aOrder[2];
                aOrder[2] = t;
                t = aOrder[1];
                aOrder[1] = aOrder[3];
                aOrder[3] = t;
            }
            t = aOrder[0];
            if (gPlayers[t].nStrokes[h] > gPlayers[pOrder[1]].nStrokes[h]) {
                aOrder[0] = aOrder[1];
                aOrder[1] = t;
            }
            t = aOrder[2];
            if (gPlayers[t].nStrokes[h] > gPlayers[pOrder[3]].nStrokes[h]) {
                aOrder[2] = aOrder[3];
                aOrder[3] = t;
            }
        }
    }
    // fake match: the tee-order loop reuses the hole counter h; a counter of its own gets another
    // register (h, t or w all match)
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) && !gPlayers[aOrder[h]].bPlayerCut &&
            !fn_800E82AC(fn_800E8848(aOrder[h]))) {
            return aOrder[h];
        }
    }
    pCourse = fn_8000C594();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut && !fn_800E82AC(fn_800E8848(i)) &&
            PLAYER(i)->ball.nLie != LIE_GREEN_e) {
            dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
            dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
            d = fn_80009680(dx * dx + dz * dz);
            if (d > fBest) {
                fBest = d;
                nBest = i;
            }
        }
    }
    if (nBest == 5) {
        fBest = 0.0f;
        nBest = 5;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut &&
                !fn_800E82AC(fn_800E8848(i))) {
                dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
                dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
                d = fn_80009680(dx * dx + dz * dz);
                if (d > fBest) {
                    fBest = d;
                    nBest = i;
                }
            }
        }
    }
    if (nBest == nPlayer) {
        return 5;
    }
    return nBest;
}

// A player's team (TW06 has this as GameModeFourBall::GetPlayerTeam; its best ball mode has none).
int fn_800E8848(int nPlayer) {
    return nPlayer / 2;
}

// TW06: GameModeBestBall::HoleFinished. Both teams are done.
u8 fn_800E8858(int nPlayer, u8 bCheck) {
    int bDone = 0;
    if (fn_800E82AC(0) && fn_800E82AC(1)) {
        bDone = 1;
    }
    return bDone;
}

// TW06: GameModeBestBall::GameFinished. No selected hole is left.
u8 fn_800E88A8(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// GoToPlayoff: never (TW06's GameModeBestBall does not override GameModeBase::GoToPlayoff).
u8 fn_800E8904(u8 bCheck) {
    return 0;
}

// TW06: GameModeBestBall::EndHole. On each team the ball that does not count (or was not holed)
// is marked 9.
void fn_800E890C(void) {
    int nHole = Game_CurHoleIndex();
    if (Player_IsHoled(0)) {
        if (Player_IsHoled(1)) {
            if (gPlayers[0].nStrokes[nHole] < gPlayers[1].nStrokes[nHole]) {
                gPlayers[1].nStrokes[nHole] = 9;
            } else {
                gPlayers[0].nStrokes[nHole] = 9;
            }
        } else {
            gPlayers[1].nStrokes[nHole] = 9;
        }
    } else {
        gPlayers[0].nStrokes[nHole] = 9;
    }
    if (Player_IsHoled(2)) {
        if (Player_IsHoled(3)) {
            if (gPlayers[2].nStrokes[nHole] < gPlayers[3].nStrokes[nHole]) {
                gPlayers[3].nStrokes[nHole] = 9;
            } else {
                gPlayers[2].nStrokes[nHole] = 9;
            }
        } else {
            gPlayers[3].nStrokes[nHole] = 9;
        }
    } else {
        gPlayers[2].nStrokes[nHole] = 9;
    }
}

// TW06: GameModeBestBall::EndGame. A human team that beats an all-CPU team wins money: half the two
// CPU golfers' base prizes plus their per-stroke prizes for up to 5 strokes of margin. EA reuses the
// team loop's counter for the inner loop, so the loop ends after the first team that wins.
void fn_800E8A68(void) {
    int i;
    int nFirst;
    int nSum;
    int nOtherTeam;
    int nTheirs;
    int nMargin;
    int nRating1;
    int nRating2;
    int nOurs;
    int nOther2;
    int nBase;
    int nMoney;
    int nOther;
    int nProfile;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        for (i = 0; i < 2; i++) {
            if (Team_IsAllHuman(i)) {
                if (i == 0) {
                    nFirst = 0;
                    nOther = 2;
                    nOtherTeam = 1;
                } else {
                    nFirst = 2;
                    nOther = 0;
                    nOtherTeam = 0;
                }
                if (Team_IsAllCPU(nOtherTeam)) {
                    nOurs = fn_800E1788(nFirst);
                    nOurs += fn_800E1788(nFirst + 1);
                    nTheirs = fn_800E1788(nOther);
                    nOther2 = nOther + 1;
                    nTheirs += fn_800E1788(nOther2);
                    if (nOurs < nTheirs) {
                        nMargin = nTheirs - nOurs;
                        if (nMargin > 5) {
                            nMargin = 5;
                        }
                        nRating1 = fn_800D3C7C(nOther);
                        nRating2 = fn_800D3C7C(nOther2);
                        nSum = lbl_80200538.aStrokePrize[nRating1].nBase +
                               lbl_80200538.aStrokePrize[nRating2].nBase;
                        nMoney = nSum + lbl_80200538.aStrokePrize[nRating1].nPerStroke * nMargin;
                        nMoney += lbl_80200538.aStrokePrize[nRating2].nPerStroke * nMargin;
                        nBase = nSum / 2;
                        nMoney /= 2;
                        for (i = 0; i < 2; i++) {
                            nProfile = gPlayers[nFirst + i].nIndex;
                            if (gpSaveData[nProfile].bActive) {
                                fn_80125910(1);
                                if (nBase) {
                                    fn_800E4364(0, 0x76, nBase, nProfile);
                                }
                                fn_800D3548(nFirst + i, nMoney, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

// TW06: GM_BestBallMode_GetTeamHoleScore. The team's (better) score on a hole.
int fn_800E8C24(int nPlayer, int nHole) {
    if (gPlayers[nPlayer].nStrokes[nHole] <= gPlayers[fn_800E83A8(nPlayer)].nStrokes[nHole]) {
        return gPlayers[nPlayer].nStrokes[nHole];
    }
    return gPlayers[fn_800E83A8(nPlayer)].nStrokes[nHole];
}

// TW06: GM_BestBallMode_GetTeamRelativeScore. The team's score against par so far (and on the
// current hole once holed, when asked).
int fn_800E8CA8(int nPlayer, u8 bCurrent) {
    int nPar;
    int nScore;
    int h;
    int n;
    nScore = 0;
    nPar = 0;
    n = gpGame->nCurHole;
    if (bCurrent && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && n < 18) {
        n++;
    }
    for (h = 0; h < n; h++) {
        if (gpGame->bHoleSelected[h]) {
            nPar += fn_800D2AD8(h);
            nScore += fn_800E8C24(nPlayer, h);
        }
    }
    return nScore - nPar;
}
