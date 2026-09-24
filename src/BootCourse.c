// BootCourse.c (our name): sets up a round on the next of four fixed course/hole/golfer set-ups
// (fn_80099ED8, called from GoEntry.c and uiProcessInterface.c; fn_8009A1F4 puts the session
// back), and a 120-second timer that runs while fn_8008AC40 is false and raises a flag when it runs
// out (fn_8009A180; GoEntry.c tests the flag with fn_8009A1EC). Its .bss starts on its own 8-byte
// boundary (0x801DB8A0, shared by fn_80099ED8 and fn_8009A1F4).

#include "golfer.h"
#include "game.h"
#include "engine.h"

u8 fn_8008AC40(void);

// Per start (lbl_80281F90 cycles 0..3): the course, the hole, and the two players' golfers.
const s32 lbl_80183A98[4] = {5, 17, 19, 15};
const s32 lbl_80183AA8[4] = {17, 17, 17, 17};
const s32 lbl_80183AB8[4] = {0, 12, 9, 1};
const s32 lbl_80183AC8[4] = {4, 15, 27, 7};

// Uninitialised data, defined last-first (CodeWarrior lays it out in reverse).
Session lbl_801DB8A0;           // the session as it was before fn_80099ED8 changed it
f32 lbl_80281F98;               // seconds counted
u8  lbl_80281F94;               // the timer ran out
s32 lbl_80281F90;               // which of the four set-ups the next start uses

// Saves the session and sets up a two-player game on the next of four course/hole/golfer set-ups,
// moving on through the courses until one is on the disc. With uFlags bit 14 it is a one-player
// game on a fixed course and hole per set-up instead, with a random pin.
void fn_80099ED8(void) {
    int n;
    Mem_cpy(&lbl_801DB8A0, &gSession, sizeof(Session));
    gSession.a8[0] = 1;
    gSession.nSplitScreen = 0;
    gSession.b12 = 0;
    gSession.bReplay = 0;
    gSession.nPaused = 0;
    gSession.nNumPlayers = 2;
    fn_800E0B38(0);
    gSession.nController[0] = 9;
    gSession.nGolfer[0] = lbl_80183AB8[lbl_80281F90];
    gSession.nTeeSet[0] = 2;
    gSession.aProfile[0].n0 = 0;
    gSession.nController[1] = 9;
    gSession.nGolfer[1] = lbl_80183AC8[lbl_80281F90];
    gSession.nTeeSet[1] = 2;
    gSession.aProfile[0].n0 = 0;
    gSession.options.nC = 0;
    fn_800E14E0(lbl_80183A98[lbl_80281F90]);
    fn_800E1260(0);
    fn_800E1404(lbl_80183AA8[lbl_80281F90]);
    n = lbl_80281F90;
    while (!fn_80110180()) {
        fn_800E14E0(lbl_80183A98[n++]);
        if (n == 4) {
            n = 0;
        }
    }
    gpGame->nPinSet[Game_CurHoleIndex()] = 0;
    if (gSession.uFlags & 0x4000) {
        gSession.nNumPlayers = 1;
        gSession.nController[0] = 9;
        gSession.nTeeSet[0] = 2;
        gSession.aProfile[0].n0 = 0;
        switch (lbl_80281F90) {
        case 0:
        case 1:
            gSession.nGolfer[0] = 1;
            break;
        case 2:
            gSession.nGolfer[0] = 7;
            break;
        case 3:
            gSession.nGolfer[0] = 29;
            break;
        default:
            gSession.nGolfer[0] = 1;
            break;
        }
        fn_800E1260(0);
        switch (lbl_80281F90) {
        case 0:
            fn_800E14E0(20);
            fn_800E1404(0);
            break;
        case 1:
            fn_800E14E0(19);
            fn_800E1404(17);
            break;
        case 2:
            fn_800E14E0(17);
            fn_800E1404(1);
            break;
        case 3:
            fn_800E14E0(18);
            fn_800E1404(17);
            break;
        }
        gpGame->nPinSet[Game_CurHoleIndex()] = Rand_Next(0) & 3;
    }
    lbl_80281F90 = lbl_80281F90 + 1;
    lbl_80281F90 = lbl_80281F90 & 3;
}

void fn_8009A16C(void) {
    lbl_80281F94 = 0;
    lbl_80281F98 = 0.0f;
}

u8 fn_8009A180(void) {
    if (fn_8008AC40()) {
        return 0;
    }
    lbl_80281F98 += gSession.fFrameTime;
    if (lbl_80281F98 >= 120.0f) {
        lbl_80281F94 = 1;
        return 1;
    }
    return 0;
}

u8 fn_8009A1EC(void) {
    return lbl_80281F94;
}

// Put the session back as fn_80099ED8 found it, all but n28.
void fn_8009A1F4(void) {
    s32 n28;
    if (gSession.a8[0] != 0) {
        n28 = gSession.n28;
        Mem_cpy(&gSession, &lbl_801DB8A0, sizeof(Session));
        gSession.n28 = n28;
    }
}
