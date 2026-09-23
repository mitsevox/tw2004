// Small functions found by the sweep (sweep.py). Original file: GoPostFx.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

extern u8 lbl_801D50C0[];
void Vec_Copy();

void fn_800386F0(s32 p0, s32 p1);
void fn_800386F0(s32 p0, s32 p1) {
    Vec_Copy(((lbl_801D50C0 + (p0 * 20)) + 0x4), p1, (p0 * 20));
}
