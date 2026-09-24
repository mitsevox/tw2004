// Trax.c (our name): the EA Trax music display: the 'TRAX' and 'TRXT' stream objects (the song
// list and the EA Trax logo) and, for 240 frames after a song starts, its names in a box that
// slides in from the left and fades out.

#include "engine.h"
#include "golfer.h"
#include "terrain.h"
#include "platform.h"
#include "frontend/fe.h"
#include "trax.h"

// Defined in reverse address order: CodeWarrior lays out .bss last-defined first.
TraxTrack lbl_801F846C[TRAX_NUM_TRACKS];
TraxState lbl_801F8458;

u8 fn_800BA080(void);
void fn_800BA118(UStreamObject* pObject);
void fn_800BA15C(UStreamObject* pObject);
f32 fn_800BA3A4(void);
f32 fn_800BA3D8(void);
f32 fn_800BA40C(void);
f32 fn_800BA440(void);
f32 fn_800BA504(f32 fAlpha);
void fn_800BA550(void);
f32 fn_800BA6CC(void);
f32 fn_800BA700(void);
void UFont_SetMode(s32 v);
void fn_80012B9C(f32 fX, f32 fY);
void UFont_SetFont(s32 nFont);
void fn_80076128(s32 n);

void fn_800B9FF0(void) {
    lbl_801F8458.bShow = 0;
    lbl_801F8458.nFrames = 0;
    lbl_801F8458.nTrack = 0;
    lbl_801F8458.nLogo = -1;
    if (gSession.nGameType == 10) {
        lbl_801F8458.nFont = 2;
        return;
    }
    lbl_801F8458.nFont = 1;
}

void fn_800BA038(void) {
    if (fn_800BA080()) {
        fn_80010544(lbl_801F8458.nLogo);
    }
    lbl_801F8458.nLogo = -1;
}

// The logo is loaded.
u8 fn_800BA080(void) {
    return ((u32)((-1 - lbl_801F8458.nLogo) | (lbl_801F8458.nLogo + 1)) >> 31);
}

void fn_800BA0A0(void) {
    UStream_RegisterHandler('TRAX', fn_800BA118);
    UStream_RegisterHandler('TRXT', fn_800BA15C);
}

void fn_800BA0E4(void) {
    UStream_UnregisterHandler('TRAX');
    UStream_UnregisterHandler('TRXT');
}

// The song list.
void fn_800BA118(UStreamObject* pObject) {
    Mem_cpy(lbl_801F846C, pObject->pData, sizeof(lbl_801F846C));
    fn_80009E70(pObject);
}

// The logo's texture bank.
void fn_800BA15C(UStreamObject* pObject) {
    lbl_801F8458.nLogo = fn_800107C0(pObject, NULL, 0);
    fn_80009E70(pObject);
}

// Draw the box (with the logo when it is loaded) and the song's names.
void fn_800BA1A4(void) {
    f32 vColour[4];
    f32 aXY[8];
    f32 aUV[8];
    TexBank* pBank;
    TexEntry* pTex;

    if (lbl_801D87C0.b0 || (gSession.uFlags & 0x4000)) {
        return;
    }
    if (lbl_801F8458.bShow && lbl_801F8458.nFrames < 240) {
        if (fn_800BA080()) {
            pBank = fn_800106C4(lbl_801F8458.nLogo);
            pTex = fn_800922A0(pBank);
        }
        fn_80035118(4, 5);
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(7);
        fn_8001425C(0);
        fn_80012F34(0);
        if (fn_800BA080()) {
            fn_8005CC64(pBank, pTex);
            fn_80014118(0x50);
        } else {
            fn_80014118(0x40);
        }
        fn_80012EF8();
        if (fn_800BA080()) {
            vColour[0] = 0.5f;
            vColour[1] = 0.5f;
            vColour[2] = 0.5f;
            vColour[3] = fn_800BA504(0.5f);
        } else {
            vColour[0] = 0.9f;
            vColour[1] = 0.9f;
            vColour[2] = 0.9f;
            vColour[3] = fn_800BA504(0.15f);
        }
        fn_80014194(vColour);
        fn_800141F8(aXY, aUV, fn_800BA440(), fn_800BA40C(), fn_800BA440() + fn_800BA3D8(),
                    fn_800BA40C() + fn_800BA3A4());
        fn_8001644C(0xA1, aXY, 0, aUV, 2);
        fn_800BA550();
        lbl_801F8458.nFrames++;
        return;
    }
    lbl_801F8458.bShow = 0;
    lbl_801F8458.nFrames = 0;
}

// The box's height.
f32 fn_800BA3A4(void) {
    if (fn_800BA080()) {
        return 0.13f;
    }
    return 0.13f;
}

// The box's width.
f32 fn_800BA3D8(void) {
    if (fn_800BA080()) {
        return 0.4f;
    }
    return 0.3f;
}

// The box's top.
f32 fn_800BA40C(void) {
    if (fn_800BA080()) {
        return 0.79f;
    }
    return 0.83f;
}

// The box's left: it slides in over the first 30 frames.
f32 fn_800BA440(void) {
    if (fn_800BA080()) {
        if (lbl_801F8458.nFrames <= 30) {
            return (0.02f - -0.4f) * (lbl_801F8458.nFrames / 30.0f) + -0.4f;
        }
        return 0.02f;
    }
    if (lbl_801F8458.nFrames <= 30) {
        return (0.02f - -0.3f) * (lbl_801F8458.nFrames / 30.0f) + -0.3f;
    }
    return 0.02f;
}

// fAlpha, faded out over the last 15 frames.
f32 fn_800BA504(f32 fAlpha) {
    int nLeft = 240 - lbl_801F8458.nFrames;

    if (nLeft <= 15) {
        fAlpha *= nLeft / 15.0f;
    }
    return fAlpha;
}

// The song's three names, one under the other.
void fn_800BA550(void) {
    f32 vColour[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    s8 nTrack = lbl_801F8458.nTrack;
    char szSong[0xA0];          // size unknown: the frame leaves room for this much

    vColour[3] = fn_800BA504(0.5f);
    UFont_SetMode(1);
    fn_80012B9C(0.8f, 0.8f);
    fn_8006A9AC(vColour);
    UFont_SetFont(lbl_801F8458.nFont);
    UFont_DrawString(lbl_801F846C[nTrack].sz0, fn_800BA440() + fn_800BA700(),
                fn_800BA40C() + fn_800BA6CC());
    sprintf(szSong, "\"%s\"", lbl_801F846C[nTrack].szSong);
    UFont_DrawString(szSong, fn_800BA440() + fn_800BA700(),
                0.029f + (fn_800BA40C() + fn_800BA6CC()));
    UFont_DrawString(lbl_801F846C[nTrack].sz100, fn_800BA440() + fn_800BA700(),
                0.058f + (fn_800BA40C() + fn_800BA6CC()));
    UFont_SetMode(0);
    fn_80076128(11);
}

// The text's offset down from the box's top.
f32 fn_800BA6CC(void) {
    if (fn_800BA080()) {
        return 0.025f;
    }
    return 0.025f;
}

// The text's offset right from the box's left: past the logo when there is one.
f32 fn_800BA700(void) {
    if (fn_800BA080()) {
        return 0.1f;
    }
    return 0.02f;
}

// Show song nTrack's names (bShow 1), or stop showing them.
void fn_800BA734(int bShow, s8 nTrack) {
    lbl_801F8458.bShow = bShow;
    lbl_801F8458.nTrack = nTrack;
    lbl_801F8458.nFrames = 0;
}
