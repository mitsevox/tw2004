// GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80009E70();
void fn_800137B0(void* pCamera);    // camera.h
void fn_80012EF8();
void fn_80013EA0();
void fn_80013EEC();
void fn_800137D0(s32 p0);
s32 fn_8000A0E8();
s32 fn_8000A714();
s32 fn_800BADF8();
void fn_80013CCC(u8* arg0);
extern u8* lbl_80280DF0;
void fn_80013D58(s32 p0);
void fn_80013D5C(s32 v);
void RC_vUpdateRenderCtxScreenMatricesAndInfo();
void fn_80013DD0(u8* arg0, u32 arg1);
void fn_80013D68(s32 p0);
void fn_80013D9C(u8* p0, u32 p1);
s32 fn_8000ADC0();
void fn_80013E28(u8* p, s32 v);
void fn_80013E30(u8* p, s32 v);
void fn_80013E38(u8* p, s32 v);
s32 fn_80013E40(u8* p);
f32 fn_80014134(u8* p);
f32 fn_8001413C(u8* p);
f32 fn_80014144(u8* p);
f32 fn_8001414C(u8* p);
f32 fn_80014154(u8* p);
f32 fn_8001415C(u8* p);
f32 fn_80014164(u8* p);
f32 fn_8001416C(u8* p);
f32 fn_80014174(u8* p);
f32 fn_8001417C(u8* p);
f32 fn_80014184(u8* p);
f32 fn_8001418C(u8* p);

void fn_800137B0(void* pCamera) {
    fn_80009E70(pCamera);
}

void fn_800137D0(s32 p0) {
    fn_80013EEC();
    fn_80012EF8();
    fn_80013EA0(p0);
}

void fn_80013CCC(u8* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0x1DC)) != 0) {
        fn_8000A0E8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x11C);
        fn_8000A0E8(arg0 + 0xDC, arg0 + 0x19C);
    } else {
        fn_800BADF8((*(s32*)((u8*)(arg0) + 0x10)) + 0x44, arg0 + 0x1C, arg0 + 0x11C, 4);
        fn_800BADF8(arg0 + 0xDC, arg0 + 0x1C, arg0 + 0x19C, 4);
    }
    fn_8000A714(arg0 + 0x11C, arg0 + 0x15C);
}

void fn_80013D58(s32 p0) {
}

void fn_80013D5C(s32 v) {
    *(s32*)(lbl_80280DF0 + 0x0) = v;
}

void fn_80013D68(s32 p0) {
    RC_vUpdateRenderCtxScreenMatricesAndInfo();
    fn_80013D58(p0);
}

void fn_80013D9C(u8* p0, u32 p1) {
    fn_80013DD0(p0, p1);
    fn_80013CCC(p0);
}

void fn_80013DD0(u8* arg0, u32 arg1) {
    if (arg1 == 0U) {
        fn_8000ADC0(arg0 + 0x1C);
        (*(s8*)((u8*)(arg0) + 0x1DC)) = 1;
        return;
    }
    fn_8000A0E8(arg1, arg0 + 0x1C);
    (*(s8*)((u8*)(arg0) + 0x1DC)) = 0;
}

void fn_80013E28(u8* p, s32 v) {
    *(s32*)(p + 0x14) = v;
}

void fn_80013E30(u8* p, s32 v) {
    *(s32*)(p + 0x18) = v;
}

void fn_80013E38(u8* p, s32 v) {
    *(s32*)(p + 0x10) = v;
}

s32 fn_80013E40(u8* p) {
    return *(s32*)(p + 0x18);
}

f32 fn_80014134(u8* p) {
    return *(f32*)(p + 0x14);
}

f32 fn_8001413C(u8* p) {
    return *(f32*)(p + 0x10);
}

f32 fn_80014144(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_8001414C(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_80014154(u8* p) {
    return *(f32*)(p + 0x28);
}

f32 fn_8001415C(u8* p) {
    return *(f32*)(p + 0xC);
}

f32 fn_80014164(u8* p) {
    return *(f32*)(p + 0x4);
}

f32 fn_8001416C(u8* p) {
    return *(f32*)(p + 0x8);
}

f32 fn_80014174(u8* p) {
    return *(f32*)(p + 0x0);
}

f32 fn_8001417C(u8* p) {
    return *(f32*)(p + 0x2C);
}

f32 fn_80014184(u8* p) {
    return *(f32*)(p + 0x30);
}

f32 fn_8001418C(u8* p) {
    return *(f32*)(p + 0x28);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

extern u8* lbl_80280E08;
void fn_8001425C(s32 v);
f32 fn_80014268(u8* p);
f32 fn_80014270(u8* p);
f32 fn_80014278(u8* p);
double tan();
f32 fn_80014280(f32 x0);
extern u8 lbl_80281C98;
void fn_800142A4(s8 v);
void fn_800131C4(int nController);
void fn_8001437C(void);

void fn_8001425C(s32 v) {
    *(s32*)(lbl_80280E08 + 0xD0) = v;
}

f32 fn_80014268(u8* p) {
    return *(f32*)(p + 0xAC);
}

f32 fn_80014270(u8* p) {
    return *(f32*)(p + 0xA8);
}

f32 fn_80014278(u8* p) {
    return *(f32*)(p + 0xA4);
}

f32 fn_80014280(f32 x0) {
    f32 t0;
    t0 = tan(x0);
    return t0;
}

void fn_800142A4(s8 v) {
    lbl_80281C98 = v;
}

void fn_8001437C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_800131C4(var_r31);
        var_r31 += 1;
    } while (var_r31 < 4);
}

// ---- end of sweep code ----
