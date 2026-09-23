// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802824B8;

void fn_8010F7E4(s32 p0);
void fn_8010F7E4(s32 p0) {
    *(s32*)(((u8*)lbl_802824B8) + 0x4) = p0;
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}
