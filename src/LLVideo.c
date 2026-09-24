// LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree): the movie player (the
// intro, the credits, the golfers' bios). The stream loader hands it the movie's MPG2 chunks,
// which wait in a queue (llvideo.h) until the decoder takes them. Partly decompiled.

#include "game_types.h"
#include "ustream.h"
#include "llvideo.h"
#include "camera.h"
#include "terrain.h"

u8   LLVideo_UpdateStream(Video* pVideo, int* pnQueued);
void LLVideo_PreloadQueue_800752DC(Video* pVideo);
void LLVideo_ChunkAddBufferRef(VideoChunk* pChunk);
void LLVideo_ChunkReleaseBuffer(VideoChunk* pChunk);
void LLVideo_QueueReset(VideoQueue* pQueue);
int  LLVideo_QueueAdd(VideoQueue* pQueue, VideoChunk* pChunk);
VideoChunk* LLVideo_QueueRemove(VideoQueue* pQueue);
int  LLVideo_QueueGetCount(VideoQueue* pQueue);
u8   LLVideo_QueueIsEmpty(VideoQueue* pQueue);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283A50), before the 0.0f, 0.1f and 0.5f fn_800755F0 uses first; its body is unknown.
static f32 LLVideo_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Runs the stream loader once; pnQueued, if given, gets how many chunks are now queued.
u8 LLVideo_UpdateStream(Video* pVideo, int* pnQueued) {
    u8 bRet = UStream_Update();
    if (pnQueued != NULL) {
        *pnQueued = LLVideo_QueueGetCount(&pVideo->queue);
    }
    return bRet;
}

// Runs the stream loader until 16 chunks are queued (before the movie starts).
void LLVideo_PreloadQueue_800752DC(Video* pVideo) {
    int nQueued;
    do {
        LLVideo_UpdateStream(pVideo, &nQueued);
    } while (nQueued < 16);
}

// A queued chunk holds on to its stream buffer.
void LLVideo_ChunkAddBufferRef(VideoChunk* pChunk) {
    UStream_AddBufferRef(&pChunk->pBuffer);
}

// A chunk is done with: let go of its stream buffer.
void LLVideo_ChunkReleaseBuffer(VideoChunk* pChunk) {
    UStream_ReleaseObjectBuffer(&pChunk->pBuffer);
}

// Empties the queue.
void LLVideo_QueueReset(VideoQueue* pQueue) {
    s32 i;
    pQueue->nHead = 0;
    pQueue->nTail = 0;
    pQueue->nCount = 0;
    for (i = 0; i < VIDEO_QUEUE_SIZE; i++) {
        pQueue->apChunk[i] = NULL;
    }
}

// Queues a chunk (its stream buffer stays in use meanwhile); returns the slot it went in.
int LLVideo_QueueAdd(VideoQueue* pQueue, VideoChunk* pChunk) {
    int nSlot = pQueue->nHead;
    pQueue->nHead++;
    if (pQueue->nHead == VIDEO_QUEUE_SIZE) {
        pQueue->nHead = 0;
    }
    pQueue->nCount++;
    pQueue->apChunk[nSlot] = pChunk;
    LLVideo_ChunkAddBufferRef(pChunk);
    return nSlot;
}

// Takes the oldest chunk off the queue (NULL when it is empty).
VideoChunk* LLVideo_QueueRemove(VideoQueue* pQueue) {
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

    apChunk[0] = LLVideo_QueueRemove(&pVideo->queue);
    if (apChunk[0] == NULL) {
        pVideo->bStarved = 1;
        return NULL;
    }
    nSize = apChunk[0]->uSize;
    nChunks = apChunk[0]->nMore + 1;
    for (i = 1; i < nChunks; i++) {
        apChunk[i] = LLVideo_QueueRemove(&pVideo->queue);
        if (apChunk[i] == NULL) {
            pVideo->bStarved = 1;
            for (j = 0; j < i; j++) {
                LLVideo_ChunkReleaseBuffer(apChunk[j]);
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
        LLVideo_ChunkReleaseBuffer(apChunk[i]);
    }
    return pData;
}

void   fn_800757B8(void);
Video* fn_80075800(void);
void   LLVideo_SetFrameRate(Video* pVideo, int nRate);
void   fn_800758B4(Video* pVideo);
Video* fn_80075904(int nSlot, Video* pVideo);
void   LLVideo_Stop_8007599C(Video* pVideo);
void   LLVideo_Start_80075A14(Video* pVideo);
void   LLVideo_SetLastFrameTime_80075A98(Video* pVideo);
void   LLVideo_UpdateAll_80075AD0(void);
u8     LLVideo_IsFrameDue_80075BF4(Video* pVideo);

// GameAudio.c
u8   fn_800A7770(void);
void fn_800A7994(void);
void fn_800A79B4(void);
void fn_800A79D4(void);
void fn_800A79F4(void);
// the renderer
void fn_80008380(void);
void fn_800083A0(void);
void UFont_SetMode_80012898(s32 v);
s32  UFont_GetMode_800128A4(void);
void fn_80012B2C(f32 x0, f32 x1);
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_800137D0(void* pCamera);
void fn_800162A8(void);
void fn_80007254(void);
u8   fn_80007258(void);
void fn_80007260(void);
void VIWaitForRetrace(void);
// the stream loader (UStream.c) and the music (fn_800BA734)
int  UStream_OpenFileByName(const char* pName);
void UStream_SetAutoRead(u8 bAuto);
int  UStream_Stop(void);
int  UStream_Close(int nStream);
void fn_800BA734(int n, s8 nTrack);

void fn_80075C48(void);
void fn_80075C68(void);
void fn_80075C88(void);
void fn_80075D58(void);
int  LLVideo_GetFrame_800760A0(Video* pVideo);
u8   LLVideo_HasEnded_800760A8(Video* pVideo);
void fn_800760B0(int nX, int nY, int nWidth, int nHeight);
void fn_800760D8(LLPict* pPict);
void fn_800760F4(f32* pUV, LLPict* pPict);
void fn_80076128(s32 p0);

// Clears the screen for one frame (two when nFlags bit 0 is clear). With bit 1 it first darkens
// it over 30 frames (black at alpha 0.1 each frame, 0.5 for the last two), then clears.
void fn_800755F0(int nFlags) {
    f32 xy[8];
    f32 colour[4];
    int bFade;
    int bBit0;
    int i;
    int nFrames;

    bFade = nFlags & 2;
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
    colour[3] = bFade ? 0.1f : 0.5f;
    fn_80014194(colour);
    fn_800141F8(xy, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80008380();
    if (bFade) {
        nFrames = 30;
    } else {
        nFrames = (nFlags & 1) ? 2 : 1;
    }
    bBit0 = nFlags & 1;
    for (i = 0; i < nFrames; i++) {
        if (bFade && i >= nFrames - 2) {
            colour[3] = 0.5f;
            fn_80014194(colour);
        }
        fn_800162A8();
        fn_80006EDC();
        fn_800137D0(fn_8001614C());
        // the original tests bBit0 here although both branches make the same call
        if (bBit0) {
            fn_800760B0(0, 0, 512, 448);
        } else {
            fn_800760B0(0, 0, 512, 448);
        }
        fn_80035118(4, 5);
        fn_8001425C(0);
        fn_80014118(0x40);
        fn_80012F50(0, 6, 0x80);
        fn_80012F18(7);
        fn_80012EF8();
        fn_8001644C(0xA1, xy, 0, NULL, 2);
        fn_80013400();
        fn_80006FE8();
        fn_80008380();
        if (!bBit0) {
            fn_80007254();
        }
        if (bBit0) {
            fn_80007260();
        }
        fn_800083A0();
    }
    if (bFade) {
        fn_800755F0(bBit0);
    }
}

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
    LLVideo_SetFrameRate(pVideo, 33);
    return pVideo;
}

// Sets the frame rate.
void LLVideo_SetFrameRate(Video* pVideo, int nRate) {
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
void LLVideo_HandleChunk(VideoChunk* pChunk) {
    Video* pVideo = lbl_80281200->apVideo[pChunk->nSlot];
    if (pVideo != NULL && pVideo->b1020) {
        LLVideo_QueueAdd(&pVideo->queue, pChunk);
    }
    LLVideo_ChunkReleaseBuffer(pChunk);
}

// Stops a running movie and gives back every chunk it still holds.
void LLVideo_Stop_8007599C(Video* pVideo) {
    if (pVideo->b1020) {
        fn_800A79B4();
        pVideo->b1020 = 0;
        if (pVideo->p1018 != NULL) {
            LLVideo_ChunkReleaseBuffer(pVideo->p1018);
            pVideo->p1018 = NULL;
        }
        while (!LLVideo_QueueIsEmpty(&pVideo->queue)) {
            LLVideo_ChunkReleaseBuffer(LLVideo_QueueRemove(&pVideo->queue));
        }
    }
}

// Starts a movie: waits for the sound side, then reads ahead until 16 chunks are queued.
void LLVideo_Start_80075A14(Video* pVideo) {
    do {
        fn_800A4BDC();
    } while (fn_800A7770());
    fn_800A7994();
    LLVideo_QueueReset(&pVideo->queue);
    pVideo->p1018 = NULL;
    pVideo->b1020 = 1;
    pVideo->b1021 = 0;
    pVideo->nFrame = -1;
    pVideo->bEnded = 0;
    pVideo->bStarved = 0;
    fn_8002FF94(&pVideo->pict, &pVideo->stream);
    LLVideo_SetLastFrameTime_80075A98(pVideo);
    LLVideo_PreloadQueue_800752DC(pVideo);
}

// The next frame is timed from now.
void LLVideo_SetLastFrameTime_80075A98(Video* pVideo) {
    pVideo->tLast = fn_800954A4(0);
}

// Runs every movie once: each one whose next frame is due decodes it into its picture; one whose
// data ran out is stopped.
void LLVideo_UpdateAll_80075AD0(void) {
    Video* pVideo;
    s32 i;
    fn_800A79F4();
    for (i = 0; i < NUM_VIDEO_SLOTS; i++) {
        pVideo = lbl_80281200->apVideo[i];
        if (pVideo != NULL && pVideo->b1020 && !pVideo->b1021 && LLVideo_IsFrameDue_80075BF4(pVideo)) {
            LLVideo_SetLastFrameTime_80075A98(pVideo);
            if (pVideo->bStarved || fn_8003001C(&pVideo->pict, &pVideo->stream)) {
                pVideo->bEnded = 1;
                LLVideo_Stop_8007599C(pVideo);
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
u8 LLVideo_IsFrameDue_80075BF4(Video* pVideo) {
    if (fn_8006E118(fn_800954A4(0), pVideo->tLast) < pVideo->fFrameTime) {
        return 0;
    }
    return 1;
}

void fn_80075C48(void) {
    fn_80007254();
}

void fn_80075C68(void) {
    fn_80007254();
}

// Sets the renderer up for full-screen movie frames (512 x 448); fn_80075D58 puts it back.
void fn_80075C88(void) {
    fn_80008380();
    fn_800140E8(0, 512, 448, 0, 8, 1);
    fn_80014118(0x10);
    fn_80012F34(0);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    fn_800760B0(0, 0, 512, 448);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80014194(NULL);
    lbl_80281200->n20 = UFont_GetMode_800128A4();
    UFont_SetMode_80012898(1);
    fn_80012B2C(1.0f, 1.0f);
    fn_80076128(10);
}

// Puts the renderer back after a movie.
void fn_80075D58(void) {
    fn_80008380();
    fn_80006EDC();
    fn_80012B2C(1.0f, 1.0f);
    UFont_SetMode_80012898(lbl_80281200->n20);
    fn_80012F34(1);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(3);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80012EF8();
    fn_80006FE8();
    fn_800083A0();
    fn_80008380();
}

// Plays a movie until its data runs out, showing each new frame full screen, at most 33 times a
// second. pfnStop(pVideo, nArg), if given, is asked every frame whether to stop early.
void LLVideo_RunPlayback_80075DEC(Video* pVideo, u8 (*pfnStop)(Video* pVideo, int nArg), int nArg) {
    f32 xy[8];
    f32 uv[8];
    u64 tFrame;
    u8 bFirst;
    u8 bDone;
    int nLastFrame;

    fn_80075C48();
    fn_80075C88();
    LLVideo_Start_80075A14(pVideo);
    tFrame = fn_800954A4(0);
    nLastFrame = -1;
    bFirst = 1;
    bDone = 0;
    do {
        if (!LLVideo_UpdateStream(pVideo, NULL)) {
            bDone = 1;
        } else {
            LLVideo_UpdateAll_80075AD0();
            if (pfnStop != NULL && pfnStop(pVideo, nArg)) {
                UStream_Stop();
                LLVideo_Stop_8007599C(pVideo);
            }
            if (LLVideo_HasEnded_800760A8(pVideo)) {
                UStream_Stop();
                LLVideo_Stop_8007599C(pVideo);
            }
            if (nLastFrame == LLVideo_GetFrame_800760A0(pVideo)) {
                continue;
            }
            nLastFrame = LLVideo_GetFrame_800760A0(pVideo);
            fn_80006EDC();
            fn_800162A8();
            fn_800760B0(0, 0, 512, 448);
            fn_80012EF8();
            if (bFirst) {
                fn_800141F8(xy, NULL, 0.0f, 0.0f, 1.0f, 1.0f);
                fn_800760F4(uv, &pVideo->pict);
                bFirst = 0;
            }
        }
        fn_800760D8(&pVideo->pict);
        fn_80012EF8();
        fn_8001644C(0xA1, xy, 0, uv, 2);
        while (fn_8006E118(fn_800954A4(0), tFrame) < 1.0f / 33.0f) {
        }
        fn_80006FE8();
        VIWaitForRetrace();
        tFrame = fn_800954A4(0);
        fn_800083A0();
        fn_80008380();
    } while (!bDone);
    LLVideo_Stop_8007599C(pVideo);
    fn_800755F0(6);
    fn_80075D58();
    fn_80075C68();
}

// Plays the movie file pName in slot 0 (LLVideo_RunPlayback_80075DEC), after clearing the screen (fn_800755F0).
void LLVideo_PlayFile(const char* pName, u8 (*pfnStop)(Video* pVideo, int nArg), int nArg, int nFlags) {
    int nStream;
    Video* pVideo;

    lbl_80281EB8 = fn_80007258();
    if (lbl_80281EB8) {
        fn_80006FE8();
    }
    fn_800BA734(0, 0);
    fn_800755F0(nFlags | 1);
    nStream = UStream_OpenFileByName(pName);
    if (nStream != -1) {
        UStream_SetAutoRead(1);
        pVideo = fn_80075800();
        fn_80075904(0, pVideo);
        LLVideo_RunPlayback_80075DEC(pVideo, pfnStop, nArg);
        UStream_SetAutoRead(0);
        fn_800758B4(pVideo);
        UStream_Close(nStream);
    }
}

// How many chunks are queued.
int LLVideo_QueueGetCount(VideoQueue* pQueue) {
    return pQueue->nCount;
}

// The queue is empty.
u8 LLVideo_QueueIsEmpty(VideoQueue* pQueue) {
    return pQueue->nCount == 0;
}

// The frames decoded so far (-1 before the first).
int LLVideo_GetFrame_800760A0(Video* pVideo) {
    return pVideo->nFrame;
}

// The decoder has run out.
u8 LLVideo_HasEnded_800760A8(Video* pVideo) {
    return pVideo->bEnded;
}

// Sets the renderer's rectangle (x, y, width, height).
void fn_800760B0(int nX, int nY, int nWidth, int nHeight) {
    lbl_801B8980.nBC = nX;
    lbl_801B8980.nC4 = nY;
    lbl_801B8980.nC0 = nWidth;
    lbl_801B8980.nC8 = nHeight;
    lbl_801B8980.u110 |= 0x200;
}

// The next draw uses this picture.
void fn_800760D8(LLPict* pPict) {
    lbl_801B8980.pPict10C = pPict;
    lbl_801B8980.uFlags |= 4;
}

// Fills the texture coordinates for drawing a picture (fn_8001644C).
void fn_800760F4(f32* pUV, LLPict* pPict) {
    pUV[0] = 0.0f;
    pUV[1] = 0.0f;
    pUV[2] = 0.0f;
    pUV[3] = 1.0f;
    pUV[4] = pPict->f6C;
    pUV[5] = pPict->f70;
    pUV[6] = 0.0f;
    pUV[7] = 1.0f;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80076128(s32 p0) {
    UFontContext* pCtx;
    pCtx = UFont_GetContext();
    pCtx->nA4 = p0;
}

// ---- end of sweep code ----
