// FE_CrAPDB.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

u8 fn_8001E9CC(s32, s32);
s32 fn_80077ACC();
s32 fn_80104020(s32 arg0);
s32 fn_801048B0(u32 arg0);
extern s32 lbl_80282460;
s32 fn_80104F68(s32 p0);
u32 fn_80104F7C(s32 arg0);
extern s32 lbl_80282474;
s32 fn_80105140(s32 p0);
void UStream_UnregisterHandler();
void fn_80105154(void);
void fn_80077B78();
void fn_801050D0();
void fn_80105240(void);
void* fn_80104E84();
u8* fn_80105264(void);
s16 fn_80105298(void);
s16 fn_801052CC(void);
s16 fn_80105300(void);
s16 fn_80105334(void);
s32 fn_80105368(void);
s32 fn_8010539C(void);
s32 fn_801053D0(void);
void fn_80104FA8();
void fn_80105494();
void fn_801054CC();
void fn_80105504();
void fn_8010553C();
void fn_80105404(void);
void fn_80105428(void);
void fn_8010544C(void);
void fn_80105470(void);
s32 fn_80105574(void);
s16 fn_801055A8(void);
void fn_80105B80();
void fn_80105B4C(s32 p0, s32 p1, s32 p2, s32 p3);
s32 fn_80105C00(void);
s32 fn_80105C30(void);
void fn_80105EFC(void);
s32 fn_801061C8(s8 arg0);
s32 fn_8015F844(s8*, s32*);
s32 strcpy(s8*, s32);
extern s32 lbl_80281748;
s32 fn_8010645C(s32 arg0, s8* arg1);

s32 fn_80104020(s32 arg0) {
    s32 temp_r3;

    temp_r3 = fn_80077ACC();
    if ((fn_8001E9CC(temp_r3 + 0xB054, arg0) == 0) && (fn_8001E9CC(temp_r3 + 0xB1CC, arg0) != 0)) {
        return 1;
    }
    return 0;
}

s32 fn_801048B0(u32 arg0) {
    if ((arg0 <= 2U) || ((u32) (arg0 - 7) <= 1U) || ((s32) arg0 == 0xC) || ((s32) arg0 == 0x13) || ((s32) arg0 == 0x14)) {
        return 1;
    }
    return 0;
}

s32 fn_80104F68(s32 p0) {
    return (*(s32*)(((u8*)lbl_80282460) + 0x8) + (p0 * 280));
}

u32 fn_80104F7C(s32 arg0) {
    return (arg0 - (*(s32*)((u8*)(lbl_80282460) + 8))) / 280;
}

s32 fn_80105140(s32 p0) {
    return *(s32*)(((u8*)lbl_80282474) + (((s16)p0) << 2));
}

void fn_80105154(void) {
    UStream_UnregisterHandler(1129471809);
    UStream_UnregisterHandler(1129471827);
}

void fn_80105240(void) {
    fn_801050D0();
    fn_80077B78();
}

u8* fn_80105264(void) {
    u8* temp_r3;

    temp_r3 = (u8*)fn_80104E84();
    if (temp_r3 == NULL) {
        return NULL;
    }
    return temp_r3 + 4;
}

s16 fn_80105298(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return 0;
    }
    return (*(s16*)((u8*)(temp_r3) + 0x44));
}

s16 fn_801052CC(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return 0;
    }
    return (*(s16*)((u8*)(temp_r3) + 0x46));
}

s16 fn_80105300(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return 0;
    }
    return (*(s16*)((u8*)(temp_r3) + 0x48));
}

s16 fn_80105334(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1;
    }
    return (*(s16*)((u8*)(temp_r3) + 0x2C));
}

s32 fn_80105368(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1;
    }
    return (*(s32*)((u8*)(temp_r3) + 0x30));
}

s32 fn_8010539C(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1;
    }
    return (*(s32*)((u8*)(temp_r3) + 0x34));
}

s32 fn_801053D0(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1;
    }
    return (*(s32*)((u8*)(temp_r3) + 0x38));
}

void fn_80105404(void) {
    fn_80104FA8();
    fn_80105494();
}

void fn_80105428(void) {
    fn_80104FA8();
    fn_801054CC();
}

void fn_8010544C(void) {
    fn_80104FA8();
    fn_80105504();
}

void fn_80105470(void) {
    fn_80104FA8();
    fn_8010553C();
}

s32 fn_80105574(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1U;
    }
    return (*(u8*)((u8*)(temp_r3) + 0x41));
}

s16 fn_801055A8(void) {
    void* temp_r3;

    temp_r3 = fn_80104E84();
    if (temp_r3 == NULL) {
        return -1;
    }
    return (*(s16*)((u8*)(temp_r3) + 0x42));
}

void fn_80105B4C(s32 p0, s32 p1, s32 p2, s32 p3) {
    s32 t0;
    t0 = (s32)fn_80104E84();
    fn_80105B80(t0, p3);
}

s32 fn_80105C00(void) {
    return *(s32*)(lbl_80282460 + 0x0);
}

s32 fn_80105C30(void) {
    return ((u32)((-lbl_80282460) | lbl_80282460) >> 31);
}

void fn_80105EFC(void) {
}

s32 fn_801061C8(s8 arg0) {
    if ((arg0 == (s8) (*(u8*)((u8*)(lbl_80282460) + 4))) || (arg0 == 2)) {
        return 1;
    }
    return 0;
}

s32 fn_8010645C(s32 arg0, s8* arg1) {
    u32 temp_r0;

    temp_r0 = (*(u32*)((u8*)(lbl_80282460) + 0xC));
    if (temp_r0 == 0U) {
        return 0;
    }
    if (arg1 == NULL) {
        return 0;
    }
    if (arg0 == -1) {
        return 0;
    }
    strcpy(arg1, temp_r0 + arg0);
    if (fn_8015F844(arg1, &lbl_80281748) == 0) {
        *arg1 = 0;
    }
    return 1;
}

// ---- end of sweep code ----
