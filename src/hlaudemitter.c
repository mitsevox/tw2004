// hlaudemitter.c (TW06's name, by structure: golf/audio/engine/hl/hlaudemitter.c, after
// hlaudvoice.c and before UAudContainers.c): the sound engine's emitter instances, a pool of 256
// (0x34 bytes each, lbl_801F2740) found by id (fn_800AD674). Most calls check the instance exists,
// then pass on to the functions at 0x800A8200-0x800A8524. Its extent is its data: it is the first
// to use the .bss at 0x801F2668 and the .sdata2 block 0x80284008-0x80284018.
// Not yet decompiled: the functions below are the sweep's.

#include "core/audtrack.h"

// The emitter functions below 0x800A85CC (not placed yet).
void fn_800A8248(u8 nId, s32 n1, s32 n2);
void fn_800A82CC(u8 nId, s32 n1, s32 n2);
void fn_800A834C(void);
void fn_800A8394(u8 nId, s32 n1, s32 n2, s32 n3);
void fn_800A8424(u8 nId, s32 n1, f32 f);
void fn_800A84A4(u8 nId, s32 n1, f32 f);

AudInstance* fn_800AD674(u8 nId);

void fn_800AD1C4(void) {
}

void fn_800AD950(u8 nId, s32 n1, s32 n2) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8248(nId, n1, n2);
    }
}

void fn_800AD9AC(u8 nId, s32 n1, s32 n2) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A82CC(nId, n1, n2);
    }
}

void fn_800ADA08(void) {
    fn_800A834C();
}

void fn_800ADA28(u8 nId, s32 n1, s32 n2, s32 n3) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8394(nId, n1, n2, n3);
    }
}

void fn_800ADA94(u8 nId, s32 n1, f32 f) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A8424(nId, n1, f);
    }
}

void fn_800ADAF0(u8 nId, s32 n1, f32 f) {
    if (fn_800AD674(nId) != NULL) {
        fn_800A84A4(nId, n1, f);
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
