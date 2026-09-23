// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GameModeBattle_RemoveClub(s32, s32);
s32 GameModeBattle_AddClub(s32, s32);

void fn_80089F24(void* arg0);
void fn_80089F24(void* arg0) {
    if ((s32) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        GameModeBattle_AddClub((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
        return;
    }
    GameModeBattle_RemoveClub((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 8)));
}