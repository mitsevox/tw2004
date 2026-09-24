// LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree): the movie player (the
// intro, the credits, the golfers' bios). The stream loader hands it the movie's MPG2 chunks,
// which wait in a queue (llvideo.h) until the decoder takes them. Partly decompiled.

#include "game_types.h"
#include "ustream.h"
#include "llvideo.h"

int  fn_80075280(Video* pVideo, int* pnQueued);
void fn_800752DC(Video* pVideo);
void fn_8007531C(void* pChunk);
void fn_8007533C(void* pChunk);
void fn_8007535C(VideoQueue* pQueue);
int  fn_8007540C(VideoQueue* pQueue, void* pChunk);
void* fn_80075470(VideoQueue* pQueue);
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
void fn_8007531C(void* pChunk) {
    UStream_AddBufferRef((UStreamBuffer**)pChunk);
}

// A chunk is done with: let go of its stream buffer.
void fn_8007533C(void* pChunk) {
    UStream_ReleaseObjectBuffer((UStreamBuffer**)pChunk);
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
int fn_8007540C(VideoQueue* pQueue, void* pChunk) {
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
void* fn_80075470(VideoQueue* pQueue) {
    void* pChunk;
    if (pQueue->nCount == 0) return NULL;
    pChunk = pQueue->apChunk[pQueue->nTail];
    pQueue->nTail++;
    if (pQueue->nTail == VIDEO_QUEUE_SIZE) {
        pQueue->nTail = 0;
    }
    pQueue->nCount--;
    return pChunk;
}

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281200;
void fn_800757B8(void);
void fn_80007254();
void fn_80075C48(void);
void fn_80075C68(void);
s32 UStream_Close(s32);
s32 UStream_OpenFileByName(s32);
s32 UStream_SetAutoRead(s32);
void fn_80006FE8(void);
u8 fn_80007258();
s32 fn_800755F0(s32);
s32 fn_80075800();
s32 fn_800758B4(s32);
s32 fn_80075904(s32, s32);
s32 fn_800BA734(s32, s32);
s32 fn_80075DEC(s32, s32, s32);
extern u8 lbl_80281EB8;
void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void fn_800757B8(void) {
    *(s32*)((u8*)lbl_80281200) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x4) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x8) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0xC) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x10) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x14) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x18) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x1C) = 0;
}

void fn_80075C48(void) {
    fn_80007254();
}

void fn_80075C68(void) {
    fn_80007254();
}

void fn_80075FB8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_r3_2;
    s32 temp_r3_3;
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
