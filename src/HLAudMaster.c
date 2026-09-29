// HLAudMaster.c (TW07's golf/audio/engine/hl/HLAudMaster.c, which has Mas_SetSubmixChan,
// Mas_SetSubmixAll and Mas_SetSubmixMuteAll): the sound engine's master settings, the volume of
// each of the 32 curves (submix channels) and their mute bits. Split from hlaudmovie.c by its
// data: its .sdata (0x80281460) and .sbss (0x80282060-0x80282065) each end in padding to the next
// file's 8-aligned start, which one file's packed globals cannot leave, and Mic_InitModule after it
// is the first to use the next file's .sbss.
// Ses_GetEmitterTemplateFromID and audfrac_Mul come before the Mas_ functions and have no data of their own:
// kept here, as nothing places them elsewhere.

#include "core/audtrack.h"
#include "core/startup.h"

f32 lbl_80281460 = 1.0f;                // Mas_SetTickRate's rate

f32 lbl_801F17D0[32];

u8 lbl_80282064;
s32 lbl_80282060;

// A sound (EA: an emitter template) by its number: bank 0's from 0 up, bank 1's from -1 down.
AudSound* Ses_GetEmitterTemplateFromID(s16 nSound) {
    AudBank* pBank;

    if (nSound >= 0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
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

    lbl_80282064 = 2;
    lbl_80282060 = 0;
    for (i = 0; i < 32; i++) {
        lbl_801F17D0[i] = 1.0f;
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

// Scales the sequencer's event delays (read through fn_800AB39C) for nRate engine ticks a second:
// nRate / 60, so 1.0 at 60, which is what Aud_InitOnce is always given.
void Mas_SetTickRate(u8 nRate) {
    lbl_80281460 = nRate != 60 ? nRate / 60.0f : 1.0f;
}

// Stores the sound output mode (Aud_SetOutputmode; 2 from Mas_InitModule). Nothing in this build
// reads it.
void Mas_SetOutputMode(u8 n) {
    lbl_80282064 = n;
}

// Sets submix channel nCurve's volume (0..31, 1.0 full), which scales every track played through it
// (Mas_GetSubmix).
void Mas_SetSubmixChan(u8 nCurve, f32 fVolume) {
    lbl_801F17D0[nCurve] = fVolume;
}

// Sets the volumes of submix channels 0..nCurves-1 from pVolumes.
void Mas_SetSubmixAll(u8 nCurves, f32* pVolumes) {
    Mem_cpy(lbl_801F17D0, pVolumes, nCurves * sizeof(f32));
}

// Sets the mute mask: bit n set mutes submix channel n (Mas_IsChanMuted).
void Mas_SetSubmixMuteAll(s32 n) {
    lbl_80282060 = n;
}
