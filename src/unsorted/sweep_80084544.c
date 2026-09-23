// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 EASBio_IsBioLoaded();

void fn_80084544(s32 p0, u8* p1);
void fn_80084544(s32 p0, u8* p1) {
    s32 t0;
    t0 = EASBio_IsBioLoaded();
    *(s32*)p1 = (t0 & 0xFF);
}
