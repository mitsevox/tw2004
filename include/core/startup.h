// startup.h (our name): the types and data of startUp.c, the boot-time systems: the sound voices
// (lbl_802820E8, 50 of them, each wrapping one hardware voice) and the audio-RAM (ARAM) heap.
//
// port: almost everything here drives the GameCube's audio hardware through Nintendo's libraries
// (AX voices, the MIX mixer, ARAM DMA, the CPU data cache). A PC build keeps the voice table and
// its state machine and replaces those calls with its own sound back end.

#ifndef CORE_STARTUP_H
#define CORE_STARTUP_H

#include "engine.h"

// ---- the GameCube libraries (port: GameCube only) ---------------------------------------------

// A hardware voice (AX): only the fields this code touches.
typedef struct AXVPB {
    u8   unk0[0x1C];
    u32  u1C;                   // 0x1C   which parts of the voice to send to the hardware
    u8   unk20[0x146 - 0x20];
    u16  n146;                  // 0x146  nonzero while the voice plays
    u8   unk148[0x1A6 - 0x148];
    u16  n1A6;                  // 0x1A6  Voice 0x08..0x53 is copied here when it starts
    u8   unk1A8[0x1AE - 0x1A8];
    u16  n1AE;                  // 0x1AE  } where the sound ends, in 4-bit units, as two halves
    u16  n1B0;                  // 0x1B0  }
    u16  n1B2;                  // 0x1B2  } where the voice is playing, in 4-bit units, as two
    u16  n1B4;                  // 0x1B4  } halves (Voice.u14 is copied here when it starts)
} AXVPB;

int    OSDisableInterrupts(void);       // returns whether interrupts were on
int    OSRestoreInterrupts(int bEnabled);
void   DCStoreRange(void* p, u32 uLen);         // write the CPU cache back to memory
void   DCInvalidateRange(void* p, u32 uLen);    // drop the CPU cache over a range
AXVPB* AXAcquireVoice(u32 uPriority, void (*pfnDropped)(void* pVpb), u32 uUser);
void   AXSetVoiceSrcType(AXVPB* pVpb, u32 uType);
void   AXSetVoiceState(AXVPB* pVpb, u16 uState);   // 0 stopped, 1 running

// MIX: set a new voice's input, aux A and aux B levels (dB x 10), pan, surround pan and fader.
void   fn_80145B24(AXVPB* pVpb, u32 uMode, int nInput, int nAuxA, int nAuxB, int nPan, int nSPan,
                   int nFader);

// Keeps a global the code never uses through the linker's dead-stripping (startUp.c's data has
// three). `#pragma force_active` does not mark uninitialised data; this does.
#ifdef __MWERKS__
#define KEEP_UNUSED __declspec(export)
#else
#define KEEP_UNUSED
#endif

// ---- the voice table --------------------------------------------------------------------------

#define NUM_VOICES 50
#define VOLUME_MIN (-904)               // -90.4 dB: silent

// A voice's state flags (the word at 0x4). The bSet... flags ask the mixer callback (fn_800AF324)
// to pass a changed setting on to the hardware on its next pass.
typedef struct VoiceFlags {
    u32  nState     : 3;        // 0x4  0, 1 set up (fn_800AFDC8), 2 start asked for (fn_800AFBD8),
                                //      3 playing, 6 released (fn_800AFBD8)
    u32  nEnvStage  : 2;        //      the envelope's stage: 0 attack, 1 decay, 2 sustain, 3 release
    u32  bSetInput  : 1;        //      n58 changed
    u32  bSetPan    : 1;        //      nPan changed
    u32  bSetSPan   : 1;        //      nSPan changed
    u32  bSetSrc    : 1;        // 0x5  u40 changed
    u32  bSetAuxA   : 1;        //      nAuxA changed
    u32  bSetAdpcm  : 1;        //      a18..n3E changed
    u32  bSetLoop   : 1;        //      n50..n54 changed
    u32  bStart     : 1;        //      start playing (fn_800AFBD8)
    u32  bRelease   : 1;        //      go to the envelope's release (fn_800AFBD8)
    u32  n5_03      : 2;        //      passes left before a pause mutes, then stops, the voice
    u32  bResume    : 1;        // 0x6  undo a pause (fn_800AFCBC)
    u32  b6_40      : 1;
    u32  b6_20      : 1;        //      aux A (reverb) on
    u32  b6_10      : 1;
    u32  bLost      : 1;        //      the hardware took the voice away (fn_800AF93C)
    u32  n6_7F8     : 8;        //      passes to wait before asking for it back
    u32  unk7       : 3;
} VoiceFlags;

typedef union VoiceFlagWord {
    u32        u;
    VoiceFlags b;
} VoiceFlagWord;

// A voice's volume envelope (the word at 0x60). The level (n64, 0..0xFFFF) climbs by
// nAttack * 16 each pass to full, falls by nDecay * 0x1000 to nSustain * 0x1000 + 0xFFF, and after
// a release falls by nRelease * 16 to silence (fn_800AF324).
typedef struct VoiceEnvelope {
    u32  nAttack    : 12;       // 0x60
    u32  nDecay     : 4;        // 0x61
    u32  nSustain   : 4;        // 0x62
    u32  nRelease   : 12;
} VoiceEnvelope;

// A sound's header (0x38 bytes): where its data sits in ARAM (in 4-bit units) and its ADPCM
// decoder state, as fn_800AFDC8 copies it into a voice.
typedef struct SoundHeader {
    u32  u0;                    // 0x00  -> Voice.u14
    u32  u4;                    // 0x04  -> Voice.u10
    u32  u8;                    // 0x08  -> Voice.uC
    u32  uC;                    // 0x0C  -> Voice.n8
    u32  a10[8];                // 0x10  -> Voice.a18
    u16  n30;                   // 0x30  -> Voice.n3A
    u16  n32;                   // 0x32  -> Voice.n50
    u16  n34;                   // 0x34  -> Voice.n52
    u16  n36;                   // 0x36  -> Voice.n54
} SoundHeader;
LAYOUT_ASSERT(SoundHeader, 0x38);

// 0x08..0x53 is copied word by word into the hardware voice at 0x1A6 when the voice starts
// (fn_800AF324).
typedef struct Voice {
    AXVPB*        pVpb;         // 0x00
    VoiceFlagWord flags;        // 0x04
    u16           n8;           // 0x08
    u16           nA;           // 0x0A
    u32           uC;           // 0x0C
    u32           u10;          // 0x10
    u32           u14;          // 0x14  the start, in 4-bit units (fn_800AFD8C halves it)
    u32           a18[8];       // 0x18  the ADPCM decoder's coefficients (copied word by word)
    u16           n38;          // 0x38
    u16           n3A;          // 0x3A  the first ADPCM frame's header (predictor and scale)
    u16           n3C;          // 0x3C
    u16           n3E;          // 0x3E
    u32           u40;          // 0x40
    u8            unk44[0x50 - 0x44];
    u16           n50;          // 0x50  the frame header to use when it loops
    u16           n52;          // 0x52
    u16           n54;          // 0x54
    s16           n56;          // 0x56  volume, 0..0x3FFF
    s16           n58;          // 0x58  n56 in dB x 10 (fn_800AFF9C)
    s16           nAuxA;        // 0x5A  aux A level, dB x 10
    u8            nPan;         // 0x5C  0 left .. 127 right
    u8            nSPan;        // 0x5D  surround pan
    u8            unk5E[0x60 - 0x5E];
    VoiceEnvelope env;          // 0x60
    u16           n64;          // 0x64  the envelope's level
    u16           n66;          // 0x66  the level the decay stops at
} Voice;
LAYOUT_ASSERT(Voice, 0x68);

extern Voice* lbl_802820E8;     // NUM_VOICES entries

// One 0x8000-byte chunk of a streamed sound (fn_800AB72C): the decoder's coefficients, then the
// ADPCM data it DMAs into one half of the voice's ARAM buffer. The data's first byte is its first
// frame's header (predictor and scale).
typedef struct StreamChunk {
    u32  a0[8];                 // 0x000  -> Voice.a18
    u8   unk20[0x100 - 0x20];
    u8   aData[0x7F00];         // 0x100
} StreamChunk;
LAYOUT_ASSERT(StreamChunk, 0x8000);

// One block of a movie's stereo sound (fn_800A8AD4): each channel's coefficients, then each
// channel's 0x2FC0 bytes of ADPCM data.
typedef struct MovieSoundBlock {
    u8   unk0[0x1A];
    u16  a1A[16];               // 0x1A  the left channel's coefficients -> Voice.a18
    u8   unk3A[0x3C - 0x3A];
    u16  a3C[16];               // 0x3C  the right channel's
    u8   unk5C[0x60 - 0x5C];
    u8   aDataL[0x2FC0];        // 0x60
    u8   aDataR[0x2FC0];        // 0x3020
} MovieSoundBlock;

// ---- the ARAM heap ----------------------------------------------------------------------------

#define ARAM_HEAP_SIZE 0x3FC000
#define ARAM_ZERO_SIZE 0x400            // the silent block at the start of the heap (fn_800B0568)

extern u32   lbl_802820F0;      // which of the eight 0xFE00-byte blocks at lbl_80282108 are taken
extern s32   lbl_802820F4;      // how many are taken
extern void* lbl_802820F8;      // the ARAM heap
extern u32   lbl_802820FC;      // its ARAM address
extern u32   lbl_80282100;      // the silent block's ARAM address
extern void* lbl_80282104;      // the zeroes DMA'd into it, freed once the DMA is done
extern u32   lbl_80282108;      // the blocks' ARAM address
extern void* lbl_8028210C;      // the heap's bookkeeping (0x2A4 bytes)
extern u8    lbl_80282110;      // set when that DMA is done

// DMA nLen bytes from main memory to ARAM; pfnDone is called when it is done. Returns 1.
int fn_800B044C(u32 uAram, void* pSrc, int nLen, void (*pfnDone)(void), int n);

// ---- the built-in sounds ----------------------------------------------------------------------

// One of the two sounds in startUp.c's own data, copied to ARAM at boot (fn_800B07A0).
typedef struct BootSound {
    void*       pData;          // 0x00  in main memory
    u32         uSize;          // 0x04
    u32         uAram;          // 0x08  where fn_800B07A0 put it
    u32         uC;             // 0x0C  -> Voice.u40
    SoundHeader hdr;            // 0x10  u0 and u4 count from uAram until fn_800B07A0 rebases them
} BootSound;
LAYOUT_ASSERT(BootSound, 0x48);

extern BootSound lbl_8018FE98[2];
extern u8    lbl_8018F040[0x600];   // } the two sounds' ADPCM data: game data, so it stays in the
extern u8    lbl_8018F640[0x858];   // } DOL's own data (split before startUp.c's)
extern u16   lbl_80282118;      // the next voice fn_800B0858 plays on

// ---- the rest ---------------------------------------------------------------------------------

// A memory card's state as the card code (MC_Gc.c) keeps it, one per slot and entry; fn_8009F7F4
// copies one out. Only the fields fn_800B09C8 reads.
typedef struct CardInfo {
    u32  uFlags;                // 0x00  0x02, 0x08, 0x10, 0x20, 0x40, 0x80: see fn_800B09C8
    s32  n4;                    // 0x04  compared with what the save kinds 0 and 3 need
    u8   unk8[0x84 - 0x8];
    s32  n84;                   // 0x84  compared with fn_8009D3DC + fn_8009D50C
    s32  n88;                   // 0x88  the sector size (fn_800B09C8 wants 0x2000)
    u8   unk8C[0x98 - 0x8C];
} CardInfo;
LAYOUT_ASSERT(CardInfo, 0x98);

// A card slot and entry, as the save-kind functions (fn_80084FB4) take them. 12 bytes: the size
// fn_800B09C8's stack frame proves; the last word is never read there.
typedef struct CardPos {
    s32  nSlot;                 // 0x0
    s32  n;                     // 0x4
    u8   unk8[4];
} CardPos;
LAYOUT_ASSERT(CardPos, 0xC);

// The memory-card status table: for each of the two card slots, lbl_80282138[slot] entries
// (always 1), each with the status fn_800B09C8 read (lbl_80282150), the status last reported
// (lbl_80282148) and whether it has been reported (lbl_80282140).
#define NUM_CARD_SLOTS 2
extern s32   lbl_80282138[NUM_CARD_SLOTS];
extern s32   lbl_80282140[NUM_CARD_SLOTS][1];
extern s32   lbl_80282148[NUM_CARD_SLOTS][1];
extern s32   lbl_80282150[NUM_CARD_SLOTS][1];
extern s32   lbl_80281498;      // } the entry and slot the reports reached; -1 to start again
extern s32   lbl_8028149C;      // }
extern u8    lbl_802814A0;
extern u8    lbl_80282120;      // fn_800B0960 keeps a memory-card result here
extern s32   lbl_80282124;      // how many 'LEGL' objects fn_800B166C has kept (it keeps two)
extern u32   lbl_80282128;      // the second one's size
extern u32   lbl_8028212C;      // the first one's size
extern void* lbl_80282130;      // the second one's copy
extern void* lbl_80282134;      // the first one's copy

#endif
