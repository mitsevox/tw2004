// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8020CF90[];

s32 fn_800F0EF4(s32 p0);
s32 fn_800F0F10(s32 p0);
s32 fn_800F0F30(s32 p0);
s32 fn_800F0EF4(s32 p0) {
    return (*(s32*)(lbl_8020CF90 + 0x4DA0) + *(s32*)(lbl_8020CF90 + (p0 * 48)));
}

s32 fn_800F0F10(s32 p0) {
    return (*(s32*)(lbl_8020CF90 + 0x4DA0) + *(s32*)((lbl_8020CF90 + (p0 * 48)) + 0x4));
}

s32 fn_800F0F30(s32 p0) {
    return *(s32*)((lbl_8020CF90 + (*(s32*)((lbl_8020CF90 + (p0 * 48)) + 0xC) << 7)) + 0x1604);
}
