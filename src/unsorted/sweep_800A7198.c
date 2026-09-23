// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ADB4C();
s32 fn_800ADC44();
extern void* gpGame;

void fn_800A7198(u8 arg0);
void fn_800A7198(u8 arg0) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADC44(1, 0, arg0);
        fn_800ADB4C(1, 1, 1);
    }
}