// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;

void fn_8007DAB0(u8* p0, u8* p1);
void fn_8007DAB0(u8* p0, u8* p1) {
    *(s32*)p1 = *(u8*)(((u8*)gpSaveData) + (*(s32*)p0 * 67072));
}
