// GameManager.c (our name; TW06's functions here are GM_*): the round's bookkeeping - turns,
// strokes, mulligans, the post-shot reaction, walking to the ball, the in-the-hole display.
// TW06's copy of the file keeps the same function order, which gives most of the names below
// (docs/tw06-names.md); each one is checked against what our code does.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

typedef struct View View;

void  fn_800E0A84(u8 v);
void  fn_800D29E8(void);
void  fn_800D33F0(void);
int   fn_800E177C(void);
void  fn_800E2470(void);
void  fn_8006F4B4(void);
void  fn_800170C4(int nView, int a);
void  fn_800E299C(void);
void  Wind_Generate(void);
void  fn_800E3B28(void);
void  fn_800DA36C(void);
void  GM_FlyByMode_Init(void);
void  fn_800D8D38(int nPlayer);
void  Caddie_Stop(void);
void  fn_8001D7A4(int nHandle);
u8    fn_800E0A90(int nPlayer);
u8    fn_800E1CA8(void);                  // int in GameRound.c; the callers here test the byte
void  fn_800D439C(int nPlayer, int a);
void  fn_800D9834(int nPlayer);
u8    fn_8012591C(void);
void  fn_80125854(int a);
void  fn_8011989C(int nPlayer, int nStrokes);
void  GM_GolferConcede_Hole(int nPlayer);
void  GM_EndOfGolferTurn_HoleFinished(int nPlayer);
void  GM_EndOfGolferTurn_GameFinished(int nPlayer);
void  GM_HoleFinished_GameNotFinished(int nPlayer);
u8    GM_CheckForAIConcede(int nPlayer);

SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, u8* pBall);
u8    Ter_PointInFreeDropNetwork(u8* pBall);
void  fn_800E4164(int nMessage, int nPlayer, f32 f);
void  fn_80063CF0(void* pView, int nCamera, int nPlayer);
u8    Ter_CheckObjectAndHazardObstruction(u8* pBall, int a, int b, int c, f32 f1, f32 f2, f32 f3);
u8    Ter_SearchAreaForDropLocation(int nPlayer, int a, int b, f32* pOut);

void  fn_800E0AC4(int a);
void  fn_800E0A98(int a);
void  fn_800D8FE4(int nPlayer);
void  fn_800D3DDC(int nPlayer);
void  fn_800DA48C(int nPlayer);
u8    fn_800DA2AC(void);
void  fn_800DAD54(void);
u8    fn_800DA174(void);
u8    fn_800DA1D4(void);
u8    fn_800E2DB4(int nPlayer);
void  fn_800D9458(int nPlayer);
void  fn_800D4030(int nPlayer);
void  GM_CheckBallForUIHints(int nPlayer);
u8    fn_800E23B0(int nPlayer, int nStrokes);
u8    fn_8008AC40(void);
void  fn_800D9350(int nPlayer);
void  fn_800BB0A8(void);
void  fn_800335F8(int a);
void  fn_8006C4C0(int nPlayer);
void  fn_8006C4A0(void);
void  fn_800C70F8(void* pView, int a);
void  fn_800957FC(int nHandle, int a);
u8*   fn_80016CFC(int nView);
void  fn_800E0AF0(f32* pFrom, f32* pTo, f32* pOut);

extern u8  gReplayData[];                   // 0x801D6030
extern u8* lbl_80281F78;

// The tee positions follow the pins in the per-hole data (fn_8000C594).
typedef struct HoleTees {
    u8     unk0[0xB0];
    PinPos tee[4];                          // 0xB0  one per tee set
} HoleTees;

f32   fn_800336E4(void);
f32   fn_800336F4(void);
void  GOLFERSTATE_Push(int nState, int nPlayer);
u8    fn_800E415C(void);
u8    fn_800E45CC(void);
u8    fn_800E46B4(void);
void  fn_800E2A88(void);
void  fn_800E1018(int nPlayer, int nHole);
void  fn_800C6C8C(void);
void  fn_800E41C8(void);
u8    Ter_PointInOOBNetwork(u8* pBall);

void  Vec3Copy(void* pSrc, void* pDst);
int   fn_8006AA9C(int nPlayer);             // how the shot turned out (0..4, 8+)
void  fn_8006AAB4(int nPlayer, int a);

typedef struct Vec4 { f32 x, y, z, w; } Vec4;
u32   fn_800136DC(int nController);         // buttons: held << 16 | pressed this frame
u32   fn_800142AC(int nButton, int a);      // a button's mask
u8    fn_80014300(u32 uMask);               // any pad pressed these buttons
u8    fn_80063C7C(void* pView);
u8    fn_80063C90(void* pView);             // the camera is still moving
void  fn_80063BF4(void* pView, f32 f, f32* pVec);
void  fn_80062D0C(int nPlayer);
void  fn_80062B78(int nPlayer);
void  fn_80062B74(int nPlayer);
void  fn_80062B70(void);
u8    Player_IsNotCPU(int nPlayer);
void  fn_800E41D4(int nPlayer);
void  fn_8006C300(int nPlayer);

extern Vec4 lbl_80184D30;

void  Shot_Prepare(int nPlayer, u8 bNotify);
void  BreakLine_Start(int nView);            // GoBreakLine.c
void  fn_8009B970(int nView);
void  fn_800689D4(int nPlayer);
void  fn_800C4E80(void* pView, int nPlayer);
u8    GM_bIsZoomButtonPressed(int nPlayer);
u8    GM_bIsElevatorCamButtonPressed(int nPlayer);
u8    fn_800E012C(int nPlayer);
u8    fn_800DFF0C(int nPlayer);
u8    fn_80068AC8(int nPlayer);

u64   fn_800954A4(int a);                   // a time stamp
f32   fn_8006E118(u64 tEnd, u64 tStart);    // seconds between two time stamps
int   GameEffects_BallUpdatesThisFrame(int nPlayer);
u8    fn_800C71A4(void* pView, int nPlayer);
void  fn_8006B2C4(int nPlayer, int a);
u8    fn_800BB1F8(int nPlayer);

void  fn_8001DB04(int nHandle, f32* pOut);  // the golfer's position
void  Ter_GetEnclosingGroundData(CourseInfo* pCourse, f32* pPos, f32* pHighA, SurfaceType** ppSurfA, f32* pNormA,
                                 f32* pHighB, SurfaceType** ppSurfB, f32* pNormB);
void  fn_800E0B14(f32* pA, f32* pB, f32* pOut);
u8    fn_8004560C(void);
int   fn_80095798(int nHandle);

void  fn_800E5228(void);

typedef struct SaveProfile SaveProfile;
int   GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);
f32   GM_GetBonusProgress(SaveProfile* pProfile);
int   fn_800588F4(SaveProfile* pProfile, int a, int i);

extern s32 lbl_80189528[14];
extern s32 lbl_801894D0[6];

// A completion flag in the save profile: the first byte of a 4-byte entry.
typedef struct Flag4 {
    u8 b;
    u8 pad[3];
} Flag4;

// The parts of a save profile the progress counters read (offsets from the reads).
struct SaveProfile {
    u8    unk0;
    char  szName[0x1C - 0x1];   // 0x001  compared against the record holders' names
    u8    b1C[0x3A - 0x1C];     // 0x01C  indexed by lbl_80189528
    u8    b3A[0xC8 - 0x3A];     // 0x03A  indexed by lbl_801894D0
    struct { u8 b; u8 pad[7]; } aC8[31];   // 0x0C8
    Flag4 a1C0[16];             // 0x1C0
    u8    unk200[0x20C - 0x200];
    Flag4 a20C[75];             // 0x20C
    Flag4 a338[25];             // 0x338
    Flag4 a39C[39];             // 0x39C
    u8    unk438[0x5000 - 0x438];
    s32   n5000;                // 0x5000
    u8    unk5004[0x516C - 0x5004];
    s32   a516C[29];            // 0x516C
};

int   strcmp(const char* a, const char* b);

extern u8* gpSaveData;
extern u8  lbl_80202898[];
extern s32 lbl_80282278;
extern u8  lbl_8028227C;
extern u8  gNumPlayersSetUp;                // 0x80281D48 (Golfer.c)

void fn_800DCAD8(void) {
    fn_800E58B4(50);
}

void GM_vCloseModuleONCE(void) {
}

u8 fn_800DCB00(void) {
    return 0;
}

u8 fn_800DCB08(void) {
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

u8 fn_800DCB74(void) {
    return *(u8*)(lbl_80202898 + 0x11);
}

// out = a - b (three floats); the same helper as Ball.c's fn_80055EA0.
asm void fn_800DCB84(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
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
        GM_FlyByMode_Init();
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_800D8D38(i);
    }
    gpGame->b134 = 0;
    lbl_80282278 = gpGame->pfn1D4(5);
    EVENT_Trigger(0xFF, 0, 0, -1);
    for (i = 0; i < 5; i++) {
        int j;              // j only steers the register choice (found by the permuter)
        gpGame->n144[i] = 0;
        j = i;
        gpGame->n158[j] = 0;
        i = j;
        gPlayers[i].unkC2F = 0;
        gPlayers[j].unkC2D = 0;
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
            GM_GolferConcede_Hole(nPlayer);
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
    u8           bOut  = fn_800E2B40(nPlayer, (Ball*)pBall);
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

// TW06: GM_CheckBallForUIHints. The score message after holing out: a hole in one, or strokes
// against par from albatross (-3, message 5) to triple bogey (+3, message 11), else "+N".
void GM_CheckBallForUIHints(int nPlayer) {
    int nDiff;
    if (gPlayers[nPlayer].nStrokes[gpGame->nCurHole] == 1) {
        fn_800E4164(4, nPlayer, 0.0f);
        return;
    }
    nDiff = gPlayers[nPlayer].nStrokes[gpGame->nCurHole] - fn_800D2B08();
    switch (nDiff) {
    case -3: fn_800E4164(5, nPlayer, 0.0f); break;
    case -2: fn_800E4164(6, nPlayer, 0.0f); break;
    case -1: fn_800E4164(7, nPlayer, 0.0f); break;
    case 0:  fn_800E4164(8, nPlayer, 0.0f); break;
    case 1:  fn_800E4164(9, nPlayer, 0.0f); break;
    case 2:  fn_800E4164(10, nPlayer, 0.0f); break;
    case 3:  fn_800E4164(11, nPlayer, 0.0f); break;
    default: fn_800E4164(12, nPlayer, nDiff); break;
    }
}

// TW06: GM_ShowYardage. With the mode's yardage display on: how far the ball went, flat.
void GM_ShowYardage(int nPlayer) {
    if (gpGame->bShowYardage) {
        Player* p = &gPlayers[nPlayer];
        f32     dx;
        f32     dz;
        dx = *(f32*)(p->ball + 0) - p->fBallX;
        dz = *(f32*)(p->ball + 8) - p->fBallZ;
        fn_800E4164(1, nPlayer, fn_80009680(dx * dx + dz * dz));
    }
}

// TW06: GM_BumpBallForObstructions. A ball at rest against an obstruction or hazard is moved to
// a drop point nearby, or else back where it was before the shot.
void GM_BumpBallForObstructions(int nPlayer) {
    int n;                      // a copy of nPlayer: register order only (a "fake match", found by the permuter)
    f32 vDrop[4];
    if (gpGame->bBumpObstructions) {
        Player* p;
        u8*     pBall;
        p = &gPlayers[nPlayer];
        n = nPlayer;
        if (p->nLie != 0) {
            pBall = p->ball;
            if (Ter_CheckObjectAndHazardObstruction(pBall, 0, 1, 1, 1.5f, 2.0f, 0.577f)) {
                if (Ter_SearchAreaForDropLocation(n, 0, 0, vDrop)) {
                    Physics_DropBall((Ball*)pBall, vDrop);
                    return;
                }
                Physics_DropBall((Ball*)pBall, gPlayers[nPlayer].vPreShot);
                if (gPlayers[n].vA44[0] == gPlayers[n].fBallX && gPlayers[n].vA44[2] == gPlayers[n].fBallZ) {
                    fn_80055AA8((Ball*)pBall, gPlayers[nPlayer].vPreShot, n);
                }
            }
        }
    }
}

// TW06: GM_PlayerTookShot. After every shot: replay bookkeeping, the stroke, the out-of-bounds
// check, then - if the ball is in play - holed (score message, the mode's hook), or the hole's
// stroke limit reached (the ball is picked up: lie "holed", 10 or 11 strokes, putts 999), or a
// mode message, or the yardage. Out of bounds goes to the mode's hook instead.
void GM_PlayerTookShot(int nPlayer) {
    u8   bOut;
    if (fn_800E23EC(nPlayer) && !(gPlayers[nPlayer].uFlags & 8)) {
        fn_800E0AC4(1);
    }
    if (!Player_IsCPU(nPlayer) && gSession.nSplitScreen == 0 && gpGame->b287 && gReplayData[0xF10]) {
        fn_800E0A98(1);
    }
    GM_PlayerAddStroke(nPlayer);
    bOut = GM_CheckForBallOOB(nPlayer);
    if (!bOut) {
        fn_800D8FE4(nPlayer);
        fn_800D3DDC(nPlayer);
    }
    fn_800DA48C(nPlayer);
    if (fn_800DA2AC()) {
        fn_800DAD54();
        if (fn_800DA174()) {
            fn_800E4364(1, 10, 0, 0);
        }
        if (fn_800DA1D4()) {
            fn_800E4364(1, 11, 0, 0);
        }
    }
    if (!bOut) {
        if (fn_800E2DB4(nPlayer)) {
            fn_800D9458(nPlayer);
            fn_800D4030(nPlayer);
            GM_CheckBallForUIHints(nPlayer);
            gpGame->pfn218(nPlayer);
        } else {
            if (fn_800E23B0(nPlayer, gPlayers[nPlayer].nStrokes[gpGame->nCurHole])) {
                gPlayers[nPlayer].nLie = LIE_HOLED;
                gPlayers[nPlayer].unkC2D = 1;
                if (fn_800EE470()) {
                    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 10;
                } else {
                    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 11;
                }
                gPlayers[nPlayer].nPutts[gpGame->nCurHole] = 999;
                fn_800D9458(nPlayer);
                fn_800E4164(3, nPlayer, 0.0f);
                gpGame->pfn218(nPlayer);
                gpGame->pfn21C(nPlayer);
            } else if (fn_8008AC40() && gPlayers[nPlayer].bEE0) {
                int n = gPlayers[nPlayer].nEE4;
                if (n == 3) {
                    fn_800E4164(0x13, nPlayer, n);
                    if (nPlayer == 0) {
                        gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
                        gPlayers[1].nHolesWon++;
                    } else {
                        gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
                        gPlayers[0].nHolesWon++;
                    }
                } else if (n == 2) {
                    fn_800E4164(0x12, nPlayer, n);
                } else {
                    fn_800E4164(0x11, nPlayer, n);
                }
            } else {
                GM_ShowYardage(nPlayer);
            }
        }
    } else {
        gpGame->pfn250(nPlayer);
    }
    fn_800D9350(nPlayer);
}

// Taking a mulligan. Not allowed when mulligans are off, the hole was conceded, or fn_800E53B8
// says no; in mulligan mode 2 each player gets one (0xC28). The shot is undone: effects stopped,
// the mode told, the golfer back in the Swing state, and the views of other players sharing this
// screen (and still playing the hole) updated.
u8 GM_PlayerTakeMulligan(int nPlayer) {
    int     i;
    Player* q;
    if (fn_800E177C() == 0) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == GS_CONCEDED) {
        return 0;
    }
    if (fn_800E53B8()) {
        return 0;
    }
    if (fn_800E177C() == 2) {
        if (gPlayers[nPlayer].unkC28) {
            return 0;
        }
        gPlayers[nPlayer].unkC28 = 1;
    }
    fn_800BB0A8();
    fn_800E4204();
    fn_800335F8(1);
    fn_800A76E4();
    fn_8006C4C0(nPlayer);
    gPlayers[nPlayer].unkC2E = 1;
    gPlayers[nPlayer].unkC2F = 1;
    gpGame->pfn254(nPlayer);
    if (gSession.bReplay) {
        fn_8006C4A0();
    }
    fn_800C70F8(fn_80017028(gPlayers[nPlayer].nView0), 1);
    fn_800957D8(gPlayers[nPlayer].nShotHandle);
    fn_800957FC(gPlayers[nPlayer].nShotHandle, 1);
    GOLFERSTATE_Switch(GS_SWING, nPlayer);
    fn_800E3D38(nPlayer, 1);
    fn_80016CFC(gPlayers[nPlayer].nView0)[0x275] = 1;
    for (i = 0, q = gPlayers; i < gSession.nNumPlayers; i++, q++) {
        if (q->bPlayerCut == 0 && q->nView0 == gPlayers[nPlayer].nView0 &&
            q->nLie != 10 && q->nLie != LIE_GREEN && q->nLie != LIE_HOLED) {
            fn_80016CFC(gPlayers[nPlayer].nView0)[0x275] = 0;
        }
    }
    return 1;
}

// Whether to play the pre-shot routine (our reading; TW06's name for this one is not certain).
// The mode's setting 0x290: 0 never, 1 always; otherwise always off the tee, never with clubs 0-8
// (woods and long irons) from elsewhere, never with an obstruction nearby, else 85% of the time.
// On course 18, hole 10, not within 40 yards of the tee.
int fn_800DDFB4(int nPlayer) {
    f32 v[4];
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        return gPlayers[nPlayer].nLie == 0;
    }
    if (gpGame->n290 == 0) {
        return 0;
    }
    if (Game_GetCourse() == 0x12 && fn_80015464() == 10) {
        fn_800E0AF0(&gPlayers[nPlayer].fBallX, &((HoleTees*)fn_8000C594())->tee[gSession.nTeeSet[nPlayer]].x, v);
        v[1] = 0.0f;
        if ((f32)fn_80009680(fn_80009744(v)) < 40.0f) {
            return 0;
        }
    }
    if (gpGame->n290 == 1) {
        return 1;
    }
    if (gPlayers[nPlayer].nLie == 0) {
        return 1;
    }
    if (gPlayers[nPlayer].nLie != 0 && gPlayers[nPlayer].nClub < 9) {
        return 0;
    }
    if (Ter_CheckObjectAndHazardObstruction(gPlayers[nPlayer].ball, 0, 1, 1, *(f32*)(lbl_80281F78 + 0x16C), 4.0f, 0.577f)) {
        return 0;
    }
    return (Rand_Next(1) % 100) < 85;
}

// TW06: GM_ShowPostShotAnimation. Whether the golfer plays a reaction after the shot. Never on
// course 18's 10th within 40 yards of the tee, in one special stance on surface 45, or when the
// golfer stands out of bounds, in water, or on a slope steeper than 0.1 with the ball 0.2 above.
// Then by how the shot turned out (0..4): after a putt 80%, always, always, 70%, 90%, else 50%;
// after other shots 35%, always, always, 70%, 90%, else 50%.
int GM_ShowPostShotAnimation(int nPlayer) {
    f32          vPos[4];
    f32          vNormA[4];
    f32          vNormB[4];
    f32          vFlat[4];
    f32          vTee[4];
    SurfaceType* pSurfA;
    SurfaceType* pSurfB;
    f32          fHighA;
    f32          fHighB;
    int          nResult;
    CourseInfo*  pCourse;
    SurfaceType* pSurf;
    f32          fHigh;
    f32          fLen;
    f32          fRise;
    f32          fSlope;

    nResult = fn_8006AA9C(nPlayer);
    if (!fn_800E27A8()) {
        return 0;
    }
    if (Game_GetCourse() == 0x12 && fn_80015464() == 10) {
        fn_800E0AF0(&gPlayers[nPlayer].fBallX, &((HoleTees*)fn_8000C594())->tee[gSession.nTeeSet[nPlayer]].x, vTee);
        vTee[1] = 0.0f;
        if ((f32)fn_80009680(fn_80009744(vTee)) < 40.0f) {
            return 0;
        }
    }
    if (*(s32*)((u8*)gPlayers[nPlayer].nShotHandle + 0x438) == 11 && *(s32*)(gPlayers[nPlayer].ball + 0x78) == 0x2D) {
        return 0;
    }
    pCourse = fn_8000C594();
    if (pCourse) {
        fn_8001DB04(gPlayers[nPlayer].nShotHandle, vPos);
        Ter_GetEnclosingGroundData(pCourse, vPos, &fHighA, &pSurfA, vNormA, &fHighB, &pSurfB, vNormB);
        if (-65536.125f == fHighA && -65536.125f == fHighB) {
            return 0;
        }
        if (-65536.125f == fHighA) {
            fHigh = fHighB;
            pSurf = pSurfB;
        } else if (-65536.125f == fHighB) {
            fHigh = fHighA;
            pSurf = pSurfA;
        } else {
            fHigh = fn_8000AD9C(fHighB - gPlayers[nPlayer].fBallY);
            if (fn_8000AD9C(fHighA - gPlayers[nPlayer].fBallY) < fHigh) {
                fHigh = fHighA;
                pSurf = pSurfA;
            } else {
                fHigh = fHighB;
                pSurf = pSurfB;
            }
        }
        fn_800E0B14(vPos, &gPlayers[nPlayer].fBallX, vFlat);
        vFlat[1] = 0.0f;
        fLen  = fn_80009680(fn_80009744(vFlat));
        fRise = gPlayers[nPlayer].fBallY - fHigh;
        if (0.0f != fLen) {
            fSlope = fRise / fLen;
        } else {
            fSlope = 0.0f;
        }
        if (!Ter_PointInOOBNetwork((u8*)vPos) || (pSurf != NULL && !(pSurf->u34 & 1)) || pSurf->nClass == 7 ||
            pSurf->nClass == 16 || (fn_8000AD9C(fSlope) > 0.1f && fRise > 0.2f)) {
            return 0;
        }
    }
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        if (gPlayers[nPlayer].nLie == LIE_HOLED || *(s32*)(gPlayers[nPlayer].ball + 0x78) == 16 ||
            nResult == 2 || nResult == 1) {
            return 1;
        }
        return 0;
    }
    if (gPlayers[nPlayer].bPlanReady) {
        return nResult == 2;
    }
    if (fn_8004560C()) {
        return 1;
    }
    if (fn_80095780(gPlayers[nPlayer].nShotHandle) == 9 || fn_80095798(gPlayers[nPlayer].nShotHandle) == 9) {
        return 1;
    }
    if (gPlayers[nPlayer].uFlags & 1) {
        return *(s32*)((u8*)gPlayers[nPlayer].nShotHandle + 0x1790) != 0;
    }
    if (gPlayers[nPlayer].nShotKind == SHOT_PUTT) {
        switch (nResult) {
        case 0:  return Rand_Next(1) % 100 < 80;
        case 1:  return Rand_Next(1) % 100 < 100;
        case 2:  return 1;
        case 3:  return Rand_Next(1) % 100 < 70;
        case 4:  return Rand_Next(1) % 100 < 90;
        default: return Rand_Next(1) % 100 < 50;
        }
    } else {
        switch (fn_8006AA9C(nPlayer)) {
        case 0:  return Rand_Next(1) % 100 < 35;
        case 1:  return Rand_Next(1) % 100 < 100;
        case 2:  return 1;
        case 3:  return Rand_Next(1) % 100 < 70;
        case 4:  return Rand_Next(1) % 100 < 90;
        default: return Rand_Next(1) % 100 < 50;
        }
    }
}

// TW06: GM_ShowPostShotCrowdFlyby (by position). Two measures of the shot (fn_800336E4 at least 5,
// fn_800336F4 at least 0.5).
int GM_ShowPostShotCrowdFlyby(void) {
    if (fn_800336E4() >= 5.0f && fn_800336F4() >= 0.5f) {
        return 1;
    }
    return 0;
}

// TW06: GM_FlyByMode_Init. The player the mode picks starts the hole flyover.
void GM_FlyByMode_Init(void) {
    int n = gpGame->pfn1D4(5);
    GOLFERSTATE_Push(GS_INITIAL_FLY_BY, n);
    fn_8001704C(gPlayers[n].nView0, n);
}

// TW06: GM_Update.
void GM_Update(void) {
    if (gSession.nGameType == 6) {
        gpGame->pfn220();
        if ((fn_800E5110() && fn_800E415C()) || fn_800E45CC()) {
            fn_800E46B4();
        } else if (gpGame->b27E && !fn_800E4BF8()) {
            fn_800E2A88();
        }
        gpGame->n12C = gSession.unk24;
    }
}

// TW06: GM_RestartHole. Every player back to the hole's start, the mode told, the flyover again
// if the mode has one, effects reset.
void GM_RestartHole(void) {
    int i;
    if (gpGame->b279) {
        for (i = 0; i < 5; i++) {
            fn_800E1018(i, gpGame->nCurHole);
        }
        fn_800E299C();
        gpGame->pfn224();
        if (gpGame->b27F) {
            GM_FlyByMode_Init();
        }
        gpGame->n12C = gSession.unk24;
        fn_800E3D90();
        fn_800E3B28();
        GameEffects_ResetGameEffectSettings();
        fn_800C6C8C();
        for (i = 0; i < gSession.nNumPlayers; i++) {
            fn_800957D8(PLAYER(i)->nShotHandle);
            fn_800957FC(PLAYER(i)->nShotHandle, 1);
        }
        fn_800E5714(2);
    }
}

// TW06: GM_GetGolferDistanceToPin.
f32 GM_GetGolferDistanceToPin(int nPlayer) {
    return fn_800D0478(nPlayer);
}

// TW06: GM_ReplaceOOBBall. A ball out of bounds (or flagged at 0x30E) is dropped at a legal point
// nearby when there is one; otherwise it goes back where it was before the shot.
void GM_ReplaceOOBBall(int nPlayer) {
    f32  v[4];
    f32* pPre;
    u8*  pBall;
    if ((gPlayers[nPlayer].b30E ||
         (Ter_PointInOOBNetwork(gPlayers[nPlayer].ball) && !gPlayers[nPlayer].bLowIQPenalty)) &&
        Ter_SearchAreaForDropLocation(nPlayer, 1, 1, v)) {
        Physics_DropBall((Ball*)gPlayers[nPlayer].ball, v);
        return;
    }
    pBall = gPlayers[nPlayer].ball;
    pPre  = gPlayers[nPlayer].vPreShot;
    Physics_DropBall((Ball*)pBall, pPre);
    if (gPlayers[nPlayer].vA44[0] == gPlayers[nPlayer].fBallX &&
        gPlayers[nPlayer].vA44[2] == gPlayers[nPlayer].fBallZ) {
        fn_80055AA8((Ball*)pBall, pPre, nPlayer);
    }
}

// The same drop without the out-of-bounds test.
void fn_800DEB5C(int nPlayer) {
    f32  v[4];
    f32* pPre;
    u8*  pBall;
    if (Ter_SearchAreaForDropLocation(nPlayer, 1, 1, v)) {
        Physics_DropBall((Ball*)gPlayers[nPlayer].ball, v);
        return;
    }
    pBall = gPlayers[nPlayer].ball;
    pPre  = gPlayers[nPlayer].vPreShot;
    Physics_DropBall((Ball*)pBall, pPre);
    if (gPlayers[nPlayer].vA44[0] == gPlayers[nPlayer].fBallX &&
        gPlayers[nPlayer].vA44[2] == gPlayers[nPlayer].fBallZ) {
        fn_80055AA8((Ball*)pBall, pPre, nPlayer);
    }
}

// TW06: GM_GolferConcede_Hole. The player picks up: lie "holed", 999 strokes and putts. If that
// leaves one golfer on the hole (outside mode 18), everyone else is finished too - in match play
// the opponent wins the hole without putting out. Then the Conceded state.
void GM_GolferConcede_Hole(int nPlayer) {
    Player* p;
    int     i;
    int     nPlayers;
    int     n;
    fn_800E3D38(nPlayer, 0);
    p = &gPlayers[nPlayer];
    gPlayers[nPlayer].nLie = LIE_HOLED;
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 999;
    p->nPutts[gpGame->nCurHole] = 999;
    *(s32*)(gPlayers[nPlayer].ball + 0x64) = 0;
    if (Game_GetMode() != 0x12) {
        nPlayers = gNumPlayersSetUp;
        n = 0;
        for (i = 0; i < nPlayers; i++) {
            if (PLAYER(i)->nLie != LIE_HOLED) {
                n++;
            }
        }
        if (n == 1) {
            for (i = 0; i < nPlayers; i++) {
                if (i != nPlayer) {
                    PLAYER(i)->nLie = LIE_HOLED;
                    *(s32*)(PLAYER(i)->ball + 0x64) = 0;
                }
            }
        }
    }
    fn_800E5714(1);
    fn_800E5724(nPlayer);
    fn_800E41C8();
    GOLFERSTATE_Switch(GS_CONCEDED, nPlayer);
}

// TW06: GM_MovePlayerToBall. The player's position becomes the ball's, the pre-shot position is
// saved, and the height is set from the ground under it: the upper surface unless there is none
// or it is more than a quarter yard above, then the lower one, else the height is kept.
void GM_MovePlayerToBall(int nPlayer) {
    f32         fLow;
    f32         fHigh;
    Player*     p     = &gPlayers[nPlayer];
    u8*         pBall = p->ball;
    f32*        pPos  = &p->fBallX;
    CourseInfo* pCourse;
    f32         f;
    Vec3Copy(pBall, pPos);
    Vec_Copy((f32*)pBall, p->vPreShot);
    pCourse = fn_8000C594();
    if (pCourse) {
        Ter_GetEnclosingGroundHeight(pCourse, pPos, &fLow, &fHigh);
        f = fHigh;
        if (-65536.125f == f || f > 0.25f + gPlayers[nPlayer].fBallY) {
            f = fLow;
            if (-65536.125f == fLow) {
                f = gPlayers[nPlayer].fBallY;
            }
        }
        gPlayers[nPlayer].fBallY = f;
    }
}

// TW06: GM_CheckForShotChanges. A human's buttons while setting up: three camera/aim buttons
// (9, 10, 30; not in modes 22 and 26), then the aiming cameras - zoom, elevator - or the mode's
// re-plan button 47 (a fresh default target and shot), the green camera, or the mid-hole flyover
// (unless cameras are skipped). Buttons 11-14 are taunts/reactions (events 0x12-0x15); any of it
// updates the golfer's emotion.
void GM_CheckForShotChanges(int nPlayer) {
    u8 bChanged = 0;
    if (Player_IsCPU(nPlayer)) {
        return;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_GREEN_MORPH) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(9, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
            fn_800E3D38(nPlayer, 1);
            bChanged = 1;
        } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(10, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xE, 0, -1);
            fn_800E3D38(nPlayer, 1);
            bChanged = 1;
        } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1E, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xF, 0, -1);
            fn_800E3D38(nPlayer, 1);
            bChanged = 1;
        }
        if (GM_bIsZoomButtonPressed(nPlayer)) {
            GOLFERSTATE_Push(GS_ZOOM, nPlayer);
        } else if (!gpGame->b28D && GM_bIsElevatorCamButtonPressed(nPlayer)) {
            GOLFERSTATE_Push(GS_ELEVATOR, nPlayer);
        } else if (gpGame->b28D && (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2F, 0))) {
            if (gpGame->pfn258(nPlayer)) {
                AI_DefaultTarget(nPlayer);
                Shot_Prepare(nPlayer, 1);
                BreakLine_Start(gPlayers[nPlayer].nView0);
                fn_8009B970(gPlayers[nPlayer].nView0);
                fn_8001C804(nPlayer, 1, 1);
                fn_800957D8(gPlayers[nPlayer].nShotHandle);
                fn_80095744(gPlayers[nPlayer].nShotHandle, 5);
                fn_800689D4(nPlayer);
                fn_80062C38();
                fn_800E3D38(nPlayer, 1);
            }
        } else if (!gpGame->b28D && fn_800E012C(nPlayer)) {
            fn_800C4E80(fn_80017028(gPlayers[nPlayer].nView0), nPlayer);
        } else if (fn_800DFF0C(nPlayer)) {
            if (SESSION_OPTIONS->bSkipCameras) return;
            if (fn_8008AC40()) return;
            GOLFERSTATE_Push(GS_MID_HOLE_FLY_BY, nPlayer);
        }
    }
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1)) {
        EVENT_Trigger(nPlayer, 0x12, 0, -1);
        bChanged = 1;
    } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1)) {
        EVENT_Trigger(nPlayer, 0x13, 0, -1);
        bChanged = 1;
    }
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1)) {
        EVENT_Trigger(nPlayer, 0x14, 0, -1);
        bChanged = 1;
    } else if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1)) {
        EVENT_Trigger(nPlayer, 0x15, 0, -1);
        bChanged = 1;
    }
    if (fn_80068AC8(nPlayer)) {
        bChanged = 1;
    }
    if (bChanged) {
        Emotion_UpdatePlayerEmotion(nPlayer);
    }
}

// TW06: GM_DoPostShotInHoleUI. Every frame after a holed ball, until the golfer's turn ends: with
// the score display (flag 8) the turn ends as soon as the camera is done; otherwise the camera
// is moved on, and a human may take a mulligan (button 25), watch the replay (button 24, if one
// was recorded, the mode allows it and the hole was not conceded) or continue (button 0); a CPU
// continues on any pad's button 0.
void GM_DoPostShotInHoleUI(int nPlayer) {
    void* pView = fn_80017028(gPlayers[nPlayer].nView0);
    Vec4  vOffset = lbl_80184D30;
    if ((gPlayers[nPlayer].uFlags & 8) && fn_80063C7C(pView)) {
        GM_EndOfGolferTurn(nPlayer);
        fn_80062D0C(nPlayer);
        return;
    }
    if (fn_800E46B4()) {
        return;
    }
    if (!fn_800E4254(nPlayer)) {
        if (fn_80063C7C(pView)) {
            GM_EndOfGolferTurn(nPlayer);
            return;
        }
        if (fn_80063C90(pView)) {
            return;
        }
        fn_80062B78(nPlayer);
        fn_80062B74(nPlayer);
        fn_80062B70();
        fn_80063BF4(pView, *(f32*)(lbl_80281F78 + 0x170), (f32*)&vOffset);
        return;
    }
    if (Player_IsNotCPU(nPlayer) && gSession.nSplitScreen == 0) {
        if (gSession.bReplay == 0 && (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) &&
            !(gPlayers[nPlayer].uFlags & 8)) {
            if (GM_PlayerTakeMulligan(nPlayer)) {
                fn_80062D0C(nPlayer);
            }
            return;
        }
        if (gReplayData[0xF10] && (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x18, 0)) &&
            gpGame->b287 && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED && !fn_800E53B8() &&
            !(*(u32*)((u8*)gPlayers[nPlayer].nShotHandle + 0x10) & 0x40)) {
            fn_80062D0C(nPlayer);
            fn_8006C300(nPlayer);
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
            return;
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) && !fn_8008AC40()) {
            fn_800E41D4(nPlayer);
        }
    } else if (Player_IsNotCPU(nPlayer)) {
        if (!fn_8008AC40() && (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0))) {
            fn_800E41D4(nPlayer);
        }
    } else if (Player_IsCPU(nPlayer) && !fn_8008AC40() && fn_80014300(fn_800142AC(0, 0))) {
        fn_800E41D4(nPlayer);
    }
}

// TW06: GM_ChooseRemoveBallState (by position). Whether the golfer takes the ball out of the cup
// with the special animation: never in the special session mode, when fn_800E27A8 says no, or
// after a picked-up ball. A shot of kind 2 first asks the mode (with the stroke taken back). Then
// yes if 0xC2B is set; no during animation 9, beyond 5 yards (0xA64), or off the green; a scripted
// answer in uFlags bits 0/1; otherwise 10% of the time two or more under par, else 25%.
int GM_ChooseRemoveBallState(int nPlayer) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        return 0;
    }
    if (!fn_800E27A8()) {
        return 0;
    }
    if (gPlayers[nPlayer].unkC2D) {
        return 0;
    }
    if (fn_8006AA9C(nPlayer) == 2) {
        gPlayers[nPlayer].nStrokes[gpGame->nCurHole]--;
        if (!gpGame->pfn1FC(nPlayer)) {
            gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
            if (gPlayers[nPlayer].bPlanReady) {
                fn_8006AAB4(nPlayer, 1);
                return 1;
            }
            return 0;
        }
        gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
    }
    if (gPlayers[nPlayer].bPlanReady) {
        return 1;
    }
    if (fn_80095780(gPlayers[nPlayer].nShotHandle) == 9) {
        return 0;
    }
    if (gPlayers[nPlayer].fA64 > 5.0f) {
        return 0;
    }
    if (*(s32*)(gPlayers[nPlayer].ball + 0x78) < 0 || *(s32*)(gPlayers[nPlayer].ball + 0x78) >= 156 ||
        gSurfaceTypes[*(s32*)(gPlayers[nPlayer].ball + 0x78)].nClass != 3) {
        return 0;
    }
    if (gPlayers[nPlayer].uFlags & 1) {
        return (gPlayers[nPlayer].uFlags >> 1) & 1;
    }
    if (fn_800D2B08() - gPlayers[nPlayer].nStrokes[gpGame->nCurHole] > 1) {
        if (Rand_Next(1) % 10 == 0) {
            return 1;
        }
    } else if (!(Rand_Next(1) & 3)) {
        return 1;
    }
    return 0;
}

// TW06: GM_SimulateBallMovement. Each frame of a shot: the ball's physics steps for this frame
// (none while the view holds it), then the look-ahead copy (ballBefore) is run on ahead within a
// time budget of 0.83 ms minus what the real ball took, until it comes to rest. When it has, the
// golfer's reaction can start early: for a shot of kind 8 or 9 that will stop 2 to 5.5 yards out
// (close to the nearest it got), a holed ball at par or better starts it half the time, and a
// miss that came within 0.2 of the hole always does (animation 9). A scripted reaction (uFlags
// bit 0) plays when the ball passes the saved distance instead.
void GM_SimulateBallMovement(int nPlayer) {
    int     nSteps = 0;
    u64     t0;
    int     nUpdates;
    int     i;
    f32     fBudget;
    f32     fMs;
    Player* p;
    u8*     pBall;
    s32*    pState;
    u64     t1;
    u8      bReact;
    u8      bOn;
    f32     fDist;
    u32*    pFlags;
    u8*     pDone;
    s32*    pLie;

    t0 = fn_800954A4(0);
    nUpdates = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (gpGame->n294 != 0 && fn_800C71A4(fn_80017028(gPlayers[nPlayer].nView0), nPlayer)) {
        nUpdates = 0;
    }
    p = &gPlayers[nPlayer];
    pBall = p->ball;
    for (i = 0; i < nUpdates; i++) {
        Physics_Simulate((Ball*)pBall, 20);
    }
    fMs = 1000.0f * fn_8006E118(fn_800954A4(0), t0);
    fBudget = 0.83f - fMs;
    if (fn_8008AC40()) {
        fBudget = 0.83f;
    }
    if (gSession.nSplitScreen == 0 && gSession.fFrameTime > 0.0f) {
        Ball_SetSimulating(1);
        fn_80050D2C(1);
        pBall = p->ballBefore;
        pState = (s32*)(p->ballBefore + 0x64);
        while (*pState != 1 && *pState != 5 && *pState != 0 && fBudget > 0.1f) {
            t1 = fn_800954A4(0);
            Physics_Simulate((Ball*)pBall, 20);
            fMs = 1000.0f * fn_8006E118(fn_800954A4(0), t1);
            nSteps++;
            fBudget -= fMs;
            if (fn_8008AC40()) {
                if (nSteps < 2) {
                    fBudget = 0.83f;
                } else {
                    fBudget = 0.0f;
                }
            }
            if (*pState == 1 || *pState == 5 || *pState == 0) {
                if (!(gPlayers[nPlayer].uFlags & 8)) {
                    EVENT_Trigger(nPlayer, 0x3C, 0, -1);
                    fn_8006B2C4(nPlayer, 1);
                }
                break;
            }
        }
        fn_80050D2C(0);
        Ball_SetSimulating(0);
        if (fn_800BB1F8(nPlayer)) {
            bReact = (u32)(fn_8006AA9C(nPlayer) - 8) <= 1;
            bOn    = fn_800E27A8();
            fDist  = fn_800D0478(nPlayer);
            if (bOn) {
                pFlags = &gPlayers[nPlayer].uFlags;
                if (*pFlags & 1) {
                    if ((*pFlags & 4) && fDist < gPlayers[nPlayer].fEEC) {
                        fn_80095744(gPlayers[nPlayer].nShotHandle, 9);
                    }
                } else if (bReact) {
                    pDone = &gPlayers[nPlayer].bRehearsalDone;
                    if (!*pDone && fDist < 5.5f && fDist > 2.0f &&
                        fDist - *(f32*)(gPlayers[nPlayer].ball + 0x60) < 0.3f) {
                        pLie = (s32*)(gPlayers[nPlayer].ballBefore + 0x68);
                        if (*pLie == LIE_HOLED && Hole_ScoreAfterTapIn(nPlayer) <= 0) {
                            if (Rand_Next(1) % 100 < 50) {
                                *pFlags |= 4;
                                gPlayers[nPlayer].fEEC = fDist;
                                fn_80095744(gPlayers[nPlayer].nShotHandle, 9);
                            }
                        } else if (*pLie != LIE_HOLED && *(f32*)(gPlayers[nPlayer].ballBefore + 0x60) < 0.2f) {
                            *pFlags |= 4;
                            gPlayers[nPlayer].fEEC = fDist;
                            fn_80095744(gPlayers[nPlayer].nShotHandle, 9);
                        }
                        *pDone = 1;
                    }
                }
            }
        }
    }
}

// TW06: GM_CheckControllerPulled (by position). When the mode says a controller was pulled and
// player 1's camera is at rest, the pause for it (fn_800E5228).
void GM_CheckControllerPulled(void) {
    if (gpGame->pfn234()) {
        if (!fn_80063C90(fn_80017028(gPlayers[0].nView0))) {
            fn_800E5228();
        }
    }
}

// A saved custom round (0x70 bytes): 18 holes, each a hole number and the course it is from.
typedef struct SavedRound {
    u8   unk0[2];
    s8   nHoleNum[18];          // 0x02
    s32  nCourse[18];           // 0x14
    u8   unk5C[0x70 - 0x5C];
} SavedRound;

typedef struct SaveSlot {
    u8         unk0[0x5244];
    SavedRound round[1];        // 0x5244
    u8         unk52B4[0x10600 - 0x52B4];
} SaveSlot;

// TW06: GM_SetupCustomHoleSelection. Loads a saved custom round (slot nSaveSlot, record
// nSaveCourse) into the round's hole list.
void GM_SetupCustomHoleSelection(void) {
    int       i;
    SaveSlot* pSave;
    for (i = 0; i < 18; i++) {
        pSave = (SaveSlot*)gpSaveData;
        gpGame->nHoleNum[i] = pSave[gpGame->nSaveSlot].round[gpGame->nSaveCourse].nHoleNum[i];
        gpGame->nHoleCourse[i] = ((SaveSlot*)gpSaveData)[gpGame->nSaveSlot].round[gpGame->nSaveCourse].nCourse[i];
    }
}

// Button 8 tapped: released after 2 to 5 frames of holding (the count is kept by GM_bIsZoomButtonPressed).
// A tap asks for the mid-hole flyover (when the mode has one, 0x280); a hold is the zoom camera.
u8 fn_800DFF0C(int nPlayer) {
    if (!gpGame->b280) {
        return 0;
    }
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 0)) ||
        (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 1))) {
        return 0;
    }
    if (gpGame->n144[nPlayer] <= 1) {
        gpGame->n144[nPlayer] = 0;
        return 0;
    }
    if (gpGame->n144[nPlayer] < 6) {
        gpGame->n144[nPlayer] = 0;
        return 1;
    }
    return 0;
}

// TW06: GM_bIsZoomButtonPressed. Button 8 held: counts frames (at 59.94 a second) and says yes
// once it has been held for 6.
u8 GM_bIsZoomButtonPressed(int nPlayer) {
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 0)) && gpGame->n144[nPlayer] == 0) {
        gpGame->n144[nPlayer]++;
    } else if (gpGame->n144[nPlayer] > 0) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 1)) {
            gpGame->n144[nPlayer] += (int)(59.94f * gSession.fFrameTime + 0.5f);
            if (gpGame->n144[nPlayer] >= 6) {
                gpGame->n144[nPlayer] = 0;
                return 1;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

// Button 47 tapped (2 to 19 frames): the green camera.
u8 fn_800E012C(int nPlayer) {
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2F, 0)) ||
        (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2F, 1))) {
        return 0;
    }
    if (gpGame->n158[nPlayer] <= 1) {
        gpGame->n158[nPlayer] = 0;
        return 0;
    }
    if (gpGame->n158[nPlayer] < 20) {
        gpGame->n158[nPlayer] = 0;
        return 1;
    }
    return 0;
}

// TW06: GM_bIsElevatorCamButtonPressed. Button 47 held for 20 frames.
u8 GM_bIsElevatorCamButtonPressed(int nPlayer) {
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2F, 0)) && gpGame->n158[nPlayer] == 0) {
        gpGame->n158[nPlayer]++;
    } else if (gpGame->n158[nPlayer] > 0) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2F, 1)) {
            gpGame->n158[nPlayer] += (int)(59.94f * gSession.fFrameTime + 0.5f);
            if (gpGame->n158[nPlayer] >= 20) {
                gpGame->n158[nPlayer] = 0;
                return 1;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

// TW06: GM_vGetAllTimeRecordsHeld (by position). How many all-time records the profile's golfer
// holds: in the first table a top-5 entry with the same value as the record; in the other two,
// an entry that is at least the record of any of the first three blocks of that kind.
// The original compares a record three times over (the three branches are in the binary): most
// likely a macro written for a record of several fields, all of which are the one value here.
#define RECORD_AT_LEAST(a, b) ((a) >= (b) && (a) >= (b) && (a) >= (b))

int GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile) {
    int n = 0;
    int j, i, k, b;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 5; j++) {
            if (strcmp(gSession.recA[i][j].szName, pProfile->szName) == 0 &&
                gSession.recA[i][j].nValue == gSession.recA[i][0].nValue) {
                n++;
                break;
            }
        }
    }
    for (i = 0; i < 3; i++) {
        for (k = 0; k < 3; k++) {
            for (j = 0; j < 5; j++) {
                if (strcmp(gSession.recB[k][i][j].szName, pProfile->szName) == 0) {
                    for (b = 0; b < 3; b++) {
                        if (RECORD_AT_LEAST(gSession.recB[k][i][j].nValue, gSession.recB[b][i][0].nValue)) {
                            n++;
                            goto nextB;
                        }
                    }
                }
            }
        }
    nextB:;
    }
    for (i = 0; i < 2; i++) {
        for (k = 0; k < 5; k++) {
            for (j = 0; j < 5; j++) {
                if (strcmp(gSession.recC[k][i][j].szName, pProfile->szName) == 0) {
                    for (b = 0; b < 3; b++) {
                        if (RECORD_AT_LEAST(gSession.recC[k][i][j].nValue, gSession.recC[b][i][0].nValue)) {
                            n++;
                            goto nextC;
                        }
                    }
                }
            }
        }
    nextC:;
    }
    return n;
}

// TW06: GM_GetGameProgress (by position). The profile's completion score: one point for each of
// the 25 entries at 0x338, and half a point for each other finished thing (0x5000,
// the 29 entries at 0x516C not in state 3, the 31 flags at 0xC8, the first 23 entries at 0x39C
// equal to 1, the 75 items fn_800588F4 reports, and the two indexed flag tables), plus the bonus
// progress below.
f32 GM_GetGameProgress(SaveProfile* pProfile) {
    f32 f = 0.0f;
    int i;
    for (i = 0; i < 25; i++) {
        if (pProfile->a338[i].b) {
            f += 1.0f;
        }
    }
    if (pProfile->n5000 >= 1) {
        f += 0.5f;
    }
    for (i = 0; i < 29; i++) {
        if (pProfile->a516C[i] != 3) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 31; i++) {
        if (pProfile->aC8[i].b) {
            f += 1.0f;
        }
    }
    for (i = 0; i < 23; i++) {
        if (pProfile->a39C[i].b == 1) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 75; i++) {
        if (fn_800588F4(pProfile, 0, i)) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 14; i++) {
        if (pProfile->b1C[lbl_80189528[i]]) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 6; i++) {
        if (pProfile->b3A[lbl_801894D0[i]]) {
            f += 0.5f;
        }
    }
    return f + GM_GetBonusProgress(pProfile);
}

// TW06: GM_GetBonusProgress (by position). One point for each of the 75 entries at 0x20C, half a point for entries 23-38 at 0x39C and 0-15 at 0x1C0, and half a point for each
// all-time record held.
f32 GM_GetBonusProgress(SaveProfile* pProfile) {
    f32 f = 0.0f;
    int i;
    for (i = 0; i < 75; i++) {
        if (pProfile->a20C[i].b) {
            f += 1.0f;
        }
    }
    for (i = 23; i < 39; i++) {
        if (pProfile->a39C[i].b) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 12; i++) {
        if (pProfile->a1C0[i].b) {
            f += 0.5f;
        }
    }
    for (i = 12; i < 16; i++) {
        if (pProfile->a1C0[i].b) {
            f += 0.5f;
        }
    }
    f += 0.5f * GM_vGetAllTimeRecordsHeld(pProfile);
    return f;
}

void fn_800E0A84(u8 v) {
    gpGame->b135 = v;
}

u8 fn_800E0A90(int nPlayer) {
    return 0;
}

void fn_800E0A98(int a) {
    fn_80062D6C(46, (u8)a);
}

void fn_800E0AC4(int a) {
    fn_80062D6C(29, (u8)a);
}
