// Code8005D2E4.c (our name): own unit, split off Swing.c at 0x8005D2E4. Its .sbss starts on the
// 8-aligned address after StateGolfer.c's padding at 0x80281E01..0x80281E08 and ends padded at
// 0x80281E0A..0x80281E10, where stateFunc.c's .sbss starts.

#include "golfer.h"
#include "game.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
s8            lbl_80281E09;
u8            lbl_80281E08;

void fn_8005D2E4(void) {
    lbl_80281E09 = -1;
    lbl_80281E08 = 0;
}

void fn_8005D2F8(void) {
    if (lbl_80281E09 > -1 && lbl_801883C0[lbl_80281E09].pfnUpdate != NULL) {
        lbl_801883C0[lbl_80281E09].pfnUpdate();
    }
}

void fn_8005D348(void) {
    if (lbl_80281E09 > -1 && lbl_801883C0[lbl_80281E09].pfnExit != NULL) {
        lbl_801883C0[lbl_80281E09].pfnExit();
    }
    lbl_80281E09 = -1;
    lbl_80281E08 = 0;
}

void fn_8005D3A8(s8 nState) {
    void (*pfn)(void);
    if (lbl_80281E09 > -1 && (pfn = lbl_801883C0[lbl_80281E09].pfnExit) != NULL) {
        lbl_80281E08 = 1;
        pfn();
        lbl_80281E08 = 0;
    }
    lbl_80281E09 = nState;
    pfn = lbl_801883C0[lbl_80281E09].pfnEnter;
    if (pfn != NULL) {
        lbl_80281E08 = 1;
        pfn();
        lbl_80281E08 = 0;
    }
}
