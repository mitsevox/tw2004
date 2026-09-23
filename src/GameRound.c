// GameRound.c (our name): the round and hole setup that follows GameManager.c - setting up a
// hole, the stroke limit, created-course data, hole names. TW06 has no counterpart file.

#include "golfer.h"
#include "ball.h"
#include "physics.h"
#include "game.h"
#include "engine.h"

int  fn_800E19A4(int nPlayer, int nHoles);
void fn_800E25CC(u8 b);
int  fn_800E1CE8(int a, int b);
u8   fn_800588F4(u8* pProfile, int a, int i);

void  fn_800D8D5C(int nPlayer, int a);
void  fn_800E2470(void);
void  fn_800E30D4(void);
void  fn_800E2FD8(void);
void  fn_800E3050(int nCourse);
int   fn_8011937C(int nPlayer, int a, u8 b);

int   fn_800E8C24(int nPlayer, int nHole);
int   sprintf(char* pBuf, const char* pFmt, ...);

extern u8   gNumPlayersSetUp;               // 0x80281D48 (Golfer.c)
extern char lbl_80282270[8];                // the hole name

void  fn_800E3AF4(void);
s32   fn_800E3AEC(int a);
u8    fn_800E3AE4(int nPlayer, int a);
u8    fn_800E3ADC(int a);
s32   fn_800E3AD4(void);
void  fn_800CF158(int nPlayer);
u8    fn_800CF450(int nPlayer);
void  fn_800CFE74(void);
void  fn_800D0098(void);
void  fn_800D030C(void);
void  fn_800E3AD0(int nPlayer);
u8    fn_800E3AC8(int nPlayer);
u8    fn_800E3AC0(void);
u8    fn_800E3AB8(int nPlayer);
s32   fn_800E3AB0(void);
void  fn_800E3AAC(int nPlayer, int nTime);
void  fn_800E3AA8(int nPlayer, int nId);
s32   fn_800E3AA0(int a, int nTarget);
void  fn_800FF700(void);
void  fn_800E9E40(void);
void  fn_800F80FC(void);
void  fn_80101FEC(void);
void  fn_800F944C(void);
void  fn_800F9610(void);
void  fn_800F986C(void);
void  fn_800ED738(void);
void  fn_800EACD8(void);
void  fn_800F125C(void);
void  fn_800FFF34(void);
void  fn_800FEAFC(void);
void  fn_800F6A60(void);
void  fn_800F2984(void);
void  fn_800F39F4(void);
void  fn_800F4B40(void);
void  fn_800F5AAC(void);
void  fn_800FE1B4(void);
void  fn_800E81C4(void);
void  fn_800E8D58(void);
void  fn_800E68F0(void);
void  fn_800EDD18(void);
void  fn_800F0448(void);
void  fn_800E7980(void);
void  fn_8010C4A0(void);
void  fn_80125E68(void);
extern s32 lbl_80282278;

u8    fn_800E3AF8(void);
void  fn_800E0A84(u8 v);
void  fn_800E1404(int nHole);
u8    Ter_Use3DCupGeometry(void);
void  fn_80057364(int a);
int   fn_800D3118(int nRound, int nHole);    // a built round's course for a hole
int   fn_800D315C(int nRound, int nHole);    // and its hole number (1-based)

// The tee positions follow the pins in the per-hole data (fn_8000C594).
typedef struct HoleTees {
    u8     unk0[0xB0];
    PinPos tee[4];                          // 0xB0  one per tee set
} HoleTees;

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

int   fn_80110180(void);                    // the current hole can be played (inferred)
int   fn_800D2ABC(int nCourse, int nHole);  // a hole's par
extern u8* lbl_80281DF4;                    // unlock flags (a second save block)

// The 20 course ids the mixed rounds pick from (lbl_80184D40).
typedef struct CourseList {
    u32 a[20];
} CourseList;
extern CourseList lbl_80184D40;

// A course counts as unlocked when any of the five profiles (or the second block) has its flag.
#define COURSE_UNLOCKED(c, k) (gpSaveData[(k) * 0x10600 + (c) + 0x3A] || lbl_80281DF4[(c) + 0x3A])


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

// Sets up a game mode: every rule flag and callback to its default (the callbacks are mostly
// empty stubs), then the mode's own setup (27 modes), then the round as holes 1..18.
void fn_800E0B38(int nMode) {
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
    gpGame->pfn1C8 = fn_800E3AF4;
    gpGame->pfn1CC = fn_800E3AF4;
    gpGame->pfn1D0 = fn_800E3AF4;
    gpGame->pfn1D4 = fn_800E3AEC;
    gpGame->pfn1D8 = fn_800E3AE4;
    gpGame->pfn1DC = fn_800E3ADC;
    gpGame->pfn1E0 = fn_800E3AD4;
    gpGame->pfn1E4 = fn_800E3AF4;
    gpGame->pfn1E8 = fn_800E3AF4;
    gpGame->pfn1EC = fn_800E3AF4;
    gpGame->pfn1F0 = fn_800E3AF4;
    gpGame->pfn1F4 = fn_800E3AF4;
    gpGame->pfn1F8 = fn_800CF158;
    gpGame->pfn1FC = fn_800CF450;
    gpGame->pfn200 = fn_800CFE74;
    gpGame->pfn204 = fn_800D0098;
    gpGame->pfn208 = fn_800D030C;
    gpGame->pfn210 = fn_800E3AD0;
    gpGame->pfn214 = fn_800E3AF4;
    gpGame->pfn218 = fn_800E3AD0;
    gpGame->pfn21C = fn_800E3AD0;
    gpGame->pfn220 = fn_800E3AF4;
    gpGame->pfn224 = fn_800E3AF4;
    gpGame->pfn228 = fn_800E3AD0;
    gpGame->pfn22C = fn_800E3AD0;
    gpGame->pfn230 = fn_800E3AC8;
    gpGame->pfn234 = fn_800E3AC0;
    gpGame->pfn238 = fn_800E3AB8;
    gpGame->pfn23C = fn_800E3AD0;
    gpGame->pfn240 = fn_800E3AB0;
    gpGame->pfn244 = fn_800E3AD0;
    gpGame->pfn248 = fn_800E3AD0;
    gpGame->pfn24C = fn_800E3AD0;
    gpGame->pfn250 = fn_800E3AD0;
    gpGame->pfn254 = fn_800E3AD0;
    gpGame->pfn258 = fn_800E3AC8;
    gpGame->pfn25C = fn_800E3AAC;
    gpGame->pfn260 = fn_800E3AD0;
    gpGame->pfn264 = fn_800E3AC8;
    gpGame->pfn268 = fn_800E3AA8;
    gpGame->pfn26C = fn_800E3AA0;
    gpGame->pfn20C = fn_800E3AD0;
    lbl_80282278 = 0;
    switch (Game_GetMode()) {
    case 0:
        fn_800FF700();
        break;
    case 1:
        fn_800E9E40();
        break;
    case 2:
        fn_800F80FC();
        break;
    case 4:
        fn_80101FEC();
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
        fn_800EACD8();
        break;
    case 10:
        fn_800F125C();
        break;
    case 11:
        fn_800FFF34();
        break;
    case 12:
        fn_800FEAFC();
        break;
    case 13:
        fn_800F6A60();
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
        fn_800FE1B4();
        break;
    case 19:
        fn_800E81C4();
        break;
    case 20:
        fn_800E8D58();
        break;
    case 21:
        fn_800E68F0();
        break;
    case 23:
        fn_800EDD18();
        break;
    case 24:
        fn_800F0448();
        break;
    case 25:
        fn_800E7980();
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
        fn_800E3050(nCourse);
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
    int nPar;
    int nStrokes = 0;
    int i;
    int nEnd = gpGame->nCurHole;
    nPar = 0;
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

// Runs the mode's player choice twice (fn_800E292C runs it once).
void fn_800E295C(void) {
    gpGame->pfn1D4(gpGame->pfn1D4(5));
}

// The start of a hole: every player's ball on their tee, the look-ahead copy and the saved
// positions reset, and everyone waiting.
void fn_800E299C(void) {
    CourseInfo* pCourse = fn_8000C594();
    int         i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        gPlayers[i].nLie = 0;
        fn_80055AA8((Ball*)gPlayers[i].ball, &((HoleTees*)pCourse)->tee[gSession.nTeeSet[i]].x, i);
        Mem_cpy(gPlayers[i].ballBefore, gPlayers[i].ball, 0xBC);
        Vec_Copy(&((HoleTees*)pCourse)->tee[gSession.nTeeSet[i]].x, &gPlayers[i].fBallX);
        Vec_Copy(&((HoleTees*)pCourse)->tee[gSession.nTeeSet[i]].x, gPlayers[i].vA44);
        GOLFERSTATE_Set(GS_WAIT, (u8)i);
        gPlayers[i].bLowIQPenalty = 0;
    }
}

// When every player is waiting (in split screen, those not holed yet are sent back to their
// pre-shot state instead), the mode's "everyone done" callback.
void fn_800E2A88(void) {
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
        gpGame->pfn1D0();
    }
}

// Out of bounds: outside the in-bounds area, or the ball out (state 5) or in lie 16.
u8 fn_800E2B40(int nPlayer, Ball* pBall) {
    if (!Ter_PointInOOBNetwork(pBall->vPos)) {
        return 1;
    }
    if (pBall->nState == PHYSICS_BALLSTATE_BallOutOfBounds_e || pBall->nLie == LIE_OUT_OF_BOUNDS_e) {
        return 1;
    }
    return 0;
}

// A random hole from the round's selection, not the one just played.
void fn_800E2BA4(void) {
    int  nHoles[18];
    int  n = 0;
    int  i;
    int  nCur;
    if (fn_800E3AF8()) {
        for (i = 0; i < 18; i++) {
            gpGame->bHoleSaved[i] = gpGame->bHoleSelected[i];
        }
        fn_800E0A84(0);
    }
    nCur = Game_CurHoleIndex();
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSaved[i] && i != nCur) {
            nHoles[n] = i;
            n++;
        }
    }
    if (n == 0) {
        fn_800E1260(0);
        fn_800E1480(nCur);
        fn_800E1404(nCur);
        return;
    }
    fn_800E1260(0);
    nCur = nHoles[Rand_Next(0) % n];
    fn_800E1480(nCur);
    fn_800E1404(nCur);
}

// Whether the ball is in the hole: it is (lie "holed") when Ter_Use3DCupGeometry says so and the lie is
// already holed, or, when it says no, when the ball is within half a yard of the pin.
u8 fn_800E2DB4(int nPlayer) {
    CourseInfo* pCourse;
    int         nHole;
    f32         dx;
    f32         dz;
    f32         fDist;
    u8          b;
    if (fn_800E39F0()) {
        return 0;
    }
    pCourse = fn_8000C594();
    nHole = Game_CurrentHole();
    dx = *(f32*)(gPlayers[nPlayer].ball + 0) - pCourse->pin[nHole].x;
    dz = *(f32*)(gPlayers[nPlayer].ball + 8) - pCourse->pin[nHole].z;
    fDist = fn_80009680(dx * dx + dz * dz);
    b = Ter_Use3DCupGeometry();
    if ((b && gPlayers[nPlayer].nLie == LIE_HOLED) || (!b && fDist < 0.5f)) {
        gPlayers[nPlayer].nLie = LIE_HOLED;
        return 1;
    }
    return 0;
}

// Whether the player is placing the ball (state 22) or the mode says so.
u8 fn_800E2EAC(int nPlayer) {
    s8  nState = GOLFERSTATE_GetCurrentState(nPlayer);
    int b = 0;
    if (nState == GS_PLACE_BALL || gpGame->pfn230(nPlayer)) {
        b = 1;
    }
    return b;
}

// With none of the five save slots in use and player 1 human, fn_80057364(0).
void fn_800E2F14(void) {
    int i;
    u8  bDead = 0;
    u8  bAny;
    if (gSession.unk5B34 == 0) {
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
        if (gpSaveData[i * 0x10600] == 1) {
            bAny = 1;
        }
    }
    if (!bAny && !Player_IsCPU(0)) {
        fn_80057364(0);
    }
}

// Builds mixed round 22 from its table: each hole's course and hole number.
void fn_800E2FD8(void) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nHoleCourse[i] = fn_800D3118(22, i);
        gpGame->nHoleNum[i] = fn_800D315C(22, i) - 1;
    }
}

// The same for rounds 24-29.
void fn_800E3050(int nCourse) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nHoleCourse[i] = fn_800D3118(nCourse, i);
        gpGame->nHoleNum[i] = fn_800D315C(nCourse, i) - 1;
    }
}

// Builds a random mixed round from the unlocked courses: two par 3s on the front nine and two on
// the back (never on neighbouring holes), then four par 5s the same way, then par 4s everywhere
// else. Each hole comes from a random course, no hole twice, and every course is used once
// before any is used again.
void fn_800E30D4(void) {
    CourseList courses;
    s8  holes[18];
    u8  bUsed[20];
    u32 slots[4];
    int nAvail = 0;
    int i;
    int k;
    u32 nCourse;
    int n;
    u32 nPick;
    u32 nHole;
    int h;
    s8  nHoles;
    s8* p;

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
            slots[n] = Rand_Next(1) % 9;
        } else {
            slots[n] = Rand_Next(1) % 9 + 9;
        }
        for (k = 0; k < n; k++) {
            if (slots[n] == slots[k] || slots[n] == slots[k] + 1 || slots[n] == slots[k] - 1) {
                goto slot3;
            }
        }
    pick3:
        nPick = Rand_Next(1) % nAvail;
        gpGame->nCurCourse = courses.a[nPick];
        gpGame->nCurHoleNum = 0;
        while (!fn_80110180()) {
            nPick = Rand_Next(1) % nAvail;
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
        nHole = holes[Rand_Next(1) % nHoles];
        for (k = 0; k < n; k++) {
            if (nCourse == gpGame->nHoleCourse[slots[k]] && nHole == gpGame->nHoleNum[slots[k]]) {
                goto pick3;
            }
        }
        if (bUsed[nPick] == 1) {
            for (k = 0; k < nAvail; k++) {
                if (!bUsed[k]) {
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
            slots[n] = Rand_Next(1) % 9;
        } else {
            slots[n] = Rand_Next(1) % 9 + 9;
        }
        if (gpGame->nHoleNum[slots[n]] != -1) {
            goto slot5;
        }
        for (k = 0; k < n; k++) {
            if (slots[n] == slots[k] || slots[n] == slots[k] + 1 || slots[n] == slots[k] - 1) {
                goto slot5;
            }
        }
    pick5:
        nPick = Rand_Next(1) % nAvail;
        gpGame->nCurCourse = courses.a[nPick];
        gpGame->nCurHoleNum = 0;
        while (!fn_80110180()) {
            nPick = Rand_Next(1) % nAvail;
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
        nHole = holes[Rand_Next(1) % nHoles];
        for (k = 0; k < n; k++) {
            if (nCourse == gpGame->nHoleCourse[slots[k]] && nHole == gpGame->nHoleNum[slots[k]]) {
                goto pick5;
            }
        }
        if (bUsed[nPick] == 1) {
            for (k = 0; k < nAvail; k++) {
                if (!bUsed[k]) {
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
            nPick = Rand_Next(1) % nAvail;
            gpGame->nCurCourse = courses.a[nPick];
            gpGame->nCurHoleNum = 0;
            while (!fn_80110180()) {
                nPick = Rand_Next(1) % nAvail;
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
            nHole = holes[Rand_Next(1) % nHoles];
            for (k = 0; k < i; k++) {
                if (nCourse == gpGame->nHoleCourse[k] && nHole == gpGame->nHoleNum[k]) {
                    goto pick4;
                }
            }
            if (bUsed[nPick] == 1) {
                for (k = 0; k < nAvail; k++) {
                    if (!bUsed[k]) {
                        goto pick4;
                    }
                }
            }
            gpGame->nHoleCourse[i] = nCourse;
            bUsed[nPick] = 1;
            gpGame->nHoleNum[i] = nHole;
        }
    }
    fn_800E1434();
}

// A gimme (formerly its own unit, Gimme.c): the Gimmes option is on, it is not split screen or a
// replay, the session is not in the mode with both flag bits 0x4000 and 0x8000, the game mode
// allows gimmes and its rules callback does not object, and the ball is within half a yard
// (18 inches) of the pin - on the putter, or in any shot of a one-player game. Called from swing
// state 14; yes leads to state 15 (the tap-in is planned) and 16 (played for the player).
u8 Gimme_Allowed(int nPlayer) {
    if (!SESSION_OPTIONS->bGimmes) return 0;
    if (gSession.nSplitScreen) return 0;
    if (gSession.bReplay) return 0;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) return 0;
    if (!gpGame->bGimmesAllowed) return 0;
    if (gpGame->pfn1D8(nPlayer, 1)) return 0;
    if (fn_800D0478(nPlayer) > 0.5f) return 0;
    if (gPlayers[nPlayer].nClub != CLUB_PUTTER && gSession.nNumPlayers > 1) return 0;
    return 1;
}

void fn_800E292C(void) {
    gpGame->pfn1D4(5);
}

// Modes 13-17.
u8 fn_800E39F0(void) {
    if (Game_GetMode() == 13 || Game_GetMode() == 14 || Game_GetMode() == 15 || Game_GetMode() == 16 ||
        Game_GetMode() == 17) {
        return 1;
    }
    return 0;
}

// Modes 6-8.
int fn_800E3A54(void) {
    if (Game_GetMode() == 6 || Game_GetMode() == 7 || Game_GetMode() == 8) {
        return 1;
    }
    return 0;
}

// The default mode callbacks fn_800E0B38 installs.
s32 fn_800E3AA0(int a, int nTarget) {
    return 0;
}

void fn_800E3AA8(int nPlayer, int nId) {
}

void fn_800E3AAC(int nPlayer, int nTime) {
}

s32 fn_800E3AB0(void) {
    return 0;
}

u8 fn_800E3AB8(int nPlayer) {
    return 1;
}

u8 fn_800E3AC0(void) {
    return 1;
}

u8 fn_800E3AC8(int nPlayer) {
    return 0;
}

void fn_800E3AD0(int nPlayer) {
}

s32 fn_800E3AD4(void) {
    return 0;
}

u8 fn_800E3ADC(int a) {
    return 0;
}

u8 fn_800E3AE4(int nPlayer, int a) {
    return 0;
}

s32 fn_800E3AEC(int a) {
    return 0;
}

void fn_800E3AF4(void) {
}

u8 fn_800E3AF8(void) {
    return gpGame->b135;
}

void fn_800E3B04(void) {
    fn_800E58B4(31);
}
