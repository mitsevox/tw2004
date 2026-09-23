// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281DF0;

void fn_800588D4(s16 arg0);
void fn_800588D4(s16 arg0) {
    (*(s8*)((u8*)(&lbl_80281DF0) + 0)) = 1;
    (*(s16*)((u8*)(&lbl_80281DF0) + 2)) = arg0;
}