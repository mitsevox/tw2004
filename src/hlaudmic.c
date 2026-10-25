// hlaudmic.c: the sound engine's listeners, one per view: their memory block, how many a session has
// (which the emitters' 3D sound reads) and each listener's reverb preset.

#include "core/audtrack.h"
#include "core/startup.h"

AudMicBlock* gMicData;                  // the listeners' block (nothing reads it)
u8 gMicCount;                           // listeners in the session, one per view

// The listener step of Aud_InitOnce: allocates the listeners' block from the sound engine's memory
// and clears it. Returns 0 when the memory is full, else 1. Nothing reads the block in this build.
u8 Mic_InitModule(void) {
    u8 ok;

    ok = 0;
    gMicData = AudMem_Alloc(sizeof(AudMicBlock));
    if (gMicData != NULL) {
        Mem_set(gMicData, 0, sizeof(AudMicBlock));
        ok = 1;
    }
    return ok;
}

// The listener step of Ses_Init: stores the session's listener count, one per view, for the
// emitters' 3D sound (hlaudemitter.c). The session and subsession ids are not read. Always 1.
u8 Mic_InitSession(u8 sessID, u8 subsessID, u8 nMics) {
    gMicCount = nMics;
    return 1;
}

// The listener step of Ses_Exit: empty in this build.
void Mic_ExitSession(void) {
}

// Gives listener micID reverb preset presetID by passing the preset on to Rvb_SetPreset, which is
// empty in this build. micID is not read. Aud_MicSetRvbPreset calls it only when the preset changes.
void Mic_SetRvbPreset(u8 micID, u8 presetID) {
    Rvb_SetPreset(presetID);
}
