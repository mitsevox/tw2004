// HLAudMaster.c (TW07's golf/audio/engine/hl/HLAudMaster.c, which has Mas_SetSubmixChan,
// Mas_SetSubmixAll and Mas_SetSubmixMuteAll): the sound engine's master settings, the volume of
// each of the 32 curves (submix channels) and their mute bits. Split from hlaudmovie.c by its data: its .sdata (0x80281460) and .sbss (0x80282060-
// 0x80282065) each end in padding to the next file's 8-aligned start, which one file's packed
// globals cannot leave, and fn_800A8754 after it is the first to use the next file's .sbss.
// fn_800A85CC and fn_800A85FC come before the Mas_ functions and have no data of their own:
// kept here, as nothing places them elsewhere.

#include "core/audtrack.h"
#include "core/startup.h"

f32 lbl_80281460 = 1.0f;                // fn_800A86BC's rate

f32 lbl_801F17D0[32];

u8 lbl_80282064;
s32 lbl_80282060;

// A sound by its number: bank 0's from 0 up, bank 1's from -1 down.
AudSound* fn_800A85CC(s16 nSound) {
    AudBank* pBank;

    if (nSound >= 0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
        nSound = -1 - nSound;
    }
    return pBank->apSounds[nSound];
}

f32 fn_800A85FC(f32 fVolume, f32 fCurve) {
    return fVolume * fCurve;
}

u8 fn_800A8604(void) {
    s32 i;

    lbl_80282064 = 2;
    lbl_80282060 = 0;
    for (i = 0; i < 32; i++) {
        lbl_801F17D0[i] = 1.0f;
    }
    return 1;
}

u8 fn_800A86B0(void) {
    return 1;
}

void fn_800A86B8(void) {
}

void fn_800A86BC(u8 nRate) {
    lbl_80281460 = nRate != 60 ? nRate / 60.0f : 1.0f;
}

void fn_800A8700(u8 n) {
    lbl_80282064 = n;
}

void Mas_SetSubmixChan(u8 nCurve, f32 fVolume) {
    lbl_801F17D0[nCurve] = fVolume;
}

void Mas_SetSubmixAll(u8 nCurves, f32* pVolumes) {
    Mem_cpy(lbl_801F17D0, pVolumes, nCurves * sizeof(f32));
}

void fn_800A874C(s32 n) {
    lbl_80282060 = n;
}
