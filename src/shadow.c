// shadow.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's shadow,
// drawn into a 256x256 texture (the state at lbl_802814A8). Not yet decompiled; the sweep code
// below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 GXGetTexBufferSize(s32, s32, s32, s32, s32);
s32 fn_8002A624();
extern void* lbl_802814A8;
s32 fn_800B2360();
s32 GXCopyTex(s32, s32);
s32 GXPixModeSync();
s32 GXSetTexCopyDst(s32, s32, s32, s32);
s32 GXSetTexCopySrc(s32, s32, s32, s32);
void fn_80076A54();
s32 fn_8000A0E8(s32, s32);
s32 fn_8000A798(s32, s32);
s32 fn_8000ADC0(s32);
s32 fn_8001728C(s32);
void fn_800B2314(void);
void fn_800B235C(void);
void fn_800B2438(u8 arg0);
void fn_800B246C(void);
void fn_800B2470(void);
void fn_800B2724(void);
void fn_800B3438(u8* p0, f32 x0, f32 x1);
void fn_800B3484(s32 arg0, s32 arg1);

void fn_800B2314(void) {
    void* temp_r31;

    temp_r31 = lbl_802814A8;
    GXGetTexBufferSize(0x100, 0x100, 9, 0, 0);
    (*(s32*)((u8*)(temp_r31) + 0x400)) = fn_8002A624();
}

void fn_800B235C(void) {
}

void fn_800B2438(u8 arg0) {
    u8 var_r3;

    var_r3 = 0x20;
    if (arg0 != 0) {
        var_r3 = 0x30;
    }
    fn_800B2360(var_r3);
}

void fn_800B246C(void) {
}

void fn_800B2470(void) {
    void* temp_r31;

    temp_r31 = lbl_802814A8;
    GXSetTexCopySrc(0, 0, 0x100, 0x100);
    GXSetTexCopyDst(0x100, 0x100, 0x27, 0);
    GXCopyTex((*(s32*)((u8*)(temp_r31) + 0x400)), 0);
    GXPixModeSync();
}

void fn_800B2724(void) {
    *(s32*)(((u8*)lbl_802814A8) + 0x4F0) = 0;
}

void fn_800B3438(u8* p0, f32 x0, f32 x1) {
    *(f32*)(p0 + 0x10) = x0;
    *(f32*)(p0 + 0x14) = x1;
    fn_80076A54();
}

void fn_800B3484(s32 arg0, s32 arg1) {
    if (arg1 == 0) {
        fn_8000ADC0(arg0 + 4);
        fn_8000ADC0(arg0 + 0x44);
    } else {
        fn_8000A0E8(arg1, arg0 + 0x44);
        fn_8000A798(arg1, arg0 + 4);
    }
    fn_8001728C(arg0);
}

// ---- end of sweep code ----
