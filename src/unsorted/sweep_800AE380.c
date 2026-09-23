// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_8000B70C(s32, s32);
u32 fn_80048808(s32);
extern u32 lbl_802820D0;

void fn_800AE380(void);
void fn_800AE380(void) {
    if ((u32) lbl_802820D0 == 0U) {
        lbl_802820D0 = fn_80048808((*(s32*)((u8*)(fn_8000B70C(0x54454F20, 0x2713)) + 4)));
    }
}