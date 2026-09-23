// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802811F0;

f32 fn_8006C630(void);
f32 fn_8006C630(void) {
    return *(f32*)(((u8*)lbl_802811F0) + 0x18);
}
