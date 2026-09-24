// hlaudvoice.c (TW06's name, by structure: golf/audio/engine/hl/hlaudvoice.c, the file after
// hlaudtrackstm.c): the sound engine's voices. Each wraps one of startUp.c's hardware voices; a
// track takes one per channel (fn_800AC4A0) and is called back when it ends. Its extent is its
// data: it is the first to use the .bss at 0x801F19B8 and the .sdata2 block 0x80283FF8-0x80284008.

#include "core/audtrack.h"
#include "core/startup.h"

void fn_800AFBD8(u16 nVoice, u8 bOn);   // startUp.c
void fn_800AFDC8(u16 nVoice, SoundHeader* pHdr);        // startUp.c
void fn_800B0114(u16 nVoice, VoiceEnvelope* pEnv);      // startUp.c
void fn_800AFEF4(u16 nVoice, s16 nVolume, int a, int b); // startUp.c
void fn_800B0034(u16 nVoice, u8 nPan, int nMode, int bPlaying);        // startUp.c
void fn_800B00A4(u16 nVoice, u32 u, int a);             // startUp.c
void fn_800B01B4(u16 nVoice, u8 bA, u8 bB);             // startUp.c
u8   fn_800AFB98(u16 nVoice);                           // startUp.c
void fn_800B0430(void);                                 // startUp.c
s16  fn_800AFEDC(u16 nVoice);                           // startUp.c
u32  fn_800B06F4(void);                                 // startUp.c

void fn_800AC330(void);
u8   fn_800AC6B0(AudVoiceRequest* pRequest, s16* pPriority);
f32  fn_800ACEC4(f32 fA, f32 fB);

// Set the voice pool up: every voice free, numbered after its hardware voice.
void fn_800AC330(void) {
    AudVoicePool* pPool = lbl_801F19B8;
    AudVoicePool* pEnd = lbl_801F19B8 + 1;
    UList* pList;
    UList* pListEnd;
    AudVoice* pVoice;
    AudVoice* pVoiceEnd;
    u16 nVoice;

    fn_80005AE8(pPool, 0, sizeof(lbl_801F19B8));
    for (; pPool < pEnd; pPool++) {
        pList = pPool->aLists;
        pVoice = pPool->aVoices;
        pVoiceEnd = pPool->aVoices + AUD_NUM_VOICES;
        pListEnd = pList + 3;
        for (; pList < pListEnd; pList++) {
            fn_800ADE70(pList, AUD_NUM_VOICES);
        }
        nVoice = 0;
        for (; pVoice < pVoiceEnd; pVoice++) {
            pVoice->nHwVoice = nVoice++;
            pVoice->n10 = -1;
        }
        fn_800AE0DC(&pPool->free, pPool->aVoices, AUD_NUM_VOICES, sizeof(AudVoice));
    }
}

u8 fn_800AC470(void) {
    fn_800AC330();
    return 1;
}

s32 fn_800AC494(void) {
    return 1;
}

void fn_800AC49C(void) {
}

// Takes a voice for a request: a free one, or else (with 8 or fewer free) one on a list up to the
// request's n4 whose playing volume the request beats, stolen from its track. A stolen voice
// skips its first settings (bA_5). NULL when none can be had.
AudVoice* fn_800AC4A0(AudVoiceRequest* pRequest) {
    AudVoicePool* pPool = lbl_801F19B8;
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
        pListEnd = &pPool->aLists[pRequest->n4] + 1;
        if (pRequest->n4 == 2) {
            pListEnd--;
        }
        for (; pList < pListEnd; pList++) {
            for (pCand = (AudVoice*)pList->pHead; pCand != NULL; pCand = (AudVoice*)pCand->link.pNext) {
                // port: EA passes an argument fn_800AFEDC ignores
                nVolume = ((s16 (*)(u16, int))fn_800AFEDC)(pCand->nHwVoice, 0);
                if (fn_800AC6B0(pRequest, &nVolume)) {
                    pVoice = pCand;
                    break;
                }
            }
            if (pVoice != NULL) {
                if (pVoice->pfnCallback != NULL) {
                    pVoice->pfnCallback(pVoice, 1);
                }
                if (pPool->free.nFree != 0) {
                    fn_800ACB28(pVoice);
                    pVoice = NULL;
                } else {
                    fn_800AFBD8(pVoice->nHwVoice, 0);
                    fn_800ADF6C(pList, &pVoice->link);
                    bStolen = 1;
                }
                break;
            }
        }
    }
    if (pPool->free.nFree != 0) {
        pVoice = fn_800AE1AC(&pPool->free);
    }
    if (pVoice != NULL) {
        pVoice->n10 = pRequest->n4;
        pVoice->uC = 0;
        pVoice->n14 = 0;
        pVoice->n15 = 4;
        pVoice->pTone = NULL;
        pVoice->pfnCallback = pRequest->pfnCallback;
        pVoice->pUser = pRequest->pUser;
        pVoice->nIndex = pRequest->nIndex;
        pVoice->flags.n = pRequest->flags.n;
        pVoice->n3E = 0;
        pVoice->flags.b.bHalf = 0;
        if (bStolen) {
            pVoice->flags.b.bA_5 = 1;
        }
        if (pRequest->flags.b.b12) {
            if (pVoice->uAram != 0) {
                fn_800B0748(pVoice->uAram);
            }
            pVoice->uAram = fn_800B06F4();
            if (pVoice->uAram == 0) {
                fn_800ACB28(pVoice);
                return NULL;
            }
            pVoice->uPlayPos = pVoice->uAram;
        }
        fn_800ADEC8(&pPool->aLists[pRequest->n4], &pVoice->link);
    }
    return pVoice;
}

// Can this request take a voice playing at *pPriority?
u8 fn_800AC6B0(AudVoiceRequest* pRequest, s16* pPriority) {
    return pRequest->nPriority > *pPriority;
}

// Sets a sequenced voice up to play its tone at pitch fPitch and volume nVolume; the params' a8
// (when flagged) change the tone's attack and decay.
void fn_800AC6D0(AudVoice* pVoice, AudVoiceParams* pParams, u8 nVolume, f32 fPitch) {
    u16 nHwVoice = pVoice->nHwVoice;
    AudSeqTone* pTone = pVoice->pTone;
    VoiceEnvelope* pEnv = pTone->pEnv;
    VoiceEnvelope** ppEnv = &pEnv;  // fake match: the original keeps &pEnv in a register
    u32 uRate = pTone->u4 + fn_800AB32C(pTone->u8 - pTone->u4);

    if (pParams->flags.n != 0) {
        if (pParams->flags.b.b5) {
            (*ppEnv)->nAttack = pParams->a8[0];
        }
        if (pParams->flags.b.b4) {
            (*ppEnv)->nDecay = pParams->a8[1];
        }
    }
    pVoice->uC = fn_800ACEC4(uRate, fPitch);
    pVoice->n14 = nVolume;
    pVoice->flags.b.bA_0 = 1;
    fn_800AFDC8(nHwVoice, pTone->pHeader);
    // EA bug: hands over the address of the pointer, so the voice's envelope is the pointer's bits;
    // the tone's own envelope (changed above, for every voice that plays it) is never used.
    fn_800B0114(nHwVoice, (VoiceEnvelope*)ppEnv);
}

// Sets a streamed voice up to play uLen bytes of its ARAM buffer at nRate, looping: a slow attack,
// full sustain and a quick release when bLoud.
void fn_800AC7DC(AudVoice* pVoice, u32 uLen, u32 nRate, u8 bLoud) {
    VoiceEnvelope env;
    SoundHeader hdr;
    u16 nHwVoice;

    *(u32*)&env = 0;            // the envelope's whole word cleared first (it is 4 bytes)
    nHwVoice = pVoice->nHwVoice;
    env.nAttack = 0x200;
    env.nDecay = 0;
    env.nSustain = 0xF;
    env.nRelease = bLoud ? 0x10 : 0x80;
    pVoice->uC = nRate;
    pVoice->n14 = 0x7F;
    pVoice->flags.b.bA_0 = 1;
    fn_80005AE8(&hdr, 0, sizeof(hdr));
    hdr.uC = 1;
    // the buffer's start and end in 4-bit units, past the first frame's header
    hdr.u0 = hdr.u4 = pVoice->uAram;
    hdr.u4 += uLen;
    hdr.u0 *= 2;
    hdr.u4 *= 2;
    hdr.u0 += 2;
    hdr.u4 -= 1;
    hdr.u8 = hdr.u0;
    pVoice->uC = (f32)pVoice->uC * 2.048f;
    fn_800AFDC8(nHwVoice, &hdr);
    fn_800B0114(nHwVoice, &env);
}

// Passes a voice's settings on to its hardware voice; a voice just set up is started first.
void fn_800AC91C(AudVoice* pVoice, AudVoiceParams* pParams) {
    u16 nHwVoice = pVoice->nHwVoice;
    u32 uRate;
    u8 bSetRate;
    int bPlaying;
    u8 bReverb;

    if (!pVoice->flags.b.bA_5) {
        uRate = pVoice->uC;
        if (pVoice->flags.b.bA_0) {
            bReverb = !pVoice->flags.b.bA_6;
            fn_800AFBD8(nHwVoice, 1);
            fn_800B01B4(nHwVoice, bReverb, bReverb);
            bPlaying = 0;
            pVoice->flags.b.bA_0 = 0;
            bSetRate = 1;
        } else {
            bPlaying = 1;
            bSetRate = 0;
        }
        if (pParams->flags.b.bVolume) {
            fn_800AFEF4(nHwVoice, pParams->nVolume, 0, bPlaying);
            fn_800B0034(nHwVoice, pParams->nPan, 2, bPlaying);
            fn_800B0034(nHwVoice, pParams->n7, 3, bPlaying);
        }
        if (pParams->flags.b.bPitch) {
            uRate = fn_800ACEC4(pVoice->uC, pParams->fPitch);
            bSetRate = 1;
        }
        if (bSetRate) {
            fn_800B00A4(nHwVoice, uRate, bPlaying);
        }
    } else {
        pVoice->flags.b.bA_5 = 0;
    }
}

// Pauses or resumes a voice's hardware voice.
void fn_800ACA5C(AudVoice* pVoice, u8 bPause) {
    if (pVoice != NULL && !pVoice->flags.b.bB_6) {
        fn_800AFCBC(pVoice->nHwVoice, bPause);
    }
}

// Lets a voice end: its hardware voice is stopped and it moves to list 0, where fn_800ACB98 frees
// it once the hardware voice is done.
void fn_800ACA94(AudVoice* pVoice) {
    if (!pVoice->flags.b.bStopped) {
        fn_800AFBD8(pVoice->nHwVoice, 0);
        if (pVoice->n10 > 0) {
            fn_800ADF6C(&lbl_801F19B8->aLists[pVoice->n10], &pVoice->link);
            fn_800ADE88(&lbl_801F19B8->aLists[0], &pVoice->link);
            pVoice->n10 = 0;
        }
        pVoice->flags.b.bStopped = 1;
    }
}

// Stops a voice at once: it forgets its track and gives back its ARAM buffer.
void fn_800ACB28(AudVoice* pVoice) {
    fn_800ACA94(pVoice);
    pVoice->pfnCallback = NULL;
    pVoice->pUser = NULL;
    pVoice->nIndex = 0;
    if (pVoice->flags.b.bA_4) {
        fn_800ACA5C(pVoice, 1);
        if (pVoice->uAram != 0) {
            fn_800B0748(pVoice->uAram);
            pVoice->uAram = 0;
            pVoice->uPlayPos = 0;
        }
    }
}

// Once a frame: frees the ended voices whose hardware voice is done (telling their track), and counts
// the voices in use.
void fn_800ACB98(void) {
    AudVoicePool* pPool = lbl_801F19B8;
    AudVoicePool* pEnd = lbl_801F19B8 + 1;
    AudVoice* pVoice;
    AudVoice* pNext;

    lbl_802820B0 = AUD_NUM_VOICES;
    for (; pPool < pEnd; pPool++) {
        for (pVoice = (AudVoice*)pPool->aLists[0].pHead; pVoice != NULL; pVoice = pNext) {
            pNext = (AudVoice*)pVoice->link.pNext;
            if (pVoice->n15 <= 0) {
                if (fn_800AFB98(pVoice->nHwVoice)) {
                    fn_800ADF6C(&pPool->aLists[0], &pVoice->link);
                    fn_800AE1DC(&pPool->free, pVoice);
                    // port: EA passes arguments fn_800B0430 (empty) ignores
                    ((void (*)(u16, int))fn_800B0430)(pVoice->nHwVoice, pVoice->flags.b.bA_1 != 0);
                    if (pVoice->flags.b.bA_4) {
                        pVoice->flags.b.bHalf = 0;
                        if (pVoice->uAram != 0) {
                            fn_800B0748(pVoice->uAram);
                            pVoice->uAram = 0;
                            pVoice->uPlayPos = 0;
                        }
                    }
                    pVoice->n10 = -1;
                    pVoice->flags.n = 0;
                    if (pVoice->pfnCallback != NULL) {
                        pVoice->pfnCallback(pVoice, 0);
                        pVoice->pfnCallback = NULL;
                        pVoice->pUser = NULL;
                        pVoice->nIndex = 0;
                    }
                }
            } else {
                pVoice->n15--;
            }
        }
        lbl_802820B0 -= (u8)pPool->free.nFree;
    }
}

// Pause (bPause) or resume every voice. Resuming leaves the streamed voices (bA_4) paused when
// bStreams is set, for Stm_Tick to resume. Each pause flips the order the voices are gone through.
void fn_800ACCF4(u8 bPause, u8 bStreams) {
    AudVoicePool* pPool = lbl_801F19B8;
    AudVoicePool* pEnd = lbl_801F19B8 + 1;
    AudVoice* pVoice;
    AudVoice* pVoiceEnd;

    if (bPause) {
        lbl_802820B4 = 1 - lbl_802820B4;
    }
    for (; pPool < pEnd; pPool++) {
        pVoice = pPool->aVoices;
        pVoiceEnd = pPool->aVoices + AUD_NUM_VOICES;
        if (lbl_802820B4 != 0) {
            for (; pVoice < pVoiceEnd; pVoice++) {
                if (bPause) {
                    fn_800AFCBC(pVoice->nHwVoice, 1);
                } else if (bStreams && pVoice->flags.b.bA_4) {
                    pVoice->flags.b.bB_6 = 1;
                } else {
                    fn_800AFCBC(pVoice->nHwVoice, 0);
                }
            }
        } else {
            // pVoiceEnd itself walks back from the last voice
            for (pVoiceEnd--; pPool->aVoices <= pVoiceEnd; pVoiceEnd--) {
                if (bPause) {
                    fn_800AFCBC(pVoiceEnd->nHwVoice, 1);
                } else if (bStreams && pVoiceEnd->flags.b.bA_4) {
                    pVoiceEnd->flags.b.bB_6 = 1;
                } else {
                    fn_800AFCBC(pVoiceEnd->nHwVoice, 0);
                }
            }
        }
    }
}

// Has a streamed voice crossed into the other half of its ARAM buffer since the last call? Then
// *puPos gets the half it has just left, free to be refilled.
u8 fn_800ACE38(AudVoice* pVoice, u32* puPos) {
    u32 uPos = fn_800AFD8C(pVoice->nHwVoice);
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

f32 fn_800ACEC4(f32 fA, f32 fB) {
    return fA * fB;
}
