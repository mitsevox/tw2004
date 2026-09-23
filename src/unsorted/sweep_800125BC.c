// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(void*);
void fn_800127D8(s32 arg0, s32 arg1, s32);

void fn_800125BC(void* arg0);
void fn_800125BC(void* arg0) {
    s32 var_r5;
    s32 temp_r3;

    var_r5 = 0;
    temp_r3 = (*(s32*)((u8*)(arg0) + 0x20));
    if (temp_r3 & 0x80) {
        var_r5 = 1;
    }
    fn_800127D8(temp_r3 & 0xFFFFFF7F, (*(s32*)((u8*)(arg0) + 0)), var_r5);
    fn_80009E70(arg0);
}