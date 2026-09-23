// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void Anim_SetRate(u8* p, f32 v);
void Anim_SetRate(u8* p, f32 v) {
    *(f32*)(p + 0x14) = v;
}
