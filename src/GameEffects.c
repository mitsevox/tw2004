// GameEffects.c (TW06's name, GameEffects_*): slow motion, the "GameBreaker" (a letterbox, a
// wider view and a slow-down during a shot), the heartbeat rumble, the time rate. TW06's copy of
// the file keeps the same function order, which gives the names.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

u8    fn_800B4AE0(void);
void  fn_800DBFAC(void);
void  fn_800DC18C(void);
void  fn_800DC290(f32 fHeight);
int   fn_800D8750(int a, int b, int c, char* szName, int nPlayer);   // szName: a profile's name
u8    fn_800BCD24(int nPlayer);

GameEffects lbl_80202898;   // the effects state (game.h; GameManager, GoGolfCam and others read it)

// Starts a scripted GameBreaker for player nWho, for reason nWhy (a bit in uFlags).
#define GB_START(nWho, nWhy)                                                                \
    if (!lbl_80202898.bGameBreaker || lbl_80202898.bClosing || lbl_80202898.nGBType != 0) { \
        lbl_80202898.bClosing = 0;                                                          \
        lbl_80202898.bGameBreaker = 1;                                                      \
        lbl_80202898.fGBTime = 0.0f;                                                        \
        lbl_80202898.f24 = 0.0f;                                                            \
        lbl_80202898.b19 = 0;                                                               \
        lbl_80202898.nGBType = 0;                                                           \
        lbl_80202898.nPlayer = (nWho);                                                      \
        lbl_80202898.bPaused = 0;                                                           \
        lbl_80202898.uFlags = 1 << (nWhy);                                                  \
        lbl_80202898.nHeartbeats = 0;                                                       \
        EVENT_Trigger((nWho), 0x3D, 0, -1);                                                 \
    }

// TW06: GameEffects_InitGameEffectSettings (by position and size).
void fn_800DAE44(void) {
    lbl_80202898.b10 = 0;
    lbl_80202898.b11 = 0;
    lbl_80202898.bSlowMo = 0;
    lbl_80202898.b9 = 0;
    lbl_80202898.n28 = 0;
    lbl_80202898.bGameBreaker = 0;
    lbl_80202898.b19 = 0;
    lbl_80202898.bPaused = 0;
    lbl_80202898.bRumble = 0;
    lbl_80202898.nHeartbeats = 0;
    lbl_80202898.f54 = 1.0f;
}

// Every effect off: a GameBreaker still up is closed (its end events), the rumble stopped.
void GameEffects_ResetGameEffectSettings(void) {
    int i;
    fn_800DAF74();
    lbl_80202898.bPaused = 0;
    lbl_80202898.bRumble = 0;
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.nGBType == 0) {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
        } else {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
        }
        fn_800DCAD8();
    }
    lbl_80202898.bGameBreaker = 0;
    lbl_80202898.bSpinWindowDone = 0;
    lbl_80202898.b19 = 0;
    lbl_80202898.b4A = 0;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (fn_8002E898_IsPad(gSession.nController[i])) {
            fn_800131C4(gSession.nController[i]);
        }
    }
}

// TW06: GameEffects_ResetGameEffectTimeSettings (by position and size).
GameEffects* fn_800DAF74(void) {
    lbl_80202898.b10 = 0;
    lbl_80202898.b11 = 0;
    lbl_80202898.bSlowMo = 0;
    lbl_80202898.b9 = 0;
    lbl_80202898.n28 = 0;
    return &lbl_80202898;
}

// TW06: GameEffects_AdjustTimeRate (by position). The game's time step for a frame that took
// fFrameTime: rounded to whole 60 Hz ticks (at most 3; a longer frame counts as 1), then scaled
// by the effects.
f32 fn_800DAF98(f32 fFrameTime) {
    f32 fTicks = 1.0f;
    f32 fBest = 10000.0f;
    int i;
    f32 d;
    if (fn_800DCB08() && 0.0f != fFrameTime) {
        fFrameTime = FRAME_TIME;
    }
    if (fn_8005D2DC()) {
        fFrameTime = 0.0f;
    }
    if (fn_800DCB00()) {
        if (0.0f != fFrameTime) {
            fFrameTime = FRAME_TIME;
        }
        fn_800DCAFC();
    }
    if (fn_800C6CCC()) {
        return 0.0f;
    }
    for (i = 0; i < 5; i++) {
        d = fabsf(i / FRAME_RATE - fFrameTime);
        if (d < fBest) {
            fBest = d;
        } else if (i > 0) {
            fTicks = i - 1;
            break;
        }
    }
    if (fn_800C6CB0()) {
        if (!fn_800B4AE0()) {
            fTicks *= 0.75f;
        }
    } else if (lbl_80202898.b9) {
        fTicks *= 2.0f;
        lbl_80202898.fC -= fFrameTime;
        if (lbl_80202898.fC < 0.0f) {
            lbl_80202898.b9 = 0;
        }
    } else {
        if (lbl_80202898.b10) {
            fTicks *= 2.0f;
        }
        if (lbl_80202898.b11) {
            fTicks *= 0.5f;
            lbl_80202898.n28++;
        }
    }
    if (lbl_80202898.bSlowMo) {
        return FRAME_TIME * fTicks * lbl_80202898.fSlowMo;
    }
    return FRAME_TIME * fTicks;
}

// How many physics steps the ball takes this frame: one per FRAME_TIME of frame time (rounded;
// twice that in mode 26), none when the frame time is 0, one outside the ball's flight. With the
// slow-down on and a frame shorter than FRAME_TIME, it moves only on every n2C-th frame.
int GameEffects_BallUpdatesThisFrame(int nPlayer) {
    if (gSession.nGameType == 3 || fn_800C714C()) {
        return 1;
    }
    if (0.0f == gSession.fFrameTime) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SIMULATE) {
        return 1;
    }
    if (fn_800DCB74() && gSession.fFrameTime < FRAME_TIME) {
        return fn_800DCB3C() != 0;
    }
    if (Game_GetMode() == 26) {
        if (gSession.fFrameTime <= FRAME_TIME) {
            return 2;
        }
        return 0.5f + 2.0f * gSession.fFrameTime / FRAME_TIME;
    }
    if (gSession.fFrameTime <= FRAME_TIME) {
        return 1;
    }
    return 0.5f + gSession.fFrameTime / FRAME_TIME;
}

// The target nearest the player's aim point (an inline in EA's source; calling fn_800F1D34
// directly does not match).
static inline int GE_CurrentTarget(int nPlayer) {
    return fn_800F1D34(nPlayer);
}

// TW06: GameEffects_ScriptedGameBreakerTrigger (by position; the same player and reason arguments).
// A scripted GameBreaker for reason nReason: reason 12 only while the round can still beat the
// course record, reason 15 only when three times fA64 (the ball's distance from the pin, in feet)
// beats record kind 2 (the longest putt), any other reason always. Only for a human, one view, not
// in a replay, and only when gpGame->b285 is set.
void fn_800DB30C(int nPlayer, int nReason) {
    if (((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) || gSession.bReplay ||
        gSession.nSplitScreen || gSession.a8[0] || !gpGame->b285) {
        return;
    }
    if (lbl_80202898.bGameBreaker != 1 && !Player_IsCPU(nPlayer)) {
        if (nReason == 12) {
            if (fn_800E17AC(nPlayer) + 1 >= gSession.aCourseRecord[Game_GetCourse()].aRecord[0][0].nValue) {
                return;
            }
        } else if (nReason == 15 && !(3.0f * gPlayers[nPlayer].fA64 >
                                      gSession.aCourseRecord[Game_GetCourse()].aRecord[2][0].nValue)) {
            return;
        }
        GB_START(nPlayer, nReason);
    }
}

// Scripted GameBreakers for the target games on course 7, for any player (not in a replay or split
// screen): mode 14 when fn_800F354C gives 4 (reason 17), mode 15 when every other player is out
// (22), mode 17 with 39 targets hit and mode 16 with 39 hit and the aimed-at target not yet (23).
void fn_800DB4E8(int nPlayer) {
    u8 bStart = 0;
    int nReason;
    int i;
    int n;
    if (!gSession.bReplay && !gSession.nSplitScreen && !gSession.a8[0]) {
        switch (Game_GetCourse()) {
        case 7:
            break;
        default:
            return;
        }
        if (lbl_80202898.bGameBreaker != 1) {
            if (Game_GetMode() == 14) {
                if (fn_800F354C(nPlayer) == 4) {
                    bStart = 1;
                    nReason = 17;
                }
            } else if (Game_GetMode() == 15) {
                n = 0;
                for (i = 0; i < gNumPlayersSetUp; i++) {
                    if (i != nPlayer && gPlayers[i].nE88 < 5) {
                        n++;
                    }
                }
                if (n == 0) {
                    bStart = 1;
                    nReason = 22;
                }
            } else if (Game_GetMode() == 17) {
                if (fn_800F20C0(nPlayer) == 39) {
                    bStart = 1;
                    nReason = 23;
                }
            } else if ((Game_GetMode() == 16 || Game_GetMode() == 16) && fn_800F20C0(nPlayer) == 39 &&
                       gPlayers[nPlayer].nDE4[GE_CurrentTarget(nPlayer)] == 0) {
                bStart = 1;
                nReason = 23;
            }
            if (bStart) {
                GB_START(nPlayer, nReason);
            }
        }
    }
}

// The GameBreaker camera: none on course 7; otherwise a camera at the shot's full distance.
void fn_800DB714(int nPlayer) {
    int nLie;
    f32 fDist;
    View* pView;
    if ((!(gSession.uFlags & 0x4000) || !(gSession.uFlags & 0x8000)) && lbl_80202898.bGameBreaker) {
        if (Game_GetCourse() == 7) {
            lbl_80202898.b19 = 1;
            return;
        }
        nLie = gPlayers[nPlayer].ball.nLie;
        fDist = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        fDist *= Physics_GetLiePowerPercentage(&gPlayers[nPlayer].ball);
        fDist *= fn_8005B64C(nPlayer);
        fn_80045494(0, nPlayer);
        fn_80045558(0, nPlayer);
        pView = fn_80017028(gPlayers[nPlayer].nView[0]);
        pView->p74 = fn_8003BDBC(nPlayer, nLie, 3, 0xC, 1, fDist);
        lbl_80202898.b19 = 1;
        if (lbl_80202898.f24 > 0.8f) {
            lbl_80202898.f24 = 0.8f;
        }
    }
}

// No TW06 name settled (by position it falls among ScriptedGameBreakerBallHitTrigger and
// IsScriptedGameBreaker). Whether the putt about to be played is a big one: on the green, and
// putting for two under par or better, or its length makes record kind 2's list, or the mode's
// pfn1F8 or fn_800BCD24 says so, or a birdie putt when fn_800D0620 gives 11, or an eagle putt when
// fn_800D089C gives 1 (by the score after a tap-in).
int fn_800DB86C(int nPlayer) {
    int bPossible = 0;
    int nPar;
    int nStrokes;
    CourseInfo* pCourse;
    void* pSurface;
    f32 fDist;
    pCourse = fn_8000C594();
    if (!pCourse) {
        return 0;
    }
    if (!gpGame->b285) {
        return 0;
    }
    nPar = fn_800D2B08();
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    pSurface = Ter_GetSupportingGroundMaterial(pCourse, gPlayers[nPlayer].vTarget2);
    fDist = fn_800D0478(nPlayer);
    if (!pSurface) {
        return 0;
    }
    if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && nPar - nStrokes >= 2) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e &&
               fn_800D8750(2, fDist, 0, gpSaveData[nPlayer].szName, nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && fn_800DCB10(nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && fn_800BCD24(nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && fn_800D0620(nPlayer, 0, 0) == 11 &&
               Hole_ScoreAfterTapIn(nPlayer) < 0) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && fn_800D089C(nPlayer, 0) == 1 &&
               Hole_ScoreAfterTapIn(nPlayer) < -1) {
        bPossible = 1;
    }
    return bPossible;
}

// TW06: GameEffects_InFlightGameBreakerTrigger (by position). A predicted GameBreaker starts, for
// a human's ball still at least 1 (the putter), 10 (a drive) or 5 (otherwise) from the look-ahead
// ball, with its own camera; a golfer who is not putting may get animation 14.
void fn_800DBA50(int nPlayer) {
    int nClass;
    int nLie;
    View* pView;
    CamSequence* pSeq;
    CamShot* pShot;
    f32 fDist;
    f32 fTime;
    f32 f2;
    int nKind;
    int nB;
    f32 f3;
    f32 v2[4];
    f32 v[4];
    if (((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) || gSession.bReplay ||
        gSession.nSplitScreen || gSession.a8[0] || !gpGame->b285) {
        return;
    }
    if (!(gPlayers[nPlayer].uFlags & 8) && !Player_IsCPU(nPlayer) && lbl_80202898.bGameBreaker != 1) {
        fn_800DCB84(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].ballBefore.vPos, v);
        v[1] = 0.0f;
        fDist = fn_80009680(fn_80009744(v));
        if (gPlayers[nPlayer].nClub == 25) {
            if (fDist < 1.0f) {
                return;
            }
        } else if (gPlayers[nPlayer].nShotKind == 1) {
            if (fDist < 10.0f) {
                return;
            }
        } else if (fDist < 5.0f) {
            return;
        }
        if (!gPlayers[nPlayer].b30D && fn_8000C594()) {
            fn_800DCB84(gPlayers[nPlayer].ball.vStart, gPlayers[nPlayer].ballBefore.vPos, v2);
            v2[1] = 0.0f;
            fDist = fn_80009680(fn_80009744(v2));
            nLie = gPlayers[nPlayer].ball.nLie;
            if (gPlayers[nPlayer].ballBefore.nSurface >= 0) {
                nClass = gSurfaceTypes[gPlayers[nPlayer].ballBefore.nSurface].nClass;
            } else {
                nClass = 10;
            }
            lbl_80202898.bClosing = 0;
            lbl_80202898.bGameBreaker = 1;
            lbl_80202898.fGBTime = 0.0f;
            lbl_80202898.f24 = 0.0f;
            lbl_80202898.nGBType = 1;
            lbl_80202898.nPlayer = nPlayer;
            lbl_80202898.bPaused = 0;
            lbl_80202898.nHeartbeats = 0;
            fn_80045494(0, nPlayer);
            fn_80045558(0, nPlayer);
            pView = fn_80017028(gPlayers[nPlayer].nView[0]);
            pSeq = fn_8003BDBC(nPlayer, nLie, nClass, 0xB, 1, fDist);
            pShot = fn_8003A950(pSeq, 0, &nKind, &fTime, &f2, &nB, &f3, nPlayer);
            if (pShot != NULL && pView->script.pShot != pShot && pView->script.pNextShot != pShot &&
                !fn_800451A8(&pView->script, pShot, nPlayer)) {
                if (nKind == 5 && fn_8003DC78(pShot)) {
                    if (gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e &&
                        fn_80095780(gPlayers[nPlayer].pChar) != 9) {
                        fn_80095744(gPlayers[nPlayer].pChar, 14);
                        if (0.0f == fTime) {
                            fTime = FRAME_TIME;
                        }
                    }
                }
                pView->p74 = pSeq;
                pView->script.nC4 = 0;
                pView->script.nC8 = 25;
            }
            EVENT_Trigger(nPlayer, 0x3F, 0, -1);
        }
    }
}

// TW06: GameEffects_EndGameBreaker (by position). The letterbox starts closing, with the end event;
// music 3 plays (a predicted one, or a scripted one that did it), or the old music comes back.
void fn_800DBDA8(int nPlayer) {
    if (lbl_80202898.bGameBreaker) {
        lbl_80202898.bClosing = 1;
        if (lbl_80202898.fGBTime > 0.8f) {
            lbl_80202898.fGBTime = 0.8f;
        }
        switch (lbl_80202898.nGBType) {
        case 1:
            EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
            if (lbl_80202898.b4A) {
                fn_800BD83C(lbl_80202898.u4C, 0);
                lbl_80202898.b4A = 0;
            }
            fn_800A6DCC(3, 1);
            return;
        case 0:
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
            if (fn_800DC818(&gPlayers[lbl_80202898.nPlayer].ball, lbl_80202898.nPlayer, 0)) {
                if (lbl_80202898.b4A) {
                    fn_800BD83C(lbl_80202898.u4C, 0);
                    lbl_80202898.b4A = 0;
                }
                fn_800A6DCC(3, 1);
            } else {
                if (lbl_80202898.b47) {
                    fn_800BD83C(lbl_80202898.u48, 0);
                }
                fn_800A6DCC(lbl_80202898.n4F, 1);
                lbl_80202898.b4E = 0;
            }
            lbl_80202898.b47 = 0;
            break;
        }
    }
}

// TW06: GameEffects_RenderGameBreakerEffects (by position).
void fn_800DBF34(void) {
    if (lbl_80202898.bGameBreaker && gSession.nPaused == 0 && !lbl_80202898.bPaused) {
        switch (lbl_80202898.nGBType) {
        case 1:
            fn_800DBFAC();
            return;
        case 0:
            fn_800DC18C();
            break;
        }
    }
}

// TW06: GameEffects_RenderPredictedGB (by position). Each frame of a predicted GameBreaker: the
// letterbox, and for a human the slow-down while the ball is within 2 (a putt) or 4 of the
// look-ahead ball and the letterbox is not closing.
void fn_800DBFAC(void) {
    f32 fHeight;
    f32 fDist;
    f32 v[3];
    GameEffects* pGE;
    if (lbl_80202898.fGBTime < 0.8f) {
        fHeight = 0.15f * (lbl_80202898.fGBTime / 0.8f);
    } else {
        fHeight = 0.15f;
    }
    pGE = &lbl_80202898;       // fake match: steers the register choice (found by the permuter)
    fn_800DCB84(gPlayers[pGE->nPlayer].ball.vPos, gPlayers[pGE->nPlayer].ballBefore.vPos, v);
    v[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(v));
    if (!Player_IsCPU(lbl_80202898.nPlayer)) {
        if (gPlayers[lbl_80202898.nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            if (fDist < 2.0f && !lbl_80202898.bClosing) {
                fn_80045494(1, lbl_80202898.nPlayer);
            } else {
                fn_80045494(0, pGE->nPlayer);
            }
        } else if (fDist < 4.0f && !lbl_80202898.bClosing) {
            fn_80045494(1, pGE->nPlayer);
        } else {
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        fn_80045494(0, pGE->nPlayer);
    }
    fn_800DC290(fHeight);
    if (lbl_80202898.bClosing) {
        lbl_80202898.fGBTime -= gSession.fFrameTime;
        if (lbl_80202898.fGBTime < 0.0f) {
            lbl_80202898.bGameBreaker = 0;
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        lbl_80202898.fGBTime += gSession.fFrameTime;
    }
}

// TW06: GameEffects_RenderScriptedGB (by position). The letterbox opens (or closes) at the
// game's pace; once closed the GameBreaker is over.
void fn_800DC18C(void) {
    f32 fHeight;
    if (lbl_80202898.b19) {
        lbl_80202898.f24 -= gSession.fFrameTime;
    } else {
        lbl_80202898.f24 += gSession.fFrameTime;
    }
    if (lbl_80202898.b19) {
        if (lbl_80202898.fGBTime < 0.8f) {
            fHeight = 0.15f * (lbl_80202898.fGBTime / 0.8f);
        } else {
            fHeight = 0.15f;
        }
        fn_800DC290(fHeight);
        if (lbl_80202898.bClosing) {
            lbl_80202898.fGBTime -= gSession.fFrameTime;
            if (lbl_80202898.fGBTime < 0.0f) {
                lbl_80202898.bGameBreaker = 0;
            }
        } else {
            lbl_80202898.fGBTime += gSession.fFrameTime;
        }
    }
}

// TW06: GameEffects_DrawLetterBoxes (by position). Two half-transparent black bars of fHeight
// (a fraction of the screen), at the top and bottom.
void fn_800DC290(f32 fHeight) {
    f32 colour[4];
    f32 xy[8];
    f32 uv[8];
    fn_8001425C(0);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012F50(0, 6, 0x80);
    fn_80014118(0);
    fn_80012EF8();
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
    colour[3] = 0.5f;
    fn_800141F8(xy, uv, 0.0f, 0.0f, 1.0f, fHeight);
    fn_80014194(colour);
    fn_8001644C(0xA1, xy, 0, uv, 2);
    fn_800141F8(xy, uv, 0.0f, 1.0f - fHeight, 1.0f, 1.0f);
    fn_80014194(colour);
    fn_8001644C(0xA1, xy, 0, uv, 2);
    fn_80012F34(1);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80012EF8();
}

// TW06: GameEffects_FieldOfViewChange (by position). The GameBreaker's field-of-view change: up to
// 0.349 (20 degrees) over the first 0.8 seconds; 0 when none runs or it is paused.
f32 GameEffects_FieldOfViewChange(void) {
    if (!lbl_80202898.bGameBreaker) {
        return 0.0f;
    }
    if (gSession.nPaused != 0) {
        return 0.0f;
    }
    if (lbl_80202898.bPaused) {
        return 0.0f;
    }
    if (lbl_80202898.nGBType == 1) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.34906587f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    if (lbl_80202898.nGBType == 0) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.34906587f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    return 0.0f;
}

// TW06: GameEffects_DepthOfFieldChange (by position).
f32 fn_800DC45C(void) {
    return 0.0f;
}

// TW06: GameEffects_SimulateBall (by position). A putt always; otherwise once the spin window is done.
u8 fn_800DC464(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
        return 1;
    }
    return lbl_80202898.bSpinWindowDone;
}

// TW06: GameEffects_SpinWindowDone (by position). The look-ahead copy restarts from the ball.
void fn_800DC498(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (!gSession.bReplay || p->ballBefore.nState != 0) {
        Mem_cpy(&p->ballBefore, &p->ball, sizeof(Ball));
        p->ballBefore.nPlayer = -1;
    }
    lbl_80202898.bSpinWindowDone = 1;
}

// TW06: GameEffects_IsSlowDownSwingOn (by position). Super slow motion is on; nPlayer is unused.
u8 fn_800DC514(int nPlayer) {
    return lbl_80202898.bSlowMo;
}

// Super slow motion on (with its rate) or off, with events 0x35/0x37 on and 0x36/0x38 off (by
// whether it slows down or speeds up).
void GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate) {
    if (bOn) {
        if (!lbl_80202898.bSlowMo) {
            if (fRate < 1.0f) {
                EVENT_Trigger(nPlayer, 0x35, gPlayers[nPlayer].vBall, -1);
            } else {
                EVENT_Trigger(nPlayer, 0x37, gPlayers[nPlayer].vBall, -1);
            }
            lbl_80202898.bSlowMo = bOn;
        }
        lbl_80202898.fSlowMo = fRate;
        lbl_80202898.b11 = 0;
        lbl_80202898.b10 = 0;
        return;
    }
    if (lbl_80202898.bSlowMo) {
        lbl_80202898.bSlowMo = bOn;
        if (lbl_80202898.fSlowMo < 1.0f) {
            EVENT_Trigger(nPlayer, 0x36, gPlayers[nPlayer].vBall, -1);
            return;
        }
        EVENT_Trigger(nPlayer, 0x38, gPlayers[nPlayer].vBall, -1);
    }
}

// TW06: GameEffects_UpdateGameEffects (by position). A heartbeat rumble lasts 5 frames.
void fn_800DC664(int nPlayer) {
    int nController = gPlayers[nPlayer].nController;
    if (lbl_80202898.bRumble) {
        if (++lbl_80202898.nRumbleFrames == 5) {
            lbl_80202898.bRumble = 0;
            if (fn_8002E898_IsPad(nController)) {
                fn_80013130(nController, 0);
            }
        }
    }
}

// TW06: GameEffects_VibrateControllerForHeartbeat (by position). Up to 40 beats.
void fn_800DC6E8(int nPlayer) {
    int nController;
    if (lbl_80202898.nHeartbeats < 40) {
        lbl_80202898.nHeartbeats++;
        nController = gPlayers[nPlayer].nController;
        if (!lbl_80202898.bRumble) {
            if (fn_8002E898_IsPad(nController)) {
                fn_80013130(nController, 0xFF);
            }
            lbl_80202898.bRumble = 1;
            lbl_80202898.nRumbleFrames = 0;
        }
    }
}

// TW06: GameEffects_SkipOtherCommentary (by position). While a GameBreaker is up: always for a
// predicted one; while it closes, if the shot "did it"; otherwise once it is fully open.
u8 fn_800DC784(void) {
    if (!lbl_80202898.bGameBreaker) {
        return 0;
    }
    if (lbl_80202898.nGBType == 1) {
        return 1;
    }
    if (lbl_80202898.bClosing) {
        return fn_800DC818(&gPlayers[lbl_80202898.nPlayer].ball, lbl_80202898.nPlayer, 0);
    }
    return lbl_80202898.fGBTime >= 0.8f;
}

// TW06: GameEffects_ScriptedGBDidIt (by position). Whether the shot earned its GameBreaker:
// holed within the stroke limit, on the green of a par 5 in two (with uFlags bit 0x4000), or one of
// two record checks (fn_800D7660, fn_800D782C: a drive record; without bNext and with neither, a
// big message waiting, fn_800E5344).
u8 fn_800DC818(Ball* pBall, int nPlayer, u8 bNext) {
    u8  bEagle;
    int a;
    int b;
    int nStrokes;
    if (pBall->nLie != LIE_GREEN_e || !(lbl_80202898.uFlags & 0x4000)) {
        bEagle = 0;
    } else if (fn_800D2B08() != 5) {
        bEagle = 0;
    } else if (bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 1) {
        bEagle = 0;
    } else if (!bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 2) {
        bEagle = 0;
    } else {
        bEagle = 1;
    }
    a = fn_800D7660(nPlayer, pBall, bNext);
    b = fn_800D782C(nPlayer, pBall, 0, bNext, 1);
    if (!bNext) {
        if (b == 0 && a == 0) {
            b = fn_800E5344();
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    } else {
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    }
    if ((pBall->nLie == LIE_INCUP_e && !fn_800E23B0(nPlayer, nStrokes - 1)) || bEagle || b || a) {
        return 1;
    }
    return 0;
}

// TW06: GameEffects_Pause (by position). Pauses or resumes a GameBreaker, with its sound events.
void fn_800DC9D4(int a) {
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.bPaused) {
            lbl_80202898.bPaused = 0;
            if (lbl_80202898.nGBType == 0) {
                EVENT_Trigger(lbl_80202898.nPlayer, 0x3D, 0, -1);
                return;
            }
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3F, 0, -1);
            return;
        }
        lbl_80202898.bPaused = 1;
        if (lbl_80202898.nGBType == 0) {
            EVENT_Trigger(lbl_80202898.nPlayer, 0x3E, 0, -1);
            return;
        }
        EVENT_Trigger(lbl_80202898.nPlayer, 0x40, 0, -1);
    }
}

// The letterbox grows to 15% of the screen over its first 0.8 seconds.
f32 GameEffects_GetLetterboxHeight(void) {
    if (lbl_80202898.bGameBreaker) {
        if (lbl_80202898.fGBTime < 0.8f) {
            return 0.15f * (lbl_80202898.fGBTime / 0.8f);
        }
        return 0.15f;
    }
    return 0.0f;
}
