// sitdev.h (our name): the data SitDevFile.c shares with the code around 0x80067560..0x80067B80
// (which frees its scripts and sets its values) and 0x800BCD68.

#ifndef SITDEV_H
#define SITDEV_H

#include "engine.h"
#include "endian.h"
#include "ball.h"

#define SITDEV_NUM_VALUES 96

// An event queued for the scripts (fn_80067710 adds them, fn_8006795C runs and clears them).
typedef struct SitDevEvent {
    s32   nPlayer;              // 0x00
    u8    nEvent;               // 0x04  event.c's event number
    u8    unk5[3];
} SitDevEvent;

// The block lbl_802811B8 points at (lbl_801D5AB0, 0x140 bytes).
typedef struct SitDevData {
    u16   aValue[SITDEV_NUM_VALUES];            // 0x000  set through fn_80067B1C
    u32   aSetBits[SITDEV_NUM_VALUES / 32];     // 0x0C0  bit n: aValue[n] has been set
    void* pCC;                                  // 0x0CC  SitDev_LoadScripts' argument; freed by fn_8006765C
    void* pD0;                                  // 0x0D0  freed by fn_8006765C when set
    u8*   pD4;                                  // 0x0D4  allocated by SitDev_LoadScripts, one byte per
                                                //        SitDevScripts.n10; freed by fn_8006765C
    u8    abPlayed[14];                         // 0x0D8  per kind of action: one has played already
    u8    unkE6[2];
    struct SitDevEntry8* pE8;                   // 0x0E8  the last line played (fn_800BD580 kind 1)
    SitDevEvent aEvents[10];                    // 0x0EC  this frame's events, n13C of them
    s32   n13C;                                 // 0x13C  cleared with the block by fn_80067608
} SitDevData;
LAYOUT_ASSERT(SitDevData, 0x140);

extern SitDevData* lbl_802811B8;    // 0x802811B8 (.sdata): &lbl_801D5AB0

// A halfword the loader rewrites (fn_800BB52C): on disc its two bit-fields are in the other bit
// order, so it reads the raw value and stores its low 11 bits and its top 5 bits back as fields.
typedef union SitDevBits {
    u16 uRaw;
    struct {
        u16 n11 : 11;           // bits 15..5
        u16 n5 : 5;             // bits 4..0
    } s;
} SitDevBits;

// An entry of the scripts' first table (SitDevScripts.p14, 0x30 bytes; 0x80067710 runs them).
// Its conditions (fn_800BB7AC): for each bit n set in auTests, in order, test k compares
// SitDevData.aValue[n] with aArg[k] by aOp[k].
typedef struct SitDevEntry {
    u8         n0;              // 0x00  its SitDevData.pD4 byte (runs once); 0: none
    u8         nEvent;          // 0x01  the queued event it waits for; 0: any
    SitDevBits b2;              // 0x02
    u32        auTests[SITDEV_NUM_VALUES / 32];   // 0x04
    u8         aOp[8];          // 0x10  0 always true, 1 ==, 2 !=, 3 <, 4 >, 5 any common bit
    u16        aArg[8];         // 0x18
    u16        aActions[4];     // 0x28  SitDevScripts.p18 entries to try (fn_800BCD68), 0xFFF0 ends
} SitDevEntry;

// An entry of the scripts' second table (SitDevScripts.p18, 0x68 bytes): what a script does.
typedef struct SitDevAction {
    u8         nKind;           // 0x00  its SitDevData.abPlayed byte; kinds 1 and 2 are commentary
    u8         nChance;         // 0x01  percent
    u8         unk2;
    u8         bSound;          // 0x03  nonzero: play a sound from aList (fn_800BCE70), else run
                                //       p1C entries (fn_800BCF84)
    u16        aList[50];       // 0x04  a deck (fn_800BB218..fn_800BB334); 0xFFF0 ends
} SitDevAction;

// An entry of the scripts' third table (SitDevScripts.p1C, 8 bytes): one thing to do.
typedef struct SitDevEntry8 {
    u8         nKind;           // 0x00  fn_800BD580's switch; also its SitDevData.abPlayed byte
    u8         unk1;
    SitDevBits b2;              // 0x02
    u32        n4;              // 0x04  its argument (a sound, a music, ...)
} SitDevEntry8;

// The situation scripts' header (lbl_80282208): the block whose address is the first word of
// SitDev_LoadScripts' argument. fn_800BB4B4 turns the offsets at 0x14..0x20 into pointers.
typedef struct SitDevScripts {
    u32   nEntries;             // 0x00  entries at p14 (0x80067710)
    u32   n04;                  // 0x04  entries at p18
    u32   n08;                  // 0x08  entries at p1C
    u32   n0C;                  // 0x0C  words at p20
    u32   n10;                  // 0x10  bytes in SitDevData.pD4 (fn_800BD74C clears them)
    SitDevEntry*  p14;          // 0x14
    SitDevAction* p18;          // 0x18  (fn_800BCD68)
    SitDevEntry8* p1C;          // 0x1C
    u8*   p20;                  // 0x20
} SitDevScripts;

extern SitDevScripts* lbl_80282208; // 0x80282208 (.sbss), NULL until the scripts are loaded
extern s32 lbl_80282210;            // 0x80282210 (.sbss): entries in use in lbl_801FA1C0 (fn_800BB6DC)

// The byte-swap layouts of the header and the p14, p18 and p1C entries (fn_8001F08C).
extern SwapField lbl_80191168[9];
extern SwapField lbl_801911B0[7];
extern SwapField lbl_801911E8[5];
extern SwapField lbl_80191210[4];

// A situation zone, from chunk 5 of the hole's data (fn_800BB6DC): an outline (with its
// net.nNumNodes nodes), and then the zone's bits (a u32 right after the last node).
typedef struct SitDevZone {
    TNetwork net;               // 0x0
} SitDevZone;

extern SitDevZone* lbl_801FA1C0[10];    // the hole's zones
extern s32 lbl_80282210;                // how many

extern u8 lbl_80281E28;             // cleared by fn_800BCD68 when lbl_80281E29 is set and nothing played
extern u8 lbl_80281E29;             // cleared by fn_800BCD68 after every run

// Per value: nonzero when the scripts compare it as signed (fn_800BB8A8).
extern u8 lbl_80193188[88];

// Per game mode: the bit fn_800BB3F8 returns for it, -1 for none.
extern s32 lbl_801910F8[28];

void fn_800BD74C(void);             // clear SitDevData.pD4
extern s32 lbl_801FA198[5];         // per player; cleared by fn_800BB1C0
extern s32 lbl_801FA1AC[5];         // per player; 1: fn_800BB1F8 is true

// Store uValue in pValues[nIndex] and set bit nIndex of pSetBits.
void fn_80067B1C(u16* pValues, int nIndex, u16 uValue, u32* pSetBits);

void fn_80067710(int nPlayer, int a, u8 nEvent);    // event.c's handlers call it for most events

// SitDevFile.c
void fn_800BB0C8(void);
void fn_800BB1A8(struct Ball* pBall);
void SitDev_LoadScripts(SitDevScripts** ppScripts); // the 'sscr' stream handler
void fn_800BB6DC(u8* pChunk);                   // the course loader for chunk 5

#endif
