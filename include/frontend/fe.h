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
    u8  unk48[0x4C - 0x48];
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
extern u8* lbl_80281EC8;                // a copy of the 'BIO ' stream object's data (fn_80076F80)

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
    u8   unk2E[2];
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
    u8   unk58[0x118 - 0x58];
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

extern CrAPDB* lbl_80282460;
extern UStreamObject* lbl_80282464;     // the 'CR_A' object (the assets), kept until freed
extern UStreamObject* lbl_80282468;     // the 'CR_S' object (their names)
extern s32* lbl_80282474;               // per part: the index of its first asset
extern char lbl_801932C8[CRAP_NUM_PARTS][32];   // per part: the name of its "All ..." entry that
                                        // lists every category ("All Headwear"), or ""

int  fn_80103B28(int nAsset);           // the asset nAsset takes its attributes from (itself,
                                        // or for lock kind 28 the asset its nLock names)
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
int  fn_80077B08(void);                 // its player slot
u8   fn_80077B18(int nGolfer);          // a yes/no list over golfers 0..28 (Golfer.c asks it)
void fn_80077B78(void);                 // pick the day's random assets (fn_80077C1C)
FEMovie* fn_800770FC(void);             // the next free place in the movie queue
void fn_80077780(void);
void fn_80077808(int nSlot);
void fn_80077968(int nSlot);
GolferRecord* fn_80077A80(int nGolfer); // a golfer's record (created golfers: the profile's)
void fn_80079AD4(void);
void fn_800A75B4(void);                 // (0x800A75B4) FE_Manager.c calls it after queueing a movie

// ---- the menus' message table (FE_MessageTable.c) --------------------------------------------

void fn_80079EA8(void);                 // fill the table
void fn_80084FF0(int n);                // sets lbl_80281FFC

extern char* lbl_80191990[30];          // per course: a string the menus show (a replay's course
                                        // picks it)
extern s32 lbl_80281FFC;                // set by fn_80084FF0

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

// A saved logo (0x1022 bytes).
typedef struct LogoRecord {
    u8  aPixels[0x1000];        // 0x0000  64 x 64 or 128 x 32 colour indexes
    u8  unk1000[0x1021 - 0x1000];
    u8  nShape;                 // 0x1021  LOGO_SQUARE or LOGO_RECT
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

u8*  fn_8010FB70(void);                 // the logo's pixels
s16* fn_8010FBC4(void);                 // the palette
void fn_8010FC3C(u8* pDst, u8* pSrc, int a, int nWidth, int nHeight);   // copy pixels: a = 0
                                        // from a texture into the logo, 1 from the logo into one

#endif
