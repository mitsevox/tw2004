// GameMode23.c (our name): game mode 23, a run of 31 lessons (lbl_80205F3C, 0x64 bytes each, loaded
// from a stream object) with the player's progress kept in the save profile (+0xB634..).

#include "game_types.h"

void fn_800EDE78(void);
extern u8 lbl_80205F3C[];
void UStream_UnregisterHandler();
void fn_8000E790();
void fn_800EDEE8(void);
void fn_800EDF34(s32 p0);
void fn_800EDF60(s32 p0);
void fn_800EDF90(s32 p0);
extern s32 gpGame;
extern s32 gpSaveData;
void fn_800EE064(void);
extern u8 lbl_8028233C;
u8 fn_800EE470(void);
extern u8 lbl_80205F30[];
s32 fn_800EE8B0(void);
u8* fn_800EE8B8(void);
void fn_80119934();
void fn_800EF294(void);
s32 fn_800EF834(void);
s32 fn_800EFB88(void);
s32 fn_800EFBAC(void);
s32 fn_800EFDFC(s32 p0);
u8* fn_800EFA70(s32 i);
s32 fn_800EFE3C(s32 i);
u8* fn_800EFE60(s32 p0);
s32 fn_801197CC();
void fn_800F009C(void);
u8 fn_8011908C(s32, s32);
s32 fn_8011937C(s32, s32, u8);
void fn_800F018C(void);

// One lesson (0x64 bytes).
typedef struct Lesson {
    s32  n0;
    s32  nPlan;                 // 0x04  1-based entry in the plan table (0 = one step)
    u8   unk8[8];
    s32  n10;                   // 0x10
    char szName[0x10];          // 0x14
    s32  n24;                   // 0x24
    s16  aTimes[10][2];         // 0x28  per step, in seconds
    u16  a50[10];               // 0x50  per step
} Lesson;
#define LESSONS ((Lesson*)lbl_80205F3C)

typedef struct Triple {
    s32 a, b, c;
} Triple;
#define TRIPLES ((Triple*)(lbl_80205F3C + 0x6FC8))

// The lesson part of a save profile (0x10600 bytes).
typedef struct LessonSave {
    char szName[0x10];          // 0x00
    s32  n10;                   // 0x10
    u8   unk14[0x1C - 0x14];
    s32  n1C;                   // 0x1C  1 = done
    u8   b20;                   // 0x20
    u8   unk21[3];
} LessonSave;
typedef struct Profile {
    u8         unk0[0xB634];
    s32        nB634;           // 0xB634
    s32        nLesson;         // 0xB638
    s32        nStep;           // 0xB63C
    LessonSave aLesson[31];     // 0xB640
    u8         unkBA9C[0x10600 - 0xBA9C];
} Profile;
#define PROFILES ((Profile*)gpSaveData)

extern u8  lbl_80281670[];
extern u8  gSession[];
extern s32 lbl_80282338;
s32  fn_801190D8(s32 a);
void fn_800E4364(u32 nQueue, s32 a, s32 b, s32 c);
s32  fn_800EFBD0(s32 i);
u8   fn_800EF83C(u16* pId, s32* pOut);
char* strcpy(char* pDst, const char* pSrc);
s32  fn_800F02A8(void);
s32  fn_800EFA9C(s32 i);

void fn_800EDE78(void) {
}

void fn_800EDEE8(void) {
    UStream_UnregisterHandler(1346847075);
    UStream_UnregisterHandler(1346847092);
    UStream_UnregisterHandler(1346847088);
    UStream_UnregisterHandler(1346847086);
}

void fn_800EDF34(s32 p0) {
    fn_8000E790(p0, 3100, lbl_80205F3C);
}

void fn_800EDF60(s32 p0) {
    fn_8000E790(p0, 2604, (lbl_80205F3C + 0xC1C));
}

void fn_800EDF90(s32 p0) {
    fn_8000E790(p0, 132, (lbl_80205F3C + 0x6FC8));
}

void fn_800EE064(void) {
    *(s32*)(((u8*)gpGame) + 0xE0) = *(s32*)((lbl_80205F3C + ((*(s32*)((lbl_80205F3C + (*(s32*)(((u8*)(gpSaveData + 0x10000)) - 0x49C8) * 100)) + 0x4) - 1) * 84)) + 0xC1C);
}

u8 fn_800EE470(void) {
    return lbl_8028233C;
}

s32 fn_800EE8B0(void) {
    return 3;
}

u8* fn_800EE8B8(void) {
    return lbl_80205F30;
}

void fn_800EF294(void) {
    fn_80119934(0);
}

s32 fn_800EF834(void) {
    return 31;
}

s32 fn_800EFBAC(void) {
    s32 t0;
    t0 = fn_800EFB88();
    return (t0 + 2004);
}

s32 fn_800EFDFC(s32 p0) {
    return (*(s32*)(lbl_80205F3C + 0x704C) + *(s32*)(lbl_80205F3C + (p0 * 100)));
}

s32 fn_800EFE3C(s32 i) {
    return *(s32*)(fn_800EFA70(i) + 0x10);
}

u8* fn_800EFE60(s32 p0) {
    return ((lbl_80205F3C + (p0 * 100)) + 0x14);
}

void fn_800F009C(void) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, 0);
    t1 = fn_8011908C(0, t0);
    fn_8011937C(0, t0, (((u32)__cntlzw((t1 & 0xFF)) >> 5) & 0xFF));
}

void fn_800F018C(void) {
    fn_8011937C(0, 0, (u8) (fn_8011908C(0, 0) == 0));
}

// The mode ends: one player back, and the options it changed come back.
void fn_800EE02C(void) {
    ((s32*)gpGame)[0xC / 4] = 1;
    ((s32*)gpGame)[0x10 / 4] = 1;
    *(s32*)(gSession + 0xE84) = *(s32*)lbl_80281670;
    *(s32*)(gSession + 0xE88) = lbl_80282338;
    lbl_8028233C = 0;
}

// The lesson and step profile 0 is on.
s32 fn_800EF908(s32* pStep) {
    *pStep = PROFILES[0].nStep;
    return PROFILES[0].nLesson;
}

void fn_800EF940(void) {
    fn_800EFBD0(PROFILES[0].nLesson + 1);
}

// The last lesson there is.
s32 fn_800EF984(void) {
    s32 nLast = 0;
    s32 i = fn_800EFBD0(1);
    while (i != -1) {
        nLast = i;
        i = fn_800EFBD0(i + 1);
    }
    return nLast;
}

void fn_800EF094(s32 n) {
    *(s32*)(lbl_80205F30 + 0) = 1;
    *(s32*)(lbl_80205F30 + 4) = fn_801190D8(0);
    *(s32*)(lbl_80205F30 + 8) = n;
}

// A progress bar out of 10: lessons done x 10 / 31, at most 9.
u16 fn_800EF0E0(void) {
    u16 n = 9;
    u16 t = fn_800F02A8() * 10 / 31;
    if (t <= 9) {
        n = t;
    }
    return n;
}

// Lesson i (0..30), or none.
u8* fn_800EFA70(s32 i) {
    if (i != -1 && i < 31) {
        return (u8*)&LESSONS[i];
    }
    return 0;
}

// Lesson i's step count (from its entry in the second table; 1 without one).
s32 fn_800EFA9C(s32 i) {
    if (LESSONS[i].nPlan) {
        return *(s32*)(lbl_80205F3C + LESSONS[i].nPlan * 0x54 + 0xBC8);
    }
    return 1;
}

s32 fn_800EFB88(void) {
    return PROFILES[0].nB634;
}

u8* fn_800EFC80(void) {
    u16 nId;
    s32 n;
    if (fn_800EF83C(&nId, &n)) {
        return fn_800EFA70(nId);
    }
    return 0;
}

// Two times of a lesson's step k, in milliseconds.
s32 fn_800EFCC0(s32 i, s32 k) {
    return ((Lesson*)fn_800EFA70(i))->aTimes[k][0] * 1000;
}

s32 fn_800EFCFC(s32 i, s32 k) {
    return ((Lesson*)fn_800EFA70(i))->aTimes[k][1] * 1000;
}

u16 fn_800EFD38(s32 i) {
    u8* p = fn_800EFA70(i);
    if (!p) {
        return 0xFFFF;
    }
    return ((Lesson*)p)->a50[fn_800EFB88()];
}

s32 fn_800EFE18(void) {
    return PROFILES[0].nLesson;
}

s32 fn_800EFE78(s32 i) {
    return LESSONS[i].n24;
}

void fn_800F0258(s32 i, char* pDst) {
    strcpy(pDst, PROFILES->aLesson[i].szName);
}

s32 fn_800F0290(s32 i) {
    return PROFILES->aLesson[i].n10;
}

// How many lessons are done (flag 1 in the profile's list at +0xC8, 8 bytes each).
s32 fn_800F02A8(void) {
    s32 n = 0;
    s32 i;
    for (i = 0; i < 31; i++) {
        if (((u8*)gpSaveData)[0xC8 + i * 8] == 1) {
            n++;
        }
    }
    return n;
}

s32 fn_800F02D4(s32 i) {
    return TRIPLES[i].a;
}

s32 fn_800F02EC(s32 i) {
    return TRIPLES[i].b;
}

s32 fn_800F0304(s32 i) {
    return TRIPLES[i].c;
}

s32 fn_800F0428(s32 n) {
    return PROFILES[n].nLesson;
}
