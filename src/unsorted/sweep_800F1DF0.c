// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800F7DE8();

void fn_800F1DF0(void);
void fn_800F1DF0(void) {
    if (Game_GetMode() == 0xD) {
        fn_800F7DE8();
    }
}