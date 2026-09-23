// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802813B8;

void fn_80035584(s32 v);
void fn_80035584(s32 v) {
    *(s32*)(lbl_802813B8 + 0x1930) = v;
}
