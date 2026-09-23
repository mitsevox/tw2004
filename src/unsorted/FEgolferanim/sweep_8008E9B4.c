// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

u32 Char_SetClip(s32, s32, s32, s32);
s32 fn_8001BE88(s32, u32, s32, void*, f32);
s32 fn_8001C5B4(s32, s32);
extern void* lbl_80281EE0;
extern f32 lbl_80283B3C;

void fn_8008E9B4(void);
void fn_8008E9B4(void) {
    u32 temp_r3;

    fn_8001C5B4((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 5);
    temp_r3 = Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 8, 0, 0);
    if (temp_r3 != 0U) {
        fn_8001BE88((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), temp_r3, 0, lbl_80281EE0, lbl_80283B3C);
    }
    fn_8001C5B4((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), (*(s32*)((u8*)(lbl_80281EE0) + 0x1B8)));
}