// SitDevMisc.c (EA file, TW06/TW07): own unit, its .sbss is padded to 8 at 0x80282201..0x80282208

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

s32 lbl_801910F8[28] = {
    0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1, 7, 8, 9, 10, 11, 12, 13, 14,
    -1, 0,
};

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    lbl_80282200;     // 0x80282200  the watched ball has reached surface 105
Ball* lbl_802821FC;     // 0x802821FC  the watched ball, NULL for none
u32   lbl_802821F8;     // 0x802821F8  gSession.nFrameCount when it started

s32 lbl_801FA1AC[5];
s32 lbl_801FA198[5];

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

// ---- picking without repeats ---------------------------------------------------------------

// A list of nCount u16 values used as a deck: the top bit marks one already drawn, 0xFFF0 is an
// empty slot.

// The slots in use.
int fn_800BB218(u16* pList, int nCount) {
    int i;
    int nUsed = 0;
    for (i = 0; i < nCount; i++) {
        if (pList[i] != 0xFFF0) {
            nUsed++;
        }
    }
    return nUsed;
}

// The values not drawn yet; when every one is drawn, put them all back and return nCount.
int fn_800BB248(u16* pList, int nCount) {
    int nLeft = 0;
    int i;
    u16* p = pList;
    for (i = 0; i < nCount; i++) {
        if ((*pList & 0x8000) != 0x8000) {
            nLeft++;
        }
        pList++;
    }
    if (nLeft == 0) {
        i = nCount;
        while (i-- > 0) {
            *p &= 0x7FFF;
            p++;
        }
        return nCount;
    }
    return nLeft;
}

// Draw the nPick'th value not drawn yet (nLeft: fn_800BB248's count) and mark it drawn.
u32 fn_800BB334(u16* pList, int nCount, int nLeft, u32 nPick) {
    int i;
    u32 n = 0;
    for (i = 0; i < nCount; i++) {
        if (!(*pList & 0x8000)) {
            if (nPick == n) {
                nPick = *pList;
                *pList |= 0x8000;
                break;
            }
            n++;
        }
        pList++;
    }
    return nPick;
}

// The player's place in the PGA Tour event (1 outside mode 23) as a band: 0 for the top 3,
// 1 for the top 10, 2 for the top 25, 3 below.
int fn_800BB37C(int nPlayer) {
    int nRank;
    if (Game_GetMode() == 23) {
        nRank = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, 0);
    } else {
        nRank = 1;
    }
    if (nRank <= 3) return 0;
    if (nRank <= 10) return 1;
    return nRank <= 25 ? 2 : 3;
}

// The game mode's bit (lbl_801910F8), 0 for a mode without one.
u16 fn_800BB3F8(int nMode) {
    s32 nBit = lbl_801910F8[nMode];
    return nBit == -1 ? 0 : 1 << nBit;
}
