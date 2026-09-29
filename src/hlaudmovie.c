// hlaudmovie.c (EA's name: TW06 lists golf/audio/engine/hl/hlaudmovie.c; TW07 has neither the
// file nor the Mov_ functions, whose names are EA's, from the strings they hand to the audio
// lock): the movie player's sound, which plays each chunk of a movie's stereo sound on two
// voices. Its extent: the Mov_ functions between TW07's HLAudMic.c functions (hlaudmic.c) and
// HLAudSession.c's (hlaudsession.c); its data is gMovieSound (.bss 0x801F1850) and the four lock
// names (.data 0x8018EB30-0x8018EB5D).

#include "core/audtrack.h"
#include "core/startup.h"

MovieSound gMovieSound;                 // the movie's two voices and their rings

// DMA-done callback of Mov_SendSoundBlock while the movie plays: nChannel is the channel of the
// finished transfer (0 left, 1 right). The left one's arrival counts the block as sent and moves
// nSendBlock on round the ring.
void Mov_BlockSentCB(u32 nChannel) {
    if (gMovieSound.nState == 2 && nChannel == 0) {
        gMovieSound.uSent += MOVIE_BLOCK_SIZE;
        if (++gMovieSound.nSendBlock >= MOVIE_BLOCKS) {
            gMovieSound.nSendBlock = 0;
        }
    }
}

// The movie sound's start-up step in Aud_InitOnce: nothing to set up. Always 1.
u8 Mov_InitModule(void) {
    return 1;
}

// Takes the movie's two voices (left, right; priority 0x3FFF, list 2, looping) and places their
// rings of MOVIE_BLOCKS blocks in ARAM: the left one at AudAram_GetMovieBuffer's address (0x4400), the right
// one after it. With both voices the state becomes 1 (filling before the start); without, it stays
// 0 (off). Holds the audio lock throughout.
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
    AudLock_Lock("Mov_Init");
    gMovieSound.pLeft = Voc_Alloc(&request);
    gMovieSound.pRight = Voc_Alloc(&request);
    if (gMovieSound.pLeft != NULL && gMovieSound.pRight != NULL) {
        gMovieSound.pLeft->uAram = AudAram_GetMovieBuffer();
        gMovieSound.pRight->uAram = gMovieSound.pLeft->uAram + MOVIE_BLOCKS * MOVIE_BLOCK_SIZE;
        gMovieSound.uPlayed = 0;
        gMovieSound.uSent = 0;
        gMovieSound.nPlayBlock = 0;
        gMovieSound.nSendBlock = 0;
        gMovieSound.nState = 1;
    }
    AudLock_Unlock("Mov_Init");
}

// Gives the movie's two voices back (Voc_Delete) and resets both rings; the state becomes 0 (off).
// Holds the audio lock throughout.
void Mov_Exit(void) {
    AudLock_Lock("Mov_Exit");
    if (gMovieSound.pLeft != NULL) {
        gMovieSound.pLeft->uAram = 0;
        Voc_Delete(gMovieSound.pLeft);
        gMovieSound.pLeft = NULL;
    }
    if (gMovieSound.pRight != NULL) {
        gMovieSound.pRight->uAram = 0;
        Voc_Delete(gMovieSound.pRight);
        gMovieSound.pRight = NULL;
    }
    gMovieSound.uPlayed = 0;
    gMovieSound.uSent = 0;
    gMovieSound.nPlayBlock = 0;
    gMovieSound.nSendBlock = 0;
    gMovieSound.nState = 0;
    AudLock_Unlock("Mov_Exit");
}

// Starts both voices looping over their rings at 22050 Hz, panned hard left and right.
void Mov_Start(void) {
    AudVoiceParams params;

    AudLock_Lock("Mov_Start");
    Voc_StartStream(gMovieSound.pLeft, MOVIE_BLOCKS * MOVIE_BLOCK_SIZE, 22050, 1);
    Voc_StartStream(gMovieSound.pRight, MOVIE_BLOCKS * MOVIE_BLOCK_SIZE, 22050, 1);
    params.flags.n = 0;
    params.flags.b.bVolume = 1;
    params.nVolume = 0x2FFF;
    params.nPan = 0;
    params.n7 = 0x7F;
    Voc_Render(gMovieSound.pLeft, &params);
    params.nVolume = 0x2FFF;
    params.nPan = 0x7F;
    params.n7 = 0x7F;
    Voc_Render(gMovieSound.pRight, &params);
    gMovieSound.nState = 2;
    AudLock_Unlock("Mov_Start");
}

// A chunk of the movie's sound came in (UStream.c's DSPM, VAGM and XADP chunks): each voice's
// decoder is set from it (HwVoice_SetMovieDecoder) and each channel is DMA'd into the next block of
// its voice's ring. Nothing while off (state 0). Before the start (state 1) the blocks advance here; while
// playing, Mov_BlockSentCB advances them when the left channel's DMA is done.
void Mov_SendSoundBlock(MovieSoundBlock* pBlock) {
    int nMode;
    u8* pDataL;
    u8* pDataR;
    u32 uLeft;
    u32 uRight;

    if (gMovieSound.nState == 0) return;
    if (gMovieSound.nSendBlock == 0) {
        nMode = 0;
    } else {
        nMode = 2;
        if (gMovieSound.nSendBlock == MOVIE_BLOCKS - 1) {
            nMode = 1;
        }
    }
    pDataL = pBlock->aDataL;
    pDataR = pDataL + MOVIE_BLOCK_SIZE;
    uLeft = gMovieSound.pLeft->uAram + gMovieSound.nSendBlock * MOVIE_BLOCK_SIZE;
    uRight = gMovieSound.pRight->uAram + gMovieSound.nSendBlock * MOVIE_BLOCK_SIZE;
    HwVoice_SetMovieDecoder(gMovieSound.pLeft->nHwVoice, pBlock, 0, nMode);
    HwVoice_SetMovieDecoder(gMovieSound.pRight->nHwVoice, pBlock, 1, nMode);
    if (gMovieSound.nState == 1) {
        AudDma_ToAram(uLeft, pDataL, MOVIE_BLOCK_SIZE, NULL, 0);
        AudDma_ToAram(uRight, pDataR, MOVIE_BLOCK_SIZE, NULL, 1);
    } else {
        // fake match: pBlock->aDataL instead of pDataL (same address) keeps pBlock live into this
        // branch: its 29th allocator neighbour puts it in r30 ahead of the &gMovieSound temp.
        AudDma_ToAram(uLeft, pBlock->aDataL, MOVIE_BLOCK_SIZE, Mov_BlockSentCB, 0);
        AudDma_ToAram(uRight, pDataR, MOVIE_BLOCK_SIZE, Mov_BlockSentCB, 1);
    }
    // Before the start nothing plays, so the blocks advance here instead of in Mov_BlockSentCB.
    if (gMovieSound.nState == 1) {
        gMovieSound.uSent += MOVIE_BLOCK_SIZE;
        if (++gMovieSound.nSendBlock >= MOVIE_BLOCKS) {
            gMovieSound.nSendBlock = 0;
        }
    }
}

// Follows the left voice through its ring: once it has played past a block, that block is done.
void Mov_Tick(void) {
    u32 uStart;

    AudLock_Lock("Mov_Tick");
    switch (gMovieSound.nState) {
    case 2:
        uStart = gMovieSound.nPlayBlock * MOVIE_BLOCK_SIZE;
        if (HwVoice_GetPlayPos(gMovieSound.pLeft->nHwVoice) - gMovieSound.pLeft->uAram - uStart >=
            MOVIE_BLOCK_SIZE) {
            gMovieSound.uPlayed += MOVIE_BLOCK_SIZE;
            if (++gMovieSound.nPlayBlock >= MOVIE_BLOCKS) {
                gMovieSound.nPlayBlock = 0;
            }
        }
        break;
    }
    AudLock_Unlock("Mov_Tick");
}
