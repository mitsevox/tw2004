// HLAudMaster.c (TW07's golf/audio/engine/hl/HLAudMaster.c, whose Mas_ functions these are, in its
// order; TW06 lists hl/hlaudmaster.c): the sound engine's master settings, the volume of each of
// the 32 submix channels (EA's name; our tracks call them curves) and their mute bits, the output
// mode, and the rate the sequencer's delays are scaled to. Split from hlaudmovie.c by its data:
// its .sdata (0x80281460) and .sbss (0x80282060-0x80282065) each end in padding to the next file's
// 8-aligned start, which one file's packed globals cannot leave, and Mic_InitModule after it is
// the first to use the next file's .sbss.
// Ses_GetEmitterTemplateFromID (TW07: HLAudSession.c's) and audfrac_Mul (TW07: an inline in
// UAudFrac.h) come before the Mas_ functions and have no data of their own: kept here, as nothing
// places them elsewhere (AudTable.c, EA's hlaudemitter.c, ends just before them).

#include "core/audtrack.h"
#include "core/startup.h"

f32 gMasTickRateScale = 1.0f;           // nRate / 60 from Mas_SetTickRate (1.0 at 60)

f32 gMasSubmixVolumes[32];              // each submix channel's volume, 1.0 full

u8 gMasOutputMode;                      // Mas_SetOutputMode's (2), read nowhere
s32 gMasMuteMask;                       // bit n set: submix channel n muted

// A sound (EA: an emitter template) by its number: bank 0's from 0 up, bank 1's from -1 down.
AudSound* Ses_GetEmitterTemplateFromID(s16 nSound) {
    AudBank* pBank;

    if (nSound >= 0) {
        pBank = gSesBank0;
    } else {
        pBank = gSesBank1;
        nSound = -1 - nSound;
    }
    return pBank->apSounds[nSound];
}

// The engine's fraction product (EA's audfrac_Mul, a UAudFrac.h inline, here out of line): volumes
// and attenuations times each other.
f32 audfrac_Mul(f32 fVolume, f32 fCurve) {
    return fVolume * fCurve;
}

// The master's start-up step in Aud_InitOnce: every submix channel at full volume (1.0), none
// muted, output mode 2. Always 1.
u8 Mas_InitModule(void) {
    s32 i;

    gMasOutputMode = 2;
    gMasMuteMask = 0;
    for (i = 0; i < 32; i++) {
        gMasSubmixVolumes[i] = 1.0f;
    }
    return 1;
}

// The master's step in Ses_Init: nothing to do. Always 1.
u8 Mas_InitSession(void) {
    return 1;
}

// The master's step in Ses_Exit: empty in this build.
void Mas_ExitSession(void) {
}

// Scales the sequencer's event delays (read through Mas_GetUpdateRateScale) for nRate engine ticks a second:
// nRate / 60, so 1.0 at 60, which is what Aud_InitOnce is always given.
void Mas_SetTickRate(u8 nRate) {
    gMasTickRateScale = nRate != 60 ? nRate / 60.0f : 1.0f;
}

// Stores the sound output mode (Aud_SetOutputmode; 2 from Mas_InitModule). Nothing in this build
// reads it.
void Mas_SetOutputMode(u8 nMode) {
    gMasOutputMode = nMode;
}

// Sets submix channel nCurve's volume (0..31, 1.0 full), which scales every track played through it
// (Mas_GetSubmix).
void Mas_SetSubmixChan(u8 nCurve, f32 fVolume) {
    gMasSubmixVolumes[nCurve] = fVolume;
}

// Sets the volumes of submix channels 0..nCurves-1 from pVolumes.
void Mas_SetSubmixAll(u8 nCurves, f32* pVolumes) {
    Mem_cpy(gMasSubmixVolumes, pVolumes, nCurves * sizeof(f32));
}

// Sets the mute mask: bit n of uMask set mutes submix channel n (Mas_IsChanMuted).
void Mas_SetSubmixMuteAll(s32 uMask) {
    gMasMuteMask = uMask;
}
