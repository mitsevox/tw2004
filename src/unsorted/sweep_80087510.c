// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* gpGame;

void fn_80087510(void* arg0);
void fn_80087510(void* arg0) {
    (*(s32 (**)(s32, s32, void*))((u8*)(gpGame) + 0x25C))((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0);
}