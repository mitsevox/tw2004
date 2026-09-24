// rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree): the decoder for
// the 'MAD' movie format (TW06's rcmp folder splits it into maddec.c and madidct.c): the block
// decoder and inverse DCT, and its stream handler. Not yet decompiled; the code below is
// the sweep's matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801910E0[];
extern s32 lbl_802821C0;
extern s32 lbl_802821C4;
s32 fn_80009B34();
void fn_800B90F4(s32 p0, s32 p1);
void fn_800B9100(u8* p0, s32 p1, s32 p2);
void fn_800B9178(void* arg0);
s32 fn_800B91B8(void* arg0);
s32 fn_800B94B0(s32* arg0);
void fn_800B95FC(u32 arg0);
void* fn_800B9760();
void fn_800B9624();
void* fn_800B9700(s32 arg0);
void* fn_800B97A8(s32 arg0);
s32 fn_800B9864();
void fn_800B9808(s32 arg0, void* arg1);
u8 fn_800B9930(u8* p0);
extern s32 lbl_802821D4;
void UStream_UnregisterHandler();
s32 fn_8000FB88();
void fn_800B9988(void);
void fn_800B99BC(s32 p0);
s32 fn_8000B4B8(void*);
u8 fn_8000B508();
s32 fn_80045D80(s32);
void fn_800B9A50(void* arg0);
void fn_800B99FC(void* arg0);
s32 fn_800075CC(s32);
s32 fn_80009E70();
extern u8 lbl_802814E8;
extern s32 lbl_802821D0;
void fn_800B9A88(void);
void* fn_8000B70C(s32, s32);
u32 fn_80048808(s32);
extern u32 lbl_802821D8;
extern u32 lbl_802821DC;
extern u32 lbl_802821E0;
void fn_800B9B48(void);

void fn_800B90F4(s32 p0, s32 p1) {
    lbl_802821C4 = p0;
    lbl_802821C0 = p1;
}

void fn_800B9100(u8* p0, s32 p1, s32 p2) {
    s32 t0;
    *(s32*)(p0 + 0xC) = 0;
    t0 = fn_80009B34(((u32)((p2 * p1) * 3) >> 1), 1, 32, lbl_801910E0, 79);
    *(s32*)(p0 + 0x8) = t0;
    *(s32*)p0 = p1;
    *(s32*)(p0 + 0x4) = p2;
}

void fn_800B9178(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 8)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 8)));
        (*(u32*)((u8*)(arg0) + 8)) = 0U;
    }
}

s32 fn_800B91B8(void* arg0) {
    (*(s8*)((u8*)(arg0) + 0)) = 1;
    (*(s32*)((u8*)(arg0) + 0x18)) = 0;
    (*(s32*)((u8*)(arg0) + 0x10)) = 0;
    (*(s32*)((u8*)(arg0) + 0x1C)) = 0;
    (*(s8*)((u8*)(arg0) + 0x14)) = 0;
    (*(s32*)((u8*)(arg0) + 0x20)) = 0;
    (*(s32*)((u8*)(arg0) + 0x38)) = 0;
    (*(s32*)((u8*)(arg0) + 0x24)) = 0;
    (*(s32*)((u8*)(arg0) + 0x3C)) = 0;
    (*(s32*)((u8*)(arg0) + 0x28)) = 0;
    (*(s32*)((u8*)(arg0) + 0x40)) = 0;
    (*(s32*)((u8*)(arg0) + 0x2C)) = 0;
    (*(s32*)((u8*)(arg0) + 0x44)) = 0;
    (*(s32*)((u8*)(arg0) + 0x30)) = 0;
    (*(s32*)((u8*)(arg0) + 0x48)) = 0;
    (*(s32*)((u8*)(arg0) + 0x34)) = 0;
    (*(s32*)((u8*)(arg0) + 0x4C)) = 0;
    return 1;
}

s32 fn_800B94B0(s32* arg0) {
    if (arg0 != NULL) {
        return *arg0;
    }
    return 0x4D41446B;
}

void fn_800B95FC(u32 arg0) {
    if (arg0 != 0U) {
        fn_80009E70();
    }
}

void* fn_800B9700(s32 arg0) {
    void* temp_r3;

    temp_r3 = fn_800B9760(arg0 + 0x38);
    if (temp_r3 == NULL) {
        return NULL;
    }
    fn_800B9624(arg0 + 0x20, temp_r3);
    (*(s32*)((u8*)(temp_r3) + 0xC)) = 2;
    return temp_r3;
}

void* fn_800B97A8(s32 arg0) {
    void* temp_r3;

    temp_r3 = fn_800B9760(arg0 + 0x38);
    if (temp_r3 == NULL) {
        return NULL;
    }
    fn_800B9624(arg0 + 0x20, temp_r3);
    (*(s32*)((u8*)(temp_r3) + 0xC)) = 1;
    return temp_r3;
}

void fn_800B9808(s32 arg0, void* arg1) {
    (*(u32*)((u8*)(arg1) + 0xC)) = (u32) ((*(u32*)((u8*)(arg1) + 0xC)) - 1);
    if ((u32) (*(u32*)((u8*)(arg1) + 0xC)) == 0U) {
        fn_800B9864(arg0, arg1);
        fn_800B9624(arg0 + 0x38, arg1);
    }
}

u8 fn_800B9930(u8* p0) {
    return (((u32)__cntlzw((2 - *(u8*)(p0 + 0x14))) >> 5) & 0xFF);
}

void fn_800B9988(void) {
    UStream_UnregisterHandler(1413828384);
    UStream_UnregisterHandler(1111575622);
}

void fn_800B99BC(s32 p0) {
    s32 t0;
    t0 = fn_8000FB88(p0, 0, 0);
    lbl_802821D4 = t0;
    fn_80009E70(p0);
}

void fn_800B99FC(void* arg0) {
    if (fn_8000B508() == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800B9A50;
        fn_8000B4B8(arg0);
    }
}

void fn_800B9A50(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC((*(s32*)((u8*)(temp_r31) + 0x10)));
    fn_80009E70(temp_r31);
}

void fn_800B9A88(void) {
    lbl_802821E0 = 0;
    lbl_802821DC = 0;
    lbl_802821D8 = 0;
    lbl_802821D4 = 0;
    lbl_802821D0 = 0;
    lbl_802814E8 = 1;
}

void fn_800B9B48(void) {
    void* temp_r3;
    void* temp_r3_2;
    void* temp_r3_3;

    if ((u32) lbl_802821E0 == 0U) {
        temp_r3 = fn_8000B70C(0x54454F20, 0x2710);
        if (temp_r3 != NULL) {
            lbl_802821E0 = fn_80048808((*(s32*)((u8*)(temp_r3) + 4)));
        }
    }
    if ((u32) lbl_802821DC == 0U) {
        temp_r3_2 = fn_8000B70C(0x54454F20, 0x272E);
        if (temp_r3_2 != NULL) {
            lbl_802821DC = fn_80048808((*(s32*)((u8*)(temp_r3_2) + 4)));
        }
    }
    if ((u32) lbl_802821D8 == 0U) {
        temp_r3_3 = fn_8000B70C(0x54454F20, 0x2738);
        if (temp_r3_3 != NULL) {
            lbl_802821D8 = fn_80048808((*(s32*)((u8*)(temp_r3_3) + 4)));
        }
    }
}

// ---- end of sweep code ----
