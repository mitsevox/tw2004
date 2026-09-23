// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282460;

u32 fn_80104F7C(s32 arg0);
u32 fn_80104F7C(s32 arg0) {
    return (arg0 - (*(s32*)((u8*)(lbl_80282460) + 8))) / 280;
}