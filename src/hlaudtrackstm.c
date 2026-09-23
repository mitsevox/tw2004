// hlaudtrackstm.c (our name, after TW06's golf/audio/engine/hl/hlaudtrackstm.c; the Stm_ names and
// the read-queue names are EA's own, from the name strings its functions hand to the audio locks):
// the streamed tracks of the sound engine (music and long sounds read from disc). Each track reads
// its stream into a main-memory buffer through a queue of disc reads (lbl_801F18B8), DMAs each
// block into its voices' ARAM buffers, and keeps the reads ahead of what the voices play.

#include "core/audtrack.h"
#include "core/startup.h"

int  fn_80006478(s32 hFile, u8* pDst, u32 uLen, u32 uOffset,
                 void (*pfnDone)(int nBytes, int nError, AudTrack* pTrack, u8 nId), int n,
                 AudTrack* pTrack, u8 nId, int n19);                 // read from disc, not waiting
void fn_800AA1B8(AudTrack* pTrack, int n);
void fn_800AA400(void);
f32  fn_800AA44C(u8 nCurve);
u16  fn_800AB32C(u16 nRange);                                        // a random number below nRange
void fn_800ACA5C(AudVoice* pVoice, int bPause);
u8   fn_800ACE38(AudVoice* pVoice, u32* puPos);
s32  DVDGetDriveStatus(void);

void fn_800AB860(AudTrack* pTrack);
void fn_800ABC54(AudTrack* pTrack);
void fn_800AC310(AudTrack* pTrack);
s32  fn_800AC328(void);
void RemoveFromAudStreamQueue(AudTrack* pTrack);
void fn_800AB99C(int nBytes, int nError, AudTrack* pTrack, u8 nId);

// Applies a play list or stream change that came in while the track was busy.
u8 fn_800AB3A4(AudTrack* pTrack) {
    u8 bChanged;

    bChanged = 0;
    if (pTrack->nNextPlayList != 0xFF) {
        Stm_SetPlayList(pTrack, pTrack->nNextPlayList);
        bChanged = 1;
        pTrack->nNextPlayList = 0xFF;
    }
    if (pTrack->nNextStream != 0xFFFF) {
        if (pTrack->pTmpl->pPlayList != NULL) {
            Stm_SetStream(pTrack, pTrack->nNextStream, 0);
            bChanged = 1;
        }
        pTrack->nNextStream = 0xFFFF;
    }
    return bChanged;
}

// Starts the voices on what is in their ARAM buffers.
void fn_800AB428(AudTrack* pTrack) {
    AudPlayList* pList;
    u8 i;
    int bLoud;

    pList = pTrack->pTmpl->pPlayList;
    fn_800A85FC(pTrack->f44, fn_800AA44C(pList->n3));
    bLoud = pList->n3 == 15;
    for (i = 0; i < pList->nChannels; i++) {
        fn_800AC7DC(pTrack->apVoices[i], 0xFE00, pList->n4, bLoud);
    }
    pTrack->nState = 6;
}

// Queues a disc read; returns 0 when the queue is full.
u8 fn_800AB4C0(s32 hFile, u8* pDst, u32 uLen, u32 uOffset,
               void (*pfnDone)(int nBytes, int nError, AudTrack* pTrack, u8 nId), AudTrack* pTrack,
               u8 nId, u8 n19) {
    u8 bQueued;
    AudStreamRead* pRead;

    bQueued = 0;
    fn_800B59BC("AddToAudStreamReadQueue");
    if (lbl_801F18B8.queue.nCount + 1 <= lbl_801F18B8.queue.nMax) {
        pRead = fn_800AE03C(&lbl_801F18B8.queue);
        pRead->hFile = hFile;
        bQueued = 1;
        pRead->pDst = pDst;
        pRead->uLen = uLen;
        pRead->uOffset = uOffset;
        pRead->pfnDone = pfnDone;
        pRead->pTrack = pTrack;
        pRead->nId = nId;
        pRead->n19 = n19;
        pRead->bRestart = 0;
    }
    fn_800B59EC("AddToAudStreamReadQueue");
    return bQueued;
}

// Queues a refill of the track's whole buffer; returns 0 when the queue is full.
u8 fn_800AB570(AudTrack* pTrack) {
    u8 bQueued;
    AudStreamRead* pRead;

    bQueued = 0;
    fn_800B59BC("AddToAudStreamReadQueue");
    if (lbl_801F18B8.queue.nCount + 1 <= lbl_801F18B8.queue.nMax) {
        pRead = fn_800AE03C(&lbl_801F18B8.queue);
        memset(pRead, 0, sizeof(AudStreamRead));
        pRead->pTrack = pTrack;
        bQueued = 1;
        pRead->bRestart = 1;
    }
    fn_800B59EC("AddToAudStreamReadQueue");
    return bQueued;
}

// The oldest read is done: take it off the queue so the next one can start.
void RemoveFromAudStreamQueue(AudTrack* pTrack) {
    fn_800B59BC("RemoveFromAudStreamQueue");
    fn_800AE084(&lbl_801F18B8.queue);
    lbl_801F18B8.bBusy = 0;
    fn_800B59EC("RemoveFromAudStreamQueue");
}

// Starts the oldest queued read, unless one is under way.
void ProcessAudStreamReadQueue(void) {
    u8 bMore;
    AudStreamRead* pRead;

    bMore = 1;
    while (bMore) {
        fn_800B59BC("ProcessAudStreamReadQueue");
        if (lbl_801F18B8.queue.nCount != 0 && lbl_801F18B8.bBusy == 0) {
            lbl_801F18B8.bBusy = 1;
            pRead = (AudStreamRead*)lbl_801F18B8.queue.pRead;
            if (pRead->bRestart) {
                fn_800AB860(pRead->pTrack);
            } else {
                fn_80006478(pRead->hFile, pRead->pDst, pRead->uLen, pRead->uOffset, pRead->pfnDone, 0,
                            pRead->pTrack, pRead->nId, pRead->n19);
            }
        } else {
            bMore = 0;
        }
        fn_800B59EC("ProcessAudStreamReadQueue");
    }
}

// DMAs one block per channel from the buffer into the voices' ARAM buffers, into the half each
// voice is not playing, and flips the halves. pfnDone(1) comes with the last channel's DMA.
void fn_800AB72C(AudTrack* pTrack, void (*pfnDone)(u32 bLast), u32 uStep, u8 bSkipEmpty) {
    u8 i;
    u8* pSrc;
    AudPlayList* pList;
    AudVoice* pVoice;
    u8 nChannels;
    u32 uAram;

    i = 0;
    pSrc = pTrack->pBuffer;
    pList = pTrack->pTmpl->pPlayList;
    lbl_802820AC = pTrack;
    while (i < (nChannels = pList->nChannels)) {
        pVoice = pTrack->apVoices[i];
        if ((!bSkipEmpty || pVoice != NULL) && pVoice != NULL) {
            uAram = pVoice->uAram + pVoice->bHalf * 0x7F00;
            fn_800B0268(pVoice->nHwVoice, (StreamChunk*)pSrc, 0x8000, pVoice->bHalf);
            fn_800B044C(uAram, pSrc + 0x100, 0x7F00, pfnDone, i == nChannels - 1);
            pSrc += uStep;
            pVoice->bHalf = !pVoice->bHalf;
        }
        i++;
    }
}

// The refill's DMA callback.
void fn_800AB818(u32 bLast) {
    AudTrack* pTrack;

    pTrack = lbl_802820AC;
    pTrack->uFilled += 0x8000;
    if (bLast) {
        lbl_802820AC = NULL;
        RemoveFromAudStreamQueue(pTrack);
    }
}

// Refills the whole buffer from its start (after the stream looped).
void fn_800AB860(AudTrack* pTrack) {
    fn_80005AE8(pTrack->pBuffer, 0, sizeof(StreamChunk));
    fn_800AB72C(pTrack, fn_800AB818, 0, 0);
}

// At the stream's end: loop back, or mark it ended when it does not loop.
void fn_800AB8B4(AudTrack* pTrack) {
    if (pTrack->uFilled < pTrack->uLength) return;
    if (pTrack->pStream->uLoop == 0xFFFFFFFF) {
        pTrack->flags.b.bEnded = 1;
        return;
    }
    pTrack->uFilled = 0;
    pTrack->uPlayed = 0;
}

// A block's DMA callback.
void fn_800AB8FC(u32 bLast) {
    AudTrack* pTrack;

    pTrack = lbl_802820AC;
    lbl_802820AC->uFilled += 0x8000;
    if (bLast) {
        fn_800AB8B4(lbl_802820AC);
        lbl_802820AC = NULL;
        RemoveFromAudStreamQueue(pTrack);
    }
}

// Moves the read position past a block, back to the loop point at the stream's end.
void fn_800AB958(AudTrack* pTrack, u32 uLen) {
    u32 uLoop;

    pTrack->uRead += uLen;
    pTrack->uReadPos += uLen;
    if (pTrack->uReadPos < pTrack->uLength) return;
    uLoop = pTrack->pStream->uLoop;
    if (uLoop == 0xFFFFFFFF) return;
    pTrack->uReadPos = uLoop;
}

// A disc read is done: DMA it to the voices, unless the track moved on meanwhile.
void fn_800AB99C(int nBytes, int nError, AudTrack* pTrack, u8 nId) {
    if (pTrack->nReadId != nId || pTrack->pTmpl == NULL || pTrack->pTmpl->pPlayList == NULL ||
        pTrack->pStream == NULL) {
        RemoveFromAudStreamQueue(pTrack);
        return;
    }
    fn_800AB958(pTrack, nBytes);
    fn_800AB72C(pTrack, fn_800AB8FC, 0x8000, 1);
}

// Takes a voice per channel and queues the first read.
void fn_800ABA28(AudTrack* pTrack) {
    AudPlayList* pList;
    s32 hFile;
    AudVoiceRequest request;
    u8 i;
    AudVoice* pVoice;

    pList = pTrack->pTmpl->pPlayList;
    hFile = fn_800AC328();
    if (pList == NULL) return;
    if (pTrack->nState == 5) return;
    if (pTrack->pStream == NULL) return;
    request.flags.n = 0;
    request.flags.b.b14 = 1;
    request.nPriority = 0x3FFF;
    i = 0;
    request.n4 = 2;
    request.pfnCallback = fn_800AA400;
    request.pUser = pTrack;
    request.flags.b.b12 = 1;
    request.flags.b.b11 = pList->nId >> 2;
    for (; i < pList->nChannels; i++) {
        request.nIndex = i;
        pVoice = fn_800AC4A0(&request);
        if (pVoice == NULL) return;
        pTrack->apVoices[i] = pVoice;
    }
    if (++lbl_802820A8 == 0) {
        lbl_802820A8 = 1;
    }
    pTrack->uReadPos = 0;
    pTrack->uRead = 0;
    pTrack->uFilled = 0;
    pTrack->uPlayed = 0;
    pTrack->b5C = 0;
    pTrack->nState = 4;
    pTrack->nReadId = lbl_802820A8;
    pTrack->n5D += pList->nChannels;
    pTrack->flags.n = 0;
    fn_800AB4C0(hFile, pTrack->pBuffer, (pTrack->uBufferSize >> 1) * pList->nChannels,
                pTrack->pStream->uOffset, fn_800AB99C, pTrack, pTrack->nReadId, 0);
}

// Sets up the read queue (the module's start-up).
s32 fn_800ABBC8(void) {
    fn_80005AE8(lbl_801F18B8.aReads, 0, sizeof(lbl_801F18B8.aReads));
    fn_800AE00C(&lbl_801F18B8.queue, lbl_801F18B8.aReads, 8, sizeof(AudStreamRead));
    lbl_801F18B8.bBusy = 0;
    return 1;
}

void fn_800ABC34(AudTrack* pTrack) {
    fn_800ABC54(pTrack);
}

// Clears a track's stream state.
void fn_800ABC54(AudTrack* pTrack) {
    pTrack->pStream = NULL;
    pTrack->pBuffer = NULL;
    pTrack->uBufferSize = 0;
    pTrack->uReadPos = 0;
    pTrack->uRead = 0;
    pTrack->uFilled = 0;
    pTrack->uPlayed = 0;
    pTrack->uLength = 0;
    pTrack->nLastStream = 0xFFFF;
    pTrack->nNextStream = 0xFFFF;
    pTrack->nNextPlayList = 0xFF;
    pTrack->nReadId = 0;
    pTrack->flags.n = 0;
}

void Stm_Exit(AudTrack* pTrack) {
    fn_800B596C("Stm_Exit");
    fn_800B04CC(pTrack);
    if (pTrack->pBuffer != NULL) {
        fn_800A9434(pTrack->pBuffer, pTrack->uBufferSize, pTrack->pTmpl->pPlayList->nId);
    }
    fn_800ABC54(pTrack);
    fn_800B5994("Stm_Exit");
}

void Stm_Start(AudTrack* pTrack) {
    if (pTrack->pTmpl->pPlayList == NULL) return;
    fn_800B596C("Stm_Start");
    if (pTrack->nState != 5) {
        fn_800ABA28(pTrack);
    } else {
        fn_800AB428(pTrack);
    }
    fn_800B5994("Stm_Start");
}

void fn_800ABD7C(AudTrack* pTrack) {
    fn_800AC310(pTrack);
    pTrack->nReadId = 0;
}

// Once a frame: resume voices after a disc error, start the voices once the buffer is full, pause
// them when the reads fall behind, queue the next read, and apply waiting changes. Returns
// whether the track is past starting.
u8 Stm_Tick(AudTrack* pTrack) {
    u8 bFed;
    AudPlayList* pList;
    AudVoice** ppVoice;
    int i;
    u32 uOld;
    u32 uFull;
    u32 uLen;
    u8 bBehind;
    u32 uPos;

    bFed = 0;
    fn_800B596C("Stm_Tick");
    pList = pTrack->pTmpl->pPlayList;
    if (DVDGetDriveStatus() == 0) {
        ppVoice = pTrack->apVoices;
        for (i = 0; i < pList->nChannels; i++, ppVoice++) {
            if (*ppVoice != NULL && (*ppVoice)->bB_6) {
                (*ppVoice)->bB_6 = 0;
                fn_800ACA5C(*ppVoice, 0);
            }
        }
    }
    switch (pTrack->nState) {
    case 4:
        uFull = pList->nChannels * 0x7F00;
        if (pTrack->uLength <= uFull) {
            uFull = pTrack->uLength;
        }
        if (pTrack->uFilled != 0 && pTrack->uFilled >= uFull) {
            if (pTrack->flags.b.b6) {
                bFed = 1;
                pTrack->flags.b.b6 = 0;
                pTrack->nState = 5;
            } else {
                bFed = 1;
                fn_800AB428(pTrack);
            }
        }
        break;
    case 3:
    case 6:
        ppVoice = pTrack->apVoices;
        if (*ppVoice == NULL) break;
        uOld = (*ppVoice)->uPlayPos;
        bFed = fn_800ACE38(*ppVoice, &uPos);
        if (uOld != 0) {
            pTrack->uPlayed += (*ppVoice)->uPlayPos - uOld;
            if ((*ppVoice)->uPlayPos < uOld) {
                pTrack->uPlayed += 0xFE00;
            }
        }
        if (bFed) break;
        bBehind = 0;
        if (pTrack->uPlayed > (pTrack->uFilled >> 15) / pList->nChannels * 0x7F00 - 0xCB3) {
            bBehind = 1;
        }
        if (bBehind) {
            if (!pTrack->flags.b.bStarved) {
                pTrack->flags.b.bStarved = 1;
                for (i = 0; i < pList->nChannels; i++) {
                    fn_800ACA5C(pTrack->apVoices[i], 1);
                }
            }
        } else if (pTrack->flags.b.bStarved) {
            pTrack->flags.b.bStarved = 0;
            for (i = 0; i < pList->nChannels; i++) {
                fn_800ACA5C(pTrack->apVoices[i], 0);
            }
        }
        break;
    }
    if (bFed) {
        if (pTrack->flags.b.bEnded) {
            if (fn_800AB570(pTrack)) {
                pTrack->flags.b.bEnded = 0;
                pTrack->flags.b.b3 = 1;
            }
        } else if (pTrack->flags.b.b3) {
            pTrack->flags.b.b3 = 0;
            if (pTrack->pStream->uLoop == 0xFFFFFFFF) {
                fn_800AA1B8(pTrack, 1);
            } else {
                pTrack->uFilled = 0;
            }
        } else if (pTrack->nState != 3) {
            uLen = pTrack->uLength - pTrack->uReadPos;
            if (pList->nChannels << 15 <= uLen) {
                uLen = pList->nChannels << 15;
            }
            fn_800AB4C0(fn_800AC328(), pTrack->pBuffer, uLen, pTrack->pStream->uOffset + pTrack->uReadPos,
                        fn_800AB99C, pTrack, pTrack->nReadId, 0);
        }
    }
    ProcessAudStreamReadQueue();
    fn_800B5994("Stm_Tick");
    if (pTrack->nState == 2 && fn_800AB3A4(pTrack)) {
        Stm_Start(pTrack);
    }
    return pTrack->nState != 2;
}

// Switches the track to another play list; while it plays, the change waits for the next start.
void Stm_SetPlayList(AudTrack* pTrack, u8 nPlayList) {
    AudTrackTmpl* pTmpl;
    u8 nOld;
    AudPlayList* pList;
    AudPlayList* pOld;
    u32 uSize;

    nOld = 0;
    pTmpl = pTrack->pTmpl;
    fn_800B596C("Stm_SetPlayList");
    if (pTrack->nState > 2) {
        pTrack->nNextPlayList = nPlayList;
    } else {
        pList = fn_800A9564(nPlayList);
        if (pTrack->nState == 2) {
            pOld = pTmpl->pPlayList;
            nOld = pOld->nId;
        } else {
            pOld = NULL;
        }
        if (pList != pOld) {
            uSize = fn_800A955C(pList->nId);
            if (pTrack->pBuffer != NULL) {
                fn_800A9434(pTrack->pBuffer, pTrack->uBufferSize, nOld);
            }
            pTmpl->pPlayList = pList;
            pTrack->pBuffer = fn_800A942C(uSize, pList->nId);
            pTrack->uBufferSize = uSize;
            pTmpl->n2 = pList->nChannels;
        }
    }
    fn_800B5994("Stm_SetPlayList");
}

// Picks the stream to play (0xFFFE: a random one, not the last one again); while the track plays,
// nMode 1 cancels any waiting change and nMode 2 stops the track first.
void Stm_SetStream(AudTrack* pTrack, u16 nStream, int nMode) {
    AudTrackTmpl* pTmpl;
    AudPlayList* pList;

    pTmpl = pTrack->pTmpl;
    fn_800B596C("Stm_SetStream");
    if (pTrack->nState > 2) {
        if (nMode != 1) {
            if (nMode == 2) {
                fn_800AA1B8(pTrack, 0);
            }
            pTrack->nNextStream = nStream;
        } else {
            pTrack->nNextStream = 0xFFFF;
            pTrack->nNextPlayList = 0xFF;
        }
    } else {
        pList = pTmpl->pPlayList;
        if (pList != NULL) {
            if (nStream == 0xFFFE) {
                nStream = fn_800AB32C(pList->nStreams);
                if (nStream == pTrack->nLastStream) {
                    nStream++;
                    if (nStream >= pList->nStreams) {
                        nStream = 0;
                    }
                }
                pTrack->nLastStream = nStream;
            }
            pTrack->pStream = fn_800A9438(pList, nStream, &pTrack->uLength);
        }
    }
    fn_800B5994("Stm_SetStream");
}

void fn_800AC310(AudTrack* pTrack) {
    pTrack->nNextPlayList = 0xFF;
    pTrack->nNextStream = 0xFFFF;
}

// The stream file.
s32 fn_800AC328(void) {
    return lbl_80281468;
}
