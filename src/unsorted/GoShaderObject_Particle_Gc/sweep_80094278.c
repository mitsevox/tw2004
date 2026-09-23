// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802813A8;

void fn_80094278(void);
void fn_80094278(void) {
    *(u8*)(((u8*)lbl_802813A8) + 0x10) = (1 - *(u8*)(((u8*)lbl_802813A8) + 0x10));
}
