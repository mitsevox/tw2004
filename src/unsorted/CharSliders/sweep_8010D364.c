// Small functions found by the sweep (sweep.py). Original file: CharSliders.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

s32 Game_GetMode();

s32 fn_8010D364(void);
s32 fn_8010D364(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((26 - t0)) >> 5) & 0xFF);
}
