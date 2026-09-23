// GameRound.c (our name): the round and hole setup that follows GameManager.c - setting up a
// hole, the stroke limit, created-course data, hole names. TW06 has no counterpart file.

#include "golfer.h"

int  fn_800E19A4(int nPlayer, int nHoles);
void fn_800E25CC(u8 b);
void fn_800E1434(void);
void fn_800E1480(int nHole);
int  fn_800E1CE8(int a, int b);
u8   fn_800588F4(u8* pProfile, int a, int i);

void  fn_800D8D5C(int nPlayer, int a);
void  fn_800E2470(void);
void  fn_800E30D4(void);
void  fn_800E2FD8(void);
void  fn_800E3050(void);
int   fn_800D2AD8(int nHole);               // a hole's par
u8    fn_800EE470(void);
int   fn_8011937C(int nPlayer, int a, u8 b);

int   fn_800E8C24(int nPlayer, int nHole);
u8    Player_IsCPU(int nPlayer);
int   sprintf(char* pBuf, const char* pFmt, ...);
int   Game_GetCourse(void);

extern u8   gNumPlayersSetUp;               // 0x80281D48 (Golfer.c)
extern char lbl_80282270[8];                // the hole name

extern u8* gpSaveData;
extern u8  lbl_8028227C;

// out = a - b (four floats)
asm void fn_800E0AF0(register f32* pA, register f32* pB, register f32* pOut) {
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

// out = a - b (three floats)
asm void fn_800E0B14(register f32* pA, register f32* pB, register f32* pOut) {
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

// Clears one player's record of one hole: strokes, putts, points and the rest.
void fn_800E1018(int nPlayer, int nHole) {
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
// the session's pin option (-1 = the first pin), and the first hole chosen.
void fn_800E1074(void) {
    int     j;
    int     i;
    Player* p;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 18; j++) {
            fn_800E1018(i, j);
        }
        p = (Player*)((u8*)gPlayers + i * sizeof(Player));
        for (j = 0; j < 4; j++) {
            p->nRoundScore[j] = 0;
        }
        p->unk28C = 0;
        p->nHolesWon = 0;
        p->n274 = 0;
        p->n2D8 = 0;
        p->n2DC = 0;
        p->n2E0 = 0;
        p->n308 = 0;
        p->nC44 = 3000;
        p->nC3C = 0;
        p->nC4C = 0;
        p->nC48 = 0;
        for (j = 0; j < 18; j++) {
            p->nC6C[j] = 0;
        }
        fn_800D8D5C(i, 0);
        if (gpSaveData[p->nIndex * 0x10600] != 0) {
            gpSaveData[p->nIndex * 0x10600 + 0x70] = 0;
        }
    }
    for (i = 0; i < 18; i++) {
        if (gSession.unk5B38 == -1) {
            gpGame->holeOrder[i] = 0;
        } else {
            gpGame->holeOrder[i] = gSession.unk5B38;
        }
    }
    fn_800E2470();
    gpGame->nE0 = 1;
    gpGame->bD5 = 0;
}

// A hole-selection preset for the round: 0 none, 1 all, 2 the front nine, 3 the back nine,
// 4/5/6 only the par 5s/4s/3s, 7 all; then the first selected hole.
void fn_800E1260(int nPreset) {
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
            if (fn_800D2AD8(i) == 5) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 5:
            if (fn_800D2AD8(i) == 4) {
                gpGame->bHoleSelected[i] = 1;
            } else {
                gpGame->bHoleSelected[i] = 0;
            }
            break;
        case 6:
            if (fn_800D2AD8(i) == 3) {
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
    fn_800E1434();
}

// Adds a hole to the round and moves to the round's first hole.
void fn_800E1404(int nHole) {
    gpGame->bHoleSelected[nHole] = 1;
    fn_800E1434();
}

// Moves to the round's first hole.
void fn_800E1434(void) {
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            fn_800E1480(i);
            return;
        }
    }
}

// Makes a hole of the round the current one: its number, and its course when the round mixes
// courses.
void fn_800E1480(int nHole) {
    gpGame->nCurHole = nHole;
    gpGame->nCurHoleNum = gpGame->nHoleNum[nHole];
    if (gpGame->b136 || gpGame->b137 || gpGame->b139 || gpGame->b138) {
        gpGame->nCurCourse = gpGame->nHoleCourse[nHole];
    }
}

// Sets the round's course. 23, 22 and 24..29 are the mixed rounds (built by fn_800E30D4,
// fn_800E2FD8 and fn_800E3050); any other value is one course's holes 1..18.
void fn_800E14E0(int nCourse) {
    int i;
    if (nCourse == 23) {
        fn_800E30D4();
        gpGame->b137 = 1;
        return;
    }
    gpGame->b137 = 0;
    if (nCourse == 22) {
        gpGame->b138 = 1;
        fn_800E2FD8();
        gpGame->nCurCourse = gpGame->nHoleCourse[gpGame->nCurHole];
        return;
    }
    gpGame->b138 = 0;
    if (nCourse >= 24 && nCourse < 30) {
        gpGame->b139 = nCourse - 23;
        fn_800E3050();
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
int fn_800E16F4(void) {
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return i;
        }
    }
    return -1;
}

// Whether the current hole is the round's first.
u8 fn_800E1734(void) {
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return i == gpGame->nCurHole;
        }
    }
    return 0;
}

// The mode's mulligan rule: 0 none, 2 one per player per round.
int fn_800E177C(void) {
    return gpGame->nMulligans;
}

void fn_800E1788(int nPlayer) {
    fn_800E19A4(nPlayer, 18);
}

// A player's total for the first nHoles holes: the mode's points in mode 18 (match play),
// fn_800E8C24's count in mode 19, strokes otherwise.
int fn_800E19A4(int nPlayer, int nHoles) {
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

// Whether the round plays every hole (or the mode's own answer, 0xD5, when it keeps one).
u8 fn_800E1BBC(void) {
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
int fn_800E1CA8(void) {
    int b = 1;
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            b = 0;
        }
    }
    return b;
}

// A player's total strokes for the round.
int fn_800E17AC(int nPlayer) {
    int i;
    int n = 0;
    for (i = 0; i < 18; i++) {
        n += gPlayers[nPlayer].nStrokes[i];
    }
    return n;
}

// Strokes against par over the holes played so far (and the current one, when asked and the
// ball is in the hole).
int fn_800E184C(int nPlayer, u8 bCurrent) {
    int nPar = 0;
    int nStrokes = 0;
    int i;
    int nEnd = gpGame->nCurHole;
    if (bCurrent && gPlayers[nPlayer].nLie == LIE_HOLED && nEnd < 18) {
        nEnd++;
    }
    for (i = 0; i < nEnd; i++) {
        if (gpGame->bHoleSelected[i]) {
            nPar += fn_800D2AD8(i);
            nStrokes += gPlayers[nPlayer].nStrokes[i];
        }
    }
    return nStrokes - nPar;
}

// The score shown for a player: the online game's, the mode's own total, or strokes against par.
int fn_800E1904(int nPlayer, u8 bCurrent) {
    if (fn_800EE470()) {
        return fn_8011937C(nPlayer, 0, bCurrent);
    }
    if (gpGame->bD4) {
        return gPlayers[nPlayer].n2D8;
    }
    if (gpGame->nDC < gpGame->nE0) {
        return fn_800E184C(nPlayer, bCurrent);
    }
    return 0;
}

// The 75 marked holes (three to five per course; the items GM_GetGameProgress counts with
// fn_800588F4): a course and hole to the item index, or -1. EA wrote the cases as 1-based hole
// numbers; the courses are in the original's order, which numbers the items.
int fn_800E1CE8(int nCourse, int nHole) {
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

int fn_800E22E4(int nSlot, int a, int b) {
    int i = fn_800E1CE8(a, b);
    if (i != -1) {
        return fn_800588F4(gpSaveData + nSlot * 0x10600, 0, i);
    }
    return 0;
}

u8 fn_800E234C(int nSlot, int a, int b) {
    int i = fn_800E1CE8(a, b);
    if (i != -1) {
        return fn_800588F4(gpSaveData + nSlot * 0x10600, 1, i);
    }
    return 0;
}

// The hole's stroke limit: with the session's limit option (0x5B39) on and the mode using it,
// 10 strokes ends the hole (GM_PlayerTookShot picks the ball up).
u8 fn_800E23B0(int nPlayer, int nStrokes) {
    if (gSession.unk5B39 && gpGame->bStrokeLimit && nStrokes >= 10) {
        return 1;
    }
    return 0;
}

// Whether a player may take a mulligan: humans only, the mode allows them, and in the
// one-per-round rule not already used.
u8 fn_800E23EC(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800E177C() == 0) {
        return 0;
    }
    if (fn_800E177C() == 2 && gPlayers[nPlayer].unkC28) {
        return 0;
    }
    return 1;
}

// Gives every player their mulligan back.
void fn_800E2470(void) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->unkC28 = 0;
    }
}

// A number per game mode (1, 2 or 4; 0 for most): modes 6-8 give 1 or 2 by gpGame->n4.
int fn_800E2520(int nMode) {
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

void fn_800E25CC(u8 b) {
    lbl_8028227C = b;
    gSession.nSplitScreen = b;
}

// Split screen for the mode: modes 0-2 as the session has it, 6, 7 and 26 always, others never.
void fn_800E25E0(void) {
    switch (Game_GetMode()) {
    case 6:
    case 7:
        fn_800E25CC(1);
        return;
    case 26:
        fn_800E25CC(1);
        return;
    case 0:
    case 1:
    case 2:
        if (gSession.nSplitScreen == 1) {
            fn_800E25CC(1);
            return;
        }
        fn_800E25CC(0);
        return;
    default:
        fn_800E25CC(0);
        return;
    }
}

int fn_800E27A8(void) {
    return gpGame->n294 != 0;
}

// The course's folder name ("01_Peb" = Pebble Beach ...). Course 5's is "22_Ant".
char* fn_800E2680(void) {
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

// TW06: GameManager::GetHoleName. "HOLE_01" .. "HOLE_18".
char* GameManager_GetHoleName(int nHole) {
    sprintf(lbl_80282270, "HOLE_%02d", nHole + 1);
    return lbl_80282270;
}

// Seconds since the round's clock was last reset (gpGame->n12C holds the session's frame count
// then).
int fn_800E27C0(void) {
    return (1.0f / 59.94f) * (f32)(u32)(gSession.unk24 - gpGame->n12C);
}
