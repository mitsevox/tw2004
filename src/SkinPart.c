// SkinPart.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the matched
// small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_800CD7CC(s32 p0, s32 p1);
s32 fn_8011389C();
void fn_800CE128(void* arg0);
void fn_800CE164(void);
void fn_800CE168(void);
void fn_800CE16C(void);

// ---- sweep code (not yet cleaned up) ----

void fn_800CEE98();
void fn_800CCA1C(void);
void fn_800CCA3C(void);
s32 fn_800CCA40(void** arg0);
s32 fn_800CCEA0(void** arg0);

void fn_800CCA1C(void) {
    fn_800CEE98();
}

void fn_800CCA3C(void) {
}

s32 fn_800CCA40(void** arg0) {
    void* temp_r3;
    void* temp_r3_2;

    temp_r3_2 = *arg0;
    if (temp_r3_2 == NULL) {
        return 0;
    }
    temp_r3 = (*(void**)((u8*)(temp_r3_2) + 0x48));
    if (temp_r3 != NULL) {
        return (*(s32*)((u8*)(temp_r3) + 0x48));
    }
    return 0;
}

s32 fn_800CCEA0(void** arg0) {
    void* temp_r3;
    void* temp_r3_2;

    temp_r3_2 = *arg0;
    if (temp_r3_2 == NULL) {
        return 0;
    }
    temp_r3 = (*(void**)((u8*)(temp_r3_2) + 0x48));
    if (temp_r3 != NULL) {
        return (*(s32*)((u8*)(temp_r3) + 0x70));
    }
    return 0;
}

// ---- end of sweep code ----

s32 fn_800CD7CC(s32 p0, s32 p1) {
    return p1;
}

void fn_800CE128(void* arg0) {
    if (((u32) (*(u32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)) != 0U) && ((*(s32*)((u8*)(arg0) + 0x10D4)) & 2)) {
        fn_8011389C();
    }
}

void fn_800CE164(void) {
}

void fn_800CE168(void) {
}

void fn_800CE16C(void) {
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8010AD50();
void fn_800CEDE0(s32 p0, s32 p1);
extern u8 lbl_80282238;
void fn_800CEE88(u8 v);
u8 fn_800CEE90(void);
void fn_800CEE98(void);
void fn_800CEEBC(void);
u8 fn_800CEEC0(u8* p);
s32 fn_800CEEF4(u8* p);
s32 fn_800CEEFC(u8* p);

void fn_800CEDE0(s32 p0, s32 p1) {
    fn_8010AD50(p1);
}

void fn_800CEE88(u8 v) {
    lbl_80282238 = v;
}

u8 fn_800CEE90(void) {
    return lbl_80282238;
}

void fn_800CEE98(void) {
    fn_800CEE88(1);
}

void fn_800CEEBC(void) {
}

u8 fn_800CEEC0(u8* p) {
    return *(u8*)(p + 0x4);
}

s32 fn_800CEEF4(u8* p) {
    return *(s32*)(p + 0x8);
}

s32 fn_800CEEFC(u8* p) {
    return *(s32*)(p + 0xC);
}

// ---- end of sweep code ----
