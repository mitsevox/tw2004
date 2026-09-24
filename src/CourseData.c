// CourseData.c (our name): the course table read from the disc ('CRI ' chunk: every course's 18
// holes with their par, wind and per-tee values) and the built rounds ('CMPS' chunk: 18 holes
// picked from other courses), with the par and hole lookups the round, the tour and the HUD use.

#include "game_types.h"
#include "game.h"

void fn_800D29EC(void);
void fn_800D2A30(void);
void fn_800D2A64(UStreamObject* pObject);
void fn_800D2A90(UStreamObject* pObject);
s32  fn_800D2B4C(int nHole);
s32  fn_800D2B80(int nCourse, int nHole, int nTee);
s32  fn_800D2D40(int nTee);
s32  fn_800D2DA0(int nTee);
s32  fn_800D2E00(int nTee);
s32  fn_800D2E60(void);
s32  fn_800D2EB0(void);
u8   fn_800D3004(void);
u8   fn_800D304C(int nHole);
int  fn_800D30B4(int nRound);
int  fn_800D3208(void);

// Nothing to free: the tables are static.
void fn_800D29E8(void) {
}

void fn_800D29EC(void) {
    UStream_RegisterHandler('CRI ', fn_800D2A64);
    UStream_RegisterHandler('CMPS', fn_800D2A90);
}

void fn_800D2A30(void) {
    UStream_UnregisterHandler('CRI ');
    UStream_UnregisterHandler('CMPS');
}

// port: both chunks are copied straight into their tables; they are big-endian on disc, so a
// little-endian port converts them field by field here (docs/format-byteorder.md)
void fn_800D2A64(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA2F4), lbl_801FA2F4);
}

void fn_800D2A90(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA1F8), lbl_801FA1F8);
}

int fn_800D2ABC(int nCourse, int nHole) {
    return lbl_801FA2F4[nCourse].aHoles[nHole].nPar;
}

// The par of the round's hole nHole.
int fn_800D2AD8(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].nPar;
}

// The current hole's par.
int fn_800D2B08(void) {
    return lbl_801FA2F4[Game_GetCourse()].aHoles[fn_80015464()].nPar;
}

s32 fn_800D2B4C(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].n04;
}

// A hole's value for tee set nTee (0..3), 0 for any other.
s32 fn_800D2B80(int nCourse, int nHole, int nTee) {
    switch (nTee) {
    case 0:
        return lbl_801FA2F4[nCourse].aHoles[nHole].n14;
    case 1:
        return lbl_801FA2F4[nCourse].aHoles[nHole].n10;
    case 2:
        return lbl_801FA2F4[nCourse].aHoles[nHole].n0C;
    case 3:
        return lbl_801FA2F4[nCourse].aHoles[nHole].n08;
    }
    return 0;
}

s32 fn_800D2C30(int nHole, int nTee) {
    return fn_800D2B80(gpGame->nHoleCourse[nHole], gpGame->nHoleNum[nHole], nTee);
}

s32 fn_800D2C68(int nTee) {
    return fn_800D2B80(Game_GetCourse(), fn_80015464(), nTee);
}

// The current hole's wind: its direction and its speed.
int fn_800D2CB0_HoleWindDir(void) {
    return lbl_801FA2F4[Game_GetCourse()].aHoles[fn_80015464()].nWindDir;
}

f32 fn_800D2CF8_HoleWindSpeed(void) {
    return lbl_801FA2F4[Game_GetCourse()].aHoles[fn_80015464()].fWindSpeed;
}

// fn_800D2C30 added up over the round's 18 holes, the front nine and the back nine.
s32 fn_800D2D40(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 0; i < 18; i++) {
        nSum += fn_800D2C30(i, nTee);
    }
    return nSum;
}

s32 fn_800D2DA0(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 0; i < 9; i++) {
        nSum += fn_800D2C30(i, nTee);
    }
    return nSum;
}

s32 fn_800D2E00(int nTee) {
    s32 nSum = 0;
    int i;
    for (i = 9; i < 18; i++) {
        nSum += fn_800D2C30(i, nTee);
    }
    return nSum;
}

// The par of the round's front nine and back nine.
s32 fn_800D2E60(void) {
    s32 nPar = 0;
    int i;
    for (i = 0; i < 9; i++) {
        nPar += fn_800D2AD8(i);
    }
    return nPar;
}

s32 fn_800D2EB0(void) {
    s32 nPar = 0;
    int i;
    for (i = 9; i < 18; i++) {
        nPar += fn_800D2AD8(i);
    }
    return nPar;
}

// A course's par from a tee set: 72 for course 23; for 22, the built round's 18 holes added up.
s32 fn_800D2F00(int nCourse, int nTeeSet) {
    int nHoleCourse;
    int i;
    s32 nPar;
    if (nCourse == 23) return 72;
    if (nCourse == 22) {
        nPar = 0;
        for (i = 0; i < 18; i++) {
            nHoleCourse = fn_800D3118(22, i);
            nPar += fn_800D2ABC(nHoleCourse, fn_800D315C(22, i) - 1);
        }
        return nPar;
    }
    return lbl_801FA2F4[nCourse].aTeeSets[nTeeSet].nPar;
}

// The par of the round's 18 holes.
s32 fn_800D2FB4(s32 nTeeSet) {
    s32 nPar = 0;
    int i;
    for (i = 0; i < 18; i++) {
        nPar += fn_800D2AD8(i);
    }
    return nPar;
}

u8 fn_800D3004(void) {
    return lbl_801FA2F4[Game_GetCourse()].aHoles[fn_80015464()].b34;
}

u8 fn_800D304C(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].b35;
}

u8 fn_800D3080(int nHole) {
    return lbl_801FA2F4[gpGame->nHoleCourse[nHole]].aHoles[gpGame->nHoleNum[nHole]].b37;
}

// Which row of the 'CMPS' table a built round (courses 22 and 24..29) uses.
int fn_800D30B4(int nRound) {
    switch (nRound) {
    case 22:
        return 0;
    case 24:
        return 1;
    case 25:
        return 2;
    case 26:
        return 3;
    case 27:
        return 4;
    case 28:
        return 5;
    case 29:
        return 6;
    }
    return 0;
}

int fn_800D3118(int nRound, int nHole) {
    return lbl_801FA1F8[fn_800D30B4(nRound)].aHoles[nHole].nCourse;
}

int fn_800D315C(int nRound, int nHole) {
    int nRow = fn_800D30B4(nRound);
    return lbl_801FA1F8[nRow].aHoles[nHole].nHole;
}

int fn_800D31A4(int nPar) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (nPar == fn_800D2AD8(i)) {
            nCount++;
        }
    }
    return nCount;
}

// The number of the 18 holes that are a par 4 or 5.
int fn_800D3208(void) {
    return fn_800D31A4(4) + fn_800D31A4(5);
}
