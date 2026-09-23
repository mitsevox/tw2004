// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 Session_SetGolfer(s32, s32);
extern void* lbl_80281ED4;

void fn_8007C12C(void* arg0);
void fn_8007C12C(void* arg0) {
    if (Game_GetMode() == 5) {
        (*(s8*)((u8*)(lbl_80281ED4) + 0x11703)) = 1;
    }
    Session_SetGolfer((*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)(arg0) + 0)));
}