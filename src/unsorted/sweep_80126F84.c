// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80195498[];

void fn_80126F84(s32 p0);
void fn_80126F84(s32 p0) {
    *(s32*)(lbl_80195498 + 0x4) = p0;
}
