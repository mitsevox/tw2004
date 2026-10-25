// ScreenClear.c: darkens the screen with a black full-screen quad, drawn over a few frames while
// the controllers and the sound keep running, as the main loop leaves the start-up state.

#include "game_types.h"
#include "engine.h"
#include "terrain.h"

// The gnScreenClearMode value ScreenClear_DarkenScreen tests; nothing sets it.
#define SCREENCLEAR_MODE_ONE_FRAME 1

// Only read, never written, so always 0, never SCREENCLEAR_MODE_ONE_FRAME.
s32 gnScreenClearMode;

static f32 ScreenClear_OrderConstants(f32 x) {  // fake match: never called, orders the float constants
    x += 1.0f;
    if (x < 0.0f) {
        return 0.0f;
    }
    return x * 0.5f;
}

// Draws a black quad over the whole screen, once per frame, reading the controllers and running
// the sound each frame: with bFade, 30 frames at alpha 0.1, the last two at 0.5, each followed by
// Displ_EndFrameEmpty. The game's one call (GO_bMainLoopDone, as the start-up state's main loop
// ends) passes bFade 1; no caller passes 0.
void ScreenClear_DarkenScreen(u8 bFade) {
    f32 aColour[4];
    f32 aXY[8];
    u32 bTwoFrameMode;
    int i;
    int nFrames;

    aColour[0] = 0.0f;
    aColour[1] = 0.0f;
    aColour[2] = 0.0f;
    bTwoFrameMode = gnScreenClearMode != SCREENCLEAR_MODE_ONE_FRAME;
    aColour[3] = bFade ? 0.1f : 0.5f;
    RenderView_SetColor(aColour);
    RenderView_MakeQuad(aXY, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    RenderView_SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
    DL_vWaitForEndOfDrawing();
    if (bFade) {
        nFrames = 30;
    } else {
        nFrames = bTwoFrameMode ? 2 : 1;
    }
    for (i = 0; i < nFrames; i++) {
        if (bFade && i >= nFrames - 2) {
            aColour[3] = 0.5f;
            RenderView_SetColor(aColour);
        }
        RenderView_FrameEmpty();
        Displ_FrameStart();
        // both cases set the same 512 x 448 area
        if (bTwoFrameMode) {
            RenderState_SetScissor(0, 0, 512, 448);
        } else {
            RenderState_SetScissor(0, 0, 512, 448);
        }
        RenderState_SetBlendFactors(4, 5);
        RenderView_SetUseCurrentMatrices(0);
        RenderState_SetDrawFlags(0x40);
        DS_vSetAlphaTestMode(0, DS_COMPARE_GEQUAL, 0x80);
        DS_vSetZBufferMode(DS_COMPARE_ALWAYS);
        RenderState_Flush();
        RenderView_DrawPrimitive(0xA1, aXY, NULL, NULL, 2);
        Input_vUpdate();
        Gaud_Cycle();
        if (!bTwoFrameMode) {
            Displ_WaitFrameEmpty();
        }
        LLObj_FrameEmpty();
        Displ_FrameEnd();
        DL_vWaitForEndOfDrawing();
        if (bTwoFrameMode) {
            Displ_EndFrameEmpty();
        }
    }
    DS_vSetAlphaTestMode(0, DS_COMPARE_GEQUAL, 0x80);
    DS_vSetZBufferMode(DS_COMPARE_LEQUAL);
    RenderState_Flush();
    LLObj_FrameEmpty();
    DL_vWaitForEndOfDrawing();
}
