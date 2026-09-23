// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Mem_cpy(s32, s32, s32);
s32 fn_80077ACC();
s32 fn_800CCA40(void*, void*);
s32 fn_800CCEA0(void*);
extern void* lbl_80281EE0;

void fn_80103D6C(void);
void fn_80103D6C(void) {
    s32 temp_r31;
    void* temp_r30;

    temp_r31 = fn_80077ACC();
    temp_r30 = (*(void**)((u8*)((*(void**)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8))) + 0x3C));
    Mem_cpy(temp_r31 + 0x5614, (*(s32*)((u8*)(temp_r30) + 0x10B4)), fn_800CCA40(temp_r30, lbl_80281EE0) * 8);
    Mem_cpy(temp_r31 + 0x5754, (*(s32*)((u8*)(temp_r30) + 0x10C4)), fn_800CCEA0(temp_r30) * 8);
}