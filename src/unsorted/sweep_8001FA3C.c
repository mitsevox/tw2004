// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_RegisterHandler(s32, void (*)(void*), s32);
s32 fn_80009E70(void*);
u32 fn_8001F804(s32);
extern u32 lbl_80281CB4;

void fn_8001FA3C(void* arg0);
void fn_8001FA3C(void* arg0) {
    u32 temp_r0;

    temp_r0 = (*(u32*)((u8*)(arg0) + 0x20));
    if ((temp_r0 < 2U) && ((u32) (&lbl_80281CB4)[temp_r0] == 0U)) {
        (&lbl_80281CB4)[temp_r0] = fn_8001F804((*(s32*)((u8*)(arg0) + 0)));
    }
    fn_80009E70(arg0);
}