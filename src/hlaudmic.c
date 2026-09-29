// hlaudmic.c (EA's name: TW06 lists golf/audio/engine/hl/hlaudmic.c, and TW07's HLAudMic.c has
// these Mic_ functions in this order, among others): the sound engine's listeners (EA:
// microphones), their count per session and a reverb preset each. The first of three EA files
// that follow HLAudMaster.c in alphabetical link order (TW06: hlaudmic.c, hlaudmovie.c,
// hlaudsession.c). Its extent is its data: HLAudMaster.c before it ends its .sbss at 0x80282065
// in padding to this file's 8-aligned start (0x80282068); its two globals fill 0x80282068-6F,
// and the session's (hlaudsession.c) follow; the Mov_ functions after it are not in TW07's
// HLAudMic.c.

#include "core/audtrack.h"
#include "core/startup.h"

void Rvb_SetPreset(u8 nPreset);         // AudReverb.c

// Defined last-address-first, as CodeWarrior lays out .sbss.
AudBlock48* gMicData;                   // the listeners' block (nothing reads it)
u8 gMicCount;                           // listeners in the session, one per view

// Allocates and clears the listeners' 0x48-byte block (gMicData) from the sound engine's memory
// (AudMem_Alloc), the listener step of Aud_InitOnce. Returns 0 when the memory is full, else 1.
// Nothing reads the block in this build.
u8 Mic_InitModule(void) {
    u8 bOk;

    bOk = 0;
    gMicData = AudMem_Alloc(sizeof(AudBlock48));
    if (gMicData != NULL) {
        Mem_set(gMicData, 0, sizeof(AudBlock48));
        bOk = 1;
    }
    return bOk;
}

// A session's listeners (Ses_Init): stores nListeners, one per view, which AudTable.c's 3D sound
// reads (gMicCount). The session and subsession ids are not read. Always 1.
u8 Mic_InitSession(u8 nSession, u8 nSubsession, u8 nListeners) {
    gMicCount = nListeners;
    return 1;
}

// The listeners' step in Ses_Exit: empty in this build.
void Mic_ExitSession(void) {
}

// Passes listener nMic's reverb preset nPreset on to AudReverb.c (Rvb_SetPreset, empty in this
// build). Aud_MicSetRvbPreset calls it only when the preset changes.
void Mic_SetRvbPreset(u8 nMic, u8 nPreset) {
    Rvb_SetPreset(nPreset);
}
