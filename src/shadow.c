// shadow.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer's shadow,
// drawn into a 256x256 palette texture with its own render camera (the state at lbl_802814A8,
// shadow.h), then copied out of the frame buffer.

#include "golfer.h"
#include "shadow.h"

void  fn_80076A54(f32* pRect);
void  fn_80036054(void* pMesh, int n, s32* pDesc);   // Skin.c: set up a mesh object
void  fn_800360A0(void* pMesh);                      // Skin.c: free it
void  fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void  fn_80016948(void);
void  fn_800169AC(void);

void fn_800B2360(u8 nIntensity);
void fn_800B24D0(int nWidth, int nHeight);
void fn_800B26DC(void);
void fn_800B2724(void);

// Where the frame buffer is copied to: the screen copy's buffer.
void fn_800B2314(void) {
    Shadow* p = lbl_802814A8;

    GXGetTexBufferSize(256, 256, 9, 0, 0);
    p->pImage = fn_8002A624();
}

void fn_800B235C(void) {
}

// The palette: entry 0 clear, every other entry nIntensity.
void fn_800B2360(u8 nIntensity) {
    Shadow* p = lbl_802814A8;
    int i;

    for (i = 0; i < 256; i++) {
        if (i != 0) {
            p->aLut[i][0] = nIntensity;
            p->aLut[i][1] = nIntensity;
            p->aLut[i][2] = nIntensity;
            p->aLut[i][3] = nIntensity;
        } else {
            p->aLut[i][0] = 0;
            p->aLut[i][1] = 0;
            p->aLut[i][2] = 0;
            p->aLut[i][3] = 0;
        }
    }
}

void fn_800B2438(u8 bHigh) {
    int nIntensity = 0x20;

    if (bHigh) {
        nIntensity = 0x30;
    }
    fn_800B2360(nIntensity);
}

void fn_800B246C(void) {
}

// Copy what was drawn into the shadow's texture.
void fn_800B2470(void) {
    Shadow* p = lbl_802814A8;

    GXSetTexCopySrc(0, 0, 256, 256);
    GXSetTexCopyDst(256, 256, 0x27, 0);
    GXCopyTex(p->pImage, 0);
    GXPixModeSync();
}

void fn_800B24D0(int nWidth, int nHeight) {
    Shadow* p = lbl_802814A8;

    p->nWidth = nWidth;
    p->nHeight = nHeight;
}

void fn_800B24E0(f32 f) {
    fn_800B2360((int)(f * 27.0f) + 5);
}

// Set the shadow up: its buffers, its render camera and texture, its mesh objects.
void SH_vSetShadowIntensity(u8 bHigh) {
    Shadow* p = lbl_802814A8;
    int i;
    s32 aDesc[2];

    fn_800B2314();
    lbl_80282170 = fn_80009B34(0x6000, 2, 16, "shadow.c", 180);
    lbl_8028216C = fn_80009B34(0x2000, 2, 16, "shadow.c", 181);
    lbl_80282168 = fn_80009B34(0x4000, 2, 16, "shadow.c", 182);
    lbl_80282164 = fn_80009B34(0x1000, 2, 16, "shadow.c", 183);
    lbl_80282160 = fn_80009B34(0x1000, 2, 16, "shadow.c", 184);
    p->pLens = fn_80076400();
    p->pRect = fn_80076ACC();
    p->pFrameBuf = fn_8006E1C8();
    fn_8006E26C(p->pFrameBuf, 0.0f, 0.0f, 256.0f, 256.0f, 1.0f, 1.0f);
    fn_800171D8(p->pRect, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800B3438(p->pRect, 1.0f, 1.0f);
    p->pCamera = fn_8001371C(p->pLens, p->pFrameBuf, p->pRect);
    fn_800B24D0(256, 256);
    fn_800B26DC();
    fn_800B2438(bHigh);
    fn_800B2724();
    p->f4DC = 30.0f;
    p->f4E0 = 0.0f;
    p->n4E4 = 0;
    p->f4E8 = 0.0f;
    p->n4EC = 1;
    aDesc[0] = 0x800;
    aDesc[1] = 0x200;
    for (i = 0; i < 2; i++) {
        fn_80036054(p->aMesh[i], 0, aDesc);
    }
}

void fn_800B26DC(void) {
    fn_8002A528(&lbl_802814A8->tex, lbl_802814A8->nWidth, lbl_802814A8->nHeight, lbl_802814A8->pImage,
                lbl_802814A8->aLut, 9, 2, 0, 0);
}

void fn_800B2724(void) {
    lbl_802814A8->n4F0 = 0;
}

// Shut the shadow down: the mesh objects, the render camera, the buffers.
void fn_800B2734(void) {
    Shadow* p = lbl_802814A8;
    int i;

    for (i = 0; i < 2; i++) {
        fn_800360A0(p->aMesh[i]);
    }
    if (p->pFrameBuf != NULL) {
        fn_8006E214(p->pFrameBuf);
        p->pFrameBuf = NULL;
    }
    if (p->pCamera != NULL) {
        fn_800137B0(p->pCamera);
        p->pCamera = NULL;
    }
    if (p->pRect != NULL) {
        fn_80076B18(p->pRect);
        p->pRect = NULL;
    }
    if (p->pLens != NULL) {
        fn_8007644C(p->pLens);
        p->pLens = NULL;
    }
    fn_800B246C();
    fn_80009E70(lbl_80282170);
    fn_80009E70(lbl_8028216C);
    fn_80009E70(lbl_80282168);
    fn_80009E70(lbl_80282164);
    fn_80009E70(lbl_80282160);
    fn_800B235C();
}

// Clear the texture's area to black with a full-screen quad.
void fn_800B281C(void) {
    f32 aXY[8];
    f32 aColour[4];

    fn_80016B54(lbl_802814A8->nWidth, lbl_802814A8->nHeight, 1.0f, 1.0f);
    fn_80016948();
    fn_800169AC();
    fn_8001425C(0);
    fn_80014118(0);
    fn_80012F50(0, 6, 0x80);
    fn_80012EF8();
    aColour[0] = 0.0f;
    aColour[1] = 0.0f;
    aColour[2] = 0.0f;
    aColour[3] = 0.0f;
    fn_800141F8(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    aXY[2] = 0.0f;
    aXY[6] = 0.0f;
    fn_80014194(aColour);
    fn_8001644C(0xA1, aXY, 0, NULL, 2);
}

void fn_800B3438(f32* pRect, f32 x, f32 y) {
    pRect[4] = x;
    pRect[5] = y;
    fn_80076A54(pRect);
}

// Four floats: pOut gets pA plus pB.
#ifdef __MWERKS__
asm void fn_800B3460(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_800B3460(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif

// ---- sweep code (not yet cleaned up) ----

s32 fn_8000A0E8(s32, s32);
s32 fn_8000A798(s32, s32);
s32 fn_8000ADC0(s32);
s32 fn_8001728C(s32);
void fn_800B3484(s32 arg0, s32 arg1);

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
