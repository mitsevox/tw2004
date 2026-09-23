// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80260E18[];
extern s32 lbl_80281984;
extern s32 lbl_8028251C;
s32 fn_80077ACC();

void fn_801240A8(void);
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
