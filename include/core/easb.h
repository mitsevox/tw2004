#ifndef CORE_EASB_H
#define CORE_EASB_H

// The EA Sports Bio library ("EASB"): EA's cross-game player profile on the memory card. The
// game side is EASportsBio.c; the library is the code after SkinBurn.c (0x80127D84-0x8012E950).
// Error codes and the image layout are TW06's (its PDB keeps the EASB types).

#include "game_types.h"

// Every library call returns one of these (TW06's EASBErrorE).
typedef enum EASBErrorE {
    EASB_ERROR_NONE = 0,
    EASB_ERROR_NOFILE = 1,
    EASB_ERROR_NODEVICE = 2,
    EASB_ERROR_NULL_PARAMETERS = 3,
    EASB_ERROR_INVALID_PARAMETERS = 4,
    EASB_ERROR_CALLBACK_NOT_SET = 5,
    EASB_ERROR_STRING_TOO_LARGE = 6,
    EASB_ERROR_STRING_TOO_SMALL = 7,
    EASB_ERROR_OUT_OF_MEMORY = 8,
    EASB_ERROR_NOT_ENOUGH_SPACE = 9,
    EASB_ERROR_HEADER_NOT_LOADED = 10,
    EASB_ERROR_PRODUCT_NOT_LOADED = 11,
    EASB_ERROR_PRODUCT_ALREADY_LOADED = 12,
    EASB_ERROR_IMAGE_NOT_LOADED = 13,
    EASB_ERROR_IMAGE_ALREADY_LOADED = 14,
    EASB_ERROR_INITIALIZED = 15,
    EASB_ERROR_NOT_INITIALIZED = 16,
    EASB_ERROR_CANNOT_REOPEN = 17,
    EASB_ERROR_WRONG_FILE = 18,
    EASB_ERROR_FILE_OPEN = 19,
    EASB_ERROR_FILE_EXISTS = 20,
    EASB_ERROR_FILE_CORRUPT = 21,
    EASB_ERROR_SECTION_CORRUPT = 22,
    EASB_ERROR_INVALID_LEVEL = 23,
    EASB_ERROR_INVALID_HEADER = 24,
    EASB_ERROR_INVALID_PRODUCT = 25,
    EASB_ERROR_INVALID_ACCOMPLISHMENT = 26,
    EASB_ERROR_INVALID_LANGUAGE = 27,
    EASB_ERROR_INVALID_ICON = 28,
    EASB_ERROR_INVALID_IMAGE = 29,
    EASB_ERROR_PROCESS_IN_PROGRESS = 30,
    EASB_ERROR_NO_PROCESS_IN_PROGRESS = 31,
    EASB_ERROR_IMAGE_NOT_SUPPORTED = 32,
    EASB_ERROR_INTERNAL = 33,
    EASB_ERROR_UNKNOWN = 34
} EASBErrorE;

// What a call needs from the memory-card session (the argument of fn_8012CCD8).
#define EASB_NEED_FILE 0        // the Bio file must be open
#define EASB_NEED_NO_FILE 1     // no file may be open
#define EASB_NEED_ANY 2         // either

// A game's picture in the Bio: a Windows BMP (TW06's EASBImage_t).
typedef struct EASBImage {
    u8 aBitMapFileHeader[0x36];     // 0x0000
    u8 aColorTable[0x400];          // 0x0036: 256 BGRA entries
    u8 aData[0x4000];               // 0x0436: 128x128 pixels, 8 bits each
} EASBImage;

// This game's product record, as stored in the Bio file. Times are seconds since 1970
// (TibExt.c's clock).
typedef struct EASBProduct {
    u8 unk0[0x4C];
    u32 uTime;                      // 0x004C: last update, kept within 2003-2023 by fn_80128BC4
    u32 u50;                        // 0x0050: play time while b11E0 is set
    u32 u54;                        // 0x0054: play time while b11E0 is clear
    u32 u58;                        // 0x0058: counter raised by fn_8012D8C4
    u32 u5C;                        // 0x005C: counter raised by fn_8012D93C
    u8 unk60[0x1102];
    u16 uLevel;                     // 0x1162: only rises, up to EASB_MAX_LEVEL
    u8 unk1164[2];
    u8 bValid;                      // 0x1166
    u8 b1167;                       // 0x1167
} EASBProduct;                      // size 0x1168

#define EASB_MAX_LEVEL 1250         // fn_8012DAB8 also takes 1251

// The library's state, allocated when it starts (lbl_802825B8).
typedef struct EASBState {
    void* pAllocator;               // 0x0000: what the library allocates from (TibExt.c)
    u8 unk4[0x50];
    u32 u54;                        // 0x0054: the Bio's totals, raised with the product's
    u32 u58;                        // 0x0058
    u32 u5C;                        // 0x005C
    u32 u60;                        // 0x0060
    u8 unk64[4];
    EASBProduct product;            // 0x0068: this game's record
    u8 b11D0;                       // 0x11D0
    u8 unk11D1[3];
    u8* pProductBuffer;             // 0x11D4: EASB_PRODUCT_BUFFER_SIZE bytes, from the heap
    u8* pImageBuffer;               // 0x11D8: color table, pixels, then a "loaded" flag at 0x4400
    u32 uLastTime;                  // 0x11DC: when fn_8012D1A0 last added up play time
    u8 b11E0;                       // 0x11E0: which play-time counter runs (fn_8012DD7C)
    u8 unk11E1[3];
    EASBImage* pImage;              // 0x11E4: the game's own picture
} EASBState;

#define EASB_PRODUCT_BUFFER_SIZE 0x1B328

extern EASBState* lbl_802825B8;

// TibExt.c: the library's memory and clock glue.
void fn_80122128(void* pAllocator, void* p, u32 uSize, u32 uAlign);
u32 fn_80122150(void);              // the real-time clock, in seconds since 1970

// The code before EASB.c (still sweep code).
u32 fn_80128468(u32 uA, u32 uB);    // uA + uB, saturating at 0xFFFFFFFF
u32 fn_80128BC4(u32 uTime);         // clamps a time to 2003-01-01..2023-01-01
s32 fn_8012881C(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4);
s32 fn_801288DC(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6);
EASBErrorE fn_8012C5F8(u32* p0, EASBProduct* pProduct, u8* p2);
EASBErrorE fn_8012C69C(void);
EASBErrorE fn_8012C73C(void);
EASBErrorE fn_8012C774(u8* pBuffer);
EASBErrorE fn_8012C7BC(u32* p0, EASBProduct* pProduct, s32 arg2);
u8 fn_8012C83C(void);
u8 fn_8012C848(void);
s32 fn_8012C888(s32 arg0);

// EASB.c
EASBErrorE fn_8012CCD8(s32 nNeed);
EASBErrorE fn_8012CD8C(void);
EASBErrorE fn_8012CF00(void);
EASBErrorE fn_8012D0D4(void);
EASBErrorE fn_8012D1A0(void);
EASBErrorE fn_8012D560(void);
EASBErrorE fn_8012D5B0(void);
EASBErrorE fn_8012D694(void);
EASBErrorE fn_8012D6C8(void);
EASBErrorE fn_8012D710(void);
EASBErrorE fn_8012D744(s32 arg0);
EASBErrorE fn_8012D794(s32 arg0);
s32 fn_8012D7F0(void);
EASBErrorE fn_8012D8C4(u32 uCount);
EASBErrorE fn_8012D93C(u32 uCount);
EASBErrorE fn_8012DAB8(u16 uLevel);
s32 fn_8012DB30(s32 arg0, s32 arg1, s32 arg2, u32 uTime);
void fn_8012DD24(s32 arg0, s32 arg1, s32 arg2);
EASBErrorE fn_8012DD7C(u8 bFlag);
EASBErrorE fn_8012DDE0(u32* pOut);
EASBErrorE fn_8012DE38(u32* pOut);
s32 fn_8012E818(void);
EASBErrorE fn_8012E820(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4);
EASBErrorE fn_8012E8A8(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6);

#endif
