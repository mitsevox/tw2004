// MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

u32 fn_80009B34(s32, s32, s32, s32*, s32);
s32 fn_8009EF98();
u32 fn_800B6564();
extern s32 lbl_802813DC;
extern s32 lbl_80281FC0;
extern u32 lbl_80281FC4;
extern u32 lbl_80281FD8;
extern u32 lbl_80281FDC;
extern u32 lbl_80281FE0;
extern u32 lbl_80281FE4;
extern u32 lbl_80281FE8;
extern u32 lbl_80281FEC;
void fn_8009CC00(void);
s32 fn_80009E70();
s32 fn_800A1BE0();
s32 fn_800B6594();
extern u32 lbl_80281FB8;
extern u32 lbl_80281FBC;
void fn_8009CC88(void);
void fn_8009DCEC(s32 p0, s32 p1);
void fn_8009CD7C(void);
void fn_8009CD80(s32 p0, s32 p1);
void fn_8009DCE8(void);
s32 fn_8009D74C();
void fn_8009DBAC();
void fn_8009DC80();
void UStream_UnregisterHandler();
void fn_8009EAF0(void);
void fn_8009EB30(s32 v);
void fn_8009EB38(s32 v);
void fn_8009EB40(void);
extern u8 lbl_8018C6E8[];
void fn_8009EECC();
void fn_8009EF68(s32 p0, s32 p1);
s32 fn_8009EF90(void);
extern u8 lbl_801E3180[];
extern s32 lbl_80281FC8;
void CARDRead();
s32 fn_8009F208(s32 p0, s32 p1, s32 p2);
s32 fn_8009F35C(void);
s32 fn_8009F364(void);

void fn_8009CC00(void) {
    u32 temp_r3;
    u32 temp_r3_2;

    lbl_80281FC0 = 0x50020;
    if ((u32) lbl_80281FC4 == 0U) {
        lbl_80281FC4 = fn_800B6564();
    }
    temp_r3 = fn_80009B34(0x50000, 1, 0x20, &lbl_802813DC, 0x81);
    lbl_80281FE8 = temp_r3;
    lbl_80281FE4 = temp_r3;
    lbl_80281FEC = temp_r3;
    temp_r3_2 = fn_80009B34(0x50000, 1, 0x20, &lbl_802813DC, 0x85);
    lbl_80281FDC = temp_r3_2;
    lbl_80281FD8 = temp_r3_2;
    lbl_80281FE0 = temp_r3_2;
    fn_8009EF98();
}

void fn_8009CC88(void) {
    if ((u32) lbl_80281FB8 != 0U) {
        fn_80009E70(lbl_80281FB8);
        lbl_80281FB8 = 0U;
    }
    if ((u32) lbl_80281FBC != 0U) {
        fn_80009E70(lbl_80281FBC);
        lbl_80281FBC = 0U;
    }
    fn_800A1BE0();
    if ((u32) lbl_80281FE8 != 0U) {
        fn_80009E70(lbl_80281FE8);
        lbl_80281FEC = 0U;
        lbl_80281FE4 = 0U;
        lbl_80281FE8 = 0U;
    }
    if ((u32) lbl_80281FC4 != 0U) {
        fn_800B6594(lbl_80281FC4);
        lbl_80281FC4 = 0U;
    }
}

void fn_8009CD7C(void) {
}

void fn_8009CD80(s32 p0, s32 p1) {
    fn_8009DCEC(p0, p1);
}

void fn_8009DCE8(void) {
}

void fn_8009DCEC(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_8009D74C();
    fn_8009DC80(p0, p1, t0);
    fn_8009DBAC(p0, p1);
}

void fn_8009EAF0(void) {
    UStream_UnregisterHandler(1296255264);
    UStream_UnregisterHandler(1296253472);
    UStream_UnregisterHandler(1700882285);
}

void fn_8009EB30(s32 v) {
    lbl_80281FB8 = v;
}

void fn_8009EB38(s32 v) {
    lbl_80281FBC = v;
}

void fn_8009EB40(void) {
}

void fn_8009EF68(s32 p0, s32 p1) {
    fn_8009EECC(p0, p1, lbl_8018C6E8);
}

s32 fn_8009EF90(void) {
    return lbl_80281FC0;
}

s32 fn_8009F208(s32 p0, s32 p1, s32 p2) {
    CARDRead((lbl_801E3180 + (p0 * 20)), p1, p2, lbl_80281FC8, (p0 * 20));
    lbl_80281FC8 = (lbl_80281FC8 + p2);
    return 0;
}

s32 fn_8009F35C(void) {
    return 0;
}

s32 fn_8009F364(void) {
    return 0;
}

// ---- end of sweep code ----
