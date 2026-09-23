#ifndef CORE_EASB_H
#define CORE_EASB_H

// The EA Sports Bio library ("EASB"): EA's cross-game player profile on the memory card. The
// game side is EASportsBio.c; the library is the code after SkinBurn.c (0x80127D84-0x8012E950).
// Error codes and the image layout are TW06's (its PDB keeps the EASB types).

#include "game_types.h"
#include "Common/TagFile.h"

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

// Where a memory-card operation the library runs in steps has got to (TW06's EASBProcessE).
typedef enum EASBProcessE {
    EASB_PROCESS_NONE = 0,
    EASB_PROCESS_CONTINUE = 1,
    EASB_PROCESS_COMPLETE = 2
} EASBProcessE;

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

// The text sizes of the records, in characters with the end: a name must be shorter.
#define EASB_PRODUCT_NAME_SIZE 0x24
#define EASB_GAMES_PLAYED_TYPE_SIZE 0x14
#define EASB_ACCOMPLISHMENT_NAME_SIZE 0x40

// Times the library accepts, in seconds since 1970 (TibExt.c's clock).
#define EASB_TIME_FIRST 0x3E122F80  // 2003-01-01 00:00:00
#define EASB_TIME_LAST 0x63B0CD00   // 2023-01-01 00:00:00

// One accomplishment in a product record (EASBProduct.aAccomplishments). fn_80128054 checks it.
typedef struct EASBAccomplishment {
    u16 szName[EASB_ACCOMPLISHMENT_NAME_SIZE];  // 0x00: wide text
    u32 uTime;                      // 0x80: when it was set, within 2003-2023
    u16 uLanguage;                  // 0x84: the language of szName (fn_801298FC)
    u8 u86;                        // 0x86: 1 to 250; the first sort key unless sorting by time
    u8 bValid;                      // 0x87
} EASBAccomplishment;               // size 0x88
LAYOUT_ASSERT(EASBAccomplishment, 0x88);

#define EASB_MAX_ACCOMPLISHMENTS 32 // per product record

// The Bio's totals over every product record (EASBState.totals; fn_80128580 adds a record in).
typedef struct EASBTotals {
    u32 u0;                         // 0x00: EASBProduct.u50
    u32 u4;                         // 0x04: EASBProduct.u54
    u32 u8;                         // 0x08: EASBProduct.u58, never below uC
    u32 uC;                         // 0x0C: EASBProduct.u5C
    u8 nProducts;                   // 0x10: the records added in
} EASBTotals;                       // size 0x14
LAYOUT_ASSERT(EASBTotals, 0x14);

// This game's product record, as stored in the Bio file. Times are seconds since 1970
// (TibExt.c's clock).
typedef struct EASBProduct {
    char szName[EASB_PRODUCT_NAME_SIZE];                // 0x0000: EASBInitParams.szProductName
    u16 szGamesPlayedType[EASB_GAMES_PLAYED_TYPE_SIZE]; // 0x0024: EASBInitParams.szGamesPlayedType
    u32 uTime;                      // 0x004C: last update, kept within 2003-2023 by fn_80128BC4
    u32 u50;                        // 0x0050: play time while b11E0 is set
    u32 u54;                        // 0x0054: play time while b11E0 is clear
    u32 u58;                        // 0x0058: counter raised by fn_8012D8C4
    u32 u5C;                        // 0x005C: counter raised by fn_8012D93C
    EASBAccomplishment aAccomplishments[EASB_MAX_ACCOMPLISHMENTS];  // 0x0060: cleared by fn_8012CD8C
    u16 u1160;                      // 0x1160
    u16 uLevel;                     // 0x1162: only rises, up to EASB_MAX_LEVEL
    u16 uGamesPlayedTypeLanguage;   // 0x1164: EASBInitParams.uGamesPlayedTypeLanguage
    u8 bValid;                      // 0x1166
    u8 b1167;                       // 0x1167
} EASBProduct;                      // size 0x1168
LAYOUT_ASSERT(EASBProduct, 0x1168);

#define EASB_MAX_LEVEL 1250         // fn_8012DAB8 also takes 1251

// Which product record a call means (the u8 the per-game calls take): the Bio file holds
// EASB_MAX_PRODUCTS games' records, loaded into EASBState.pProductBuffer.
#define EASB_MAX_PRODUCTS 25
#define EASB_PRODUCT_OURS 26        // this game's own record (EASBState.product)
#define EASB_PRODUCT_NONE 27        // no record (EASBState.b11D0 when this game has no slot)

// A game's picture as the Bio file keeps it (EASBState.pImageBuffer; EASBProduct.b1167 picks one).
typedef struct EASBImageSlot {
    u8 aColorTable[0x400];          // 0x0000: EASBImage.aColorTable
    u8 aData[0x4000];               // 0x0400: EASBImage.aData
    u8 bLoaded;                     // 0x4400
} EASBImageSlot;                    // size 0x4401
LAYOUT_ASSERT(EASBImageSlot, 0x4401);

// The library's state, allocated when it starts (lbl_802825B8).
typedef struct EASBState {
    u32 uHeapID;                    // 0x0000: EASBInitParams.uHeapID, passed to TibExt.c's allocator
    u8 unk4[0x50];
    EASBTotals totals;              // 0x0054: the Bio's totals, raised with the product's
    EASBProduct product;            // 0x0068: this game's record
    u8 b11D0;                       // 0x11D0: this game's slot in pProductBuffer, or EASB_PRODUCT_NONE
    u8 unk11D1[3];
    EASBProduct* pProductBuffer;    // 0x11D4: EASB_MAX_PRODUCTS records, from the heap
    EASBImageSlot* pImageBuffer;    // 0x11D8: one picture per product record
    u32 uLastTime;                  // 0x11DC: when fn_8012D1A0 last added up play time
    u8 b11E0;                       // 0x11E0: which play-time counter runs (fn_8012DD7C)
    u8 unk11E1[3];
    EASBImage* pImage;              // 0x11E4: the game's own picture
} EASBState;

#define EASB_PRODUCT_BUFFER_SIZE (EASB_MAX_PRODUCTS * sizeof(EASBProduct))     // 0x1B328

// What the game passes to fn_8012D394 to start the library (TW06's EASBInitParams_t).
typedef struct EASBInitParams {
    char* szProductName;            // 0x00: this game's name in the Bio
    u16* szGamesPlayedType;         // 0x04: what the Bio counts this game's games in (wide text)
    SFIOFuncTable* pCallbacks;      // 0x08: the memory-card glue (TibExt.c)
    u32 uHeapID;                    // 0x0C
    u16 uGamesPlayedTypeLanguage;   // 0x10: the language of szGamesPlayedType, two letters
} EASBInitParams;

extern EASBState* lbl_802825B8;

// TibExt.c: the library's memory and clock glue.
// The library's allocator (TW06's signatures). The heap id is not used: the game's current heap
// (fn_8000A0B4) is.
void* TibExtMemAlloc(u32 uHeapID, u32 uSize, u32 uAlign);
void TibExtMemFree(u32 uHeapID, void* p, u32 uSize, u32 uAlign);
u32 TibExtCurrentTimeGet(void);     // the real-time clock, in seconds since 1970
SFIOFuncTable* fn_801221F0(void);   // fills in and returns the memory-card callbacks

// TibExt.c's memory-card glue (lbl_80260D88): the callbacks it hands the shared file library,
// then the result of the last card call.
typedef struct TibExtCard {
    SFIOFuncTable fn;               // 0x00: fn_801221F0 fills these in
    s32 nError;                     // 0x44: the last card call's error, as the file library's code
    s32 n48;                        // 0x48: the last call's result (a size, a count, a file)
    char szFileName[0x44];          // 0x4C: the save file the probe found
} TibExtCard;
LAYOUT_ASSERT(TibExtCard, 0x90);

extern TibExtCard* lbl_80281970;
extern s32 lbl_80194758[46];        // the file library's code for each card error (by -error)

// A storage operation's arguments, kept in EASBStorage.args while its steps run (fn_8012BF18).
typedef struct EASBStorageArgs {
    char* szName;                   // 0x00: the product name the game's record must have (fn_8012AA7C)
    EASBTotals* pTotals;            // 0x04
    EASBProduct* pProduct;          // 0x08: one record, or fn_8012C774's EASB_MAX_PRODUCTS
    EASBImageSlot* pImageSlots;     // 0x0C: EASB_MAX_PRODUCTS pictures (fn_8012AE40)
    EASBImage* pImage;              // 0x10
    void* pHeader;                  // 0x14: the save's banner and icon block (SFIOCreate)
    int eDevice;                    // 0x18: where to create the file (TagFile_BeginSave)
} EASBStorageArgs;                  // size 0x1C

// One of the storage operations (lbl_80195520, by operation number): whether it starts the
// tag-file library first and shuts it down after, and its steps in order.
typedef struct EASBStorageOp {
    s32 nOperation;                 // 0x00: its own number
    u8 bStartTagFile;               // 0x04: fn_8012BF18 starts the library (fn_8012CAA8)
    u8 bStopTagFile;                // 0x05: fn_8012C03C shuts it down (fn_8012CC48)
    u8 unk6[2];
    s32 anSteps[8];                 // 0x08: EASB_STEP_END ends the list
} EASBStorageOp;                    // size 0x28

#define EASB_OPERATION_ERROR 10     // clean-up after a failed operation (fn_8012C1AC)
#define EASB_OPERATION_NONE 11      // EASBStorage.nOperation when nothing runs
#define EASB_STEP_END 0x14

extern EASBStorageOp lbl_80195520[EASB_OPERATION_NONE + 1];

// The storage code's state (lbl_802825B0, 0x1A8 bytes from fn_8012BD0C).
typedef struct EASBStorage {
    SFIOFuncTable callbacks;        // 0x000: a copy of EASBInitParams.pCallbacks
    TagSession savedSession;        // 0x044: the open file's session, kept between operations
    u32 uHeapID;                    // 0x08C
    u8 b90;                         // 0x090
    u8 bFileOpen;                   // 0x091
    u8 b92;                         // 0x092
    u8 unk93;
    s32 n94;                        // 0x094: how the game's slot was picked (fn_8012A4C4): 3 its own
                                    //        record, 2 an empty one, 1 the oldest; fn_80129F98's mode
    u8 nSlot;                       // 0x098: this game's record, or EASB_PRODUCT_NONE
    u8 unk99[3];
    u8* pBuffer;                    // 0x09C: one record's packed bytes (fn_8012CAA8)
    u32 uBufferSize;                // 0x0A0: the biggest record: an image (0x4301)
    s32 nHeadState;                 // 0x0A4: 1 when the HEAD record is read or written, 2 when missing
    s32 anProductState[EASB_MAX_PRODUCTS];  // 0x0A8: the same for each PROD record
    EASBTotals totals;              // 0x10C: the totals in the file's HEAD record
    TagSession session;             // 0x120: the tag-file session of the running operation
    u8 nRecord;                     // 0x168: the record a step is on (EASB_PRODUCT_NONE: HEAD)
    u8 nFoundSlot;                  // 0x169: the slot fn_8012A4C4 picks for the game
    u8 unk16A[2];
    u32 uOldestTime;                // 0x16C: the oldest record seen by fn_8012A4C4
    s32 nFoundKind;                 // 0x170: how nFoundSlot was picked (becomes n94)
    s32 nLastError;                 // 0x174: the tag-file library's last error (fn_8012C98C)
    s32 nResult;                    // 0x178: the error that stopped nLastOperation (fn_8012C1AC)
    s32 nLastOperation;             // 0x17C: the operation that failed
    EASBStorageArgs args;           // 0x180: the running operation's arguments
    s32 nOperation;                 // 0x19C: EASB_OPERATION_NONE when idle
    s32* pnSteps;                   // 0x1A0: the running operation's steps (EASBStorageOp.anSteps)
    EASBProcessE eStepProcess;      // 0x1A4: where the running step has got to (fn_8012C388)
} EASBStorage;
LAYOUT_ASSERT(EASBStorage, 0x1A8);

extern EASBStorage* lbl_802825B0;

// EASBStorage.c: the code before EASB.c.
EASBErrorE fn_80127F88(EASBProduct* pProduct);  // EASB_ERROR_INVALID_PRODUCT if the record is bad
EASBErrorE fn_80128054(EASBAccomplishment* pAccomplishment);
u8 fn_801280F8(u16 uLanguage);      // is it a language the library knows
u8 fn_801281B4(u16 uLanguage, u16* aLanguages, u8 nLanguages);  // is uLanguage in the list
EASBErrorE fn_8012835C(u16* sz, u32 uSize, u32* puLength);
EASBErrorE fn_801283B0(EASBInitParams* pParams);
void fn_80128488(EASBProduct* pProduct, u8 bValid, char* szName, u16* szGamesPlayedType, u16 uLanguage);
u32 fn_80128468(u32 uA, u32 uB);    // uA + uB, saturating at 0xFFFFFFFF
void fn_80128528(EASBTotals* pTotals, EASBProduct* pProduct);
void fn_80128624(EASBProduct* aProducts, u32 nCount);          // a shell sort of the records
void fn_8012872C(EASBAccomplishment** apList, u32 nCount, s32 nSort);
s32 fn_80128CA0(char* szA, char* szB, u8 bCase);                // compares two texts (0: same)
char* fn_80128BF8(char* szDest, char* szSrc, u32 uSize);        // bounded string copy
u16* fn_80128C4C(u16* szDest, u16* szSrc, u32 uLength);         // the same for wide text
void fn_80128EC0(char* sz);
void fn_80128F04(u16* sz, u16 uLanguage);
EASBErrorE fn_80128FD4(EASBTotals* pTotals, u16* puLevel, f32* pfProgress);
EASBErrorE fn_801291A8(u16 uLevel, u16 u1160, u16* puLevel);
EASBErrorE fn_80129218(u16 uLevel, u16 u1160, u16* puLevel);
u32 fn_80128BC4(u32 uTime);         // clamps a time to 2003-01-01..2023-01-01
EASBErrorE fn_8012881C(u32 uTime, u16* pnDays, u8* pnHours, u8* pnMinutes, u8* pnSeconds);
EASBErrorE fn_801288DC(u32 uTime, u16* pnYear, u8* pnMonth, u8* pnDay, u8* pnHours, u8* pnMinutes,
                       u8* pnSeconds);
EASBErrorE fn_8012C5F8(EASBTotals* pTotals, EASBProduct* pProduct, char* szName);
EASBErrorE fn_8012C69C(void);
EASBErrorE fn_8012C73C(void);
EASBErrorE fn_8012C774(EASBProduct* pProducts);
EASBErrorE fn_8012C7BC(EASBTotals* pTotals, EASBProduct* pProduct, EASBImage* pImage);
u8 fn_8012C83C(void);
u8 fn_8012C848(void);
EASBErrorE fn_8012C854(u8* pnSlot);  // the storage code's slot number (EASB_PRODUCT_NONE: none)
EASBErrorE fn_8012C888(u32* pOut);

// EASB.c
EASBErrorE fn_8012CCD8(s32 nNeed);
EASBErrorE fn_8012CD8C(void);
EASBErrorE fn_8012CF00(void);
EASBErrorE fn_8012CF64(void);
EASBErrorE fn_8012D030(void);
EASBErrorE fn_8012D0D4(void);
EASBErrorE fn_8012D1A0(void);
EASBErrorE fn_8012D290(u8 nProduct, EASBProduct** ppProduct);
EASBErrorE fn_8012D394(EASBInitParams* pParams);
EASBErrorE fn_8012D560(void);
EASBErrorE fn_8012D5B0(void);
EASBErrorE fn_8012D5E4(void* p0, void* p1);
EASBErrorE fn_8012D694(void);
EASBErrorE fn_8012D6C8(void);
EASBErrorE fn_8012D710(void);
EASBErrorE fn_8012D744(u32* pOut);
EASBErrorE fn_8012D794(void* pImage);
EASBErrorE fn_8012D7F0(void);
EASBErrorE fn_8012D7F8(EASBProcessE* peProcess);  // runs the next step of the operation
EASBErrorE fn_8012D8C4(u32 uCount);
EASBErrorE fn_8012D93C(u32 uCount);
EASBErrorE fn_8012D9B4(u16* puLevel);
EASBErrorE fn_8012DA38(u16* puNextLevel);
EASBErrorE fn_8012DAB8(u16 uLevel);
EASBErrorE fn_8012DB30(u16* szName, s32 arg1, s32 nLanguage, u32 uTime);
EASBErrorE fn_8012DD24(u16* szName, s32 arg1, s32 nLanguage);
EASBErrorE fn_8012DD7C(u8 bFlag);
EASBErrorE fn_8012DDE0(u32* pOut);
EASBErrorE fn_8012DE38(u32* pOut);
EASBErrorE fn_8012DE90(u8* pnProducts);
EASBErrorE fn_8012DF4C(u16* puLevel, f32* pfProgress);
EASBErrorE fn_8012DFDC(u8 nProduct, char* szName, u32 uSize);
EASBErrorE fn_8012E084(u8 nProduct, u16* szGamesPlayedType, u32 uLength, u16* aLanguages, u8 nLanguages,
                       u16* puLanguage);
EASBErrorE fn_8012E16C(u8 nProduct, u32* puTime);
EASBErrorE fn_8012E1E0(u8 nProduct, u16* puLevel);
EASBErrorE fn_8012E25C(u8 nProduct, u32* pOut);
EASBErrorE fn_8012E2D4(u8 nProduct, u32* pOut);
EASBErrorE fn_8012E34C(u8 nProduct, u32* pOut);
EASBErrorE fn_8012E3C0(u8 nProduct, u32* pOut);
EASBErrorE fn_8012E818(u8 n, void* pImage);
EASBErrorE fn_8012E820(u32 uTime, u16* pnDays, u8* pnHours, u8* pnMinutes, u8* pnSeconds);
EASBErrorE fn_8012E8A8(u32 uTime, u16* pnYear, u8* pnMonth, u8* pnDay, u8* pnHours, u8* pnMinutes,
                       u8* pnSeconds);

// ---- EASportsBio.c: the game's side ----

// The game's Bio manager (TW06's EASBio_MgrT; same fields at the same offsets).
typedef struct EASBioMgr {
    void* pCurrentGameImage;        // 0x00
    void* pOurGameImage;            // 0x04
    void* pIcon;                    // 0x08
    u8 bBioLoaded;                  // 0x0C
    u8 bGameWon;                    // 0x0D
    u16 uCurLevel;                  // 0x0E
    u8 bNewAccomplishment;          // 0x10
    u8 unk11[3];
    s32 eCurrentRewardMessage;      // 0x14
} EASBioMgr;

// What the front end tells the player after a round (TW06's EASBio_eReward).
typedef enum EASBio_eReward {
    EASBio_eReward_None = -1,
    EASBio_eReward_LeveledUpAndUnlocked = 0,
    EASBio_eReward_LeveledUp = 1,
    EASBio_eReward_NewAccomplishment = 2
} EASBio_eReward;

// Declared in engine.h: EASBio_SetGamePlayState, EASBio_IncrementGamesWon,
// EASBio_SetCurrentGameWon, EASBio_IsCurrentGameWon.
u8 EASBio_IsBioLoaded(void);
void EASBio_SetCurrentRewardMessage(s32 eReward);
s32 EASBio_GetCurrentRewardMessage(void);
EASBio_eReward fn_801256B8(void);
void EASBio_IncrementGamesPlayed(u32 uCount);
void EASBio_SetAccomplishment(const char* szName, s32 arg1);

// The memory-card screens' Bio requests. nPort and nSlot are the card the menus picked; most of
// these do not use them (the Bio always goes to card 0).
f32  fn_80124BDC(void);
s32  fn_80125194(s32 nPort, s32 nSlot);
s32  fn_80125280(s32 nPort, s32 nSlot);
s32  fn_801252D0(s32 nPort, s32 nSlot);
s32  fn_80125354(s32 nPort, s32 nSlot);
s32  fn_801253F0(s32 nPort, s32 nSlot);
s32  fn_80125434(s32 nPort, s32 nSlot);
s32  fn_801254B8(void);
s32  fn_801254EC(void);
u8   fn_80125528(s32 nPort, s32 nSlot);
void fn_80125600(s32 a0, s32 a1);
void fn_80125648(s32 p0, s32 p1, s32 p2);
void fn_8012566C(s32 p0);
void fn_80125680(s32 p0);
s32  fn_80125928(void);             // the Bio's level (0: none loaded)

extern EASBioMgr* lbl_80281988;
extern EASBInitParams lbl_80261040;
extern char lbl_80195308[28];       // "Tiger Woods PGA TOUR(R) 2004"
extern u16 lbl_80195324[14];        // "Rounds Played", 16-bit characters
extern s32 EASB_gErrorMap[35];      // the memory-card error code for each EASBErrorE
extern u8 lbl_80282568;
extern u8 lbl_80282569;
extern u8 lbl_8028256A;
extern s32 lbl_8028256C;
extern s32 lbl_80282570;
extern s32 lbl_80282574;
extern s32 lbl_80282578;
extern u8 lbl_8028257C;
extern u8 lbl_8028257D;

#endif
