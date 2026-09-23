// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* gpGame;
extern s32 lbl_802823C0;

s32 fn_800F9308(void);
s32 fn_800F9308(void) {
    if ((u8) (*(u8*)((u8*)(gpGame) + 0xD4)) != 0) {
        return lbl_802823C0;
    }
    return lbl_802823C0 + 1;
}