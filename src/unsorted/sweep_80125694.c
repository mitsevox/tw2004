// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281988;

u8 fn_80125694(void);
void fn_801256A0(s32 v);
s32 fn_801256AC(void);
u8 fn_80125694(void) {
    return *(u8*)(lbl_80281988 + 0xC);
}

void fn_801256A0(s32 v) {
    *(s32*)(lbl_80281988 + 0x14) = v;
}

s32 fn_801256AC(void) {
    return *(s32*)(lbl_80281988 + 0x14);
}
