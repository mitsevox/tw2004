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
    u8   unkC[0x14 - 0xC];
    u8   n14;                   // 0x14   its own volume (0-127; fn_800A9590 scales it by 128)
    u8   unk15[0x28 - 0x15];
    u32  uAram;                 // 0x28   its ARAM buffer (two halves of 0x7F00 bytes)
    u32  uPlayPos;              // 0x2C   where it is playing in that buffer, in bytes
} AudVoice;

// What fn_800AC91C sets on a voice; the flags say which fields are set.
typedef struct AudVoiceParams {
    f32  fPitch;                // 0x0
    s16  nVolume;               // 0x4
    u8   nPan;                  // 0x6    0 left, 0x40 centre, 0x7F right
    u8   n7;                    // 0x7    a second pan (fn_800A7FA8's front/back), 0x7F for a movie
    u8   unk8[0xE - 0x8];
    union {
        struct {
            u8 bVolume : 1;     //        nVolume, nPan and n7 are set
            u8 bPitch : 1;      //        fPitch is set
            u8 unk0 : 6;
        } b;
        u8 n;                   //        cleared as a whole first
    } flags;                    // 0xE
} AudVoiceParams;
LAYOUT_ASSERT(AudVoiceParams, 0x10);

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
            u16 b10 : 1;        //        set for a movie's voices (Mov_Init)
            u16 b9 : 1;         //        set for a movie's voices (Mov_Init)
            u16 unk0 : 9;
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

// The template a track plays: one of a bank sound's tracks (AudSound.aTracks).
typedef struct AudTrackTmpl {
    u8   n0;                    // 0x0    bit 0x08: a streamed track; bit 0x20: cleared and set by
                                //        fn_800A94F4
    u8   unk1;
    u8   n2;                    // 0x2    its channel count (for a streamed track, its play list's)
    u8   unk3[0xA - 0x3];
    s8   nA;                    // 0xA    set by fn_800A834C
    u8   unkB[0x10 - 0xB];
    f32  f10;                   // 0x10   fn_800A7EA4's distance scale
    AudPlayList* pPlayList;     // 0x14   streamed tracks: their play list. On disc (every track) an
                                //        offset into the bank's data, made a pointer by fn_800A8FFC
    u8*  p18;                   // 0x18   an offset into the bank's sample data, made a pointer too
} AudTrackTmpl;
LAYOUT_ASSERT(AudTrackTmpl, 0x1C);

// A sound of a bank: tracks played together.
typedef struct AudSound {
    u8   nTracks;               // 0x0
    u8   unk1[0x3 - 0x1];
    u8   n3;                    // 0x3    bit 0: placed in the world (distance, pan and doppler);
                                //        bit 1: no doppler
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
    u32  n14;                   // 0x14   the size of the data the tracks' pPlayList point into
    u32  nGroups;               // 0x18
    AudGroup** ppGroups;        // 0x1C   an offset from the bank on disc
    u8   unk20[0x24 - 0x20];
    u32  n24;                   // 0x24   where the sample table starts after the sample data
    u8   unk28[0x2C - 0x28];
    u32  n2C;                   // 0x2C   the size of what sits between the sounds and the groups
    u32  nSounds;               // 0x30
    AudSound* apSounds[1];      // 0x34   nSounds of them; offsets from the array's end on disc
} AudBank;

// A playing track. The stream fields (0x64-0x8A) are cleared by fn_800ABC54.
typedef struct AudTrack {
    u8   unk0[0x8];
    AudTrackTmpl* pTmpl;        // 0x8
    AudVoice* apVoices[12];     // 0xC    one per channel; 12 is a guess (0x3E is the next field in use)
    u8   unk3C[0x44 - 0x3C];
    f32  f44;                   // 0x44   its volume
    f32  f48;                   // 0x48   a volume fn_800A9590 scales
    f32  f4C;                   // 0x4C   its pitch
    u8   unk50[0x58 - 0x50];
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

// A playing sound (AudTable.c's table of 256, lbl_80282058): its tracks and where it is heard.
typedef struct AudTableEntry {
    u8   unk0[0x24];
    f32  aPos[2][3];            // 0x24   where it is from each listener (fn_800B1A40 measures it)
    AudSound* pSound;           // 0x3C
    s16  nSound;                // 0x40   its number (fn_800A85CC)
    AudTrack* apTracks[8];      // 0x44   one per track of the sound
    f32  fPan;                  // 0x64   -1 left to 1 right
    f32  f68;                   // 0x68   -1 to 1: the second pan (AudVoiceParams.n7)
    f32  afDist[2];             // 0x6C   its distance from each listener
    f32  fDist;                 // 0x74   the nearest of them
    f32  fPitch;                // 0x78   its doppler pitch
} AudTableEntry;
LAYOUT_ASSERT(AudTableEntry, 0x7C);

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

extern AudTableEntry* lbl_80282058;     // AudTable.c's table
extern u8 lbl_80282068;                 // the number of listeners (hlaudmovie.c)
extern f32 lbl_801F17D0[32];            // the volume of each curve (fn_800AA44C; hlaudmovie.c)
extern AudStreamFile* lbl_80282070;     // the stream file's header (hlaudmovie.c)
extern AudBank* lbl_80282074;           // bank 1 (hlaudmovie.c)
extern AudBank* lbl_80282078;           // bank 0 (hlaudmovie.c)
extern u32 lbl_8028207C;                // what is loaded (hlaudmovie.c): 0x01 set up, 0x04/0x08
                                        // bank 0 and its samples, 0x10/0x20 bank 1 and its samples

// The audio locks (0x800B5934): the name is EA's label for who holds them.
void fn_800B596C(const char* szWho);    // take the stream lock
void fn_800B5994(const char* szWho);    // give it back
void fn_800B59BC(const char* szWho);    // take the read-queue lock
void fn_800B59EC(const char* szWho);    // give it back

// The audio memory stack (UAudMemStack.c).
void* fn_800B5BD8(u32 uSize);
void  fn_800B5C04(void* p);

// AudTable.c
int            fn_800A7AF0(void);
s32            fn_800A7C24(void);
void           fn_800A7C2C(void);
AudTableEntry* fn_800A7C30(u8 nEntry, s16 nSound);
void           fn_800A7CA4(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, s32 n, u16 uMask);
void           fn_800A8200(u8 nEntry);
void           fn_800A8248(u8 nEntry, u8 nTrack, s32 n);
void           fn_800A82CC(u8 nEntry, u8 nTrack, s32 n);
void           fn_800A834C(s16 nSound, u8 nTrack, s8 n);
void           fn_800A8394(u8 nEntry, u8 nTrack, s32 n, u8 b);
void           fn_800A8424(u8 nEntry, u8 nTrack, f32 fVolume);
void           fn_800A84A4(u8 nEntry, u8 nTrack, f32 fPitch);
void           fn_800A8524(AudSound* pSound, int n);
void           fn_800A8584(AudTableEntry* pEntry, u8 nTrack, s32 n);

// hlaudmovie.c
AudSound*    fn_800A85CC(s16 nSound);
f32          fn_800A85FC(f32 fVolume, f32 fCurve);
u8*          fn_800A942C(u32 uSize, u8 nPlayList);                  // the stream buffer
void         fn_800A9434(u8* pBuffer, u32 uSize, u8 nPlayList);     // give it back
AudStream*   fn_800A9438(AudPlayList* pList, u16 nStream, u32* puLength);
void         fn_800A94F4(s16 nSound, u8 nTrack, u8 bOn);
u32          fn_800A955C(u8 nPlayList);                              // the buffer size it needs
AudPlayList* fn_800A9564(u8 nPlayList);
void         fn_800A9590(AudTableEntry* pEntry, AudTrack* pTrack, f32 fVolume);
void         fn_800A96DC(AudTableEntry* pEntry, AudTrack* pTrack, f32 fVolume);

// hlaudtrackstm.c
void Stm_SetPlayList(AudTrack* pTrack, u8 nPlayList);
void Stm_SetStream(AudTrack* pTrack, u16 nStream, int nMode);

// hlaudvoice.c (0x800AC4A0-)
AudVoice* fn_800AC4A0(AudVoiceRequest* pRequest);
void      fn_800AC7DC(AudVoice* pVoice, u32 uLen, u16 n4, int bLoud);
void      fn_800AC91C(AudVoice* pVoice, AudVoiceParams* pParams);
void      fn_800ACB28(AudVoice* pVoice);

extern AudStreamQueue lbl_801F18B8;
extern s32 lbl_80281468;                // the stream file (hlaudmovie.c opens "/AudioStm_GC.sab")
extern u8 lbl_802820A8;                 // the last read id handed out (hlaudtrackstm.c)
extern AudTrack* lbl_802820AC;          // the track whose block is being DMA'd (hlaudtrackstm.c)

#endif
