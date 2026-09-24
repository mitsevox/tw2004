// hlaudemitter.c (TW06's name, by structure: golf/audio/engine/hl/hlaudemitter.c, after
// hlaudvoice.c and before UAudContainers.c): the sound engine's emitter instances, a pool of 256
// (0x34 bytes each, lbl_801F2740) found by id (fn_800AD674). Most calls check the instance exists,
// then pass on to the functions at 0x800A8200-0x800A8524. Its extent is its data: it is the first
// to use the .bss at 0x801F2668 and the .sdata2 block 0x80284008-0x80284018.
// Not yet decompiled: the functions below are the sweep's.

#include "core/audtrack.h"

AudInstance* fn_800AD674(u8 nId);
void fn_800AD800(u8 nId, f32* pPos, f32* pLast, u8 nView);
void fn_800ACB98(void);                 // hlaudvoice.c
void fn_800AF320(void);
void fn_800B0434(void);                 // startUp.c

// Runs fn_800AD450 on every instance in use, then empties every emitter. Always 1.
int fn_800AD0C4(void) {
    AudInstance* pInst;
    AudInstance* pNext;
    s32 i;

    for (pInst = lbl_801F2668.pActive; pInst != NULL; pInst = pNext) {
        pNext = pInst->pNextActive;
        fn_800AD450(pInst->nId);
    }
    for (i = 0; i < 32; i++) {
        lbl_801F2668.apFirst[i] = NULL;
        lbl_801F2668.anSound[i] = 0;
    }
    return 1;
}

void fn_800AD1C4(void) {
}

// Once a frame: hands every instance's commands to its entry and clears them (an instance with
// n24 1 sends its position again), then runs the rest of the sound engine.
void fn_800AD1C8(void) {
    AudInstance* pInst;

    for (pInst = lbl_801F2668.pActive; pInst != NULL; pInst = pInst->pNextActive) {
        fn_800A7CA4(pInst->nId, pInst->pCmd->uOn, pInst->pCmd->uOff, pInst->pCmd->auParams,
                    pInst->pCmd->aPos, pInst->pCmd->uChanged);
        pInst->pCmd->uOn = 0;
        pInst->pCmd->uOff = 0;
        pInst->pCmd->uChanged = 0;
        if (pInst->n24 == 1) {
            fn_800AD800(pInst->nId, pInst->vPos, NULL, 0);
        }
    }
    fn_800A9AC8();
    fn_800ACB98();
    fn_800AF320();
    fn_800B0434();
    lbl_80282018++;
}

// Whether bit nTrack of an instance's u22 is set; 0 for no instance.
u8 fn_800AD618(u8 nId, u8 nTrack) {
    AudInstance* pInst = fn_800AD674(nId);
    if (pInst == NULL) {
        return 0;
    }
    return (pInst->u22 & (1 << nTrack)) != 0;
}

// The instance with an id; id 0xFF is none.
AudInstance* fn_800AD674(u8 nId) {
    AudInstance* pInst = &lbl_801F2740[nId];
    if (nId == 0xFF) {
        return NULL;
    }
    return pInst;
}

// Switches an instance's track on (bOn 1: also marked in u22) or off.
void fn_800AD698(u8 nId, u8 nTrack, u8 bOn) {
    AudInstance* pInst = fn_800AD674(nId);
    u8 uBit = 1 << nTrack;
    if (pInst != NULL) {
        if (bOn == 1) {
            pInst->pCmd->uOn |= uBit;
            pInst->u22 |= uBit;
        } else {
            pInst->pCmd->uOff |= uBit;
        }
        pInst->pCmd->uChanged |= 0x200;
    }
}

// Sets which of an instance's tracks are on (the rest off), in u22 too.
void fn_800AD734(u8 nId, int n) {
    AudInstance* pInst = fn_800AD674(nId);
    if (pInst != NULL) {
        pInst->pCmd->uOn = n;
        pInst->pCmd->uOff = ~n;
        pInst->pCmd->uChanged |= 0x200;
        pInst->u22 = n;
    }
}

// Sets the parameters of one of an instance's tracks.
void fn_800AD790(u8 nId, u8 nTrack, u32 uParams) {
    AudInstance* pInst = fn_800AD674(nId);
    if (pInst != NULL) {
        pInst->pCmd->auParams[nTrack] = uParams;
        pInst->pCmd->uChanged |= (u8)(1 << nTrack);
    }
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

// The calls below do the same for every instance of an emitter.
void fn_800ADB4C(s16 nEmitter, u8 nTrack, u8 bOn) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        fn_800AD698(pInst->nId, nTrack, bOn);
    }
}

void fn_800ADBC0(s16 nEmitter, f32* pPos, f32* pLast, u8 nView) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        fn_800AD800(pInst->nId, pPos, pLast, nView);
    }
}

// The emitter's own sound first (fn_800ADA08), when it has instances.
void fn_800ADC44(s16 nEmitter, u8 nTrack, u8 n) {
    AudInstance* pInst = lbl_801F2668.apFirst[nEmitter];
    if (pInst != NULL) {
        fn_800ADA08(lbl_801F2668.anSound[nEmitter], nTrack, n);
    }
    for (; pInst != NULL; pInst = pInst->pNext) {
        fn_800AD9AC(pInst->nId, nTrack, n);
    }
}

void fn_800ADCD0(s16 nEmitter, u8 nTrack, u8 n, int bCheck) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        fn_800ADA28(pInst->nId, nTrack, n, bCheck);
    }
}

void fn_800ADD54(s16 nEmitter, u8 nTrack, f32 fVolume) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        fn_800ADA94(pInst->nId, nTrack, fVolume);
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

// Sets a four-float vector to (0, 0, 0, 1).
void fn_800ADE54(f32* pVec) {
    pVec[2] = 0.0f;
    pVec[1] = 0.0f;
    pVec[0] = 0.0f;
    pVec[3] = 1.0f;
}
