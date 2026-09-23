// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80281F1C;
extern f32 lbl_80283BA4;

f32 fn_80092210(void);
f32 fn_80092210(void) {
    if ((void* ) lbl_80281F1C != NULL) {
        return (*(f32*)((u8*)(lbl_80281F1C) + 0x18));
    }
    return lbl_80283BA4;
}