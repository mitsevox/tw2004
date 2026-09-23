#ifndef CORE_GBACABLE_H
#define CORE_GBACABLE_H

// gbacable.c: the Game Boy Advance link cable, one state block per controller port.

#include "game_types.h"
#include "platform.h"

#define GBA_NUM_CHANNELS 4

// One port's link state (lbl_80260E18[4], 0x78 bytes each). Only what the cleaned code uses.
typedef struct GbaChannel {
    s32  n0;                    // 0x00  cleared at start and when a command fails
    u8   uStatus;               // 0x04  the port's status byte (fn_80176200 reads it)
    u8   unk5[0x4C - 0x5];
    s32  n4C;                   // 0x4C
    s32  n50;                   // 0x50
    u32  uKey;                  // 0x54  0x40 + port, two port bits and their check byte (fn_801228E0)
    u8   unk58[0x5C - 0x58];
    s32  n5C;                   // 0x5C  0x40 at start
    u8   unk60[0x64 - 0x60];
    s32  n64;                   // 0x64
    s32  n68;                   // 0x68
    s32  n6C;                   // 0x6C
    s32  n70;                   // 0x70
    u8   unk74[0x78 - 0x74];
} GbaChannel;
LAYOUT_ASSERT(GbaChannel, 0x78);

extern GbaChannel lbl_80260E18[GBA_NUM_CHANNELS];
extern s32 lbl_80281984;        // the port being worked on (-1: none, fn_8012422C)

#endif
