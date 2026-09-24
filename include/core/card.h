// card.h (our name): the parts of Nintendo's CARD library (the memory card) that EA's code calls:
// MC_Gc.c in the game (core/memcard.h) and llSharedFileIO.c in EA's save library.
//
// port: GameCube only. A PC build replaces the callers' layer with file I/O.

#ifndef CORE_CARD_H
#define CORE_CARD_H

#include "game_types.h"
#include "platform.h"

// The CARD library's results that the callers handle.
#define CARD_RESULT_READY           0
#define CARD_RESULT_BUSY            -1
#define CARD_RESULT_WRONGDEVICE     -2
#define CARD_RESULT_NOCARD          -3
#define CARD_RESULT_NOFILE          -4
#define CARD_RESULT_IOERROR         -5
#define CARD_RESULT_BROKEN          -6
#define CARD_RESULT_EXIST           -7
#define CARD_RESULT_NOENT           -8
#define CARD_RESULT_INSSPACE        -9
#define CARD_RESULT_NOPERM          -10
#define CARD_RESULT_LIMIT           -11
#define CARD_RESULT_NAMETOOLONG     -12
#define CARD_RESULT_ENCODING        -13
#define CARD_RESULT_CANCELED        -14
#define CARD_RESULT_FATAL_ERROR     -128

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

// A file's directory entry (0x6C bytes); only the fields EA's code uses.
typedef struct CARDStat {
    char fileName[32];          // 0x00
    u8   unk20[0x24 - 0x20];
    u32  time;                  // 0x24  when the file was last written
    char gameName[4];           // 0x28  the game code of the file's owner (not 0-terminated)
    u8   unk2C[0x2E - 0x2C];
    u8   bannerFormat;          // 0x2E  bits 0-1 the banner's format, bit 2 set: the icon ping-pongs
    u8   unk2F;
    u32  iconAddr;              // 0x30  where the banner and icon images start in the file
    u16  iconFormat;            // 0x34  2 bits per icon frame
    u16  iconSpeed;             // 0x36  2 bits per icon frame; 0 ends the animation
    u32  commentAddr;           // 0x38  where the two comment lines are in the file
    u8   unk3C[0x6C - 0x3C];
} CARDStat;
LAYOUT_ASSERT(CARDStat, 0x6C);

// Called when an asynchronous operation ends, with the channel and the result.
typedef void (*CARDCallback)(s32 nChan, s32 nResult);

s32  CARDCheckAsync(s32 nChan, CARDCallback pfnCallback);
s32  CARDClose(CARDFileInfo* pFile);
s32  CARDCreateAsync(s32 nChan, const char* pName, u32 uSize, CARDFileInfo* pFile, CARDCallback pfnCallback);
s32  CARDDeleteAsync(s32 nChan, const char* pName, CARDCallback pfnCallback);
s32  CARDFastDeleteAsync(s32 nChan, s32 nFileNo, CARDCallback pfnCallback);
s32  CARDFastOpen(s32 nChan, s32 nFileNo, CARDFileInfo* pFile);
s32  CARDFormatAsync(s32 nChan, CARDCallback pfnCallback);
s32  CARDFreeBlocks(s32 nChan, s32* pnFreeBytes, s32* pnFreeFiles);
s32  CARDGetAttributes(s32 nChan, s32 nFileNo, u8* puAttr);
s32  CARDGetEncoding(s32 nChan, u16* puEncoding);
s32  CARDGetResultCode(s32 nChan);
void CARDInit(void);
s32  CARDSetAttributes(s32 nChan, s32 nFileNo, u8 uAttr);
s32  CARDMountAsync(s32 nChan, void* pWorkArea, CARDCallback pfnDetach, CARDCallback pfnAttach);
s32  CARDReadAsync(CARDFileInfo* pFile, void* pBuf, s32 nLen, s32 nOffset, CARDCallback pfnCallback);
s32  CARDRenameAsync(s32 nChan, const char* pOldName, const char* pNewName, CARDCallback pfnCallback);
s32  CARDWriteAsync(CARDFileInfo* pFile, const void* pBuf, s32 nLen, s32 nOffset, CARDCallback pfnCallback);
s32  CARDGetStatus(s32 nChan, s32 nFileNo, CARDStat* pStat);
s32  CARDGetXferredBytes(s32 nChan);
s32  CARDOpen(s32 nChan, const char* pName, CARDFileInfo* pFile);
s32  CARDProbeEx(s32 nChan, s32* pnMemSize, s32* pnSectorSize);
s32  CARDRead(CARDFileInfo* pFile, void* pBuf, s32 nLen, s32 nOffset);
s32  CARDSetStatus(s32 nChan, s32 nFileNo, CARDStat* pStat);
s32  CARDUnmount(s32 nChan);
s32  __CARDEnableGlobal(s32 bEnable);   // CARD_PATCH_2003; returns the previous setting

#endif
