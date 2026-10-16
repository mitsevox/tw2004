// HLAudMaster.c: the sound engine's master mix: the volume and mute bit of each submix channel the
// tracks play through, the output mode, and the update-rate scale the sequencer's event delays are
// fitted to. It opens with two small helpers: a bank sound by number, and the fraction product.

#include "core/audtrack.h"
#include "core/startup.h"

f32 gMasUpdateRateScale = 1.0f;         // the update rate given at start-up over 60 (1.0 at 60)

f32 gMasSubmixVolumes[MAS_NUM_SUBMIX_CHANS]; // each submix channel's volume, 1.0 unchanged

u8 gMasOutputMode;                      // the sound output mode (2); nothing reads it
u32 gMasMuteMask;                       // bit n set: submix channel n is muted

// A bank sound by its number: bank 0's from 0 up, bank 1's from -1 down.
TEmitter* Ses_GetEmitterTemplateFromID(s16 tmplID) {
    AudBank* pBank;

    if (tmplID >= 0) {
        pBank = gSesBank0;
    } else {
        pBank = gSesBank1;
        tmplID = -1 - tmplID;
    }
    return pBank->apSounds[tmplID];
}

f32 audfrac_Mul(f32 v1, f32 v2) {
    return v1 * v2;
}

// The master's start-up step in Aud_InitOnce: every submix channel at volume 1.0 (its tracks
// unchanged), none muted, output mode 2. Always 1.
u8 Mas_InitModule(void) {
    u8 ok;
    s32 i;

    ok = 1;
    gMasOutputMode = 2;
    gMasMuteMask = 0;
    for (i = 0; i < MAS_NUM_SUBMIX_CHANS; i++) {
        gMasSubmixVolumes[i] = 1.0f;
    }
    return ok;
}

// The master's step at the end of Ses_Init: nothing to do. Always 1.
u8 Mas_InitSession(void) {
    return 1;
}

// The master's step in Ses_Exit: nothing to do.
void Mas_ExitSession(void) {
}

// Called by Aud_InitOnce with the frame rate it is given (always 60): keeps nFps / 60, 1.0 at 60,
// by which Seq_Check scales every event delay of a loaded template (Mas_GetUpdateRateScale).
void Mas_SetUpdateRate(u8 nFps) {
    gMasUpdateRateScale = nFps != 60 ? nFps / 60.0f : 1.0f;
}

// Stores the sound output mode (Aud_SetOutputmode, which GameAudio calls with 2, the start-up
// value). Nothing reads it.
void Mas_SetOutputMode(u8 outputMode) {
    gMasOutputMode = outputMode;
}

// Sets submix channel submixChan's volume, which scales every track played through it
// (Mas_GetSubmix); 1.0 leaves them unchanged.
void Mas_SetSubmixChan(u8 submixChan, f32 attn) {
    gMasSubmixVolumes[submixChan] = attn;
}

// Sets the volumes of submix channels 0..nchans-1 from attns.
void Mas_SetSubmixAll(u8 nchans, f32* attns) {
    Mem_cpy(gMasSubmixVolumes, attns, nchans * sizeof(f32));
}

// Sets which submix channels are muted: bit n of chanmask set mutes channel n, whose volume
// Mas_GetSubmix then gives as 0.
void Mas_SetSubmixMuteAll(u32 chanmask) {
    gMasMuteMask = chanmask;
}
