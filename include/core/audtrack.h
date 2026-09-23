// audtrack.h (our name): the sound engine's playing tracks ("perfs" in TW06: Trk_AllocPerf,
// ResetStreamPerf), the voices they play on, and the streamed tracks' disc reads
// (hlaudtrackstm.c). Only the fields the matched code proves are named; the rest is padding.

#ifndef CORE_AUDTRACK_H
#define CORE_AUDTRACK_H

#include "core/audcontainers.h"

// A voice of the sound engine (hlaudvoice.c's), wrapping one of startUp.c's hardware voices.
typedef struct AudVoice {
    u8   unk0[0x8];
    u16  nHwVoice;              // 0x8    the startUp.c voice it plays on
    u8   bHalf : 1;             // 0xA    which half of its ARAM buffer the next stream block fills
    u8   unkA : 7;
    u8   unkB_7 : 1;            // 0xB
    u8   bB_6 : 1;              //        paused; Stm_Tick resumes it once the drive is fine
    u8   unkB : 6;
    u8   unkC[0x28 - 0xC];
    u32  uAram;                 // 0x28   its ARAM buffer (two halves of 0x7F00 bytes)
    u32  uPlayPos;              // 0x2C   where it is playing in that buffer, in bytes
} AudVoice;

// What fn_800AC4A0 is asked for when a track takes a voice.
typedef struct AudVoiceRequest {
    s16  nPriority;             // 0x0
    s32  n4;                    // 0x4
    union {
        struct {
            u16 unk15 : 1;
            u16 b14 : 1;
            u16 unk13 : 1;
            u16 b12 : 1;
            u16 b11 : 1;        //        bit 2 of the play list's number
            u16 unk0 : 11;
        } b;
        u16 n;                  //        cleared as a whole first
    } flags;                    // 0x8
    void (*pfnCallback)(void);  // 0xC
    void* pUser;                // 0x10   the track
    s32  nIndex;                // 0x14   the track's channel
} AudVoiceRequest;

// One stream of a play list: where it starts in the stream file and where it loops back to.
typedef struct AudStream {
    u32  uOffset;               // 0x0
    u32  uLoop;                 // 0x4    0xFFFFFFFF: it does not loop
} AudStream;

// A play list of streams (fn_800A9564 finds one by its number).
typedef struct AudPlayList {
    u16  nStreams;              // 0x0
    u8   nId;                   // 0x2    the play list's number (bit 2 goes into the voice request)
    u8   n3;                    // 0x3    its volume curve (fn_800AA44C)
    u16  n4;                    // 0x4
    u8   nChannels;             // 0x6    one voice each
} AudPlayList;

// A streamed track's flags (AudTrack 0x89), cleared as a byte.
typedef union AudTrackStmFlags {
    struct {
        u8 bStarved : 1;        // its voices are paused because the reads fell behind
        u8 b6 : 1;              // when the buffer is full: hold (state 5) instead of playing
        u8 unk5 : 1;
        u8 bEnded : 1;          // the stream ended and does not loop
        u8 b3 : 1;              // a refill was queued after the stream ended
        u8 unk0 : 3;
    } b;
    u8 n;
} AudTrackStmFlags;

// The template a track plays.
typedef struct AudTrackTmpl {
    u8   n0;                    // 0x0    bit 0x08: a streamed track
    u8   unk1;
    u8   n2;                    // 0x2    its play list's channel count
    u8   unk3[0x14 - 0x3];
    AudPlayList* pPlayList;     // 0x14   streamed tracks only
} AudTrackTmpl;

// A playing track. The stream fields (0x64-0x8A) are cleared by fn_800ABC54.
typedef struct AudTrack {
    u8   unk0[0x8];
    AudTrackTmpl* pTmpl;        // 0x8
    AudVoice* apVoices[12];     // 0xC    one per channel; 12 is a guess (0x3E is the next field in use)
    u8   unk3C[0x44 - 0x3C];
    f32  f44;                   // 0x44   its volume
    u8   unk48[0x58 - 0x48];
    s32  nState;                // 0x58   2 starting, 3 and 6 playing, 4 filling, 5 filled
    u8   b5C : 1;               // 0x5C
    u8   unk5C : 7;
    u8   n5D;                   // 0x5D
    u8   unk5E[0x64 - 0x5E];
    AudStream* pStream;         // 0x64   the stream playing
    u8*  pBuffer;               // 0x68   the read buffer (0x8000 bytes per channel and half)
    u32  uBufferSize;           // 0x6C
    u32  uReadPos;              // 0x70   the next read's position in the stream
    u32  uRead;                 // 0x74   bytes read in all
    u32  uFilled;               // 0x78   bytes sent to ARAM
    u32  uPlayed;               // 0x7C   bytes played
    u32  uLength;               // 0x80   the stream's length
    u16  nLastStream;           // 0x84   the stream picked at random last time
    u16  nNextStream;           // 0x86   0xFFFF: none waiting
    u8   nNextPlayList;         // 0x88   0xFF: none waiting
    AudTrackStmFlags flags;     // 0x89
    u8   nReadId;               // 0x8A   tells this track's reads from an older start's
} AudTrack;

// A disc read waiting in the stream read queue (lbl_801F18B8). bRestart marks a request to refill
// the whole buffer (fn_800AB860) instead of a read.
typedef struct AudStreamRead {
    s32  hFile;                 // 0x0
    u8*  pDst;                  // 0x4
    u32  uLen;                  // 0x8
    u32  uOffset;               // 0xC
    void (*pfnDone)(int nBytes, int nError, AudTrack* pTrack, u8 nId);  // 0x10
    AudTrack* pTrack;           // 0x14
    u8   nId;                   // 0x18
    u8   n19;                   // 0x19
    u8   bRestart;              // 0x1A
} AudStreamRead;
LAYOUT_ASSERT(AudStreamRead, 0x1C);

// The stream read queue and its buffer.
typedef struct AudStreamQueue {
    UQueue        queue;        // 0x0
    u8            bBusy;        // 0x18   a read is under way
    AudStreamRead aReads[8];    // 0x1C
} AudStreamQueue;
LAYOUT_ASSERT(AudStreamQueue, 0xFC);

extern AudStreamQueue lbl_801F18B8;
extern s32 lbl_80281468;                // the stream file (hlaudmovie.c opens "/AudioStm_GC.sab")
extern u8 lbl_802820A8;                 // the last read id handed out (hlaudtrackstm.c)
extern AudTrack* lbl_802820AC;          // the track whose block is being DMA'd (hlaudtrackstm.c)

#endif
