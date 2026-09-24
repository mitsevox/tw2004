// GoGrass.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "grassshader.h"
#include "endian.h"
#include "gx.h"

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
void fn_8011EBF8(void);
void fn_8011EC2C(void);
extern u8 lbl_801945E8[];
extern u8 lbl_801945F8[];
extern u8 lbl_80194618[];
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
void fn_8011FDC4(GrassBuffer* pBuffer);
void fn_8011E3B4(void);
void fn_80008380(void);
void fn_8011E468(void);
void fn_8011E4D8(GrassChunk* pChunk);
void fn_8011EB80(void);
void fn_8011E584(UStreamObject* pObject);
int  fn_8011E6B0(f32** ppA, f32** ppB);
void fn_8011FD74(GrassBuffer* pBuffer);
GrassBuffer* fn_8011FDEC(s32 nSize);
void fn_8011FF58(void);
void fn_80120194(void);
void fn_80008248(void* p);

void fn_8011E3B0(void) {
}

// The grass goes: the objects of the list in use are pushed and the stack emptied into the slots,
// then everything is freed.
void fn_8011E3B4(void) {
    int i;
    if (lbl_80281900->p370 != NULL) {
        fn_80008380();
        for (i = 0; i < lbl_80281900->anF8[lbl_80281900->n100]; i++) {
            fn_8011FDC4(lbl_80281900->apF0[lbl_80281900->n100][i]);
        }
        fn_8011FF58();
        fn_80009E70(lbl_80281900->p370);
        lbl_80281900->p370 = NULL;
        fn_80120194();
        fn_8011EAB8();
        lbl_80282514 = 0;
    }
    lbl_80281900->n1C = 0;
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

// Adds a grass data chunk: its data (after its n2 0x30-byte entries) is byte-swapped in place.
void fn_8011E4D8(GrassChunk* pChunk) {
    u8* pData;
    lbl_80281900->a48[lbl_80281900->n1C] = pChunk;
    lbl_80281900->a20[lbl_80281900->n1C] = (GrassChunkData*)((u8*)(pChunk + 1) + pChunk->n2 * 0x30);
    pData = (u8*)lbl_80281900->a20[lbl_80281900->n1C];
    fn_80076158(&pData, pData, 8, 4);
    lbl_80281900->f3B8 = lbl_80281900->a20[0]->f4;
    lbl_80281900->n3A4 = lbl_80281900->a20[0]->n0;
    lbl_80281900->n1C = lbl_80281900->n1C + 1;
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
    if (lbl_80281900->p370 != NULL && lbl_80281900->n3E0 != 0 && fn_800C6CB0() == 0) {
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

// The grass's 256x256 texture: its buffer and texture object.
void fn_8011EB80(void) {
    lbl_80282510 = fn_80009B34(GXGetTexBufferSize(256, 256, 4, 0, 0), 2, 32, "GoGrass.c", 1311);
    GXInitTexObj(&lbl_8026038C, lbl_80282510, 256, 256, 4, 0, 0, 0);
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
    GXLoadTexObj(&lbl_8026038C, 0);
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

// Puts pBuffer in the first free one of the 16 apDC slots.
void fn_8011FD74(GrassBuffer* pBuffer) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_80281900->apDC[i] == NULL) {
            lbl_80281900->apDC[i] = pBuffer;
            lbl_80281900->nE8 = lbl_80281900->nE8 + 1;
            return;
        }
    }
}

// Pushes pBuffer on the apD8 stack.
void fn_8011FDC4(GrassBuffer* pBuffer) {
    lbl_80281900->apD8[lbl_80281900->nE4] = pBuffer;
    lbl_80281900->nE4 = lbl_80281900->nE4 + 1;
}

// Takes the smallest free buffer of at least nSize out of apDC (NULL if there is none).
GrassBuffer* fn_8011FDEC(s32 nSize) {
    int nBest = -1;
    GrassBuffer* pBest = NULL;
    GrassBuffer* pBuffer;
    int i;

    if (lbl_80281900->nE8 == 0) {
        return NULL;
    }
    for (i = 0; i < 16; i++) {
        pBuffer = lbl_80281900->apDC[i];
        if (pBuffer != NULL && pBuffer->n44 >= nSize && (pBest == NULL || pBuffer->n44 < pBest->n44)) {
            nBest = i;
            pBest = pBuffer;
        }
    }
    if (pBest != NULL) {
        lbl_80281900->nE8 = lbl_80281900->nE8 - 1;
        lbl_80281900->apDC[nBest] = NULL;
        return pBest;
    }
    return NULL;
}

// Empties the apD8 stack: each buffer gets fn_80008248 on its a14 and goes to a free apDC slot.
void fn_8011FF58(void) {
    while (lbl_80281900->nE4 != 0) {
        fn_80008248(lbl_80281900->apD8[lbl_80281900->nE4 - 1]->a14);
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
    fn_80009E70(lbl_80281900->apF0[0]);
    fn_80009E70(lbl_80281900->apF0[1]);
    fn_80009E70(lbl_80281900->pEC);
    fn_80009E70(lbl_80281900->apDC);
    fn_80009E70(lbl_80281900->apD8);
}

s32 fn_8012022C(void);
f32 fn_80120244(f32 fX, f32 fM);
s32 fn_8012028C(u8* p0);

s32 fn_8012022C(void) {
    return lbl_80281900->p370 != NULL;
}

// fmod for floats: the remainder of fX / fM (both callers pass the modulus in the second argument).
f32 fn_80120244(f32 fX, f32 fM) {
    return fmod(fX, fM);
}

s32 fn_8012028C(u8* p0) {
    return (*(s32*)p0 + 88);
}

// ---- end of sweep code ----
