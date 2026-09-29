// uiEATrax.c (EA's name; Trax.c before 2026-09-29): the EA Trax music display: the 'TRAX' and 'TRXT' stream
// objects (the song list and the EA Trax logo) and, for 240 frames after a song starts, its names
// in a box that slides in from the left and fades out.
// Why uiEATrax.c: TW2003's data names "uiEATrax.c" right after "crcmp_mad_codec.c", and this file
// links right after rcmp_mad_codec.c and the Create-A-Player ball's Code800B9944.c; TW07's
// uiEATrax.c starts UI_vEATraxRegisterStreamClients, UI_vEATraxUnRegisterStreamClients,
// UI_vEATraxLoadfromStream, in this file's order.

#include "engine.h"
#include "golfer.h"
#include "terrain.h"
#include "platform.h"
#include "frontend/fe.h"
#include "trax.h"

// Defined in reverse address order: CodeWarrior lays out .bss last-defined first.
TraxTrack gEATraxSongs[TRAX_NUM_TRACKS];        // the song list ('TRAX')
TraxState gEATraxDisplay;                       // the song display now

u8 UI_EATraxIsLogoLoaded(void);
void UI_vEATraxLoadfromStream(UStreamObject* pObject);
void UI_vEATraxLoadLogoFromStream(UStreamObject* pObject);
f32 UI_EATraxGetBoxHeight(void);
f32 UI_EATraxGetBoxWidth(void);
f32 UI_EATraxGetBoxTop(void);
f32 UI_EATraxGetBoxLeft(void);
f32 UI_EATraxFadeAlpha(f32 fAlpha);
void UI_EATraxDrawSongNames(void);
f32 UI_EATraxGetTextOffsetY(void);
f32 UI_EATraxGetTextOffsetX(void);
void FO_vSetCurrentAddMode(s32 v);
void fn_80012B9C(f32 fX, f32 fY);
void UFont_SetFont(s32 nFont);
void FO_vSetCurrentColor(s32 n);

// Reset the EA Trax song display: hidden, song 0, no logo (-1), font 2 in game type 10, else 1.
// uiProcessInterface.c calls it when it resets the UI.
void UI_EATraxReset(void) {
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

// Free the EA Trax logo's texture slot, if it is loaded (uiProcessInterface.c calls it as the UI
// closes).
void UI_EATraxFreeLogo(void) {
    if (UI_EATraxIsLogoLoaded()) {
        fn_80010544(gEATraxDisplay.nLogo);
    }
    gEATraxDisplay.nLogo = -1;
}

// Whether the EA Trax logo is loaded (its texture slot, gEATraxDisplay.nLogo, is not -1).
u8 UI_EATraxIsLogoLoaded(void) {
    return ((u32)((-1 - gEATraxDisplay.nLogo) | (gEATraxDisplay.nLogo + 1)) >> 31);
}

// Register the loaders of the 'TRAX' (the song list) and 'TRXT' (the EA Trax logo) stream objects.
void UI_vEATraxRegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('TRAX', UI_vEATraxLoadfromStream);
    Stream_RegisterLoadChunkCallback('TRXT', UI_vEATraxLoadLogoFromStream);
}

void UI_vEATraxUnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('TRAX');
    Stream_UnregisterLoadChunkCallback('TRXT');
}

// The 'TRAX' stream object's loader: copy its song list into gEATraxSongs (19 songs), then free the
// object.
void UI_vEATraxLoadfromStream(UStreamObject* pObject) {
    Mem_cpy(gEATraxSongs, pObject->pData, sizeof(gEATraxSongs));
    StaticMem_Free(pObject);
}

// The 'TRXT' stream object's loader: load its texture bank (the EA Trax logo) into a texture slot
// (gEATraxDisplay.nLogo), then free the object.
void UI_vEATraxLoadLogoFromStream(UStreamObject* pObject) {
    gEATraxDisplay.nLogo = fn_800107C0(pObject, NULL, 0);
    StaticMem_Free(pObject);
}

// Draw the EA Trax song display (gomainloop.c, each frame), unless the screen is fading to black or
// the session has flag 0x4000: for 240 frames after a song starts, a box that slides in from the
// left and fades out at the end (the EA Trax logo when it is loaded, else plain light grey), with
// the song's names in it; then the display stops.
void UI_EATraxDraw(void) {
    f32 vColour[4];
    f32 aXY[8];
    f32 aUV[8];
    TexBank* pBank;
    TexEntry* pTex;

    if (gUIState.bFadeToBlack || (gSession.uFlags & 0x4000)) {
        return;
    }
    if (gEATraxDisplay.bShow && gEATraxDisplay.nFrames < 240) {
        if (UI_EATraxIsLogoLoaded()) {
            pBank = fn_800106C4(gEATraxDisplay.nLogo);
            pTex = UI_GetTexBankFirstTexture(pBank);
        }
        RenderState_SetBlendFactors(4, 5);
        DS_vSetAlphaTestMode(0, 6, 0x80);
        DS_vSetZBufferMode(7);
        RenderView_SetUseCurrentMatrices(0);
        DS_vEnableZBufferUpdate(0);
        if (UI_EATraxIsLogoLoaded()) {
            RenderState_SetBankTexture(pBank, pTex);
            RenderState_SetDrawFlags(0x50);
        } else {
            RenderState_SetDrawFlags(0x40);
        }
        RenderState_Flush();
        if (UI_EATraxIsLogoLoaded()) {
            vColour[0] = 0.5f;
            vColour[1] = 0.5f;
            vColour[2] = 0.5f;
            vColour[3] = UI_EATraxFadeAlpha(0.5f);
        } else {
            vColour[0] = 0.9f;
            vColour[1] = 0.9f;
            vColour[2] = 0.9f;
            vColour[3] = UI_EATraxFadeAlpha(0.15f);
        }
        RenderView_SetColor(vColour);
        RenderView_MakeQuad(aXY, aUV, UI_EATraxGetBoxLeft(), UI_EATraxGetBoxTop(), UI_EATraxGetBoxLeft()
                            + UI_EATraxGetBoxWidth(),
                    UI_EATraxGetBoxTop() + UI_EATraxGetBoxHeight());
        RenderView_DrawPrimitive(0xA1, aXY, 0, aUV, 2);
        UI_EATraxDrawSongNames();
        gEATraxDisplay.nFrames++;
        return;
    }
    gEATraxDisplay.bShow = 0;
    gEATraxDisplay.nFrames = 0;
}

// The box's height: 0.13, with or without the logo.
f32 UI_EATraxGetBoxHeight(void) {
    if (UI_EATraxIsLogoLoaded()) {
        return 0.13f;
    }
    return 0.13f;
}

// The box's width: 0.4 with the logo, else 0.3.
f32 UI_EATraxGetBoxWidth(void) {
    if (UI_EATraxIsLogoLoaded()) {
        return 0.4f;
    }
    return 0.3f;
}

// The box's top: 0.79 with the logo, else 0.83.
f32 UI_EATraxGetBoxTop(void) {
    if (UI_EATraxIsLogoLoaded()) {
        return 0.79f;
    }
    return 0.83f;
}

// The box's left edge: it slides in from minus its width to 0.02 over the first 30 frames.
f32 UI_EATraxGetBoxLeft(void) {
    if (UI_EATraxIsLogoLoaded()) {
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

// fAlpha, faded out over the display's last 15 frames.
f32 UI_EATraxFadeAlpha(f32 fAlpha) {
    int nLeft = 240 - gEATraxDisplay.nFrames;

    if (nLeft <= 15) {
        fAlpha *= nLeft / 15.0f;
    }
    return fAlpha;
}

// The song's three lines (gEATraxSongs[nTrack]: sz0, the song's name in quotes, sz100), one under
// the other in the box, drawn at once in black at the box's alpha in font nFont.
void UI_EATraxDrawSongNames(void) {
    f32 vColour[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    s8 nTrack = gEATraxDisplay.nTrack;
    char szSong[0xA0];          // size unknown: the frame leaves room for this much

    vColour[3] = UI_EATraxFadeAlpha(0.5f);
    FO_vSetCurrentAddMode(1);
    fn_80012B9C(0.8f, 0.8f);
    fn_8006A9AC(vColour);
    UFont_SetFont(gEATraxDisplay.nFont);
    UFont_DrawString(gEATraxSongs[nTrack].sz0, UI_EATraxGetBoxLeft() + UI_EATraxGetTextOffsetX(),
                UI_EATraxGetBoxTop() + UI_EATraxGetTextOffsetY());
    sprintf(szSong, "\"%s\"", gEATraxSongs[nTrack].szSong);
    UFont_DrawString(szSong, UI_EATraxGetBoxLeft() + UI_EATraxGetTextOffsetX(),
                0.029f + (UI_EATraxGetBoxTop() + UI_EATraxGetTextOffsetY()));
    UFont_DrawString(gEATraxSongs[nTrack].sz100, UI_EATraxGetBoxLeft() + UI_EATraxGetTextOffsetX(),
                0.058f + (UI_EATraxGetBoxTop() + UI_EATraxGetTextOffsetY()));
    FO_vSetCurrentAddMode(0);
    FO_vSetCurrentColor(11);
}

// The text's offset down from the box's top: 0.025.
f32 UI_EATraxGetTextOffsetY(void) {
    if (UI_EATraxIsLogoLoaded()) {
        return 0.025f;
    }
    return 0.025f;
}

// The text's offset right from the box's left: 0.1 with the logo (the text goes past it), else
// 0.02.
f32 UI_EATraxGetTextOffsetX(void) {
    if (UI_EATraxIsLogoLoaded()) {
        return 0.1f;
    }
    return 0.02f;
}

// Show song nTrack's names from the start (bShow 1), or stop showing them (0). GameAudio.c
// Gaud_StartMusic shows the song it starts; LLVideo.c stops it before a movie.
void UI_EATraxShowSong(int bShow, s8 nTrack) {
    gEATraxDisplay.bShow = bShow;
    gEATraxDisplay.nTrack = nTrack;
    gEATraxDisplay.nFrames = 0;
}

// data-order note: defined after the functions, so this .sdata pointer follows the file's string
// literal "\"%s\"" (0x80281508), as in the original. They are LLDisp_Gc.c's (its fn_80007368 and
// fn_80007328 count how deep interrupts are turned off through the pointer), but lie in this file's
// .sbss / .sdata.
s32 gInterruptsOffDepth;
s32* gpInterruptsOffDepth = &gInterruptsOffDepth;
