// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001E8A4(s32, s32);
s32 fn_8001E938(s32, s32);

void fn_80017864(void* arg0, s32 arg1);
void fn_80017864(void* arg0, s32 arg1) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x3C)) != 0U) {
        fn_8001E8A4(arg1 + 0x20, 0x80);
        fn_8001E8A4(arg1 + 0x30, 0x80);
        fn_8001E938(arg1, 0x80);
        fn_8001E938(arg1 + 0x10, 0x80);
    }
}