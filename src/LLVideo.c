// LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree): the movie player (the
// intro, the credits, the golfers' bios). The stream loader hands it the movie's MPG2 chunks,
// which wait in a queue (llvideo.h) until the decoder takes them. Partly decompiled.

#include "game_types.h"
#include "ustream.h"
#include "llvideo.h"

int  fn_80075280(Video* pVideo, int* pnQueued);
void fn_800752DC(Video* pVideo);
void fn_8007531C(VideoChunk* pChunk);
void fn_8007533C(VideoChunk* pChunk);
void fn_8007535C(VideoQueue* pQueue);
int  fn_8007540C(VideoQueue* pQueue, VideoChunk* pChunk);
VideoChunk* fn_80075470(VideoQueue* pQueue);
int  fn_80076088(VideoQueue* pQueue);
u8   fn_80076090(VideoQueue* pQueue);

// Runs the stream loader once; pnQueued, if given, gets how many chunks are now queued.
int fn_80075280(Video* pVideo, int* pnQueued) {
    int nRet = UStream_Update();
    if (pnQueued != NULL) {
        *pnQueued = fn_80076088(&pVideo->queue);
    }
    return nRet;
}

// Runs the stream loader until 16 chunks are queued (before the movie starts).
void fn_800752DC(Video* pVideo) {
    int nQueued;
    do {
        fn_80075280(pVideo, &nQueued);
    } while (nQueued < 16);
}

// A queued chunk holds on to its stream buffer.
void fn_8007531C(VideoChunk* pChunk) {
    UStream_AddBufferRef(&pChunk->pBuffer);
}

// A chunk is done with: let go of its stream buffer.
void fn_8007533C(VideoChunk* pChunk) {
    UStream_ReleaseObjectBuffer(&pChunk->pBuffer);
}

// Empties the queue.
void fn_8007535C(VideoQueue* pQueue) {
    s32 i;
    pQueue->nHead = 0;
    pQueue->nTail = 0;
    pQueue->nCount = 0;
    for (i = 0; i < VIDEO_QUEUE_SIZE; i++) {
        pQueue->apChunk[i] = NULL;
    }
}

// Queues a chunk (its stream buffer stays in use meanwhile); returns the slot it went in.
int fn_8007540C(VideoQueue* pQueue, VideoChunk* pChunk) {
    int nSlot = pQueue->nHead;
    pQueue->nHead++;
    if (pQueue->nHead == VIDEO_QUEUE_SIZE) {
        pQueue->nHead = 0;
    }
    pQueue->nCount++;
    pQueue->apChunk[nSlot] = pChunk;
    fn_8007531C(pChunk);
    return nSlot;
}

// Takes the oldest chunk off the queue (NULL when it is empty).
VideoChunk* fn_80075470(VideoQueue* pQueue) {
    VideoChunk* pChunk;
    if (pQueue->nCount == 0) return NULL;
    pChunk = pQueue->apChunk[pQueue->nTail];
    pQueue->nTail++;
    if (pQueue->nTail == VIDEO_QUEUE_SIZE) {
        pQueue->nTail = 0;
    }
    pQueue->nCount--;
    return pChunk;
}

// The decoder's read call (fn_8002FEB0): takes the movie's next piece off the queue and returns
// it copied into one new buffer. When the queue runs dry, the chunks taken so far are given back,
// bStarved is set and NULL is returned.
void* fn_800754C0(void* pArg) {
    Video* pVideo = pArg;
    VideoChunk* apChunk[32];            // size unknown: the stack frame has room for 33
    u8* pData;
    int i;
    u8* pDst;
    int nSize;
    int nChunks;
    int j;

    apChunk[0] = fn_80075470(&pVideo->queue);
    if (apChunk[0] == NULL) {
        pVideo->bStarved = 1;
        return NULL;
    }
    nSize = apChunk[0]->uSize;
    nChunks = apChunk[0]->nMore + 1;
    for (i = 1; i < nChunks; i++) {
        apChunk[i] = fn_80075470(&pVideo->queue);
        if (apChunk[i] == NULL) {
            pVideo->bStarved = 1;
            for (j = 0; j < i; j++) {
                fn_8007533C(apChunk[j]);
            }
            return NULL;
        }
        nSize += apChunk[i]->uSize;
    }
    pData = fn_80009B34(nSize, 1, 0x20, "LLVideo.c", 0x3C4);
    pDst = pData;
    for (i = 0; i < nChunks; i++) {
        memcpy(pDst, apChunk[i]->aData, apChunk[i]->uSize);
        pDst += apChunk[i]->uSize;
        fn_8007533C(apChunk[i]);
    }
    return pData;
}

void   fn_800757B8(void);
Video* fn_80075800(void);
void   fn_80075880(Video* pVideo, int nRate);
void   fn_800758B4(Video* pVideo);
Video* fn_80075904(int nSlot, Video* pVideo);
void   fn_8007599C(Video* pVideo);
void   fn_80075A14(Video* pVideo);
void   fn_80075A98(Video* pVideo);
void   fn_80075AD0(void);
u8     fn_80075BF4(Video* pVideo);

// GameAudio.c
void fn_800A4BDC(void);
u8   fn_800A7770(void);
void fn_800A7994(void);
void fn_800A79B4(void);
void fn_800A79D4(void);
void fn_800A79F4(void);
// GameManager.c
f32  fn_8006E118(u64 tEnd, u64 tStart);    // seconds between two time stamps

// Empties every slot.
void fn_800757B8(void) {
    s32 i;
    for (i = 0; i < NUM_VIDEO_SLOTS; i++) {
        lbl_80281200->apVideo[i] = NULL;
    }
}

// Makes a movie (not yet in a slot or running), at 33 frames a second.
Video* fn_80075800(void) {
    Video* pVideo = fn_80009B34(sizeof(Video), 2, 0x40, "LLVideo.c", 0x5C1);
    fn_8002FEB0(&pVideo->pict, &pVideo->stream, fn_800754C0, pVideo);
    pVideo->nSlot = -1;
    pVideo->b1020 = 0;
    pVideo->bFirstFrame = 0;
    fn_80075880(pVideo, 33);
    return pVideo;
}

// Sets the frame rate.
void fn_80075880(Video* pVideo, int nRate) {
    pVideo->fFrameTime = 1.0f / nRate;
}

// Frees a movie, taking it out of its slot first.
void fn_800758B4(Video* pVideo) {
    if (pVideo->nSlot != -1) {
        fn_80075904(pVideo->nSlot, NULL);
    }
    fn_8002FF38(&pVideo->pict, &pVideo->stream);
    fn_80009E70(pVideo);
}

// Puts a movie (or NULL) in a slot; returns the movie that was there.
Video* fn_80075904(int nSlot, Video* pVideo) {
    Video* pOld = lbl_80281200->apVideo[nSlot];
    if (pOld != NULL) {
        pOld->nSlot = -1;
    }
    lbl_80281200->apVideo[nSlot] = pVideo;
    if (pVideo != NULL) {
        pVideo->nSlot = nSlot;
    }
    return pOld;
}

// UStream.c hands over an MPG2 chunk: it is queued if its movie is running, else given back.
void fn_8007593C(VideoChunk* pChunk) {
    Video* pVideo = lbl_80281200->apVideo[pChunk->nSlot];
    if (pVideo != NULL && pVideo->b1020) {
        fn_8007540C(&pVideo->queue, pChunk);
    }
    fn_8007533C(pChunk);
}

// Stops a running movie and gives back every chunk it still holds.
void fn_8007599C(Video* pVideo) {
    if (pVideo->b1020) {
        fn_800A79B4();
        pVideo->b1020 = 0;
        if (pVideo->p1018 != NULL) {
            fn_8007533C(pVideo->p1018);
            pVideo->p1018 = NULL;
        }
        while (!fn_80076090(&pVideo->queue)) {
            fn_8007533C(fn_80075470(&pVideo->queue));
        }
    }
}

// Starts a movie: waits for the sound side, then reads ahead until 16 chunks are queued.
void fn_80075A14(Video* pVideo) {
    do {
        fn_800A4BDC();
    } while (fn_800A7770());
    fn_800A7994();
    fn_8007535C(&pVideo->queue);
    pVideo->p1018 = NULL;
    pVideo->b1020 = 1;
    pVideo->b1021 = 0;
    pVideo->nFrame = -1;
    pVideo->bEnded = 0;
    pVideo->bStarved = 0;
    fn_8002FF94(&pVideo->pict, &pVideo->stream);
    fn_80075A98(pVideo);
    fn_800752DC(pVideo);
}

// The next frame is timed from now.
void fn_80075A98(Video* pVideo) {
    pVideo->tLast = fn_800954A4(0);
}

// Runs every movie once: each one whose next frame is due decodes it into its picture; one whose
// data ran out is stopped.
void fn_80075AD0(void) {
    Video* pVideo;
    s32 i;
    fn_800A79F4();
    for (i = 0; i < NUM_VIDEO_SLOTS; i++) {
        pVideo = lbl_80281200->apVideo[i];
        if (pVideo != NULL && pVideo->b1020 && !pVideo->b1021 && fn_80075BF4(pVideo)) {
            fn_80075A98(pVideo);
            if (pVideo->bStarved || fn_8003001C(&pVideo->pict, &pVideo->stream)) {
                pVideo->bEnded = 1;
                fn_8007599C(pVideo);
            } else if (fn_80030040(&pVideo->pict, &pVideo->stream)) {
                pVideo->nFrame++;
                if (!pVideo->bFirstFrame) {
                    fn_8002FF98(&pVideo->pict, &pVideo->stream);
                    pVideo->bFirstFrame = 1;
                    fn_8003009C(&pVideo->pict, &pVideo->stream, 0);
                    fn_800A79D4();
                }
                fn_800300A0(&pVideo->pict, &pVideo->stream);
            }
        }
    }
}

// The movie's next frame is due.
u8 fn_80075BF4(Video* pVideo) {
    if (fn_8006E118(fn_800954A4(0), pVideo->tLast) < pVideo->fFrameTime) {
        return 0;
    }
    return 1;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80007254();
void fn_80075C48(void);
void fn_80075C68(void);
s32 UStream_Close(s32);
s32 UStream_OpenFileByName(s32);
s32 UStream_SetAutoRead(s32);
void fn_80006FE8(void);
u8 fn_80007258();
s32 fn_800755F0(s32);
s32 fn_800BA734(s32, s32);
s32 fn_80075DEC(Video*, s32, s32);
extern u8 lbl_80281EB8;
void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void fn_80075C48(void) {
    fn_80007254();
}

void fn_80075C68(void) {
    fn_80007254();
}

void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_r3_2;
    Video* temp_r3_3;
    u8 temp_r3;

    temp_r3 = fn_80007258();
    lbl_80281EB8 = temp_r3;
    if (temp_r3 != 0) {
        fn_80006FE8();
    }
    fn_800BA734(0, 0);
    fn_800755F0(arg3 | 1);
    temp_r3_2 = UStream_OpenFileByName(arg0);
    if (temp_r3_2 != -1) {
        UStream_SetAutoRead(1);
        temp_r3_3 = fn_80075800();
        fn_80075904(0, temp_r3_3);
        fn_80075DEC(temp_r3_3, arg1, arg2);
        UStream_SetAutoRead(0);
        fn_800758B4(temp_r3_3);
        UStream_Close(temp_r3_2);
    }
}

// ---- end of sweep code ----

// How many chunks are queued.
int fn_80076088(VideoQueue* pQueue) {
    return pQueue->nCount;
}

// The queue is empty.
u8 fn_80076090(VideoQueue* pQueue) {
    return pQueue->nCount == 0;
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_800760A0(u8* p);
u8 fn_800760A8(u8* p);
s32 fn_80012EC4();
void fn_80076128(s32 p0);

s32 fn_800760A0(u8* p) {
    return *(s32*)(p + 0x10A4);
}

u8 fn_800760A8(u8* p) {
    return *(u8*)(p + 0x1022);
}

void fn_80076128(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(s32*)(((u8*)t0) + 0xA4) = p0;
}

// ---- end of sweep code ----
