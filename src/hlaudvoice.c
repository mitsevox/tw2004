#include "game_types.h"

void fn_800AC330();
s32 fn_800AC470(void);
s32 fn_800AC494(void);
void fn_800AC49C(void);
s32 fn_800AFCBC(u16, u8);
void fn_800ACA5C(void* arg0, u8 arg1);
s32 fn_800B0748();
s32 fn_800ACA94();
void fn_800ACB28(void* arg0);
f32 fn_800ACEC4(f32 x0, f32 x1);

s32 fn_800AC470(void) {
    fn_800AC330();
    return 1;
}

s32 fn_800AC494(void) {
    return 1;
}

void fn_800AC49C(void) {
}

void fn_800ACA5C(void* arg0, u8 arg1) {
    if ((arg0 != NULL) && !(((u8) (*(u8*)((u8*)(arg0) + 0xB)) >> 6U) & 1)) {
        fn_800AFCBC((*(u16*)((u8*)(arg0) + 8)), arg1);
    }
}

void fn_800ACB28(void* arg0) {
    fn_800ACA94();
    (*(s32*)((u8*)(arg0) + 0x1C)) = 0;
    (*(s32*)((u8*)(arg0) + 0x20)) = 0;
    (*(s32*)((u8*)(arg0) + 0x24)) = 0;
    if (((u8) (*(u8*)((u8*)(arg0) + 0xA)) >> 4U) & 1) {
        fn_800ACA5C(arg0, 1);
        if ((u32) (*(u32*)((u8*)(arg0) + 0x28)) != 0U) {
            fn_800B0748((*(u32*)((u8*)(arg0) + 0x28)));
            (*(u32*)((u8*)(arg0) + 0x28)) = 0U;
            (*(s32*)((u8*)(arg0) + 0x2C)) = 0;
        }
    }
}

f32 fn_800ACEC4(f32 x0, f32 x1) {
    return (x0 * x1);
}
