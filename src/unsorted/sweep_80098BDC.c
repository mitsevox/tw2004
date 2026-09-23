// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(u8*);
s32 fn_800360A0(u8*);
void fn_80098BDC(u8* arg0);

void fn_80098BDC(u8* arg0);
void fn_80098BDC(u8* arg0) {
    (*(s8*)((u8*)(arg0) + 0x5C)) = 0;
    if ((s32) (*(s32*)((u8*)(arg0) + 0xB4)) < 0) {
        fn_800360A0(arg0 + 0x180);
        fn_80009E70(arg0);
    }
}