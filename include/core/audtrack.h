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
    union {
        struct {
            u8 bHalf : 1;       // 0xA    which half of its ARAM buffer the next stream block fills
            u8 bNoReverb : 1;   //        Voc_Render starts it with aux A (reverb) off
            u8 bSkipRender : 1; //        stolen: Voc_Render skips its next settings (and clears it)
            u8 bStream : 1;     //        it owns uAram, given back when it stops (Voc_Delete)
            u8 unkA_3 : 2;      //        the request's b11 and b10
            u8 bLoops : 1;      //        its tone loops: VoiceEndCB keeps the channel's note to play
                                //        again when the voice is stolen
            u8 bStart : 1;      //        set up (Voc_Start, Voc_StartStream), started by Voc_Render
            u8 bStopped : 1;    // 0xB    Voc_Stop has stopped it and taken it off its list
            u8 bHeld : 1;       //        a stream voice Voc_PauseAll left paused; Stm_Tick resumes
                                //        it once the drive is fine
            u8 unkB : 6;
        } b;
        u16 n;                  //        the request's flags (Voc_Alloc); cleared as a whole when
                                //        the voice is freed (Voc_Cycle)
    } flags;                    // 0xA
    u32  uRate;                 // 0xC    its playback rate (Voc_Start, Voc_StartStream); Voc_Render
                                //        sends it times the track's pitch
    s32  nStealLevel;           // 0x10   the request's, and the pool list it is on (0 once it is
                                //        ending): a request with a higher one can steal it
    u8   nVolume;               // 0x14   its own volume (0-127; TrkRender3D scales it by 128)
    s8   nEndDelay;             // 0x15   frames left before Voc_Cycle checks whether it has ended
    u8   unk16[0x18 - 0x16];
    struct AudSeqTone* pTone;   // 0x18   the tone a sequenced track plays on it
    void (*pfnCallback)(struct AudVoice* pVoice, int nReason);  // 0x1C   the request's
    void* pUser;                // 0x20   the request's (the track)
    s32  nIndex;                // 0x24   the request's (the track's channel)
    u32  uAram;                 // 0x28   its ARAM buffer (two halves of 0x7F00 bytes)
    u32  uPlayPos;              // 0x2C   where it is playing in that buffer, in bytes
    u8   unk30[0x3E - 0x30];
    u8   n3E;                   // 0x3E   cleared when the voice is taken (Voc_Alloc, a byte store)
    u8   unk3F;
} AudVoice;
LAYOUT_ASSERT(AudVoice, 0x40);

#define AUD_NUM_VOICES 50

// The sound engine's voices (gVocCores, a single pool).
typedef struct AudVoicePool {
    AudVoice aVoices[AUD_NUM_VOICES];   // 0x000
    UList    aLists[3];         // 0xC80  the voices in use, by their nStealLevel
    UPool    free;              // 0xCA4  the free voices
} AudVoicePool;
LAYOUT_ASSERT(AudVoicePool, 0xCAC);

extern AudVoicePool gVocCores[1];
extern u8  gVocInUse;       // the voices in use, counted by Voc_Cycle
extern s32 gVocPauseOrder;  // flipped by each pause: the order Voc_PauseAll goes through

// Settings for a voice; the flags say which fields are set. Voc_Render sets them on a voice;
// a sequenced track's events change its own copy (AudTrack 0x30), handed to Voc_Start with each
// note and cleared once it is played.
typedef struct AudVoiceParams {
    f32  fPitch;                // 0x0    an event sets it from its n4 / 65536
    s16  nVolume;               // 0x4
    u8   nPan;                  // 0x6    0 left, 0x40 centre, 0x7F right
    u8   n7;                    // 0x7    a second pan (Panning3D's front/back), 0x7F for a movie
    u8   anAdsr[2];             // 0x8    envelope values for the tone: [0] attack, [1] decay
                                //        (event 10, OnModADSRVol)
    u8   unkA[0xC - 0xA];
    u16  nStartOffset;          // 0xC    where the note starts (event 11, OnModStartOffset); not
                                //        read in this build
    union {
        struct {
            u8 bVolume : 1;     //        nVolume, nPan and n7 are set
            u8 bPitch : 1;      //        fPitch is set
            u8 bAttack : 1;     //        anAdsr[0] is set (the last one set)
            u8 bDecay : 1;      //        anAdsr[1] is set (the last one set)
            u8 bStartOffset : 1; //       nStartOffset is set
            u8 unk0 : 3;
        } b;
        u8 n;                   //        cleared as a whole first
    } flags;                    // 0xE
    u8   unkF;
} AudVoiceParams;
LAYOUT_ASSERT(AudVoiceParams, 0x10);

// What Voc_Alloc is asked for when a track takes a voice.
typedef struct AudVoiceRequest {
    s16  nPriority;             // 0x0    a sequenced note asks with its volume
    u8   n2;                    // 0x2
    u8   n3;                    // 0x3
    s32  nStealLevel;           // 0x4    how hard the voice is to steal (0-2; becomes
                                //        AudVoice.nStealLevel)
    union {
        struct {
            u16 unk15 : 1;
            u16 bNoReverb : 1;  //        aux A (reverb) off: streams, movies, and notes of sounds
                                //        with n3 & 4 or templates with n0 & 0x20
            u16 unk13 : 1;
            u16 bStream : 1;    //        Voc_Alloc gives the voice an ARAM stream buffer
            u16 b11 : 1;        //        bit 2 of the play list's number
            u16 b10 : 1;        //        set for a movie's voices (Mov_Init)
            u16 bLoops : 1;     //        the tone loops (set for a movie's voices too)
            u16 unk0 : 9;
        } b;
        u16 n;                  //        cleared as a whole first; copied into AudVoice.flags
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

// A play list of streams (Ses_GetStreamPlayList finds one by its number).
typedef struct AudPlayList {
    u16  nStreams;              // 0x0
    u8   nId;                   // 0x2    the play list's number (bit 2 goes into the voice request)
    u8   n3;                    // 0x3    its volume curve (Mas_GetSubmix)
    u16  n4;                    // 0x4    its sample rate
    u8   nChannels;             // 0x6    one voice each
    u8   nIndex;                // 0x7    its place in the stream file's list
    AudStream aStreams[1];      // 0x8    nStreams of them
} AudPlayList;

// The stream file's header (/AudioStm_GC.sab, read by Ses_AllocStreamFileHdr): its play lists. On disc each
// entry of apLists is an offset from the end of the array; Ses_ProcessStreamFileHdr turns them into pointers.
typedef struct AudStreamFile {
    u8   unk0[0x4];
    u32  nPlayLists;            // 0x4
    AudPlayList* apLists[1];    // 0x8    nPlayLists of them
} AudStreamFile;

// A streamed track's flags (AudTrack 0x89), cleared as a byte.
typedef union AudTrackStmFlags {
    struct {
        u8 bStarved : 1;        // its voices are paused because the reads fell behind
        u8 bHold : 1;           // when the buffer is full: hold (state 5) instead of playing;
                                // never set in this build
        u8 unk5 : 1;
        u8 bEnded : 1;          // the stream ended and does not loop
        u8 bSilenceQueued : 1;  // the silence after the end is queued (Stm_QueueSilence); the
                                // next refill stops the voices
        u8 unk0 : 3;
    } b;
    u8 n;
} AudTrackStmFlags;

// A tone of a sequencer bank (0x14 bytes).
typedef struct AudSeqTone {
    struct SoundHeader* pHeader;    // 0x0    the sound it plays (startup.h)
    u32  u4;                    // 0x4    Voc_Start picks the voice's uC at random from u4 up to u8
    u32  u8;                    // 0x8
    struct VoiceEnvelope* pEnv; // 0xC    its volume envelope (startup.h)
    s32  n10;                   // 0x10   bit 0: it loops
} AudSeqTone;
LAYOUT_ASSERT(AudSeqTone, 0x14);

// The tones a sequenced track plays (Ses_GetInstrumentTone picks one).
typedef struct AudSeqBank {
    u8   unk0[0x3];
    u8   n3;                    // 0x3    its volume curve, as in AudPlayList
    AudSeqTone aTones[1];       // 0x4    the count is not known
} AudSeqBank;

// A sequencer event (8 bytes). Its handler is gSeqCmdHandlers[nType].
typedef struct AudSeqEvent {
    u16  n0;                    // 0x0    ticks to wait before it runs (scaled by Seq_Check)
    u8   nType;                 // 0x2
    u8   n3;                    // 0x3
    s32  n4;                    // 0x4
} AudSeqEvent;
LAYOUT_ASSERT(AudSeqEvent, 0x8);

// The template a track plays: one of a bank sound's tracks (AudSound.aTracks, 0x1C bytes).
typedef struct AudTrackTmpl {
    u8   n0;                    // 0x0    0x01 stepped: runs only the event Seq_Step asks for (n67);
                                //        0x02 loops (OnEnd goes on to a new variation); 0x04
                                //        switched by hand (Trk_UpdatePerf's bOn / bOff; not with
                                //        0x01); 0x08 streamed; 0x10 no restart while playing;
                                //        0x20 reverb off (Ses_TmplOvrTrackRvbMode sets and clears
                                //        it); 0x40 / 0x80 Emi_TrackCallback on free / on a
                                //        variation change
    u8   n1;                    // 0x1    how the next variation is picked (AutoSelectVariation); 0: never
    u8   n2;                    // 0x2    its channel count (one voice each; for a streamed track,
                                //        its play list's)
    u8   n3;                    // 0x3    events per variation
    u16  n4;                    // 0x4    0 or 1: a single variation
    u8   n6;                    // 0x6    passed to Emi_TrackCallback
    u8   n7;                    // 0x7    variations per set
    u8   n8;                    // 0x8    sets (the track's n68 picks one)
    u8   n9;                    // 0x9    the variation picked last
    u8   nA;                    // 0xA    0xFF, or the set to use (set by Emi_SetTrackVarRangeTmpl)
    u8   unkB;
    f32  fC;                    // 0xC    copied to the track's f40
    f32  f10;                   // 0x10   Attenuation3D's distance scale
    union {
        AudPlayList* pPlayList; // 0x14   streamed tracks; its n3 is the track's volume curve
        AudSeqBank* pBank;      //        sequenced tracks; the same n3
    } data;                     //        on disc an offset into the bank's data, made a pointer
                                //        by Ses_ProcessArticulationData
    AudSeqEvent* pEvents;       // 0x18   [n8][n7][n3]; on disc an offset too
} AudTrackTmpl;
LAYOUT_ASSERT(AudTrackTmpl, 0x1C);

// A sound of a bank: tracks played together, one per channel.
typedef struct AudSound {
    u8   nTracks;               // 0x0
    u8   unk1[0x3 - 0x1];
    u8   n3;                    // 0x3    bit 0: placed in the world (distance, pan and doppler),
                                //        its tracks go in the sorted list; bit 1: no doppler;
                                //        bit 2: see OnKeyOn
    f32  f4;                    // 0x4    placed sounds: how far away it is heard
    AudTrackTmpl aTracks[1];    // 0x8    nTracks of them
} AudSound;

// One entry of a bank's sample table (0x38 bytes). Its addresses are relative to the bank's ARAM
// block, in 4-bit units; Ses_ProcessSampleData adds the block's address.
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

// A sound bank as loaded (UStream.c reads it; Ses_ProcessArticulationData fixes its offsets up).
// Two can be loaded at once: gSesBank0 (bank 0) and gSesBank1 (bank 1).
typedef struct AudBank {
    u8   unk0[0x4];
    u32  uAram;                 // 0x4    its samples' ARAM block (Ses_AllocSampleAram), 0: none
    u8   unk8[0xC - 0x8];
    u32  nSamples;              // 0xC
    AudSample* pSamples;        // 0x10   set by Ses_ProcessArticulationData
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

// A streamed track's own fields (AudTrack 0x64), cleared by ResetStreamPerf.
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
        u8 bNewVariation : 1;   // a looping End event moved to a new variation: Seq_Tick
                                // fetches its events (OnEnd)
        u8 bSorted : 1;         // in the second list, the one sorted on fDistAttn
        u8 b5 : 1;              // its source's nSound was not negative
        u8 bDetached : 1;       // taken out of its source's apTracks
        u8 bTicked : 1;         // an allocated track (state 1) is freed on its second tick
        u8 unk0 : 3;
    } b;
    u8 n;
} AudTrackFlags;

// A sequenced track's own fields (AudTrack 0x64), reset by ResetSequencerPerf.
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

// A playing track (0x8C bytes, from a pool of 32 made by Trk_InitModule).
typedef struct AudTrack {
    UListNode link;             // 0x0    in one of the two track lists (gTrkPerfLists)
    AudTrackTmpl* pTmpl;        // 0x8
    AudVoice* apVoices[8];      // 0xC    one per channel (Trk_AllocPerf clears 0x20 bytes)
    struct AudSource* pSource;  // 0x2C   the sound source the track plays for
    AudVoiceParams params;      // 0x30   its flags are cleared when the track starts
    f32  f40;                  // 0x40   from its template
    f32  fVolume;               // 0x44   its volume (Aud_EmiSetTrackAttenuation), 1 at the start
    f32  fDistAttn;             // 0x48   its distance attenuation (TW07's distAttn; TrkRender3D
                                //        scales by it); the second list is sorted on it, highest
                                //        first
    f32  fPitch;                // 0x4C   its pitch factor, 1 at the start
    f32  fPitchRamp;            // 0x50   added to fPitch every tick (event 5, OnPitchRamp)
    u8   nIndex;                // 0x54   its slot in the pool
    u8   nChannel;              // 0x55   its slot in its source's apTracks
    u8   unk56[2];
    s32  nState;                // 0x58   1 allocated, 2 stopped, 3 stopping, 4 filling, 5 filled,
                                //        6 playing (streamed tracks)
    AudTrackFlags bits;         // 0x5C
    u8   nVoices;               // 0x5D   its voices still playing
    u8   unk5E[0x62 - 0x5E];
    u16  nWait;                 // 0x62   sequenced tracks: ticks waited for the next event
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
                                //        sequencer's event OnTrackStatus sets bits too)
    u8   u1;                    // 0x1    tracks switched off (and OnTrackStatus, for events whose n4 is 0)
    u16  uChanged;              // 0x2    bits 0-7: that track's auParams was set; 0x200: u0 / u1
    u32  auParams[8];           // 0x4    each track's controller value (Aud_EmiSetControllerInt;
                                //        streamed tracks: PreprocessControllers)
    f32  aPos[2][3];            // 0x24   where it is from each listener (audvec3_ApproxLength measures it)
    AudSound* pSound;           // 0x3C
    s16  nSound;                // 0x40   its number (Ses_GetEmitterTemplateFromID)
    u8   unk42[0x44 - 0x42];
    AudTrack* apTracks[8];      // 0x44   one per track of the sound
    f32  fPan;                  // 0x64   -1 left to 1 right
    f32  f68;                   // 0x68   -1 to 1: the second pan (AudVoiceParams.n7)
    f32  afDist[2];             // 0x6C   its distance from each listener
    f32  fDist;                 // 0x74   the nearest of them
    f32  fPitch;                // 0x78   its doppler pitch
} AudSource;
LAYOUT_ASSERT(AudSource, 0x7C);

// The sequencer's event handlers, by event type (filled by Seq_InitModule).
typedef void (*AudSeqHandler)(AudSeqEvent* pEvent, AudTrack* pTrack);
extern AudSeqHandler gSeqCmdHandlers[13];

// One of hlaudemitter.c's 256 emitter instances (lbl_801F2740); only the fields read so far.
typedef struct AudInstance {
    AudSource* pCmd;            // 0x0    its sound source (Emi_AddInstance, the same number)
    struct AudInstance* pPrevActive;   // 0x4    the previous in AudEmitters.pActive's list
    struct AudInstance* pNextActive;   // 0x8    the next in AudEmitters.pActive's list (or pFree's)
    struct AudInstance* pNext;  // 0xC    the next instance of the same emitter (AudEmitters)
    f32  vPos[4];               // 0x10   its position (Aud_EmiSet3DPos)
    u8   nId;                   // 0x20   its number (its index in lbl_801F2740)
    s8   nEmitter;              // 0x21   the emitter whose list it is in (-1: none)
    u8   u22;                   // 0x22   tracks playing, a bit each (Aud_EmiGetTrackStatus; set by
                                //        Aud_EmiSetTrackStatus, cleared by Aud_EmiTrkCB)
    u8   unk23;
    s32  n24;                   // 0x24   1: Aud_EmiCycle sends vPos again every frame
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
    s16  anSound[32];           // 0x90   Aud_EmiAliasSetTrackVarRange hands it to Aud_EmiSetTrackVarRangeTmpl
    u32  nActive;               // 0xD0   instances in use (up to 256)
    u32  uFlags;                // 0xD4   bit 0: set up (Aud_EmiInitOnce)
} AudEmitters;
LAYOUT_ASSERT(AudEmitters, 0xD8);

extern AudEmitters lbl_801F2668;

// The two track lists: [0] in start order, [1] sorted on fDistAttn, highest first.
extern UList gTrkPerfLists[2];
extern UPool gTrkPerfPool;   // the free tracks
extern AudTrack* gTrkPerfs;  // the pool's memory

// A disc read waiting in the stream read queue (gAudStreamReadQueue). bSilence marks a request
// (Stm_QueueSilence) to send a block of silence to the track's voices (Stm_SendSilenceToVoices)
// instead of a read.
typedef struct AudStreamRead {
    s32  hFile;                 // 0x0
    u8*  pDst;                  // 0x4
    u32  uLen;                  // 0x8
    u32  uOffset;               // 0xC
    void (*pfnDone)(void* pDst, int nBytes, AudTrack* pTrack, u8 nId);  // 0x10
    AudTrack* pTrack;           // 0x14
    u8   nId;                   // 0x18
    u8   n19;                   // 0x19
    u8   bSilence;              // 0x1A
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
// blocks in ARAM that Mov_SendSoundBlock fills as the movie's sound chunks come in.
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

// The listeners' 0x48-byte block (hlaudmic.c's gMicData): Mic_InitModule allocates and clears it;
// nothing reads it in this build.
typedef struct AudMicBlock {
    u8   unk0[0x48];
} AudMicBlock;

extern AudSource* lbl_80282058;           // AudTable.c's table
extern u8 gMicCount;                      // the number of listeners (hlaudmic.c)
extern f32 gMasSubmixVolumes[32];         // the volume of each curve (Mas_GetSubmix; HLAudMaster.c)
extern s32 gMasMuteMask;                  // one bit per curve: 1 = muted (Mas_IsChanMuted; HLAudMaster.c)
extern f32 gMasTickRateScale;             // Mas_SetTickRate's rate, Mas_GetUpdateRateScale's
                                          // result (HLAudMaster.c)
extern s32 gSesSession;                   // Ses_IsSessionZero says whether it is 0 (hlaudsession.c)
extern AudStreamFile* gSesStreamFileHdr;  // the stream file's header (hlaudsession.c)
extern AudBank* gSesBank1;                // bank 1 (hlaudsession.c)
extern AudBank* gSesBank0;                // bank 0 (hlaudsession.c)
extern u32 gSesFlags;                     // what is loaded (hlaudsession.c): 0x01 set up, 0x04/0x08
                                          // bank 0 and its samples, 0x10/0x20 bank 1 and its samples;
                                          // the tracks tick when 0xD is set, only some when 0x40 is
extern u32 lbl_80282018;                  // the sequencer re-triggers notes when its low 4 bits are 0

// The audio locks (AudLock.c): the name is EA's label for who holds them.
void AudLock_Init(void);                          // set both locks up
void AudLock_Lock(const char* szWho);             // take the stream lock
void AudLock_Unlock(const char* szWho);           // give it back
void AudLock_LockReadQueue(const char* szWho);    // take the read-queue lock
void AudLock_UnlockReadQueue(const char* szWho);  // give it back

// AudTable.c
u8             Emi_InitModule(void);
u8             Emi_InitSession(void);
void           Emi_ExitSession(void);
AudSource*     Emi_AddInstance(u8 nEntry, s16 nSound);
void           Emi_UpdInstance(u8 nEntry, u8 uMaskA, u8 uMaskB, u32* auStreams, f32 (*aPos)[3], u16 uMask);
void           Emi_DelInstance(u8 nEntry);
void           Emi_SetTrackVariation(u8 nEntry, u8 nTrack, u8 n);
void           Emi_SetTrackVarRange(u8 nEntry, u8 nTrack, u8 n);
void           Emi_SetTrackVarRangeTmpl(s16 nSound, u8 nTrack, u8 n);       // the track's nA
void           Emi_SetTrackStep(u8 nEntry, u8 nTrack, u8 n, int bCheck);
void           Emi_SetTrackAttenuation(u8 nEntry, u8 nTrack, f32 fVolume);
void           Emi_SetTrackPitchFactor(u8 nEntry, u8 nTrack, f32 fPitch);
void           Emi_CheckTemplate(AudSound* pSound, u16 n);
void           Emi_TrackCallback(AudSource* pSource, u8 nTrack, s32 n);

// HLAudMaster.c
AudSound*    Ses_GetEmitterTemplateFromID(s16 nSound);
f32          audfrac_Mul(f32 fVolume, f32 fCurve);
u8           Mas_InitSession(void);
void         Mas_ExitSession(void);

// hlaudmic.c
u8           Mic_InitSession(u8 nSession, u8 nSubsession, u8 nListeners);
void         Mic_ExitSession(void);

// AudReverb.c
void         Rvb_SetPreset(u8 nPreset);         // a listener's reverb preset (empty in this build)

// hlaudsession.c
u8*          Ses_GetStreamBuffer(u32 uSize, u8 nPlayList);                // the stream buffer
void         Ses_FreeStreamBuffer(u8* pBuffer, u32 uSize, u8 nPlayList);  // give it back
AudStream*   Ses_GetStreamFromPlayList(AudPlayList* pList, u16 nStream, u32* puLength);
void         Ses_TmplOvrTrackRvbMode(s16 nSound, u8 nTrack, u8 bOn);
u32          Ses_GetStreamBufferSize(u8 nPlayList);                       // the buffer size it needs
AudPlayList* Ses_GetStreamPlayList(u8 nPlayList);
// The sound data UStream.c loads (its 'SONO' chunks): where each header or sample block goes, and
// the step once it is in.
void*        Ses_AllocBankHdr(u32 uSize, u32 uMemory);                    // bank 0 or 1's header
void         Ses_ProcessArticulationData(u32 uMemory);
u32          Ses_AllocSampleAram(u32 uSize, u32 uMemory);                 // an ARAM address
void         Ses_ProcessSampleData(u32 uMemory);
AudStreamFile* Ses_AllocStreamFileHdr(u32 uSize);                         // NULL: one is loaded already
void         Ses_ProcessStreamFileHdr(void);

extern AudStreamQueue gAudStreamReadQueue;
extern s32 gSesStreamFile;      // the stream file (hlaudsession.c opens "/AudioStm_GC.sab")
extern u8 gStmLastReadId;       // the last read id handed out (hlaudtrackstm.c)
extern AudTrack* gStmDmaTrack;  // the track whose block is being DMA'd (hlaudtrackstm.c)

// hlaudtrack.c
void TrkRender3D(AudSource* pSource, AudTrack* pTrack, f32 fVolume);   // placed sounds
void TrkRenderStereo(AudSource* pSource, AudTrack* pTrack, f32 fVolume);   // the others
void InsertSortWorldPerf(AudTrack* pTrack);
u8   Trk_InitModule(void);
u8   Trk_InitSession(u8 nSession, u8 nSubsession);  // Ses_Init's; not read
void Trk_ExitSession(void);
void Trk_Cycle(void);
AudTrack* Trk_AllocPerf(AudSource* pSource, AudTrackTmpl* pTmpl, u8 nChannel, f32 fDistAttn);
s32  Trk_FreePerf(AudTrack* pTrack);
void Trk_UpdatePerf(AudSource* pSource, AudTrack* pTrack, AudTrackTmpl* pTmpl, u8 nChannel, u8 bOn,
                 u8 bOff, f32 fDistAttn);
void Trk_Start(AudTrack* pTrack);
void Trk_Stop(AudTrack* pTrack);
void Trk_StopAllVoices(AudTrack* pTrack, int bNow);
u8   Trk_Tick(AudTrack* pTrack);
void Trk_Step(AudTrack* pTrack, u8 n, u8 bCheck);
void Trk_SelectVariation(AudTrack* pTrack, u8 n);
void Trk_SetVariationRange(AudTrack* pTrack, u8 n);
void Trk_Render(AudTrack* pTrack);
void Trk_Check(AudTrackTmpl* pTmpl);
void Trk_VoiceEndCB(AudVoice* pVoice, int nReason);
void Seq_SelectVariation(AudTrack* pTrack, u8 n);
f32  Mas_GetSubmix(u8 nCurve);
u8   Mas_IsChanMuted(u8 nCurve);

// hlaudtrackseq.c
void AutoSelectVariation(AudTrack* pTrack);
void VoiceEndCB(AudVoice* pVoice, int nReason);
void CheckForStolenLoopers(AudTrack* pTrack);
void OnKeyOn(AudSeqEvent* pEvent, AudTrack* pTrack);
void ResetSequencerPerf(AudTrack* pTrack);
void SetVarCmdBounds(AudTrack* pTrack, AudSeqEvent** ppEvent, AudSeqEvent** ppEnd);
AudSeqTone* Ses_GetInstrumentTone(AudSeqBank* pBank, u8 nTone);
u8   Seq_InitModule(void);
void Seq_Init(AudTrack* pTrack);
void Seq_Exit(AudTrack* pTrack);
void Seq_Start(AudTrack* pTrack);
void Seq_Stop(AudTrack* pTrack);
u8   Seq_Tick(AudTrack* pTrack);
void Seq_Step(AudTrack* pTrack, u8 n, u8 bCheck);
void Seq_SetVariationRange(AudTrack* pTrack, u8 n);
void Seq_Check(AudTrackTmpl* pTmpl);
u32  Aud_RandomBelow(u32 nRange);           // a random number below nRange
u8   Ses_IsSessionZero(void);
f32  Mas_GetUpdateRateScale(void);

// hlaudtrackstm.c
u8   Stm_InitModule(void);
void Stm_Init(AudTrack* pTrack);
void Stm_Exit(AudTrack* pTrack);
void Stm_Start(AudTrack* pTrack);
void Stm_Stop(AudTrack* pTrack);
u8   Stm_Tick(AudTrack* pTrack);
void Stm_SetPlayList(AudTrack* pTrack, u8 nPlayList);
void Stm_SetStream(AudTrack* pTrack, u16 nStream, int nMode);

// hlaudvoice.c
u8   Voc_InitSession(void);
void Voc_ExitSession(void);
AudVoice* Voc_Alloc(AudVoiceRequest* pRequest);
void Voc_Start(AudVoice* pVoice, AudVoiceParams* pParams, u8 nVolume, f32 fPitch);
void Voc_StartStream(AudVoice* pVoice, u32 uLen, u32 nRate, u8 bLoud);
void Voc_Render(AudVoice* pVoice, AudVoiceParams* pParams);
void Voc_Pause(AudVoice* pVoice, u8 bPause);
void Voc_Stop(AudVoice* pVoice);    // let it end
void Voc_Delete(AudVoice* pVoice);  // stop it now
u8   Voc_CheckStreamHalfDone(AudVoice* pVoice, u32* puPos);

// hlaudemitter.c
void Aud_EmiTrkCB(u8 nId, u8 nBit, s32 n);
int  Aud_EmiInitSession(void);      // Aud_EmiDel on every instance in use, emitters emptied
void Aud_EmiDel(u8 nId);
// For every instance of an emitter: Aud_EmiSetTrackStatus, Aud_EmiSetTrackVarRange,
// Aud_EmiSetTrackStep, Aud_EmiSetTrackAttenuation.
void Aud_EmiAliasSetTrackStatus(s16 nEmitter, u8 nTrack, u8 bOn);
void Aud_EmiAliasSetTrackVarRange(s16 nEmitter, u8 nTrack, u8 n);
void Aud_EmiAliasSetTrackStep(s16 nEmitter, u8 nTrack, u8 n, int bCheck);
void Aud_EmiAliasSetTrackAttenuation(s16 nEmitter, u8 nTrack, f32 fVolume);

#endif
