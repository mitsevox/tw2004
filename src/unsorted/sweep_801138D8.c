// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80223BB0[];

void fn_801138D8(s32 p0);
void fn_801138D8(s32 p0) {
    *(s32*)(lbl_80223BB0 + 0xC) = p0;
}
