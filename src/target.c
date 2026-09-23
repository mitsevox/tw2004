// target.c (TW06's target.c, golf/ai/target.c): where the CPU aims and where a ball may be
// placed or dropped (TW06's PlaceBall_* functions). Mostly not yet decompiled; the machine-style
// code below is the sweep's.

#include "game_types.h"

s32 fn_8000C140();
s32 Ter_iNumOOBNetworksLoaded();
s32 Ter_PointInOOBNetwork();
extern void* lbl_80281E30;
s32 fn_80069428(s32 arg0);
void* fn_80069498(void);
void fn_800E58B4();
void fn_8006A89C(void);
f32 fn_8006A8A8(u8* p);
void fn_8006A8B0(void);
f32 fn_80012ED0();
f32 fn_80012ED8();
f32 fn_80012EE0();
f32 fn_80012EE8();
s32 fn_80012EF0();
void fn_8006A8D4(s32 p0, u8* p1, u8* p2);

s32 fn_80069428(s32 arg0) {
    if ((void* ) lbl_80281E30 != NULL) {
        if (fn_8000C140(arg0, (void* ) lbl_80281E30, (*(s16*)((u8*)(lbl_80281E30) + 2))) != 0) {
            return 1;
        }
        if (Ter_iNumOOBNetworksLoaded() > 0) {
            return Ter_PointInOOBNetwork(arg0);
        }
        return 0;
    }
    return Ter_PointInOOBNetwork();
}

void* fn_80069498(void) {
    return lbl_80281E30;
}

void fn_8006A89C(void) {
    lbl_80281E30 = 0;
}

f32 fn_8006A8A8(u8* p) {
    return *(f32*)(p + 0x0);
}

void fn_8006A8B0(void) {
    fn_800E58B4(32);
}

void fn_8006A8D4(s32 p0, u8* p1, u8* p2) {
    s32 t0;
    f32 t1;
    f32 t2;
    f32 t3;
    f32 t4;
    t0 = fn_80012EF0();
    t1 = fn_80012EE8();
    t2 = fn_80012ED8(t0);
    *(f32*)p1 = (*(f32*)p1 * t2 + t1);
    t3 = fn_80012EE0(t0);
    t4 = fn_80012ED0(t0);
    *(f32*)p2 = (*(f32*)p2 * t4 + t3);
}
