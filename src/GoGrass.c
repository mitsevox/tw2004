// GoGrass.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011E3B0(void);
s32 UStream_UnregisterHandler(s32);
u8 fn_80112B80();
void fn_8011E4A4(void);
s32 fn_800C6CB0();
void fn_8011EE4C(void);
void fn_8011EF88(void);
void fn_8011F374(void);
s32 fn_8011F3AC();
extern s32 lbl_80281900;
void fn_8011E974(void);
void fn_800137B0();
void fn_8006E214();
void fn_8007644C();
void fn_80076B18();
void fn_8011EAB8(void);
s32 fn_80009E70();
extern u32 lbl_80282510;
void fn_8011EBF8(void);
void GXCopyTex();
void GXInvalidateTexAll();
void GXPixModeSync();
void GXSetTexCopyDst();
void GXSetTexCopySrc();
void fn_8011EC2C(void);
extern u8 lbl_801945E8[];
extern u8 lbl_801945F8[];
extern u8 lbl_80194618[];
extern u8 lbl_8026038C[];
void GXLoadTexObj();
void GXSetNumTevStages();
void GXSetNumTexGens();
void GXSetTevAlphaIn();
void GXSetTevAlphaOp();
void GXSetTevColorIn();
void GXSetTevColorOp();
void GXSetTevOrder();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();
void fn_80014118();
void fn_80014194();
void fn_8001425C();
void fn_8001644C();
void fn_80013CCC();
void fn_8001614C();
void fn_80016B9C();
void fn_80035138();
void fn_800352BC();
void GrassRender_vBuildAndUploadOneTimeData();
void fn_8011FDC4(s32 p0);

void fn_8011E3B0(void) {
}

void fn_8011E4A4(void) {
    if (fn_80112B80() != 0) {
        UStream_UnregisterHandler(0x67726173);
    }
}

void fn_8011E974(void) {
    if (((u32) (*(u32*)((u8*)(lbl_80281900) + 0x370)) != 0U) && ((s32) (*(s32*)((u8*)(lbl_80281900) + 0x3E0)) != 0) && (fn_800C6CB0() == 0)) {
        fn_8011EF88();
        fn_8011F3AC();
        fn_8011F374();
        if ((s32) (*(s32*)((u8*)(lbl_80281900) + 0x3CC)) != 0) {
            fn_8011EE4C();
        }
    }
}

void fn_8011EAB8(void) {
    fn_8006E214(*(s32*)(((u8*)lbl_80281900) + 0x78));
    fn_8007644C(*(s32*)(((u8*)lbl_80281900) + 0x74));
    fn_80076B18(*(s32*)(((u8*)lbl_80281900) + 0x7C));
    fn_800137B0(*(s32*)(((u8*)lbl_80281900) + 0x70));
}

void fn_8011EBF8(void) {
    if ((u32) lbl_80282510 != 0U) {
        fn_80009E70(lbl_80282510);
    }
    lbl_80282510 = 0U;
}

void fn_8011EC2C(void) {
    GXSetTexCopySrc(0, 0, 256, 256);
    GXSetTexCopyDst(256, 256, 4, 0);
    GXCopyTex(lbl_80282510, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}

void fn_8011EE4C(void) {
    fn_8001425C(0);
    fn_80014118(16);
    fn_80014194(lbl_801945E8);
    fn_80012F34(0);
    fn_80012F50(0, 6, 128);
    GXLoadTexObj(lbl_8026038C, 0);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetNumTevStages(1);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 1, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    fn_80012F18(7);
    fn_80012EF8();
    fn_8001644C(161, lbl_801945F8, 0, lbl_80194618, 2);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

void fn_8011EF88(void) {
    fn_800352BC();
    fn_8001614C();
    fn_80013CCC();
    fn_80035138(0);
    fn_80016B9C();
    fn_80012F50(0, 6, 128);
    fn_80012F34(0);
    fn_80014118(80);
    fn_80012EF8();
    GrassRender_vBuildAndUploadOneTimeData();
}

void fn_8011F374(void) {
    fn_80012F50(1, 6, 128);
    fn_80012F34(1);
    fn_80012EF8();
}

void fn_8011FDC4(s32 p0) {
    *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281900) + 0xD8)) + (*(s32*)(((u8*)lbl_80281900) + 0xE4) << 2)) = p0;
    *(s32*)(((u8*)lbl_80281900) + 0xE4) = (*(s32*)(((u8*)lbl_80281900) + 0xE4) + 1);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

s32 fn_8012022C(void);
double fn_8015F7E4();
f32 fn_80120244(f32 x0);
s32 fn_8012028C(u8* p0);

s32 fn_8012022C(void) {
    return ((u32)((-*(s32*)(((u8*)lbl_80281900) + 0x370)) | *(s32*)(((u8*)lbl_80281900) + 0x370)) >> 31);
}

f32 fn_80120244(f32 x0) {
    f32 t0;
    t0 = fn_8015F7E4(x0);
    return t0;
}

s32 fn_8012028C(u8* p0) {
    return (*(s32*)p0 + 88);
}

// ---- end of sweep code ----
