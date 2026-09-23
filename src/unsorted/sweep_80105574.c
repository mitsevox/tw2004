// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_80104E84();

s32 fn_80105574(void);
s32 fn_80105574(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1U;
    }
    return (*(u8*)((u8*)(temp_r3) + 0x41));
}