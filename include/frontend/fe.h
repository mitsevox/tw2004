// fe.h (our name): the front end, the game's menus: the menu screens (FE_Manager.c), the
// Create-A-Player database (FE_CrAPDB.c), the golfers animated on menu screens (FEgolferanim.c),
// the PGA Tour messages (FE_PGATourMessages.c) and the logo editor (FE_LogoDesign.c).

#ifndef FRONTEND_FE_H
#define FRONTEND_FE_H

#include "game_types.h"
#include "platform.h"

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
