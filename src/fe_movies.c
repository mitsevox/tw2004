// fe_movies.c (TW06's file name, a guess from the filemap): the front end's movies. Not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "llvideo.h"
#include "core/startup.h"

void fn_80008380(void);
void fn_80092250(f32* pA, f32* pB, f32* pOut);
void fn_80092080(LLPict* pPict, f32 fAlpha);    // draws the picture at that alpha
void fn_80091FC0(LLPict* pPict, int nFrames, f32 fStep);
s32  fn_800171B0(void);                 // ViewController.c
void fn_800760D8(LLPict* pPict);        // LLVideo.c
void fn_800760F4(f32* pUV, LLPict* pPict);  // LLVideo.c
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);

// ---- sweep code (not yet cleaned up) ----

void fn_80091708(void);
void fn_80006EDC();
void fn_80006FE8();
void fn_80007254();
void fn_800083A0();
void fn_80091BDC(int nPoint);
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

// Set lbl_801D8858 up once, unless the session has flag 4: the clock, the number of players in
// game type 4 (else 0) and values from it, then the 'load' object's picture (fn_800917C8).
void fn_800918A4(void) {
    if (!(gSession.uFlags & 4) && !lbl_801D8858.b18) {
        lbl_801D8858.b18 = 1;
        lbl_801D8858.n1C = -1;
        lbl_801D8858.f10 = 0.0f;
        lbl_801D8858.u20 = fn_80095368();
        lbl_801D8858.n0 = 0;
        if (gSession.nGameType == 4) {
            lbl_801D8858.n14 = gSession.nNumPlayers;
        } else {
            lbl_801D8858.n14 = 0;
        }
        lbl_801D8858.fC = lbl_801D8858.f4 = (4.83f * lbl_801D8858.n14 + 3.1f) / 8.0f;
        lbl_801D8858.f8 = 0.0f;
        fn_800917C8();
    }
}

// Draw tile nPoint of the texture bank fn_80091778 loaded at point nPoint of lbl_801D8818, an
// eighth of the screen wide: tiles 0-3 come from the top half of the texture, 4-7 from the bottom
// (their u runs past 1 and wraps).
void fn_80091BDC(int nPoint) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];

    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    fn_8001425C(0);
    fn_80012F34(0);
    fn_8005CC64(lbl_80281F20, lbl_80281F24);
    fn_80014118(0x50);
    fn_80012EF8();
    afColour[3] = 0.25f;        // EA code: overwritten at once
    afColour[0] = 0.5f;
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f;
    fn_80014194(afColour);
    fn_800141F8(afXY, NULL, lbl_801D8818[nPoint][0], lbl_801D8818[nPoint][1],
                lbl_801D8818[nPoint][0] + 0.125f, lbl_801D8818[nPoint][1] + 0.142f);
    if (nPoint < 4) {
        afUV[0] = 0.25f * nPoint;
        afUV[1] = 0.0f;
        afUV[2] = 0.0f;
        afUV[3] = 0.0f;
        afUV[4] = 0.25f + 0.25f * nPoint;
        afUV[5] = 0.5f;
        afUV[6] = 0.0f;
        afUV[7] = 0.0f;
    } else {
        afUV[0] = 0.25f * nPoint;
        afUV[1] = 0.5f;
        afUV[2] = 0.0f;
        afUV[3] = 0.0f;
        afUV[4] = 0.25f + 0.25f * nPoint;
        afUV[5] = 1.0f;
        afUV[6] = 0.0f;
        afUV[7] = 0.0f;
    }
    fn_8001644C(0xA1, afXY, 0, afUV, 2);
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

// The movies after a round in game type 1 (uiProcessInterface.c fn_80090400): "eas", then, unless
// the session has flag 0x4000, one of the two cameo movies "tigcam01"/"tigcam02" at random
// (skippable with any button); then the first 'LEGL' picture startUp.c kept, shown for 180
// frames and freed.
void fn_80091EE8(void) {
    char szPath[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    char szName[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    LLPict* pPict;

    FE_MakeMoviePath("eas", szPath);
    fn_80075FB8(szPath, NULL, 0, 0);
    if (!(gSession.uFlags & 0x4000)) {
        sprintf(szName, "tigcam%02d", (s16)((Rand_Next(0) & 1) + 1));
        FE_MakeCameoMoviePath(szName, szPath);
        fn_80075FB8(szPath, fn_80076FDC, 0, 0);
    }
    pPict = fn_8002FD00(lbl_80282134, lbl_8028212C);
    fn_80091FC0(pPict, 180, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    fn_80009E70(lbl_80282134);
    lbl_80282134 = NULL;
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

// Draw pPict over the whole 512x448 screen at fAlpha, through a frame buffer of its own, then put
// the previous render slot back.
void fn_80092080(LLPict* pPict, f32 fAlpha) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];
    GoFrameBuf frameBuf;
    s32 nOld;

    fn_800760D8(pPict);
    fn_80035118(4, 5);
    fn_80014118(0x50);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    nOld = fn_800171B0();
    fn_8006E26C(&frameBuf, 0.0f, 0.0f, 512.0f, 448.0f, 1.0f, 1.0f);
    // port: the render slot is typed s32 but holds a pointer
    fn_80092274((s32)&frameBuf);
    fn_80013EEC(fn_8001614C());
    fn_80012EF8();
    afColour[0] = 0.5f;
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f * fAlpha;
    fn_800141F8(NULL, afXY, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800760F4(afUV, pPict);
    fn_80014194(afColour);
    fn_8001644C(0xA1, afXY, 0, afUV, 2);
    fn_80092274(nOld);
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
