// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802813A8;
void fn_80009E70();

void fn_80094214(void);
void fn_80094214(void) {
    *(s32*)(((u8*)lbl_802813A8) + 0x14) = 0;
    *(u8*)(((u8*)lbl_802813A8) + 0x10) = 0;
    fn_80009E70(*(s32*)((u8*)lbl_802813A8));
    fn_80009E70(*(s32*)(((u8*)lbl_802813A8) + 0x4));
    fn_80009E70(*(s32*)(((u8*)lbl_802813A8) + 0x8));
    fn_80009E70(*(s32*)(((u8*)lbl_802813A8) + 0xC));
}
