// LLDynTex.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the matched
// small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80077ACC();
s32 fn_8010FB70();
void strcpy();
void fn_801091B8(u8* p0, u8* p1);
void fn_8010920C(u8* p0);
s32 fn_80104DB8(s16, s32, s32);
void fn_80109354(void* arg0);
void fn_8010645C();
void fn_80109388(u8* p0);
void fn_8008E364();
void fn_80109430(void);
void fn_80109434(u8* p);
s32 fn_8008B990();
s32 fn_8008E420();
s32 fn_8008E44C();
void fn_80109458(s32 p0, u8* p1);
void fn_8010948C(s32 p0, u8* p1);
void fn_801094C0(s32 p0, u8* p1);
void fn_801094FC(void);
extern s32 lbl_80281ED4;
void fn_80109500(s32 p0, u8* p1);
void fn_8010F7FC();
void fn_80109618(u8* p0);
s32 fn_8010FBCC();
void fn_80109650(u8* p0, u8* p1);
s32 fn_8010F7E4(s32);
void fn_8010969C(void* arg0);
void fn_80109700(s32 p0, u8* p1);
void fn_80109734(void);
s32 fn_80103B74(s32);
void fn_80109738(u8* p0);
void fn_8008E824();
void fn_80109760(void);
s32 fn_8008E6D4(s32);
void fn_801097FC(s32* arg0);
void fn_8008DAEC();
void fn_8010988C(void);
void fn_801098AC(void);
s32 Rand_Next(s32);
s32 fn_8007873C(s32);
s32 fn_8007975C(s32, s32, s32);
s32 fn_800797E0(s32, s32, s32, s32);
s32 fn_801073DC(s32);
void fn_80109BA4(void);
extern u8 lbl_80193AC8[];
extern s32 lbl_80281EE0;
void fn_80016D10();
s32 fn_80017028();
void fn_80079664();
void fn_8008E244();
void fn_800A73F0();
void GolfCamera_SwitchCrAPCamera();
void fn_80109CBC(void);
extern u8 lbl_80193AD4[];
void fn_80078E34();
void fn_80109D5C(void);
void fn_80109DDC(void);

void fn_801091B8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8010FB70();
    strcpy(*(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), (t0 + 4096), *(s32*)(p0 + 0x4));
    *(s32*)p1 = *(u8*)(((u8*)t0) + 0x1020);
}

void fn_8010920C(u8* p0) {
    s32 t0;
    t0 = fn_8010FB70();
    strcpy((t0 + 4096), *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

void fn_80109354(void* arg0) {
    fn_80104DB8((s16) (*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), (*(s32*)((u8*)((*(void**)((u8*)(arg0) + 8))) + 8)));
}

void fn_80109388(u8* p0) {
    fn_8010645C(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

void fn_80109430(void) {
}

void fn_80109434(u8* p) {
    fn_8008E364(*(s32*)(p + 0x0));
}

void fn_80109458(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008E420();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_8010948C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008E44C();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_801094C0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008B990();
    *(s32*)p1 = ((u32)__cntlzw((1 - t0)) >> 5);
}

void fn_801094FC(void) {
}

void fn_80109500(s32 p0, u8* p1) {
    *(s32*)p1 = *(u8*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x63F);
}

void fn_80109618(u8* p0) {
    fn_8010F7FC(*(s32*)(p0 + 0x0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC), *(s32*)(p0 + 0x10));
}

void fn_80109650(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8010FBCC(*(s32*)p0, *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC), *(s32*)(p0 + 0x10), *(s32*)(p0 + 0x14));
    *(s32*)p1 = t0;
}

void fn_8010969C(void* arg0) {
    s32 temp_r4;
    void* temp_r3;

    temp_r3 = (void*)fn_8010FB70();
    temp_r4 = (*(s32*)((u8*)(arg0) + 4));
    switch (temp_r4) {
    case 0:
        (*(s8*)((u8*)(temp_r3) + 0x1021)) = 0;
        break;
    case 1:
        (*(s8*)((u8*)(temp_r3) + 0x1021)) = 1;
        break;
    }
    fn_8010F7E4(temp_r4);
}

void fn_80109700(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8010FB70();
    *(s32*)p1 = *(u8*)(((u8*)t0) + 0x1021);
}

void fn_80109734(void) {
}

void fn_80109738(u8* p0) {
    fn_80103B74((*(s32*)p0 & 0xFF));
}

void fn_80109760(void) {
    fn_8008E824();
}

void fn_801097FC(s32* arg0) {
    s32 temp_r0;

    temp_r0 = *arg0;
    switch (temp_r0) {
    case 0:
        fn_8008E6D4(0);
        return;
    case 1:
        fn_8008E6D4(1);
        return;
    case 2:
        fn_8008E6D4(4);
        return;
    case 3:
        fn_8008E6D4(5);
        return;
    case 4:
        fn_8008E6D4(2);
        return;
    default:
        fn_8008E6D4(0);
        return;
    }
}

void fn_8010988C(void) {
    fn_8008DAEC();
}

void fn_801098AC(void) {
}

void fn_80109BA4(void) {
    s32 temp_r31;

    temp_r31 = fn_80077ACC();
    fn_80103B74(0);
    fn_801073DC(2);
    fn_801073DC(5);
    fn_801073DC(6);
    fn_801073DC(7);
    fn_801073DC(8);
    fn_801073DC(0xB);
    fn_801073DC(0xC);
    fn_801073DC(0xD);
    fn_801073DC(0xE);
    fn_8007975C(temp_r31, 0, 0x50);
    fn_8007975C(temp_r31, 1, 0);
    fn_8007975C(temp_r31, 2, 0);
    fn_8007975C(temp_r31, 7, 0);
    fn_8007975C(temp_r31, 0x13, 0x46);
    fn_8007975C(temp_r31, 0x14, 0x46);
    fn_8007975C(temp_r31, 8, 0x46);
    fn_800797E0(temp_r31, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_8007873C(temp_r31);
}

void fn_80109CBC(void) {
    s32 t0;
    s32 t2;
    s32 t3;
    t0 = fn_80077ACC();
    fn_80016D10();
    t2 = fn_80017028();
    t3 = Rand_Next(0);
    fn_800A73F0(((t3 & 0x7) + 11));
    fn_8008E244();
    fn_8008E364(0);
    GolfCamera_SwitchCrAPCamera(t2, lbl_80193AC8, *(s32*)(((u8*)lbl_80281EE0) + 0x4), 0, 0, 0);
    fn_80103B74(0);
    fn_80079664(t0);
    fn_8007873C(t0);
}

void fn_80109D5C(void) {
    s32 t0;
    s32 t2;
    t0 = fn_80077ACC();
    fn_80016D10();
    t2 = fn_80017028();
    fn_8008E244();
    fn_8008E364(1);
    GolfCamera_SwitchCrAPCamera(t2, lbl_80193AD4, *(s32*)(((u8*)lbl_80281EE0) + 0x4), 0, 0, 0);
    fn_80103B74(0);
    fn_80078E34(t0);
}

void fn_80109DDC(void) {
}

// ---- end of sweep code ----
