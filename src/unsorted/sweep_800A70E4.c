// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ADB4C(s32, s32, s32);
s32 fn_800ADC44(s32, s32, s32);
extern void* gpGame;

void fn_800A70E4(u8 arg0);
void fn_800A70E4(u8 arg0) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(0, 0, 1);
        fn_800ADC44(0, 1, arg0);
        fn_800ADB4C(0, 1, 1);
    }
}