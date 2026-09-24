// llvideo.h (our name): the movie player's types (LLVideo.c, EA's name from its asserts). The
// movies are MPG2 chunks inside a stream file; UStream.c hands each chunk to the player, which
// queues it until the decoder takes it.

#ifndef LLVIDEO_H
#define LLVIDEO_H

#include "engine.h"
#include "ustream.h"
#include "llpict.h"

#define VIDEO_QUEUE_SIZE 1024

// An MPG2 chunk as UStream.c hands it to the movie player (LLVideo_HandleChunk): the chunk's header in
// the stream buffer, its tag word replaced by that buffer. A piece of the movie spans nMore + 1
// chunks in a row.
typedef struct VideoChunk {
    UStreamBuffer* pBuffer;             // 0x00  the buffer it sits in (in use while it is queued)
    u32  uLength;                       // 0x04  header included
    u8   unk8[4];
    int  nSlot;                         // 0x0C  the slot of the movie it belongs to
    u8   unk10[4];
    u32  uSize;                         // 0x14  the data's size
    u8   unk18;
    u8   nMore;                         // 0x19  how many more chunks the piece takes
    u8   unk1A[0x40 - 0x1A];
    u8   aData[4];                      // 0x40  the data (uSize bytes)
} VideoChunk;

// The chunks waiting to be decoded, a ring.
typedef struct VideoQueue {
    VideoChunk* apChunk[VIDEO_QUEUE_SIZE];  // 0x0000
    int   nHead;                        // 0x1000  where the next chunk goes
    int   nTail;                        // 0x1004  the oldest chunk
    int   nCount;                       // 0x1008  chunks queued
} VideoQueue;
LAYOUT_ASSERT(VideoQueue, 0x100C);

// A movie being played (0x10B0 bytes; LLVideo_HandleChunk finds it by the chunk's stream number). Its
// decoder reads the queued chunks through fn_800754C0 and decodes into pict.
typedef struct Video {
    PictStream stream;                  // 0x000  the decoder and its current frame (LLPict_Gc.c)
    VideoQueue queue;                   // 0x008
    u8         unk1014[4];
    VideoChunk* p1018;                 // 0x1018 a chunk, given back when the movie stops
    int        nSlot;                   // 0x101C its slot in lbl_80281200, -1 when it has none
    u8         b1020;                   // 0x1020 set while the movie runs: chunks are queued
    u8         b1021;                   // 0x1021 set: fn_80075AD0_UpdateAll skips the movie
    u8         bEnded;                  // 0x1022 the decoder ran out (fn_800760A8_HasEnded)
    u8         bStarved;                // 0x1023 the queue ran dry while reading
    LLPict     pict;                    // 0x1024 the picture the frames are copied into
    u64        tLast;                   // 0x1098 when the last frame was due (TI_sReadCounter(0))
    f32        fFrameTime;              // 0x10A0 seconds per frame: 1.0f / the frame rate
    int        nFrame;                  // 0x10A4 frames decoded; -1 before the first
    u8         bFirstFrame;             // 0x10A8 set once the first frame has been decoded
    u8         pad10A9[7];
} Video;
LAYOUT_ASSERT(Video, 0x10B0);

// The movies being played (lbl_801D7120, 0x28 bytes, reached through lbl_80281200).
#define NUM_VIDEO_SLOTS 8
typedef struct VideoSlots {
    Video* apVideo[NUM_VIDEO_SLOTS];    // 0x00  fn_80075904 puts a movie in a slot
    int    n20;                         // 0x20  FO_eGetCurrentAddMode()'s value while a movie shows
                                        //       (fn_80075C88 saves it, fn_80075D58 puts it back)
    u8     pad24[4];
} VideoSlots;
LAYOUT_ASSERT(VideoSlots, 0x28);
extern VideoSlots* lbl_80281200;
extern u8 lbl_80281EB8;                 // fn_80007258()'s value when the last movie started

// Play the movie file pName; pfnStop(pVideo, nArg) nonzero stops it early.
void LLVideo_PlayFile(const char* pName, u8 (*pfnStop)(Video* pVideo, int nArg), int nArg, int nFlags);

#endif
