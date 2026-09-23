// Small functions found by the sweep (sweep.py). Original file: GoPostFx.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

void fn_8002A024();

void fn_80038A2C(s32 p0, u8* p1);
void fn_80038A2C(s32 p0, u8* p1) {
    fn_8002A024(1, *(s32*)(p1 + 0x14), *(f32*)((u8*)*(s32*)(p1 + 0x14)), *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x4), (*(f32*)((u8*)*(s32*)(p1 + 0x14)) + *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x8)), (*(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x4) + *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0xC)));
}
