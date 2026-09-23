// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281ED4;

void fn_8007D270(s32* arg0);
void fn_8007D270(s32* arg0) {
    if ((s32) *arg0 != 0) {
        (*(s8*)((u8*)(lbl_80281ED4) + 0x11702)) = 1;
        return;
    }
    (*(s8*)((u8*)(lbl_80281ED4) + 0x11702)) = 0;
}