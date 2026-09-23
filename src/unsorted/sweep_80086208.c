// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_CurHoleIndex();

void fn_80086208(s32 p0, u8* p1);
void fn_80086208(s32 p0, u8* p1) {
    s32 t0;
    t0 = Game_CurHoleIndex();
    *(s32*)p1 = t0;
}
