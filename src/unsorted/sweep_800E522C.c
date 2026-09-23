// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80202B88[];

void fn_800E522C(s32 p0);
void fn_800E522C(s32 p0) {
    *(u8*)(lbl_80202B88 + p0) = 1;
}
