// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281ED4;

void fn_80082D98(s32* arg0);
void fn_80082D98(s32* arg0) {
    (*(s8*)((u8*)(lbl_80281ED4) + 3)) = (s8) *arg0;
}