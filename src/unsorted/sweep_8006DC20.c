// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281F1C;

void fn_8006DC20(f32 farg0);
void fn_8006DC20(f32 farg0) {
    if ((void* ) lbl_80281F1C != NULL) {
        (*(f32*)((u8*)(lbl_80281F1C) + 0x18)) = farg0;
    }
}