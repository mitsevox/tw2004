// SitDevFile.c (EA's name, from its asserts; TW06): a watcher that follows the ball after a shot
// (an event 48 frames in, a call when it reaches surface 105), the loading of the situation
// scripts into the block lbl_802811B8 points at (sitdev.h), and the values the scripts test.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    lbl_80282200;     // 0x80282200  the watched ball has reached surface 105
Ball* lbl_802821FC;     // 0x802821FC  the watched ball, NULL for none
u32   lbl_802821F8;     // 0x802821F8  gSession.nFrameCount when it started

void fn_80067710(int nPlayer, int a, int b);   // also declared in Swing.c; belongs in a header

// ---- the watched ball ----------------------------------------------------------------------

void fn_800BB0DC(void);

void fn_800BB0A8(void) {
    fn_800BB0DC();
}

// Stop watching the ball.
void fn_800BB0C8(void) {
    lbl_802821FC = NULL;
}

// Whether the ball has reached surface 105 (see fn_800BB0E8).
u8 fn_800BB0D4(void) {
    return lbl_80282200;
}

void fn_800BB0DC(void) {
    lbl_80282200 = 0;
}

// Per frame while a ball is watched: 48 frames after it was hit, while it is flying or rolling,
// trigger event 0x4B for its player; the first time it lands on surface 105, call
// fn_80067710(player, 0, 0x22).
void fn_800BB0E8(void) {
    if (lbl_802821FC == NULL) return;
    switch (lbl_802821FC->nState) {
    case 2:
    case 3:
    case 4:
        if (48.0f == (f32)(gSession.nFrameCount - lbl_802821F8)) {
            EVENT_Trigger(lbl_802821FC->nPlayer, 0x4B, NULL, -1);
        }
        break;
    }
    if (lbl_802821FC->nSurface == 105 && !lbl_80282200) {
        fn_80067710(lbl_802821FC->nPlayer, 0, 0x22);
        lbl_80282200 = 1;
    }
}

// Start watching a ball: remember it and the frame it was hit on.
void fn_800BB1A8(Ball* pBall) {
    lbl_802821FC = pBall;
    lbl_802821F8 = gSession.nFrameCount;
}

void fn_800BB1C0(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_801FA198[i] = 0;
        lbl_801FA1AC[i] = 0;
    }
}

u8 fn_800BB1F8(int nPlayer) {
    return lbl_801FA1AC[nPlayer] == 1;
}

// ---- scripts -------------------------------------------------------------------------------

void fn_800BB4B0(void) {
}

// Set value 5 of the shared block.
void fn_800BBADC(int nValue) {
    fn_80067B1C(lbl_802811B8->aValue, 5, (u16)nValue, lbl_802811B8->aSetBits);
}

// ---- the values the scripts test -----------------------------------------------------------

void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer);
void fn_800BCB74(s32* pClass, int nSurface);
int  fn_800BCB88(void);
u8   fn_800BCBE0(void);
u8   fn_800BCC38(void);
u8   fn_800BCC48(void);
s32  fn_800BCCA0(int nPlayer);
s32  fn_800BCCCC(int nPlayer);
s32  fn_800BCCF8(int nPlayer);
u8   fn_800BCD24(int nPlayer);
u8   fn_800BCD50(void);
s32  fn_800BCD5C(void);
void fn_800BD77C(u16 uSound);
void fn_800BD7D0(u8 nMusic);
void fn_800BD7E8(u16 uSound);
void fn_800BD868(int nSound, int a);

// Correct the surface class of where a ball lies (SurfaceType.nClass) for the scripts: outside the
// course outline, or on ground a ball may not stay on, is 19 (not playable) unless it is water; a
// ball that must be dropped counts as water (7); surface 151 is 21; class 18 (green) reads as 12.
void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer) {
    u8 bWater;
    u8 bNoLie;
    bWater = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES &&
             (gSurfaceTypes[nSurface].nClass == 7 || gSurfaceTypes[nSurface].nClass == 16);
    bNoLie = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES && !(gSurfaceTypes[nSurface].u34 & 1) &&
             (gSurfaceTypes[nSurface].u34 & 2);
    if ((!Ter_PointInOOBNetwork(pBall->vPos) && !bWater) || (bNoLie && !bWater)) {
        *pClass = 19;
    }
    if (pPlayer->b30E) {
        *pClass = 7;
    }
    if (nSurface == 151) {
        *pClass = 21;
    }
    if (*pClass == 18) {
        *pClass = 12;
    }
}

// Surface 151 is class 21 (the same test as in fn_800BCA60).
void fn_800BCB74(s32* pClass, int nSurface) {
    if (nSurface == 151) {
        *pClass = 21;
    }
}

// 1, 3 or 2 by which of lbl_802811F0's flags are set (fn_800BCC48, fn_800BCBE0, flag 0x2 alone),
// otherwise 0.
int fn_800BCB88(void) {
    int nResult;
    if (fn_800BCC48()) {
        nResult = 1;
    } else if (fn_800BCBE0()) {
        nResult = 3;
    } else if (fn_80035574()) {
        nResult = 2;
    } else {
        nResult = 0;
    }
    return nResult;
}

// Neither flag 0x2 nor b14 is set, and u04's flag 0x2 is.
u8 fn_800BCBE0(void) {
    int bResult = 0;
    if (!fn_80035574() && !lbl_802811F0->b14 && fn_800BCC38()) {
        bResult = 1;
    }
    return bResult;
}

u8 fn_800BCC38(void) {
    return lbl_802811F0->u04 & 2;
}

// Flag 0x2 is set, and u04's flag 0x2 is clear or b14 is set.
u8 fn_800BCC48(void) {
    int bResult = 0;
    if (fn_80035574() && (!fn_800BCC38() || lbl_802811F0->b14)) {
        bResult = 1;
    }
    return bResult;
}

// The game mode's answers for the scripts (GameState's callbacks).
s32 fn_800BCCA0(int nPlayer) {
    return gpGame->pfn204(nPlayer);
}

s32 fn_800BCCCC(int nPlayer) {
    return gpGame->pfn208(nPlayer);
}

s32 fn_800BCCF8(int nPlayer) {
    return gpGame->pfn200(nPlayer);
}

u8 fn_800BCD24(int nPlayer) {
    return gpGame->pfn1FC(nPlayer);
}

u8 fn_800BCD50(void) {
    return gpGame->bD4;
}

s32 fn_800BCD5C(void) {
    return gpGame->nDC;
}

// Clear the scripts' per-entry bytes.
void fn_800BD74C(void) {
    u32 i;
    for (i = 0; i < lbl_80282208->n10; i++) {
        lbl_802811B8->pD4[i] = 0;
    }
}

// ---- sounds and music ----------------------------------------------------------------------

// Hand GameEffects a sound to stop later (u48), unless one is waiting already; not in mode 11.
void fn_800BD77C(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b47) {
        lbl_80202898.u48 = uSound;
        lbl_80202898.b47 = 1;
    }
}

// Tell GameEffects which music to go back to.
void fn_800BD7D0(u8 nMusic) {
    lbl_80202898.b4E = 1;
    lbl_80202898.n4F = nMusic;
}

// The same as fn_800BD77C with GameEffects' second slot (u4C).
void fn_800BD7E8(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b4A) {
        lbl_80202898.u4C = uSound;
        lbl_80202898.b4A = 1;
    }
}

void fn_800BD83C(int nSound, int a) {
    fn_800A7664(0, nSound, a);
}

void fn_800BD868(int nSound, int a) {
    fn_800A7664(2, nSound, a);
}
