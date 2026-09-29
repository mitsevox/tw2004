// hlaudsession.c (EA's name: TW06 lists golf/audio/engine/hl/hlaudsession.c, and TW07's
// HLAudSession.c has these Ses_ functions, among others): the sound engine's sessions, started,
// paused and ended, the sound banks and the stream file loaded through UStream.c, and the stream
// buffer. Its data: gSesStreamFile (.sdata 0x80281468), the session's globals (.sbss
// 0x80282070-0x80282095, after hlaudmic.c's) and the stream file's name (.data 0x8018EB60). It
// ends where TrkRender3D begins (0x800A9590), which TW07 has at the start of HLAudTrack.c.
// Ses_GetEmitterTemplateFromID (TW07: HLAudSession.c's) sits at the start of HLAudMaster.c.

#include "core/audtrack.h"
#include "core/startup.h"

void Voc_PauseAll(u8 bPause, u8 bStreams);      // hlaudvoice.c
u8   Rvb_InitSession(u8 nSession, u8 nSubsession);      // AudReverb.c
void Rvb_ExitSession(void);
void Rvb_Pause(u8 bMute);

void Ses_ResetModule(void);

s32 gSesStreamFile = -1;                // the stream file's handle, -1 until it is opened

// Defined last-address-first, as CodeWarrior lays out .sbss.
u8 gSesUnread95;                        // these four: cleared by Ses_ResetModule, read nowhere
u8 gSesUnread94;
s32 gSesUnread90;
s32 gSesUnread8C;
u8* gSesStreamBuffer;                   // the read buffer all streamed tracks share
s32 gSesSubsession;                     // the session's ids, as Ses_Init was given them
s32 gSesSession;
u32 gSesFlags;                          // 0x01 set up, 0x04/0x08 bank 0 and its samples loaded,
                                        // 0x10/0x20 bank 1 and its samples, 0x40 paused
AudBank* gSesBank0;                     // the two sound banks' headers
AudBank* gSesBank1;
AudStreamFile* gSesStreamFileHdr;       // the stream file's header

// Takes the one read buffer every streamed track shares (Ses_GetStreamBuffer hands it out) from the sound
// engine's memory: Ses_GetStreamBufferSize bytes (0x10000).
void Ses_AllocStreamBuffer(void) {
    gSesStreamBuffer = AudMem_Alloc(Ses_GetStreamBufferSize(0));
}

// The session's start-up step in Aud_InitOnce: clears the session's state (Ses_ResetModule) and
// takes the stream buffer. Always 1, even when the buffer could not be had.
u8 Ses_InitModule(void) {
    Ses_ResetModule();
    Ses_AllocStreamBuffer();
    return 1;
}

// Clears the session's state: no banks, no stream file header, nothing loaded, no ids, no stream
// buffer.
void Ses_ResetModule(void) {
    gSesStreamFileHdr = NULL;
    gSesBank1 = NULL;
    gSesBank0 = NULL;
    gSesFlags = 0;
    gSesSession = 0;
    gSesSubsession = 0;
    gSesStreamBuffer = NULL;
    gSesUnread8C = 0;
    gSesUnread90 = 0;
    gSesUnread94 = 0;
    gSesUnread95 = 0;
}

// Ends a sound session (Aud_ExitSession): the master, voice, track, emitter, listener and reverb
// session steps (most empty in this build), then startUp.c's: the boot DMA buffer freed if still
// held (AudAram_ExitSession) and every hardware voice stopped (HwVoice_ExitSession). The subsession
// id EA passes is not read.
void Ses_Exit(void) {
    Mas_ExitSession();
    Voc_ExitSession();
    Trk_ExitSession();
    Emi_ExitSession();
    Mic_ExitSession();
    Rvb_ExitSession();
    AudAram_ExitSession();
    AudDma_ExitSession();
    HwVoice_ExitSession();
}

// Starts a sound session (Aud_InitSession): nSession is 0 for the front end, 1 for play, the
// course + 1 on a hole; nSubsession is 1 on a hole; nListeners one per view. Bank 1 is always
// dropped (its ARAM block and header freed); then startUp.c's, the reverb's, listeners',
// emitters', tracks', voices' and master's session steps run in turn, each only if the one before
// succeeded. When they all succeed, nSubsession 0 drops bank 0 as well and clears the loaded bits,
// and bit 0x01 of gSesFlags (set up) is set. Returns 0 when a step failed, else 1.
u8 Ses_Init(u8 nSession, u8 nSubsession, u8 nListeners) {
    u8 bOk;

    gSesSession = nSession;
    gSesSubsession = nSubsession;
    if (gSesBank1 != NULL) {
        if (gSesBank1->uAram != 0) {
            AudAram_Free(gSesBank1->uAram);
            gSesBank1->uAram = 0;
        }
        AudMem_Free(gSesBank1);
        gSesBank1 = NULL;
    }
    gSesFlags &= ~0x30;
    if ((bOk = HwVoice_InitSession()) && (bOk = AudDma_InitSession()) && (bOk = AudAram_InitSession()) &&
        (bOk = Rvb_InitSession(nSession, nSubsession)) &&
        (bOk = Mic_InitSession(nSession, nSubsession, nListeners)) && (bOk = Emi_InitSession()) &&
        (bOk = Trk_InitSession(nSession, nSubsession)) && (bOk = Voc_InitSession()) &&
        (bOk = Mas_InitSession())) {
        if (nSubsession == 0) {
            if (gSesBank0 != NULL) {
                if (gSesBank0->uAram != 0) {
                    AudAram_Free(gSesBank0->uAram);
                    gSesBank0->uAram = 0;
                }
                AudMem_Free(gSesBank0);
                gSesBank0 = NULL;
            }
            gSesFlags = 0;
        }
        gSesFlags |= 1;
    }
    return bOk;
}

// Pauses (bPause 1) or resumes all voices (Voc_PauseAll; with bSpinupDelay the streamed voices stay
// paused for Stm_Tick to resume), takes the reverb off while paused (Rvb_Pause), and keeps the
// state in bit 0x40 of gSesFlags: while it is set, Trk_Cycle ticks only some tracks.
void Ses_Pause(u8 bPause, u8 bSpinupDelay) {
    Voc_PauseAll(bPause, bSpinupDelay);
    Rvb_Pause(bPause);
    if (bPause) {
        gSesFlags |= 0x40;
    } else {
        gSesFlags &= ~0x40;
    }
}

// UStream.c's 'shdr' chunk for bank uMemory (0 or 1): takes uSize bytes of the sound engine's
// memory for the bank's header and returns where to load it.
void* Ses_AllocBankHdr(u32 uSize, u32 uMemory) {
    switch (uMemory) {
    case 0:
        return gSesBank0 = AudMem_Alloc(uSize);
    case 1:
        return gSesBank1 = AudMem_Alloc(uSize);
    }
    // fake match: EA bug: no return for any other memory (UStream.c passes only 0 and 1 here)
}

// Bank uMemory's header is loaded (UStream.c): turns its stored offsets into pointers: the group
// table and each group's sample entries, the sample table, and each sound's tracks (their play list
// or sequence bank, and their events); each sound is then checked (Emi_CheckTemplate). Sets bit
// 0x04 (bank 0) or 0x10 (bank 1) of gSesFlags.
void Ses_ProcessArticulationData(u32 uMemory) {
    AudBank* pBank;
    u8 i;
    u8* pSounds;
    u8 nGroup;
    u8* pData;
    u8* pSampleData;
    u8 bBank0;
    AudSound* pSound;
    u32 j;
    AudGroup* pGroup;
    AudTrackTmpl* pTrack;
    AudTrackTmpl* pEnd;

    bBank0 = uMemory == 0;
    if (bBank0) {
        pBank = gSesBank0;
    } else {
        pBank = gSesBank1;
    }
    pSounds = (u8*)&pBank->apSounds[pBank->nSounds];
    pData = pSounds + pBank->n2C + pBank->nGroups * 4;
    pSampleData = pData + pBank->n14;
    // port: the offsets are stored in the pointer fields
    pBank->ppGroups = (AudGroup**)((u8*)pBank + (uptr)pBank->ppGroups);
    pBank->pSamples = (AudSample*)(pSampleData + pBank->n24);
    for (nGroup = 0; nGroup < pBank->nGroups; nGroup++) {
        pGroup = (AudGroup*)(pData + (uptr)pBank->ppGroups[nGroup]);
        for (j = 0; j < pGroup->nEntries; j++) {
            pGroup->aEntries[j].pSample =
                (AudSample*)((u8*)pBank->pSamples + (uptr)pGroup->aEntries[j].pSample);
        }
        pBank->ppGroups[nGroup] = pGroup;
    }
    for (i = 0; i < pBank->nSounds; i++) {
        pSound = (AudSound*)(pSounds + (uptr)pBank->apSounds[i]);
        pTrack = pSound->aTracks;
        pEnd = pTrack + pSound->nTracks;
        for (; pTrack < pEnd; pTrack++) {
            // a sequenced track's data.pBank is the same kind of offset
            pTrack->data.pPlayList = (AudPlayList*)(pData + (uptr)pTrack->data.pPlayList);
            pTrack->pEvents = (AudSeqEvent*)(pSampleData + (uptr)pTrack->pEvents);
        }
        Emi_CheckTemplate(pSound, i);
        pBank->apSounds[i] = pSound;
    }
    if (bBank0) {
        gSesFlags |= 4;
    } else {
        gSesFlags |= 0x10;
    }
}

// UStream.c's 'samp' chunk: takes an ARAM block of uSize bytes for bank uMemory's samples (0 is
// bank 0, anything else bank 1) and returns its ARAM address, where the samples are DMA'd.
u32 Ses_AllocSampleAram(u32 uSize, u32 uMemory) {
    AudBank* pBank;

    if (uMemory == 0) {
        pBank = gSesBank0;
    } else {
        pBank = gSesBank1;
    }
    return pBank->uAram = AudAram_Alloc(uSize);
}

// Bank uMemory's samples are in ARAM (UStream.c): moves each sample's start, end and loop addresses
// (4-bit units, so plus uAram * 2) into the bank's ARAM block; a sample that does not loop (uC 0)
// gets 0x8002 as its loop address. Sets bit 0x08 (bank 0) or 0x20 (bank 1) of gSesFlags, also
// when the bank has no ARAM block.
void Ses_ProcessSampleData(u32 uMemory) {
    AudBank* pBank;
    u8 i;
    u8 bBank0;
    AudSample* pSample;

    bBank0 = uMemory == 0;
    if (bBank0) {
        pBank = gSesBank0;
    } else {
        pBank = gSesBank1;
    }
    pSample = pBank->pSamples;
    if (pBank->uAram == 0) {
        if (bBank0) {
            gSesFlags |= 8;
        } else {
            gSesFlags |= 0x20;
        }
        return;
    }
    for (i = 0; i < pBank->nSamples; pSample++, i++) {
        pSample->u0 += pBank->uAram * 2;
        pSample->u4 += pBank->uAram * 2;
        if (pSample->uC != 0) {
            pSample->u8 += pBank->uAram * 2;
        } else {
            pSample->u8 = 0x8002;
        }
    }
    if (bBank0) {
        gSesFlags |= 8;
    } else {
        gSesFlags |= 0x20;
    }
}

// UStream.c's 'shdr' chunk with id 2: takes uSize bytes of the sound engine's memory for the stream
// file's header and returns where to load it; NULL when a header is already loaded.
AudStreamFile* Ses_AllocStreamFileHdr(u32 uSize) {
    if (gSesStreamFileHdr == NULL) {
        return gSesStreamFileHdr = AudMem_Alloc(uSize);
    }
    return NULL;
}

// The stream file's header is loaded (UStream.c): opens the stream file ("/AudioStm_GC.sab") and
// turns the play lists' stored offsets into pointers. Only once: nothing happens while the file is
// open (gSesStreamFile not -1).
void Ses_ProcessStreamFileHdr(void) {
    s32 hFile;
    u8* pLists;
    u32 i;

    if (gSesStreamFile == -1) {
        hFile = fn_800060E0("/AudioStm_GC.sab");
        pLists = (u8*)gSesStreamFileHdr->apLists + gSesStreamFileHdr->nPlayLists * sizeof(AudPlayList*);
        for (i = 0; i < gSesStreamFileHdr->nPlayLists; i++) {
            // port: the offsets are stored in the pointer fields
            gSesStreamFileHdr->apLists[i] = (AudPlayList*)(pLists + (uptr)gSesStreamFileHdr->apLists[i]);
        }
        gSesStreamFile = hFile;
    }
}

// A streamed track's read buffer (Stm_SetPlayList): every play list gets the same shared buffer,
// whatever the size.
u8* Ses_GetStreamBuffer(u32 uSize, u8 nPlayList) {
    return gSesStreamBuffer;
}

// Gives a streamed track's read buffer back (Stm_Exit, Stm_SetPlayList): empty, as the one buffer
// is shared and never freed.
void Ses_FreeStreamBuffer(u8* pBuffer, u32 uSize, u8 nPlayList) {
}

// Stream nStream of a play list (the last one when it is past the end), and its length: up to
// the next stream, the next play list's first, or the end of the file.
AudStream* Ses_GetStreamFromPlayList(AudPlayList* pList, u16 nStream, u32* puLength) {
    AudStream* pStream;
    u32 uEnd;

    if (nStream >= pList->nStreams) {
        nStream = pList->nStreams - 1;
    }
    pStream = &pList->aStreams[nStream];
    if (puLength != NULL) {
        if (nStream < (u16)(pList->nStreams - 1)) {
            uEnd = pStream[1].uOffset;
        } else if (pList->nIndex < gSesStreamFileHdr->nPlayLists - 1) {
            uEnd = Ses_GetStreamPlayList(pList->nIndex + 1)->aStreams[0].uOffset;
        } else {
            uEnd = fn_800065B0(gSesStreamFile);
        }
        *puLength = uEnd - pStream->uOffset;
    }
    return pStream;
}

// Turns reverb on (bOn 1) or off for track nTrack of sound nSound's template (bit 0x20 of the
// track's n0, set: no reverb), so for every instance of the sound (Aud_SesTmplOvrTrackRvbMode).
void Ses_TmplOvrTrackRvbMode(s16 nSound, u8 nTrack, u8 bOn) {
    AudTrackTmpl* pTrack;

    pTrack = &Ses_GetEmitterTemplateFromID(nSound)->aTracks[nTrack];
    if (bOn == 0) {
        pTrack->n0 |= 0x20;
    } else {
        pTrack->n0 &= 0xDF;
    }
}

// The read buffer size play list nPlayList needs: 0x10000 bytes for every one.
u32 Ses_GetStreamBufferSize(u8 nPlayList) {
    return 0x10000;
}

// Play list nPlayList of the stream file (the last one when it is past the end).
AudPlayList* Ses_GetStreamPlayList(u8 nPlayList) {
    if (nPlayList >= gSesStreamFileHdr->nPlayLists) {
        nPlayList = gSesStreamFileHdr->nPlayLists - 1;
    }
    return gSesStreamFileHdr->apLists[nPlayList];
}
