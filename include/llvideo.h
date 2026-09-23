// llvideo.h (our name): the movie player's types (LLVideo.c, EA's name from its asserts). The
// movies are MPG2 chunks inside a stream file; UStream.c hands each chunk to the player, which
// queues it until the decoder takes it.

#ifndef LLVIDEO_H
#define LLVIDEO_H

#include "engine.h"

#define VIDEO_QUEUE_SIZE 1024

// The chunks waiting to be decoded, a ring. Each chunk's first word points at the stream buffer
// it sits in, which stays in use (referenced) while the chunk is queued.
typedef struct VideoQueue {
    void* apChunk[VIDEO_QUEUE_SIZE];    // 0x0000
    int   nHead;                        // 0x1000  where the next chunk goes
    int   nTail;                        // 0x1004  the oldest chunk
    int   nCount;                       // 0x1008  chunks queued
} VideoQueue;
LAYOUT_ASSERT(VideoQueue, 0x100C);

// A movie being played (fn_8007593C finds it by the chunk's stream number); only the fields read
// so far.
typedef struct Video {
    u8         unk0[8];
    VideoQueue queue;                   // 0x008
} Video;

#endif
