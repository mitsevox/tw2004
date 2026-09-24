// hlaudemitter.c (TW06's name, by structure: golf/audio/engine/hl/hlaudemitter.c, after
// hlaudvoice.c and before UAudContainers.c): the sound engine's emitter instances, a pool of 256
// (0x34 bytes each, lbl_801F2740) found by id (fn_800AD674). Most calls check the id (0xFF: none),
// then pass on to the functions at 0x800A8200-0x800A8524. Its extent is its data: it is the first
// to use the .bss at 0x801F2668 and the .sdata2 block 0x80284008-0x80284018.
// An instance's pCmd is its AudTable.c entry (fn_800A7C30's AudSource, the same number).

#include "core/audtrack.h"
#include "golfer.h"
#include "unsorted/cull.h"

AudInstance* fn_800AD674(u8 nId);
void fn_800ADE54(f32* pVec);
void fn_800AD800(u8 nId, f32* pPos, f32* pLast, u8 nView);
void Voc_Cycle(void);                 // hlaudvoice.c
void fn_800AF320(void);
void fn_800B0434(void);                 // startUp.c

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80284008), before the 0.0f fn_800AD800 uses first; its body is unknown.
static f32 hlaudemitter_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets up the instances, all on the free list, and empties every emitter. Always 1.
int fn_800ACECC(void) {
    AudInstance* pInst;
    int i;

    pInst = lbl_801F2740;
    fn_80005AE8(lbl_801F2740, 0, sizeof(lbl_801F2740));
    for (i = 0; i < 256; i++, pInst++) {
        pInst->nId = i;
        pInst->pPrevActive = pInst - 1;
        pInst->pNextActive = pInst + 1;
    }
    lbl_801F2668.pFree = lbl_801F2740;
    lbl_801F2668.pFree->pPrevActive = NULL;
    lbl_801F2668.pFreeTail = &lbl_801F2740[255];
    lbl_801F2668.pFreeTail->pNextActive = NULL;
    lbl_801F2668.pActive = NULL;
    lbl_801F2668.pActiveTail = NULL;
    lbl_801F2668.nActive = 0;
    for (i = 0; i < 32; i++) {
        lbl_801F2668.apFirst[i] = NULL;
        lbl_801F2668.anSound[i] = 0;
    }
    lbl_801F2668.uFlags |= 1;
    return 1;
}

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
        fn_800A7CA4(pInst->nId, pInst->pCmd->u0, pInst->pCmd->u1, pInst->pCmd->auParams,
                    pInst->pCmd->aPos, pInst->pCmd->uChanged);
        pInst->pCmd->u0 = 0;
        pInst->pCmd->u1 = 0;
        pInst->pCmd->uChanged = 0;
        if (pInst->n24 == 1) {
            fn_800AD800(pInst->nId, pInst->vPos, NULL, 0);
        }
    }
    fn_800A9AC8();
    Voc_Cycle();
    fn_800AF320();
    fn_800B0434();
    lbl_80282018++;
}

// Takes a free instance for sound nSound: onto the tail of the active list and, when nEmitter is
// not negative, the tail of that emitter's list (its first instance sets the emitter's sound). Its
// source's commands are cleared and handed on once. Returns its id, or 0xFF when all 256 are in use.
u8 fn_800AD280(s16 nSound, s16 nEmitter, int n24, int n28, void (*pfnCallback)(u8 nId, u8 nBit, s32 n)) {
    AudInstance* pInst;
    AudInstance* p;

    if (lbl_801F2668.nActive >= 256) {
        return 0xFF;
    }
    pInst = lbl_801F2668.pFree;
    lbl_801F2668.pFree = pInst->pNextActive;
    if (lbl_801F2668.pFree != NULL) {
        lbl_801F2668.pFree->pPrevActive = NULL;
    } else {
        lbl_801F2668.pFreeTail = NULL;
    }
    // port: EA passes five more arguments than fn_800A7C30 takes
    pInst->pCmd = ((AudSource* (*)(u8, s16, int, int, int, int, int))fn_800A7C30)(pInst->nId, nSound, 0, 0,
                                                                                  0, 0, 0);
    pInst->pCmd->u0 = 0;
    pInst->pCmd->u1 = 0;
    pInst->pCmd->uChanged = 0;
    pInst->u22 = 0;
    pInst->unk23 = 0;
    pInst->n24 = n24;
    pInst->n28 = n28;
    pInst->pfnCallback = pfnCallback;
    fn_800ADE54(pInst->vPos);
    pInst->pPrevActive = lbl_801F2668.pActiveTail;
    pInst->pNextActive = NULL;
    if (pInst->pPrevActive != NULL) {
        pInst->pPrevActive->pNextActive = pInst;
    } else {
        lbl_801F2668.pActive = pInst;
    }
    lbl_801F2668.pActiveTail = pInst;
    lbl_801F2668.nActive++;
    if (nEmitter >= 0) {
        p = lbl_801F2668.apFirst[nEmitter];
        if (p == NULL) {
            lbl_801F2668.apFirst[nEmitter] = pInst;
            lbl_801F2668.anSound[nEmitter] = nSound;
        } else {
            for (; p != NULL; p = p->pNext) {
                if (p->pNext == NULL) {
                    p->pNext = pInst;
                    break;
                }
            }
        }
    }
    pInst->nEmitter = nEmitter;
    pInst->pNext = NULL;
    fn_80005AE8(pInst->pCmd->auParams, 0, sizeof(pInst->pCmd->auParams));
    fn_80005AE8(pInst->pCmd->aPos, 0, sizeof(pInst->pCmd->aPos));
    fn_800A7CA4(pInst->nId, 0, 0, pInst->pCmd->auParams, pInst->pCmd->aPos, 0);
    return pInst->nId;
}

// Frees instance nId: out of the active list onto the head of the free list, and out of its
// emitter's list (the emitter's sound is cleared with its last instance).
void fn_800AD450(u8 nId) {
    AudInstance* pInst = &lbl_801F2740[nId];
    AudInstance* p;
    AudInstance* pPrev;

    if (nId != 0xFF && (lbl_801F2668.uFlags & 1)) {
        fn_800A8200(nId);
        if (pInst == lbl_801F2668.pActiveTail) {
            if (pInst->pPrevActive != NULL) {
                lbl_801F2668.pActiveTail = pInst->pPrevActive;
                pInst->pPrevActive->pNextActive = NULL;
            } else {
                lbl_801F2668.pActive = NULL;
                lbl_801F2668.pActiveTail = NULL;
            }
        } else if (pInst == lbl_801F2668.pActive) {
            if (pInst->pNextActive != NULL) {
                lbl_801F2668.pActive = pInst->pNextActive;
                pInst->pNextActive->pPrevActive = NULL;
            } else {
                lbl_801F2668.pActive = NULL;
                lbl_801F2668.pActiveTail = NULL;
            }
        } else {
            pInst->pPrevActive->pNextActive = pInst->pNextActive;
            pInst->pNextActive->pPrevActive = pInst->pPrevActive;
        }
        if (lbl_801F2668.pFree != NULL) {
            lbl_801F2668.pFree->pPrevActive = pInst;
        } else {
            lbl_801F2668.pFreeTail = pInst;
        }
        pInst->pNextActive = lbl_801F2668.pFree;
        pInst->pPrevActive = NULL;
        lbl_801F2668.pFree = pInst;
        lbl_801F2668.nActive--;
        if (pInst->nEmitter >= 0) {
            pPrev = NULL;
            for (p = lbl_801F2668.apFirst[pInst->nEmitter]; p != NULL; p = p->pNext) {
                if (p == pInst) {
                    if (pPrev != NULL) {
                        pPrev->pNext = pInst->pNext;
                    } else {
                        lbl_801F2668.apFirst[pInst->nEmitter] = pInst->pNext;
                    }
                    break;
                }
                pPrev = p;
            }
            if (lbl_801F2668.apFirst[pInst->nEmitter] == NULL) {
                lbl_801F2668.anSound[pInst->nEmitter] = 0;
            }
        }
        pInst->nEmitter = -1;
        pInst->pNext = NULL;
    }
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
            pInst->pCmd->u0 |= uBit;
            pInst->u22 |= uBit;
        } else {
            pInst->pCmd->u1 |= uBit;
        }
        pInst->pCmd->uChanged |= 0x200;
    }
}

// Sets which of an instance's tracks are on (the rest off), in u22 too.
void fn_800AD734(u8 nId, int n) {
    AudInstance* pInst = fn_800AD674(nId);
    if (pInst != NULL) {
        pInst->pCmd->u0 = n;
        pInst->pCmd->u1 = ~n;
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

// Moves instance nId to pPos (NULL: where it is), its old position into pLast (when not NULL),
// and works out where each view in use hears it: with n28 0 in the view's camera space, else as
// it is for view nView and far above (0, 10000, 0) for the other.
// EA bug: with pPos NULL and no view in use, pPos is still NULL at the last Vec3Copy.
void fn_800AD800(u8 nId, f32* pPos, f32* pLast, u8 nView) {
    int i;
    AudInstance* pInst;
    CamLens* pLens;
    f32* pRel;
    Vec4 vRel;

    pInst = fn_800AD674(nId);
    if (pInst != NULL) {
        for (i = 0; i < 2; i++) {
            if ((gSession.nGameType == 3 || fn_800170A0(i)) && fn_80017004(i) != NULL) {
                pLens = ((Camera*)fn_80017004(i))->unk10;
                if (pPos == NULL) {
                    pPos = pInst->vPos;
                }
                if (pInst->n28 == 0) {
                    fn_800BAD60(pLens->m44, (Vec4*)pPos, &vRel);
                    pRel = &vRel.x;
                } else if (i == nView) {
                    pRel = pPos;
                } else {
                    pRel = &vRel.x;
                    vRel.x = 0.0f;
                    vRel.y = 10000.0f;
                    vRel.z = 0.0f;
                }
                pInst->pCmd->aPos[i][0] = pRel[0];
                pInst->pCmd->aPos[i][1] = pRel[1];
                pInst->pCmd->aPos[i][2] = pRel[2];
            }
        }
        if (pLast != NULL) {
            Vec3Copy(pInst->vPos, pLast);
        }
        Vec3Copy(pPos, pInst->vPos);
        pInst->pCmd->uChanged |= 0x400;
    }
}

// The calls below pass on to AudTable.c's entry nId (the same number as the instance);
// fn_800ADA08 passes a sound number instead (its definition, shared by all its instances).
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
