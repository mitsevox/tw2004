// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DD8;
void fn_80010028();

void fn_80010544(s32 p0);
void fn_80010544(s32 p0) {
    fn_80010028(*(s32*)(((u8*)*(s32*)((u8*)lbl_80280DD8)) + (p0 * 12)), lbl_80280DD8);
    *(s32*)(((u8*)*(s32*)((u8*)lbl_80280DD8)) + (p0 * 12)) = 0;
}
