// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80194748[];
void fn_80009B34();
s32 fn_8000A0B4();

void fn_801220D4(s32 p0, s32 p1, s32 p2);
void fn_801220D4(s32 p0, s32 p1, s32 p2) {
    s32 t0;
    t0 = fn_8000A0B4();
    fn_80009B34(p1, t0, p2, lbl_80194748, 42);
}
