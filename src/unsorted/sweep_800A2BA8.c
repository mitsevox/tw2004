// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802813F8;

void fn_800A2BA8(void);
void fn_800A2BA8(void) {
    *(s32*)(((u8*)lbl_802813F8) + 0x4) = (1 - *(s32*)(((u8*)lbl_802813F8) + 0x4));
}
