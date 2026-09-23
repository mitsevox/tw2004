// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80282238;

void fn_800CEE88(u8 v);
u8 fn_800CEE90(void);
void fn_800CEE98(void);
void fn_800CEEBC(void);
u8 fn_800CEEC0(u8* p);
void fn_800CEE88(u8 v) {
    lbl_80282238 = v;
}

u8 fn_800CEE90(void) {
    return lbl_80282238;
}

void fn_800CEE98(void) {
    fn_800CEE88(1);
}

void fn_800CEEBC(void) {
}

u8 fn_800CEEC0(u8* p) {
    return *(u8*)(p + 0x4);
}
