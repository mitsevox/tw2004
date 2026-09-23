// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();

s32 fn_80100294(void);
s32 fn_80100294(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((11 - t0)) >> 5) & 0xFF);
}
