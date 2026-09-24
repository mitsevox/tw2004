// hwsRender_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "engine.h"
#include "charstate.h"

// ---- sweep code (not yet cleaned up) ----

extern void* lbl_802824E0;
void fn_80112CEC(void);
s32 fn_80036464();
s32 fn_80037F80();
s32 fn_8011EBF8();
u8 fn_80112B80();
void fn_80112DA0(void);

void fn_80112CEC(void) {
    if (lbl_802824E0 != NULL) {
        fn_80009E70(lbl_802824E0);
        lbl_802824E0 = NULL;
    }
}

void fn_80112DA0(void) {
    fn_80112CEC();
    if (fn_80112B80() != 0) {
        fn_8011EBF8();
    }
    fn_80037F80();
    fn_80036464();
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_801136C4();
void fn_8011368C(u8* p0, s32 p1, s32 p2, s32 p3);
u32* fn_80113764(void);
void fn_80113840(void);
void fn_80113844();
void fn_8011387C(void);
void fn_8011389C(void);
void fn_801138D8(HwsOverrideTable* pTable);
void fn_801138E8(void);
void fn_801138EC(s32 p0);
void fn_801138F8(s32 p0);
void fn_80113904(s32 p0);
void fn_80113A7C(void);
void fn_80113B14(void);
void fn_80113BAC(void);
void fn_80113E54(u8* p, s32 v);
void fn_80113E5C(void);

void fn_8011368C(u8* p0, s32 p1, s32 p2, s32 p3) {
    fn_801136C4(p0, *(s32*)(p0 + 0x0), p1, p2, p3, p1);
}

u32* fn_80113764(void) {
    return lbl_80223BB0.s10.p48;
}

void fn_80113840(void) {
}

void fn_8011387C(void) {
    fn_80113844();
}

void fn_8011389C(void) {
    fn_80113840();
    lbl_80223BB0.s10.n50 = 0;
}

// Make pTable the current mesh overrides.
void fn_801138D8(HwsOverrideTable* pTable) {
    lbl_80223BB0.pOverride = pTable;
}

void fn_801138E8(void) {
}

void fn_801138EC(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_801138F8(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113904(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113A7C(void) {
    fn_80113E5C();
}

void fn_80113B14(void) {
    fn_80113E5C();
}

void fn_80113BAC(void) {
    fn_80113E5C();
}

void fn_80113E54(u8* p, s32 v) {
    *(s32*)(p + 0x0) = v;
}

void fn_80113E5C(void) {
}

// ---- end of sweep code ----
