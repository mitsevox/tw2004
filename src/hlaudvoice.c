// hlaudvoice.c (TW06's name, by structure: golf/audio/engine/hl/hlaudvoice.c, the file after
// hlaudtrackstm.c): the sound engine's voices. Each wraps one of startUp.c's hardware voices; a
// track takes one per channel (fn_800AC4A0) and is called back when it ends. Its extent is its
// data: it is the first to use the .bss at 0x801F19B8 and the .sdata2 block 0x80283FF8-0x80284008.

#include "core/audtrack.h"
#include "core/startup.h"

void fn_800AFBD8(u16 nVoice, u8 bOn);   // startUp.c

void fn_800AC330(void);
u8   fn_800AC6B0(AudVoiceRequest* pRequest, s16* pPriority);

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

// Can this request take a voice playing at *pPriority?
u8 fn_800AC6B0(AudVoiceRequest* pRequest, s16* pPriority) {
    return pRequest->nPriority > *pPriority;
}

// Pauses or resumes a voice's hardware voice.
void fn_800ACA5C(AudVoice* pVoice, u8 bPause) {
    if (pVoice != NULL && !pVoice->bB_6) {
        fn_800AFCBC(pVoice->nHwVoice, bPause);
    }
}

// Lets a voice end: its hardware voice is stopped and it moves to list 0, where fn_800ACB98 frees
// it once the hardware voice is done.
void fn_800ACA94(AudVoice* pVoice) {
    if (!pVoice->bStopped) {
        fn_800AFBD8(pVoice->nHwVoice, 0);
        if (pVoice->n10 > 0) {
            fn_800ADF6C(&lbl_801F19B8->aLists[pVoice->n10], &pVoice->link);
            fn_800ADE88(&lbl_801F19B8->aLists[0], &pVoice->link);
            pVoice->n10 = 0;
        }
        pVoice->bStopped = 1;
    }
}

// Stops a voice at once: it forgets its track and gives back its ARAM buffer.
void fn_800ACB28(AudVoice* pVoice) {
    fn_800ACA94(pVoice);
    pVoice->pfnCallback = NULL;
    pVoice->pUser = NULL;
    pVoice->nIndex = 0;
    if (pVoice->bA_4) {
        fn_800ACA5C(pVoice, 1);
        if (pVoice->uAram != 0) {
            fn_800B0748(pVoice->uAram);
            pVoice->uAram = 0;
            pVoice->uPlayPos = 0;
        }
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
                } else if (bStreams && pVoice->bA_4) {
                    pVoice->bB_6 = 1;
                } else {
                    fn_800AFCBC(pVoice->nHwVoice, 0);
                }
            }
        } else {
            // pVoiceEnd itself walks back from the last voice
            for (pVoiceEnd--; pPool->aVoices <= pVoiceEnd; pVoiceEnd--) {
                if (bPause) {
                    fn_800AFCBC(pVoiceEnd->nHwVoice, 1);
                } else if (bStreams && pVoiceEnd->bA_4) {
                    pVoiceEnd->bB_6 = 1;
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
    int bCrossed = 0;

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
