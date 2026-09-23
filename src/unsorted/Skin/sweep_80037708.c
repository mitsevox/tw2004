// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80008380();
s32 fn_80009E70();
s32 fn_800CE168();
s32 fn_80112910();
s32 fn_80112A58();
s32 fn_80037D5C();

s32 fn_80037708(void* arg0);
s32 fn_80037708(void* arg0) {
    void* temp_r3;

    if (!((*(s32*)((u8*)(arg0) + 0x10D4)) & 2)) {
        return 0;
    }
    fn_80008380();
    temp_r3 = (*(void**)((u8*)(arg0) + 0));
    if ((temp_r3 != NULL) && ((u32) (*(u32*)((u8*)(temp_r3) + 0x48)) != 0U)) {
        fn_80112910((*(s32*)((u8*)(arg0) + 0x1090)));
        (*(s32*)((u8*)(arg0) + 0x1090)) = 0;
        fn_80112910((*(s32*)((u8*)(arg0) + 0x1098)));
        (*(s32*)((u8*)(arg0) + 0x1098)) = 0;
        fn_80112A58((*(s32*)((u8*)(arg0) + 0x10A0)));
        (*(s32*)((u8*)(arg0) + 0x10A0)) = 0;
        fn_80037D5C((*(u32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)));
    }
    fn_800CE168(arg0);
    if ((u32) (*(u32*)((u8*)(arg0) + 0x108C)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x108C)));
    }
    (*(u32*)((u8*)(arg0) + 0x108C)) = 0U;
    if ((u32) (*(u32*)((u8*)(arg0) + 0x10CC)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x10CC)));
    }
    (*(u32*)((u8*)(arg0) + 0x10CC)) = 0U;
    if ((u32) (*(u32*)((u8*)(arg0) + 0x10D0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x10D0)));
    }
    (*(u32*)((u8*)(arg0) + 0x10D0)) = 0U;
    (*(s32*)((u8*)(arg0) + 0x10D4)) = (s32) ((*(s32*)((u8*)(arg0) + 0x10D4)) & 0xFFFFFFFD);
    return 1;
}