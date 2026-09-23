// LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80011160(void);
s32 fn_80009E70();
void fn_80011164(void* arg0);
void fn_800111A4(s32 p0);
extern u8 lbl_801A3478[];
extern u8 lbl_801A3494[];
void GXSetProjectionv();
void fn_80012444();
void fn_800112DC(void);
void fn_80011C8C(void);

void fn_80011160(void) {
}

void fn_80011164(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x470)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x470)));
        (*(u32*)((u8*)(arg0) + 0x470)) = 0U;
    }
}

void fn_800111A4(s32 p0) {
    fn_80011164((void*)p0);
    fn_80009E70(p0);
}

void fn_800112DC(void) {
    fn_80012444(lbl_801A3494);
    GXSetProjectionv(lbl_801A3478);
}

void fn_80011C8C(void) {
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_80012438(u8* p);
void GXSetTexCoordGen2();
void fn_8001247C(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800124CC();
void fn_800124A4(void);
void fn_800124A8(void);
void GXBegin();
void fn_80012520(void);
void fn_80012540(f32 farg0, f32 farg1);
void fn_80012550(s32 p0);
void fn_8001255C(f32 farg0, f32 farg1);

void fn_80012438(u8* p) {
    *(s32*)(p + 0x474) = 0;
}

void fn_8001247C(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}

void fn_800124A4(void) {
}

void fn_800124A8(void) {
    fn_800124A4();
    fn_800124CC();
}

void fn_80012520(void) {
    GXBegin();
}

void fn_80012540(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_80012550(s32 p0) {
    *(volatile s32*)0xCC008000 = p0;
}

void fn_8001255C(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

// ---- end of sweep code ----
