// memcard.h (our name): the memory card. MC.c is the game side (finding a card, loading and saving
// the save profile, the messages for no card, card full, damaged card...); MC_Gc.c is the GameCube
// layer under it, over the CARD library. Both files are EA's (asserts).
//
// A card is addressed as (nPort, nSlot), EA's PlayStation 2 scheme: nPort is the GameCube's card
// slot (0 = slot A, 1 = slot B; the CARD library's channel) and nSlot a multitap slot, always 0 here
// (one per port: fn_8009CD10 sets lbl_80282000[nPort] to 1 and lbl_80282008[nPort], "has a
// multitap", to 0).
//
// port: MC_Gc.c is the layer a PC build replaces with file I/O; the functions below it marked
// "GameCube only" are the CARD library. The save data is written and read as raw big-endian bytes
// (docs/format-byteorder.md).

#ifndef CORE_MEMCARD_H
#define CORE_MEMCARD_H

#include "engine.h"

#define MC_NUM_PORTS 2              // GameCube card slots A and B
#define MC_NUM_SLOTS 1              // multitap slots per port

// What MC_Gc.c knows about the card in one port and slot (0x98 bytes).
typedef struct MCCardState {
    u32  uFlags;                // 0x00  MC_CARD_* below
    s32  nFreeBlocks;           // 0x04  free space, in whole sectors (CARDFreeBlocks' bytes, rounded up)
    u8   unk8[0x84 - 0x8];
    s32  nFreeFiles;            // 0x84  free directory entries (CARDFreeBlocks)
    s32  nSectorSize;           // 0x88  CARDProbeEx
    u32  u8C;                   // 0x8C  CARDGetXferredBytes when an operation starts (fn_8009CB9C)
    s32  nMemSize;              // 0x90  the card's size (CARDProbeEx)
    u8   b94;                   // 0x94  set when a format fails with an I/O error; a mount clears
                                //       it, but puts it back when the card turns out broken
    u8   unk95[3];
} MCCardState;
LAYOUT_ASSERT(MCCardState, 0x98);

// MCCardState.uFlags. Each bit is set or cleared where the CARD library returns the result named.
#define MC_CARD_PRESENT     0x02    // cleared on CARD_RESULT_NOCARD
#define MC_CARD_MOUNTED     0x04    // set by a mount (fn_8009D74C), cleared by an unmount (fn_8009DBAC)
#define MC_CARD_WRONGDEVICE 0x10    // CARD_RESULT_WRONGDEVICE: not a memory card
#define MC_CARD_IOERROR     0x20    // CARD_RESULT_IOERROR
#define MC_CARD_BROKEN      0x40    // CARD_RESULT_BROKEN
#define MC_CARD_ENCODING    0x80    // CARD_RESULT_ENCODING, or a card with a non-ASCII encoding

// The save file's names on the card: EA kept the PlayStation 2 names (SLUS-20757 is the PS2
// release), the second a backup copy.
#define MC_FILE_NAME    "BASLUS-20757"
#define MC_BACKUP_NAME  "BU_BASLUS-20757"
#define MC_DIR_NAME     "../BASLUS-20757"   // the PS2 save directory; here it stands for the file

#define MC_BUFFER_SIZE  0x50000     // one save file image in memory

extern MCCardState lbl_801F1510[MC_NUM_PORTS][MC_NUM_SLOTS];
extern u8    lbl_80281FD0[MC_NUM_PORTS];    // the card in this port had an I/O error: its
                                            // operations fail with -25 until the next start
extern s32   lbl_80282000[MC_NUM_PORTS];    // slots per port (1)
extern u8    lbl_80282008[MC_NUM_PORTS];    // the port has a multitap (0)

// Two save file images (MC_BUFFER_SIZE each), each reached through three pointers. MC_Gc.c
// allocates them, or borrows skalib's scratch memory, and parks them in ARAM between uses.
extern void* lbl_80281FD8;      // } the second image
extern void* lbl_80281FDC;      // }
extern void* lbl_80281FE0;      // }
extern void* lbl_80281FE4;      // } the first image
extern void* lbl_80281FE8;      // }
extern void* lbl_80281FEC;      // }
extern u32   lbl_80281FC0;      // the size parked in ARAM (MC_BUFFER_SIZE + 0x20)
extern u32   lbl_80281FC4;      // the ARAM address they are parked at (0: none yet)
extern UStreamObject* lbl_80281FB8;     // the 'MCI ' object (fn_8009EB30)
extern UStreamObject* lbl_80281FBC;     // the 'MCB ' object (fn_8009EB38)

// ---- MC_Gc.c ------------------------------------------------------------------------------------

void fn_8009CC00(void);
void fn_8009CC88(void);
void fn_8009CD10(void);
void fn_8009CD7C(void);
s32  fn_8009D614(s32 nPort, s32 nSlot, const char* pName);
s32  fn_8009D74C(s32 nPort, s32 nSlot);     // mount the card; 0, or -22 when it was mounted already
s32  fn_8009DBAC(s32 nPort, s32 nSlot);     // unmount it
void fn_8009DCEC(s32 nPort, s32 nSlot);
s32  fn_8009DD44(s32 nPort, s32 nSlot, const char* pName);
void fn_8009EA98(void);
void fn_8009EAF0(void);
u32  fn_8009EF90(void);
s32  fn_8009F5E4(s32 nPort, s32 nSlot, const char* pName);    // delete the save file

// ---- between MC_Gc.c and MC.c (0x8009F6A0..0x8009FAD0, not yet in a unit) ------------------------

// The card's state as an error code: -4 no card, -1 when uFlags bit 0x08 is clear (a mount sets it,
// a format in progress or an encoding error clears it), -35 not mounted, else 0.
s32  fn_8009F734(s32 nPort, s32 nSlot);
u8   fn_8009F7E8(int nPort);        // lbl_80282008[nPort]
u8   fn_8009F850(void);

// ---- MC.c ---------------------------------------------------------------------------------------

s32  fn_8009FAD0(void);
s32  fn_800A0A7C(s32 nPort, s32 nSlot);
void fn_800A1BE0(void);
void fn_800A1D4C(UStreamObject* pObject);  // the 'eagm' handler
s32  fn_800A2100(s32 nPort, s32 nSlot);

// ---- the CARD library (port: GameCube only) ------------------------------------------------------

// An open file on a card.
typedef struct CARDFileInfo {
    s32  chan;                  // 0x00
    s32  fileNo;                // 0x04
    s32  offset;                // 0x08
    s32  length;                // 0x0C
    u16  iBlock;                // 0x10
    u16  unk12;
} CARDFileInfo;
LAYOUT_ASSERT(CARDFileInfo, 0x14);

s32  CARDClose(CARDFileInfo* pFile);
s32  CARDRead(CARDFileInfo* pFile, void* pBuf, s32 nLen, s32 nOffset);

extern CARDFileInfo lbl_801E3180[127];  // the open files, by file number
extern s32   lbl_802813D8;      // the file open through fn_8009F3D4 (-1: none)
extern s32   lbl_80281FC8;      // where the next fn_8009F208 read starts

#endif
