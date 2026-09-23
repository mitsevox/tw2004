// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80223C70[];
s32 fn_80117600();
void fn_8011ABF8(s32 arg0);

s32 fn_801197A4(void);
s32 fn_801197CC(s32 p0, s32 p1);
s32 fn_801197A4(void) {
    s32 t0;
    t0 = fn_80117600();
    return (*(s32*)(((u8*)t0) + 0x14) & 0xFF);
}

s32 fn_801197CC(s32 p0, s32 p1) {
    fn_8011ABF8(p0);
    return *(s32*)(lbl_80223C70 + (p1 << 2));
}
