// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80012C30(s32);
extern f32 lbl_80283B34;

void fn_8008A788(void** arg0, f32* arg1);
void fn_8008A788(void** arg0, f32* arg1) {
    *arg1 = lbl_80283B34 * fn_80012C30((*(s32*)((u8*)(*arg0) + 8)));
}