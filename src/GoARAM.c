// GoARAM.c (EA's name, from its asserts): the audio RAM (ARAM): its set-up (fn_800B62DC), its heap
// and the queue of DMA transfers between main memory and ARAM (the state at lbl_802814C8). Not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern void* lbl_802814C8;
void fn_800B6214(void* arg0);
s32 fn_80009E70();
s32 fn_8009527C(s32);
void fn_800B5D08(void* arg0);
void fn_800B5D34();
void fn_800B5E88();
s32 fn_80007328();
s32 fn_80007368();
s32 fn_800B04EC(s32, s32, s32);
void fn_800B051C();
void fn_800B65C0();
void fn_800B61B4(void* arg0);
void fn_800B64D8(void);
void fn_800B655C(void);
void fn_800B6560(void);
void fn_800B6564(s32 p0);
void fn_800B6594(s32 p0);
s32 fn_800B67B4(void* arg0);
void fn_800B67EC(void* arg0);
void fn_800B6844(s32 p0, s32 p1, s32 p2);
void fn_800B68B4(s32 p0, s32 p1, s32 p2);

void fn_800B61B4(void* arg0) {
    s32 (*temp_r12)(s32);

    temp_r12 = (*(s32 (**)(s32))((u8*)(arg0) + 0x20));
    if (temp_r12 != NULL) {
        temp_r12((*(s32*)((u8*)(arg0) + 0x24)));
    }
    if ((*(u8*)((u8*)(arg0) + 0x38)) & 1) {
        fn_800B6214(arg0);
        return;
    }
    (*(s32*)((u8*)(arg0) + 0x28)) = 2;
}

void fn_800B6214(void* arg0) {
    void* temp_r0;
    void* temp_r4;
    void* temp_r5;

    (*(s32*)((u8*)(arg0) + 0x28)) = 0;
    temp_r5 = (*(void**)((u8*)(arg0) + 0x3C));
    if (arg0 == temp_r5) {
        (*(void**)((u8*)(lbl_802814C8) + 0x10)) = NULL;
    } else {
        if (arg0 == (void* ) (*(void**)((u8*)(lbl_802814C8) + 0x10))) {
            (*(void**)((u8*)(lbl_802814C8) + 0x10)) = temp_r5;
        }
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x40))) + 0x3C)) = (void* ) (*(void**)((u8*)(arg0) + 0x3C));
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x3C))) + 0x40)) = (void* ) (*(void**)((u8*)(arg0) + 0x40));
        (*(void**)((u8*)(arg0) + 0x40)) = arg0;
        (*(void**)((u8*)(arg0) + 0x3C)) = arg0;
    }
    (*(s32*)((u8*)(lbl_802814C8) + 0x20)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x20)) - 1);
    temp_r0 = (*(void**)((u8*)(lbl_802814C8) + 0x14));
    if (temp_r0 != NULL) {
        (*(void**)((u8*)(arg0) + 0x3C)) = temp_r0;
        (*(void**)((u8*)(arg0) + 0x40)) = (void* ) (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        temp_r4 = (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        if (temp_r4 != NULL) {
            (*(void**)((u8*)(temp_r4) + 0x3C)) = arg0;
        }
        (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40)) = arg0;
    }
    (*(void**)((u8*)(lbl_802814C8) + 0x14)) = arg0;
    (*(s32*)((u8*)(lbl_802814C8) + 0x24)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x24)) + 1);
}

void fn_800B64D8(void) {
    if ((u32) (*(u32*)((u8*)(lbl_802814C8) + 0x18)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_802814C8) + 0x18)));
        (*(u32*)((u8*)(lbl_802814C8) + 0x18)) = 0U;
    }
    fn_8009527C((*(s32*)((u8*)(lbl_802814C8) + 8)));
    (*(s32*)((u8*)(lbl_802814C8) + 8)) = 0;
    fn_800B5D08((*(void**)((u8*)(lbl_802814C8) + 0)));
    (*(s32*)((u8*)(lbl_802814C8) + 0)) = 0;
    fn_8009527C((*(s32*)((u8*)(lbl_802814C8) + 4)));
    (*(s32*)((u8*)(lbl_802814C8) + 4)) = 0;
}

void fn_800B655C(void) {
}

void fn_800B6560(void) {
}

void fn_800B6564(s32 p0) {
    fn_800B5D34(*(s32*)((u8*)lbl_802814C8), p0, 32, lbl_802814C8);
}

void fn_800B6594(s32 p0) {
    fn_800B5E88(*(s32*)((u8*)lbl_802814C8), p0, lbl_802814C8);
}

s32 fn_800B67B4(void* arg0) {
    s32 temp_r31;

    fn_80007368();
    temp_r31 = (*(s32*)((u8*)(arg0) + 0x28));
    fn_80007328();
    return temp_r31;
}

void fn_800B67EC(void* arg0) {
    do {
    } while (fn_800B67B4(arg0) != 2);
    fn_800B04EC((*(s32*)((u8*)(arg0) + 0x30)), (*(s32*)((u8*)(arg0) + 0x34)), (*(s32*)((u8*)(arg0) + 0x2C)));
    fn_80007368();
    fn_800B6214(arg0);
    fn_80007328();
}

void fn_800B6844(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 0);
    fn_800B65C0(p0, p1, p2, 0, 1, 0, 0, 0);
}

void fn_800B68B4(s32 p0, s32 p1, s32 p2) {
    fn_800B051C(p0, p2, 1);
    fn_800B65C0(p1, p0, p2, 1, 1, 0, 0, 0);
}

// ---- end of sweep code ----
