// GoGrass.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "grassshader.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011E3B0(void);
u8 fn_80112B80();
void fn_8011E4A4(void);
s32 fn_800C6CB0();
void fn_8011EE4C(void);
void fn_8011EF88(void);
void fn_8011F374(void);
s32 fn_8011F3AC();
void fn_8011E974(void);
void fn_800137B0();
void fn_8006E214();
void fn_8007644C();
void fn_80076B18();
void fn_8011EAB8(void);
extern void* lbl_80282510;
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
void fn_8011FDC4(void* pObject);
void fn_8011E468(void);
void fn_8011E584(UStreamObject* pObject);
int  fn_8011E6B0(f32** ppA, f32** ppB);
void fn_8011FD74(void* pObject);
void fn_8011FF58(void);
void fn_80120194(void);
void fn_80008248(void* p);

void fn_8011E3B0(void) {
}

// The grass's stream handler ('gras') is registered when fn_80112B80 allows it.
void fn_8011E468(void) {
    if (fn_80112B80() != 0) {
        UStream_RegisterHandler('gras', fn_8011E584);
    }
}

void fn_8011E4A4(void) {
    if (fn_80112B80() != 0) {
        UStream_UnregisterHandler(0x67726173);
    }
}

// A sort order: by the float at +8 of the objects the two entries point to, the larger first
// (equal gives -1).
int fn_8011E6B0(f32** ppA, f32** ppB) {
    f32 fA = (*ppA)[2];
    f32 fB = (*ppB)[2];
    if (fA < fB) {
        return 1;
    }
    if (fA >= fB) {
        return -1;
    }
    return 0;
}

void fn_8011E974(void) {
    if (lbl_80281900->u370 != 0 && lbl_80281900->n3E0 != 0 && fn_800C6CB0() == 0) {
        fn_8011EF88();
        fn_8011F3AC();
        fn_8011F374();
        if (lbl_80281900->n3CC != 0) {
            fn_8011EE4C();
        }
    }
}

void fn_8011EAB8(void) {
    fn_8006E214(lbl_80281900->p78);
    fn_8007644C(lbl_80281900->p74);
    fn_80076B18(lbl_80281900->p7C);
    fn_800137B0(lbl_80281900->p70);
}

void fn_8011EBF8(void) {
    if (lbl_80282510 != NULL) {
        fn_80009E70(lbl_80282510);
    }
    lbl_80282510 = NULL;
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

// Puts pObject in the first free one of the 16 apDC slots.
void fn_8011FD74(void* pObject) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_80281900->apDC[i] == NULL) {
            lbl_80281900->apDC[i] = pObject;
            lbl_80281900->nE8 = lbl_80281900->nE8 + 1;
            return;
        }
    }
}

// Pushes pObject on the apD8 stack.
void fn_8011FDC4(void* pObject) {
    lbl_80281900->apD8[lbl_80281900->nE4] = pObject;
    lbl_80281900->nE4 = lbl_80281900->nE4 + 1;
}

// Empties the apD8 stack: each object gets fn_80008248 on its +0x14 and goes to a free apDC slot.
void fn_8011FF58(void) {
    while (lbl_80281900->nE4 != 0) {
        fn_80008248((u8*)lbl_80281900->apD8[lbl_80281900->nE4 - 1] + 0x14);
        fn_8011FD74(lbl_80281900->apD8[lbl_80281900->nE4 - 1]);
        lbl_80281900->nE4 = lbl_80281900->nE4 - 1;
    }
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

// Frees the grass's allocations: each record's p40, then the tables.
void fn_80120194(void) {
    int i;
    for (i = 0; i < lbl_80281900->nE0; i++) {
        fn_80009E70(lbl_80281900->pEC[i].p40);
    }
    fn_80009E70(lbl_80281900->pF0);
    fn_80009E70(lbl_80281900->pF4);
    fn_80009E70(lbl_80281900->pEC);
    fn_80009E70(lbl_80281900->apDC);
    fn_80009E70(lbl_80281900->apD8);
}

s32 fn_8012022C(void);
f32 fn_80120244(f32 fX, f32 fM);
s32 fn_8012028C(u8* p0);

s32 fn_8012022C(void) {
    return lbl_80281900->u370 != 0;
}

// fmod for floats: the remainder of fX / fM (both callers pass the modulus in the second argument).
f32 fn_80120244(f32 fX, f32 fM) {
    return fmod(fX, fM);
}

s32 fn_8012028C(u8* p0) {
    return (*(s32*)p0 + 88);
}

// ---- end of sweep code ----
