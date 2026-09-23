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
    u8   unkA_6 : 2;
    u8   bA_4 : 1;              //        it owns uAram, given back when it stops (fn_800ACB28)
    u8   unkA_3 : 2;
    u8   bA_1 : 1;              //        fn_800AA5A0 keeps the channel's event when it is set
    u8   unkA_0 : 1;
    u8   unkB_7 : 1;            // 0xB
    u8   bB_6 : 1;              //        paused; Stm_Tick resumes it once the drive is fine
    u8   unkB : 6;
    u8   unkC[0x10 - 0xC];
    s32  n10;                   // 0x10   the request's n4: a voice with a lower one can be stolen
    u8   unk14[0x18 - 0x14];
    struct AudSeqTone* pTone;   // 0x18   the tone a sequenced track plays on it
    void (*pfnCallback)(struct AudVoice* pVoice, int nReason);  // 0x1C   the request's
    void* pUser;                // 0x20   the request's (the track)
    s32  nIndex;                // 0x24   the request's (the track's channel)
    u32  uAram;                // 0x28   its ARAM buffer (two halves of 0x7F00 bytes)
    u32  uPlayPos;              // 0x2C   where it is playing in that buffer, in bytes
} AudVoice;

// What fn_800AC4A0 is asked for when a track takes a voice.
typedef struct AudVoiceRequest {
    s16  nPriority;             // 0x0    a sequenced note asks with its volume
    u8   n2;                    // 0x2
    u8   n3;                    // 0x3
    s32  n4;                    // 0x4    how hard the voice is to steal (becomes AudVoice n10)
    union {
        struct {
            u16 unk15 : 1;
            u16 b14 : 1;
            u16 unk13 : 1;
            u16 b12 : 1;
            u16 b11 : 1;        //        bit 2 of the play list's number
            u16 unk10 : 1;
            u16 b9 : 1;         //        the tone loops
            u16 unk0 : 9;
        } b;
        u16 n;                  //        cleared as a whole first
    } flags;                    // 0x8
    void (*pfnCallback)(struct AudVoice* pVoice, int nReason);  // 0xC   called when the voice ends
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

// A tone of a sequencer bank (0x14 bytes).
typedef struct AudSeqTone {
    u8   unk0[0x10];
    s32  n10;                   // 0x10   bit 0: it loops
} AudSeqTone;
LAYOUT_ASSERT(AudSeqTone, 0x14);

// The tones a sequenced track plays (fn_800AB384 picks one).
typedef struct AudSeqBank {
    u8   unk0[0x3];
    u8   n3;                    // 0x3    its volume curve, as in AudPlayList
    AudSeqTone aTones[1];       // 0x4    the count is not known
} AudSeqBank;

// A sequencer event (8 bytes). Its handler is lbl_801F1880[nType].
typedef struct AudSeqEvent {
    u16  n0;                    // 0x0    ticks to wait before it runs (scaled by fn_800AB1B8)
    u8   nType;                 // 0x2
    u8   n3;                    // 0x3
    s32  n4;                    // 0x4
} AudSeqEvent;
LAYOUT_ASSERT(AudSeqEvent, 0x8);

// The template a track plays (0x1C bytes, in its source template's aTracks).
typedef struct AudTrackTmpl {
    u8   n0;                    // 0x0    0x01: events run one at a time (n67); 0x08: streamed;
                                //        0x40/0x80: fn_800A8584 on free/on a variation change
    u8   n1;                    // 0x1    how the next variation is picked (fn_800AA4BC); 0: never
    u8   n2;                    // 0x2    its channel count (one voice each)
    u8   n3;                    // 0x3    events per variation
    u16  n4;                    // 0x4    0 or 1: a single variation
    u8   n6;                    // 0x6    passed to fn_800A8584
    u8   n7;                    // 0x7    variations per set
    u8   n8;                    // 0x8    sets (the track's n68 picks one)
    u8   n9;                    // 0x9    the variation picked last
    u8   nA;                    // 0xA    0xFF, or the set to use
    u8   unkB;
    f32  fC;                    // 0xC    copied to the track's f40
    u8   unk10[0x14 - 0x10];
    union {
        AudPlayList* pPlayList; // 0x14   streamed tracks; its n3 is the track's volume curve
        AudSeqBank* pBank;      //        sequenced tracks; the same n3
    } data;
    AudSeqEvent* pEvents;       // 0x18   [n8][n7][n3]
} AudTrackTmpl;
LAYOUT_ASSERT(AudTrackTmpl, 0x1C);

// The voice settings a sequenced track's events change (AudTrack 0x30), handed to fn_800AC6D0
// with each note and cleared once it is played.
typedef struct AudVoiceParams {
    f32  f0;                    // 0x0    from an event (n4 / 65536)
    u8   unk4[0x8 - 0x4];
    u8   a8[2];                 // 0x8    from an event, by its n3
    u8   unkA[0xC - 0xA];
    u16  nC;                    // 0xC
    union {
        struct {
            u8 unk7 : 1;
            u8 bF0 : 1;         // f0 was set
            u8 b5 : 1;          // a8 was last set at index 0
            u8 b4 : 1;          // a8 was last set at index 1
            u8 bC : 1;          // nC was set
            u8 unk0 : 3;
        } b;
        u8 n;
    } flags;                    // 0xE
    u8   unkF;
} AudVoiceParams;
LAYOUT_ASSERT(AudVoiceParams, 0x10);

// A streamed track's own fields (AudTrack 0x64), cleared by fn_800ABC54.
typedef struct AudTrackStm {
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
} AudTrackStm;

// A track's flags (AudTrack 0x5C), cleared as a byte when it is allocated.
typedef union AudTrackFlags {
    struct {
        u8 b7 : 1;
        u8 bSorted : 1;         // in the second list, the one sorted on f48
        u8 b5 : 1;              // its source's n40 was not negative
        u8 bDetached : 1;       // taken out of its source's apTracks
        u8 bTicked : 1;         // an allocated track (state 1) is freed on its second tick
        u8 unk0 : 3;
    } b;
    u8 n;
} AudTrackFlags;

// A sequenced track's own fields (AudTrack 0x64), reset by fn_800AAE08.
typedef struct AudTrackSeq {
    u8   n64;                   // 0x64   its variation
    u8   n65;                   // 0x65   the channel the next note tries first
    u8   n66;                   // 0x66   the next event in the variation (n3: done)
    u8   n67;                   // 0x67   0xFF, or the event to run next (templates with n0 & 1)
    u8   n68;                   // 0x68   its set of variations
    u8   n69;                   // 0x69
    u8   unk6A[0x6C - 0x6A];
    AudSeqEvent* apEvents[8];   // 0x6C   the note playing on each channel
} AudTrackSeq;

// A playing track (0x8C bytes, from a pool of 32 made by fn_800A98B4).
typedef struct AudTrack {
    UListNode link;             // 0x0    in one of the two track lists (lbl_801F1868)
    AudTrackTmpl* pTmpl;        // 0x8
    AudVoice* apVoices[8];      // 0xC    one per channel (fn_800A9BC8 clears 0x20 bytes)
    struct AudSource* pSource;  // 0x2C   the sound source the track plays for
    AudVoiceParams params;      // 0x30   its flags are cleared when the track starts
    f32  f40;                  // 0x40   from its template
    f32  f44;                   // 0x44   its volume
    f32  f48;                   // 0x48   the second list is sorted on it, highest first
    f32  f4C;                   // 0x4C   advanced by f50 every tick
    f32  f50;                   // 0x50
    u8   nIndex;                // 0x54   its slot in the pool
    u8   nChannel;              // 0x55   its slot in its source's apTracks
    u8   unk56[2];
    s32  nState;                // 0x58   1 allocated, 2 stopped, 3 stopping, 4 filling, 5 filled,
                                //        6 playing (streamed tracks)
    AudTrackFlags bits;         // 0x5C
    u8   n5D;                   // 0x5D   its voices still playing
    u8   unk5E[0x62 - 0x5E];
    u16  n62;                   // 0x62   sequenced tracks: ticks waited for the next event
    union {
        AudTrackStm stm;        // 0x64   streamed tracks (pTmpl->n0 & 8)
        AudTrackSeq seq;        // 0x64   sequenced tracks
    } u;
} AudTrack;
LAYOUT_ASSERT(AudTrack, 0x8C);

// A sound source: one track per channel. Only the fields the tracks read are known.
typedef struct AudSourceTmpl {
    u8   unk0[0x3];
    u8   n3;                    // 0x3    bit 0: its tracks go in the sorted list; bit 2: see fn_800AA744
    u8   unk4[0x8 - 0x4];
    AudTrackTmpl aTracks[1];    // 0x8    one per channel; the count is not known
} AudSourceTmpl;

typedef struct AudSource {
    u8   u0;                    // 0x0    bits set by the sequencer's event fn_800AAB48
    u8   u1;                    // 0x1    the same, for events whose n4 is 0
    u8   unk2[0x3C - 0x2];
    AudSourceTmpl* pTmpl;       // 0x3C
    s16  n40;                   // 0x40
    u8   unk42[0x44 - 0x42];
    AudTrack* apTracks[1];      // 0x44   one per channel; the count is not known
} AudSource;

// The sequencer's event handlers, by event type (filled by fn_800AAD18).
typedef void (*AudSeqHandler)(AudSeqEvent* pEvent, AudTrack* pTrack);
extern AudSeqHandler lbl_801F1880[13];

// One of hlaudemitter.c's 256 emitter instances (lbl_801F2740); only the fields read so far.
typedef struct AudInstance {
    u8   unk0[0x22];
    u8   u22;                   // 0x22   bits cleared by fn_800ADDC8
    u8   unk23[0x30 - 0x23];
    void (*pfnCallback)(u8 nId, u8 nBit, s32 n);  // 0x30
} AudInstance;
LAYOUT_ASSERT(AudInstance, 0x34);

// The two track lists: [0] in start order, [1] sorted on f48, highest first.
extern UList lbl_801F1868[2];
extern UPool lbl_80282098;              // the free tracks
extern AudTrack* lbl_802820A0;          // the pool's memory

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

// hlaudmovie.c's, used by the tracks.
extern f32 lbl_801F17D0[32];            // the volume curves
extern s32 lbl_80282060;                // one bit per curve: 1 = flat (fn_800AA498)
extern f32 lbl_80281460;                // returned by fn_800AB39C
extern s32 lbl_80282080;                // fn_800AB374 says whether it is 0
extern u32 lbl_8028207C;            // 0xD: the tracks tick; 0x40: only some do
void fn_800A8584(AudSource* pSource, u8 n, int n2);
f32  fn_800A85FC(f32 fVolume, f32 fCurve);
void fn_800A9590(AudSource* pSource, AudTrack* pTrack);   // pan and volume, sorted tracks
void fn_800A96DC(AudSource* pSource, AudTrack* pTrack);   // the same for the others
AudStream* fn_800A9438(AudPlayList* pList, u16 nStream, u32* puLength);
AudPlayList* fn_800A9564(u8 nPlayList);
extern u32 lbl_80282018;                // the sequencer re-triggers notes when its low 4 bits are 0

// hlaudtrack.c
void fn_800A9808(AudTrack* pTrack);
u8   fn_800A98B4(void);
s32  fn_800A9A50(void);
void fn_800A9AC8(void);
AudTrack* fn_800A9BC8(AudSource* pSource, AudTrackTmpl* pTmpl, u8 nChannel, f32 fPriority);
s32  fn_800A9D7C(AudTrack* pTrack);
void fn_800AA0D8(AudTrack* pTrack);
void fn_800AA118(AudTrack* pTrack);
void fn_800AA1B8(AudTrack* pTrack, int bNow);
u8   fn_800AA2A4(AudTrack* pTrack);
void fn_800AA34C(AudTrack* pTrack);
void fn_800AA400(AudVoice* pVoice, int nReason);
void fn_800AA444(AudTrack* pTrack, u8 n);
f32  fn_800AA44C(u8 nCurve);
u8   fn_800AA498(u8 nCurve);

// hlaudtrackseq.c
void fn_800AA4BC(AudTrack* pTrack);
void fn_800AA5A0(AudVoice* pVoice, int nReason);
void fn_800AA618(AudTrack* pTrack);
void fn_800AA744(AudSeqEvent* pEvent, AudTrack* pTrack);
void fn_800AAE08(AudTrack* pTrack);
void fn_800AB0CC(AudTrack* pTrack, AudSeqEvent** ppEvent, AudSeqEvent** ppEnd);
AudSeqTone* fn_800AB384(AudSeqBank* pBank, u8 nTone);
u8   fn_800AAD18(void);
void fn_800AADE8(AudTrack* pTrack);
void fn_800AAE70(AudTrack* pTrack);
void fn_800AAE90(AudTrack* pTrack);
void fn_800AAEEC(AudTrack* pTrack);
u8   fn_800AAEFC(AudTrack* pTrack);
void fn_800AB118(AudTrack* pTrack, u8 n, u8 bCheck);
void fn_800AB14C(AudTrack* pTrack, u8 n);
void fn_800AB1B8(AudTrackTmpl* pTmpl);
u32  fn_800AB32C(u32 nRange);           // a random number below nRange
u8   fn_800AB374(void);
f32  fn_800AB39C(void);

// hlaudtrackstm.c
u8   fn_800ABBC8(void);
void fn_800ABC34(AudTrack* pTrack);
void Stm_Exit(AudTrack* pTrack);
void Stm_Start(AudTrack* pTrack);
void fn_800ABD7C(AudTrack* pTrack);
u8   Stm_Tick(AudTrack* pTrack);
void Stm_SetPlayList(AudTrack* pTrack, u8 nPlayList);
void Stm_SetStream(AudTrack* pTrack, u16 nStream, int nMode);

// hlaudvoice.c
AudVoice* fn_800AC4A0(AudVoiceRequest* pRequest);
void fn_800AC6D0(AudVoice* pVoice, AudVoiceParams* pParams, u8 n, f32 f);
void fn_800AC7DC(AudVoice* pVoice, u32 uLen, u16 n4, int bLoud);
void fn_800ACA5C(AudVoice* pVoice, u8 bPause);
void fn_800ACA94(AudVoice* pVoice);     // let it end
void fn_800ACB28(AudVoice* pVoice);     // stop it now
u8   fn_800ACE38(AudVoice* pVoice, u32* puPos);

#endif
