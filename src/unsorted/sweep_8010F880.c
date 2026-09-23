// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802824B8;

void fn_8010F880(void);
void fn_8010F880(void) {
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}
