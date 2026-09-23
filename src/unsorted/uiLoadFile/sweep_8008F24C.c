// Small functions found by the sweep (sweep.py). Original file: uiLoadFile.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8008F164(u32);
s32 fn_80090B10();
extern void* lbl_80281F1C;

void fn_8008F24C(void);
void fn_8008F24C(void) {
    if ((u32) (*(u32*)((u8*)(lbl_80281F1C) + 0xC)) != 0U) {
        fn_80090B10();
        fn_8008F164((*(u32*)((u8*)(lbl_80281F1C) + 0xC)));
        (*(u32*)((u8*)(lbl_80281F1C) + 0xC)) = 0U;
    }
}