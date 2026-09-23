// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802814C8;
void fn_800B5D34();
void fn_800B5E88();

void fn_800B6564(s32 p0);
void fn_800B6594(s32 p0);
void fn_800B6564(s32 p0) {
    fn_800B5D34(*(s32*)((u8*)lbl_802814C8), p0, 32, lbl_802814C8);
}

void fn_800B6594(s32 p0) {
    fn_800B5E88(*(s32*)((u8*)lbl_802814C8), p0, lbl_802814C8);
}
