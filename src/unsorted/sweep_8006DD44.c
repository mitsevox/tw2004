// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8002F32C(s32);
extern void* lbl_80281100;

void fn_8006DD44(void);
void fn_8006DD44(void) {
    if (((s32) (*(s32*)((u8*)(lbl_80281100) + 4)) != 0) && ((s32) (*(s32*)((u8*)(lbl_80281100) + 8)) != 0)) {
        fn_8002F32C(1);
    }
}