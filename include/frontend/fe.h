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
    u8  a5[5];                  // 0x005
    s8  aBackup[5];             // 0x00A  per player slot: its row in p658 (-1: none)
    u8  unkF[0x20 - 0xF];
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
    u8  unk0[0x44];
    f32 fFade;                  // 0x44  the fade to black before a movie, 0 to 1
    u8  unk48[0x4C - 0x48];
} FEScreen;
LAYOUT_ASSERT(FEScreen, 0x4C);

extern FEScreen lbl_801D87C0;

// The profile being worked on in the menus (lbl_80281ED4 points to it).
typedef struct FEProfile {
    u8  unk0[2];
    s8  nSlot;                  // 0x00002  the player slot whose profile it is
    u8  unk3[0x10 - 0x3];
    SaveProfile profile;        // 0x00010  a working copy
    u8  unk10610[0x1063F - 0x10610];
    u8  bCopy;                  // 0x1063F  the working copy is the profile, not the slot's own
} FEProfile;

extern FEProfile* lbl_80281ED4;
extern u8* lbl_80281EC8;                // a copy of the 'BIO ' stream object's data (fn_80076F80)

void FE_MakeMoviePath(char* pName, char* pPath);        // "data/movies/<name>.NGC"
void FE_MakeCameoMoviePath(char* pName, char* pPath);   // "data/movies/cameos/<name>.NGC"
void FE_CrAP_TurnOnPart(int a, int b, int c);          // FE_CrAPDB.c
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
