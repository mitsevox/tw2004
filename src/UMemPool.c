// UMemPool.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8000AD34(void* arg0, void* arg1);
double fabs(double x);
double fn_8015F7C4();
f32 fn_8000AD78(f32 x0);
f32 fabsf(f32 x0);
void fn_8000AF1C(void);
extern s32 lbl_80281BD8;
void fn_8000AE9C();
s32 fn_800951A0();
void fn_8000AF20(void);
void fn_8009527C();
void fn_8000AF58(void);
double fn_8015F804();
f32 fn_8000AF7C(f32 x0);

void fn_8000AD34(void* arg0, void* arg1) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;

    temp_f1 = (*(f32*)((u8*)(arg0) + 0));
    (*(f32*)((u8*)(arg0) + 0)) = (f32) (*(f32*)((u8*)(arg1) + 0));
    (*(f32*)((u8*)(arg1) + 0)) = temp_f1;
    temp_f1_2 = (*(f32*)((u8*)(arg0) + 4));
    (*(f32*)((u8*)(arg0) + 4)) = (f32) (*(f32*)((u8*)(arg1) + 4));
    (*(f32*)((u8*)(arg1) + 4)) = temp_f1_2;
    temp_f1_3 = (*(f32*)((u8*)(arg0) + 8));
    (*(f32*)((u8*)(arg0) + 8)) = (f32) (*(f32*)((u8*)(arg1) + 8));
    (*(f32*)((u8*)(arg1) + 8)) = temp_f1_3;
    temp_f1_4 = (*(f32*)((u8*)(arg0) + 0xC));
    (*(f32*)((u8*)(arg0) + 0xC)) = (f32) (*(f32*)((u8*)(arg1) + 0xC));
    (*(f32*)((u8*)(arg1) + 0xC)) = temp_f1_4;
}

f32 fn_8000AD78(f32 x0) {
    f32 t0;
    t0 = fn_8015F7C4(x0);
    return t0;
}

f32 fabsf(f32 x0) {
    f32 t0;
    t0 = fabs(x0);
    return t0;
}

// 0x8000AE94: the absolute value of a double (fabsf, which rounds its result to a float, is
// the float version).
double fabs(double x) {
    return __fabs(x);
}

void fn_8000AF1C(void) {
}

void fn_8000AF20(void) {
    s32 t1;
    fn_8000AF1C();
    t1 = fn_800951A0(4096, 16, 1);
    lbl_80281BD8 = t1;
    fn_8000AE9C();
}

void fn_8000AF58(void) {
    fn_8009527C(lbl_80281BD8);
}

f32 fn_8000AF7C(f32 x0) {
    f32 t0;
    t0 = fn_8015F804(x0);
    return t0;
}

// ---- end of sweep code ----
