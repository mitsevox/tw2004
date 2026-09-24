// fe_movies.c (TW06's file name, a guess from the filemap): the front end's movies. Not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80008380(void);
void fn_80092250(f32* pA, f32* pB, f32* pOut);
void fn_80092080(LLPict* pPict, f32 fAlpha);    // draws the picture at that alpha
void fn_80091FC0(LLPict* pPict, int nFrames, f32 fStep);

// ---- sweep code (not yet cleaned up) ----

void fn_80091708(void);
void fn_80006EDC();
void fn_80006FE8();
void fn_80007254();
void fn_800083A0();
void fn_80091BDC();
void fn_800A4BDC();
void fn_80091B98(s32 p0);
void fn_80012EF8();
void fn_8005CC64();
void fn_80091DB8(int nFrames);
void fn_80091D84(void);
void fn_80091EE4(void);
void fn_8009220C(void);
f32 fn_80092210(void);
void fn_80013E30();
void fn_80092274(s32 p0);

void fn_80091454(void) {
    lbl_80281370 = 0;
}

void fn_80091708(void) {
}

void fn_80091B98(s32 p0) {
    fn_80006EDC();
    fn_80091BDC(p0);
    fn_80006FE8();
    fn_80007254();
    fn_800083A0();
    fn_800A4BDC();
}

void fn_80091D84(void) {
    fn_80091DB8(30);
    fn_8005CC64(0, 0);
    fn_80012EF8();
}

void fn_80091EE4(void) {
}

void fn_8009220C(void) {
}

f32 fn_80092210(void) {
    if (lbl_80281F1C != NULL) {
        return lbl_80281F1C->f18;
    }
    return 0.0f;
}

void fn_80092274(s32 p0) {
    fn_80013E30(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

TexEntry* fn_800922A0(TexBank* pBank) {
    return pBank->p8;
}

// ---- end of sweep code ----

// Eight points across the screen, an eighth apart, all at height 0.839 (fn_80091BDC draws at them).
void fn_8009170C(void) {
    lbl_801D8818[0][0] = 0.0f;
    lbl_801D8818[0][1] = 0.839f;
    lbl_801D8818[1][0] = 0.125f;
    lbl_801D8818[1][1] = 0.839f;
    lbl_801D8818[2][0] = 0.25f;
    lbl_801D8818[2][1] = 0.839f;
    lbl_801D8818[3][0] = 0.375f;
    lbl_801D8818[3][1] = 0.839f;
    lbl_801D8818[4][0] = 0.5f;
    lbl_801D8818[4][1] = 0.839f;
    lbl_801D8818[5][0] = 0.625f;
    lbl_801D8818[5][1] = 0.839f;
    lbl_801D8818[6][0] = 0.75f;
    lbl_801D8818[6][1] = 0.839f;
    lbl_801D8818[7][0] = 0.875f;
    lbl_801D8818[7][1] = 0.839f;
}

// Load the texture bank from LoadData.c's 'txf2' copy, unless the session has flag 4.
void fn_80091778(void) {
    if (gSession.uFlags & 4) return;
    lbl_80281378 = fn_800107C0(lbl_80281C0C, NULL, 0);
    lbl_80281F20 = fn_800106C4(lbl_80281378);
    lbl_80281F24 = fn_800922A0(lbl_80281F20);
}

// Decodes the picture in the 'load' object, once.
void fn_800917C8(void) {
    if (lbl_801D8858.p30 == NULL) {
        lbl_801D8858.p30 = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    }
}

// Free the picture fn_800917C8 decoded, unless the session has flag 4.
void fn_80091818(void) {
    if (!(gSession.uFlags & 4) && lbl_801D8858.p30 != NULL) {
        fn_8002FE70(lbl_801D8858.p30);
        fn_8002FEAC();
        lbl_801D8858.p30 = NULL;
    }
}

// Free the bank fn_80091778 loaded.
void fn_80091870(void) {
    if (gSession.uFlags & 4) return;
    fn_80010544(lbl_80281378);
}

// Decode the picture in the 'load' object, show it for nFrames frames (fading in over 30), then
// free it.
void fn_80091DB8(int nFrames) {
    LLPict* pPict;

    pPict = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    fn_80091FC0(pPict, nFrames, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    fn_8002FEAC();
}

// Show pPict for nFrames frames, fading it in by fStep a frame (up to 1).
void fn_80091FC0(LLPict* pPict, int nFrames, f32 fStep) {
    f32 fAlpha = 0.0f;
    int i;

    for (i = 0; i < nFrames; i++) {
        fn_80006EDC();
        fAlpha += fStep;
        if (fAlpha > 1.0f) {
            fAlpha = 1.0f;
        }
        fn_80092080(pPict, fAlpha);
        fn_80006FE8();
        fn_80007254();
        fn_800083A0();
        fn_800A4BDC();
        fn_800B7490();
    }
}

// Free the pixel data of every bank whose entry in lbl_801D8890 has a positive n4.
void fn_80092198(void) {
    int i;

    for (i = 0; i < FE_NUM_801D8890; i++) {
        if (lbl_801D8890[i].n4 > 0) {
            fn_80008380();
            fn_8000FFAC(lbl_801A26DC[i]);
        }
    }
}

// The picture's f6C and f70, then 0 and 1.
void fn_8009222C(f32* pOut, LLPict* pPict) {
    pOut[0] = pPict->f6C;
    pOut[1] = pPict->f70;
    pOut[2] = 0.0f;
    pOut[3] = 1.0f;
}

// b + a into out (four floats)
#ifdef __MWERKS__
asm void fn_80092250(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80092250(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif
