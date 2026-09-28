// GameRound.c (our name; EA's file is GameModeCore.c: the functions it shares with TW07's
// GameModeCore.c, GM_SetModeType to GM_RenderBallTarget, come in the same order): the round's
// rules and state, after GameManager.c - a game mode's setup and default callbacks, the round's
// holes and courses (Random 18, Dream 18, the regional rounds), scores against par, mulligans, the
// stroke limit, gimmes, honors, playoff holes, par-5 eagle records, course and hole folder names.

#include "golfer.h"
#include "ball.h"
#include "physics.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/pgatoursim.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState and the (u8) on GOLFERSTATE_Set's player (see game.h).

int  GM_GetPlayerRoundScoreThroughHole(int nPlayer, int nHoles);
void GM_SetSplitScreen(u8 b);

void  GM_ClearGameBonusStats(int nPlayer);   // clears the player's words at 0x314-0x350
void  GM_BuildRandom18(void);
void  GM_BuildDream18(void);
void  GM_BuildRegionalRound(int nCourse);

// GameRound.c's data. The uninitialised globals are defined last address first (CodeWarrior lays
// each section out in reverse order of definition).
GameState  lbl_802028F0;                    // the game state (reached through gpGame)
GameState* gpGame = &lbl_802028F0;          // golfer.h
u8   lbl_8028227C;                          // game.h
s32  lbl_80282278;                          // game.h
char lbl_80282270[8];                       // the hole name

void  GM_DefaultNoOp(void);
s32   GM_DefaultGetHonors(int a);
u8    GM_DefaultHoleFinished(int nPlayer, u8 bCheck);
u8    GM_DefaultGameFinished(u8 bCheck);
u8    GM_DefaultGoToPlayoff(u8 bCheck);
void  GM_DefaultNoOpPlayer(int nPlayer);
u8    GM_DefaultFalsePlayer(int nPlayer);
u8    GM_DefaultTrue(void);
u8    GM_DefaultTruePlayer(int nPlayer);
s32   GM_DefaultZeroPlayer(int nPlayer);
void  GM_DefaultSetTimeLeft(int nPlayer, int nTime);
void  GM_DefaultBonusCollected(int nPlayer, int nId);
s32   GM_DefaultTargetState(int a, int nTarget);
void  GameModeDriverPGATour_Init(void);
void  fn_8010C4A0(void);
void  fn_80125E68(void);

u8    GM_GetNeedToBuildPlayoffHoleList(void);

// Four floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void GM_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void GM_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// The 20 course ids the random mixed round picks from (lbl_80184D40).
typedef struct CourseList {
    u32 a[20];
} CourseList;
const CourseList lbl_80184D40 = {{0, 1, 2, 3, 0, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};

// A course counts as unlocked when any of the five profiles (or the second block) has its flag.
#define COURSE_UNLOCKED(c, k) (gpSaveData[k].aCourseUnlocked[c] || lbl_80281DF4->aCourseUnlocked[c])

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void GM_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void GM_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Sets up a game mode: the rule flags and callbacks to their defaults (the callbacks are mostly
// empty stubs), then the mode's own setup (26 modes; 3 has none), then the round as holes 1..18.
void GM_SetModeType(int nMode) {
    int i;
    gpGame->nMode = nMode;
    gpGame->bD4 = 0;
    gpGame->nD8 = 0;
    gpGame->b136 = 0;
    gpGame->b137 = 0;
    gpGame->b138 = 0;
    gpGame->b139 = 0;
    gpGame->nSaveSlot = 5;
    gpGame->nSaveCourse = 0;
    gpGame->bShowYardage = 1;
    gpGame->b271 = 1;
    gpGame->bStrokeLimit = 1;
    gpGame->b273 = 1;
    gpGame->b274 = 1;
    gpGame->b275 = 1;
    gpGame->b276 = 1;
    gpGame->b277 = 1;
    gpGame->bGimmesAllowed = 1;
    gpGame->b279 = 1;
    gpGame->bAIConcedes = 0;
    gpGame->b27B = 1;
    gpGame->b27C = 1;
    gpGame->b27D = 1;
    gpGame->b27E = 1;
    gpGame->b27F = 1;
    gpGame->b280 = 1;
    gpGame->b281 = 1;
    gpGame->b282 = 1;
    gpGame->b283 = 1;
    gpGame->b284 = 1;
    gpGame->b285 = 1;
    gpGame->b286 = 1;
    gpGame->b287 = 1;
    gpGame->b288 = 1;
    gpGame->b289 = 1;
    gpGame->b28A = 1;
    gpGame->bNoWind = 0;
    gpGame->bBumpObstructions = 1;
    gpGame->b28D = 0;
    gpGame->n290 = 2;
    gpGame->n294 = 1;
    gpGame->b28E = 0;
    gpGame->pfnInit = GM_DefaultNoOp;
    gpGame->pfnShutdown = GM_DefaultNoOp;
    gpGame->pfnSetupNextGolfer = GM_DefaultNoOp;
    gpGame->pfnGetHonors = GM_DefaultGetHonors;
    gpGame->pfnHoleFinished = GM_DefaultHoleFinished;
    gpGame->pfnGameFinished = GM_DefaultGameFinished;
    gpGame->pfnGoToPlayoff = GM_DefaultGoToPlayoff;
    gpGame->pfn1E4 = GM_DefaultNoOp;
    gpGame->pfnEndHole = GM_DefaultNoOp;
    gpGame->pfn1EC = GM_DefaultNoOp;
    gpGame->pfn1F0 = GM_DefaultNoOp;
    gpGame->pfnEndGame = GM_DefaultNoOp;
    gpGame->pfn1F8 = fn_800CF158;
    gpGame->pfn1FC = fn_800CF450;
    gpGame->pfn200 = fn_800CFE74;
    gpGame->pfn204 = fn_800D0098;
    gpGame->pfn208 = fn_800D030C;
    gpGame->pfn210 = GM_DefaultNoOpPlayer;
    gpGame->pfn214 = GM_DefaultNoOp;
    gpGame->pfn218 = GM_DefaultNoOpPlayer;
    gpGame->pfn21C = GM_DefaultNoOpPlayer;
    gpGame->pfn220 = GM_DefaultNoOp;
    gpGame->pfn224 = GM_DefaultNoOp;
    gpGame->pfn228 = GM_DefaultNoOpPlayer;
    gpGame->pfn22C = GM_DefaultNoOpPlayer;
    gpGame->pfn230 = GM_DefaultFalsePlayer;
    gpGame->pfn234 = GM_DefaultTrue;
    gpGame->pfn238 = GM_DefaultTruePlayer;
    gpGame->pfn23C = GM_DefaultNoOpPlayer;
    gpGame->pfn240 = GM_DefaultZeroPlayer;
    gpGame->pfn244 = GM_DefaultNoOpPlayer;
    gpGame->pfnEndGolferTurn = GM_DefaultNoOpPlayer;
    gpGame->pfn24C = GM_DefaultNoOpPlayer;
    gpGame->pfn250 = GM_DefaultNoOpPlayer;
    gpGame->pfn254 = GM_DefaultNoOpPlayer;
    gpGame->pfn258 = GM_DefaultFalsePlayer;
    gpGame->pfn25C = GM_DefaultSetTimeLeft;
    gpGame->pfn260 = GM_DefaultNoOpPlayer;
    gpGame->pfn264 = GM_DefaultFalsePlayer;
    gpGame->pfn268 = GM_DefaultBonusCollected;
    gpGame->pfn26C = GM_DefaultTargetState;
    gpGame->pfn20C = GM_DefaultNoOpPlayer;
    lbl_80282278 = 0;
    switch (Game_GetMode()) {
    case 0:
        GameModeStroke_Init();
        break;
    case 1:
        GameModeMatch_Init();
        break;
    case 2:
        GameModeSkins_Init();
        break;
    case 4:
        GameMode4_Init();
        break;
    case 6:
        fn_800F944C();
        break;
    case 7:
        fn_800F9610();
        break;
    case 8:
        fn_800F986C();
        break;
    case 9:
        fn_800ED738();
        break;
    case 5:
        PlayNow_Init();
        break;
    case 10:
        GameModeReplay_Init();
        break;
    case 11:
        Lessons_Init();
        break;
    case 12:
        GameMode12_Init();
        break;
    case 13:
        GameModeSkillZoneTimed_Init();
        break;
    case 14:
        fn_800F2984();
        break;
    case 15:
        fn_800F39F4();
        break;
    case 16:
        fn_800F4B40();
        break;
    case 17:
        fn_800F5AAC();
        break;
    case 18:
        GameModeStableford_Init();
        break;
    case 19:
        fn_800E81C4();
        break;
    case 20:
        GameModeFourBall_Init();
        break;
    case 21:
        GameModeAlternateShot_Init();
        break;
    case 23:
        GameModeDriverPGATour_Init();
        break;
    case 24:
        GameModeDriverRTE_Init();
        break;
    case 25:
        GameModeBattle_Init();
        break;
    case 26:
        fn_8010C4A0();
        break;
    case 22:
        fn_80125E68();
        break;
    }
    for (i = 0; i < 18; i++) {
        gpGame->nHoleNum[i] = i;
    }
}

// Clears one player's record of one hole: strokes, putts, points and the rest.
void GM_ClearPlayerHoleData(int nPlayer, int nHole) {
    gPlayers[nPlayer].nStrokes[nHole] = 0;
    gPlayers[nPlayer].nPutts[nHole] = 0;
    gPlayers[nPlayer].nModePoints[nHole] = 0;
    gPlayers[nPlayer].n22C[nHole] = 0;
    gPlayers[nPlayer].n290[nHole] = 0;
    gPlayers[nPlayer].b2F6[nHole] = 0;
    gPlayers[nPlayer].b2E4[nHole] = 0;
    gpGame->b16C[nPlayer][nHole] = 0;
    gPlayers[nPlayer].nD28[nHole] = 0;
    gPlayers[nPlayer].nD70[nHole] = 0;
}

// A new round: every player's holes and round totals cleared, the pin for every hole set from
// the session's pin option (-1 = the first pin), and every player's mulligan given back.
void GM_ClearDataForNewGame(void) {
    int     j;
    int     i;
    Player* p;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 18; j++) {
            GM_ClearPlayerHoleData(i, j);
        }
        p = PLAYER(i);
        for (j = 0; j < 4; j++) {
            p->nRoundScore[j] = 0;
        }
        p->bPlayerCut = 0;
        p->nHolesWon = 0;
        p->n274 = 0;
        p->n2D8 = 0;
        p->n2DC = 0;
        p->n2E0 = 0;
        p->n308 = 0;
        p->nC44 = 3000;
        p->nC3C = 0;
        p->uC48 = 0;
        for (j = 0; j < 18; j++) {
            p->nC6C[j] = 0;
        }
        GM_ClearGameBonusStats(i);
        if (gpSaveData[p->nIndex].bActive != 0) {
            gpSaveData[p->nIndex].b70 = 0;
        }
    }
    for (i = 0; i < 18; i++) {
        if (gSession.nPinSet == -1) {
            gpGame->nPinSet[i] = 0;
        } else {
            gpGame->nPinSet[i] = gSession.nPinSet;
        }
    }
    GM_ClearMulliganCounters();
    gpGame->nE0 = 1;
    gpGame->bD5 = 0;
}

// Selects the round's holes by preset nPreset: 0 none, 1 all 18, 2 the front nine, 3 the back nine,
// 4/5/6 only the par 5s/4s/3s, 7 all 18; then makes the first selected hole the current one.
void GM_SelectHoleSet(int nPreset) {
    int i;
    for (i = 0; i < 18; i++) {
        switch (nPreset) {
        case 0:
            gpGame->bHoleSelected[i] = 0;
            break;
        case 1:
            gpGame->bHoleSelected[i] = 1;
            break;
        case 2:
            if (i < 9) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 3:
            if (i >= 9) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 4:
            if (Course_GetHolePar(i) == 5) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 5:
            if (Course_GetHolePar(i) == 4) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 6:
            if (Course_GetHolePar(i) == 3) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 7:
            gpGame->bHoleSelected[i] = 1;
            break;
        }
    }
    GM_InitializeCurrentHoleToFirstSelected();
}

// Adds a hole to the round and moves to the round's first hole.
void GM_SelectSingleHole(int nHole) {
    gpGame->bHoleSelected[nHole] = 1;
    GM_InitializeCurrentHoleToFirstSelected();
}

// Makes the round's first selected hole the current one (GM_SetCurrentHole); nothing changes when
// no hole is selected.
void GM_InitializeCurrentHoleToFirstSelected(void) {
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            GM_SetCurrentHole(i);
            return;
        }
    }
}

// Makes a hole of the round the current one: its number, and its course when the round mixes
// courses.
void GM_SetCurrentHole(int nHole) {
    gpGame->nCurHole = nHole;
    gpGame->nCurHoleNum = gpGame->nHoleNum[nHole];
    if (gpGame->b136 || gpGame->b137 || gpGame->b139 || gpGame->b138) {
        gpGame->nCurCourse = gpGame->nHoleCourse[nHole];
    }
}

// Sets the round's course. 23 builds "Random 18" (GM_BuildRandom18, flag b137), 22 "Dream 18"
// (GM_BuildDream18, b138) and 24..29 a regional round (GM_BuildRegionalRound, b139 = 1..6); 22 and
// 24..29 then take the current hole's course from the built round. Any other value is one course
// played as its holes 1..18; while a custom round is set up (b136) only the current course changes.
void GM_SetCurrentCourse(int nCourse) {
    int i;
    if (nCourse == 23) {
        GM_BuildRandom18();
        gpGame->b137 = 1;
        return;
    }
    gpGame->b137 = 0;
    if (nCourse == 22) {
        gpGame->b138 = 1;
        GM_BuildDream18();
        gpGame->nCurCourse = gpGame->nHoleCourse[gpGame->nCurHole];
        return;
    }
    gpGame->b138 = 0;
    if (nCourse >= 24 && nCourse < 30) {
        gpGame->b139 = nCourse - 23;
        GM_BuildRegionalRound(nCourse);
        gpGame->nCurCourse = gpGame->nHoleCourse[gpGame->nCurHole];
    } else {
        gpGame->b139 = 0;
        gpGame->nCurCourse = nCourse;
    }
    if (!gpGame->b136 && !gpGame->b137 && !gpGame->b138 && !gpGame->b139) {
        for (i = 0; i < 18; i++) {
            gpGame->nHoleCourse[i] = nCourse;
        }
        for (i = 0; i < 18; i++) {
            gpGame->nHoleNum[i] = i;
        }
    }
}

// The round's next hole after the current one, or -1.
int GM_GetNextSelectedHole(void) {
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return i;
        }
    }
    return -1;
}

// Whether the current hole is the round's first.
u8 GM_OnFirstSelectedHole(void) {
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return i == gpGame->nCurHole;
        }
    }
    return 0;
}

// The mode's mulligan rule: 0 none, 1 any number, 2 one per player per nine.
int Game_GetMulliganRule(void) {
    return gpGame->nMulligans;
}

// A player's total over all 18 holes in the mode's scoring: see GM_GetPlayerRoundScoreThroughHole.
int GM_GetPlayerRoundScore(int nPlayer) {
    return GM_GetPlayerRoundScoreThroughHole(nPlayer, 18);
}

// A player's total strokes for the round.
int GM_GetPlayerRoundStrokes(int nPlayer) {
    int i;
    int n = 0;
    for (i = 0; i < 18; i++) {
        n += gPlayers[nPlayer].nStrokes[i];
    }
    return n;
}

// Strokes against par over the holes played so far (and the current one, when asked and the
// ball is in the hole).
int GM_GetGolferRelativeCurrentScore(int nPlayer, u8 bCurrent) {
    int nPar;
    int nStrokes = 0;
    int i;
    int nEnd = gpGame->nCurHole;
    nPar = 0;
    if (bCurrent && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && nEnd < 18) {
        nEnd++;
    }
    for (i = 0; i < nEnd; i++) {
        if (gpGame->bHoleSelected[i]) {
            nPar += Course_GetHolePar(i);
            nStrokes += gPlayers[nPlayer].nStrokes[i];
        }
    }
    return nStrokes - nPar;
}

// The score against par shown for a player: the PGA TOUR simulation's while a tour event runs
// (GM_Currently_PgaTourMode), n2D8 in a playoff (gpGame->bD4), else
// GM_GetGolferRelativeCurrentScore while the event's round number (gpGame->nDC) is below its round
// count (nE0), and 0 after the last round.
int GM_GetGolferRelativeCumulativeScore(int nPlayer, u8 bCurrent) {
    if (GM_Currently_PgaTourMode()) {
        return GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, 0, bCurrent);
    }
    if (gpGame->bD4) {
        return gPlayers[nPlayer].n2D8;
    }
    if (gpGame->nDC < gpGame->nE0) {
        return GM_GetGolferRelativeCurrentScore(nPlayer, bCurrent);
    }
    return 0;
}

// A player's total for the first nHoles holes: the mode's points in mode 18 (Stableford), the
// team's better score per hole (fn_800E8C24) in mode 19 (best ball), strokes otherwise.
int GM_GetPlayerRoundScoreThroughHole(int nPlayer, int nHoles) {
    int n;
    int i;
    if (Game_GetMode() == 19) {
        n = 0;
        for (i = 0; i < nHoles; i++) {
            n += fn_800E8C24(nPlayer, i);
        }
    } else if (Game_GetMode() == 18) {
        n = 0;
        for (i = 0; i < nHoles; i++) {
            n += gPlayers[nPlayer].nModePoints[i];
        }
    } else {
        n = 0;
        for (i = 0; i < nHoles; i++) {
            n += gPlayers[nPlayer].nStrokes[i];
        }
    }
    return n;
}

// Whether the round plays every hole (in a playoff, gpGame->bD4, the answer bD5 kept from
// before the playoff narrowed the selection).
u8 GM_FullRoundOfGolf(void) {
    int i;
    if (gpGame->bD4) {
        return gpGame->bD5;
    }
    for (i = 0; i < 18; i++) {
        if (!gpGame->bHoleSelected[i]) {
            return 0;
        }
    }
    return 1;
}

// Whether the current hole is the round's last.
u8 GM_CurrentlyOnLastHole(void) {
    u8 b = 1;
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            b = 0;
        }
    }
    return b;
}

// A course's par 5 to its index in the profile's par-5 eagle records (0..74: two to five per
// course, none on course 7), or -1 for a hole that is not one; nHole counts from 0.
// GM_UserHasEagledHole and GM_GetPar5EagleDate read the records (fn_800588F4) and
// GM_GetGameProgress counts them. EA wrote the cases as 1-based hole numbers; the courses come in
// the original's order, which numbers the records.
int GM_ConvertCourseAndHoleToPar5EagleIndex(int nCourse, int nHole) {
    switch (nCourse) {
    case 0:
        switch (nHole + 1) {
        case 2: return 0;
        case 6: return 1;
        case 14: return 2;
        case 18: return 3;
        default: return -1;
        }
    case 1:
        switch (nHole + 1) {
        case 2: return 4;
        case 4: return 5;
        case 10: return 6;
        case 15: return 7;
        default: return -1;
        }
    case 2:
        switch (nHole + 1) {
        case 2: return 8;
        case 9: return 9;
        case 11: return 10;
        case 16: return 11;
        default: return -1;
        }
    case 3:
        switch (nHole + 1) {
        case 2: return 12;
        case 9: return 13;
        case 13: return 14;
        case 18: return 15;
        default: return -1;
        }
    case 6:
        switch (nHole + 1) {
        case 6: return 16;
        case 15: return 17;
        case 17: return 18;
        default: return -1;
        }
    case 12:
        switch (nHole + 1) {
        case 2: return 19;
        case 6: return 20;
        case 11: return 21;
        case 18: return 22;
        default: return -1;
        }
    case 9:
        switch (nHole + 1) {
        case 4: return 23;
        case 6: return 24;
        case 12: return 25;
        case 16: return 26;
        default: return -1;
        }
    case 10:
        switch (nHole + 1) {
        case 1: return 27;
        case 7: return 28;
        case 11: return 29;
        case 14: return 30;
        default: return -1;
        }
    case 11:
        switch (nHole + 1) {
        case 4: return 31;
        case 9: return 32;
        case 10: return 33;
        case 12: return 34;
        case 18: return 35;
        default: return -1;
        }
    case 13:
        switch (nHole + 1) {
        case 3: return 36;
        case 13: return 37;
        case 15: return 38;
        default: return -1;
        }
    case 15:
        switch (nHole + 1) {
        case 5: return 39;
        case 14: return 40;
        default: return -1;
        }
    case 14:
        switch (nHole + 1) {
        case 6: return 41;
        case 9: return 42;
        case 13: return 43;
        case 18: return 44;
        default: return -1;
        }
    case 16:
        switch (nHole + 1) {
        case 2: return 45;
        case 6: return 46;
        case 11: return 47;
        default: return -1;
        }
    case 17:
        switch (nHole + 1) {
        case 1: return 48;
        case 6: return 49;
        case 7: return 50;
        case 11: return 51;
        case 15: return 52;
        default: return -1;
        }
    case 18:
        switch (nHole + 1) {
        case 5: return 53;
        case 8: return 54;
        case 13: return 55;
        case 18: return 56;
        default: return -1;
        }
    case 19:
        switch (nHole + 1) {
        case 5: return 57;
        case 9: return 58;
        case 15: return 59;
        case 18: return 60;
        default: return -1;
        }
    case 20:
        switch (nHole + 1) {
        case 4: return 61;
        case 10: return 62;
        case 16: return 63;
        default: return -1;
        }
    case 5:
        switch (nHole + 1) {
        case 4: return 64;
        case 7: return 65;
        case 13: return 66;
        default: return -1;
        }
    case 8:
        switch (nHole + 1) {
        case 4: return 67;
        case 6: return 68;
        case 12: return 69;
        case 16: return 70;
        default: return -1;
        }
    case 4:
        switch (nHole + 1) {
        case 1: return 71;
        case 7: return 72;
        case 10: return 73;
        case 18: return 74;
        default: return -1;
        }
    }
    return -1;
}

// Whether save profile nSlot has eagled par 5 b (counted from 0) of course a: the record's flag
// (fn_800588F4 kind 0); 0 when the hole is not a par 5.
u8 GM_UserHasEagledHole(int nSlot, int a, int b) {
    int i = GM_ConvertCourseAndHoleToPar5EagleIndex(a, b);
    if (i != -1) {
        return fn_800588F4(&gpSaveData[nSlot], 0, i);
    }
    return 0;
}

// The date save profile nSlot eagled par 5 b of course a (fn_800588F4 kind 1); 0 when the hole is
// not a par 5.
int GM_GetPar5EagleDate(int nSlot, int a, int b) {
    int i = GM_ConvertCourseAndHoleToPar5EagleIndex(a, b);
    if (i != -1) {
        return fn_800588F4(&gpSaveData[nSlot], 1, i);
    }
    return 0;
}

// Whether nStrokes has reached the hole's stroke limit of 10: only with the stroke-limit option on
// (gSession.bStrokeLimit) and a mode that uses it (gpGame->bStrokeLimit); GM_PlayerTookShot then
// picks the ball up. nPlayer is not read.
u8 GM_IsShotOverLimit(int nPlayer, int nStrokes) {
    if (gSession.bStrokeLimit && gpGame->bStrokeLimit && nStrokes >= 10) {
        return 1;
    }
    return 0;
}

// Whether a player may take a mulligan: humans only, the mode allows them, and in the
// one-per-nine rule not already used.
u8 GM_CanPlayerTakeMulligan(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (Game_GetMulliganRule() == 0) {
        return 0;
    }
    if (Game_GetMulliganRule() == 2 && gPlayers[nPlayer].bMulliganUsed) {
        return 0;
    }
    return 1;
}

// Gives every player their mulligan back.
void GM_ClearMulliganCounters(void) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->bMulliganUsed = 0;
    }
}

// The fewest players game mode nMode takes, for the front end (FE_MessageTable messages 2, 4 and
// 6): 1 for stroke play (0), 4, 5, 9, Stableford (18), 23 and 24; 2 for match play (1), skins (2)
// and 26; 4 for the team modes 19-21 (best ball, four-ball, alternate shot); speed golf (6-8) 1 or
// 2 by gpGame->n4; 0 for any other mode.
int GM_GetMinPlayersForMode(int nMode) {
    switch (nMode) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 2;
    case 4:
        return 1;
    case 5:
        return 1;
    case 6:
    case 7:
    case 8:
        return gpGame->n4 == 0 ? 1 : 2;
    case 9:
        return 1;
    case 18:
        return 1;
    case 19:
        return 4;
    case 20:
        return 4;
    case 21:
        return 4;
    case 23:
        return 1;
    case 24:
        return 1;
    case 26:
        return 2;
    }
    return 0;
}

// Turns split screen on or off (gSession.nSplitScreen), keeping the choice in lbl_8028227C, from
// which GameMode22.c restores it.
void GM_SetSplitScreen(u8 b) {
    lbl_8028227C = b;
    gSession.nSplitScreen = b;
}

// Split screen for the mode: modes 0-2 as the session has it, 6, 7 and 26 always, others never.
void GM_SetSplitScreenForMode(void) {
    switch (Game_GetMode()) {
    case 6:
    case 7:
        GM_SetSplitScreen(1);
        return;
    case 26:
        GM_SetSplitScreen(1);
        return;
    case 0:
    case 1:
    case 2:
        if (gSession.nSplitScreen == 1) {
            GM_SetSplitScreen(1);
            return;
        }
        GM_SetSplitScreen(0);
        return;
    default:
        GM_SetSplitScreen(0);
        return;
    }
}

// The current course's folder name on the disc ("01_Peb" = Pebble Beach ...; course 4's is
// "22_Ant"), "none" past course 20. StreamManagerHole and the disc check build their data paths
// from it.
char* GM_GetCourseName(void) {
    switch (Game_GetCourse()) {
    case 0:  return "01_Peb";
    case 1:  return "02_Pri";
    case 2:  return "03_Saw";
    case 3:  return "04_Vol";
    case 4:  return "22_Ant";
    case 5:  return "06_Bet";
    case 6:  return "07_Bir";
    case 7:  return "08_Dri";
    case 8:  return "09_Bay";
    case 9:  return "10_For";
    case 10: return "11_Spy";
    case 11: return "12_Pop";
    case 12: return "13_Hig";
    case 13: return "14_Sco";
    case 14: return "15_Tor";
    case 15: return "16_Sai";
    case 16: return "17_Sah";
    case 17: return "18_Jpn";
    case 18: return "19_Aus";
    case 19: return "20_Kap";
    case 20: return "21_Pin";
    }
    return "none";
}

// Hole nHole's folder name (nHole counted from 0): "HOLE_01" .. "HOLE_18", in one static buffer the
// next call overwrites.
char* GameManager_GetHoleName(int nHole) {
    sprintf(lbl_80282270, "HOLE_%02d", nHole + 1);
    return lbl_80282270;
}

// Whether the mode allows the golfer's post-shot reactions (GM_ShowPostShotAnimation,
// GM_ChooseRemoveBallState): gpGame->n294, on by default and cleared by modes 8, 13-15, 17 and 22.
u8 GM_IsValidPostShotGameType(void) {
    return gpGame->n294 != 0;
}

// Seconds since the session's frame count was stored in gpGame->n12C (by GM_Update in game type
// 6, and by GM_RestartHole).
int GM_GetElapsedHoleTime(void) {
    return (1.0f / FRAME_RATE) * (f32)(u32)(gSession.nFrameCount - gpGame->n12C);
}

// Whether the player may be given a gimme (TW07: GM_CanPlayerTapIn, at the same place in
// GameModeCore.c): the Gimmes option is on, not split screen, not a saved replay, not both session
// flags 0x4000 and 0x8000, the mode allows gimmes (gpGame->bGimmesAllowed), the mode's
// pfnHoleFinished(nPlayer, 1) does not already end the hole, and the ball is within half a yard (18
// inches) of the pin; with more than one player the putter must also be in hand. Swing state 14
// asks it; yes leads to state 15 (the tap-in is planned) and 16 (played for the player).
u8 Gimme_Allowed(int nPlayer) {
    if (!gSession.options.bGimmes) return 0;
    if (gSession.nSplitScreen) return 0;
    if (gSession.bReplay) return 0;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) return 0;
    if (!gpGame->bGimmesAllowed) return 0;
    if (gpGame->pfnHoleFinished(nPlayer, 1)) return 0;
    if (fn_800D0478(nPlayer) > 0.5f) return 0;
    if (gPlayers[nPlayer].nClub != CLUB_PUTTER_e && gSession.nNumPlayers > 1) return 0;
    return 1;
}

// The first player to play: the one the mode says plays after nobody (5).
s32 GM_GetHonors(void) {
    return gpGame->pfnGetHonors(5);
}

// The player after the first (the mode's choice run twice).
s32 GM_GetSecondHonors(void) {
    return gpGame->pfnGetHonors(gpGame->pfnGetHonors(5));
}

// Puts every set-up player's ball on their tee set's tee at the start of a hole (and on a restart):
// lie 0, the physics ball reset, the before-shot copy (ballBefore) and the vBall/vA44 positions at
// the tee, the golfer waiting (GS_WAIT) and the low-IQ penalty cleared.
void GM_InitBallsToTee(void) {
    CourseInfo* pCourse = Ter_GetTGD();
    int         i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        gPlayers[i].ball.nLie = 0;
        Physics_InitBall(&gPlayers[i].ball, &pCourse->tee[gSession.nTeeSet[i]].x, i);
        Mem_cpy(&gPlayers[i].ballBefore, &gPlayers[i].ball, sizeof(Ball));
        LLMath_CopyVec(&pCourse->tee[gSession.nTeeSet[i]].x, gPlayers[i].vBall);
        LLMath_CopyVec(&pCourse->tee[gSession.nTeeSet[i]].x, gPlayers[i].vA44);
        GOLFERSTATE_Set(GS_WAIT, (u8)i);
        gPlayers[i].bLowIQPenalty = 0;
    }
}

// When every player is waiting (in split screen, those not holed yet are sent back to their
// pre-shot state instead), the mode's pfnSetupNextGolfer callback.
void GM_SetupGolfer_IfAllWaiting(void) {
    int i;
    u8  bBusy = 0;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) != GS_WAIT) {
            bBusy = 1;
        } else if (gSession.nSplitScreen && !Player_IsHoled(i)) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
            bBusy = 1;
        }
    }
    if (!bBusy) {
        gpGame->pfnSetupNextGolfer();
    }
}

// Whether the ball is out of bounds: its position is outside the course's in-bounds area
// (Ter_PointInOOBNetwork), or its physics state or lie already says so. nPlayer is not read.
u8 GM_IsBallOOB(int nPlayer, Ball* pBall) {
    if (!Ter_PointInOOBNetwork(pBall->vPos)) {
        return 1;
    }
    if (pBall->nState == PHYSICS_BALLSTATE_BallOutOfBounds_e || pBall->nLie == LIE_OUT_OF_BOUNDS_e) {
        return 1;
    }
    return 0;
}

// Picks the next playoff hole: the first time (GM_GetNeedToBuildPlayoffHoleList) the round's
// selected holes are saved as the pool (bHoleSaved); then the selection becomes one random pool
// hole other than the current one, or the current hole again when there is no other, and it becomes
// the current hole.
void GM_Pick_PlayOffHole(void) {
    int  nHoles[18];
    int  n = 0;
    int  i;
    int  nCur;
    if (GM_GetNeedToBuildPlayoffHoleList()) {
        for (i = 0; i < 18; i++) {
            gpGame->bHoleSaved[i] = gpGame->bHoleSelected[i];
        }
        GM_SetNeedToBuildPlayoffHoleList(0);
    }
    nCur = Game_CurHoleIndex();
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSaved[i] && i != nCur) {
            nHoles[n] = i;
            n++;
        }
    }
    if (n == 0) {
        GM_SelectHoleSet(0);
        GM_SetCurrentHole(nCur);
        GM_SelectSingleHole(nCur);
        return;
    }
    GM_SelectHoleSet(0);
    nCur = nHoles[Misc_RandFunc(0) % n];
    GM_SetCurrentHole(nCur);
    GM_SelectSingleHole(nCur);
}

// Whether the ball is in the hole (never in modes 13-17), setting its lie to holed: with
// Ter_Use3DCupGeometry on, when the lie already is; with it off, within half a yard of the pin.
u8 GM_CheckForBallInHole(int nPlayer) {
    CourseInfo* pCourse;
    int         nPinSet;
    f32         dx;
    f32         dz;
    f32         fDist;
    u8          b;
    if (GM_Currently_SkillZoneMode()) {
        return 0;
    }
    pCourse = Ter_GetTGD();
    nPinSet = Game_CurrentPinSet();
    dx = gPlayers[nPlayer].ball.vPos[0] - pCourse->pin[nPinSet].x;
    dz = gPlayers[nPlayer].ball.vPos[2] - pCourse->pin[nPinSet].z;
    fDist = Math_Sqrt(dx * dx + dz * dz);
    b = Ter_Use3DCupGeometry();
    if ((b && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) || (!b && fDist < 0.5f)) {
        gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
        return 1;
    }
    return 0;
}

// Whether target.c draws the player's ball-placement target: while the golfer is placing the ball
// (GS_PLACE_BALL), or when the mode's pfn230 says so.
u8 GM_RenderBallTarget(int nPlayer) {
    s8  nState = GOLFERSTATE_GetCurrentState(nPlayer);
    int b = 0;
    if (nState == GS_PLACE_BALL || gpGame->pfn230(nPlayer)) {
        b = 1;
    }
    return b;
}

// When play starts (GO_vInitIG): if no save profile is active and player 1 is human, save profile 0
// is set up afresh as "USER1" (fn_80057364). The loop before it does nothing (see inside).
void GM_SetupDefaultProfile(void) {
    int i;
    u8  bDead = 0;
    u8  bAny;
    if (gSession.n5B34 == 0) {
        // Dead code in the original: a loop over the holes testing a flag that is always 0 here.
        // Only the empty counting loop survives compilation, so the body is unknown.
        for (i = 0; i < 18; i++) {
            if (bDead) {
                fn_80057364(i);
            }
        }
    }
    bAny = 0;
    for (i = 0; i < 5; i++) {
        if (gpSaveData[i].bActive == 1) {
            bAny = 1;
        }
    }
    if (!bAny && !Player_IsCPU(0)) {
        fn_80057364(0);
    }
}

// Builds the "Dream 18" round (course 22): each hole's course and hole number from row 0 of the
// 'CMPS' table (fn_800D3118, fn_800D315C; the table counts holes from 1).
void GM_BuildDream18(void) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nHoleCourse[i] = fn_800D3118(22, i);
        gpGame->nHoleNum[i] = fn_800D315C(22, i) - 1;
    }
}

// Builds regional round nCourse (24..29: US Northwest, US Southwest, US East, Europe, Pacific, S.
// Hemisphere) from its row of the 'CMPS' table, as GM_BuildDream18 does.
void GM_BuildRegionalRound(int nCourse) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nHoleCourse[i] = fn_800D3118(nCourse, i);
        gpGame->nHoleNum[i] = fn_800D315C(nCourse, i) - 1;
    }
}

// Builds the "Random 18" round (course 23): two par 3s on the front nine and two on the back (not
// on neighbouring holes), then four par 5s the same way on holes still free (kept apart from each
// other only), then par 4s everywhere else. Each hole comes from a random course among the first
// nAvail entries of lbl_80184D40 (nAvail = how many of its courses any profile has unlocked) whose
// data is on the disc in the drive (fn_80110180; it retries until one is); no course and hole
// twice, and no course again while another is unused. Ends on the round's first hole.
// fake match: nCourse starts at 0 though every path sets it before use; without the initializer the
// par-4 loop's registers come out differently (found by the permuter).
void GM_BuildRandom18(void) {
    CourseList courses;
    u8  holes[18];      // the hole numbers (0..17) of the chosen par on the chosen course
    u8  bUsed[20];
    u32 slots[4];
    int nAvail = 0;
    int i;
    int k;
    u32 nCourse = 0;
    int n;
    u32 nPick;
    u32 nHole;
    int h;
    s8  nHoles;
    u8* p;

    courses = lbl_80184D40;
    for (i = 0; i < 20; i++) {
        bUsed[i] = 0;
    }
    for (i = 0; i < 20; i++) {
        for (k = 0; k < 5; k++) {
            if (COURSE_UNLOCKED(courses.a[i], k)) {
                nAvail++;
                break;
            }
        }
    }
    for (n = 0; n < 18; n++) {
        gpGame->nHoleCourse[n] = 0;
        gpGame->nHoleNum[n] = -1;
    }

    // Par 3s.
    for (n = 0; n < 4; n++) {
    slot3:
        if (n < 2) {
            slots[n] = Misc_RandFunc(1) % 9;
        } else {
            slots[n] = Misc_RandFunc(1) % 9 + 9;
        }
        for (k = 0; k < n; k++) {
            if (slots[n] == slots[k] || slots[n] == slots[k] + 1 || slots[n] == slots[k] - 1) {
                // fake match: a retry jump, as the binary branches; structured retries untried
                goto slot3;
            }
        }
    pick3:
        nPick = Misc_RandFunc(1) % nAvail;
        gpGame->nCurCourse = courses.a[nPick];
        gpGame->nCurHoleNum = 0;
        while (!fn_80110180()) {
            nPick = Misc_RandFunc(1) % nAvail;
            gpGame->nCurCourse = courses.a[nPick];
            gpGame->nCurHoleNum = 0;
            if (!fn_80110180()) {
                bUsed[nPick] = 1;
            }
        }
        nCourse = courses.a[nPick];
        for (k = 0; k < 20; k++) {
            for (i = 0; i < 5; i++) {
                if (COURSE_UNLOCKED(k, i)) {
                    break;
                }
            }
            if (k == nPick) {
                nCourse = courses.a[nPick];
                break;
            }
        }
        p = holes;
        nHoles = 0;
        for (h = 0; h < 18; h++) {
            if (fn_800D2ABC(nCourse, h) == 3) {
                *p++ = h;
                nHoles++;
            }
        }
        // fake match: the original sign-extends the picked byte into a register of its own (the
        // (s8) of a u8); a plain s8 array read loads straight into nHole's register
        nHole = (s8)holes[Misc_RandFunc(1) % nHoles];
        for (k = 0; k < n; k++) {
            if (nCourse == gpGame->nHoleCourse[slots[k]] && nHole == gpGame->nHoleNum[slots[k]]) {
                // fake match: a retry jump, as the binary branches; structured retries untried
                goto pick3;
            }
        }
        if (bUsed[nPick] == 1) {
            for (k = 0; k < nAvail; k++) {
                if (!bUsed[k]) {
                    // fake match: a retry jump, as the binary branches; structured retries untried
                    goto pick3;
                }
            }
        }
        bUsed[nPick] = 1;
        gpGame->nHoleCourse[slots[n]] = nCourse;
        gpGame->nHoleNum[slots[n]] = nHole;
    }

    // Par 5s.
    for (n = 0; n < 4; n++) {
    slot5:
        if (n < 2) {
            slots[n] = Misc_RandFunc(1) % 9;
        } else {
            slots[n] = Misc_RandFunc(1) % 9 + 9;
        }
        if (gpGame->nHoleNum[slots[n]] != -1) {
            // fake match: a retry jump, as the binary branches; structured retries untried
            goto slot5;
        }
        for (k = 0; k < n; k++) {
            if (slots[n] == slots[k] || slots[n] == slots[k] + 1 || slots[n] == slots[k] - 1) {
                // fake match: a retry jump, as the binary branches; structured retries untried
                goto slot5;
            }
        }
    pick5:
        nPick = Misc_RandFunc(1) % nAvail;
        gpGame->nCurCourse = courses.a[nPick];
        gpGame->nCurHoleNum = 0;
        while (!fn_80110180()) {
            nPick = Misc_RandFunc(1) % nAvail;
            gpGame->nCurCourse = courses.a[nPick];
            gpGame->nCurHoleNum = 0;
            if (!fn_80110180()) {
                bUsed[nPick] = 1;
            }
        }
        nCourse = courses.a[nPick];
        p = holes;
        nHoles = 0;
        for (h = 0; h < 18; h++) {
            if (fn_800D2ABC(nCourse, h) == 5) {
                *p++ = h;
                nHoles++;
            }
        }
        nHole = (s8)holes[Misc_RandFunc(1) % nHoles];
        for (k = 0; k < n; k++) {
            if (nCourse == gpGame->nHoleCourse[slots[k]] && nHole == gpGame->nHoleNum[slots[k]]) {
                // fake match: a retry jump, as the binary branches; structured retries untried
                goto pick5;
            }
        }
        if (bUsed[nPick] == 1) {
            for (k = 0; k < nAvail; k++) {
                if (!bUsed[k]) {
                    // fake match: a retry jump, as the binary branches; structured retries untried
                    goto pick5;
                }
            }
        }
        bUsed[nPick] = 1;
        gpGame->nHoleCourse[slots[n]] = nCourse;
        gpGame->nHoleNum[slots[n]] = nHole;
    }

    // Par 4s for the rest.
    for (i = 0; i < 18; i++) {
        if (gpGame->nHoleNum[i] == -1) {
        pick4:
            nPick = Misc_RandFunc(1) % nAvail;
            gpGame->nCurCourse = courses.a[nPick];
            gpGame->nCurHoleNum = 0;
            while (!fn_80110180()) {
                nPick = Misc_RandFunc(1) % nAvail;
                gpGame->nCurCourse = courses.a[nPick];
                gpGame->nCurHoleNum = 0;
                if (!fn_80110180()) {
                    bUsed[nPick] = 1;
                }
            }
            nHoles = 0;
            nCourse = courses.a[nPick];
            p = holes;
            for (h = 0; h < 18; h++) {
                if (fn_800D2ABC(nCourse, h) == 4) {
                    *p++ = h;
                    nHoles++;
                }
            }
            nHole = (s8)holes[Misc_RandFunc(1) % nHoles];
            for (k = 0; k < i; k++) {
                if (nCourse == gpGame->nHoleCourse[k] && nHole == gpGame->nHoleNum[k]) {
                    // fake match: a retry jump, as the binary branches; structured retries untried
                    goto pick4;
                }
            }
            if (bUsed[nPick] == 1) {
                for (k = 0; k < nAvail; k++) {
                    if (!bUsed[k]) {
                        // fake match: a retry jump, as the binary branches; structured retries untried
                        goto pick4;
                    }
                }
            }
            gpGame->nHoleCourse[i] = nCourse;
            bUsed[nPick] = 1;
            gpGame->nHoleNum[i] = nHole;
        }
    }
    GM_InitializeCurrentHoleToFirstSelected();
}

// Whether the game mode is one of the skill-zone games (modes 13-17).
u8 GM_Currently_SkillZoneMode(void) {
    if (Game_GetMode() == 13 || Game_GetMode() == 14 || Game_GetMode() == 15 || Game_GetMode() == 16 ||
        Game_GetMode() == 17) {
        return 1;
    }
    return 0;
}

// Whether the game mode is speed golf (modes 6-8).
u8 GM_IsSpeedGolfMode(void) {
    if (Game_GetMode() == 6 || Game_GetMode() == 7 || Game_GetMode() == 8) {
        return 1;
    }
    return 0;
}

// The default mode callbacks GM_SetModeType installs.
s32 GM_DefaultTargetState(int a, int nTarget) {
    return 0;
}

void GM_DefaultBonusCollected(int nPlayer, int nId) {
}

void GM_DefaultSetTimeLeft(int nPlayer, int nTime) {
}

s32 GM_DefaultZeroPlayer(int nPlayer) {
    return 0;
}

u8 GM_DefaultTruePlayer(int nPlayer) {
    return 1;
}

u8 GM_DefaultTrue(void) {
    return 1;
}

u8 GM_DefaultFalsePlayer(int nPlayer) {
    return 0;
}

void GM_DefaultNoOpPlayer(int nPlayer) {
}

u8 GM_DefaultGoToPlayoff(u8 bCheck) {
    return 0;
}

u8 GM_DefaultGameFinished(u8 bCheck) {
    return 0;
}

u8 GM_DefaultHoleFinished(int nPlayer, u8 bCheck) {
    return 0;
}

s32 GM_DefaultGetHonors(int a) {
    return 0;
}

void GM_DefaultNoOp(void) {
}

// Whether GM_Pick_PlayOffHole still has to save the round's hole selection as the playoff holes
// (gpGame->b135).
u8 GM_GetNeedToBuildPlayoffHoleList(void) {
    return gpGame->b135;
}

// Sends UI message 31 (no values): from GUI_Init at the start of a hole, and when the last
// pause-menu screen closes (fn_800E5240).
void GUI_SendMessage31(void) {
    fn_800E58B4(31);
}
