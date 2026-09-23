// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008C938();
s32 fn_8008C93C();
s32 fn_8008CC30();
s32 fn_8008CE2C();
s32 fn_8008CE88(s32);
s32 fn_800B9CF0(s32);
extern s32 lbl_80281330;
extern void* lbl_80281EE0;

void fn_8008C844(void);
void fn_8008C844(void) {
    if (((u8) (*(u8*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 0x18)) != 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x86)) == 0) && ((u8) (*(u8*)((u8*)(lbl_80281EE0) + 0x88)) == 0) && ((s32) lbl_80281330 != 0)) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x18C)) = 1;
        fn_8008C938();
        fn_8008C93C();
        fn_8008CE88(0);
        fn_800B9CF0(0);
        fn_8008CC30();
        fn_8008CE2C();
        fn_8008C93C();
    }
}