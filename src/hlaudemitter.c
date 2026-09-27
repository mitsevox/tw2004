// hlaudemitter.c (our name; EA's file is HLAudEmitterPool.c: TW07's
// golf/audio/engine/special/HLAudEmitterPool.c has these Aud_Emi functions in the same order, and
// TW06 lists special/hlaudemitterpool.c too. EA's hl/hlaudemitter.c is our AudTable.c.): the sound
// engine's emitter instances, the game's handles on playing sounds. A pool of 256 (0x34 bytes
// each, lbl_801F2740) found by id (Aud_CheckEmitterInstance; 0xFF: none); instances of the same
// sound can be grouped under an emitter (EA: an alias) and driven together (Aud_EmiAlias*). Most
// calls check the id, then queue the command in the instance's AudTable.c entry (its pCmd, the
// same number) or pass it on to AudTable.c's Emi_ function; Aud_EmiCycle hands the queued commands
// over once a frame. Its extent is its data: it is the first to use the .bss at 0x801F2668 and the
// .sdata2 block 0x80284008-0x80284018.

#include "core/audtrack.h"
#include "golfer.h"
#include "unsorted/cull.h"

AudInstance* Aud_CheckEmitterInstance(u8 nId);
void vec4flt_Zero3(f32* pVec);
void Aud_EmiSet3DPos(u8 nId, f32* pPos, f32* pLast, u8 nView);
void Voc_Cycle(void);                 // hlaudvoice.c
void fn_800AF320(void);
void fn_800B0434(void);                 // startUp.c

AudInstance lbl_801F2740[256];
AudEmitters lbl_801F2668;

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80284008), before the 0.0f Aud_EmiSet3DPos uses first; its body is unknown.
static f32 hlaudemitter_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets up the instances, all on the free list, and empties every emitter. Always 1.
u8 Aud_EmiInitOnce(void) {
    AudInstance* pInst;
    int i;

    pInst = lbl_801F2740;
    Mem_set(lbl_801F2740, 0, sizeof(lbl_801F2740));
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

// Runs Aud_EmiDel on every instance in use, then empties every emitter. Always 1.
int Aud_EmiInitSession(void) {
    AudInstance* pInst;
    AudInstance* pNext;
    s32 i;

    for (pInst = lbl_801F2668.pActive; pInst != NULL; pInst = pNext) {
        pNext = pInst->pNextActive;
        Aud_EmiDel(pInst->nId);
    }
    for (i = 0; i < 32; i++) {
        lbl_801F2668.apFirst[i] = NULL;
        lbl_801F2668.anSound[i] = 0;
    }
    return 1;
}

void Aud_EmiExitSession(void) {
}

// Once a frame (Gaud_Cycle): hands every instance's queued commands (tracks on / off, controller
// values, position) to its AudTable.c entry through Emi_UpdInstance and clears them (an instance
// with n24 1 sends its position again every frame), then ticks the tracks (Trk_Cycle) and voices
// (Voc_Cycle) and counts the frame (lbl_80282018).
void Aud_EmiCycle(void) {
    AudInstance* pInst;

    for (pInst = lbl_801F2668.pActive; pInst != NULL; pInst = pInst->pNextActive) {
        Emi_UpdInstance(pInst->nId, pInst->pCmd->u0, pInst->pCmd->u1, pInst->pCmd->auParams,
                    pInst->pCmd->aPos, pInst->pCmd->uChanged);
        pInst->pCmd->u0 = 0;
        pInst->pCmd->u1 = 0;
        pInst->pCmd->uChanged = 0;
        if (pInst->n24 == 1) {
            Aud_EmiSet3DPos(pInst->nId, pInst->vPos, NULL, 0);
        }
    }
    Trk_Cycle();
    Voc_Cycle();
    fn_800AF320();
    fn_800B0434();
    lbl_80282018++;
}

// Takes a free instance for sound nSound: onto the tail of the active list and, when nEmitter is
// not negative, the tail of that emitter's list (its first instance sets the emitter's sound). Its
// source's commands are cleared and handed on once. Returns its id, or 0xFF when all 256 are in use.
u8 Aud_EmiAdd(s16 nSound, s16 nEmitter, int n24, int n28, void (*pfnCallback)(u8 nId, u8 nBit, s32 n)) {
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
    // port: EA passes five more arguments than Emi_AddInstance takes
    pInst->pCmd = ((AudSource* (*)(u8, s16, int, int, int, int, int))Emi_AddInstance)(pInst->nId, nSound,
                                                                                      0, 0, 0, 0, 0);
    pInst->pCmd->u0 = 0;
    pInst->pCmd->u1 = 0;
    pInst->pCmd->uChanged = 0;
    pInst->u22 = 0;
    pInst->unk23 = 0;
    pInst->n24 = n24;
    pInst->n28 = n28;
    pInst->pfnCallback = pfnCallback;
    vec4flt_Zero3(pInst->vPos);
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
    Mem_set(pInst->pCmd->auParams, 0, sizeof(pInst->pCmd->auParams));
    Mem_set(pInst->pCmd->aPos, 0, sizeof(pInst->pCmd->aPos));
    Emi_UpdInstance(pInst->nId, 0, 0, pInst->pCmd->auParams, pInst->pCmd->aPos, 0);
    return pInst->nId;
}

// Frees instance nId: out of the active list onto the head of the free list, and out of its
// emitter's list (the emitter's sound is cleared with its last instance).
void Aud_EmiDel(u8 nId) {
    AudInstance* pInst = &lbl_801F2740[nId];
    AudInstance* p;
    AudInstance* pPrev;

    if (nId != 0xFF && (lbl_801F2668.uFlags & 1)) {
        Emi_DelInstance(nId);
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

// Whether track nTrack of instance nId is playing: switched on by Aud_EmiSetTrackStatus /
// Aud_EmiSetAllTrackStatus and not yet reported back through Aud_EmiTrkCB. 0 for no instance (id
// 0xFF).
u8 Aud_EmiGetTrackStatus(u8 nId, u8 nTrack) {
    AudInstance* pInst = Aud_CheckEmitterInstance(nId);
    if (pInst == NULL) {
        return 0;
    }
    return (pInst->u22 & (1 << nTrack)) != 0;
}

// The instance with an id; id 0xFF is none.
AudInstance* Aud_CheckEmitterInstance(u8 nId) {
    AudInstance* pInst = &lbl_801F2740[nId];
    if (nId == 0xFF) {
        return NULL;
    }
    return pInst;
}

// Switches track nTrack of instance nId on (bOn 1; also marks it playing for Aud_EmiGetTrackStatus)
// or off. Only queued in its entry: the sound engine gets it at the next Aud_EmiCycle.
void Aud_EmiSetTrackStatus(u8 nId, u8 nTrack, u8 bOn) {
    AudInstance* pInst = Aud_CheckEmitterInstance(nId);
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

// Switches the tracks of instance nId whose bits are set in n on and all the others off (a bit per
// track), and marks the same bits playing for Aud_EmiGetTrackStatus. Sent at the next Aud_EmiCycle.
void Aud_EmiSetAllTrackStatus(u8 nId, int n) {
    AudInstance* pInst = Aud_CheckEmitterInstance(nId);
    if (pInst != NULL) {
        pInst->pCmd->u0 = n;
        pInst->pCmd->u1 = ~n;
        pInst->pCmd->uChanged |= 0x200;
        pInst->u22 = n;
    }
}

// Sets controller nTrack of instance nId to uParams: one 32-bit value per track (for a streamed
// track its stream, play list and play mode: AudTable.c's PreprocessControllers) and marks it
// changed. Sent at the next Aud_EmiCycle.
void Aud_EmiSetControllerInt(u8 nId, u8 nTrack, u32 uParams) {
    AudInstance* pInst = Aud_CheckEmitterInstance(nId);
    if (pInst != NULL) {
        pInst->pCmd->auParams[nTrack] = uParams;
        pInst->pCmd->uChanged |= (u8)(1 << nTrack);
    }
}

// Moves instance nId to pPos (NULL: where it is), its old position into pLast (when not NULL),
// and works out where each view in use hears it: with n28 0 in the view's camera space, else as
// it is for view nView and far above (0, 10000, 0) for the other.
// EA bug: with pPos NULL and no view in use, pPos is still NULL at the last Vec3Copy.
void Aud_EmiSet3DPos(u8 nId, f32* pPos, f32* pLast, u8 nView) {
    int i;
    AudInstance* pInst;
    CamLens* pLens;
    f32* pRel;
    Vec4 vRel;

    pInst = Aud_CheckEmitterInstance(nId);
    if (pInst != NULL) {
        for (i = 0; i < 2; i++) {
            if ((gSession.nGameType == 3 || fn_800170A0(i)) && ViewController_GetCamera(i) != NULL) {
                pLens = ((Camera*)ViewController_GetCamera(i))->unk10;
                if (pPos == NULL) {
                    pPos = pInst->vPos;
                }
                if (pInst->n28 == 0) {
                    Mtx_MultVec4(pLens->m44, (Vec4*)pPos, &vRel);
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
// Aud_EmiSetTrackVarRangeTmpl passes a sound number instead (its definition, shared by all its instances).
void Aud_EmiSetTrackVariation(u8 nId, u8 nTrack, u8 n) {
    if (Aud_CheckEmitterInstance(nId) != NULL) {
        Emi_SetTrackVariation(nId, nTrack, n);
    }
}

void Aud_EmiSetTrackVarRange(u8 nId, u8 nTrack, u8 n) {
    if (Aud_CheckEmitterInstance(nId) != NULL) {
        Emi_SetTrackVarRange(nId, nTrack, n);
    }
}

void Aud_EmiSetTrackVarRangeTmpl(s16 nSound, u8 nTrack, u8 n) {
    Emi_SetTrackVarRangeTmpl(nSound, nTrack, n);
}

void Aud_EmiSetTrackStep(u8 nId, u8 nTrack, u8 n, int bCheck) {
    if (Aud_CheckEmitterInstance(nId) != NULL) {
        Emi_SetTrackStep(nId, nTrack, n, bCheck);
    }
}

// Sets the volume of track nTrack of emitter instance nId, if the instance is alive.
void Aud_EmiSetTrackAttenuation(u8 nId, u8 nTrack, f32 fVolume) {
    if (Aud_CheckEmitterInstance(nId) != NULL) {
        Emi_SetTrackAttenuation(nId, nTrack, fVolume);
    }
}

void Aud_EmiSetTrackPitchFactor(u8 nId, u8 nTrack, f32 fPitch) {
    if (Aud_CheckEmitterInstance(nId) != NULL) {
        Emi_SetTrackPitchFactor(nId, nTrack, fPitch);
    }
}

// The calls below do the same for every instance of an emitter.
void Aud_EmiAliasSetTrackStatus(s16 nEmitter, u8 nTrack, u8 bOn) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        Aud_EmiSetTrackStatus(pInst->nId, nTrack, bOn);
    }
}

// Aud_EmiSet3DPos for every instance of emitter nEmitter: they all move to pPos.
void Aud_EmiAliasSet3DPos(s16 nEmitter, f32* pPos, f32* pLast, u8 nView) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        Aud_EmiSet3DPos(pInst->nId, pPos, pLast, nView);
    }
}

// The emitter's own sound first (Aud_EmiSetTrackVarRangeTmpl), when it has instances.
void Aud_EmiAliasSetTrackVarRange(s16 nEmitter, u8 nTrack, u8 n) {
    AudInstance* pInst = lbl_801F2668.apFirst[nEmitter];
    if (pInst != NULL) {
        Aud_EmiSetTrackVarRangeTmpl(lbl_801F2668.anSound[nEmitter], nTrack, n);
    }
    for (; pInst != NULL; pInst = pInst->pNext) {
        Aud_EmiSetTrackVarRange(pInst->nId, nTrack, n);
    }
}

// Aud_EmiSetTrackStep for every instance of emitter nEmitter.
void Aud_EmiAliasSetTrackStep(s16 nEmitter, u8 nTrack, u8 n, int bCheck) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        Aud_EmiSetTrackStep(pInst->nId, nTrack, n, bCheck);
    }
}

// Aud_EmiSetTrackAttenuation for every instance of emitter nEmitter: track nTrack's volume.
void Aud_EmiAliasSetTrackAttenuation(s16 nEmitter, u8 nTrack, f32 fVolume) {
    AudInstance* pInst;
    for (pInst = lbl_801F2668.apFirst[nEmitter]; pInst != NULL; pInst = pInst->pNext) {
        Aud_EmiSetTrackAttenuation(pInst->nId, nTrack, fVolume);
    }
}

// A track of instance nId reports back (AudTable.c's Emi_TrackCallback): n 0 when the track was
// freed (Trk_FreePerf), 1 from a sequencer event (fn_800AA698). Marks track nBit not playing
// (Aud_EmiGetTrackStatus) and passes the report on to the instance's callback (Aud_EmiAdd's
// pfnCallback), when it has one.
void Aud_EmiTrkCB(u8 nId, u8 nBit, s32 n) {
    AudInstance* pInst;
    void (*pfnCallback)(u8 nId, u8 nBit, s32 n);

    pInst = Aud_CheckEmitterInstance(nId);
    if (pInst != NULL) {
        pInst->u22 &= (u8)~(1 << nBit);
        pfnCallback = pInst->pfnCallback;
        if (pfnCallback != NULL) {
            pfnCallback(nId, nBit, n);
        }
    }
}

// Sets a four-float vector to (0, 0, 0, 1).
void vec4flt_Zero3(f32* pVec) {
    pVec[2] = 0.0f;
    pVec[1] = 0.0f;
    pVec[0] = 0.0f;
    pVec[3] = 1.0f;
}
