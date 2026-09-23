// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern void* lbl_802811B8;
extern s32 lbl_80282208;

void fn_8006765C(void);
void fn_8006765C(void) {
    if ((u32) (*(u32*)((u8*)(lbl_802811B8) + 0xD0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_802811B8) + 0xD0)));
    }
    fn_80009E70((*(s32*)((u8*)(lbl_802811B8) + 0xCC)));
    fn_80009E70((*(s32*)((u8*)(lbl_802811B8) + 0xD4)));
    lbl_80282208 = 0;
}