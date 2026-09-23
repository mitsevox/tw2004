// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 Player_IsHoled(s32);

s32 fn_800F9C00(void);
s32 fn_800F9C00(void) {
    if ((Player_IsHoled(0) != 0) || (Player_IsHoled(1) != 0)) {
        return 1;
    }
    return 0;
}