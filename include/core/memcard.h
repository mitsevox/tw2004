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

#include "game.h"
#include "game/save.h"
#include "core/card.h"

#define MC_NUM_PORTS 2              // GameCube card slots A and B
#define MC_NUM_SLOTS 1              // multitap slots per port

// What MC_Gc.c knows about the card in one port and slot (0x98 bytes).
typedef struct MCCardState {
    u32  uFlags;                // 0x00  MC_CARD_* below
    s32  nFreeBlocks;           // 0x04  free space, in whole sectors (CARDFreeBlocks' bytes, rounded up)
    u8   unk8[0x10 - 0x8];
    char aszName[4][0x1D];      // 0x10  four names the menus show (FE_MessageTable fn_8007C3C8)
    s32  nFreeFiles;           // 0x84  free directory entries (CARDFreeBlocks)
    s32  nSectorSize;           // 0x88  CARDProbeEx
    s32  nXferStart;            // 0x8C  CARDGetXferredBytes when an operation starts (fn_8009CB9C)
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

// The game's memory card results (0 is success). Most are MC_Gc.c's translation of a CARD library
// result, named after it: fn_8009CDA0, fn_8009CEF8, fn_8009DFD8, fn_8009E130 and the others turn
// CARD_RESULT_X into MC_ERR_X. The rest are the game's own, named from where they are returned.
typedef enum MCError {
    MC_ERR_NOCARD       = -3,   // CARD_RESULT_NOCARD
    MC_ERR_INSSPACE     = -5,   // CARD_RESULT_INSSPACE: the card is full
    MC_ERR_NAMETOOLONG  = -6,   // CARD_RESULT_NAMETOOLONG
    MC_ERR_NOFILE       = -12,  // CARD_RESULT_NOFILE: no such file; also "no file open" (fn_8009F488)
    MC_ERR_BADDATA      = -18,  // the file read back is not a good save (the wrong size or
                                // attributes, or fn_800A233C rejects it)
    MC_ERR_MOUNTED      = -22,  // fn_8009D74C: the card was mounted already (callers treat it as 0,
                                // but do not unmount after)
    MC_ERR_NOPERM       = -23,  // CARD_RESULT_NOPERM
    MC_ERR_BROKEN       = -24,  // CARD_RESULT_BROKEN
    MC_ERR_IOERROR      = -25,  // CARD_RESULT_IOERROR; the card is then marked damaged (lbl_80281FD0)
    MC_ERR_NOENT        = -26,  // CARD_RESULT_NOENT: no free directory entry
    MC_ERR_CANCELED     = -27,  // CARD_RESULT_CANCELED
    MC_ERR_UNKNOWN      = -28,  // any CARD result not handled
    MC_ERR_ENCODING     = -29,  // CARD_RESULT_ENCODING
    MC_ERR_WRONGDEVICE  = -31,  // CARD_RESULT_WRONGDEVICE
    MC_ERR_EXIST        = -32,  // CARD_RESULT_EXIST
    MC_ERR_FATAL        = -33,  // CARD_RESULT_FATAL_ERROR
    MC_ERR_LIMIT        = -34,  // CARD_RESULT_LIMIT
    MC_ERR_NOTMOUNTED   = -35   // fn_8009F734, fn_8009E918: the card is not mounted
} MCError;

// A card and something on it, as MC.c's file functions take them (12 bytes).
typedef struct MCCardPos {
    s32  nPort;                 // 0x0
    s32  nSlot;                 // 0x4
    s32  n8;                    // 0x8  a bit index into the save image's flags (MC.c)
} MCCardPos;
LAYOUT_ASSERT(MCCardPos, 0xC);

// A card position and a string, as fn_800A0C6C takes them (the menus pass a name typed in).
typedef struct MCCardPosStr {
    MCCardPos pos;              // 0x0
    char* szC;                  // 0xC
} MCCardPosStr;
LAYOUT_ASSERT(MCCardPosStr, 0x10);

// The memory-card screens' operations (lbl_8018C7D8): four sets of five, one set per kind of save
// (fn_80084FF0 picks one; set 0 is the game's save, starting with fn_8009FE90 and fn_8009FCFC; set 3
// is the EA Sports Bio's). apfn[4] gives the save's size on the card (startUp.c). The menus'
// messages call them through fn_80084FB4 and its neighbours with the card they picked.
typedef s32 (*MCOp)(MCCardPos* pPos);
typedef struct MCOpSet {
    MCOp apfn[5];               // 0x00
} MCOpSet;
LAYOUT_ASSERT(MCOpSet, 0x14);

extern MCOpSet lbl_8018C7D8[4];

// The save file's names on the card: EA kept the PlayStation 2 names (SLUS-20757 is the PS2
// release), the second a backup copy.
#define MC_FILE_NAME    "BASLUS-20757"
#define MC_BACKUP_NAME  "BU_BASLUS-20757"
#define MC_DIR_NAME     "../BASLUS-20757"   // the PS2 save directory; here it stands for the file

#define MC_BUFFER_SIZE  0x50000     // one save file image in memory

// The session's record tables as the save file keeps them: a copy of gSession from aCourseRecord
// to recC (0xF00..0x5B2C, the same layout).
typedef struct SaveRecords {
    CourseRecord aCourseRecord[NUM_COURSE_RECORDS];    // 0x0000
    RecordEntry recA[8][5];     // 0x41A0
    RecordEntry recB[3][3][5];  // 0x44C0
    RecordEntry recC[5][2][5];  // 0x4844
} SaveRecords;
LAYOUT_ASSERT(SaveRecords, 0x4C2C);

// The end of the saved data: a mark and a checksum of everything before it (fn_800A23BC).
typedef struct SaveTrailer {
    char aMagic[3];             // 0x0  "@BE" (fn_800A233C also takes "@BD")
    u8   unk3;
    u32  uChecksum;             // 0x4  fn_800A23BC over the image up to here, this word read as 0
} SaveTrailer;
LAYOUT_ASSERT(SaveTrailer, 0x8);

#define NUM_SAVE_REPLAYS    5   // the replay bits of SaveImage.uFlags
#define NUM_SAVE_PROFILES   4   // the profile bits

// The save file, as it is read and written whole (MC_BUFFER_SIZE bytes). It starts with the
// GameCube's comment strings, banner and icon (fn_8009E544 fills them); uFlags says which of the
// parts after it hold data.
// port: raw big-endian bytes on the card; a port reads and writes it field by field.
typedef struct SaveImage {
    char szGameName[32];        // 0x00000  the card's comment strings (fn_8009E544)
    char szComment[32];         // 0x00020
    u8   aBanner[0x1800];       // 0x00040  from the 'MCB ' object
    u8   aIcon[0x800];          // 0x01840  from the 'MCI ' object
    u32  uFlags;                // 0x02040  MC_SAVE_* below
    GameOptions options;        // 0x02044  gSession.options
    SaveRecords records;        // 0x020CC
    Replay aReplay[NUM_SAVE_REPLAYS];           // 0x06CF8  saved shots (gReplayData)
    SaveProfile aProfile[NUM_SAVE_PROFILES];    // 0x0B8C0
    s32  n4D0C0;                // 0x4D0C0  -> lbl_80281FF8 when MC_SAVE_4D0C0 is set
    SaveTrailer trailer;        // 0x4D0C4
    u8   unk4D0CC[MC_BUFFER_SIZE - 0x4D0CC];
} SaveImage;
LAYOUT_ASSERT(SaveImage, MC_BUFFER_SIZE);

// SaveImage.uFlags
#define MC_SAVE_OPTIONS     0x0002
#define MC_SAVE_RECORDS     0x0004
#define MC_SAVE_PROFILE(n)  (0x8 << (n))    // aProfile[n]
#define MC_SAVE_REPLAY(n)   (0x80 << (n))   // aReplay[n]
#define MC_SAVE_4D0C0       0x1000          // n4D0C0

extern MCCardState lbl_801F1510[MC_NUM_PORTS][MC_NUM_SLOTS];
extern u8    lbl_80281FD0[MC_NUM_PORTS];    // the card in this port had an I/O error: its
                                            // operations fail with -25 until the next start
extern s32   lbl_80282000[MC_NUM_PORTS];    // slots per port (1)
extern u8    lbl_80282008[MC_NUM_PORTS];    // the port has a multitap (0)

// Two save file images, each reached through three pointers. MC_Gc.c allocates them, or borrows
// skalib's scratch memory, and parks them in ARAM between uses. MC.c reads the card into the
// first and builds what it writes in the second.
extern SaveImage* lbl_80281FD8; // } the second image
extern SaveImage* lbl_80281FDC; // }
extern SaveImage* lbl_80281FE0; // }
extern SaveImage* lbl_80281FE4; // } the first image
extern SaveImage* lbl_80281FE8; // }
extern SaveImage* lbl_80281FEC; // }
extern s32   lbl_80281FF8;      // SaveImage.n4D0C0 of the save loaded (0 when it has none)

// One entry of the list the 'eagm' stream object carries (fn_800A1D4C builds it, 0x4C bytes).
typedef struct MCEagmEntry {
    u8    b0;                   // 0x00  cleared when the list is loaded
    u8    unk1[3];
    char* p4;                   // 0x04  n8 names of 16 characters each
    s32   n8;                   // 0x08
    char  szName[0x40];         // 0x0C
} MCEagmEntry;
LAYOUT_ASSERT(MCEagmEntry, 0x4C);

extern u32   lbl_801F1110[256]; // the save checksum's CRC table (fn_800A253C)
extern MCEagmEntry* lbl_80281FF0;   // the 'eagm' list (fn_800A1BE0 frees it)
extern s32   lbl_80281FF4;      // its number of entries
extern u32   lbl_80281FC0;     // the size parked in ARAM (MC_BUFFER_SIZE + 0x20)
extern u32   lbl_80281FC4;      // the ARAM address they are parked at (0: none yet)
extern UStreamObject* lbl_80281FB8;     // the 'MCI ' object (fn_8009EB30)
extern UStreamObject* lbl_80281FBC;     // the 'MCB ' object (fn_8009EB38)

// ---- MC_Gc.c ------------------------------------------------------------------------------------

void fn_8009CC00(void);
void fn_8009CC88(void);
void fn_8009CD10(void);
void fn_8009CD7C(void);
// The space a save needs: compared with MCCardState.nFreeBlocks (EASportsBio.c passes 0, 3).
s32  fn_8009D1D8(s32 nPort, s32 nSlot, s32 arg2, s32 arg3);
s32  fn_8009D3DC(s32 nPort, s32 nSlot);
s32  fn_8009D50C(s32 nPort, s32 nSlot);     // new files an EA Sports Bio save needs (0 or 1)
s32  fn_8009D614(s32 nPort, s32 nSlot, const char* pName);
s32  fn_8009D74C(s32 nPort, s32 nSlot);     // mount the card; 0, or -22 when it was mounted already
s32  fn_8009DBAC(s32 nPort, s32 nSlot);     // unmount it
void fn_8009DCEC(s32 nPort, s32 nSlot);
s32  fn_8009DD44(s32 nPort, s32 nSlot, const char* pName);
s32  fn_8009DD94(s32 nPort, s32 nSlot, const char* pName, void* pBuf, s32 nLen);  // read a file
// Fill in the file's comment strings (szGameName, szComment), banner and icon.
void fn_8009E544(char* pGameName, char* pComment, u8* pIcon, u8* pBanner);
// Write a file. pBackupName (the PS2's backup copy) is not used here.
s32  fn_8009E604(s32 nPort, s32 nSlot, const char* pName, void* pBuf, s32 nLen,
                 const char* pBackupName);
s32  fn_8009E758(s32 nPort, s32 nSlot, const char* pName);
s32  fn_8009E918(s32 nPort, s32 nSlot);     // format the card
void fn_8009EA98(void);
void fn_8009EAF0(void);
s32  fn_8009EE28(s32 nPort, s32 nSlot);
u32  fn_8009EF90(void);
void fn_8009F02C(void);             // bring the images back from ARAM (fn_8009EF98 parks them)
// Up to nMax names of the card's files whose name holds pPattern, into apName; how many in pnFound.
s32  fn_8009F0F0(s32 nPort, s32 nSlot, const char* pPattern, char** apName, s32 nMax, s32* pnFound);
// Read from open file nFile, on from the last read. arg3 is not used (TibExt passes 0).
s32  fn_8009F208(s32 nFile, void* pBuf, s32 nLen, s32 arg3);
s32  fn_8009F258(s32 nFile, void* pBuf, s32 nLen);          // write to open file nFile
s32  fn_8009F2D8(s32 nFile, s32 nOffset, u8 bFromStart);    // move open file nFile's position
s32  fn_8009F35C(void);             // always 0
s32  fn_8009F364(void);             // always 0
s32  fn_8009F36C(s32 nPort, s32 nSlot, s32* pnFreeBytes);   // the card's free space
// The card's free directory entries. pName is not used.
s32  fn_8009F3A0(s32 nPort, s32 nSlot, const char* pName, s32* pnFreeFiles);
// Open pName, its file number into pnFile.
s32  fn_8009F3D4(s32 nPort, s32 nSlot, const char* pName, u32 uFlags, s32* pnFile);
s32  fn_8009F488(s32 nFile);        // close open file nFile
// Create pName with nLen bytes, but only when it is the save directory's name (the PS2's mkdir).
s32  fn_8009F514(s32 nPort, s32 nSlot, const char* pName, s32 nLen);
s32  fn_8009F5E4(s32 nPort, s32 nSlot, const char* pName);    // delete the save file

// Look through the card's files for one whose name holds "BASLUS-20572": 0 if there is one, else
// MC_ERR_NOFILE. nSlot is not used.
s32  fn_8009F6A0(s32 nPort, s32 nSlot);
// The card's state as an error code: -4 no card, -1 when uFlags bit 0x08 is clear (a mount sets it,
// a format in progress or an encoding error clears it), -35 not mounted, else 0.
s32  fn_8009F734(s32 nPort, s32 nSlot);
s32  fn_8009F728(int nPort);        // lbl_80281FD0[nPort] (the menus read it as a whole word)
u8   fn_8009F7E8(int nPort);        // lbl_80282008[nPort]
void fn_8009F7F4(MCCardState* pState, int nPort, int nSlot);   // copy out lbl_801F1510[nPort][nSlot]
MCCardState* fn_8009F834(s32 nPort, s32 nSlot);                 // &lbl_801F1510[nPort][nSlot]

// ---- MC.c ---------------------------------------------------------------------------------------

u8   fn_8009F850(void);             // at boot: fn_800A13E8 on each card until one succeeds

s32  fn_8009FAD0(void);
s32  fn_800A0A7C(s32 nPort, s32 nSlot);
void fn_800A1BE0(void);
void fn_800A1D4C(UStreamObject* pObject);  // the 'eagm' handler
s32  fn_800A2100(s32 nPort, s32 nSlot);
s32  fn_800A218C(s32 nPort, s32 nSlot);    // always MC_ERR_NOFILE
s32  fn_800A2194(s32 nPort, s32 nSlot);

// ---- the save file's checksum (0x800A233C) -------------------------------------------------------

// Whether the data from pData up to pTrailer is a good save: the mark, then the checksum.
u8   fn_800A233C(void* pData, SaveTrailer* pTrailer);
// The checksum (a CRC-32) of pData up to pTrailer's uChecksum, the mark included. uChecksum is
// set to 0 while it runs and put back after, though the sum stops short of it.
u32  fn_800A23BC(void* pData, SaveTrailer* pTrailer);

// ---- the 'eagm' list and string helpers (MC.c) ----------------------------------------------------

u8    fn_800A2604(s32 nEntry);      // lbl_80281FF0[nEntry].b0
char* fn_800A2614(s32 nEntry);      // lbl_80281FF0[nEntry].szName
s32   fn_800A2628(void);            // lbl_80281FF4
s32   fn_800A27F4(void);            // lbl_80281FF8
// Copy a string of 16-bit characters into 8-bit ones (and back), nMax characters at most, the
// terminator included. A character above 0xFF becomes 0xAC.
void  fn_800A2774(const u16* szSrc, char* szDst, s32 nMax);
void  fn_800A27BC(const char* szSrc, u16* szDst, s32 nMax);

// ---- MC_Gc.c's CARD state (the CARD library itself is in core/card.h) ----------------------------

extern CARDFileInfo lbl_801E3180[127];  // the open files, by file number
extern s32   lbl_802813D8;      // the file open through fn_8009F3D4 (-1: none)
extern s32   lbl_80281FC8;      // where the next fn_8009F208 read starts
extern s32   lbl_80281FB4;      // the size of the operation in progress (fn_8009CB9C)

#endif
