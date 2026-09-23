// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802814B8;

void fn_800B4BB8(void);
void fn_800B4BD8(void);
void fn_800B4BB8(void) {
    *(s32*)((u8*)lbl_802814B8) = 0;
    *(s32*)(((u8*)lbl_802814B8) + 0x4) = (1 - *(s32*)(((u8*)lbl_802814B8) + 0x4));
}

void fn_800B4BD8(void) {
    *(s32*)((u8*)lbl_802814B8) = ((((*(s32*)((u8*)lbl_802814B8) + 1) & 0x1) ^ ((u32)(*(s32*)((u8*)lbl_802814B8) + 1) >> 31)) - ((u32)(*(s32*)((u8*)lbl_802814B8) + 1) >> 31));
}
