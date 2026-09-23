// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
extern u8 lbl_801D7148[];

void fn_80082C74(u8* p0);
void fn_80082C74(u8* p0) {
    *(u8*)(((u8*)gpSaveData) + (*(s32*)p0 * 67072)) = 0;
    *(u8*)(lbl_801D7148 + *(s32*)p0) = 0;
}
