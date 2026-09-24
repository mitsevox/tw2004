// hwsRender_Gc.c (EA's name, from its asserts): the GameCube renderer's setup: a screen buffer
// sized for one or two views, and the state (lbl_80223BB0) the skin drawing reads.

#include "engine.h"
#include "charstate.h"
#include "golfer.h"
#include "game.h"

void fn_80036460(int n);                // Skin.c
void PostFx_CopyScreenToBuffer(void);   // gomainloop.c
void fn_8011EB80(void);
void fn_80112B34(void);                 // hwsOverride_Gc.c

void* lbl_802824E0;                     // the screen buffer (fn_80112C64)
u32 lbl_802824EC;                       // and its size

// Whether the extra pass runs: one view, at most three players (two on course 14's hole 11).
u8 fn_80112B80(void) {
    int nHole = fn_80015464();

    if (Game_GetCourse() == 14 && nHole == 11 && gSession.nNumPlayers > 2) {
        return 0;
    }
    if (gSession.nNumPlayers > 3) {
        return 0;
    }
    return gSession.nSplitScreen == 0;
}

// The same limit for course 14's hole 11 alone: at most three players.
int fn_80112C04(void) {
    int nHole = fn_80015464();

    if (Game_GetCourse() == 14 && nHole == 11 && gSession.nNumPlayers > 3) {
        return 0;
    }
    return 1;
}

// Allocate the screen buffer: bigger for split screen.
void fn_80112C64(int bSplit) {
    if (bSplit == 0) {
        lbl_802824E0 = fn_80009B34(0x30C00, 2, 32, "hwsRender_Gc.c", 266);
        lbl_802824EC = 0x30C00;
    } else {
        lbl_802824E0 = fn_80009B34(0x5DC00, 2, 32, "hwsRender_Gc.c", 271);
        lbl_802824EC = 0x5DC00;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80112CEC(void);
s32 fn_80036464();
s32 fn_80037F80();
s32 fn_8011EBF8();
void fn_80112DA0(void);

void fn_80112CEC(void) {
    if (lbl_802824E0 != NULL) {
        fn_80009E70(lbl_802824E0);
        lbl_802824E0 = NULL;
    }
}

// ---- end of sweep code ----

void fn_80112D20(void) {
    if (gSession.nSplitScreen) {
        fn_80112C64(1);
    } else {
        fn_80112C64(0);
    }
    if (fn_80112B80()) {
        fn_8011EB80();
    }
    PostFx_CopyScreenToBuffer();
    if (gSession.nSplitScreen) {
        fn_80036460(3600);
        return;
    }
    fn_80036460(1800);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80112DA0(void) {
    fn_80112CEC();
    if (fn_80112B80() != 0) {
        fn_8011EBF8();
    }
    fn_80037F80();
    fn_80036464();
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_801136C4();
void fn_8011368C(u8* p0, s32 p1, s32 p2, s32 p3);
u32* fn_80113764(void);
void fn_80113840(void);
void fn_80113844(void);
void fn_8011387C(void);
void fn_8011389C(void);
void fn_801138D8(HwsOverrideTable* pTable);
void fn_801138E8(void);
void fn_801138EC(s32 p0);
void fn_801138F8(s32 p0);
void fn_80113904(s32 p0);
void fn_80113A7C(void);
void fn_80113B14(void);
void fn_80113BAC(void);
void fn_80113E54(u8* p, s32 v);
void fn_80113E5C(void);

void fn_8011368C(u8* p0, s32 p1, s32 p2, s32 p3) {
    fn_801136C4(p0, *(s32*)(p0 + 0x0), p1, p2, p3, p1);
}

u32* fn_80113764(void) {
    return lbl_80223BB0.s10.p48;
}

void fn_80113840(void) {
}

// ---- end of sweep code ----

// Clear the renderer's state.
void fn_80113844(void) {
    lbl_80223BB0.p0 = NULL;
    lbl_80223BB0.n4 = 0;
    lbl_80223BB0.pOverride = NULL;
    lbl_80223BB0.n8 = 0;
    fn_80112B34();
}

// ---- sweep code (not yet cleaned up) ----

void fn_8011387C(void) {
    fn_80113844();
}

void fn_8011389C(void) {
    fn_80113840();
    lbl_80223BB0.s10.n50 = 0;
}

// ---- end of sweep code ----

void fn_801138CC(void* p) {
    lbl_80223BB0.p0 = p;
}

// ---- sweep code (not yet cleaned up) ----

// Make pTable the current mesh overrides.
void fn_801138D8(HwsOverrideTable* pTable) {
    lbl_80223BB0.pOverride = pTable;
}

void fn_801138E8(void) {
}

void fn_801138EC(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_801138F8(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113904(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113A7C(void) {
    fn_80113E5C();
}

void fn_80113B14(void) {
    fn_80113E5C();
}

void fn_80113BAC(void) {
    fn_80113E5C();
}

void fn_80113E54(u8* p, s32 v) {
    *(s32*)(p + 0x0) = v;
}

void fn_80113E5C(void) {
}

// ---- end of sweep code ----
