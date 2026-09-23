// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80005AE8(s32*, s32, s32);

s32* fn_8000B078(void* arg0);
s32* fn_8000B078(void* arg0) {
    s32* temp_r31;

    temp_r31 = (*(s32**)((u8*)(arg0) + 0xC));
    if (temp_r31 != NULL) {
        (*(s32**)((u8*)(arg0) + 0xC)) = (s32* ) *temp_r31;
        (*(u16*)((u8*)(arg0) + 0xA)) = (u16) ((*(u16*)((u8*)(arg0) + 0xA)) - 1);
        fn_80005AE8(temp_r31, 0xBB, (*(s32*)((u8*)(arg0) + 0)));
    }
    return temp_r31;
}