// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GameModeDriverPGATour_GetName();
void GameModeDriverPGATour_GetCurrentEventID();
void strcpy();

void fn_8008A468(u8* p0);
void fn_8008A468(u8* p0) {
    s32 t1;
    GameModeDriverPGATour_GetCurrentEventID();
    t1 = GameModeDriverPGATour_GetName();
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t1, *(s32*)p0);
}
