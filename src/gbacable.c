// gbacable.c (EA's name, from its asserts): not yet decompiled.
#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_8028255C;
extern s32 lbl_80282560;
s32 DVDGetCurrentDiskID();
s32 OSGetTick();
void fn_801229F8();
void fn_80175FB8();
void fn_80123FF8(void);
s32 OSGetResetButtonState();
s32 OSResetSystem(s32, s32, s32);
s32 fn_80123CBC(s32, s32);
s32 fn_80123E34();
extern s32 lbl_80282540;
void fn_8012402C(void);
extern s32 lbl_80281980;
extern s32 lbl_80282518;
void fn_8012408C(s32 v);
s32 fn_80124094(void);
void fn_8012409C(void);
extern u8 lbl_80260E18[];
extern s32 lbl_80281984;
extern s32 lbl_8028251C;
s32 fn_80077ACC();
void fn_801240A8(void);
extern s32 lbl_80282520;
extern s32 lbl_80282524;
extern s32 lbl_80282528;
extern s32 lbl_8028252C;
extern s32 lbl_80282530;
extern s32 lbl_80282534;
extern s32 lbl_80282538;
extern s32 lbl_8028253C;
void fn_801241AC(s32 v);
s32 fn_801241B4(void);
void fn_801241BC(s32 v);
s32 fn_801241C4(void);
s32 fn_801241CC(void);
void fn_801241D4(s32 v);
void fn_801241DC(s32 v);
s32 fn_801241E4(void);
void fn_801241EC(s32 v);
s32 fn_801241F4(void);
void fn_801241FC(s32 v);
s32 fn_80124204(void);
void fn_8012420C(s32 v);
s32 fn_80124214(void);
void fn_8012421C(s32 v);
s32 fn_80124224(void);
void fn_8012422C(void);
extern s32 lbl_8028254C;
extern s32 lbl_80282550;
extern s32 lbl_80282554;
extern s32 lbl_80282558;
void fn_80124238(s32 arg0, s32 arg1);
s32 fn_80124280(s32 arg0);

void fn_80123FF8(void) {
    s32 t0;
    s32 t1;
    t0 = DVDGetCurrentDiskID();
    lbl_8028255C = t0;
    t1 = OSGetTick();
    lbl_80282560 = t1;
    fn_801229F8();
    fn_80175FB8();
}

void fn_8012402C(void) {
    fn_80123E34();
    fn_80123CBC(0, 0);
    if (OSGetResetButtonState() != 0) {
        lbl_80282540 = 1;
        return;
    }
    if ((s32) lbl_80282540 != 0) {
        OSResetSystem(0, 1, 0);
    }
}

void fn_8012408C(s32 v) {
    lbl_80281980 = v;
}

s32 fn_80124094(void) {
    return lbl_80281980;
}

void fn_8012409C(void) {
    lbl_80282518 = 1;
}

void fn_801240A8(void) {
    s32 t0;
    t0 = fn_80077ACC();
    *(u8*)(((u8*)t0) + 0x50) = 1;
    *(u8*)(((u8*)t0) + 0x51) = 1;
    *(u8*)(((u8*)t0) + 0x52) = 1;
    *(u8*)(((u8*)t0) + 0x53) = 1;
    *(u8*)(((u8*)t0) + 0x54) = 1;
    *(u8*)(((u8*)t0) + 0x55) = 1;
    *(u8*)(((u8*)t0) + 0x56) = 1;
    *(u8*)(((u8*)t0) + 0x57) = 1;
    *(u8*)(((u8*)t0) + 0x58) = 1;
    *(u8*)(((u8*)t0) + 0x59) = 1;
    *(u8*)(((u8*)t0) + 0x5A) = 1;
    *(u8*)(((u8*)t0) + 0x5B) = 1;
    *(u8*)(((u8*)t0) + 0x5C) = 1;
    *(u8*)(((u8*)t0) + 0x5D) = 1;
    *(u8*)(((u8*)t0) + 0x5E) = 1;
    *(u8*)(((u8*)t0) + 0x5F) = 1;
    *(u8*)(((u8*)t0) + 0x60) = 1;
    *(u8*)(((u8*)t0) + 0x61) = 1;
    *(u8*)(((u8*)t0) + 0x62) = 1;
    lbl_8028251C = 1;
}

void fn_801241AC(s32 v) {
    lbl_80282520 = v;
}

s32 fn_801241B4(void) {
    return lbl_80282520;
}

void fn_801241BC(s32 v) {
    lbl_80282524 = v;
}

s32 fn_801241C4(void) {
    return lbl_80282524;
}

s32 fn_801241CC(void) {
    return lbl_80282528;
}

void fn_801241D4(s32 v) {
    lbl_80282528 = v;
}

void fn_801241DC(s32 v) {
    lbl_8028252C = v;
}

s32 fn_801241E4(void) {
    return lbl_8028252C;
}

void fn_801241EC(s32 v) {
    lbl_80282530 = v;
}

s32 fn_801241F4(void) {
    return lbl_80282530;
}

void fn_801241FC(s32 v) {
    lbl_80282538 = v;
}

s32 fn_80124204(void) {
    return lbl_80282538;
}

void fn_8012420C(s32 v) {
    lbl_8028253C = v;
}

s32 fn_80124214(void) {
    return lbl_8028253C;
}

void fn_8012421C(s32 v) {
    lbl_80282534 = v;
}

s32 fn_80124224(void) {
    return lbl_80282534;
}

void fn_8012422C(void) {
    lbl_80281984 = -1;
}

void fn_80124238(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        lbl_80282558 = arg1;
        return;
    case 1:
        lbl_80282554 = arg1;
        return;
    case 2:
        lbl_80282550 = arg1;
        return;
    case 3:
        lbl_8028254C = arg1;
        return;
    }
}

s32 fn_80124280(s32 arg0) {
    switch (arg0) {
    case 0:
        return lbl_80282558;
    case 1:
        return lbl_80282554;
    case 2:
        return lbl_80282550;
    case 3:
        return lbl_8028254C;
    default:
        return 0;
    }
}

// ---- end of sweep code ----
