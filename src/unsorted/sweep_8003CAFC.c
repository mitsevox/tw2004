// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8003D0EC();

s32 fn_8003CAFC(void* arg0);
s32 fn_8003CAFC(void* arg0) {
    if (arg0 == NULL) {
        return 0;
    }
    if (fn_8003D0EC() == 0) {
        return 0;
    }
    if ((s32) (*(u8*)((u8*)(arg0) + 0)) != 0x44) {
        return 0;
    }
    if ((s32) (*(u8*)((u8*)(arg0) + 1)) != 0x45) {
        return 0;
    }
    return (s8) (*(u8*)((u8*)(arg0) + 2)) == 0x46;
}