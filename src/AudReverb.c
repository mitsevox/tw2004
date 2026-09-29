// AudReverb.c (our name): the sound's aux A effect. Rvb_InitModule takes a 128 KB buffer for the
// effects library's memory, Rvb_SetMode switches between a high-quality reverb (two settings)
// and a delay, and Rvb_Pause takes the effect off or puts it back. Between uiObject.c and
// startUp.c.

#include "game_types.h"
#include "platform.h"
#include "golfer.h"
#include "core/startup.h"
#include "core/audcontainers.h"

void* Rvb_FxAlloc(u32 uSize);
void Rvb_FxFree(void* p);
void Rvb_SetMode(s8 nMode);
u8 Rvb_InitModule(void);
u8 Rvb_InitSession(u8 nKind, u8 bOn);
void Rvb_ExitSession(void);
void Rvb_Pause(u8 bMute);
void Rvb_SetPreset(void);
void Rvb_Cycle(void);

// The effects' settings: a 2.5 second reverb for mode 0, a 4 second one for mode 2, and for mode
// 1 a delay of about half a second (499 and 501 ms left and right) with a little feedback.
AXFX_REVERBHI gRvbReverbShort = { {0}, 0, 0.5f, 1.0f, 2.5f, 0.6f, 0.0f, 0.5f };
AXFX_DELAY gRvbDelay = { {0}, {499, 501, 10}, {15, 15, 0}, {100, 100, 0} };
AXFX_REVERBHI gRvbReverbLong = { {0}, 0, 0.9f, 1.0f, 4.0f, 0.2f, 0.0f, 0.5f };

// Uninitialised data, defined last-first (CodeWarrior lays it out in reverse).
UAudMemStack gRvbMemStack;              // the effect memory
UAudMemStackBlock gRvbMemBlocks[32];     // the effect memory's blocks
s8 gRvbMode = -1;                   // the mode set up (-1: none yet)
void* gRvbFxState;                     // the effect's state
AXAuxCallback gRvbFxCallback;             // the effect running
u8* gRvbFxMemory;                       // the effect memory's buffer

// The effects library's allocator hook (AXFXSetHooks): uSize bytes of the effect memory, which
// Rvb_SetMode starts over at each change.
void* Rvb_FxAlloc(u32 uSize) {
    return AudMemStack_AllocTop(&gRvbMemStack, uSize);
}

// The effects library's free hook: empty, as the effect memory is never freed piecemeal.
void Rvb_FxFree(void* p) {
}

// Switches the aux A effect to nMode: 1 the delay, 0 the 2.5 s reverb, 2 the 4 s reverb. Nothing
// when it is already in nMode; otherwise the effect memory starts over and the new effect is set up
// and installed (only if its set-up succeeded) with interrupts off.
void Rvb_SetMode(s8 nMode) {
    AXFX_REVERBHI* pReverb;
    int bEnabled;
    int bOk;

    if (nMode != gRvbMode) {
        gRvbMode = nMode;
        bEnabled = OSDisableInterrupts();
        AudMemStack_Init(&gRvbMemStack, gRvbFxMemory, 0x20000, 32, gRvbMemBlocks, 4);
        if (gRvbMode == 1) {
            gRvbFxState = &gRvbDelay;
            gRvbFxCallback = AXFXDelayCallback;
            bOk = AXFXDelayInit(&gRvbDelay);
        } else {
            pReverb = (gRvbMode == 0) ? &gRvbReverbShort : &gRvbReverbLong;
            gRvbFxState = pReverb;
            gRvbFxCallback = AXFXReverbHiCallback;
            bOk = AXFXReverbHiInit(pReverb);
        }
        if (bOk == 1) {
            AXRegisterAuxACallback(gRvbFxCallback, gRvbFxState);
        }
        OSRestoreInterrupts(bEnabled);
    }
}

// The reverb's start-up step in Aud_InitOnce: takes the 128 KB effect memory from the sound
// engine's memory and gives the effects library its allocator hooks. Always 1.
u8 Rvb_InitModule(void) {
    gRvbFxMemory = AudMem_Alloc(0x20000);
    AXFXSetHooks(Rvb_FxAlloc, Rvb_FxFree);
    return 1;
}

// The reverb's step in Ses_Init: picks the session's effect (Rvb_SetMode). The 2.5 s reverb (mode
// 0) in subsession 0 (the front end, play before a hole), the 4 s reverb (mode 2) in session 8
// (course 7) on hole index 2 (Game_GetCurHoleNum), the delay (mode 1) on every other hole. Always
// 1.
u8 Rvb_InitSession(u8 nKind, u8 bOn) {
    s8 nHole;                           // fake match: EA keeps the hole index as a signed byte
    s8 nMode;

    nHole = Game_GetCurHoleNum();
    if (bOn == 0) {
        nMode = 0;
    } else if (nKind == 8 && nHole == 2) {
        nMode = 2;
    } else {
        nMode = 1;
    }
    Rvb_SetMode(nMode);
    return 1;
}

// The reverb's step in Ses_Exit: empty in this build.
void Rvb_ExitSession(void) {
}

// Takes the aux A effect off while the sound is paused (bMute set, Ses_Pause) and puts the running
// one back after.
void Rvb_Pause(u8 bMute) {
    if (bMute != 0) {
        AXRegisterAuxACallback(NULL, NULL);
        return;
    }
    AXRegisterAuxACallback(gRvbFxCallback, gRvbFxState);
}

// A listener's reverb preset (Mic_SetRvbPreset): empty in this build, so the presets change
// nothing.
void Rvb_SetPreset(void) {
}

// The reverb's per-frame step (Aud_EmiCycle, after Trk_Cycle and Voc_Cycle): empty in this build.
void Rvb_Cycle(void) {
}
