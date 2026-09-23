// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(s32);
s32 fn_800975FC(void*);

void fn_80097624(void* arg0);
void fn_80097624(void* arg0) {
    if ((u16) (*(u16*)((u8*)(arg0) + 0xA)) != 0) {
        fn_80009E70((*(s32*)((u8*)(arg0) + 0x10)));
        fn_80009E70((*(s32*)((u8*)(arg0) + 0x14)));
        fn_80009E70((*(s32*)((u8*)(arg0) + 0x18)));
        (*(s32*)((u8*)(arg0) + 0x10)) = 0;
        (*(s32*)((u8*)(arg0) + 0x14)) = 0;
        (*(s32*)((u8*)(arg0) + 0x18)) = 0;
        fn_800975FC(arg0);
    }
}