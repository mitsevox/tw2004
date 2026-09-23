// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_CurHoleIndex();
u8 fn_800ED6F0();
extern void* gpGame;
extern s32 lbl_80281568;

u8 fn_800DA174(void);
u8 fn_800DA174(void) {
    if ((fn_800ED6F0() != 0) && (Game_CurHoleIndex() == 0x11) && ((u8) (*(u8*)((u8*)(gpGame) + 0xD4)) == 0)) {
        return 1U;
    }
    return (u8) (lbl_80281568 == Game_CurHoleIndex());
}