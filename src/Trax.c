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
TraxTrack gEATraxSongs[TRAX_NUM_TRACKS];
TraxState gEATraxDisplay;

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
void FO_vSetCurrentAddMode(s32 v);
void fn_80012B9C(f32 fX, f32 fY);
void UFont_SetFont(s32 nFont);
void fn_80076128(s32 n);

void fn_800B9FF0(void) {
    gEATraxDisplay.bShow = 0;
    gEATraxDisplay.nFrames = 0;
    gEATraxDisplay.nTrack = 0;
    gEATraxDisplay.nLogo = -1;
    if (gSession.nGameType == 10) {
        gEATraxDisplay.nFont = 2;
        return;
    }
    gEATraxDisplay.nFont = 1;
}

void fn_800BA038(void) {
    if (fn_800BA080()) {
        fn_80010544(gEATraxDisplay.nLogo);
    }
    gEATraxDisplay.nLogo = -1;
}

// The logo is loaded.
u8 fn_800BA080(void) {
    return ((u32)((-1 - gEATraxDisplay.nLogo) | (gEATraxDisplay.nLogo + 1)) >> 31);
}

void UI_vEATraxRegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('TRAX', fn_800BA118);
    Stream_RegisterLoadChunkCallback('TRXT', fn_800BA15C);
}

void UI_vEATraxUnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('TRAX');
    Stream_UnregisterLoadChunkCallback('TRXT');
}

// The song list.
void fn_800BA118(UStreamObject* pObject) {
    Mem_cpy(gEATraxSongs, pObject->pData, sizeof(gEATraxSongs));
    StaticMem_Free(pObject);
}

// The logo's texture bank.
void fn_800BA15C(UStreamObject* pObject) {
    gEATraxDisplay.nLogo = fn_800107C0(pObject, NULL, 0);
    StaticMem_Free(pObject);
}

// Draw the box (with the logo when it is loaded) and the song's names.
void fn_800BA1A4(void) {
    f32 vColour[4];
    f32 aXY[8];
    f32 aUV[8];
    TexBank* pBank;
    TexEntry* pTex;

    if (gUIState.bFadeToBlack || (gSession.uFlags & 0x4000)) {
        return;
    }
    if (gEATraxDisplay.bShow && gEATraxDisplay.nFrames < 240) {
        if (fn_800BA080()) {
            pBank = fn_800106C4(gEATraxDisplay.nLogo);
            pTex = UI_GetTexBankFirstTexture(pBank);
        }
        RenderState_SetBlendFactors(4, 5);
        DS_vSetAlphaTestMode(0, 6, 0x80);
        DS_vSetZBufferMode(7);
        RenderView_SetUseCurrentMatrices(0);
        DS_vEnableZBufferUpdate(0);
        if (fn_800BA080()) {
            RenderState_SetBankTexture(pBank, pTex);
            RenderState_SetDrawFlags(0x50);
        } else {
            RenderState_SetDrawFlags(0x40);
        }
        RenderState_Flush();
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
        RenderView_SetColor(vColour);
        RenderView_MakeQuad(aXY, aUV, fn_800BA440(), fn_800BA40C(), fn_800BA440() + fn_800BA3D8(),
                    fn_800BA40C() + fn_800BA3A4());
        RenderView_DrawPrimitive(0xA1, aXY, 0, aUV, 2);
        fn_800BA550();
        gEATraxDisplay.nFrames++;
        return;
    }
    gEATraxDisplay.bShow = 0;
    gEATraxDisplay.nFrames = 0;
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
        if (gEATraxDisplay.nFrames <= 30) {
            return (0.02f - -0.4f) * (gEATraxDisplay.nFrames / 30.0f) + -0.4f;
        }
        return 0.02f;
    }
    if (gEATraxDisplay.nFrames <= 30) {
        return (0.02f - -0.3f) * (gEATraxDisplay.nFrames / 30.0f) + -0.3f;
    }
    return 0.02f;
}

// fAlpha, faded out over the last 15 frames.
f32 fn_800BA504(f32 fAlpha) {
    int nLeft = 240 - gEATraxDisplay.nFrames;

    if (nLeft <= 15) {
        fAlpha *= nLeft / 15.0f;
    }
    return fAlpha;
}

// The song's three names, one under the other.
void fn_800BA550(void) {
    f32 vColour[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    s8 nTrack = gEATraxDisplay.nTrack;
    char szSong[0xA0];          // size unknown: the frame leaves room for this much

    vColour[3] = fn_800BA504(0.5f);
    FO_vSetCurrentAddMode(1);
    fn_80012B9C(0.8f, 0.8f);
    fn_8006A9AC(vColour);
    UFont_SetFont(gEATraxDisplay.nFont);
    UFont_DrawString(gEATraxSongs[nTrack].sz0, fn_800BA440() + fn_800BA700(),
                fn_800BA40C() + fn_800BA6CC());
    sprintf(szSong, "\"%s\"", gEATraxSongs[nTrack].szSong);
    UFont_DrawString(szSong, fn_800BA440() + fn_800BA700(),
                0.029f + (fn_800BA40C() + fn_800BA6CC()));
    UFont_DrawString(gEATraxSongs[nTrack].sz100, fn_800BA440() + fn_800BA700(),
                0.058f + (fn_800BA40C() + fn_800BA6CC()));
    FO_vSetCurrentAddMode(0);
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
    gEATraxDisplay.bShow = bShow;
    gEATraxDisplay.nTrack = nTrack;
    gEATraxDisplay.nFrames = 0;
}

// data-order note: defined after the functions, so this .sdata pointer follows the file's string
// literal "\"%s\"" (0x80281508), as in the original.
s32 gInterruptsOffDepth;
s32* gpInterruptsOffDepth = &gInterruptsOffDepth;
