// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_80049018();

s32 fn_800490B8(void);
s32 fn_800490B8(void) {
    void* temp_r3;

    temp_r3 = fn_80049018();
    if (temp_r3 != NULL) {
        return (*(s32*)((u8*)(temp_r3) + 0x134));
    }
    return -2;
}