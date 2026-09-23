// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80280DD8;

void fn_800106A0(s32 v);
void fn_800106AC(s32 v);
void fn_800106B8(u8 v);
void fn_800106A0(s32 v) {
    *(s32*)(lbl_80280DD8 + 0x8) = v;
}

void fn_800106AC(s32 v) {
    *(s32*)(lbl_80280DD8 + 0x10) = v;
}

void fn_800106B8(u8 v) {
    *(u8*)(lbl_80280DD8 + 0x14) = v;
}
