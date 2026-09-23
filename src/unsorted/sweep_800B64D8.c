// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
s32 fn_8009527C(s32);
s32 fn_800B5D08(s32);
extern void* lbl_802814C8;

void fn_800B64D8(void);
void fn_800B64D8(void) {
    if ((u32) (*(u32*)((u8*)(lbl_802814C8) + 0x18)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_802814C8) + 0x18)));
        (*(u32*)((u8*)(lbl_802814C8) + 0x18)) = 0U;
    }
    fn_8009527C((*(s32*)((u8*)(lbl_802814C8) + 8)));
    (*(s32*)((u8*)(lbl_802814C8) + 8)) = 0;
    fn_800B5D08((*(s32*)((u8*)(lbl_802814C8) + 0)));
    (*(s32*)((u8*)(lbl_802814C8) + 0)) = 0;
    fn_8009527C((*(s32*)((u8*)(lbl_802814C8) + 4)));
    (*(s32*)((u8*)(lbl_802814C8) + 4)) = 0;
}