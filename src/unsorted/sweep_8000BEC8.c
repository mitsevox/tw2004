// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF8;

void fn_8000BEC8(void);
void fn_8000BEC8(void) {
    *(s32*)(((u8*)lbl_80280DF8) + 0x508) = 0;
}
