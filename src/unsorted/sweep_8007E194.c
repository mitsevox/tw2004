// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
extern void* gpGame;

void fn_8007E194(s32* arg0);
void fn_8007E194(s32* arg0) {
    if (Game_GetMode() == 7) {
        (*(s32*)((u8*)(gpGame) + 8)) = 0;
        return;
    }
    if (Game_GetMode() == 9) {
        (*(s32*)((u8*)(gpGame) + 8)) = 1;
        return;
    }
    (*(s32*)((u8*)(gpGame) + 8)) = (s32) *arg0;
}