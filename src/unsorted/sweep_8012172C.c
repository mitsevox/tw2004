// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801946B0[];
extern u8 lbl_80260CB8[];
s32 fn_80102A44();

void fn_8012172C(u8* p0, u8* p1);
void fn_8012172C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80102A44(*(s32*)(lbl_801946B0 + (*(s32*)p0 << 2)));
    *(s32*)p1 = t0;
}
