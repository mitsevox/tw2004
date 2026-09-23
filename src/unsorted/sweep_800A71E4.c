// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ADB4C(s32, s32, s32);
extern void* gpGame;

void fn_800A71E4(void);
void fn_800A71E4(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0x288)) != 0) {
        fn_800ADB4C(1, 1, 0);
    }
}