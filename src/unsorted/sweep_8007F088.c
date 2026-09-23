// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
s32 fn_800E0334();

void fn_8007F088(u8* p0, u8* p1);
void fn_8007F088(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800E0334((gpSaveData + (*(s32*)p0 * 67072)), p1, gpSaveData);
    *(s32*)p1 = t0;
}
