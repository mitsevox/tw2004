// Skin.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8001614C();
void fn_80035F40();
void fn_80035F1C(void);
void fn_8006EADC();
void fn_8006ED70();
void fn_80035FBC(void);
void fn_80035FDC(void);
s32 fn_8003532C();
void fn_8003612C(s32 p0);
void fn_80035FFC(void);
void fn_80036024(f32 x0);
void fn_800360A0(void* arg0);
void fn_8006E7A4();
s32 Vec_Copy(s32);
void fn_8003614C(void* arg0);
void fn_80036460(int n);
s32 fn_80009E70();
extern u32 lbl_80281D70;
extern s32 lbl_80281D74;
extern s32 lbl_80281D78;
void fn_80036464(void);
void fn_800364A0(void);
s32 fn_80008380();
s32 fn_800CE168();
s32 fn_80112910();
s32 fn_80112A58();
s32 fn_80037D5C();
s32 fn_80037708(void* arg0);

void fn_80035F1C(void) {
    fn_8001614C();
    fn_80035F40();
}

void fn_80035FBC(void) {
    fn_8006ED70();
}

void fn_80035FDC(void) {
    fn_8006EADC();
}

void fn_80035FFC(void) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8003612C((t0 + 84));
}

void fn_80036024(f32 x0) {
    s32 t0;
    t0 = fn_8003532C();
    *(f32*)(((u8*)t0) + 0x6C) = x0;
}

void fn_800360A0(void* arg0) {
    s32 (*temp_r12)();

    temp_r12 = (*(s32 (**)())((u8*)((*(void**)((u8*)(arg0) + 0x24))) + 4));
    if (temp_r12 != NULL) {
        temp_r12(arg0);
    }
}

void fn_8003612C(s32 p0) {
    fn_8006E7A4(p0);
}

void fn_8003614C(void* arg0) {
    if (arg0 != NULL) {
        Vec_Copy((*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0x38))) + 4)) + 0x1C);
    }
}

void fn_80036460(int n) {
}

void fn_80036464(void) {
    if ((u32) lbl_80281D70 != 0U) {
        fn_80009E70(lbl_80281D70);
    }
    lbl_80281D70 = 0U;
    lbl_80281D78 = 0;
    lbl_80281D74 = 0;
}

void fn_800364A0(void) {
    lbl_80281D74 = 0;
}

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

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

s32 fn_800CD56C();
s32 fn_801127C4();
s32 fn_8011CD84();
void fn_80037CD8(void* arg0);

void fn_80037CD8(void* arg0) {
    void* temp_r3;

    fn_80037708(arg0);
    fn_8011CD84(arg0);
    fn_800CD56C(arg0);
    temp_r3 = (*(void**)((u8*)(arg0) + 0));
    if (temp_r3 != NULL) {
        if ((void* ) (*(void**)((u8*)(temp_r3) + 0x48)) != NULL) {
            fn_801127C4((void* ) (*(void**)((u8*)(temp_r3) + 0x48)));
            fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)));
        }
        fn_80009E70((*(void**)((u8*)(arg0) + 0)));
    }
    if ((u32) (*(u32*)((u8*)(arg0) + 0x1088)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x1088)));
    }
    fn_80009E70(arg0);
}

// ---- end of sweep code ----
