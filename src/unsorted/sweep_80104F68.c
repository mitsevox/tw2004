// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282460;

s32 fn_80104F68(s32 p0);
s32 fn_80104F68(s32 p0) {
    return (*(s32*)(((u8*)lbl_80282460) + 0x8) + (p0 * 280));
}
