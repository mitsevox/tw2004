// fe.h (our name): the front end, the game's menus: the menu screens (FE_Manager.c), the
// Create-A-Player database (FE_CrAPDB.c), the golfers animated on menu screens (FEgolferanim.c),
// the PGA Tour messages (FE_PGATourMessages.c) and the logo editor (FE_LogoDesign.c).

#ifndef FRONTEND_FE_H
#define FRONTEND_FE_H

#include "game_types.h"
#include "platform.h"
#include "game/save.h"

// ---- the menu screens (FE_Manager.c) ---------------------------------------------------------

// A movie waiting to be played (0x108 bytes).
typedef struct FEMovie {
    s32 nKind;                  // 0x000  FE_MOVIE_CREDITS or FE_MOVIE_BIO
    u8  unk4[0x104 - 0x4];
    s32 nBio;                   // 0x104  FE_MOVIE_BIO: which one, from 0 ("bio01")
} FEMovie;
LAYOUT_ASSERT(FEMovie, 0x108);

#define FE_MOVIE_CREDITS 1      // "credits"
#define FE_MOVIE_BIO     3      // "bios/bio<nn>"
#define FE_NUM_MOVIES    6      // the length of the movie queue

// The front end's state (lbl_801D7148, 0x660 bytes). Only what the cleaned code reads.
typedef struct FEState {
    u8  aLoaded[5];             // 0x000  per player slot: a profile is loaded (its name is shown)
    u8  aCPU[5];                // 0x005  per player slot: a CPU player (fn_80079AD4 gives it
                                //        CONTROLLER_CPU and no profile)
    s8  aBackup[5];             // 0x00A  per player slot: its row in p658 (-1: none)
    u8  b0F;                    // 0x00F  set by fn_80076E48
    u8  b10;                    // 0x010  set by fn_80076E48
    u8  b11;                    // 0x011  cleared by fn_80079AD4
    u8  unk12[2];
    s32 nMode;                  // 0x014  the game mode the menus start in (fn_80079AD4): the
                                //        session's, or 4, 23, 27 or 28
    u8  b18;                    // 0x018  set by fn_80076E48; cleared by the "THEKITCHENSINK" cheat code
    u8  unk19[3];
    s32 n1C;                    // 0x01C  cleared by fn_80076E48
    s32 nMovieNext;             // 0x020  } the movie queue: the next to play, and where the next
    s32 nMovieFree;             // 0x024  } one is added (equal when it is empty)
    FEMovie aMovies[FE_NUM_MOVIES];     // 0x028
    SaveProfile* p658;          // 0x658  backup copies of the profiles; moved to ARAM and freed
                                //        by fn_80079D30
    u8  unk65C[0x660 - 0x65C];
} FEState;
LAYOUT_ASSERT(FEState, 0x660);

extern FEState lbl_801D7148;

// The front end's screen state (lbl_801D87C0, 0x4C bytes). Only what the cleaned code reads.
typedef struct FEScreen {
    u8  b0;                     // 0x00  set by fn_80079AD4
    u8  a1[9];                  // 0x01  read by a menu message (fn_8007C7EC: 1 for index 9)
    u8  unkA[0x2C - 0xA];
    u8  a2C[4];                 // 0x2C  read and cleared by menu messages
    u8  unk30[0x38 - 0x30];
    s32 n38;                    // 0x38  a menu message reads it (fn_8007DAD4)
    u8  unk3C[0x44 - 0x3C];
    f32 fFade;                  // 0x44  the fade to black before a movie, 0 to 1
    u8  unk48;
    u8  b49;                    // 0x49  FEgolferanim.c's fn_8008EB10 tests it
    u8  unk4A[0x4C - 0x4A];
} FEScreen;
LAYOUT_ASSERT(FEScreen, 0x4C);

extern FEScreen lbl_801D87C0;

// lbl_801D8858 (0x38 bytes), also used by the code at 0x8009170C. Only what the cleaned code reads.
typedef struct FE801D8858 {
    u8  unk0[0x18];
    u8  b18;                    // 0x18
    u8  unk19[0x30 - 0x19];
    s32 n30;                    // 0x30  from fn_8002FD00 (0x8009170C's code)
    u8  unk34[0x38 - 0x34];
} FE801D8858;
LAYOUT_ASSERT(FE801D8858, 0x38);

extern FE801D8858 lbl_801D8858;

// lbl_801D880C (0xC bytes), also read by uiProcessInterface.c. A menu message sets n4 and clears n0.
typedef struct FE801D880C {
    s32 n0;                     // 0x0  0..2; uiProcessInterface.c sets it to -1
    s32 n4;                     // 0x4
    u8  unk8[4];
} FE801D880C;
LAYOUT_ASSERT(FE801D880C, 0xC);

extern FE801D880C lbl_801D880C;

// One of 200 entries (lbl_801D8890); uiProcessInterface.c sets them from lbl_801D8ED0.
typedef struct FE801D8890 {
    u8  b0;                     // 0x0
    u8  b1;                     // 0x1
    u8  unk2[2];
    s32 n4;                     // 0x4
} FE801D8890;
LAYOUT_ASSERT(FE801D8890, 0x8);

#define FE_NUM_801D8890 200
extern FE801D8890 lbl_801D8890[FE_NUM_801D8890];

// The profile being worked on in the menus (lbl_80281ED4 points to it; 0x11708 bytes, allocated
// and cleared by fn_8007744C).
typedef struct FEProfile {
    u8  b0;                     // 0x00000  with game mode 10, the menus start in mode 27
    s8  n1;                    // 0x00001  -1 when it is set up
    s8  nSlot;                  // 0x00002  the player slot whose profile it is
    s8  n3;                     // 0x00003  } set and read by menu messages (FE_MessageTable.c)
    s8  n4;                     // 0x00004  }
    s8  n5;                     // 0x00005  }
    u8  unk6[0x10 - 0x6];
    SaveProfile profile;        // 0x00010  a working copy
    u8  unk10610[0x10620 - 0x10610];
    s8  n10620;                 // 0x10620  read and cleared by menu messages
    u8  unk10621[0x1063F - 0x10621];
    u8  bCopy;                  // 0x1063F  the working copy is the profile, not the slot's own
    u8  b10640;                 // 0x10640
    u8  unk10641[3];
    s32 nDateSeed;              // 0x10644  } fn_80077C1C, from today's date: per b (0, 1) and
    s16 aKind[2][3];            // 0x10648  } category (-1, -2, -3), a random asset kind, and up
    s16 aPart[2][3][5];         // 0x10654  } to five random assets of it (fn_80105FF8's part
    s16 aChoice[2][3][5];       // 0x10690  } and choice; -1: none)
    u8  unk106CC[0x106D0 - 0x106CC];
    u64 uSquareHash;            // 0x106D0  the hash of "__LogoSquare" (the square logo's texture)
    u64 uRectHash;              // 0x106D8  the hash of "__LogoRect"
    u8  unk106E0[0x11702 - 0x106E0];
    u8  b11702;                 // 0x11702
    u8  b11703;                 // 0x11703
    s32 n11704;                 // 0x11704
} FEProfile;
LAYOUT_ASSERT(FEProfile, 0x11708);

extern FEProfile* lbl_80281ED4;

// One golfer's bio in the 'BIO ' stream object, as the menus show it (FE_MessageTable.c
// fn_8007CE7C, fn_8007CF4C, fn_8007D028).
typedef struct FEBio {
    s32  nId;                   // 0x000  the golfer it describes (fn_8007CE7C searches on it)
    char sz4[0x20];             // 0x004  } texts the menus show
    char sz24[0x10];            // 0x024  }
    s32  a34[6];                // 0x034  numbers the menus show
    char sz4C[0x1C];            // 0x04C
    s32  n68;                   // 0x068
    s32  nCourse;               // 0x06C  shown as the course's name (lbl_80191990); -1: "N/A"
    char sz70[0x28];            // 0x070
    char sz98[0x160];           // 0x098  up to five lines, split at '\n'
} FEBio;
LAYOUT_ASSERT(FEBio, 0x1F8);

extern s32 lbl_801894E8[16];            // golfer ids fn_80080388 counts as unlocked

#define FE_NUM_BIOS 29
extern FEBio* lbl_80281EC8;             // a copy of the 'BIO ' stream object's data (fn_80076F80)

// The profile backups (FEState.p658) can be moved out to ARAM (fn_80079D30) and back (fn_80079DAC).
#define FE_BACKUP_SIZE 0x41820          // the four slots' backups (4 x 0x10600) and 0x20 more
extern u32 lbl_80281ECC;                // their size
extern u32 lbl_80281ED0;                // their ARAM address while they are there (0: not there)

// ARAM (the audio memory, used as spare storage). skalib.c declares its own copies of these.
u32  fn_800B6564(u32 uSize);                          // ARAM alloc
void fn_800B6594(u32 uAram);                          // ARAM free
void fn_800B6844(void* pSrc, u32 uAram, u32 uSize);   // copy to ARAM
void fn_800B68B4(void* pDst, u32 uAram, u32 uSize);   // copy from ARAM
void fn_800B67EC(void);                               // wait for the ARAM copy

// ---- the golfers animated on menu screens (FEgolferanim.c) -----------------------------------

// A state of the golfer loader (lbl_80189AA0): run by fn_8008B864.
typedef struct FEGolferState {
    void (*pfnEnter)(void);     // 0x00
    void (*pfnUpdate)(void);    // 0x04  every frame
    void (*pfnExit)(void);      // 0x08
    void (*pfnAbort)(void);     // 0x0C  fn_8008B704
    s32  nNext;                 // 0x10  the state that follows it
} FEGolferState;
LAYOUT_ASSERT(FEGolferState, 0x14);

#define FE_NUM_GOLFER_STATES 5  // state 0 is empty

// The golfer loader's state machine (lbl_801D8708).
typedef struct FEGolferMachine {
    s32 nNext;                  // 0x0  the state after this one
    s32 nState;                 // 0x4  the running state (0: stopped)
    u8  bDone;                  // 0x8  the state is finished: go to nNext
    u8  bEnter;                 // 0x9  the state's pfnEnter is still to run
    u8  bAbort;                 // 0xA  set by fn_8008B704
    u8  bPaused;                // 0xB
} FEGolferMachine;
LAYOUT_ASSERT(FEGolferMachine, 0xC);

// ---- the Create-A-Player database (FE_CrAPDB.c) ----------------------------------------------

// A created golfer is put together from CRAP_NUM_PARTS parts (headwear, shirts, shoes...), and
// each part offers a list of assets to choose from.
#define CRAP_NUM_PARTS 24

// A Create-A-Player asset (0x118 bytes): a hat, a shirt, a colour... The 'CR_A' stream object is
// the array of them all (fn_80105188). Only what the cleaned code reads.
typedef struct CrAPAsset {
    u8   unk0[4];
    char szName[0x28 - 0x4];    // 0x004  "White", "Bright Red", "... backwards" ...
    s16  nPart;                 // 0x028  the part it is a choice for
    s16  nCategory;             // 0x02A  its category: where the category's name ("Hats",
                                //        "Visors") starts in the 'CR_S' strings (fn_801064EC)
    s16  n2C;                   // 0x02C
    s16  n2E;                   // 0x02E  0..15; fn_801074D4 tests it
    s32  n30;                   // 0x030
    s32  n34;                   // 0x034
    s32  n38;                   // 0x038
    s8   nAttrA;                // 0x03C  } the two attributes it raises (-1: none) and the tier
    s8   nTierA;                // 0x03D  } it raises each to
    s8   nAttrB;                // 0x03E  }
    s8   nTierB;                // 0x03F  }
    s8   n40;                   // 0x040  the database's n4 it is offered with (2: any; fn_801061C8)
    s8   nLockKind;             // 0x041  } how it is unlocked and the number that goes with it
    s16  nLock;                 // 0x042  } (fn_80078008)
    s16  n44;                   // 0x044
    s16  n46;                   // 0x046
    s16  n48;                   // 0x048
    s8   a4A[0x58 - 0x4A];      // 0x04A  indexed by fn_80105644's last argument
    u8   unk58[0x70 - 0x58];
    u64  aPart[4];              // 0x070  } the ids of four skin parts it sets (fn_800CDAFC finds
    u64  aVariant[4];           // 0x090  } them) and the id of each one's variant (fn_80106A64)
    u64  aSet[4];               // 0x0B0  the ids of four skin sets; taking the asset off puts
                                //        them back to "Defaults" (fn_80106DA0)
    u8   unkD0[0x110 - 0xD0];
    s16  n110;                  // 0x110  the offset in 'CR_S' of its unlock text (-1: none; fn_8010651C)
    s16  n112;                 // 0x112  } offsets of strings in 'CR_S' (fn_801064EC); n114 is
    s16  n114;                  // 0x114  } passed to fn_8008E724 with the asset's name (fn_80104094)
    u8   unk116[2];
} CrAPAsset;
LAYOUT_ASSERT(CrAPAsset, 0x118);

// The Create-A-Player database (0x18 bytes, allocated by fn_801037F8).
typedef struct CrAPDB {
    s32  nAssets;               // 0x00  how many assets pAssets holds
    s8   n4;                    // 0x04  which assets are offered (fn_801061C8): fn_80103B8C sets it
    u8   unk5[3];
    CrAPAsset* pAssets;         // 0x08  the 'CR_A' object's data: every asset
    char* pStrings;             // 0x0C  } the 'CR_S' object's data (the names the assets use)
    u32  uStringsSize;          // 0x10  } and its size
    u8   b14;                   // 0x14  1 when allocated; fn_80103B74 sets it
    u8   unk15[3];
} CrAPDB;
LAYOUT_ASSERT(CrAPDB, 0x18);

// A 0x2C-byte record of the Create-A-Player database's table lbl_80282470 (64 of them,
// fn_801037F8); fn_80107244 copies one out.
typedef struct CrAPRecord {
    s16  n0;                    // 0x00
    u8   unk2[2];
    s32  n4;                    // 0x04
    char sz8[0x2C - 0x8];       // 0x08
} CrAPRecord;
LAYOUT_ASSERT(CrAPRecord, 0x2C);

extern CrAPDB* lbl_80282460;
extern UStreamObject* lbl_80282464;     // the 'CR_A' object (the assets), kept until freed
extern UStreamObject* lbl_80282468;     // the 'CR_S' object (their names)
extern s32 lbl_8028246C;                // cleared by fn_801037F8
extern CrAPRecord* lbl_80282470;        // 64 records (fn_80107244); freed by fn_80103A64
extern s32* lbl_80282474;               // per part: the index of its first asset
extern s32* lbl_80282478;               // per part, 24 entries: the categories fn_80104AF4 found
extern s32* lbl_8028247C;               // 0x600 entries, rows 24 apart (fn_80103920 clears 64 from
                                        // each row's start); fn_801048EC stores a part's choice count
extern s32* lbl_80282480;               // 24 entries (fn_80103920 sets them to -1)
extern char lbl_801935C8[16][32];      // 16 names (fn_80107294)
extern s32 lbl_802816E8;                // } an asset to put on and one to take off when
extern s32 lbl_802816EC;                // } fn_80104804 runs (-1: none)
extern char lbl_801932C8[CRAP_NUM_PARTS][32];   // per part: the name of its "All ..." entry that
                                        // lists every category ("All Headwear"), or ""

int  fn_80103B28(int nAsset);           // the asset nAsset takes its attributes from (itself,
                                        // or for lock kind 28 the asset its nLock names)
CrAPAsset* fn_80103B4C(CrAPAsset* pAsset);  // the same, by asset
u8   fn_80103B80(void);                 // the database's b14
void fn_80103B8C(s8 n);                 // set the database's n4 (which assets are offered)
s8   fn_80103BC0(int nAsset);           // an asset's n40
s8   fn_80103BB4(void);                 // the database's n4
int  fn_80103D14(s16 nSlot);            // the profile's aAF80[nSlot], an asset (-1 past slot 52)
int  fn_801049C8(s16 nPart);
void fn_80104804(void);
u8   fn_80104DB8(s16 nPart, int n, char* pDst); // copy the name of a part's entry n (for 0 its
                                        // "All ..." entry when it has one); 0 if there is none
int  fn_80105C44(s16 nPart, int b);
void fn_80105FF8(int nAsset, s16* pKind, s32* pPart, s32* pChoice);
u8   fn_801061C8(s8 n);                 // an asset with this n40 is offered
int  fn_80106244(s16 nPart);            // the asset in the first slot of aAF80 whose asset is of the part
                                        // (-1: none)
u8   fn_8010645C(int nOffset, char* pDst);  // copy a 'CR_S' name ("" for "NONE")
void fn_801072CC(s16 nPart, s32* pLocked, s32* pB1CC, s32* pB344, s32* pAll);  // count a part's
                                        // offered assets: locked, with each bit set, and all
u8   fn_801074D4(int nAsset);
s16  fn_8010742C(int nAsset);           // the part an asset is a choice for
int  fn_80107444(int nAsset);           // an asset's n38
u8   fn_80104020(int nAsset);           // the asset may be picked: not locked when last checked,
                                        // and its aB1CC bit is set
int  fn_801048EC(s16 nPart, int b);     // how many choices a part has
int  fn_80104FA8(s16 nPart, int b, int i);          // a part's choice i: the asset's index
CrAPAsset* fn_80104E84(s16 nPart, int b, int i);    // a part's choice i: the asset
CrAPAsset* fn_80104F68(int nAsset);     // an asset by index
int  fn_80105494(int nAsset);           // } the two attributes an asset raises (-1: none)
int  fn_80105504(int nAsset);           // }
int  fn_801054CC(int nAsset);           // } and the tier it raises each to
int  fn_8010553C(int nAsset);           // }
s8   fn_801055DC(int nAsset);           // } an asset's lock kind and number (-1: no such
s16  fn_80105610(int nAsset);           // } asset)
s32  fn_80105C00(void);                 // how many assets there are
u8   fn_80105C30(void);                 // the Create-A-Player database is allocated
char* fn_801064EC(int nCategory);       // a category's name
int  stricmp(const char* a, const char* b);           // 0x8015F844 (MSL): strcmp ignoring case

void FE_MakeMoviePath(char* pName, char* pPath);        // "data/movies/<name>.NGC"
void FE_MakeCameoMoviePath(char* pName, char* pPath);   // "data/movies/cameos/<name>.NGC"
void FE_CrAP_TurnOnPart(s16 nPart, int b, int c);      // FE_CrAPDB.c
SaveProfile* fn_80077ACC(void);         // the profile being worked on
int  fn_80078604(int a, int b, int c);  // a date (month, day, year from fn_8011E020) packed
int  fn_80077B08(void);                 // its player slot
u8   fn_80077B18(int nGolfer);          // a yes/no list over golfers 0..28 (Golfer.c asks it)
void fn_80077B78(void);                 // pick the day's random assets (fn_80077C1C)
FEMovie* fn_800770FC(void);             // the next free place in the movie queue
void fn_80077780(void);
void fn_80077808(int nSlot);
void fn_80077968(int nSlot);
void fn_800779BC(int a, int b);         // swap backup rows a and b (p658)
GolferRecord* fn_80077A80(int nGolfer); // a golfer's record (created golfers: the profile's)
void fn_80078620(int n, int* pA, int* pB, int* pC);     // unpack n = b * 1000000 + a * 10000 + c
void fn_80079AD4(void);
void fn_800A75B4(void);                 // (0x800A75B4) FE_Manager.c calls it after queueing a movie

// ---- the menus' message table (FE_MessageTable.c) --------------------------------------------

void fn_80079EA8(void);                 // fill the table
void fn_80084FF0(int n);                // sets lbl_80281FFC

extern char* lbl_80191990[30];          // per course: a string the menus show (a replay's course
                                        // picks it)
extern s32 lbl_80281FFC;                // set by fn_80084FF0: the lbl_8018C7D8 set (memcard.h) the
                                        // menus' memory-card messages use

// ---- the golfers animated on menu screens (FEgolferanim.c) ------------------------------------

int  fn_8008B990(void);
void fn_8008DAEC(void);
void fn_8008E244(void);
void fn_8008E364(int n);
int  fn_8008E420(void);
int  fn_8008E44C(void);
char* fn_8008E6BC(void);
void fn_8008E6D4(int n);
void fn_8008E724(char* szAnim, char* szShot, s8 n, u8 bLoop);
void fn_8008E824(void);
void fn_8008E860(int n);
u8   fn_8008E944(u8 b, f32 f);
void fn_8008EB70(void);

// ---- the logo editor (FE_LogoDesign.c) -------------------------------------------------------

// A logo is 8-bit colour indexes into a 256-colour palette, either 64 x 64 (shape 0, drawn into
// the texture "__LogoSquare") or 128 x 32 (shape 1, "__LogoRect").
#define LOGO_SQUARE 0
#define LOGO_RECT   1

// The logo being edited (12 bytes, allocated by fn_8010F748).
typedef struct LogoEdit {
    s32 n0;                     // 0x0  which logo: fn_8010FB70 picks its 0x1022-byte record by it
    s32 nShape;                 // 0x4  LOGO_SQUARE or LOGO_RECT
    u8  bDirty;                 // 0x8  changed since it was last copied into its texture
} LogoEdit;
LAYOUT_ASSERT(LogoEdit, 0xC);

// A saved logo (0x1022 bytes): the profile holds five (ProfileLogos) and fn_8010FB70 picks the
// one LogoEdit.n0 names.
typedef struct LogoRecord {
    u8   aPixels[0x1000];       // 0x0000  64 x 64 or 128 x 32 colour indexes
    char szName[0x20];          // 0x1000
    u8   b1020;                 // 0x1020
    u8   nShape;                // 0x1021  LOGO_SQUARE or LOGO_RECT
} LogoRecord;
LAYOUT_ASSERT(LogoRecord, 0x1022);

// The part of the save profile from 0x5500 that char_tex_manager.c is given (fn_80077ACC() +
// 0x5500); only the logos are known.
typedef struct ProfileLogos {
    u8  unk0[0x9D0];
    LogoRecord aLogo[5];        // 0x09D0  the user logos ("_usrtextr0".."_usrtextr4")
} ProfileLogos;

extern LogoEdit* lbl_802824B8;
extern s16* lbl_802824BC;               // the palette: 256 colours, 1-bit alpha (the sign bit)
                                        // and 5-5-5 RGB; read signed (lha)
extern u8 lbl_802824C0;                 // the palette has been copied from "__LogoSquare"

void fn_8010F7C0(s32 n);                // pick the logo to edit (LogoEdit.n0)
s32  fn_8010F7D8(void);                 // which logo is edited
void fn_8010F7E4(s32 nShape);           // set its shape
void fn_8010F7FC(int nColor, u32* pR, u32* pG, u32* pB, u32* pA);  // a palette colour, 0-255 each
void fn_8010F880(void);                 // mark the logo changed
void fn_8010F890(char* pName);          // load the logo from a texture
void fn_8010F90C(int nX, int nY, int nColor);   // set a pixel
LogoRecord* fn_8010FB70(void);          // the logo being edited
s16* fn_8010FBC4(void);                 // the palette
int  fn_8010FBCC(int nX, int nY, u32* pR, u32* pG, u32* pB, u32* pA);  // a pixel's colour index,
                                        // and its colour as fn_8010F7FC gives it
void fn_8010FC3C(u8* pDst, u8* pSrc, int a, int nWidth, int nHeight);   // copy pixels: a = 0
                                        // from a texture into the logo, 1 from the logo into one
u8*  fn_8010FF5C(u8* pLogo, int nWidth, int nHeight);   // the logo's pixels as a texture (in
                                        // lbl_80212B60)

extern u8 lbl_80212B60[64 * 64];        // a logo's pixels laid out as a texture (fn_8010FF5C);
                                        // 64 x 64 or 128 x 32

#endif
