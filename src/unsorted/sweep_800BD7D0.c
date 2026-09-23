// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80202898[];

void fn_800BD7D0(s32 p0);
void fn_800BD7D0(s32 p0) {
    *(u8*)(lbl_80202898 + 0x4E) = 1;
    *(u8*)(lbl_80202898 + 0x4F) = p0;
}
