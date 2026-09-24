#ifndef CORE_GBACABLE_H
#define CORE_GBACABLE_H

// gbacable.c: the Game Boy Advance link cable, one state block per controller port.

#include "game_types.h"
#include "platform.h"
#include "engine.h"

#define GBA_NUM_CHANNELS 4

// The link's context block (0x20 bytes): the GameCube sends its own and reads the GBA's back in
// 4-byte parts (fn_8012311C, "GbaOpen"; fn_80122FD8, "GbaReadContext").
typedef struct GbaContext {
    u8   b0;                    // 0x00  in the GBA's: 0 before the link is opened
    u8   nChan;                 // 0x01  the port
    u8   b2;                    // 0x02
    u8   b3;                    // 0x03  1 once a context is made; 0 in the GBA's: none yet
    u32  uStart;                // 0x04  the tick the link code started at (lbl_80282560)
    u32  uTick;                 // 0x08  the tick the context was made at
    s32  nC;                    // 0x0C
    u8   unk10[0x20 - 0x10];
} GbaContext;

// One port's link state (lbl_80260E18[4], 0x78 bytes each). Only what the cleaned code uses.
typedef struct GbaChannel {
    s32  n0;                    // 0x00  cleared at start and when a command fails; 2 once linked
    u8   uStatus;               // 0x04  the port's status byte (fn_80176200 reads it)
    u8   unk5[0x8 - 0x5];
    GbaContext sent;            // 0x08  the GameCube's context
    GbaContext got;             // 0x28  the GBA's, as read
    u32  u48;                   // 0x48  the tick of the context in use
    s32  n4C;                   // 0x4C  the word the GBA answers the handshake with
    s32  n50;                   // 0x50
    u32  uKey;                  // 0x54  0x40 + port, two port bits and their check byte (fn_801228E0)
    u32  u58;                   // 0x58  from the GBA: four d-pad bits (20-23) and a check byte
    s32  n5C;                   // 0x5C  what SIProbe finds on the port (0x40000: a GBA); 0x40 at start
    u8   unk60[0x64 - 0x60];
    s32  n64;                   // 0x64  set when u58 is new
    s32  n68;                   // 0x68
    s32  n6C;                   // 0x6C
    s32  n70;                   // 0x70
    u8   unk74[0x78 - 0x74];
} GbaChannel;
LAYOUT_ASSERT(GbaChannel, 0x78);

extern GbaChannel lbl_80260E18[GBA_NUM_CHANNELS];
extern DVDDiskID* lbl_8028255C;  // the disc's ID: its game code goes to the GBA in the handshake
extern u32 lbl_80282560;        // the tick the link code started at (fn_80123FF8)
extern PadStatus lbl_80260FF8[GBA_NUM_CHANNELS];   // the pads as fn_80123E34 reads them
extern const u32 lbl_80184E30[GBA_NUM_CHANNELS];   // each port's PADReset bit (0x80000000 >> port)

// Time-base ticks in a millisecond (the time base runs at a quarter of the bus clock, which the OS
// keeps at 0x800000F8). A GBA command waits 100 ms for the GBA.
#define GBA_TICKS_PER_MS  (*(u32*)0x800000F8 / 4 / 1000)
#define GBA_TIMEOUT_TICKS (GBA_TICKS_PER_MS * 100)
extern s32 lbl_80281984;        // the port being worked on (-1: none, fn_8012422C)

#endif
