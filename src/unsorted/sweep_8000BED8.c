// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* gpGame;

s32 Game_GetMode(void);
s32 Game_GetMode(void) {
    return *(s32*)(gpGame + 0x0);
}
