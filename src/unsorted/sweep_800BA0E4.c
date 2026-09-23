// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F8458[];
extern u8 lbl_801F846C[];
void UStream_UnregisterHandler();
void fn_80005628();
void fn_80009E70();
s32 fn_800107C0();

void fn_800BA0E4(void);
void fn_800BA118(u8* p0);
void fn_800BA15C(s32 p0);
void fn_800BA0E4(void) {
    UStream_UnregisterHandler(1414676824);
    UStream_UnregisterHandler(1414682708);
}

void fn_800BA118(u8* p0) {
    fn_80005628(lbl_801F846C, *(s32*)p0, 7296);
    fn_80009E70(p0);
}

void fn_800BA15C(s32 p0) {
    s32 t0;
    t0 = fn_800107C0(p0, 0, 0);
    *(s32*)(lbl_801F8458 + 0x10) = t0;
    fn_80009E70(p0, lbl_801F8458);
}
