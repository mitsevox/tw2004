// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* gpGame;

s32 fn_800E177C(void);
s32 fn_800E177C(void) {
    return *(s32*)(gpGame + 0x8);
}
