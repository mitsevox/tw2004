// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 strcpy(s32, s32*);
extern s32 lbl_802812C4;

void fn_80082A10(void** arg0, s32* arg1);
void fn_80082A10(void** arg0, s32* arg1) {
    *arg1 = 0;
    strcpy((*(s32*)((u8*)(*arg0) + 8)), &lbl_802812C4);
}