// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern void* lbl_802811D8;

void fn_8006E424(void);
void fn_8006E424(void) {
    if ((u32) (*(u32*)((u8*)(lbl_802811D8) + 0xC)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_802811D8) + 0xC)));
        (*(u32*)((u8*)(lbl_802811D8) + 0xC)) = 0U;
    }
}