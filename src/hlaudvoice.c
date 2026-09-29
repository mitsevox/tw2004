// hlaudvoice.c (TW06's golf/audio/engine/hl/hlaudvoice.c, the file after hlaudtrackstm.c; TW07's
// HLAudVoice.c has these Voc_ functions in the same order): the sound engine's voices. Each wraps
// one of startUp.c's hardware voices; a track (or the movie sound) takes one per channel
// (Voc_Alloc), stealing a quieter one when few are free, and is called back when it ends. The
// voices in use sit on three lists by how hard they are to steal; Voc_Cycle frees the ended ones
// each frame. audfrac_MulU at the end is an out-of-line copy of a UAudFrac.h inline (TW07). Its
// extent is its data: it is the first to use the .bss at 0x801F19B8 and the .sdata2 block
// 0x80283FF8-0x80284008.

#include "core/audtrack.h"
#include "core/startup.h"

void HwVoice_StartOrRelease(u16 nVoice, u8 bOn);   // startUp.c
void HwVoice_SetSound(u16 nVoice, SoundHeader* pHdr);        // startUp.c
void HwVoice_SetEnvelope(u16 nVoice, VoiceEnvelope* pEnv);      // startUp.c
void HwVoice_SetVolume(u16 nVoice, s16 nVolume, int a, int b); // startUp.c
void HwVoice_SetPan(u16 nVoice, u8 nPan, int nMode, int bPlaying);        // startUp.c
void HwVoice_SetRate(u16 nVoice, u32 u, int a);             // startUp.c
void HwVoice_SetReverb(u16 nVoice, u8 bA, u8 bB);             // startUp.c
u8   HwVoice_IsFree(u16 nVoice);                           // startUp.c
void HwVoice_OnVoiceFreed(void);                                 // startUp.c
s16  HwVoice_GetVolume(u16 nVoice);                           // startUp.c
u32  AudAram_AllocStreamBuffer(void);                                 // startUp.c

void Voc_ResetModule(void);
u8   VoicePowerCompare(AudVoiceRequest* pRequest, s16* pPriority);
f32  audfrac_MulU(f32 fA, f32 fB);

AudVoicePool gVocCores[1];              // the voices, their lists and free pool (EA: TVoCore)

s32 gVocPauseOrder;                     // flipped by each pause: 1 walks the voices forwards
u8  gVocInUse;                          // voices in use, counted by Voc_Cycle

// Set the voice pool up: every voice free, numbered after its hardware voice.
void Voc_ResetModule(void) {
    AudVoicePool* pPool = gVocCores;
    AudVoicePool* pEnd = gVocCores + 1;
    UList* pList;
    UList* pListEnd;
    AudVoice* pVoice;
    AudVoice* pVoiceEnd;
    u16 nVoice;

    Mem_set(pPool, 0, sizeof(gVocCores));
    for (; pPool < pEnd; pPool++) {
        pList = pPool->aLists;
        pVoice = pPool->aVoices;
        pVoiceEnd = pPool->aVoices + AUD_NUM_VOICES;
        pListEnd = pList + 3;
        for (; pList < pListEnd; pList++) {
            UList_Reset(pList, AUD_NUM_VOICES);
        }
        nVoice = 0;
        for (; pVoice < pVoiceEnd; pVoice++) {
            pVoice->nHwVoice = nVoice++;
            pVoice->nStealLevel = -1;
        }
        UPool_Init(&pPool->free, pPool->aVoices, AUD_NUM_VOICES, sizeof(AudVoice));
    }
}

// The voices' start-up step in Aud_InitOnce: sets the voice pool up (Voc_ResetModule). Always 1.
u8 Voc_InitModule(void) {
    Voc_ResetModule();
    return 1;
}

// The voices' step in Ses_Init: nothing to do. Always 1.
u8 Voc_InitSession(void) {
    return 1;
}

// The voices' step in Ses_Exit: empty in this build.
void Voc_ExitSession(void) {
}

// Takes a voice for a request (Mov_Init, a sequenced note, a stream's StartStreamVoices); NULL at
// priority 0 or when none can be had. With 8 or fewer free, the first voice on lists 0 to
// nStealLevel (0 and 1 when it is 2) that the request outranks (VoicePowerCompare, against its
// hardware voice's volume) is stolen: its track is told (callback reason 1), then it is deleted if
// a free voice is left, else taken over (it then skips its next Voc_Render, bSkipRender). A
// request with bStream also gets an ARAM stream block (AudAram_AllocStreamBuffer); without one the
// voice is deleted and NULL returned. The voice goes on list nStealLevel.
AudVoice* Voc_Alloc(AudVoiceRequest* pRequest) {
    AudVoicePool* pPool = gVocCores;
    AudVoice* pVoice = NULL;
    u8 bStolen = 0;
    UList* pList;
    UList* pListEnd;
    AudVoice* pCand;
    s16 nVolume;

    if (pRequest->nPriority == 0) {
        return NULL;
    }
    if (pPool->free.nFree <= 8) {
        pList = pPool->aLists;
        pListEnd = &pPool->aLists[pRequest->nStealLevel] + 1;
        if (pRequest->nStealLevel == 2) {
            pListEnd--;
        }
        for (; pList < pListEnd; pList++) {
            for (pCand = (AudVoice*)pList->pHead; pCand != NULL; pCand = (AudVoice*)pCand->link.pNext) {
                // port: EA passes an argument HwVoice_GetVolume ignores
                nVolume = ((s16 (*)(u16, int))HwVoice_GetVolume)(pCand->nHwVoice, 0);
                if (VoicePowerCompare(pRequest, &nVolume)) {
                    pVoice = pCand;
                    break;
                }
            }
            if (pVoice != NULL) {
                if (pVoice->pfnCallback != NULL) {
                    pVoice->pfnCallback(pVoice, 1);
                }
                if (pPool->free.nFree != 0) {
                    Voc_Delete(pVoice);
                    pVoice = NULL;
                } else {
                    HwVoice_StartOrRelease(pVoice->nHwVoice, 0);
                    UList_DeleteAt(pList, &pVoice->link);
                    bStolen = 1;
                }
                break;
            }
        }
    }
    if (pPool->free.nFree != 0) {
        pVoice = UPool_Alloc(&pPool->free);
    }
    if (pVoice != NULL) {
        pVoice->nStealLevel = pRequest->nStealLevel;
        pVoice->uRate = 0;
        pVoice->nVolume = 0;
        pVoice->nEndDelay = 4;
        pVoice->pTone = NULL;
        pVoice->pfnCallback = pRequest->pfnCallback;
        pVoice->pUser = pRequest->pUser;
        pVoice->nIndex = pRequest->nIndex;
        pVoice->flags.n = pRequest->flags.n;
        pVoice->n3E = 0;
        pVoice->flags.b.bHalf = 0;
        if (bStolen) {
            pVoice->flags.b.bSkipRender = 1;
        }
        if (pRequest->flags.b.bStream) {
            if (pVoice->uAram != 0) {
                AudAram_FreeStreamBuffer(pVoice->uAram);
            }
            pVoice->uAram = AudAram_AllocStreamBuffer();
            if (pVoice->uAram == 0) {
                Voc_Delete(pVoice);
                return NULL;
            }
            pVoice->uPlayPos = pVoice->uAram;
        }
        UList_PushTail(&pPool->aLists[pRequest->nStealLevel], &pVoice->link);
    }
    return pVoice;
}

// Does the request outrank a voice playing at volume *pPriority (Voc_Alloc's steal test)? Only a
// strictly higher priority does.
u8 VoicePowerCompare(AudVoiceRequest* pRequest, s16* pPriority) {
    return pRequest->nPriority > *pPriority;
}

// Sets a sequenced voice up to play its tone: the rate is picked at random between the tone's u4
// and u8, times fPitch; its volume is nVolume (0-127). A params anAdsr flagged by bAttack / bDecay
// changes the tone's attack / decay, for every voice that plays the tone. Voc_Render starts it.
void Voc_Start(AudVoice* pVoice, AudVoiceParams* pParams, u8 nVolume, f32 fPitch) {
    u16 nHwVoice = pVoice->nHwVoice;
    AudSeqTone* pTone = pVoice->pTone;
    VoiceEnvelope* pEnv = pTone->pEnv;
    VoiceEnvelope** ppEnv = &pEnv;  // fake match: the original keeps &pEnv in a register
    u32 uRate = pTone->u4 + Aud_RandomBelow(pTone->u8 - pTone->u4);

    if (pParams->flags.n != 0) {
        if (pParams->flags.b.bAttack) {
            (*ppEnv)->nAttack = pParams->anAdsr[0];
        }
        if (pParams->flags.b.bDecay) {
            (*ppEnv)->nDecay = pParams->anAdsr[1];
        }
    }
    pVoice->uRate = audfrac_MulU(uRate, fPitch);
    pVoice->nVolume = nVolume;
    pVoice->flags.b.bStart = 1;
    HwVoice_SetSound(nHwVoice, pTone->pHeader);
    // EA bug: hands over the address of the pointer, so the voice's envelope is the pointer's bits;
    // the tone's own envelope (changed above, for every voice that plays it) is never used.
    HwVoice_SetEnvelope(nHwVoice, (VoiceEnvelope*)ppEnv);
}

// Sets a streamed voice up (Mov_Start, StartStreamVoices) to loop over uLen bytes of its ARAM
// buffer at nRate Hz: attack 0x200, full sustain, release 0x10 with bLoud (the movie's), else 0x80.
// Voc_Render starts it.
void Voc_StartStream(AudVoice* pVoice, u32 uLen, u32 nRate, u8 bLoud) {
    VoiceEnvelope env;
    SoundHeader hdr;
    u16 nHwVoice;

    *(u32*)&env = 0;            // the envelope's whole word cleared first (it is 4 bytes)
    nHwVoice = pVoice->nHwVoice;
    env.nAttack = 0x200;
    env.nDecay = 0;
    env.nSustain = 0xF;
    env.nRelease = bLoud ? 0x10 : 0x80;
    pVoice->uRate = nRate;
    pVoice->nVolume = 0x7F;
    pVoice->flags.b.bStart = 1;
    Mem_set(&hdr, 0, sizeof(hdr));
    hdr.bLoop = 1;
    // the buffer's start and end in 4-bit units, past the first frame's header
    hdr.uStart = hdr.uEnd = pVoice->uAram;
    hdr.uStart *= 2;
    hdr.uStart += 2;
    hdr.uEnd += uLen;
    hdr.uEnd *= 2;
    hdr.uEnd -= 1;
    hdr.uLoop = hdr.uStart;
    pVoice->uRate = (f32)pVoice->uRate * 2.048f;
    HwVoice_SetSound(nHwVoice, &hdr);
    HwVoice_SetEnvelope(nHwVoice, &env);
}

// Passes a voice's settings (volume and both pans, pitch) on to its hardware voice. A voice just
// set up (bStart) is started first, with aux A on unless bNoReverb; a stolen voice skips one call
// (bSkipRender, cleared here).
void Voc_Render(AudVoice* pVoice, AudVoiceParams* pParams) {
    u16 nHwVoice = pVoice->nHwVoice;
    u32 uRate;
    u8 bSetRate;
    int bPlaying;
    u8 bReverb;

    if (!pVoice->flags.b.bSkipRender) {
        uRate = pVoice->uRate;
        if (pVoice->flags.b.bStart) {
            bReverb = !pVoice->flags.b.bNoReverb;
            HwVoice_StartOrRelease(nHwVoice, 1);
            HwVoice_SetReverb(nHwVoice, bReverb, bReverb);
            bPlaying = 0;
            pVoice->flags.b.bStart = 0;
            bSetRate = 1;
        } else {
            bPlaying = 1;
            bSetRate = 0;
        }
        if (pParams->flags.b.bVolume) {
            HwVoice_SetVolume(nHwVoice, pParams->nVolume, 0, bPlaying);
            HwVoice_SetPan(nHwVoice, pParams->nPan, 2, bPlaying);
            HwVoice_SetPan(nHwVoice, pParams->n7, 3, bPlaying);
        }
        if (pParams->flags.b.bPitch) {
            uRate = audfrac_MulU(pVoice->uRate, pParams->fPitch);
            bSetRate = 1;
        }
        if (bSetRate) {
            HwVoice_SetRate(nHwVoice, uRate, bPlaying);
        }
    } else {
        pVoice->flags.b.bSkipRender = 0;
    }
}

// Pauses (bPause 1) or resumes a voice's hardware voice. Nothing for NULL, or while bHeld is set (a
// streamed voice Voc_PauseAll left paused for Stm_Tick).
void Voc_Pause(AudVoice* pVoice, u8 bPause) {
    if (pVoice != NULL && !pVoice->flags.b.bHeld) {
        HwVoice_Pause(pVoice->nHwVoice, bPause);
    }
}

// Lets a voice end: its hardware voice is stopped and it moves to list 0, where Voc_Cycle frees
// it once the hardware voice is done.
void Voc_Stop(AudVoice* pVoice) {
    if (!pVoice->flags.b.bStopped) {
        HwVoice_StartOrRelease(pVoice->nHwVoice, 0);
        if (pVoice->nStealLevel > 0) {
            UList_DeleteAt(&gVocCores->aLists[pVoice->nStealLevel], &pVoice->link);
            UList_PushHead(&gVocCores->aLists[0], &pVoice->link);
            pVoice->nStealLevel = 0;
        }
        pVoice->flags.b.bStopped = 1;
    }
}

// Stops a voice for good (Voc_Stop) and forgets its track, so Voc_Cycle frees it without calling
// back. A voice that owns an ARAM buffer (bStream) is also paused and gives the buffer back.
void Voc_Delete(AudVoice* pVoice) {
    Voc_Stop(pVoice);
    pVoice->pfnCallback = NULL;
    pVoice->pUser = NULL;
    pVoice->nIndex = 0;
    if (pVoice->flags.b.bStream) {
        Voc_Pause(pVoice, 1);
        if (pVoice->uAram != 0) {
            AudAram_FreeStreamBuffer(pVoice->uAram);
            pVoice->uAram = 0;
            pVoice->uPlayPos = 0;
        }
    }
}

// Once a frame: frees the ended voices whose hardware voice is done (telling their track), and counts
// the voices in use.
void Voc_Cycle(void) {
    AudVoicePool* pPool = gVocCores;
    AudVoicePool* pEnd = gVocCores + 1;
    AudVoice* pVoice;
    AudVoice* pNext;

    gVocInUse = AUD_NUM_VOICES;
    for (; pPool < pEnd; pPool++) {
        for (pVoice = (AudVoice*)pPool->aLists[0].pHead; pVoice != NULL; pVoice = pNext) {
            pNext = (AudVoice*)pVoice->link.pNext;
            if (pVoice->nEndDelay <= 0) {
                if (HwVoice_IsFree(pVoice->nHwVoice)) {
                    UList_DeleteAt(&pPool->aLists[0], &pVoice->link);
                    UPool_Free(&pPool->free, pVoice);
                    // port: EA passes arguments HwVoice_OnVoiceFreed (empty) ignores
                    ((void (*)(u16, int))HwVoice_OnVoiceFreed)(pVoice->nHwVoice, pVoice->flags.b.bLoops != 0);
                    if (pVoice->flags.b.bStream) {
                        pVoice->flags.b.bHalf = 0;
                        if (pVoice->uAram != 0) {
                            AudAram_FreeStreamBuffer(pVoice->uAram);
                            pVoice->uAram = 0;
                            pVoice->uPlayPos = 0;
                        }
                    }
                    pVoice->nStealLevel = -1;
                    pVoice->flags.n = 0;
                    if (pVoice->pfnCallback != NULL) {
                        pVoice->pfnCallback(pVoice, 0);
                        pVoice->pfnCallback = NULL;
                        pVoice->pUser = NULL;
                        pVoice->nIndex = 0;
                    }
                }
            } else {
                pVoice->nEndDelay--;
            }
        }
        gVocInUse -= (u8)pPool->free.nFree;
    }
}

// Pause (bPause) or resume every voice. Resuming leaves the streamed voices (bStream) paused when
// bStreams is set, for Stm_Tick to resume. Each pause flips the order the voices are gone through.
void Voc_PauseAll(u8 bPause, u8 bStreams) {
    AudVoicePool* pPool = gVocCores;
    AudVoicePool* pEnd = gVocCores + 1;
    AudVoice* pVoice;
    AudVoice* pVoiceEnd;

    if (bPause) {
        gVocPauseOrder = 1 - gVocPauseOrder;
    }
    for (; pPool < pEnd; pPool++) {
        pVoice = pPool->aVoices;
        pVoiceEnd = pPool->aVoices + AUD_NUM_VOICES;
        if (gVocPauseOrder != 0) {
            for (; pVoice < pVoiceEnd; pVoice++) {
                if (bPause) {
                    HwVoice_Pause(pVoice->nHwVoice, 1);
                } else if (bStreams && pVoice->flags.b.bStream) {
                    pVoice->flags.b.bHeld = 1;
                } else {
                    HwVoice_Pause(pVoice->nHwVoice, 0);
                }
            }
        } else {
            // pVoiceEnd itself walks back from the last voice
            for (pVoiceEnd--; pPool->aVoices <= pVoiceEnd; pVoiceEnd--) {
                if (bPause) {
                    HwVoice_Pause(pVoiceEnd->nHwVoice, 1);
                } else if (bStreams && pVoiceEnd->flags.b.bStream) {
                    pVoiceEnd->flags.b.bHeld = 1;
                } else {
                    HwVoice_Pause(pVoiceEnd->nHwVoice, 0);
                }
            }
        }
    }
}

// Has a streamed voice crossed into the other half of its ARAM buffer since the last call? Then
// *puPos gets the half it has just left, free to be refilled.
u8 Voc_CheckStreamHalfDone(AudVoice* pVoice, u32* puPos) {
    u32 uPos = HwVoice_GetPlayPos(pVoice->nHwVoice);
    u32 uAram = pVoice->uAram;
    u32 uHalf;
    u8 bCrossed = 0;

    uHalf = uAram + 0x7F00;

    if (uPos < uHalf && pVoice->uPlayPos >= uHalf) {
        *puPos = uHalf;
        bCrossed = 1;
    } else if (uPos >= uHalf && pVoice->uPlayPos < uHalf) {
        bCrossed = 1;
        *puPos = uAram;
    }
    pVoice->uPlayPos = uPos;
    return bCrossed;
}

// The engine's unsigned fraction product (EA's audfrac_MulU, a UAudFrac.h inline, here out of
// line): a rate times a pitch factor.
f32 audfrac_MulU(f32 fA, f32 fB) {
    return fA * fB;
}
