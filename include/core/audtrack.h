// audtrack.h (our name): the sound engine's playing tracks ("perfs" in TW06: Trk_AllocPerf,
// ResetStreamPerf), the voices they play on, and the streamed tracks' disc reads
// (hlaudtrackstm.c). Only the fields the matched code proves are named; the rest is padding.

#ifndef CORE_AUDTRACK_H
#define CORE_AUDTRACK_H

#include "core/audcontainers.h"

// A voice of the sound engine (hlaudvoice.c's), wrapping one of startUp.c's hardware voices.
typedef struct AudVoice {
    UListNode link;             // 0x0    in one of its pool's lists (AudVoicePool.aLists)
    u16  nHwVoice;              // 0x8    the startUp.c voice it plays on
    u8   bHalf : 1;             // 0xA    which half of its ARAM buffer the next stream block fills
    u8   unkA_6 : 2;
    u8   bA_4 : 1;              //        it owns uAram, given back when it stops (fn_800ACB28)
    u8   unkA_3 : 2;
    u8   bA_1 : 1;              //        fn_800AA5A0 keeps the channel's event when it is set
    u8   unkA_0 : 1;
    u8   bStopped : 1;          // 0xB    fn_800ACA94 has stopped it and taken it off its list
    u8   bB_6 : 1;              //        paused; Stm_Tick resumes it once the drive is fine
    u8   unkB : 6;
    u32  uC;                    // 0xC
    s32  n10;                   // 0x10   the request's n4, and the pool list it is on: a voice with
                                //        a lower one can be stolen
    u8   n14;                   // 0x14   its own volume (0-127; fn_800A9590 scales it by 128)
    s8   n15;                   // 0x15   frames left before fn_800ACB98 checks whether it has ended
    u8   unk16[0x18 - 0x16];
    struct AudSeqTone* pTone;   // 0x18   the tone a sequenced track plays on it
    void (*pfnCallback)(struct AudVoice* pVoice, int nReason);  // 0x1C   the request's
    void* pUser;                // 0x20   the request's (the track)
    s32  nIndex;                // 0x24   the request's (the track's channel)
    u32  uAram;                 // 0x28   its ARAM buffer (two halves of 0x7F00 bytes)
    u32  uPlayPos;              // 0x2C   where it is playing in that buffer, in bytes
    u8   unk30[0x3E - 0x30];
    u16  n3E;                   // 0x3E   cleared when the voice is taken (fn_800AC4A0)
} AudVoice;
LAYOUT_ASSERT(AudVoice, 0x40);

#define AUD_NUM_VOICES 50

// The sound engine's voices (lbl_801F19B8, a single pool).
typedef struct AudVoicePool {
    AudVoice aVoices[AUD_NUM_VOICES];   // 0x000
    UList    aLists[3];         // 0xC80  the voices in use, by their n10
    UPool    free;              // 0xCA4  the free voices
} AudVoicePool;
LAYOUT_ASSERT(AudVoicePool, 0xCAC);

extern AudVoicePool lbl_801F19B8[1];
extern s32 lbl_802820B4;                // flipped by each pause: the order fn_800ACCF4 goes through

// Settings for a voice; the flags say which fields are set. fn_800AC91C sets them on a voice;
// a sequenced track's events change its own copy (AudTrack 0x30), handed to fn_800AC6D0 with each
// note and cleared once it is played.
typedef struct AudVoiceParams {
    f32  fPitch;                // 0x0    an event sets it from its n4 / 65536
    s16  nVolume;               // 0x4
    u8   nPan;                  // 0x6    0 left, 0x40 centre, 0x7F right
    u8   n7;                    // 0x7    a second pan (fn_800A7FA8's front/back), 0x7F for a movie
    u8   a8[2];                 // 0x8    from an event, by its n3
    u8   unkA[0xC - 0xA];
    u16  nC;                    // 0xC
    union {
        struct {
            u8 bVolume : 1;     //        nVolume, nPan and n7 are set
            u8 bPitch : 1;      //        fPitch is set
            u8 b5 : 1;          //        a8 was last set at index 0
            u8 b4 : 1;          //        a8 was last set at index 1
            u8 bC : 1;          //        nC is set
            u8 unk0 : 3;
        } b;
        u8 n;                   //        cleared as a whole first
    } flags;                    // 0xE
    u8   unkF;
} AudVoiceParams;
LAYOUT_ASSERT(AudVoiceParams, 0x10);

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
            u16 b10 : 1;        //        set for a movie's voices (Mov_Init)
            u16 b9 : 1;         //        the tone loops (set for a movie's voices too)
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
    u16  n4;                    // 0x4    its sample rate
    u8   nChannels;             // 0x6    one voice each
    u8   nIndex;                // 0x7    its place in the stream file's list
    AudStream aStreams[1];      // 0x8    nStreams of them
} AudPlayList;

// The stream file's header (/AudioStm_GC.sab, read by fn_800A9374): its play lists. On disc each
// entry of apLists is an offset from the end of the array; fn_800A93AC turns them into pointers.
typedef struct AudStreamFile {
    u8   unk0[0x4];
    u32  nPlayLists;            // 0x4
    AudPlayList* apLists[1];    // 0x8    nPlayLists of them
} AudStreamFile;

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

// The template a track plays: one of a bank sound's tracks (AudSound.aTracks, 0x1C bytes).
typedef struct AudTrackTmpl {
    u8   n0;                    // 0x0    0x01: events run one at a time (n67); 0x08: streamed;
                                //        0x20: cleared and set by fn_800A94F4;
                                //        0x40/0x80: fn_800A8584 on free/on a variation change
    u8   n1;                    // 0x1    how the next variation is picked (fn_800AA4BC); 0: never
    u8   n2;                    // 0x2    its channel count (one voice each; for a streamed track,
                                //        its play list's)
    u8   n3;                    // 0x3    events per variation
    u16  n4;                    // 0x4    0 or 1: a single variation
    u8   n6;                    // 0x6    passed to fn_800A8584
    u8   n7;                    // 0x7    variations per set
    u8   n8;                    // 0x8    sets (the track's n68 picks one)
    u8   n9;                    // 0x9    the variation picked last
    u8   nA;                    // 0xA    0xFF, or the set to use (set by fn_800A834C)
    u8   unkB;
    f32  fC;                    // 0xC    copied to the track's f40
    f32  f10;                   // 0x10   fn_800A7EA4's distance scale
    union {
        AudPlayList* pPlayList; // 0x14   streamed tracks; its n3 is the track's volume curve
        AudSeqBank* pBank;      //        sequenced tracks; the same n3
    } data;                     //        on disc an offset into the bank's data, made a pointer
                                //        by fn_800A8FFC
    AudSeqEvent* pEvents;       // 0x18   [n8][n7][n3]; on disc an offset too
} AudTrackTmpl;
LAYOUT_ASSERT(AudTrackTmpl, 0x1C);

// A sound of a bank: tracks played together, one per channel.
typedef struct AudSound {
    u8   nTracks;               // 0x0
    u8   unk1[0x3 - 0x1];
    u8   n3;                    // 0x3    bit 0: placed in the world (distance, pan and doppler),
                                //        its tracks go in the sorted list; bit 1: no doppler;
                                //        bit 2: see fn_800AA744
    f32  f4;                    // 0x4    placed sounds: how far away it is heard
    AudTrackTmpl aTracks[1];    // 0x8    nTracks of them
} AudSound;

// One entry of a bank's sample table (0x38 bytes). Its addresses are relative to the bank's ARAM
// block, in 4-bit units; fn_800A929C adds the block's address.
typedef struct AudSample {
    u32  u0;                    // 0x0
    u32  u4;                    // 0x4
    u32  u8;                    // 0x8    0x8002 when uC is 0
    u32  uC;                    // 0xC
    u8   unk10[0x38 - 0x10];
} AudSample;
LAYOUT_ASSERT(AudSample, 0x38);

// An entry of a bank's group (0x14 bytes).
typedef struct AudGroupEntry {
    AudSample* pSample;         // 0x0    an offset into the sample table on disc
    u8   unk4[0x14 - 0x4];
} AudGroupEntry;

typedef struct AudGroup {
    u16  nEntries;              // 0x0
    u8   unk2[0x4 - 0x2];
    AudGroupEntry aEntries[1];  // 0x4    nEntries of them
} AudGroup;

// A sound bank as loaded (UStream.c reads it; fn_800A8FFC fixes its offsets up). Two can be loaded
// at once: lbl_80282078 (bank 0) and lbl_80282074 (bank 1).
typedef struct AudBank {
    u8   unk0[0x4];
    u32  uAram;                 // 0x4    its samples' ARAM block (fn_800A925C), 0: none
    u8   unk8[0xC - 0x8];
    u32  nSamples;              // 0xC
    AudSample* pSamples;        // 0x10   set by fn_800A8FFC
    u32  n14;                   // 0x14   the size of the data the tracks' data.pPlayList point into
    u32  nGroups;               // 0x18
    AudGroup** ppGroups;        // 0x1C   an offset from the bank on disc
    u8   unk20[0x24 - 0x20];
    u32  n24;                   // 0x24   where the sample table starts after the sample data
    u8   unk28[0x2C - 0x28];
    u32  n2C;                   // 0x2C   the size of what sits between the sounds and the groups
    u32  nSounds;               // 0x30
    AudSound* apSounds[1];      // 0x34   nSounds of them; offsets from the array's end on disc
} AudBank;

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
        u8 b5 : 1;              // its source's nSound was not negative
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
    f32  f48;                   // 0x48   a volume (fn_800A9590 scales it); the second list is
                                //        sorted on it, highest first
    f32  f4C;                   // 0x4C   its pitch, advanced by f50 every tick
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

// A sound source: a playing sound (AudTable.c's table of 256, lbl_80282058), one track per
// channel of its sound, and where it is heard.
typedef struct AudSource {
    u8   u0;                    // 0x0    tracks switched on, a bit each (hlaudemitter.c; the
                                //        sequencer's event fn_800AAB48 sets bits too)
    u8   u1;                    // 0x1    tracks switched off (and fn_800AAB48, for events whose n4 is 0)
    u16  uChanged;              // 0x2    bits 0-7: that track's auParams was set; 0x200: u0 / u1
    u32  auParams[8];           // 0x4    per track (hlaudemitter.c's fn_800AD790)
    f32  aPos[2][3];            // 0x24   where it is from each listener (fn_800B1A40 measures it)
    AudSound* pSound;           // 0x3C
    s16  nSound;                // 0x40   its number (fn_800A85CC)
    u8   unk42[0x44 - 0x42];
    AudTrack* apTracks[8];      // 0x44   one per track of the sound
    f32  fPan;                  // 0x64   -1 left to 1 right
    f32  f68;                   // 0x68   -1 to 1: the second pan (AudVoiceParams.n7)
    f32  afDist[2];             // 0x6C   its distance from each listener
    f32  fDist;                 // 0x74   the nearest of them
    f32  fPitch;                // 0x78   its doppler pitch
} AudSource;
LAYOUT_ASSERT(AudSource, 0x7C);

// The sequencer's event handlers, by event type (filled by fn_800AAD18).
typedef void (*AudSeqHandler)(AudSeqEvent* pEvent, AudTrack* pTrack);
extern AudSeqHandler lbl_801F1880[13];

// One of hlaudemitter.c's 256 emitter instances (lbl_801F2740); only the fields read so far.
typedef struct AudInstance {
    AudSource* pCmd;            // 0x0    its sound source (fn_800A7C30, the same number)
    struct AudInstance* pPrevActive;   // 0x4    the previous in AudEmitters.pActive's list
    struct AudInstance* pNextActive;   // 0x8    the next in AudEmitters.pActive's list (or pFree's)
    struct AudInstance* pNext;  // 0xC    the next instance of the same emitter (AudEmitters)
    f32  vPos[4];               // 0x10   its position (fn_800AD800)
    u8   nId;                   // 0x20   its number (its index in lbl_801F2740)
    s8   nEmitter;              // 0x21   the emitter whose list it is in (-1: none)
    u8   u22;                   // 0x22   bits cleared by fn_800ADDC8
    u8   unk23;
    s32  n24;                   // 0x24   1: fn_800AD1C8 sends vPos again every frame
    s32  n28;                   // 0x28   0: vPos is moved into each view's camera space
    u8   unk2C[0x30 - 0x2C];
    void (*pfnCallback)(u8 nId, u8 nBit, s32 n);  // 0x30
} AudInstance;
LAYOUT_ASSERT(AudInstance, 0x34);

extern AudInstance lbl_801F2740[256];   // the instances, by id (0xFF: none)

// hlaudemitter.c's emitters (lbl_801F2668, 0xD8 bytes): per emitter, its list of instances and
// a sound number. 32 fit the layout: the list heads end where the sound numbers begin.
typedef struct AudEmitters {
    AudInstance* pFree;         // 0x0    the free instances, linked through pNextActive
    AudInstance* pFreeTail;     // 0x4
    AudInstance* pActive;       // 0x8    the instances in use, linked through pNextActive
    AudInstance* pActiveTail;   // 0xC
    AudInstance* apFirst[32];   // 0x10   linked through AudInstance.pNext
    s16  anSound[32];           // 0x90   fn_800ADC44 hands it to fn_800ADA08
    u32  nActive;               // 0xD0   instances in use (up to 256)
    u32  uFlags;                // 0xD4   bit 0: set up (fn_800ACECC)
} AudEmitters;
LAYOUT_ASSERT(AudEmitters, 0xD8);

extern AudEmitters lbl_801F2668;

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

// The movie player's sound (hlaudmovie.c): two voices, left and right, each playing a ring of ten
// blocks in ARAM that fn_800A8AD4 fills as the movie's sound chunks come in.
#define MOVIE_BLOCK_SIZE 0x2FC0         // one channel's ADPCM data per block (MovieSoundBlock)
#define MOVIE_BLOCKS     10

typedef struct MovieSound {
    AudVoice* pLeft;            // 0x0
    AudVoice* pRight;           // 0x4
    u32  uPlayed;               // 0x8    bytes played, per channel
    u32  uSent;                 // 0xC    bytes sent to ARAM, per channel
    u8   nPlayBlock;            // 0x10   the block playing
    u8   nSendBlock;            // 0x11   the block the next chunk fills
    u8   nState;                // 0x12   0 off, 1 filling before the start, 2 playing
    u8   unk13[0x18 - 0x13];
} MovieSound;
LAYOUT_ASSERT(MovieSound, 0x18);

// A 0x48-byte block fn_800A8754 allocates and clears (lbl_8028206C); nothing reads it yet.
typedef struct AudBlock48 {
    u8   unk0[0x48];
} AudBlock48;

extern AudSource* lbl_80282058;         // AudTable.c's table
extern u8 lbl_80282068;                 // the number of listeners (hlaudmovie.c)
extern f32 lbl_801F17D0[32];            // the volume of each curve (fn_800AA44C; hlaudmovie.c)
extern s32 lbl_80282060;                // one bit per curve: 1 = flat (fn_800AA498; hlaudmovie.c)
extern f32 lbl_80281460;                // fn_800A86BC's rate, fn_800AB39C's result (hlaudmovie.c)
extern s32 lbl_80282080;                // fn_800AB374 says whether it is 0 (hlaudmovie.c)
extern AudStreamFile* lbl_80282070;     // the stream file's header (hlaudmovie.c)
extern AudBank* lbl_80282074;           // bank 1 (hlaudmovie.c)
extern AudBank* lbl_80282078;           // bank 0 (hlaudmovie.c)
extern u32 lbl_8028207C;                // what is loaded (hlaudmovie.c): 0x01 set up, 0x04/0x08
                                        // bank 0 and its samples, 0x10/0x20 bank 1 and its samples;
                                        // the tracks tick when 0xD is set, only some when 0x40 is
extern u32 lbl_80282018;                // the sequencer re-triggers notes when its low 4 bits are 0

// The audio locks (0x800B5934): the name is EA's label for who holds them.
void fn_800B596C(const char* szWho);    // take the stream lock
void fn_800B5994(const char* szWho);    // give it back
void fn_800B59BC(const char* szWho);    // take the read-queue lock
void fn_800B59EC(const char* szWho);    // give it back

// The audio memory stack (UAudMemStack.c).
void* fn_800B5BD8(u32 uSize);
void  fn_800B5C04(void* p);

// AudTable.c
u8             fn_800A7AF0(void);
s32            fn_800A7C24(void);
void           fn_800A7C2C(void);
AudSource*     fn_800A7C30(u8 nEntry, s16 nSound);
void           fn_800A7CA4(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, f32 (*aPos)[3], u16 uMask);
void           fn_800A8200(u8 nEntry);
void           fn_800A8248(u8 nEntry, u8 nTrack, u8 n);
void           fn_800A82CC(u8 nEntry, u8 nTrack, u8 n);
void           fn_800A834C(s16 nSound, u8 nTrack, u8 n);       // the track's nA
void           fn_800A8394(u8 nEntry, u8 nTrack, u8 n, int bCheck);
void           fn_800A8424(u8 nEntry, u8 nTrack, f32 fVolume);
void           fn_800A84A4(u8 nEntry, u8 nTrack, f32 fPitch);
void           fn_800A8524(AudSound* pSound, int n);
void           fn_800A8584(AudSource* pSource, u8 nTrack, s32 n);

// hlaudmovie.c
AudSound*    fn_800A85CC(s16 nSound);
f32          fn_800A85FC(f32 fVolume, f32 fCurve);
u8*          fn_800A942C(u32 uSize, u8 nPlayList);                  // the stream buffer
void         fn_800A9434(u8* pBuffer, u32 uSize, u8 nPlayList);     // give it back
AudStream*   fn_800A9438(AudPlayList* pList, u16 nStream, u32* puLength);
void         fn_800A94F4(s16 nSound, u8 nTrack, u8 bOn);
u32          fn_800A955C(u8 nPlayList);                              // the buffer size it needs
AudPlayList* fn_800A9564(u8 nPlayList);
void         fn_800A9590(AudSource* pSource, AudTrack* pTrack, f32 fVolume);   // placed sounds
void         fn_800A96DC(AudSource* pSource, AudTrack* pTrack, f32 fVolume);   // the others

extern AudStreamQueue lbl_801F18B8;
extern s32 lbl_80281468;                // the stream file (hlaudmovie.c opens "/AudioStm_GC.sab")
extern u8 lbl_802820A8;                 // the last read id handed out (hlaudtrackstm.c)
extern AudTrack* lbl_802820AC;          // the track whose block is being DMA'd (hlaudtrackstm.c)

// hlaudtrack.c
void fn_800A9808(AudTrack* pTrack);
u8   fn_800A98B4(void);
s32  fn_800A9A50(u8 a, u8 b);          // fn_800A8DC8's a and b, unused
void fn_800A9AC4(void);
void fn_800A9AC8(void);
AudTrack* fn_800A9BC8(AudSource* pSource, AudTrackTmpl* pTmpl, u8 nChannel, f32 fPriority);
s32  fn_800A9D7C(AudTrack* pTrack);
void fn_800A9E7C(AudSource* pSource, AudTrack* pTrack, AudTrackTmpl* pTmpl, u8 nChannel, u8 bOn,
                 u8 bOff, f32 fPriority);
void fn_800AA0D8(AudTrack* pTrack);
void fn_800AA118(AudTrack* pTrack);
void fn_800AA1B8(AudTrack* pTrack, int bNow);
u8   fn_800AA2A4(AudTrack* pTrack);
void fn_800AA2EC(AudTrack* pTrack, u8 n, u8 bCheck);
void fn_800AA30C(AudTrack* pTrack, u8 n);
void fn_800AA32C(AudTrack* pTrack, u8 n);
void fn_800AA34C(AudTrack* pTrack);
void fn_800AA3D4(AudTrackTmpl* pTmpl);
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
s32  fn_800AC494(void);
void fn_800AC49C(void);
AudVoice* fn_800AC4A0(AudVoiceRequest* pRequest);
void fn_800AC6D0(AudVoice* pVoice, AudVoiceParams* pParams, u8 n, f32 f);
void fn_800AC7DC(AudVoice* pVoice, u32 uLen, u16 n4, int bLoud);
void fn_800AC91C(AudVoice* pVoice, AudVoiceParams* pParams);
void fn_800ACA5C(AudVoice* pVoice, u8 bPause);
void fn_800ACA94(AudVoice* pVoice);     // let it end
void fn_800ACB28(AudVoice* pVoice);     // stop it now
u8   fn_800ACE38(AudVoice* pVoice, u32* puPos);

// hlaudemitter.c
void fn_800ADDC8(u8 nId, u8 nBit, s32 n);
int  fn_800AD0C4(void);                 // fn_800AD450 on every instance in use, emitters emptied
void fn_800AD450(u8 nId);
void fn_800ADB4C(s16 nEmitter, u8 nTrack, u8 bOn);   // for every instance of an emitter: fn_800AD698
void fn_800ADC44(s16 nEmitter, u8 nTrack, u8 n);     // fn_800AD9AC
void fn_800ADCD0(s16 nEmitter, u8 nTrack, u8 n, int bCheck);   // fn_800ADA28
void fn_800ADD54(s16 nEmitter, u8 nTrack, f32 fVolume);        // fn_800ADA94

#endif
