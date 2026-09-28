// GameEffects.c (EA's name: TW06's and TW07's golf/gamemode/GameEffects.c, GameEffects_*; both keep
// this file's function order, which gives the names): the game's time rate (double time, half
// time, super slow motion), the GameBreaker, and the heartbeat rumble.
//
// A GameBreaker is the dramatic moment of a shot: black letterbox bars, a wider field of view, its
// own camera and a slow-down near the end, with its start and end sound events (0x3D..0x40). A
// scripted one is started by SitDev's actions for a reason (a putt for a record, the lead or an
// eagle; the target games) and ends with the post-GameBreaker commentary and crowd; a predicted
// one starts while the ball flies, from the look-ahead ball. All of it lives in one state,
// gGameEffects (game.h). The small helpers it calls (GameEffects_IsFixedTimeStepOn,
// _IsSingleStepPending, _ClearSingleStep, _StartOfSlowMoFrame, _IsHalfTimeOn, _SendMessage50,
// _Vec3Sub, GM_IsPuttForLead) are defined at the top of GameManager.c; half and double time are
// switched in gocamscripts.c (fn_80045494, fn_80045558).

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

u8    fn_800B4AE0(void);
void  GameEffects_RenderPredictedGB(void);
void  GameEffects_RenderScriptedGB(void);
void  GameEffects_DrawLetterBoxes(f32 fHeight);
// szName: a profile's name
int   HighScoreRecords_CheckRecord(int a, int b, int c, char* szName, int nPlayer);
u8    fn_800BCD24(int nPlayer);

GameEffects gGameEffects;   // the effects state (game.h; GameManager, GoGolfCam and others read it)

// Starts a scripted GameBreaker for player nWho, for reason nWhy (a bit in uFlags).
#define GB_START(nWho, nWhy)                                                                \
    if (!gGameEffects.bGameBreaker || gGameEffects.bClosing || gGameEffects.nGBType != 0) { \
        gGameEffects.bClosing = 0;                                                          \
        gGameEffects.bGameBreaker = 1;                                                      \
        gGameEffects.fGBTime = 0.0f;                                                        \
        gGameEffects.f24 = 0.0f;                                                            \
        gGameEffects.b19 = 0;                                                               \
        gGameEffects.nGBType = 0;                                                           \
        gGameEffects.nPlayer = (nWho);                                                      \
        gGameEffects.bPaused = 0;                                                           \
        gGameEffects.uFlags = 1 << (nWhy);                                                  \
        gGameEffects.nHeartbeats = 0;                                                       \
        EVENT_Trigger((nWho), 0x3D, 0, -1);                                                 \
    }

// Once, as the game starts (GO_vInitIG): double time (b10), half time (b11), super slow motion and
// the timed double speed (b9) off, the half-time frame count n28 cleared, no GameBreaker, not
// paused, no rumble or heartbeats, and f54 (a menu command sets it, GameUICommands.c) back to 1.
void GameEffects_InitGameEffectSettings(void) {
    gGameEffects.b10 = 0;
    gGameEffects.b11 = 0;
    gGameEffects.bSlowMo = 0;
    gGameEffects.b9 = 0;
    gGameEffects.n28 = 0;
    gGameEffects.bGameBreaker = 0;
    gGameEffects.b19 = 0;
    gGameEffects.bPaused = 0;
    gGameEffects.bRumble = 0;
    gGameEffects.nHeartbeats = 0;
    gGameEffects.f54 = 1.0f;
}

// Every effect off (at each hole's start and restart, the scorecards, and the swing and shot
// states' inits): the time effects (GameEffects_ResetGameEffectTimeSettings), pause and rumble. A
// GameBreaker still up ends at once, with its end event (0x3E scripted, 0x40 predicted) and UI
// message 50; the spin window and the waiting commentary line u4C are cleared, and every player's
// pad stops vibrating.
void GameEffects_ResetGameEffectSettings(void) {
    int i;
    GameEffects_ResetGameEffectTimeSettings();
    gGameEffects.bPaused = 0;
    gGameEffects.bRumble = 0;
    if (gGameEffects.bGameBreaker) {
        if (gGameEffects.nGBType == 0) {
            EVENT_Trigger(gGameEffects.nPlayer, 0x3E, 0, -1);
        } else {
            EVENT_Trigger(gGameEffects.nPlayer, 0x40, 0, -1);
        }
        GameEffects_SendMessage50();
    }
    gGameEffects.bGameBreaker = 0;
    gGameEffects.bSpinWindowDone = 0;
    gGameEffects.b19 = 0;
    gGameEffects.b4A = 0;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (fn_8002E898_IsPad(gSession.nController[i])) {
            Input_vStopVibration(gSession.nController[i]);
        }
    }
}

// The time effects off: double time, half time, super slow motion, the timed double speed (b9) and
// the half-time frame count n28. Returns the effects state (its callers,
// GameEffects_ResetGameEffectSettings and STATEFUNC_ReplaySwingInit, ignore it).
GameEffects* GameEffects_ResetGameEffectTimeSettings(void) {
    gGameEffects.b10 = 0;
    gGameEffects.b11 = 0;
    gGameEffects.bSlowMo = 0;
    gGameEffects.b9 = 0;
    gGameEffects.n28 = 0;
    return &gGameEffects;
}

// The game's time step for a frame that took fFrameTime seconds (the main loop asks once a frame,
// fn_8006D8E8). FRAME_TIME when the fixed step is on or a single step is pending (the request is
// used up), 0 while the golfer state is frozen, and 0 outright when fn_800C6CCC says so. Otherwise
// the frame is rounded to whole 60 Hz ticks (0 to 3; a longer frame counts as 1), then scaled: to
// 3/4 while the golf cameras' b56 is set (fn_800C6CB0) and the golfer has not passed animation tag
// 2 (fn_800B4AE0); else doubled while the timed double speed runs (b9, until fC counts down below
// 0); else doubled with double time and halved with half time (counting n28). Super slow motion
// then multiplies it by fSlowMo.
f32 GameEffects_AdjustTimeRate(f32 fFrameTime) {
    f32 fTicks = 1.0f;
    f32 fBest = 10000.0f;
    int i;
    f32 d;
    if (GameEffects_IsFixedTimeStepOn() && 0.0f != fFrameTime) {
        fFrameTime = FRAME_TIME;
    }
    if (GOLFERSTATE_IsFrozen()) {
        fFrameTime = 0.0f;
    }
    if (GameEffects_IsSingleStepPending()) {
        if (0.0f != fFrameTime) {
            fFrameTime = FRAME_TIME;
        }
        GameEffects_ClearSingleStep();
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
    } else if (gGameEffects.b9) {
        fTicks *= 2.0f;
        gGameEffects.fC -= fFrameTime;
        if (gGameEffects.fC < 0.0f) {
            gGameEffects.b9 = 0;
        }
    } else {
        if (gGameEffects.b10) {
            fTicks *= 2.0f;
        }
        if (gGameEffects.b11) {
            fTicks *= 0.5f;
            gGameEffects.n28++;
        }
    }
    if (gGameEffects.bSlowMo) {
        return FRAME_TIME * fTicks * gGameEffects.fSlowMo;
    }
    return FRAME_TIME * fTicks;
}

// How many physics steps the ball takes this frame: 1 in game type 3 or while the camera's script
// matrix mode is on, 0 when the frame time is 0, 1 outside the ball's flight (golfer state not
// GS_SIMULATE). With half time on and a frame shorter than FRAME_TIME, 1 only on every n2C-th frame
// (GameEffects_StartOfSlowMoFrame), else 0. Otherwise one step per FRAME_TIME of frame time,
// rounded, at least 1 (twice that, at least 2, in mode 26).
int GameEffects_BallUpdatesThisFrame(int nPlayer) {
    if (gSession.nGameType == 3 || GolfCamera_IsScriptMatrixModeOn()) {
        return 1;
    }
    if (0.0f == gSession.fFrameTime) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_SIMULATE) {
        return 1;
    }
    if (GameEffects_IsHalfTimeOn() && gSession.fFrameTime < FRAME_TIME) {
        return GameEffects_StartOfSlowMoFrame() != 0;
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

// The target nearest the player's aim point (an inline in EA's source; calling
// GameModeSkillZoneBase_GetGreenTargetted directly does not match).
static inline int GE_CurrentTarget(int nPlayer) {
    return GameModeSkillZoneBase_GetGreenTargetted(nPlayer);
}

// A scripted GameBreaker starts for nPlayer, for reason nReason (SitDev's actions fire it; the
// reason is kept as bit 1 << nReason in uFlags), with event 0x3D. Reason 12 only while the round
// can still beat the course record (record kind 0), reason 15 only when 3 * Player.fA64 beats
// record kind 2's best, any other reason always. Never in the demo, a replay or split screen, with
// gSession.a8[0] set, when the mode allows no GameBreakers (gpGame->b285 clear), for a CPU player,
// or while one is up.
void GameEffects_ScriptedGameBreakerTrigger(int nPlayer, int nReason) {
    if (((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) || gSession.bReplay ||
        gSession.nSplitScreen || gSession.a8[0] || !gpGame->b285) {
        return;
    }
    if (gGameEffects.bGameBreaker != 1 && !Player_IsCPU(nPlayer)) {
        if (nReason == 12) {
            if (GM_GetPlayerRoundStrokes(nPlayer) + 1
                >= gSession.aCourseRecord[Game_GetCourse()].aRecord[0][0].nValue) {
                return;
            }
        } else if (nReason == 15 && !(3.0f * gPlayers[nPlayer].fA64 >
                                      gSession.aCourseRecord[Game_GetCourse()].aRecord[2][0].nValue)) {
            return;
        }
        GB_START(nPlayer, nReason);
    }
}

// As a swing starts (STATEFUNC_SwingInit), the target games' scripted GameBreakers, on course 7
// only, for any player (not in a replay or split screen, not with gSession.a8[0] set, not while one
// is up): mode 14 when the player holds 4 targets (GameModeSkillZoneCapture_GetTotalTargetsHit;
// reason 17), mode 15 when every other player is out (nE88 5 or more; reason 22), mode 17 with 39 targets hit
// (GameModeSkillZoneBase_CountGreensHit; reason 23), mode 16 with 39 hit and the aimed-at target
// not yet hit (reason 23). It starts as GameEffects_ScriptedGameBreakerTrigger does (event 0x3D).
void GameEffects_TargetGameBreakerTrigger(int nPlayer) {
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
        if (gGameEffects.bGameBreaker != 1) {
            if (Game_GetMode() == 14) {
                if (GameModeSkillZoneCapture_GetTotalTargetsHit(nPlayer) == 4) {
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
                if (GameModeSkillZoneBase_CountGreensHit(nPlayer) == 39) {
                    bStart = 1;
                    nReason = 23;
                }
            } else if ((Game_GetMode() == 16 || Game_GetMode() == 16) &&
                       GameModeSkillZoneBase_CountGreensHit(nPlayer) == 39 &&
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

// As the ball is hit (STATEFUNC_SimulateInit), for a GameBreaker that is up (not in the demo): half
// and double time off, and the player's view gets GameBreaker camera sequence 0xC (fn_8003BDBC,
// from the lie the ball is hit from) for the shot's full distance (AI_MaxDistance times the lie's
// power times the swing's power). b19 is set, which starts the letterbox
// (GameEffects_RenderScriptedGB), and f24 is held to 0.8 at most. On course 7 there is no camera,
// only b19.
void GameEffects_ScriptedGameBreakerBallHitTrigger(int nPlayer) {
    int nLie;
    f32 fDist;
    View* pView;
    if ((!(gSession.uFlags & 0x4000) || !(gSession.uFlags & 0x8000)) && gGameEffects.bGameBreaker) {
        if (Game_GetCourse() == 7) {
            gGameEffects.b19 = 1;
            return;
        }
        nLie = gPlayers[nPlayer].ball.nLie;
        fDist = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        fDist *= Physics_GetLiePowerPercentage(&gPlayers[nPlayer].ball);
        fDist *= SW_vGetShotPower(nPlayer);
        fn_80045494(0, nPlayer);
        fn_80045558(0, nPlayer);
        pView = ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]);
        pView->p74 = fn_8003BDBC(nPlayer, nLie, 3, 0xC, 1, fDist);
        gGameEffects.b19 = 1;
        if (gGameEffects.f24 > 0.8f) {
            gGameEffects.f24 = 0.8f;
        }
    }
}

// Whether the putt about to be played earns a scripted GameBreaker (emotion.c asks, fn_8006B0B8).
// Needs a course loaded, a mode that allows GameBreakers (gpGame->b285), ground under the aim point
// and the ball on the green; then any of: a putt that would score an eagle or better, a distance to
// the pin (fn_800D0478) that makes record kind 2's list (HighScoreRecords_CheckRecord), a putt for
// the lead (GM_IsPuttForLead) or for the win (fn_800BCD24, the mode's pfn1FC), a birdie putt when
// fn_800D0620 gives 11, or an eagle putt when fn_800D089C gives 1 (birdie and eagle by
// Hole_ScoreAfterTapIn).
int GameEffects_IsScriptedGameBreaker(int nPlayer) {
    int bPossible = 0;
    int nPar;
    int nStrokes;
    CourseInfo* pCourse;
    void* pSurface;
    f32 fDist;
    pCourse = Ter_GetTGD();
    if (!pCourse) {
        return 0;
    }
    if (!gpGame->b285) {
        return 0;
    }
    nPar = Course_GetCurHolePar();
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    pSurface = Ter_GetSupportingGroundMaterial(pCourse, gPlayers[nPlayer].vTarget2);
    fDist = fn_800D0478(nPlayer);
    if (!pSurface) {
        return 0;
    }
    if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && nPar - nStrokes >= 2) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e &&
               HighScoreRecords_CheckRecord(2, fDist, 0, gpSaveData[nPlayer].szName, nPlayer)) {
        bPossible = 1;
    } else if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e && GM_IsPuttForLead(nPlayer)) {
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

// A predicted GameBreaker starts while the ball flies (SitDev's actions fire it), for a human
// (player flag 8 clear), none up yet; never in the demo, a replay or split screen, with
// gSession.a8[0] set, or when the mode allows no GameBreakers (gpGame->b285 clear). The ball must
// still be at least 1 (club 25, the putter), 10 (a drive, nShotKind 1) or 5 (any other shot) from
// the look-ahead ball across the ground, b30D clear and a course loaded. Half and double time go
// off, and the player's view gets GameBreaker camera sequence 0xB (fn_8003BDBC) for the shot's
// length to the look-ahead ball, by the ball's lie and the surface class where the look-ahead ball
// lies. When the shot it picks tracks the golfer (kind 5, fn_8003DC78) on a shot that is not a
// putt, the golfer is set to animation 14 unless he is in 9. Event 0x3F.
void GameEffects_InFlightGameBreakerTrigger(int nPlayer) {
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
    if (!(gPlayers[nPlayer].uFlags & 8) && !Player_IsCPU(nPlayer) && gGameEffects.bGameBreaker != 1) {
        GameEffects_Vec3Sub(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].ballBefore.vPos, v);
        v[1] = 0.0f;
        fDist = Math_Sqrt(Vec3_LengthSqClamped(v));
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
        if (!gPlayers[nPlayer].b30D && Ter_GetTGD()) {
            GameEffects_Vec3Sub(gPlayers[nPlayer].ball.vStart, gPlayers[nPlayer].ballBefore.vPos, v2);
            v2[1] = 0.0f;
            fDist = Math_Sqrt(Vec3_LengthSqClamped(v2));
            nLie = gPlayers[nPlayer].ball.nLie;
            if (gPlayers[nPlayer].ballBefore.nSurface >= 0) {
                nClass = gSurfaceTypes[gPlayers[nPlayer].ballBefore.nSurface].nClass;
            } else {
                nClass = 10;
            }
            gGameEffects.bClosing = 0;
            gGameEffects.bGameBreaker = 1;
            gGameEffects.fGBTime = 0.0f;
            gGameEffects.f24 = 0.0f;
            gGameEffects.nGBType = 1;
            gGameEffects.nPlayer = nPlayer;
            gGameEffects.bPaused = 0;
            gGameEffects.nHeartbeats = 0;
            fn_80045494(0, nPlayer);
            fn_80045558(0, nPlayer);
            pView = ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]);
            pSeq = fn_8003BDBC(nPlayer, nLie, nClass, 0xB, 1, fDist);
            pShot = DynamicCam_ChooseScriptInSequence(pSeq, 0, &nKind, &fTime, &f2, &nB, &f3, nPlayer);
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

// Ends a GameBreaker as the shot finishes (STATEFUNC_SimulateUpdate): the letterbox starts closing
// (fGBTime held to 0.8 at most). A predicted one: end event 0x40, the waiting commentary line u4C
// (if any) plays, and crowd reaction 3. A scripted one: end event 0x3E; if the shot did it
// (GameEffects_ScriptedGBDidIt), line u4C and crowd reaction 3, else the other waiting line u48 and
// crowd reaction n4F (b4E cleared); u48's slot is freed either way. nPlayer is not read: the
// GameBreaker's own player is used.
void GameEffects_EndGameBreaker(int nPlayer) {
    if (gGameEffects.bGameBreaker) {
        gGameEffects.bClosing = 1;
        if (gGameEffects.fGBTime > 0.8f) {
            gGameEffects.fGBTime = 0.8f;
        }
        switch (gGameEffects.nGBType) {
        case 1:
            EVENT_Trigger(gGameEffects.nPlayer, 0x40, 0, -1);
            if (gGameEffects.b4A) {
                fn_800BD83C(gGameEffects.u4C, 0);
                gGameEffects.b4A = 0;
            }
            Gaud_InitCrowdReactionSound(3, 1);
            return;
        case 0:
            EVENT_Trigger(gGameEffects.nPlayer, 0x3E, 0, -1);
            if (GameEffects_ScriptedGBDidIt(&gPlayers[gGameEffects.nPlayer].ball, gGameEffects.nPlayer, 0)) {
                if (gGameEffects.b4A) {
                    fn_800BD83C(gGameEffects.u4C, 0);
                    gGameEffects.b4A = 0;
                }
                Gaud_InitCrowdReactionSound(3, 1);
            } else {
                if (gGameEffects.b47) {
                    fn_800BD83C(gGameEffects.u48, 0);
                }
                Gaud_InitCrowdReactionSound(gGameEffects.n4F, 1);
                gGameEffects.b4E = 0;
            }
            gGameEffects.b47 = 0;
            break;
        }
    }
}

// Each frame (the main loop, fn_8006D27C), while a GameBreaker is up and neither the game nor the
// GameBreaker is paused: its letterbox and timing, by its type (GameEffects_RenderPredictedGB or
// GameEffects_RenderScriptedGB).
void GameEffects_RenderGameBreakerEffects(void) {
    if (gGameEffects.bGameBreaker && gSession.nPaused == 0 && !gGameEffects.bPaused) {
        switch (gGameEffects.nGBType) {
        case 1:
            GameEffects_RenderPredictedGB();
            return;
        case 0:
            GameEffects_RenderScriptedGB();
            break;
        }
    }
}

// Each frame of a predicted GameBreaker: half time on for a human while the ball is within 2 (a
// putt) or 4 (any other shot) of the look-ahead ball across the ground and the letterbox is not
// closing, off otherwise; the letterbox drawn (growing to 0.15 of the screen over the first 0.8 s);
// fGBTime advanced, or run down while closing until the GameBreaker is over (half time off).
void GameEffects_RenderPredictedGB(void) {
    f32 fHeight;
    f32 fDist;
    f32 v[3];
    GameEffects* pGE;
    if (gGameEffects.fGBTime < 0.8f) {
        fHeight = 0.15f * (gGameEffects.fGBTime / 0.8f);
    } else {
        fHeight = 0.15f;
    }
    pGE = &gGameEffects;       // fake match: steers the register choice (found by the permuter)
    GameEffects_Vec3Sub(gPlayers[pGE->nPlayer].ball.vPos, gPlayers[pGE->nPlayer].ballBefore.vPos, v);
    v[1] = 0.0f;
    fDist = Math_Sqrt(Vec3_LengthSqClamped(v));
    if (!Player_IsCPU(gGameEffects.nPlayer)) {
        if (gPlayers[gGameEffects.nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            if (fDist < 2.0f && !gGameEffects.bClosing) {
                fn_80045494(1, gGameEffects.nPlayer);
            } else {
                fn_80045494(0, pGE->nPlayer);
            }
        } else if (fDist < 4.0f && !gGameEffects.bClosing) {
            fn_80045494(1, pGE->nPlayer);
        } else {
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        fn_80045494(0, pGE->nPlayer);
    }
    GameEffects_DrawLetterBoxes(fHeight);
    if (gGameEffects.bClosing) {
        gGameEffects.fGBTime -= gSession.fFrameTime;
        if (gGameEffects.fGBTime < 0.0f) {
            gGameEffects.bGameBreaker = 0;
            fn_80045494(0, pGE->nPlayer);
        }
    } else {
        gGameEffects.fGBTime += gSession.fFrameTime;
    }
}

// Each frame of a scripted GameBreaker: f24 counts up until b19 is set (the ball is hit,
// GameEffects_ScriptedGameBreakerBallHitTrigger), then down. From b19 on the letterbox is drawn
// (growing to 0.15 of the screen over 0.8 s) and fGBTime advances, or runs down while closing until
// the GameBreaker is over.
void GameEffects_RenderScriptedGB(void) {
    f32 fHeight;
    if (gGameEffects.b19) {
        gGameEffects.f24 -= gSession.fFrameTime;
    } else {
        gGameEffects.f24 += gSession.fFrameTime;
    }
    if (gGameEffects.b19) {
        if (gGameEffects.fGBTime < 0.8f) {
            fHeight = 0.15f * (gGameEffects.fGBTime / 0.8f);
        } else {
            fHeight = 0.15f;
        }
        GameEffects_DrawLetterBoxes(fHeight);
        if (gGameEffects.bClosing) {
            gGameEffects.fGBTime -= gSession.fFrameTime;
            if (gGameEffects.fGBTime < 0.0f) {
                gGameEffects.bGameBreaker = 0;
            }
        } else {
            gGameEffects.fGBTime += gSession.fFrameTime;
        }
    }
}

// The letterbox: two half-transparent black bars fHeight high (a fraction of the screen) at the top
// and the bottom, drawn without depth writes; the depth and alpha-test modes are put back after.
void GameEffects_DrawLetterBoxes(f32 fHeight) {
    f32 colour[4];
    f32 xy[8];
    f32 uv[8];
    RenderView_SetUseCurrentMatrices(0);
    DS_vEnableZBufferUpdate(0);
    DS_vSetZBufferMode(7);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    RenderState_SetDrawFlags(0);
    RenderState_Flush();
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
    colour[3] = 0.5f;
    RenderView_MakeQuad(xy, uv, 0.0f, 0.0f, 1.0f, fHeight);
    RenderView_SetColor(colour);
    RenderView_DrawPrimitive(0xA1, xy, 0, uv, 2);
    RenderView_MakeQuad(xy, uv, 0.0f, 1.0f - fHeight, 1.0f, 1.0f);
    RenderView_SetColor(colour);
    RenderView_DrawPrimitive(0xA1, xy, 0, uv, 2);
    DS_vEnableZBufferUpdate(1);
    DS_vSetAlphaTestMode(1, 6, 0x80);
    DS_vSetZBufferMode(3);
    RenderState_Flush();
}

// The GameBreaker's field-of-view change, which the camera scripts add (gocamscripts.c): growing to
// 0.349 radians (20 degrees) over the letterbox's first 0.8 s, for either type; 0 when none is up
// or the game or the GameBreaker is paused.
f32 GameEffects_FieldOfViewChange(void) {
    if (!gGameEffects.bGameBreaker) {
        return 0.0f;
    }
    if (gSession.nPaused != 0) {
        return 0.0f;
    }
    if (gGameEffects.bPaused) {
        return 0.0f;
    }
    if (gGameEffects.nGBType == 1) {
        if (gGameEffects.fGBTime < 0.8f) {
            return 0.34906587f * (gGameEffects.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    if (gGameEffects.nGBType == 0) {
        if (gGameEffects.fGBTime < 0.8f) {
            return 0.34906587f * (gGameEffects.fGBTime / 0.8f);
        }
        return 0.34906587f;
    }
    return 0.0f;
}

// The GameBreaker's depth-of-field change, which the camera scripts add: always 0 in this build.
// fCurDof, the current depth of field, is not read (every caller passes it; TW07's takes curDOF).
f32 GameEffects_DepthOfFieldChange(f32 fCurDof) {
    return 0.0f;
}

// Whether the camera's landing estimate may simulate the ball ahead
// (CameraScript_UpdateLandingEstimate asks): always for a putt, otherwise once the spin window is
// done (GameEffects_SpinWindowDone).
u8 GameEffects_SimulateBall(int nPlayer) {
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
        return 1;
    }
    return gGameEffects.bSpinWindowDone;
}

// The spin window is over (event.c, fn_80066BE8): the look-ahead ball restarts as a copy of the
// ball (player -1), except in a replay while the look-ahead ball is at rest (nState 0); from now on
// GameEffects_SimulateBall says yes.
void GameEffects_SpinWindowDone(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (!gSession.bReplay || p->ballBefore.nState != 0) {
        Mem_cpy(&p->ballBefore, &p->ball, sizeof(Ball));
        p->ballBefore.nPlayer = -1;
    }
    gGameEffects.bSpinWindowDone = 1;
}

// Whether super slow motion is on (the swing camera asks, GolfCamera_ProcessSwingCamera); nPlayer
// is not read.
u8 GameEffects_IsSlowDownSwingOn(int nPlayer) {
    return gGameEffects.bSlowMo;
}

// Super slow motion on at rate fRate (GameEffects_AdjustTimeRate multiplies the time step by it:
// below 1 slows the game, above 1 speeds it up) or off. Turning it on sends event 0x35 (fRate below
// 1) or 0x37 once and clears half and double time; turning it off sends 0x36 or 0x38 by the rate it
// had. The heartbeat and shutter cameras and the swing replay use it.
void GameEffects_SetSuperSlowMo(u8 bOn, int nPlayer, f32 fRate) {
    if (bOn) {
        if (!gGameEffects.bSlowMo) {
            if (fRate < 1.0f) {
                EVENT_Trigger(nPlayer, 0x35, gPlayers[nPlayer].vBall, -1);
            } else {
                EVENT_Trigger(nPlayer, 0x37, gPlayers[nPlayer].vBall, -1);
            }
            gGameEffects.bSlowMo = bOn;
        }
        gGameEffects.fSlowMo = fRate;
        gGameEffects.b11 = 0;
        gGameEffects.b10 = 0;
        return;
    }
    if (gGameEffects.bSlowMo) {
        gGameEffects.bSlowMo = bOn;
        if (gGameEffects.fSlowMo < 1.0f) {
            EVENT_Trigger(nPlayer, 0x36, gPlayers[nPlayer].vBall, -1);
            return;
        }
        EVENT_Trigger(nPlayer, 0x38, gPlayers[nPlayer].vBall, -1);
    }
}

// Each frame (the main loop, fn_8006D27C): a heartbeat rumble stops after 5 frames (vibration 0 on
// the player's pad).
void GameEffects_UpdateGameEffects(int nPlayer) {
    int nController = gPlayers[nPlayer].nController;
    if (gGameEffects.bRumble) {
        if (++gGameEffects.nRumbleFrames == 5) {
            gGameEffects.bRumble = 0;
            if (fn_8002E898_IsPad(nController)) {
                Input_vVibrateWave(nController, 0);
            }
        }
    }
}

// One heartbeat (GameAudio's HeartBeatLoopCallback, on each beat of the heartbeat sound): the
// player's pad vibrates at full strength for 5 frames (GameEffects_UpdateGameEffects stops it). At
// most 40 beats; a new GameBreaker starts the count again.
void GameEffects_VibrateControllerForHeartbeat(int nPlayer) {
    int nController;
    if (gGameEffects.nHeartbeats < 40) {
        gGameEffects.nHeartbeats++;
        nController = gPlayers[nPlayer].nController;
        if (!gGameEffects.bRumble) {
            if (fn_8002E898_IsPad(nController)) {
                Input_vVibrateWave(nController, 0xFF);
            }
            gGameEffects.bRumble = 1;
            gGameEffects.nRumbleFrames = 0;
        }
    }
}

// Whether other commentary is skipped for the GameBreaker (SitDevTrigger.c asks): while a predicted
// one is up; a scripted one once its letterbox is fully open (0.8 s), and while it closes only if
// the shot did it (GameEffects_ScriptedGBDidIt).
u8 GameEffects_SkipOtherCommentary(void) {
    if (!gGameEffects.bGameBreaker) {
        return 0;
    }
    if (gGameEffects.nGBType == 1) {
        return 1;
    }
    if (gGameEffects.bClosing) {
        return GameEffects_ScriptedGBDidIt(&gPlayers[gGameEffects.nPlayer].ball, gGameEffects.nPlayer, 0);
    }
    return gGameEffects.fGBTime >= 0.8f;
}

// Whether the shot earned its scripted GameBreaker, for pBall where it ended (bNext 0) or where it
// is predicted to end (bNext 1, the look-ahead ball): in the cup within the stroke limit
// (GM_IsShotOverLimit), on the green of a par 5 in two when the GameBreaker was started for that
// (reason 14, uFlags bit 0x4000), a trophy-ball award (Earnings_CheckShotAwards) or a record
// (HighScoreRecords_GetEndOfShotRecord); after the shot, with neither of those two, also a big
// message waiting to be shown (GUI_AreTrophysOrRecordsQueued).
u8 GameEffects_ScriptedGBDidIt(Ball* pBall, int nPlayer, u8 bNext) {
    u8  bPar5In2;
    int nTrophyBall;
    int nRecordBall;
    int nStrokes;
    if (pBall->nLie != LIE_GREEN_e || !(gGameEffects.uFlags & 0x4000)) {
        bPar5In2 = 0;
    } else if (Course_GetCurHolePar() != 5) {
        bPar5In2 = 0;
    } else if (bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 1) {
        bPar5In2 = 0;
    } else if (!bNext && gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] != 2) {
        bPar5In2 = 0;
    } else {
        bPar5In2 = 1;
    }
    nTrophyBall = Earnings_CheckShotAwards(nPlayer, pBall, bNext);
    nRecordBall = HighScoreRecords_GetEndOfShotRecord(nPlayer, pBall, 0, bNext, 1);
    if (!bNext) {
        if (nRecordBall == 0 && nTrophyBall == 0) {
            nRecordBall = GUI_AreTrophysOrRecordsQueued();
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    } else {
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
    }
    if ((pBall->nLie == LIE_INCUP_e && !GM_IsShotOverLimit(nPlayer, nStrokes - 1)) || bPar5In2 ||
        nRecordBall || nTrophyBall) {
        return 1;
    }
    return 0;
}

// Pauses or resumes a GameBreaker that is up (the pause menu, the mid-hole fly-by, GameMessages.c):
// bPaused toggles, with the end event as it pauses (0x3E scripted, 0x40 predicted) and the start
// event as it resumes (0x3D, 0x3F). The argument is not read (TW07's is bool pauseOn).
void GameEffects_Pause(int a) {
    if (gGameEffects.bGameBreaker) {
        if (gGameEffects.bPaused) {
            gGameEffects.bPaused = 0;
            if (gGameEffects.nGBType == 0) {
                EVENT_Trigger(gGameEffects.nPlayer, 0x3D, 0, -1);
                return;
            }
            EVENT_Trigger(gGameEffects.nPlayer, 0x3F, 0, -1);
            return;
        }
        gGameEffects.bPaused = 1;
        if (gGameEffects.nGBType == 0) {
            EVENT_Trigger(gGameEffects.nPlayer, 0x3E, 0, -1);
            return;
        }
        EVENT_Trigger(gGameEffects.nPlayer, 0x40, 0, -1);
    }
}

// The letterbox's height as a fraction of the screen: 0 with no GameBreaker, else growing to 0.15
// over its first 0.8 s (and shrinking as it closes). The boost meter rises with it
// (UI_Obj_RenderBoostUI).
f32 GameEffects_GetLetterboxHeight(void) {
    if (gGameEffects.bGameBreaker) {
        if (gGameEffects.fGBTime < 0.8f) {
            return 0.15f * (gGameEffects.fGBTime / 0.8f);
        }
        return 0.15f;
    }
    return 0.0f;
}
