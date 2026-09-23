// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008E354();
extern void* lbl_80281EE0;

void fn_8007C254(s32* arg0);
void fn_8007C254(s32* arg0) {
    u8 temp_r5;

    temp_r5 = (*(u8*)((u8*)(lbl_80281EE0) + 0x86));
    (*(u8*)((u8*)(lbl_80281EE0) + 0x86)) = (u8) *arg0;
    if ((temp_r5 != (u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x86))) && ((s32) (*(s32*)((u8*)(lbl_80281EE0) + 0)) == 3)) {
        fn_8008E354();
    }
}