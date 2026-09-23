// hlaudmovie.c (TW06's golf/audio/engine/hl/hlaudmovie.c; Mov_Exit is paired with TW06's): the
// movie player's audio side. Its extent is proven by its data: fn_800A85CC is the first function
// to use its .sbss (0x80282060-0x80282098; the file before it ends at 0x80282058), and
// fn_800A9808 after it is the first to use the next file's .bss (0x801F1868). Not yet decompiled.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

f32 fn_800A85FC(f32 x0, f32 x1);
s32 fn_800A86B0(void);
void fn_800A86B8(void);
extern u8 lbl_80282064;
void fn_800A8700(u8 v);
extern u8 lbl_801F17D0[];
void Mem_cpy();
void fn_800A871C(s32 p0, s32 p1);
extern s32 lbl_80282060;
void fn_800A874C(s32 v);
extern u8 lbl_80282068;
s32 fn_800A87A4(s32 p0, s32 p1, s32 p2);
void fn_800A87B0(void);
void fn_800AF31C();
void fn_800A87B4(s32 p0, s32 p1);
s32 fn_800A8824(void);
extern void* lbl_80282088;
void* fn_800B5BD8(void);
void fn_800A8D00(void);
s32 fn_800A8D2C(void);
extern s32 lbl_80282074;
extern s32 lbl_80282078;
extern s32 lbl_8028207C;
extern s32 lbl_80282080;
extern s32 lbl_80282084;
extern s32 lbl_8028208C;
extern s32 lbl_80282090;
extern u8 lbl_80282094;
extern u8 lbl_80282095;
void fn_800A8D54(void);
void fn_800A7C2C();
void fn_800A9AC4();
void fn_800AC49C();
void fn_800AF2D8();
void fn_800AFB50();
void fn_800B0448();
void fn_800B0660();
void fn_800A8D88(void);
extern void* lbl_80282070;
void* fn_800A9374(void);
void* fn_800A942C(void);
void fn_800A9434(void);
s32 fn_800A955C(s32 n);

f32 fn_800A85FC(f32 x0, f32 x1) {
    return (x0 * x1);
}

s32 fn_800A86B0(void) {
    return 1;
}

void fn_800A86B8(void) {
}

void fn_800A8700(u8 v) {
    lbl_80282064 = v;
}

void fn_800A871C(s32 p0, s32 p1) {
    Mem_cpy(lbl_801F17D0, p1, ((p0 & 0xFF) << 2));
}

void fn_800A874C(s32 v) {
    lbl_80282060 = v;
}

s32 fn_800A87A4(s32 p0, s32 p1, s32 p2) {
    lbl_80282068 = p2;
    return 1;
}

void fn_800A87B0(void) {
}

void fn_800A87B4(s32 p0, s32 p1) {
    fn_800AF31C(p1);
}

s32 fn_800A8824(void) {
    return 1;
}

void fn_800A8D00(void) {
    fn_800A955C(0);
    lbl_80282088 = fn_800B5BD8();
}

s32 fn_800A8D2C(void) {
    fn_800A8D54();
    fn_800A8D00();
    return 1;
}

void fn_800A8D54(void) {
    lbl_80282070 = 0;
    lbl_80282074 = 0;
    lbl_80282078 = 0;
    lbl_8028207C = 0;
    lbl_80282080 = 0;
    lbl_80282084 = 0;
    lbl_80282088 = 0;
    lbl_8028208C = 0;
    lbl_80282090 = 0;
    lbl_80282094 = 0;
    lbl_80282095 = 0;
}

void fn_800A8D88(void) {
    fn_800A86B8();
    fn_800AC49C();
    fn_800A9AC4();
    fn_800A7C2C();
    fn_800A87B0();
    fn_800AF2D8();
    fn_800B0660();
    fn_800B0448();
    fn_800AFB50();
}

void* fn_800A9374(void) {
    void* temp_r3;

    if ((void* ) lbl_80282070 == NULL) {
        temp_r3 = fn_800B5BD8();
        lbl_80282070 = temp_r3;
        return temp_r3;
    }
    return NULL;
}

void* fn_800A942C(void) {
    return lbl_80282088;
}

void fn_800A9434(void) {
}

s32 fn_800A955C(s32 n) {
    return 65536;
}

// ---- end of sweep code ----
