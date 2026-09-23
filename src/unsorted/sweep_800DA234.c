// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
extern s32 lbl_80281570;
extern s32 lbl_80282278;
s32 Game_CurHoleIndex();

s32 fn_800DA234(void);
s32 fn_800DA234(void) {
    s32 t0;
    t0 = Game_CurHoleIndex();
    return (((u32)__cntlzw((t0 - lbl_80281570)) >> 5) & 0xFF);
}
