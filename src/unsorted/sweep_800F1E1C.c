// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800F3828();
s32 fn_800F48C4();

void fn_800F1E1C(void);
void fn_800F1E1C(void) {
    if (Game_GetMode() == 0xE) {
        fn_800F3828();
    }
    if (Game_GetMode() == 0xF) {
        fn_800F48C4();
    }
}