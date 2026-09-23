// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80012C30(s32);
extern f32 lbl_80283AE8;

void fn_8007CD58(void** arg0, f32* arg1);
void fn_8007CD58(void** arg0, f32* arg1) {
    *arg1 = lbl_80283AE8 * fn_80012C30((*(s32*)((u8*)(*arg0) + 8)));
}