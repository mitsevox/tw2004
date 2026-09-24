// AudReverb.c (our name): the sound's aux A effect. fn_800AF224 takes a 128 KB buffer for the
// effects library's memory, fn_800AF144 switches between a high-quality reverb (two settings)
// and a delay, and fn_800AF2DC takes the effect off or puts it back. Between uiobject.c and
// startUp.c.

#include "game_types.h"
#include "platform.h"
#include "golfer.h"
#include "core/startup.h"
#include "core/audcontainers.h"

void* fn_800AF114(u32 uSize);
void fn_800AF140(void* p);
void fn_800AF144(s8 nMode);
int fn_800AF224(void);
u8 fn_800AF264(u8 nKind, u8 bOn);
void fn_800AF2D8(void);
void fn_800AF2DC(u8 bMute);
void fn_800AF31C(void);
void fn_800AF320(void);

// The effects' settings: a 2.5 second reverb for mode 0, a 4 second one for mode 2, and for mode
// 1 a delay of about half a second (499 and 501 ms left and right) with a little feedback.
AXFX_REVERBHI lbl_8018EC20 = { {0}, 0, 0.5f, 1.0f, 2.5f, 0.6f, 0.0f, 0.5f };
AXFX_DELAY lbl_8018EE00 = { {0}, {499, 501, 10}, {15, 15, 0}, {100, 100, 0} };
AXFX_REVERBHI lbl_8018EE60 = { {0}, 0, 0.9f, 1.0f, 4.0f, 0.2f, 0.0f, 0.5f };

// Uninitialised data, defined last-first (CodeWarrior lays it out in reverse).
UAudMemStack lbl_801F5D88;              // the effect memory
UAudMemStackBlock lbl_801F5C08[32];     // the effect memory's blocks
s8 lbl_80281490 = -1;                   // the mode set up (-1: none yet)
void* lbl_802820E0;                     // the effect's state
AXAuxCallback lbl_802820DC;             // the effect running
u8* lbl_802820D8;                       // the effect memory's buffer

// The effects library's allocator hooks: the effect memory, never freed piecemeal.
void* fn_800AF114(u32 uSize) {
    return fn_800B5AAC(&lbl_801F5D88, uSize);
}

void fn_800AF140(void* p) {
}

// Switch the effect to nMode: 1 the delay, 0 or 2 a reverb. The effect memory starts over each
// time.
void fn_800AF144(s8 nMode) {
    AXFX_REVERBHI* pReverb;
    int bEnabled;
    int bOk;

    if (nMode != lbl_80281490) {
        lbl_80281490 = nMode;
        bEnabled = OSDisableInterrupts();
        fn_800B5A14(&lbl_801F5D88, lbl_802820D8, 0x20000, 32, lbl_801F5C08, 4);
        if (lbl_80281490 == 1) {
            lbl_802820E0 = &lbl_8018EE00;
            lbl_802820DC = AXFXDelayCallback;
            bOk = AXFXDelayInit(&lbl_8018EE00);
        } else {
            pReverb = (lbl_80281490 == 0) ? &lbl_8018EC20 : &lbl_8018EE60;
            lbl_802820E0 = pReverb;
            lbl_802820DC = AXFXReverbHiCallback;
            bOk = AXFXReverbHiInit(pReverb);
        }
        if (bOk == 1) {
            AXRegisterAuxACallback(lbl_802820DC, lbl_802820E0);
        }
        OSRestoreInterrupts(bEnabled);
    }
}

int fn_800AF224(void) {
    lbl_802820D8 = fn_800B5BD8(0x20000);
    AXFXSetHooks(fn_800AF114, fn_800AF140);
    return 1;
}

// Pick the effect: the mode 0 reverb when bOn is 0, the mode 2 reverb for nKind 8 on hole index 2
// (fn_80015464), otherwise the delay.
u8 fn_800AF264(u8 nKind, u8 bOn) {
    s8 nHole;                           // fake match: EA keeps the hole index as a signed byte
    s8 nMode;

    nHole = fn_80015464();
    if (bOn == 0) {
        nMode = 0;
    } else if (nKind == 8 && nHole == 2) {
        nMode = 2;
    } else {
        nMode = 1;
    }
    fn_800AF144(nMode);
    return 1;
}

void fn_800AF2D8(void) {
}

void fn_800AF2DC(u8 bMute) {
    if (bMute != 0) {
        AXRegisterAuxACallback(NULL, NULL);
        return;
    }
    AXRegisterAuxACallback(lbl_802820DC, lbl_802820E0);
}

void fn_800AF31C(void) {
}

void fn_800AF320(void) {
}
