// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800ED710(s32);

void fn_800ECBE4(void);
void fn_800ECBE4(void) {
    if (Game_GetMode() == 8) {
        fn_800ED710(0);
    }
}