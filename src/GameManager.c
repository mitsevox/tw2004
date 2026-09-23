// GameManager.c (our name; TW06's functions here are GM_*): the round's bookkeeping - turns,
// strokes, mulligans, the post-shot reaction, walking to the ball, the in-the-hole display.
// TW06's copy of the file keeps the same function order, which gives most of the names below
// (docs/tw06-names.md); each one is checked against what our code does.

#include "golfer.h"

typedef struct View View;

void  fn_800E58B4();
s32   fn_800E0A84(s32);
s32   fn_800E1074();
s32   fn_800E1434();
u8    fn_800EC550(void);
s32   fn_800D29E8();
s32   fn_800D33F0();
s32   fn_800E3BEC();
s32   fn_800EADD8();
s32   fn_800EDE78();
f32   fn_800D0478(int nPlayer);             // the ball's distance from the pin (yards)
int   fn_800E177C(void);
void  fn_800E2470(void);
void  fn_800E1480(int nHole);
void  fn_8006F4B4(void);
void  fn_800170C4(int nView, int a);
void  fn_800E299C(void);
void  Wind_Generate(void);
void  GameEffects_ResetGameEffectSettings(void);
void  fn_800E3B28(void);
void  fn_800DA36C(void);
void  fn_800DE828(void);
void  fn_800D8D38(int nPlayer);
void  EVENT_Trigger(int nPlayer, int nEvent, int a, int b);
void  Caddie_Stop(void);
void  fn_8001D7A4(int nHandle);
void  fn_800E4204(void);
u8    fn_800E0A90(int nPlayer);
u8    fn_800E3A54(void);
u8    Player_IsHoled(int nPlayer);
void  GOLFERSTATE_Set(int nState, int nPlayer);
u8    fn_800E1CA8(void);
void  fn_800D439C(int nPlayer, int a);
void  fn_800D9834(int nPlayer);
void  fn_80125910(int a);
u8    fn_8012591C(void);
void  fn_80125854(int a);
void  fn_800E4D94(int a);
void  fn_800E4C20(int a);
u8    fn_800E4BF8(void);
void* fn_80017028(int nView);
void  View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);
void  fn_800E3D90(void);
void  fn_80062D6C(int a, int nPlayer);
void  fn_800E3D38(int nPlayer, int a);
u8    fn_800EE470(void);
void  fn_8011989C(int nPlayer, int nStrokes);
void  fn_800DEC34(int nPlayer);
void  GM_EndOfGolferTurn_HoleFinished(int nPlayer);
void  GM_EndOfGolferTurn_GameFinished(int nPlayer);
void  GM_HoleFinished_GameNotFinished(int nPlayer);
u8    GM_CheckForAIConcede(int nPlayer);

u8    fn_800E2B40(int nPlayer, u8* pBall);
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, u8* pBall);
u8    Ter_PointInFreeDropNetwork(u8* pBall);
void  fn_800E4164(int nMessage, int nPlayer, f32 f);
void  fn_80063CF0(void* pView, int nCamera, int nPlayer);
double fn_80009680(double x);               // sqrt
u8    Ter_CheckObjectAndHazardObstruction(u8* pBall, int a, int b, int c, f32 f1, f32 f2, f32 f3);
u8    Ter_SearchAreaForDropLocation(int nPlayer, int a, int b, f32* pOut);
u8    Physics_DropBall(u8* pBall, f32* pPos);
void  fn_80055AA8(u8* pBall, f32* pPos, int nPlayer);

extern u8  lbl_80202898[];
extern s32 lbl_80282278;
extern u8  lbl_8028227C;
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)

f32 GameEffects_GetLetterboxHeight(void) {
    if (lbl_80202898[0x12] != 0) {
        if (*(f32*)(lbl_80202898 + 0x1C) < 0.8f) {
            return 0.15f * (*(f32*)(lbl_80202898 + 0x1C) / 0.8f);
        }
        return 0.15f;
    }
    return 0.0f;
}

void fn_800DCAD8(void) {
    fn_800E58B4(50);
}

void GM_vCloseModuleONCE(void) {
}

s32 fn_800DCB00(void) {
    return 0;
}

s32 fn_800DCB08(void) {
    return 0;
}

void fn_800DCB10(int nPlayer) {
    gpGame->pfn1F8(nPlayer);
}

u8 fn_800DCB3C(void) {
    if (*(s32*)(lbl_80202898 + 0x2C) == 0) {
        return 1;
    }
    return (*(s32*)(lbl_80202898 + 0x28) % *(s32*)(lbl_80202898 + 0x2C)) == 0;
}

s32 fn_800DCB74(void) {
    return *(u8*)(lbl_80202898 + 0x11);
}

void fn_800DCBA8(void) {
    lbl_8028227C = 0;
    lbl_80282278 = 0;
}

void fn_800DCBB8(void) {
}

void fn_800DCBBC(void) {
    if (fn_800EC550() == 0) {
        fn_800E1074();
        fn_800E1434();
    }
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1EC))(gpGame);
    fn_800E0A84(1);
}

void fn_800DCC04(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1F0))(gpGame);
}

void fn_800DCC30(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1CC))(gpGame);
    fn_800E3BEC();
    fn_800EDE78();
    fn_800EADD8();
    fn_800D33F0();
    fn_800D29E8();
}

// TW06: GM_GotoNextSelectedHole. Moves to the next hole this round plays, if there is one.
int GM_GotoNextSelectedHole(void) {
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (fn_800E177C() == 2 && gpGame->nCurHole < 9 && i >= 9) {
                fn_800E2470();
            }
            fn_800E1480(i);
            fn_8006F4B4();
            return 1;
        }
    }
    return 0;
}

// TW06: GM_InitForHole. A new hole: wind, the mode's hole-start hook, effects, every player's
// per-hole state, and a set of per-player flags cleared.
void GM_InitForHole(void) {
    int i;
    fn_800170C4(2, 0);
    fn_800170C4(3, 0);
    fn_800E299C();
    Wind_Generate();
    gpGame->pfn1E4();
    GameEffects_ResetGameEffectSettings();
    fn_800E3B28();
    fn_800DA36C();
    if (gpGame->b27F) {
        fn_800DE828();
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_800D8D38(i);
    }
    gpGame->b134 = 0;
    lbl_80282278 = gpGame->pfn1D4(5);
    EVENT_Trigger(0xFF, 0, 0, -1);
    for (i = 0; i < 5; i++) {
        gpGame->n144[i] = 0;
        gpGame->n158[i] = 0;
        gPlayers[i].unkC2F = 0;
        gPlayers[i].unkC2D = 0;
    }
}

// The end of a golfer's turn: the caddie stops, the mode is told, then either the hole is over
// (for everyone, or this player gave up) or, when the mode lets CPUs concede, a CPU far enough
// behind concedes; otherwise the golfer waits (state 19).
void GM_EndOfGolferTurn(int nPlayer) {
    u8 bWait;
    Caddie_Stop();
    gpGame->pfn248(nPlayer);
    fn_8001D7A4(gPlayers[nPlayer].nShotHandle);
    EVENT_Trigger(nPlayer, 4, 0, -1);
    fn_800E4204();
    if (gpGame->pfn1D8(nPlayer, 0) || fn_800E0A90(nPlayer)) {
        GM_EndOfGolferTurn_HoleFinished(nPlayer);
        return;
    }
    bWait = 0;
    if (fn_800E3A54()) {
        if (Player_IsHoled(nPlayer)) {
            bWait = 1;
        }
    } else if (Game_GetMode() == 0x1A) {
        bWait = 1;
    } else if (gpGame->bAIConcedes) {
        if (GM_CheckForAIConcede(nPlayer)) {
            fn_800DEC34(nPlayer);
        } else {
            bWait = 1;
        }
    } else {
        bWait = 1;
    }
    if (bWait) {
        GOLFERSTATE_Set(GS_WAIT, nPlayer);
    }
}

// TW06: GM_EndOfGolferTurn_HoleFinished.
void GM_EndOfGolferTurn_HoleFinished(int nPlayer) {
    int i;
    EVENT_Trigger(nPlayer, 1, 0, -1);
    gpGame->pfn1E8();
    if (fn_800E1CA8() && !gpGame->bD4 && !fn_800E0A90(nPlayer)) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            fn_800D439C(i, 0);
            fn_800D9834(i);
        }
    }
    if (gpGame->pfn1DC(0) || fn_800E0A90(nPlayer)) {
        GM_EndOfGolferTurn_GameFinished(nPlayer);
        return;
    }
    GM_HoleFinished_GameNotFinished(nPlayer);
}

// TW06: GM_EndOfGolferTurn_GameFinished.
void GM_EndOfGolferTurn_GameFinished(int nPlayer) {
    int i;
    int nView;
    EVENT_Trigger(nPlayer, 5, 0, -1);
    gpGame->b28E = 1;
    fn_80125910(0);
    gpGame->pfn1F4();
    if (fn_8012591C() && gSession.unk8[0] == 0) {
        fn_80125854(1);
    }
    if (gpGame->b273) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            fn_800D439C(i, 1);
        }
    }
    if (gSession.unk11[1] == 0 && gSession.unk8[0] == 0) {
        if (gpGame->b275) {
            if (!gpGame->b274 || fn_800EC550()) {
                fn_800E4D94(1);
            } else {
                fn_800E4D94(0);
            }
        }
        GOLFERSTATE_Set(GS_WAIT, nPlayer);
        nView = gPlayers[nPlayer].nView0;
        View_SetCamera(fn_80017028(nView), 0x11, nPlayer, nView);
        return;
    }
    gSession.unk11[1] = 1;
}

// TW06: GM_HoleFinished_GameNotFinished.
void GM_HoleFinished_GameNotFinished(int nPlayer) {
    int nView;
    gpGame->pfn210(nPlayer);
    if (gpGame->b275 && !fn_800E4BF8()) {
        if (!gpGame->b274 || fn_800EC550()) {
            fn_800E4C20(1);
        } else {
            fn_800E4C20(0);
        }
    }
    GOLFERSTATE_Set(GS_WAIT, nPlayer);
    nView = gPlayers[nPlayer].nView0;
    View_SetCamera(fn_80017028(nView), 0x11, nPlayer, nView);
    fn_800E3D90();
}

// TW06: GM_CheckForAIConcede. A CPU concedes the hole when it is not holed and either its level
// is above 2, or (checking the players before it) another player is on the green while it is
// not and it has already taken more than 3 strokes more than them.
u8 GM_CheckForAIConcede(int nPlayer) {
    u8  bConcede = 0;
    int i;
    if (Player_IsCPU(nPlayer)) {
        Player* p = &gPlayers[nPlayer];
        if (p->nLie != LIE_HOLED) {
            if (p->nLevel > 2) {
                bConcede = 1;
            } else {
                for (i = 0; i < gSession.nNumPlayers; i++) {
                    if (i == nPlayer) break;
                    if (gPlayers[i].nLie == LIE_GREEN && p->nLie != LIE_GREEN &&
                        p->nStrokes[gpGame->nCurHole] > gPlayers[i].nStrokes[gpGame->nCurHole] + 3) {
                        bConcede = 1;
                    }
                }
            }
        }
    }
    return bConcede;
}

// TW06: GM_BallHit.
void GM_BallHit(int nPlayer) {
    gPlayers[nPlayer].bLowIQPenalty = 0;
    fn_80062D6C(0x4E, nPlayer);
    if (gpGame->b271) {
        fn_800E3D38(nPlayer, 0);
    }
}

// TW06: GM_PlayerAddStroke. One more stroke on this hole, and one more putt if it was the putter.
void GM_PlayerAddStroke(int nPlayer) {
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER) {
        gPlayers[nPlayer].nPutts[gpGame->nCurHole]++;
    }
    if (fn_800EE470()) {
        fn_8011989C(nPlayer, gPlayers[nPlayer].nStrokes[gpGame->nCurHole]);
    }
}

// TW06: GM_CheckForBallOOB. After a shot: out of bounds (or no ground under the ball), or on a
// surface that needs a drop outside the free-drop area, stops the ball. A real penalty (out of
// bounds, or a surface flagged 2 - water) marks the shot, counts one more penalty in a row
// (nLevel: each gives a CPU +25 on its attributes, three make it concede), adds the penalty
// stroke, and shows "water" or "out of bounds" - unless the mode caps the hole at 10 strokes and
// that is reached. Any other shot resets the run of penalties.
u8 GM_CheckForBallOOB(int nPlayer) {
    u8*          pBall = gPlayers[nPlayer].ball;
    u8           bOut  = fn_800E2B40(nPlayer, pBall);
    SurfaceType* pSurf = Ter_GetSupportingWorldMaterial(*(CourseInfo**)(gPlayers[nPlayer].ball + 0x7C), pBall);
    u8           bDrop;
    Player*      p;

    if (pSurf == NULL) {
        bOut  = 1;
        bDrop = 0;
    } else if ((pSurf->u34 & 1) && !Ter_PointInFreeDropNetwork(pBall)) {
        bDrop = 0;
    } else {
        bDrop = 1;
    }
    gPlayers[nPlayer].bLowIQPenalty = 0;
    if (bOut || bDrop) {
        *(s32*)(gPlayers[nPlayer].ball + 0x64) = 0;
        if (bOut || (bDrop && (pSurf->u34 & 2))) {
            gPlayers[nPlayer].bLowIQPenalty = 1;
            p = &gPlayers[nPlayer];
            p->nLevel++;
            p->nStrokes[gpGame->nCurHole]++;
            if (p->nClub == CLUB_PUTTER) {
                p->nPutts[gpGame->nCurHole]++;
            }
            if (fn_800EE470()) {
                fn_8011989C(nPlayer, p->nStrokes[gpGame->nCurHole]);
            }
            if (Game_GetMode() == 8 || Game_GetMode() == 7) {
                return 1;
            }
            if (gpGame->bStrokeLimit && p->nStrokes[gpGame->nCurHole] >= 10) {
                if (fn_800EE470()) {
                    fn_8011989C(nPlayer, 10);
                }
                return 0;
            }
            if (pSurf != NULL && (pSurf->nClass == 7 || pSurf->nClass == 16)) {
                fn_800E4164(0xD, nPlayer, 0.0f);
            } else {
                fn_800E4164(2, nPlayer, 0.0f);
            }
            fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView0), 6, nPlayer);
            return 1;
        }
        fn_80063CF0(fn_80017028(gPlayers[nPlayer].nView0), 6, nPlayer);
        return 0;
    }
    gPlayers[nPlayer].nLevel = 0;
    return 0;
}

// TW06: GM_ShowYardage. With the mode's yardage display on: how far the ball went, flat.
void GM_ShowYardage(int nPlayer) {
    if (gpGame->bShowYardage) {
        Player* p = &gPlayers[nPlayer];
        f32     dx;
        f32     dz;
        dz = *(f32*)(p->ball + 8) - p->fBallZ;
        dx = *(f32*)(p->ball + 0) - p->fBallX;
        fn_800E4164(1, nPlayer, fn_80009680(dx * dx + dz * dz));
    }
}

// TW06: GM_BumpBallForObstructions. A ball at rest against an obstruction or hazard is moved to
// a drop point nearby, or else back where it was before the shot.
void GM_BumpBallForObstructions(int nPlayer) {
    f32 vDrop[4];
    if (gpGame->bBumpObstructions) {
        Player* p;
        u8*     pBall;
        p = &gPlayers[nPlayer];
        if (p->nLie != 0) {
            pBall = p->ball;
            if (Ter_CheckObjectAndHazardObstruction(pBall, 0, 1, 1, 1.5f, 2.0f, 0.577f)) {
                if (Ter_SearchAreaForDropLocation(nPlayer, 0, 0, vDrop)) {
                    Physics_DropBall(pBall, vDrop);
                    return;
                }
                Physics_DropBall(pBall, p->vPreShot);
                if (gPlayers[nPlayer].vA44[0] == gPlayers[nPlayer].fBallX &&
                    gPlayers[nPlayer].vA44[2] == gPlayers[nPlayer].fBallZ) {
                    fn_80055AA8(pBall, p->vPreShot, nPlayer);
                }
            }
        }
    }
}

// TW06: GM_GetGolferDistanceToPin.
f32 GM_GetGolferDistanceToPin(int nPlayer) {
    return fn_800D0478(nPlayer);
}
