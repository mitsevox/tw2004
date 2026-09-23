// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gReplayData[];
void strcpy();

void fn_800801D4(u8* p0);
void fn_800801D4(u8* p0) {
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), (gReplayData + 0x32), *(s32*)p0);
}
