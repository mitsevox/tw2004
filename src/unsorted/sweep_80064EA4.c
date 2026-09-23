// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern void* lbl_80281E18;

void fn_80064EA4(void);
void fn_80064EA4(void) {
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E04)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E00)) = 0;
    (*(s8*)((u8*)(lbl_80281E18) + 0x1E08)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E34)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E38)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E3C)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E40)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E44)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E48)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E4C)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E50)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E54)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E58)) = 0;
    if ((u32) (*(u32*)((u8*)(lbl_80281E18) + 0x1E64)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281E18) + 0x1E64)));
    }
    (*(u32*)((u8*)(lbl_80281E18) + 0x1E64)) = 0U;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E60)) = 0;
}