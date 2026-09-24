// GoGrass.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "grassshader.h"
#include "golfer.h"
#include "endian.h"
#include "gx.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011E170(void);
void fn_8011E6E8(void);
void fn_8011EC84(void);
void fn_8011F7F8(void);
void fn_8011E3B0(void);
u8 fn_80112B80();
void fn_8011E4A4(void);
void fn_8011EE4C(void);
void fn_8011EF88(void);
void fn_8011F374(void);
void fn_8011F3AC(void);
void fn_800082CC(void* p);
void fn_8003519C(int nRow, void* pData);   // GoTerrain.c: calls row nRow's function with pData
void fn_8011E974(void);
void fn_8011EAB8(void);
void fn_8011EBF8(void);
void fn_8011EC2C(void);
extern f32 lbl_801945B8[];     // a colour: 0.21, 0.31, 0.1, 1
extern f32 lbl_801945C8[];     // the unit square's corners
void fn_80013D5C(void* pCamera);   // makes it the current render camera
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_800760B0(int nX, int nY, int nWidth, int nHeight);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_80035F1C(void);
void fn_80016948(void);
s32  fn_8003505C(s32 n);           // sets a value, returns the old one
void fn_80034AE4(void);
extern f32 lbl_801945E8[];
extern f32 lbl_801945F8[];
extern f32 lbl_80194618[];
void fn_80035138();
void fn_800352BC();
void GrassRender_vBuildAndUploadOneTimeData();
void fn_8011FDC4(GrassBuffer* pBuffer);
void fn_8011E3B4(void);
void fn_80008380(void);
void fn_8011E468(void);
void fn_8011E4D8(GrassChunk* pChunk);
void fn_8011EB80(void);
void fn_8011E9D8(void);
void fn_8011EB04(void);
s32 fn_8011FFCC(void);
void fn_8011E584(UStreamObject* pObject);
int  fn_8011E6B0(f32** ppA, f32** ppB);
void fn_8011FD74(GrassBuffer* pBuffer);
GrassBuffer* fn_8011FDEC(s32 nSize);
void fn_8011FF58(void);
void fn_80120194(void);
void fn_80008248(void* p);

// The grass's start: its chunk loader (chunk 6) is registered and its settings get their defaults.
void fn_8011E170(void) {
    lbl_80281900->p370 = NULL;
    lbl_80281900->n1C = 0;
    // port: fn_8011E4D8 takes the chunk as a GrassChunk*
    Course_RegisterLoader(6, (void (*)(u8*))fn_8011E4D8);
    lbl_80281900->n3DC = 1;
    lbl_80281900->n3E0 = 1;
    lbl_80281900->n3F0 = 1;
    lbl_80281900->f3E8 = 4.9f;
    lbl_80281900->f3D0 = 128.0f;
    lbl_80281900->f3BC = 128.0f;
    lbl_80281900->f3C0 = 128.0f;
    lbl_80281900->f3C4 = 1.5f;
    lbl_80281900->f3C8 = 2.6f;
    lbl_80281900->nC0 = 10;
    lbl_80281900->f3D4 = 2.5914f;
    lbl_80281900->f3D8 = 2.5914f;
    lbl_80281900->f3A8 = 2.5f;
    lbl_80281900->f3AC = 2.5f;
    lbl_80281900->f3B0 = 500.0f;
    lbl_80281900->f3B4 = 0.6f;
    lbl_80281900->f3B8 = 0.13f;
    lbl_80281900->n3CC = 0;
    lbl_80281900->n10 = 0;
    lbl_80281900->n104 = 0;
    lbl_80281900->n100 = 0;
    lbl_80281900->f3EC = 0.0f;
    lbl_80281900->f3FC = 0.0f;
    lbl_80281900->f400 = -0.36f;
    lbl_80281900->n3E4 = 1;
    lbl_80281900->f40C = 0.78f;
    lbl_80281900->f410 = 0.16f;
    lbl_80281900->f414 = 0.0f;
    lbl_80281900->f418 = 0.025f;
    lbl_80281900->f41C = 0.025f;
    lbl_80281900->n3A4 = 0;
    lbl_80281900->n14 = -500;
    lbl_80281900->n16 = -500;
    lbl_80281900->n18 = 1000;
    lbl_80281900->n1A = 1000;
    lbl_80281900->af168[0] = 1.19f;
    lbl_80281900->af168[1] = 0.168f;
    lbl_80281900->af168[2] = 0.49f;
    lbl_80281900->af168[3] = 0.308f;
    lbl_80281900->af168[4] = 0.981f;
    lbl_80281900->af168[5] = 0.06f;
    lbl_80281900->af168[6] = 0.685f;
    lbl_80281900->af168[7] = 0.272f;
    fn_8002A528(&lbl_80281900->tex80, 256, 256, fn_8002A624(), NULL, 6, 0, 0, 0);
    lbl_80282514 = 0;
}

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

// The 'gras' stream handler: the hole's grass file is loaded. Its header gives the grid (defaults
// for a file before version 100); the records' two offsets become addresses.
void fn_8011E584(UStreamObject* pObject) {
    u8* pCur;
    u8* pBase;
    s32 nSkip;
    s32 n;
    int i;

    lbl_80281900->p370 = pObject;
    pCur = pObject->pData;
    fn_8011FFCC();
    fn_8011E9D8();
    nSkip = *(s32*)pCur;
    pCur += 0x10;
    pBase = pCur;
    pCur += nSkip;
    if (((GrassFileHeader*)pCur)->uVersion == 100) {
        lbl_80281900->n14 = ((GrassFileHeader*)pCur)->n6;
        lbl_80281900->n16 = ((GrassFileHeader*)pCur)->n8;
        lbl_80281900->n18 = ((GrassFileHeader*)pCur)->nA;
    } else {
        lbl_80281900->n14 = -500;
        lbl_80281900->n16 = -500;
        lbl_80281900->n18 = 400;
    }
    n = ((GrassFileHeader*)pCur)->n0;
    pCur += sizeof(GrassFileHeader);
    lbl_80281900->n1A = n / lbl_80281900->n18;
    lbl_80281900->p8 = (s16*)pCur;
    pCur += n * 2;
    lbl_80281900->n10 = *(s32*)pCur;
    lbl_80281900->pC = (GrassTile*)(pCur + 0x10);
    // port: the file's offsets are made into 32-bit addresses in place
    for (i = 0; i < lbl_80281900->n10; i++) {
        lbl_80281900->pC[i].u10 += (u32)pBase;
        lbl_80281900->pC[i].u14 += (u32)pBase;
    }
}

// The grass's frame update (after one skipped frame): new random tuning values when f3D4 changed,
// the buffers and the grass camera, the texture pass, then the 16 sway points around the circle
// and the phase moved on by f418 per 60th of a second.
void fn_8011E6E8(void) {
    f32 vPoint[4];
    int i;
    f32 fSin;
    f32 fCos;

    if (lbl_80281900->p370 == NULL) {
        return;
    }
    if (lbl_80281900->n3DC == 0) {
        return;
    }
    if (lbl_80282514 == 0) {
        lbl_80282514 = 1;
        return;
    }
    // port: as in fn_8011E974, EA's GoGrass.c saw fn_800C6CB0 as returning int
    if (((int (*)(void))fn_800C6CB0)() != 0) {
        return;
    }
    if (lbl_80281900->f3D4 != lbl_80281900->f3D8) {
        for (i = 0; i < 8; i++) {
            lbl_80281900->af168[i] = lbl_80281900->f3D4 * Rand_Float(1);
        }
        lbl_80281900->f3D8 = lbl_80281900->f3D4;
    }
    fn_8011FF58();
    fn_8011F7F8();
    fn_8011EB04();
    if (lbl_80281900->n3E4 != 0) {
        fn_8011EC84();
    }
    fSin = fn_800095F0(lbl_80281900->f40C);
    fCos = fn_80009638(lbl_80281900->f40C);
    for (i = 0; i < 16; i++) {
        vPoint[0] = fCos * lbl_80281900->f41C *
                    fn_800095F0(lbl_80281900->f414 + 2.0f * PI * ((f32)i / 16.0f));
        vPoint[1] = 0.0f;
        vPoint[2] = fSin * lbl_80281900->f41C *
                    fn_800095F0(lbl_80281900->f414 + 2.0f * PI * ((f32)i / 16.0f));
        vPoint[3] = 0.0f;
        Vec_Copy(vPoint, lbl_80281900->av230[i]);
    }
    lbl_80281900->f228 = 16.0f * lbl_80281900->f410 * fCos;
    lbl_80281900->f22C = 16.0f * lbl_80281900->f410 * fSin;
    lbl_80281900->f414 = lbl_80281900->f414 + 60.0f * (lbl_80281900->f418 * gSession.fFrameTime);
    if (lbl_80281900->f414 > 2.0f * PI) {
        lbl_80281900->f414 = lbl_80281900->f414 - 2.0f * PI;
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
    // port: EA's GoGrass.c saw fn_800C6CB0 as returning int (its result is not masked here); it
    // returns u8
    if (lbl_80281900->p370 != NULL && lbl_80281900->n3E0 != 0 && ((int (*)(void))fn_800C6CB0)() == 0) {
        fn_8011EF88();
        fn_8011F3AC();
        fn_8011F374();
        if (lbl_80281900->n3CC != 0) {
            fn_8011EE4C();
        }
    }
}

// The grass's render camera: a flat 20 x 20 lens drawing into a 256 x 256 frame buffer.
void fn_8011E9D8(void) {
    lbl_80281900->pLens = fn_80076400();
    lbl_80281900->pFrameBuf = fn_8006E1C8();
    lbl_80281900->pRect = fn_80076ACC();
    fn_8006E26C(lbl_80281900->pFrameBuf, 0.0f, 0.0f, 256.0f, 256.0f, 1.0f, 1.0f);
    fn_800171D8(lbl_80281900->pRect, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800B3438(lbl_80281900->pRect, 1.0f, 1.0f);
    fn_800768E0(lbl_80281900->pLens);
    fn_80076A0C(lbl_80281900->pLens, 1);
    fn_80076948(lbl_80281900->pLens, 20.0f, 20.0f);
    lbl_80281900->pCamera =
        fn_8001371C(lbl_80281900->pLens, lbl_80281900->pFrameBuf, lbl_80281900->pRect);
}

void fn_8011EAB8(void) {
    fn_8006E214(lbl_80281900->pFrameBuf);
    fn_8007644C(lbl_80281900->pLens);
    fn_80076B18(lbl_80281900->pRect);
    fn_800137B0(lbl_80281900->pCamera);
}

// Points the grass lens straight down from f3B0 over vB0, offset by half its view size.
void fn_8011EB04(void) {
    f32 aEye[4];
    f32 aAt[4];
    f32 fX;
    f32 fZ;

    fn_8001F004();
    fX = 0.5f * lbl_80281900->pLens->fB4 + lbl_80281900->vB0[0];
    fZ = 0.5f * lbl_80281900->pLens->fB8 + lbl_80281900->vB0[2];
    aEye[0] = fX;
    aEye[1] = lbl_80281900->f3B0;
    aEye[2] = fZ;
    aEye[3] = 1.0f;
    aAt[0] = fX;
    aAt[1] = lbl_80281900->f3B0 - 1.0f;
    aAt[2] = fZ;
    aAt[3] = 1.0f;
    fn_8007646C(lbl_80281900->pLens, aEye, aAt);
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

// Renders the grass texture: with the grass camera current, a 256 x 256 viewport is drawn and
// copied out (fn_8011EC2C), then the previous camera and the full 512 x 448 screen come back.
void fn_8011EC84(void) {
    s32 nOld;
    void* pCamera;

    pCamera = fn_8001614C();
    fn_80013D5C(lbl_80281900->pCamera);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012F50(0, 6, 128);
    fn_800140E8(1, 256, 256, 0, 1, 1);
    fn_800760B0(0, 0, 256, 256);
    fn_80016B54(256, 256, 1.0f, 1.0f);
    fn_80035F1C();
    fn_80016948();
    fn_800352BC();
    fn_80013CCC(fn_8001614C());
    fn_80013EEC(fn_8001614C());
    fn_80016B9C();
    fn_80012EF8();
    fn_8001425C(0);
    fn_80014118(0);
    fn_80014194(lbl_801945B8);
    fn_80012F34(0);
    fn_80012F50(0, 6, 128);
    fn_80012EF8();
    fn_8001644C(161, lbl_801945C8, 0, lbl_801945C8, 2);
    nOld = fn_8003505C(0);
    fn_80034AE4();
    fn_8003505C(nOld);
    fn_8011EC2C();
    fn_80013D5C(pCamera);
    fn_80013EEC(fn_8001614C());
    fn_800352BC();
    fn_80013CCC(fn_8001614C());
    fn_80016B9C();
    fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_800760B0(0, 0, 512, 448);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80035F1C();
    fn_80012F50(1, 6, 128);
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012EF8();
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
    fn_80013CCC(fn_8001614C());
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

// Draws the buffers in use in two passes: the camera's direction, flattened and normalised, picks
// the quadrant; each buffer's position goes to fn_8003519C row 17, then the buffer is drawn.
void fn_8011F3AC(void) {
    f32 vDir[4];
    s32 nPass;
    int i;
    GrassBuffer* pBuffer;
    s32 nBuffers;
    CamLens* pLens = fn_8001F004();

    nBuffers = lbl_80281900->anF8[lbl_80281900->n100];
    Vec_Copy(pLens->v24, vDir);
    vDir[1] = 0.0f;
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        fn_800BAF04(vDir, vDir);
    }
    if (vDir[2] < 0.0f) {
        lbl_80281900->n360 = 0;
    } else {
        lbl_80281900->n360 = 1;
    }
    if (vDir[0] < 0.0f) {
        lbl_80281900->n364 = 0;
    } else {
        lbl_80281900->n364 = 1;
    }
    lbl_80281900->f358 = fabs(vDir[2]);
    lbl_80281900->f35C = fabs(vDir[0]);
    lbl_80281900->f354 = lbl_80281900->f3B4;
    lbl_80281900->f348 = lbl_80281900->f3D0;
    lbl_80281900->f368 = lbl_80281900->f3B8;
    for (nPass = 0; nPass < 2; nPass++) {
        for (i = 0; i < nBuffers; i++) {
            pBuffer = lbl_80281900->apF0[lbl_80281900->n100][i];
            lbl_80281900->f34C = pBuffer->f0;
            lbl_80281900->f350 = pBuffer->f4;
            lbl_80281900->n36C = nPass;
            fn_8003519C(17, &lbl_80281900->f348);
            fn_800082CC(pBuffer->a14);
        }
    }
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

// Makes the 16 grass buffers and their lists, all free: the first four hold 600 vertices, the rest
// 450. Returns the bytes allocated.
s32 fn_8011FFCC(void) {
    s32 nBytes;
    s32 nVerts;
    s32 nSize;
    int i;

    lbl_80281900->nE4 = 0;
    lbl_80281900->nE8 = 16;
    lbl_80281900->nE0 = 16;
    lbl_80281900->anF8[0] = 0;
    lbl_80281900->anF8[1] = 0;
    lbl_80281900->pEC = fn_80009B34(16 * sizeof(GrassBuffer), 2, 16, "GoGrass.c", 3707);
    lbl_80281900->apF0[0] = fn_80009B34(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3709);
    lbl_80281900->apF0[1] = fn_80009B34(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3711);
    lbl_80281900->apD8 = fn_80009B34(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3714);
    lbl_80281900->apDC = fn_80009B34(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3716);
    nBytes = 16 * sizeof(GrassBuffer) + 4 * (16 * sizeof(GrassBuffer*));
    for (i = 0; i < lbl_80281900->nE0; i++) {
        nVerts = 450;
        if (i < 4) {
            nVerts = 600;
        }
        nSize = nVerts * 16;
        lbl_80281900->pEC[i].p40 = fn_80009B34(nSize, 2, 16, "GoGrass.c", 3729);
        nBytes += nSize;
        lbl_80281900->pEC[i].n44 = nVerts;
    }
    for (i = 0; i < lbl_80281900->nE0; i++) {
        lbl_80281900->apDC[i] = &lbl_80281900->pEC[i];
    }
    return nBytes;
}

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
void fn_80120268(f32* pA, f32* pB, f32* pOut);
s32 fn_8012028C(u8* p0);

s32 fn_8012022C(void) {
    return lbl_80281900->p370 != NULL;
}

// fmod for floats: the remainder of fX / fM (both callers pass the modulus in the second argument).
f32 fn_80120244(f32 fX, f32 fM) {
    return fmod(fX, fM);
}

// b + a into out (three floats)
#ifdef __MWERKS__
asm void fn_80120268(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80120268(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

s32 fn_8012028C(u8* p0) {
    return (*(s32*)p0 + 88);
}

// ---- end of sweep code ----
