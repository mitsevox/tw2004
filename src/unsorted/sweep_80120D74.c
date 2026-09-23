// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281908;

void fn_80120D74(void);
void fn_80120D74(void) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(lbl_80281908) + 0x354));
    (*(s32*)((u8*)(lbl_80281908) + 0x360)) = temp_r0;
    (*(s32*)((u8*)(lbl_80281908) + 0x35C)) = temp_r0;
}