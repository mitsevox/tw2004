// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Bag_CountClubs();

void fn_80089E9C(u8* p0, u8* p1);
void fn_80089E9C(u8* p0, u8* p1) {
    s32 t0;
    t0 = Bag_CountClubs(*(s32*)p0);
    *(s32*)p1 = t0;
}
