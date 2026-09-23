// hlaudemitter.c (TW06's name, by structure: golf/audio/engine/hl/hlaudemitter.c, after
// hlaudvoice.c and before UAudContainers.c): the sound engine's emitter instances, a pool of 256
// (0x34 bytes each, lbl_801F2740) found by id (fn_800AD674). Most calls check the instance exists,
// then pass on to the functions at 0x800A8200-0x800A8524. Its extent is its data: it is the first
// to use the .bss at 0x801F2668 and the .sdata2 block 0x80284008-0x80284018.
// Not yet decompiled: the functions below are the sweep's.

#include "core/audtrack.h"

AudInstance* fn_800AD674(u8 nId);

void fn_800AD1C4(void) {
}

// The calls below pass on to AudTable.c's entry nId (the same number as the instance).
void fn_800AD950(u8 nId, u8 nTrack, u8 n) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8248(nId, nTrack, n);
    }
}

void fn_800AD9AC(u8 nId, u8 nTrack, u8 n) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A82CC(nId, nTrack, n);
    }
}

void fn_800ADA08(s16 nSound, u8 nTrack, u8 n) {
    fn_800A834C(nSound, nTrack, n);
}

void fn_800ADA28(u8 nId, u8 nTrack, u8 n, int bCheck) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8394(nId, nTrack, n, bCheck);
    }
}

void fn_800ADA94(u8 nId, u8 nTrack, f32 fVolume) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8424(nId, nTrack, fVolume);
    }
}

void fn_800ADAF0(u8 nId, u8 nTrack, f32 fPitch) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A84A4(nId, nTrack, fPitch);
    }
}

// Clears bit nBit of an instance's u22 and tells its callback.
void fn_800ADDC8(u8 nId, u8 nBit, s32 n) {
    AudInstance* pInst;
    void (*pfnCallback)(u8 nId, u8 nBit, s32 n);

    pInst = fn_800AD674(nId);
    if (pInst != NULL) {
        pInst->u22 &= (u8)~(1 << nBit);
        pfnCallback = pInst->pfnCallback;
        if (pfnCallback != NULL) {
            pfnCallback(nId, nBit, n);
        }
    }
}
