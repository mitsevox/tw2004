// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_800354BC(s32);
void* fn_800354C4();
s32 fn_800354D0(s32, s32);
s32 fn_800354E4(s32, s32);
s32 fn_800354F4(s32);

void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0);
void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0) {
    s32 temp_r31;

    (*(f32*)((u8*)(arg3) + 8)) = farg0;
    (*(f32*)((u8*)(arg3) + 0xC)) = (f32) (*(f32*)((u8*)(fn_800354C4()) + 0xC));
    (*(s32*)((u8*)(arg3) + 0x10)) = arg1;
    (*(s32*)((u8*)(arg3) + 0x14)) = arg2;
    temp_r31 = fn_800354F4(arg0);
    (*(s32*)((u8*)(arg3) + 0)) = fn_800354E4(arg0, 0);
    (*(s32*)((u8*)(arg3) + 0x18)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 3);
    (*(s32*)((u8*)(arg3) + 0x1C)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 2);
    (*(s32*)((u8*)(arg3) + 0x20)) = fn_800354D0(arg0, 1);
    if (temp_r31 >= 2) {
        (*(s32*)((u8*)(arg3) + 4)) = fn_800354BC((*(s32*)((u8*)(arg3) + 0)));
        return;
    }
    (*(s32*)((u8*)(arg3) + 4)) = 0;
}