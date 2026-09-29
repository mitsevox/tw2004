#ifndef CORE_GBACABLE_H
#define CORE_GBACABLE_H

// gbacable.c: the Game Boy Advance link cable, one state block per controller port.

#include "game_types.h"
#include "platform.h"
#include "engine.h"

#define GBA_NUM_CHANNELS 4

// The link's context block (0x20 bytes): the GameCube sends its own and reads the GBA's back in
// 4-byte parts (GbaOpen, "GbaOpen"; GbaReadContext, "GbaReadContext").
typedef struct GbaContext {
    u8   b0;                    // 0x00  in the GBA's: 0 before the link is opened
    u8   nChan;                 // 0x01  the port
    u8   b2;                    // 0x02
    u8   b3;                    // 0x03  1 once a context is made; 0 in the GBA's: none yet
    u32  uStart;                // 0x04  the tick the link code started at (gGbaInitTick)
    u32  uTick;                 // 0x08  the tick the context was made at
    s32  nC;                    // 0x0C
    u8   unk10[0x20 - 0x10];
} GbaContext;

// One port's link state (gGbaChannels[4], 0x78 bytes each). Only what the cleaned code uses.
typedef struct GbaChannel {
    s32  nStep;                 // 0x00  the port's link step (Gba_StepPorts): 0 wait for a GBA,
                                //       1 handshake and open, 2 linked, 3 send our context, 4 the
                                //       contexts differ; back to 0 when a command fails
    u8   uStatus;               // 0x04  the GBA library's status byte for the port (every
                                //       GBAGetStatus / GBARead / GBAWrite / GBAReset writes it)
    u8   unk5[0x8 - 0x5];
    GbaContext sent;            // 0x08  the GameCube's context
    GbaContext got;             // 0x28  the GBA's, as read
    u32  uContextTick;          // 0x48  the tick of the context in use
    s32  nHandshakeCode;        // 0x4C  the word the GBA answers the handshake with
    s32  n50;                   // 0x50  only cleared
    u32  uKey;                  // 0x54  0x40 + port, two port bits and their check byte (Gba_CalcCheckByte)
    u32  uPadWord;              // 0x58  from the GBA: four d-pad bits (20-23) and a check byte
    u32  uProbe;                // 0x5C  what SIProbe finds on the port (0x40000 SI_GBA: a GBA; 8 or
                                //       0x40: an SI error); 0x40 at start
    u8   unk60[0x64 - 0x60];
    s32  bNewPadWord;           // 0x64  set when uPadWord is new
    u32  uGbaCash;              // 0x68  the cash the GBA holds (GbaCommunication, "FROMGBA_CASHDATA")
    s32  nCashToMove;           // 0x6C  cash to move between GBA and GameCube (GbaCommunication)
    s32  nGbaStat;              // 0x70  the GBA's answer to a stats request (GbaCommunication)
    s32  nUnlockMask;           // 0x74  the GBA's unlock mask (GbaCommunication,
                                //       "FROMGBA_UNLOCKMASK"); nothing reads it
} GbaChannel;
LAYOUT_ASSERT(GbaChannel, 0x78);

extern GbaChannel gGbaChannels[GBA_NUM_CHANNELS];
extern DVDDiskID* gGbaDiscID;  // the disc's ID: its game code goes to the GBA in the handshake
extern u32 gGbaInitTick;        // the tick the link code started at (Gba_Init)
extern PadStatus gGbaPads[GBA_NUM_CHANNELS];   // the pads as Gba_ReadPads reads them
extern const u32 gGbaPadResetBits[GBA_NUM_CHANNELS];   // each port's PADReset bit (0x80000000 >> port)

// Time-base ticks in a millisecond (the time base runs at a quarter of the bus clock, which the OS
// keeps at 0x800000F8). A GBA command waits 100 ms for the GBA.
#define GBA_TICKS_PER_MS  (*(u32*)0x800000F8 / 4 / 1000)
#define GBA_TIMEOUT_TICKS (GBA_TICKS_PER_MS * 100)
extern s32 gGbaPortInUse;        // the port being worked on (-1: none, Gba_ClearPortInUse)
extern u32 gGbaSearchDelayFrames;        // frames left before the link is first polled (Gba_UpdateLinkState)
extern u32 gGbaSearchStartTick;        // OSGetTick() at link state 0; state 1 gives up 4 s after it

// The link's setup, poll and state (gomainloop.c runs them) and the menus' getters and setters
// (FE_MessageTable.c's GM_vGba* handlers).
void Gba_Init(void);                    // start the link code (GM_vGbaStartLink)
void Gba_PollLink(void);                // read the pads and step the ports; the reset button resets
void Gba_SetState(s32 v);               // set the link state (gGbaLinkState; Gba_SetState lists them)
s32  Gba_GetState(void);
void Gba_UpdateLinkState(void);         // run the link state, once a frame
void Gba_StepPorts(s32 nCmd, s32 nArg); // move each port's link on one step, a linked one running nCmd
void Gba_MarkUnlocksGranted(void);      // set gGbaUnlocksGranted (nothing reads it)
void Gba_UnlockProfileRewards(void);    // the link's unlocks in the current profile
s32  Gba_GetCashToMove(void);           // } nCashToMove of the port in use
void Gba_SetCashToMove(s32 n);          // }
s32  Gba_GetCashOnGba(void);            // uGbaCash of the port in use
s32  Gba_GetGbaStat(void);              // nGbaStat of the port in use
s32  Gba_IsReadPending(void);           // } gGbaReadPending: only ever 0 in this build
void Gba_SetReadPending(s32 v);         // }
void Gba_SetUndoTransfer(s32 v);        // } gGbaUndoTransfer: only ever 0 in this build
s32  Gba_IsUndoTransfer(void);          // }

#endif
