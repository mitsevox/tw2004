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
    u8  unkF[0x11 - 0xF];
    u8  b11;                    // 0x011  cleared by fn_80079AD4
    u8  unk12[2];
    s32 nMode;                  // 0x014  the game mode the menus start in (fn_80079AD4): the
                                //        session's, or 4, 23, 27 or 28
    u8  unk18[0x20 - 0x18];
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
    u8  unk1[0x44 - 0x1];
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
    u8  unk3[0x10 - 0x3];
    SaveProfile profile;        // 0x00010  a working copy
    u8  unk10610[0x1063F - 0x10610];
    u8  bCopy;                  // 0x1063F  the working copy is the profile, not the slot's own
    u8  b10640;                 // 0x10640
    u8  unk10641[0x106D0 - 0x10641];
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

// A Create-A-Player asset (a hat, a shirt, a colour...). Only what the cleaned code reads.
typedef struct CrAPAsset {
    u8   unk0[4];
    char szName[0x28 - 0x4];    // 0x04  "White", "Bright Red", "... backwards" ...
    u8   unk28[2];
    s16  nCategory;             // 0x2A  fn_801064EC gives the category's name ("Hats", "Visors")
} CrAPAsset;

char* fn_801064EC(int nCategory);       // a category's name
int  fn_8015F844(const char* a, const char* b);       // strcmp ignoring case (MSL's __lower_map)

void FE_MakeMoviePath(char* pName, char* pPath);        // "data/movies/<name>.NGC"
void FE_MakeCameoMoviePath(char* pName, char* pPath);   // "data/movies/cameos/<name>.NGC"
void FE_CrAP_TurnOnPart(s16 nPart, int b, int c);      // FE_CrAPDB.c
SaveProfile* fn_80077ACC(void);         // the profile being worked on
int  fn_80077B08(void);                 // its player slot
u8   fn_80077B18(int nGolfer);          // a yes/no list over golfers 0..28 (Golfer.c asks it)

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

extern LogoEdit* lbl_802824B8;
extern s16* lbl_802824BC;               // the palette: 256 colours, 1-bit alpha (the sign bit)
                                        // and 5-5-5 RGB; read signed (lha)
extern u8 lbl_802824C0;                 // the palette has been copied from "__LogoSquare"

u8*  fn_8010FB70(void);                 // the logo's pixels
s16* fn_8010FBC4(void);                 // the palette
void fn_8010FC3C(u8* pDst, u8* pSrc, int a, int nWidth, int nHeight);   // copy pixels: a = 0
                                        // from a texture into the logo, 1 from the logo into one

#endif
