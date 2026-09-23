// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 strncpy(u8*, u32, s32, u8*);
extern u8* lbl_80281EE0;

void fn_8008E960(u32 arg0);
void fn_8008E960(u32 arg0) {
    if (arg0 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x54)) = 0;
        return;
    }
    strncpy(lbl_80281EE0 + 0x54, arg0, 0x20, lbl_80281EE0);
}