// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpGame;
extern u8 lbl_801FA2F4[];

s32 fn_800D2ABC(s32 p0, s32 p1);
s32 fn_800D2AD8(s32 p0);
s32 fn_800D2ABC(s32 p0, s32 p1) {
    return *(s32*)((lbl_801FA2F4 + (p0 * 1072)) + (p1 * 56));
}

s32 fn_800D2AD8(s32 p0) {
    return *(s32*)((lbl_801FA2F4 + (*(s32*)(((u8*)(gpGame + (p0 << 2))) + 0x18) * 1072)) + (*(s32*)(((u8*)(gpGame + (p0 << 2))) + 0x68) * 56));
}
