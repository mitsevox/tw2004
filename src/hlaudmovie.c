// hlaudmovie.c (TW06's golf/audio/engine/hl/hlaudmovie.c; Mov_Exit is paired with TW06's, and
// Mov_Init, Mov_Start and Mov_Tick are EA's names from the strings they hand to the audio lock):
// the movie player's sound, which plays each chunk of a movie's stereo sound on two voices, and
// the sound engine's setup around it: the sound banks and the stream file (loaded through
// UStream.c), the volume curves and the stream buffer. Its extent is proven by its data:
// fn_800A85CC is the first function to use its .sbss (0x80282060-0x80282098; the file before it
// ends at 0x80282058), and fn_800A9808 after it is the first to use the next file's .bss
// (0x801F1868).

#include "core/audtrack.h"
#include "core/startup.h"

void Voc_PauseAll(void);
u8   fn_800AF264(u8 a, u8 b);
void fn_800AF2D8(void);
void fn_800AF2DC(u8 b);
void fn_800AF31C(u8 n);

void fn_800A8D54(void);

f32 lbl_80281460 = 1.0f;                // fn_800A86BC's rate
s32 lbl_80281468 = -1;                  // the stream file (audtrack.h)

MovieSound lbl_801F1850;
f32 lbl_801F17D0[32];

u8 lbl_80282095;
u8 lbl_80282094;
s32 lbl_80282090;
s32 lbl_8028208C;
u8* lbl_80282088;                       // the stream buffer (fn_800A942C)
s32 lbl_80282084;
s32 lbl_80282080;
u32 lbl_8028207C;                       // what is loaded: bits 0x04/0x08 bank 0 and its samples,
                                        // 0x10/0x20 bank 1 and its samples
AudBank* lbl_80282078;
AudBank* lbl_80282074;
AudStreamFile* lbl_80282070;
AudBlock48* lbl_8028206C;
u8 lbl_80282068;
u8 lbl_80282064;
s32 lbl_80282060;

// A sound by its number: bank 0's from 0 up, bank 1's from -1 down.
AudSound* fn_800A85CC(s16 nSound) {
    AudBank* pBank;

    if (nSound >= 0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
        nSound = -1 - nSound;
    }
    return pBank->apSounds[nSound];
}

f32 fn_800A85FC(f32 fVolume, f32 fCurve) {
    return fVolume * fCurve;
}

u8 fn_800A8604(void) {
    s32 i;

    lbl_80282064 = 2;
    lbl_80282060 = 0;
    for (i = 0; i < 32; i++) {
        lbl_801F17D0[i] = 1.0f;
    }
    return 1;
}

u8 fn_800A86B0(void) {
    return 1;
}

void fn_800A86B8(void) {
}

void fn_800A86BC(u8 nRate) {
    lbl_80281460 = nRate != 60 ? nRate / 60.0f : 1.0f;
}

void fn_800A8700(u8 n) {
    lbl_80282064 = n;
}

void fn_800A8708(u8 nCurve, f32 fVolume) {
    lbl_801F17D0[nCurve] = fVolume;
}

void fn_800A871C(u8 nCurves, f32* pVolumes) {
    Mem_cpy(lbl_801F17D0, pVolumes, nCurves * sizeof(f32));
}

void fn_800A874C(s32 n) {
    lbl_80282060 = n;
}

u8 fn_800A8754(void) {
    u8 bOk;

    bOk = 0;
    lbl_8028206C = fn_800B5BD8(sizeof(AudBlock48));
    if (lbl_8028206C != NULL) {
        fn_80005AE8(lbl_8028206C, 0, sizeof(AudBlock48));
        bOk = 1;
    }
    return bOk;
}

u8 fn_800A87A4(u8 a, u8 b, u8 nListeners) {
    lbl_80282068 = nListeners;
    return 1;
}

void fn_800A87B0(void) {
}

void fn_800A87B4(u8 a, u8 n) {
    fn_800AF31C(n);
}

// The ARAM transfer of a block is done: the next chunk goes into the next block of the ring.
void fn_800A87D8(u32 n) {
    if (lbl_801F1850.nState == 2 && n == 0) {
        lbl_801F1850.uSent += MOVIE_BLOCK_SIZE;
        if (++lbl_801F1850.nSendBlock >= MOVIE_BLOCKS) {
            lbl_801F1850.nSendBlock = 0;
        }
    }
}

u8 fn_800A8824(void) {
    return 1;
}

// Takes the two voices and gives them their ARAM blocks.
void Mov_Init(void) {
    AudVoiceRequest request;

    request.nPriority = 0x3FFF;
    request.flags.n = 0;
    request.n4 = 2;
    request.flags.b.b14 = 1;
    request.flags.b.b10 = 1;
    request.flags.b.b9 = 1;
    request.pfnCallback = NULL;
    request.pUser = NULL;
    request.nIndex = 0;
    fn_800B596C("Mov_Init");
    lbl_801F1850.pLeft = Voc_Alloc(&request);
    lbl_801F1850.pRight = Voc_Alloc(&request);
    if (lbl_801F1850.pLeft != NULL && lbl_801F1850.pRight != NULL) {
        lbl_801F1850.pLeft->uAram = fn_800B0790();
        lbl_801F1850.pRight->uAram = lbl_801F1850.pLeft->uAram + MOVIE_BLOCKS * MOVIE_BLOCK_SIZE;
        lbl_801F1850.uPlayed = 0;
        lbl_801F1850.uSent = 0;
        lbl_801F1850.nPlayBlock = 0;
        lbl_801F1850.nSendBlock = 0;
        lbl_801F1850.nState = 1;
    }
    fn_800B5994("Mov_Init");
}

void Mov_Exit(void) {
    fn_800B596C("Mov_Exit");
    if (lbl_801F1850.pLeft != NULL) {
        lbl_801F1850.pLeft->uAram = 0;
        Voc_Delete(lbl_801F1850.pLeft);
        lbl_801F1850.pLeft = NULL;
    }
    if (lbl_801F1850.pRight != NULL) {
        lbl_801F1850.pRight->uAram = 0;
        Voc_Delete(lbl_801F1850.pRight);
        lbl_801F1850.pRight = NULL;
    }
    lbl_801F1850.uPlayed = 0;
    lbl_801F1850.uSent = 0;
    lbl_801F1850.nPlayBlock = 0;
    lbl_801F1850.nSendBlock = 0;
    lbl_801F1850.nState = 0;
    fn_800B5994("Mov_Exit");
}

// Starts both voices looping over their rings at 22050 Hz, panned hard left and right.
void Mov_Start(void) {
    AudVoiceParams params;

    fn_800B596C("Mov_Start");
    fn_800AC7DC(lbl_801F1850.pLeft, MOVIE_BLOCKS * MOVIE_BLOCK_SIZE, 22050, 1);
    fn_800AC7DC(lbl_801F1850.pRight, MOVIE_BLOCKS * MOVIE_BLOCK_SIZE, 22050, 1);
    params.flags.n = 0;
    params.flags.b.bVolume = 1;
    params.nVolume = 0x2FFF;
    params.nPan = 0;
    params.n7 = 0x7F;
    Voc_Render(lbl_801F1850.pLeft, &params);
    params.nVolume = 0x2FFF;
    params.nPan = 0x7F;
    params.n7 = 0x7F;
    Voc_Render(lbl_801F1850.pRight, &params);
    lbl_801F1850.nState = 2;
    fn_800B5994("Mov_Start");
}

// A chunk of the movie's sound came in: each channel goes into the next block of its voice's ring.
void fn_800A8AD4(MovieSoundBlock* pBlock) {
    u32 uRight;
    int nMode;
    u8* pDataL;
    u8* pDataR;
    u32 uLeft;

    if (lbl_801F1850.nState == 0) return;
    if (lbl_801F1850.nSendBlock == 0) {
        nMode = 0;
    } else {
        nMode = 2;
        if (lbl_801F1850.nSendBlock == MOVIE_BLOCKS - 1) {
            nMode = 1;
        }
    }
    pDataL = pBlock->aDataL;
    pDataR = pDataL + MOVIE_BLOCK_SIZE;
    uLeft = lbl_801F1850.pLeft->uAram + lbl_801F1850.nSendBlock * MOVIE_BLOCK_SIZE;
    uRight = lbl_801F1850.pRight->uAram + lbl_801F1850.nSendBlock * MOVIE_BLOCK_SIZE;
    fn_800B0338(lbl_801F1850.pLeft->nHwVoice, pBlock, 0, nMode);
    fn_800B0338(lbl_801F1850.pRight->nHwVoice, pBlock, 1, nMode);
    if (lbl_801F1850.nState == 1) {
        fn_800B044C(uLeft, pDataL, MOVIE_BLOCK_SIZE, NULL, 0);
        fn_800B044C(uRight, pDataR, MOVIE_BLOCK_SIZE, NULL, 1);
    } else {
        fn_800B044C(uLeft, pDataL, MOVIE_BLOCK_SIZE, fn_800A87D8, 0);
        fn_800B044C(uRight, pDataR, MOVIE_BLOCK_SIZE, fn_800A87D8, 1);
    }
    // Before the start nothing plays, so the blocks advance here instead of in fn_800A87D8.
    if (lbl_801F1850.nState == 1) {
        lbl_801F1850.uSent += MOVIE_BLOCK_SIZE;
        if (++lbl_801F1850.nSendBlock >= MOVIE_BLOCKS) {
            lbl_801F1850.nSendBlock = 0;
        }
    }
}

// Follows the left voice through its ring: once it has played past a block, that block is done.
void Mov_Tick(void) {
    u32 uStart;

    fn_800B596C("Mov_Tick");
    switch (lbl_801F1850.nState) {
    case 2:
        uStart = lbl_801F1850.nPlayBlock * MOVIE_BLOCK_SIZE;
        if (fn_800AFD8C(lbl_801F1850.pLeft->nHwVoice) - lbl_801F1850.pLeft->uAram - uStart >=
            MOVIE_BLOCK_SIZE) {
            lbl_801F1850.uPlayed += MOVIE_BLOCK_SIZE;
            if (++lbl_801F1850.nPlayBlock >= MOVIE_BLOCKS) {
                lbl_801F1850.nPlayBlock = 0;
            }
        }
        break;
    }
    fn_800B5994("Mov_Tick");
}

void fn_800A8D00(void) {
    lbl_80282088 = fn_800B5BD8(fn_800A955C(0));
}

u8 fn_800A8D2C(void) {
    fn_800A8D54();
    fn_800A8D00();
    return 1;
}

void fn_800A8D54(void) {
    lbl_80282070 = NULL;
    lbl_80282074 = NULL;
    lbl_80282078 = NULL;
    lbl_8028207C = 0;
    lbl_80282080 = 0;
    lbl_80282084 = 0;
    lbl_80282088 = NULL;
    lbl_8028208C = 0;
    lbl_80282090 = 0;
    lbl_80282094 = 0;
    lbl_80282095 = 0;
}

void fn_800A8D88(void) {
    fn_800A86B8();
    fn_800AC49C();
    fn_800A9AC4();
    fn_800A7C2C();
    fn_800A87B0();
    fn_800AF2D8();
    fn_800B0660();
    fn_800B0448();
    fn_800AFB50();
}

// Sets the sound engine up; b == 0 also drops bank 0. Each step must succeed for the next to run.
u8 fn_800A8DC8(u8 a, u8 b, u8 nListeners) {
    u8 bOk;

    lbl_80282080 = a;
    lbl_80282084 = b;
    if (lbl_80282074 != NULL) {
        if (lbl_80282074->uAram != 0) {
            fn_800B06CC(lbl_80282074->uAram);
            lbl_80282074->uAram = 0;
        }
        fn_800B5C04(lbl_80282074);
        lbl_80282074 = NULL;
    }
    lbl_8028207C &= ~0x30;
    if ((bOk = fn_800AFB48()) && (bOk = fn_800B0440()) && (bOk = fn_800B0624()) &&
        (bOk = fn_800AF264(a, b)) && (bOk = fn_800A87A4(a, b, nListeners)) &&
        (bOk = fn_800A7C24()) && (bOk = fn_800A9A50(a, b)) && (bOk = fn_800AC494()) &&
        (bOk = fn_800A86B0())) {
        if (b == 0) {
            if (lbl_80282078 != NULL) {
                if (lbl_80282078->uAram != 0) {
                    fn_800B06CC(lbl_80282078->uAram);
                    lbl_80282078->uAram = 0;
                }
                fn_800B5C04(lbl_80282078);
                lbl_80282078 = NULL;
            }
            lbl_8028207C = 0;
        }
        lbl_8028207C |= 1;
    }
    return bOk;
}

void fn_800A8F68(u8 b) {
    Voc_PauseAll();
    fn_800AF2DC(b);
    if (b) {
        lbl_8028207C |= 0x40;
    } else {
        lbl_8028207C &= ~0x40;
    }
}

// Where UStream.c loads a bank's header: memory 0 is bank 0, 1 bank 1.
void* fn_800A8FB4(u32 uSize, u32 uMemory) {
    switch (uMemory) {
    case 0:
        return lbl_80282078 = fn_800B5BD8(uSize);
    case 1:
        return lbl_80282074 = fn_800B5BD8(uSize);
    }
    // fake match: EA bug: no return for any other memory (UStream.c passes only 0 and 1 here)
}

// A bank's header is loaded: turn its offsets into pointers.
void fn_800A8FFC(u32 uMemory) {
    AudBank* pBank;
    u8 i;
    u8* pSounds;
    u8 nGroup;
    u8* pData;
    u8* pSampleData;
    u8 bBank0;
    AudSound* pSound;
    AudGroup* pGroup;
    u32 j;
    AudTrackTmpl* pTrack;
    AudTrackTmpl* pEnd;

    bBank0 = uMemory == 0;
    if (bBank0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
    }
    pSounds = (u8*)&pBank->apSounds[pBank->nSounds];
    pData = pSounds + (pBank->n2C + pBank->nGroups * 4);
    // port: the offsets are stored in the pointer fields
    pBank->ppGroups = (AudGroup**)((u8*)pBank + (uptr)pBank->ppGroups);
    pSampleData = pData + pBank->n14;
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
        fn_800A8524(pSound, i);
        pBank->apSounds[i] = pSound;
    }
    if (bBank0) {
        lbl_8028207C |= 4;
    } else {
        lbl_8028207C |= 0x10;
    }
}

// Where UStream.c loads a bank's samples: an ARAM block.
u32 fn_800A925C(u32 uSize, u32 uMemory) {
    AudBank* pBank;

    if (uMemory == 0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
    }
    return pBank->uAram = fn_800B0698(uSize);
}

// A bank's samples are loaded: move the sample table's addresses into its ARAM block.
void fn_800A929C(u32 uMemory) {
    AudBank* pBank;
    u8 i;
    u8 bBank0;
    AudSample* pSample;

    bBank0 = uMemory == 0;
    if (bBank0) {
        pBank = lbl_80282078;
    } else {
        pBank = lbl_80282074;
    }
    pSample = pBank->pSamples;
    if (pBank->uAram == 0) {
        if (bBank0) {
            lbl_8028207C |= 8;
        } else {
            lbl_8028207C |= 0x20;
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
        lbl_8028207C |= 8;
    } else {
        lbl_8028207C |= 0x20;
    }
}

// Where UStream.c loads the stream file's header.
AudStreamFile* fn_800A9374(u32 uSize) {
    if (lbl_80282070 == NULL) {
        return lbl_80282070 = fn_800B5BD8(uSize);
    }
    return NULL;
}

// The stream file's header is loaded: open the stream file and fix the play lists' offsets up.
void fn_800A93AC(void) {
    s32 hFile;
    u8* pLists;
    u32 i;

    if (lbl_80281468 == -1) {
        hFile = fn_800060E0("/AudioStm_GC.sab");
        pLists = (u8*)lbl_80282070->apLists + lbl_80282070->nPlayLists * sizeof(AudPlayList*);
        for (i = 0; i < lbl_80282070->nPlayLists; i++) {
            // port: the offsets are stored in the pointer fields
            lbl_80282070->apLists[i] = (AudPlayList*)(pLists + (uptr)lbl_80282070->apLists[i]);
        }
        lbl_80281468 = hFile;
    }
}

u8* fn_800A942C(u32 uSize, u8 nPlayList) {
    return lbl_80282088;
}

void fn_800A9434(u8* pBuffer, u32 uSize, u8 nPlayList) {
}

// Stream nStream of a play list (the last one when it is past the end), and its length: up to
// the next stream, the next play list's first, or the end of the file.
AudStream* fn_800A9438(AudPlayList* pList, u16 nStream, u32* puLength) {
    AudStream* pStream;
    u32 uEnd;

    if (nStream >= pList->nStreams) {
        nStream = pList->nStreams - 1;
    }
    pStream = &pList->aStreams[nStream];
    if (puLength != NULL) {
        if (nStream < (u16)(pList->nStreams - 1)) {
            uEnd = pStream[1].uOffset;
        } else if (pList->nIndex < lbl_80282070->nPlayLists - 1) {
            uEnd = fn_800A9564(pList->nIndex + 1)->aStreams[0].uOffset;
        } else {
            uEnd = fn_800065B0(lbl_80281468);
        }
        *puLength = uEnd - pStream->uOffset;
    }
    return pStream;
}

void fn_800A94F4(s16 nSound, u8 nTrack, u8 bOn) {
    AudTrackTmpl* pTrack;

    pTrack = &fn_800A85CC(nSound)->aTracks[nTrack];
    if (bOn == 0) {
        pTrack->n0 |= 0x20;
    } else {
        pTrack->n0 &= 0xDF;
    }
}

u32 fn_800A955C(u8 nPlayList) {
    return 0x10000;
}

AudPlayList* fn_800A9564(u8 nPlayList) {
    if (nPlayList >= lbl_80282070->nPlayLists) {
        nPlayList = lbl_80282070->nPlayLists - 1;
    }
    return lbl_80282070->apLists[nPlayList];
}

// Sets a placed track's voices: volume, pan and doppler pitch from the sound's place.
void fn_800A9590(AudSource* pSource, AudTrack* pTrack, f32 fVolume) {
    AudVoiceParams params;
    AudVoice** ppVoice;
    AudVoice** ppEnd;
    AudVoice* pVoice;
    f32 fPitch;

    ppVoice = pTrack->apVoices;
    ppEnd = &pTrack->apVoices[pTrack->pTmpl->n2];
    fVolume = fn_800A85FC(pTrack->f48, fVolume);
    fPitch = fn_800A85FC(pSource->fPitch, pTrack->f4C);
    params.flags.n = 0;
    params.flags.b.bVolume = 1;
    params.flags.b.bPitch = 1;
    params.fPitch = fPitch;
    for (; ppVoice < ppEnd; ppVoice++) {
        pVoice = *ppVoice;
        if (pVoice != NULL) {
            params.nVolume = fn_800A85FC(fVolume, pVoice->n14 << 7);
            params.nPan = 64.0f * pSource->fPan + 64.0f;
            params.n7 = 64.0f * pSource->f68 + 64.0f;
            Voc_Render(pVoice, &params);
        }
    }
}

// Sets a track's voices when the sound is not placed: a mono track in the centre, a stereo one's
// channels left and right in turn.
void fn_800A96DC(AudSource* pSource, AudTrack* pTrack, f32 fVolume) {
    AudVoiceParams params;
    AudVoice** ppVoice;
    AudVoice** ppEnd;
    AudVoice* pVoice;
    u8 bMono;
    s8 bRight;
    u8 nChannels;

    nChannels = pTrack->pTmpl->n2;
    params.flags.n = 0;
    params.flags.b.bVolume = 1;
    params.flags.b.bPitch = 1;
    ppVoice = pTrack->apVoices;
    ppEnd = &pTrack->apVoices[nChannels];
    bRight = 0;
    bMono = nChannels == 1;
    params.fPitch = pTrack->f4C;
    for (; ppVoice < ppEnd; ppVoice++) {
        pVoice = *ppVoice;
        if (pVoice != NULL) {
            params.nVolume = fn_800A85FC(pVoice->n14 << 7, fVolume);
            params.nPan = bMono ? 0x40 : bRight ? 0x7F : 0;
            params.n7 = 0x7F;
            Voc_Render(pVoice, &params);
        }
        bRight ^= 1;
    }
}
