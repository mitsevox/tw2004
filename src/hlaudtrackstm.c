// hlaudtrackstm.c (TW06's name: golf/audio/engine/hl/hlaudtrackstm.c; TW07's HLAudTrackStm.c
// streams another way, through stream channels and SNDStatusCallback): the streamed tracks of the
// sound engine (music and long sounds read from disc, /AudioStm_GC.sab). Each track reads its
// stream into a main-memory buffer through a queue of disc reads (gAudStreamReadQueue), DMAs each
// block into the half of its voices' ARAM buffers they are not playing, and keeps the reads ahead
// of what the voices play, pausing them when the reads fall behind. A stream that does not loop
// ends with a block of silence. The functions that hand their name to the audio locks carry EA's own names
// (AddToAudStreamReadQueue, Stm_Tick, ...); CheckQueue, StartStreamVoices, PrimeStreamer,
// ResetStreamPerf, Stm_InitModule, Stm_Init, Stm_Stop and Stm_FlushQueue are TW07's; the rest are
// read from the code.

#include "core/audtrack.h"
#include "core/startup.h"

void Stm_SendSilenceToVoices(AudTrack* pTrack);
void ResetStreamPerf(AudTrack* pTrack);
void Stm_FlushQueue(AudTrack* pTrack);
s32  Stm_GetStreamFile(void);
void RemoveFromAudStreamQueue(AudTrack* pTrack);
void Stm_ReadDoneCB(void* pDst, int nBytes, AudTrack* pTrack, u8 nId);

// .bss/.sbss in reverse address order
AudStreamQueue gAudStreamReadQueue;    // the disc reads waiting, one under way at a time
AudTrack* gStmDmaTrack;                 // the track whose blocks are being DMA'd to ARAM
u8 gStmLastReadId;                      // the last read id handed out (never 0)

// Applies the play list and stream changes that came in while the track was busy (Stm_SetPlayList,
// Stm_SetStream). Returns 1 when it applied one; Stm_Tick then starts the stopped track again.
u8 CheckQueue(AudTrack* pTrack) {
    u8 bChanged;

    bChanged = 0;
    if (pTrack->u.stm.nNextPlayList != 0xFF) {
        Stm_SetPlayList(pTrack, pTrack->u.stm.nNextPlayList);
        bChanged = 1;
        pTrack->u.stm.nNextPlayList = 0xFF;
    }
    if (pTrack->u.stm.nNextStream != 0xFFFF) {
        if (pTrack->pTmpl->data.pPlayList != NULL) {
            Stm_SetStream(pTrack, pTrack->u.stm.nNextStream, 0);
            bChanged = 1;
        }
        pTrack->u.stm.nNextStream = 0xFFFF;
    }
    return bChanged;
}

// The buffer is primed: starts each channel's voice looping over its whole ARAM buffer (0xFE00
// bytes, both halves) at the play list's sample rate, with the short release for volume curve 15,
// and marks the track playing (state 6). The submix volume it works out first is not used.
void StartStreamVoices(AudTrack* pTrack) {
    AudPlayList* pList;
    u8 bLoud;
    u8 i;

    pList = pTrack->pTmpl->data.pPlayList;
    audfrac_Mul(pTrack->fVolume, Mas_GetSubmix(pList->n3));
    bLoud = pList->n3 == 15;
    for (i = 0; i < pList->nChannels; i++) {
        Voc_StartStream(pTrack->apVoices[i], 0xFE00, pList->n4, bLoud);
    }
    pTrack->nState = 6;
}

// Queues a disc read of uLen bytes at uOffset of hFile into pDst; pfnDone gets the track and nId
// when it is done (ProcessAudStreamReadQueue starts it). Returns 0 when the queue is full.
u8 AddToAudStreamReadQueue(s32 hFile, u8* pDst, u32 uLen, u32 uOffset,
                           void (*pfnDone)(void* pDst, int nBytes, AudTrack* pTrack, u8 nId),
                           AudTrack* pTrack, u8 nId, u8 n19) {
    u8 bQueued;
    AudStreamRead* pRead;

    bQueued = 0;
    AudLock_LockReadQueue("AddToAudStreamReadQueue");
    if (gAudStreamReadQueue.queue.nCount + 1 <= gAudStreamReadQueue.queue.nMax) {
        pRead = UQueue_Push(&gAudStreamReadQueue.queue);
        pRead->hFile = hFile;
        bQueued = 1;
        pRead->pDst = pDst;
        pRead->uLen = uLen;
        pRead->uOffset = uOffset;
        pRead->pfnDone = pfnDone;
        pRead->pTrack = pTrack;
        pRead->nId = nId;
        pRead->n19 = n19;
        pRead->bSilence = 0;
    }
    AudLock_UnlockReadQueue("AddToAudStreamReadQueue");
    return bQueued;
}

// Queues, in place of a read, a block of silence for every voice (Stm_SendSilenceToVoices), once a
// stream that does not loop has been sent in full. Returns 0 when the queue is full.
u8 Stm_QueueSilence(AudTrack* pTrack) {
    u8 bQueued;
    AudStreamRead* pRead;

    bQueued = 0;
    AudLock_LockReadQueue("AddToAudStreamReadQueue");
    if (gAudStreamReadQueue.queue.nCount + 1 <= gAudStreamReadQueue.queue.nMax) {
        pRead = UQueue_Push(&gAudStreamReadQueue.queue);
        memset(pRead, 0, sizeof(AudStreamRead));
        pRead->pTrack = pTrack;
        bQueued = 1;
        pRead->bSilence = 1;
    }
    AudLock_UnlockReadQueue("AddToAudStreamReadQueue");
    return bQueued;
}

// The oldest read is done: take it off the queue so the next one can start.
void RemoveFromAudStreamQueue(AudTrack* pTrack) {
    AudLock_LockReadQueue("RemoveFromAudStreamQueue");
    UQueue_Pop(&gAudStreamReadQueue.queue);
    gAudStreamReadQueue.bBusy = 0;
    AudLock_UnlockReadQueue("RemoveFromAudStreamQueue");
}

// Starts the oldest queued read, unless one is under way.
void ProcessAudStreamReadQueue(void) {
    u8 bMore;
    AudStreamRead* pRead;

    bMore = 1;
    while (bMore) {
        AudLock_LockReadQueue("ProcessAudStreamReadQueue");
        if (gAudStreamReadQueue.queue.nCount != 0 && gAudStreamReadQueue.bBusy == 0) {
            gAudStreamReadQueue.bBusy = 1;
            pRead = (AudStreamRead*)gAudStreamReadQueue.queue.pRead;
            if (pRead->bSilence) {
                Stm_SendSilenceToVoices(pRead->pTrack);
            } else {
                // port: EA passes a (pDst, nBytes, pTrack, nId) callback and the track pointer where
                // File_ReadAsyncEx takes void (*)(int nBytes, int nError) and an s32 (n1C)
                File_ReadAsyncEx(pRead->hFile, pRead->pDst, pRead->uLen, pRead->uOffset,
                                 (void (*)(int, int))pRead->pfnDone, 0, (s32)pRead->pTrack, pRead->nId,
                                 pRead->n19);
            }
        } else {
            bMore = 0;
        }
        AudLock_UnlockReadQueue("ProcessAudStreamReadQueue");
    }
}

// DMAs one 0x8000-byte chunk per channel from the track's buffer into its voice's ARAM buffer, into
// the half the voice is not playing, and flips the halves; the chunks lie uStep apart (0: all
// channels get the same one). Channels without a voice are skipped. pfnDone(1) comes with the last
// channel's DMA; the track waits for it in gStmDmaTrack.
void Stm_SendBlockToVoices(AudTrack* pTrack, void (*pfnDone)(u32 bLast), u32 uStep, u8 bSkipEmpty) {
    u8 i;
    u8* pSrc;
    AudPlayList* pList;
    AudVoice* pVoice;
    u8 nChannels;
    u32 uAram;

    i = 0;
    pSrc = pTrack->u.stm.pBuffer;
    pList = pTrack->pTmpl->data.pPlayList;
    gStmDmaTrack = pTrack;
    while (i < (nChannels = pList->nChannels)) {
        pVoice = pTrack->apVoices[i];
        if ((!bSkipEmpty || pVoice != NULL) && pVoice != NULL) {
            uAram = pVoice->uAram + pVoice->flags.b.bHalf * 0x7F00;
            HwVoice_SetStreamDecoder(pVoice->nHwVoice, (StreamChunk*)pSrc, 0x8000, pVoice->flags.b.bHalf);
            AudDma_ToAram(uAram, pSrc + 0x100, 0x7F00, pfnDone, i == nChannels - 1);
            pSrc += uStep;
            pVoice->flags.b.bHalf ^= 1;
        }
        i++;
    }
}

// DMA callback of a silence block (Stm_SendSilenceToVoices): counts it as filled; after the last
// channel it frees the read queue for the next request.
void Stm_SilenceDmaDoneCB(u32 bLast) {
    AudTrack* pTrack;

    pTrack = gStmDmaTrack;
    pTrack->u.stm.uFilled += 0x8000;
    if (bLast) {
        gStmDmaTrack = NULL;
        RemoveFromAudStreamQueue(pTrack);
    }
}

// Run by ProcessAudStreamReadQueue for Stm_QueueSilence's request: clears one chunk of the buffer
// and DMAs it (silence) into the free half of every voice.
void Stm_SendSilenceToVoices(AudTrack* pTrack) {
    // fake match: the parameter copied through void* (decomp-notes "EA's late parameter copy") puts
    // the memset's 0 before its size, as in EA's schedule
    AudTrack* pCopy = (AudTrack*)(void*)pTrack;

    Mem_set(pCopy->u.stm.pBuffer, 0, sizeof(StreamChunk));
    Stm_SendBlockToVoices(pCopy, Stm_SilenceDmaDoneCB, 0, 0);
}

// Once the whole stream has been sent to ARAM (uFilled up to uLength): a stream that does not loop
// is marked ended (bEnded, for Stm_Tick), a looping one counts its sent and played bytes from 0
// again.
void Stm_CheckStreamEnd(AudTrack* pTrack) {
    if (pTrack->u.stm.uFilled < pTrack->u.stm.uLength) return;
    if (pTrack->u.stm.pStream->uLoop == 0xFFFFFFFF) {
        pTrack->u.stm.flags.b.bEnded = 1;
        return;
    }
    pTrack->u.stm.uFilled = 0;
    pTrack->u.stm.uPlayed = 0;
}

// DMA callback of a stream block (Stm_ReadDoneCB): counts it as filled; after the last channel it
// checks for the stream's end (Stm_CheckStreamEnd) and frees the read queue for the next read.
void Stm_BlockDmaDoneCB(u32 bLast) {
    AudTrack* pTrack;

    pTrack = gStmDmaTrack;
    pTrack->u.stm.uFilled += 0x8000;
    if (bLast) {
        Stm_CheckStreamEnd(pTrack);
        gStmDmaTrack = NULL;
        RemoveFromAudStreamQueue(pTrack);
    }
}

// Moves the read position past a block, back to the loop point at the stream's end.
void Stm_AdvanceReadPos(AudTrack* pTrack, u32 uLen) {
    u32 uLoop;

    pTrack->u.stm.uRead += uLen;
    pTrack->u.stm.uReadPos += uLen;
    if (pTrack->u.stm.uReadPos < pTrack->u.stm.uLength) return;
    uLoop = pTrack->u.stm.pStream->uLoop;
    if (uLoop == 0xFFFFFFFF) return;
    pTrack->u.stm.uReadPos = uLoop;
}

// A disc read of nBytes into pDst is done: DMA it to the voices, unless the track moved on
// meanwhile. The file reader calls it with the request's buffer, length, track and id.
void Stm_ReadDoneCB(void* pDst, int nBytes, AudTrack* pTrack, u8 nId) {
    if (pTrack->u.stm.nReadId != nId || pTrack->pTmpl == NULL || pTrack->pTmpl->data.pPlayList == NULL ||
        pTrack->u.stm.pStream == NULL) {
        RemoveFromAudStreamQueue(pTrack);
        return;
    }
    Stm_AdvanceReadPos(pTrack, nBytes);
    Stm_SendBlockToVoices(pTrack, Stm_BlockDmaDoneCB, 0x8000, 1);
}

// Primes a streamed track (Stm_Start): takes a voice per channel (steal level 2, the stream flags,
// Trk_VoiceEndCB), gives the track a new read id (never 0), clears its counts, marks it filling
// (state 4) and queues the read of the stream's first half-buffer per channel. Nothing without a
// play list or stream, or when it is primed already (state 5); when a voice cannot be had it stops
// there, keeping the voices it took.
void PrimeStreamer(AudTrack* pTrack) {
    AudPlayList* pList;
    s32 hFile;
    AudVoiceRequest request;
    u8 i;
    AudVoice* pVoice;

    pList = pTrack->pTmpl->data.pPlayList;
    hFile = Stm_GetStreamFile();
    if (pList == NULL || pTrack->nState == 5) return;
    if (pTrack->u.stm.pStream == NULL) return;
    request.flags.n = 0;
    request.nPriority = 0x3FFF;
    request.nStealLevel = 2;
    request.pfnCallback = Trk_VoiceEndCB;
    request.flags.b.bNoReverb = 1;
    i = 0;
    request.pUser = pTrack;
    request.flags.b.bStream = 1;
    request.flags.b.b11 = (pList->nId >> 2) & 1;
    for (; i < pList->nChannels; i++) {
        request.nIndex = i;
        pVoice = Voc_Alloc(&request);
        if (pVoice == NULL) return;
        pTrack->apVoices[i] = pVoice;
    }
    if (++gStmLastReadId == 0) {
        gStmLastReadId = 1;
    }
    pTrack->u.stm.uReadPos = 0;
    pTrack->u.stm.uRead = 0;
    pTrack->u.stm.uFilled = 0;
    pTrack->u.stm.uPlayed = 0;
    pTrack->bits.b.bNewVariation = 0;
    pTrack->nState = 4;
    pTrack->u.stm.nReadId = gStmLastReadId;
    pTrack->nVoices += pList->nChannels;
    pTrack->u.stm.flags.n = 0;
    AddToAudStreamReadQueue(hFile, pTrack->u.stm.pBuffer,
                            (pTrack->u.stm.uBufferSize >> 1) * pList->nChannels,
                            pTrack->u.stm.pStream->uOffset, Stm_ReadDoneCB, pTrack,
                            pTrack->u.stm.nReadId, 0);
}

// Sets up the stream read queue at start-up: 8 empty requests, none under way. Always returns 1.
u8 Stm_InitModule(void) {
    Mem_set(gAudStreamReadQueue.aReads, 0, sizeof(gAudStreamReadQueue.aReads));
    UQueue_Reset(&gAudStreamReadQueue.queue, gAudStreamReadQueue.aReads, 8, sizeof(AudStreamRead));
    gAudStreamReadQueue.bBusy = 0;
    return 1;
}

// A new streamed track (Trk_AllocPerf): clears its stream state (ResetStreamPerf).
void Stm_Init(AudTrack* pTrack) {
    ResetStreamPerf(pTrack);
}

// Clears a track's stream state: no stream, no buffer, every count 0, no play list or stream change
// waiting (0xFF / 0xFFFF), read id 0.
void ResetStreamPerf(AudTrack* pTrack) {
    pTrack->u.stm.pStream = NULL;
    pTrack->u.stm.pBuffer = NULL;
    pTrack->u.stm.uBufferSize = 0;
    pTrack->u.stm.uReadPos = 0;
    pTrack->u.stm.uRead = 0;
    pTrack->u.stm.uFilled = 0;
    pTrack->u.stm.uPlayed = 0;
    pTrack->u.stm.uLength = 0;
    pTrack->u.stm.nLastStream = 0xFFFF;
    pTrack->u.stm.nNextStream = 0xFFFF;
    pTrack->u.stm.nNextPlayList = 0xFF;
    pTrack->u.stm.nReadId = 0;
    pTrack->u.stm.flags.n = 0;
}

// A freed streamed track (Trk_FreePerf): under the stream lock, asks for its ARAM transfers to be
// cancelled (none ever is: see below), gives its buffer back and clears its stream state
// (ResetStreamPerf).
void Stm_Exit(AudTrack* pTrack) {
    AudLock_Lock("Stm_Exit");
    // EA bug: the cancel looks for transfers owned by pTrack, but every transfer is queued with
    // AudDma_ToAram's last argument as its owner, 0 or 1 (Stm_SendBlockToVoices passes "last
    // channel"), so nothing is cancelled. A block DMA still queued for the freed track goes on
    // into the ARAM buffers Voc_Delete has just given back, and its callback still counts into
    // the freed track through gStmDmaTrack.
    // port: queue the stream DMAs with the track as owner (and "last channel" apart) so they can
    //       be cancelled here.
    AudDma_CancelOwner(pTrack);
    if (pTrack->u.stm.pBuffer != NULL) {
        Ses_FreeStreamBuffer(pTrack->u.stm.pBuffer, pTrack->u.stm.uBufferSize,
                             pTrack->pTmpl->data.pPlayList->nId);
    }
    ResetStreamPerf(pTrack);
    AudLock_Unlock("Stm_Exit");
}

// Starts a streamed track (Trk_Start), under the stream lock: primes it (PrimeStreamer) or, when it
// is primed already (state 5), starts its voices. Nothing without a play list.
void Stm_Start(AudTrack* pTrack) {
    if (pTrack->pTmpl->data.pPlayList == NULL) return;
    AudLock_Lock("Stm_Start");
    if (pTrack->nState != 5) {
        PrimeStreamer(pTrack);
    } else {
        StartStreamVoices(pTrack);
    }
    AudLock_Unlock("Stm_Start");
}

// Stops a streamed track (Trk_Stop): drops its waiting changes, and read id 0 makes the reads still
// under way be ignored (Stm_ReadDoneCB).
void Stm_Stop(AudTrack* pTrack) {
    Stm_FlushQueue(pTrack);
    pTrack->u.stm.nReadId = 0;
}

// Once a frame (Trk_Tick), under the stream lock: resumes the voices paused by a disc error once
// the drive is fine; a filling track (state 4) starts its voices once its first blocks are in ARAM;
// a playing or stopping one counts the bytes played and pauses its voices while playing is about to
// catch up with the data sent (bStarved), resuming them after. When the voices start and each time
// a half of their ARAM buffers is free, it queues silence after a stream that has ended, stops the
// voices once that silence is in (a stream that does not loop), or, unless stopping, queues the
// next read. Then it starts the queued reads, and a stopped track with a waiting change
// (CheckQueue) is started again. Returns 0 once the track has stopped.
u8 Stm_Tick(AudTrack* pTrack) {
    u8 bFeed;
    AudPlayList* pList;
    AudVoice** ppVoice;
    int i;
    u32 uOld;
    u32 uFull;
    u32 uLen;
    u8 bBehind;
    u32 uPos;
    u32 uThreshold;

    bFeed = 0;
    AudLock_Lock("Stm_Tick");
    pList = pTrack->pTmpl->data.pPlayList;
    if (DVDGetDriveStatus() == 0) {
        for (i = 0; i < pList->nChannels; i++) {
            if (pTrack->apVoices[i] != NULL && pTrack->apVoices[i]->flags.b.bHeld) {
                pTrack->apVoices[i]->flags.b.bHeld = 0;
                Voc_Pause(pTrack->apVoices[i], 0);
            }
        }
    }
    switch (pTrack->nState) {
    case 4:
        uFull = pList->nChannels * 0x7F00;
        uFull = pTrack->u.stm.uLength <= uFull ? pTrack->u.stm.uLength : uFull;
        if (pTrack->u.stm.uFilled != 0 && pTrack->u.stm.uFilled >= uFull) {
            if (pTrack->u.stm.flags.b.bHold) {
                bFeed = 1;
                pTrack->u.stm.flags.b.bHold = 0;
                pTrack->nState = 5;
            } else {
                bFeed = 1;
                StartStreamVoices(pTrack);
            }
        }
        break;
    case 3:
    case 6:
        ppVoice = pTrack->apVoices;
        if (*ppVoice == NULL) break;
        uOld = (*ppVoice)->uPlayPos;
        bFeed = Voc_CheckStreamHalfDone(*ppVoice, &uPos);
        if (uOld != 0) {
            pTrack->u.stm.uPlayed += (*ppVoice)->uPlayPos - uOld;
            if ((*ppVoice)->uPlayPos < uOld) {
                pTrack->u.stm.uPlayed += 0xFE00;
            }
        }
        if (bFeed) break;
        uThreshold = pTrack->u.stm.uFilled >> 15;
        uThreshold /= pList->nChannels;
        bBehind = 0;
        if (pTrack->u.stm.uPlayed > uThreshold * 0x7F00 - 0xCB3) {
            bBehind = 1;
        }
        if (bBehind) {
            if (!pTrack->u.stm.flags.b.bStarved) {
                pTrack->u.stm.flags.b.bStarved = 1;
                for (i = 0; i < pList->nChannels; i++) {
                    Voc_Pause(ppVoice[i], 1);
                }
            }
        } else if (pTrack->u.stm.flags.b.bStarved) {
            pTrack->u.stm.flags.b.bStarved = 0;
            for (i = 0; i < pList->nChannels; i++) {
                Voc_Pause(ppVoice[i], 0);
            }
        }
        break;
    }
    if (bFeed) {
        if (pTrack->u.stm.flags.b.bEnded) {
            if (Stm_QueueSilence(pTrack)) {
                pTrack->u.stm.flags.b.bEnded = 0;
                pTrack->u.stm.flags.b.bSilenceQueued = 1;
            }
        } else if (pTrack->u.stm.flags.b.bSilenceQueued) {
            pTrack->u.stm.flags.b.bSilenceQueued = 0;
            if (pTrack->u.stm.pStream->uLoop == 0xFFFFFFFF) {
                Trk_StopAllVoices(pTrack, 1);
            } else {
                pTrack->u.stm.uFilled = 0;
            }
        } else if (pTrack->nState != 3) {
            u32 uRemaining; // fake match: separate read length keeps the original register allocation
            s32 hFile = Stm_GetStreamFile();
            // fake match: the cap first and the three volatile reads (same values) keep EA's load
            // order; otherwise the last scheduling pass lifts the cap's shift above uReadPos's load
            u32 uCap = pList->nChannels << 15;
            AudStream* pStream = ((volatile AudTrack*)pTrack)->u.stm.pStream;
            u32 uReadPos = ((volatile AudTrack*)pTrack)->u.stm.uReadPos;
            u32 uOffset = ((volatile AudStream*)pStream)->uOffset + uReadPos;
            u8* pBuffer = pTrack->u.stm.pBuffer;
            uRemaining = pTrack->u.stm.uLength - uReadPos;
            uLen = uRemaining;
            if (uCap <= uLen) {
                uLen = uCap;
            }
            AddToAudStreamReadQueue(hFile, pBuffer, uLen, uOffset, Stm_ReadDoneCB, pTrack,
                                    pTrack->u.stm.nReadId, 0);
        }
    }
    ProcessAudStreamReadQueue();
    AudLock_Unlock("Stm_Tick");
    if (pTrack->nState == 2 && CheckQueue(pTrack)) {
        Stm_Start(pTrack);
    }
    return pTrack->nState != 2;
}

// Switches a streamed track to play list nPlayList; while it is busy (state 3 and up) the change
// waits (CheckQueue). A new play list takes a new buffer and is written into the template itself,
// with its channel count, so it holds for every track of that sound.
void Stm_SetPlayList(AudTrack* pTrack, u8 nPlayList) {
    AudTrackTmpl* pTmpl;
    u8 nOld;
    AudPlayList* pList;
    AudPlayList* pOld;
    u32 uSize;
    u8* pBuffer;

    nOld = 0;
    pTmpl = pTrack->pTmpl;
    AudLock_Lock("Stm_SetPlayList");
    if (pTrack->nState > 2) {
        pTrack->u.stm.nNextPlayList = nPlayList;
    } else {
        pList = Ses_GetStreamPlayList(nPlayList);
        if (pTrack->nState == 2) {
            pOld = pTmpl->data.pPlayList;
            nOld = pOld->nId;
        } else {
            pOld = NULL;
        }
        if (pList != pOld) {
            uSize = Ses_GetStreamBufferSize(pList->nId);
            if (pTrack->u.stm.pBuffer != NULL) {
                Ses_FreeStreamBuffer(pTrack->u.stm.pBuffer, pTrack->u.stm.uBufferSize, nOld);
            }
            pBuffer = Ses_GetStreamBuffer(uSize, pList->nId);
            pTmpl->data.pPlayList = pList;
            pTrack->u.stm.pBuffer = pBuffer;
            pTrack->u.stm.uBufferSize = uSize;
            pTmpl->n2 = pList->nChannels;
        }
    }
    AudLock_Unlock("Stm_SetPlayList");
}

// Picks the stream to play (0xFFFE: a random one, not the last one again) and its length. While the
// track is busy (state 3 and up) the stream waits for the next start: nMode 0 as is, 2 after
// letting the voices end; nMode 1 instead cancels every waiting change.
void Stm_SetStream(AudTrack* pTrack, u16 nStream, int nMode) {
    AudTrackTmpl* pTmpl;
    AudPlayList* pList;

    pTmpl = pTrack->pTmpl;
    AudLock_Lock("Stm_SetStream");
    if (pTrack->nState > 2) {
        if (nMode != 1) {
            if (nMode == 2) {
                Trk_StopAllVoices(pTrack, 0);
            }
            pTrack->u.stm.nNextStream = nStream;
        } else {
            pTrack->u.stm.nNextStream = 0xFFFF;
            pTrack->u.stm.nNextPlayList = 0xFF;
        }
    } else {
        pList = pTmpl->data.pPlayList;
        if (pList != NULL) {
            if (nStream == 0xFFFE) {
                nStream = Aud_RandomBelow(pList->nStreams);
                if (nStream == pTrack->u.stm.nLastStream) {
                    nStream++;
                    if (nStream >= pList->nStreams) {
                        nStream = 0;
                    }
                }
                pTrack->u.stm.nLastStream = nStream;
            }
            pTrack->u.stm.pStream = Ses_GetStreamFromPlayList(pList, nStream, &pTrack->u.stm.uLength);
        }
    }
    AudLock_Unlock("Stm_SetStream");
}

// Drops a streamed track's waiting play list and stream changes.
void Stm_FlushQueue(AudTrack* pTrack) {
    pTrack->u.stm.nNextPlayList = 0xFF;
    pTrack->u.stm.nNextStream = 0xFFFF;
}

// The stream file's handle (hlaudsession.c opens /AudioStm_GC.sab).
s32 Stm_GetStreamFile(void) {
    return gSesStreamFile;
}
