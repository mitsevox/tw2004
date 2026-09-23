// Small functions found by the sweep (sweep.py). Original file: EASportsBio.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_80009E70();
extern void* lbl_80281988;

void fn_801250C0(void);
void fn_801250C0(void) {
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 0)));
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 4)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 4)));
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281988) + 8)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281988) + 8)));
    }
}