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

typedef struct AXVPB AXVPB;             // a hardware voice (AX); only its address is used here

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

// ---- the voice table --------------------------------------------------------------------------

#define NUM_VOICES 50
#define VOLUME_MIN (-904)               // -90.4 dB: silent

// A voice's state flags (the word at 0x4). The bSet... flags ask the mixer callback (fn_800AF324)
// to pass a changed setting on to the hardware on its next pass.
typedef struct VoiceFlags {
    u32  n4_E0      : 3;        // 0x4
    u32  n4_18      : 2;
    u32  b4_04      : 1;
    u32  bSetPan    : 1;        //      nPan changed
    u32  bSetSPan   : 1;        //      nSPan changed
    u32  bSetSrc    : 1;        // 0x5  u40 changed
    u32  bSetAuxA   : 1;        //      nAuxA changed
    u32  b5_20      : 1;
    u32  b5_10      : 1;
    u32  b5_08      : 1;
    u32  b5_04      : 1;
    u32  n5_03      : 2;
    u32  b6_80      : 1;        // 0x6
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

typedef struct Voice {
    AXVPB*        pVpb;         // 0x00
    VoiceFlagWord flags;        // 0x04
    u8            unk8[0x40 - 0x8];
    u32           u40;          // 0x40
    u8            unk44[0x56 - 0x44];
    s16           n56;          // 0x56  volume, 0..0x3FFF
    s16           n58;          // 0x58  n56 in dB x 10 (fn_800AFF9C)
    s16           nAuxA;        // 0x5A  aux A level, dB x 10
    u8            nPan;         // 0x5C  0 left .. 127 right
    u8            nSPan;        // 0x5D  surround pan
    u8            unk5E[0x68 - 0x5E];
} Voice;
LAYOUT_ASSERT(Voice, 0x68);

extern Voice* lbl_802820E8;     // NUM_VOICES entries

// ---- the ARAM heap ----------------------------------------------------------------------------

extern u32   lbl_802820F0;      // which of the eight 0xFE00-byte blocks at lbl_80282108 are taken
extern s32   lbl_802820F4;      // how many are taken
extern void* lbl_802820F8;      // the ARAM heap
extern void* lbl_80282104;      // the source buffer of fn_800B0568's DMA, freed once it is done
extern u32   lbl_80282108;      // the blocks' ARAM address
extern u8    lbl_80282110;      // set when that DMA is done

// ---- the rest ---------------------------------------------------------------------------------

extern s32   lbl_80281498;      // } where fn_800B13FC's search stopped; -1 to start again
extern s32   lbl_8028149C;      // }
extern u8    lbl_802814A0;

#endif
