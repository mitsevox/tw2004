// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80169B3C(u8* p, s32 v);
void fn_80169B44(u8* p, s32 v);
void fn_80169B3C(u8* p, s32 v) {
    *(s32*)(p + 0x28) = v;
}

void fn_80169B44(u8* p, s32 v) {
    *(s32*)(p + 0xC) = v;
}
