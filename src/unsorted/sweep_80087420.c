// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
extern s32 gpSaveData;
void fn_800C9038();
s32 fn_801197CC();

void fn_80087420(u8* p0);
void fn_80087420(u8* p0) {
    fn_800C9038(*(s32*)((gPlayers + (*(s32*)p0 * 3832)) + 0xA0C), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), gPlayers);
}
