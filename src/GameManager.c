// GameManager.c (our name; EA's file is GameMode.c: GM_vInitModuleONCE to GM_GetBonusProgress
// come in the order and with the names of TW07's GameMode.c): the round's flow - the game
// manager's setup and teardown, the start of each hole, the end of a golfer's turn, of the hole
// and of the game (payouts, scorecards, CPU concessions), strokes and penalties after a shot,
// mulligans, drops, the pre- and post-shot animations, walking to the ball, the in-the-hole
// display, the aiming buttons and the profile's completion score. The eight functions before
// GM_vInitModuleONCE and the four after GM_GetBonusProgress include TW07 header inlines
// (GameEffects.h, GameModeCore.h, GameUI.h), most likely header functions kept out of line here;
// the GameEffects ones serve only GameEffects.c.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

void  GM_Earnings_FreeStreamMemory(void);
void  GM_ClearHoleBonusStats(int nPlayer);
u8    GM_IsRoundForcedOver(int nPlayer);
void  GM_Earnings_PayRoundGoals(int nPlayer, int a);
void  GM_RecordIndividualRoundStats(int nPlayer);
void  GM_PgaTourSim_SetUserEntrantHoleStrokes(int nPlayer, int nStrokes);
void  GM_GolferConcede_Hole(int nPlayer);
void  GM_EndOfGolferTurn_HoleFinished(int nPlayer);
void  GM_EndOfGolferTurn_GameFinished(int nPlayer);
void  GM_HoleFinished_GameNotFinished(int nPlayer);
u8    GM_CheckForAIConcede(int nPlayer);

void  GM_RecordIndividualShotStats(int nPlayer);
void  GM_Earnings_PayShotGoals(int nPlayer);
void  GM_RecordIndividualHoleStats(int nPlayer);
void  GM_Earnings_PayHoledGoals(int nPlayer);
void  GM_CheckBallForUIHints(int nPlayer);
u8    OnlineGolf_bIsOnlineGame(void);
void  GM_RecordBonusShotStats(int nPlayer);
void  fn_800BB0A8(void);
void  REPLAY_Restore(int nPlayer);

u8    GM_bIsZoomButtonPressed(int nPlayer);
u8    GM_bIsElevatorCamButtonPressed(int nPlayer);
u8    GM_bIsAltSwingButtonPressed(int nPlayer);
u8    GM_bIsMidholeFlybyButtonPressed(int nPlayer);

u8    fn_800BB1F8(int nPlayer);

int   GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);
f32   GM_GetBonusProgress(SaveProfile* pProfile);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802845C0), before every constant the functions below use first (only the progress
// counters near the end load it); its body is unknown.
static f32 GameManager_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sends UI message 50 (no values). Its only caller is GameEffects_ResetGameEffectSettings, when it
// switches off a GameBreaker that is still up; nothing else sends message 50.
void GameEffects_SendMessage50(void) {
    GameMsg_Send(50);
}

// Empty in this build. The time-rate code (GameEffects_AdjustTimeRate) calls it after it has used a pending
// one-frame fixed step (GameEffects_IsSingleStepPending), so it would clear that request.
void GameEffects_ClearSingleStep(void) {
}

// Always 0 in this build. When true, the time-rate code (GameEffects_AdjustTimeRate) makes this
// frame exactly one 60 Hz tick (FRAME_TIME, unless the frame time is 0) and clears the request
// (GameEffects_ClearSingleStep).
u8 GameEffects_IsSingleStepPending(void) {
    return 0;
}

// Always 0 in this build. When true, the time-rate code (GameEffects_AdjustTimeRate) counts every
// frame with a nonzero frame time as exactly one 60 Hz tick (FRAME_TIME).
u8 GameEffects_IsFixedTimeStepOn(void) {
    return 0;
}

// Whether holing this ball would put the player in the lead: the mode's own answer (its
// pfnIsPuttForLead; GameModeDriverPGATour_IsPuttForLead on the PGA TOUR). The scripted GameBreaker
// test asks it for a ball on the green.
u8 GM_IsPuttForLead(int nPlayer) {
    return gpGame->pfnIsPuttForLead(nPlayer);
}

// Whether the ball moves on this frame of the half-time slow motion: yes on every n2C-th frame of
// it (the count n28), and on every frame when n2C is 0. GameEffects_BallUpdatesThisFrame asks it.
u8 GameEffects_StartOfSlowMoFrame(void) {
    if (gGameEffects.n2C == 0) {
        return 1;
    }
    return (gGameEffects.n28 % gGameEffects.n2C) == 0;
}

// Whether half-time slow motion is on (GameEffects.bHalfTime: the time step is halved and the slow
// frames are counted). Nothing in this build turns it on.
u8 GameEffects_IsHalfTimeOn(void) {
    return gGameEffects.bHalfTime;
}

// Three floats: pOut gets pA minus pB. GameEffects' own copy of the helper (GameRound.c has
// GM_Vec3Sub); only GameEffects calls it.
#ifdef __MWERKS__
asm void GameEffects_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
#else
// port: untested, the plain-C version for compilers without paired singles.
void GameEffects_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// Once at boot (gomainloop's once-only init list): the split-screen choice (lbl_8028227C) goes to
// one screen and the player up (lbl_80282278) to player 0.
void GM_vInitModuleONCE(void) {
    lbl_8028227C = 0;
    lbl_80282278 = 0;
}

// Empty (as in TW07): the first call of gomainloop's once-only close list.
void GM_vCloseModuleONCE(void) {
}

// A round starts (GO_vInitIG, before the course data and cameras are set up): outside a GameMode5
// challenge the game's data is cleared and the round goes to its first selected hole; then the
// mode's pfnStartGamePreData hook, and the playoff hole list is marked to be built.
void GM_InitModule_PreDataStream(void) {
    if (PlayNow_IsChallengeRunning() == 0) {
        GM_ClearDataForNewGame();
        GM_InitializeCurrentHoleToFirstSelected();
    }
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1EC))(gpGame);
    GM_SetNeedToBuildPlayoffHoleList(1);
}

// Late in a round's start (GO_vInitIG, after the players are set up): calls the mode's
// pfnStartGamePostData hook (GameModeBattle saves the golfers' bags there). TW07 has
// GM_CheckForAndReturnWager next; this build does not.
void GM_InitModule_PostDataStream(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1F0))(gpGame);
}

// A round is torn down (gomainloop): the mode's Shutdown (pfnShutdown) and the HUD (GUI_DeInit),
// then four frees that are empty in this build: the PGA TOUR one
// (GameModeDriverPGATour_FreeStreamMemory), the GameMode5 one (PlayNow_DeInit), the earnings' stream
// memory (GM_Earnings_FreeStreamMemory) and the course data (fn_800D29E8).
void GM_DeInitModule(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1CC))(gpGame);
    GUI_DeInit();
    GameModeDriverPGATour_FreeStreamMemory();
    PlayNow_DeInit();
    GM_Earnings_FreeStreamMemory();
    fn_800D29E8();
}

// Moves the round to its next selected hole and asks for it to be loaded (fn_8006F4B4); 1 when
// there was one, 0 after the last. Crossing from the front nine to the back nine under the
// one-mulligan-per-nine rule (2) gives the mulligans back. The pause menu calls it after the
// end-of-hole scorecard.
int GM_GotoNextSelectedHole(void) {
    int i;
    for (i = gpGame->nCurHole + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (Game_GetMulliganRule() == 2 && gpGame->nCurHole < 9 && i >= 9) {
                GM_ClearMulliganCounters();
            }
            GM_SetCurrentHole(i);
            fn_8006F4B4();
            return 1;
        }
    }
    return 0;
}

// A new hole: views 2 and 3 off, the balls to the tee, new wind, the mode's hole-start hook
// (pfnLoadHole), effects and HUD reset, the hole contests cleared, the flyover when the mode has
// one (b27F), each player's flags that GM_RecordBonusShotStats carries from shot to shot cleared,
// the player up set to the mode's first (GetHonors), event 0 (player 0xFF); then for all five
// players the button hold counters (n144, n158) and the shot-limit and mulligan-this-hole flags
// (bShotLimitExceeded, bUsedMulliganThisHole) cleared.
void GM_InitForHole(void) {
    int i;
    ViewController_TurnOnViewController(2, 0);
    ViewController_TurnOnViewController(3, 0);
    GM_InitBallsToTee();
    Wind_Generate();
    gpGame->pfnLoadHole();
    GameEffects_ResetGameEffectSettings();
    GUI_Init();
    HoleContest_InitForHole();
    if (gpGame->b27F) {
        GM_FlyByMode_Init();
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        GM_ClearHoleBonusStats(i);
    }
    gpGame->b134 = 0;
    lbl_80282278 = gpGame->pfnGetHonors(5);
    EVENT_Trigger(0xFF, 0, 0, -1);
    for (i = 0; i < 5; i++) {
        int j;              // fake match: j only steers the register choice (found by the permuter)
        gpGame->n144[i] = 0;
        j = i;
        gpGame->n158[j] = 0;
        i = j;
        gPlayers[i].bUsedMulliganThisHole = 0;
        gPlayers[j].bShotLimitExceeded = 0;
    }
}

// The end of a golfer's turn: the caddie stops, the mode is told (pfnEndGolferTurn), the golfer's
// character sleeps, event 4, the button prompts hidden. When the mode says the hole is finished (or
// GM_IsRoundForcedOver) it goes on to GM_EndOfGolferTurn_HoleFinished. Otherwise the golfer waits
// (GS_WAIT): in speed golf (modes 6-8) only once holed, in mode 26 always, and when the mode lets
// CPUs concede a CPU that GM_CheckForAIConcede picks concedes the hole instead.
void GM_EndOfGolferTurn(int nPlayer) {
    u8 bWait;
    Caddie_Stop();
    gpGame->pfnEndGolferTurn(nPlayer);
    Character_Sleep(gPlayers[nPlayer].pChar);
    EVENT_Trigger(nPlayer, 4, 0, -1);
    GUI_HideAllHelpTips();
    if (gpGame->pfnHoleFinished(nPlayer, 0) || GM_IsRoundForcedOver(nPlayer)) {
        GM_EndOfGolferTurn_HoleFinished(nPlayer);
        return;
    }
    bWait = 0;
    if (GM_IsSpeedGolfMode()) {
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

// The hole is over: event 1 and the mode's EndHole. On the round's last hole, outside a playoff
// (bInPlayoff), each player's round goals are paid out (GM_Earnings_PayRoundGoals) and the round
// counted in the profile (GM_RecordIndividualRoundStats). Then GM_EndOfGolferTurn_GameFinished when
// the mode says the game is over (or GM_IsRoundForcedOver), else GM_HoleFinished_GameNotFinished.
void GM_EndOfGolferTurn_HoleFinished(int nPlayer) {
    int i;
    EVENT_Trigger(nPlayer, 1, 0, -1);
    gpGame->pfnEndHole();
    if (GM_CurrentlyOnLastHole() && !gpGame->bInPlayoff && !GM_IsRoundForcedOver(nPlayer)) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GM_Earnings_PayRoundGoals(i, 0);
            GM_RecordIndividualRoundStats(i);
        }
    }
    if (gpGame->pfnGameFinished(0) || GM_IsRoundForcedOver(nPlayer)) {
        GM_EndOfGolferTurn_GameFinished(nPlayer);
        return;
    }
    GM_HoleFinished_GameNotFinished(nPlayer);
}

// The game is over: event 5, the game marked finished (b28E), the won flag cleared before the
// mode's EndGame decides it, and a win counted in EASBio outside the demo (gSession.bDemo). With
// b273 every player gets the end-of-round payout (GM_Earnings_PayRoundGoals, bRoundOver 1). Outside
// the demo and while the round is not already ending (gSession.bEndLoop), the end-of-game scorecard
// is shown when the mode has scorecards (b275; bHuman 1 unless b274 is set outside a GameMode5
// challenge), the golfer waits and the view goes to camera mode 17; otherwise gSession.bEndLoop is
// set, which ends the round.
void GM_EndOfGolferTurn_GameFinished(int nPlayer) {
    int i;
    int nView;
    EVENT_Trigger(nPlayer, 5, 0, -1);
    gpGame->b28E = 1;
    EASBio_SetCurrentGameWon(0);
    gpGame->pfnEndGame();
    if (EASBio_IsCurrentGameWon() && gSession.bDemo == 0) {
        EASBio_IncrementGamesWon(1);
    }
    if (gpGame->b273) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GM_Earnings_PayRoundGoals(i, 1);
        }
    }
    if (gSession.bEndLoop == 0 && gSession.bDemo == 0) {
        if (gpGame->b275) {
            if (!gpGame->b274 || PlayNow_IsChallengeRunning()) {
                GUI_EndOfGameScorecard(1);
            } else {
                GUI_EndOfGameScorecard(0);
            }
        }
        GOLFERSTATE_Set(GS_WAIT, nPlayer);
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 0x11, nPlayer, nView);
        return;
    }
    gSession.bEndLoop = 1;
}

// The hole is over and the game goes on: the mode's pfnEndTurnEndHoleNotGame hook, the
// between-holes scorecard when the mode has scorecards (b275) and none is up (bHuman as in
// GM_EndOfGolferTurn_GameFinished), the golfer waits, the view goes to camera mode 17 and the HUD's
// toggles are hidden.
void GM_HoleFinished_GameNotFinished(int nPlayer) {
    int nView;
    gpGame->pfnEndTurnEndHoleNotGame(nPlayer);
    if (gpGame->b275 && !GUI_ScoreCardUp()) {
        if (!gpGame->b274 || PlayNow_IsChallengeRunning()) {
            GUI_BetweenHolesScorecard(1);
        } else {
            GUI_BetweenHolesScorecard(0);
        }
    }
    GOLFERSTATE_Set(GS_WAIT, nPlayer);
    nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 0x11, nPlayer, nView);
    GUI_HideAllToggleUI();
}

// Whether a CPU golfer concedes the hole: never once holed; yes after three or more penalty shots
// in a row (nOBCount above 2); otherwise yes when one of the players before it in the order is on
// the green while it is not and it has already taken more than 3 strokes more than them on this
// hole.
u8 GM_CheckForAIConcede(int nPlayer) {
    u8  bConcede = 0;
    int i;
    if (Player_IsCPU(nPlayer)) {
        if (gPlayers[nPlayer].ball.nLie != LIE_INCUP_e) {
            if (gPlayers[nPlayer].nOBCount > 2) {
                bConcede = 1;
            } else {
                for (i = 0; i < gSession.nNumPlayers; i++) {
                    if (i == nPlayer) break;
                    if (gPlayers[(u32)i].ball.nLie == LIE_GREEN_e &&
                        gPlayers[nPlayer].ball.nLie != LIE_GREEN_e &&
                        gPlayers[nPlayer].nStrokes[gpGame->nCurHole] >
                            gPlayers[(u32)i].nStrokes[gpGame->nCurHole] + 3) {
                        bConcede = 1;
                    }
                }
            }
        }
    }
    return bConcede;
}

// The ball has just been struck (the swing state calls it): the penalty mark is cleared, UI message
// 0x4E goes out with the player, and the player's HUD is hidden when the mode says so (b271).
void GM_BallHit(int nPlayer) {
    gPlayers[nPlayer].bPenaltyShot = 0;
    GameMsg_SendInt(0x4E, nPlayer);
    if (gpGame->b271) {
        GUI_ToggleUI(nPlayer, 0);
    }
}

// One more stroke on this hole, and one more putt if it was the putter; during a PGA TOUR event
// (GM_Currently_PgaTourMode) the tour's scoreboard gets the new count.
void GM_PlayerAddStroke(int nPlayer) {
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        gPlayers[nPlayer].nPutts[gpGame->nCurHole]++;
    }
    if (GM_Currently_PgaTourMode()) {
        GM_PgaTourSim_SetUserEntrantHoleStrokes(nPlayer, gPlayers[nPlayer].nStrokes[gpGame->nCurHole]);
    }
}

// After a shot: out of bounds (GM_IsBallOOB, or no surface under the ball) or a drop (a surface
// without u34 bit 0, or any surface inside the free-drop network) stops the ball, and the camera
// gets shot kind 6. Out of bounds, or a drop on a surface with u34 bit 1, costs a stroke: the
// penalty mark (bPenaltyShot) is set, nOBCount counts penalties in a row (a CPU gets 25 points of
// attributes per level, and concedes after three), the stroke is added (and reported to a PGA TOUR
// event) and message 0xD (water, surface class 7/16) or 2 is shown; 1 is returned, but 0 once the
// mode's 10-stroke limit is reached (modes 7 and 8 return 1 straight after the stroke). A free drop
// returns 0; a shot with neither resets nOBCount and returns 0.
u8 GM_CheckForBallOOB(int nPlayer) {
    Ball*        pBall = &gPlayers[nPlayer].ball;
    u8           bOut  = GM_IsBallOOB(nPlayer, pBall);
    SurfaceType* pSurf = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, pBall->vPos);
    u8           bDrop;

    if (pSurf == NULL) {
        bOut  = 1;
        bDrop = 0;
    } else if ((pSurf->u34 & 1) && !Ter_PointInFreeDropNetwork(pBall->vPos)) {
        bDrop = 0;
    } else {
        bDrop = 1;
    }
    gPlayers[nPlayer].bPenaltyShot = 0;
    if (bOut || bDrop) {
        gPlayers[nPlayer].ball.nState = 0;
        if (bOut || (bDrop && (pSurf->u34 & 2))) {
            gPlayers[nPlayer].bPenaltyShot = 1;
            gPlayers[nPlayer].nOBCount++;
            gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
            if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
                gPlayers[nPlayer].nPutts[gpGame->nCurHole]++;
            }
            if (GM_Currently_PgaTourMode()) {
                GM_PgaTourSim_SetUserEntrantHoleStrokes(nPlayer, gPlayers[nPlayer].nStrokes[gpGame->nCurHole]);
            }
            if (Game_GetMode() == 8 || Game_GetMode() == 7) {
                return 1;
            }
            if (gpGame->bStrokeLimit && gPlayers[nPlayer].nStrokes[gpGame->nCurHole] >= 10) {
                if (GM_Currently_PgaTourMode()) {
                    GM_PgaTourSim_SetUserEntrantHoleStrokes(nPlayer, 10);
                }
                return 0;
            }
            if (pSurf != NULL && (pSurf->nClass == 7 || pSurf->nClass == 16)) {
                GUI_StartPostShotUI(0xD, nPlayer, 0.0f);
            } else {
                GUI_StartPostShotUI(2, nPlayer, 0.0f);
            }
            fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 6, nPlayer);
            return 1;
        }
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 6, nPlayer);
        return 0;
    }
    gPlayers[nPlayer].nOBCount = 0;
    return 0;
}

// The score message after holing out: a hole in one (4), or strokes against par from albatross (-3,
// message 5) to triple bogey (+3, message 11), else message 12 with the difference.
void GM_CheckBallForUIHints(int nPlayer) {
    int nDiff;
    if (gPlayers[nPlayer].nStrokes[gpGame->nCurHole] == 1) {
        GUI_StartPostShotUI(4, nPlayer, 0.0f);
        return;
    }
    nDiff = gPlayers[nPlayer].nStrokes[gpGame->nCurHole] - Course_GetCurHolePar();
    switch (nDiff) {
    case -3: GUI_StartPostShotUI(5, nPlayer, 0.0f); break;
    case -2: GUI_StartPostShotUI(6, nPlayer, 0.0f); break;
    case -1: GUI_StartPostShotUI(7, nPlayer, 0.0f); break;
    case 0:  GUI_StartPostShotUI(8, nPlayer, 0.0f); break;
    case 1:  GUI_StartPostShotUI(9, nPlayer, 0.0f); break;
    case 2:  GUI_StartPostShotUI(10, nPlayer, 0.0f); break;
    case 3:  GUI_StartPostShotUI(11, nPlayer, 0.0f); break;
    default: GUI_StartPostShotUI(12, nPlayer, nDiff); break;
    }
}

// With the mode's yardage display on (bShowYardage): post-shot message 1 with how far the ball
// went, measured flat from where the player stood to address it (vBall).
void GM_ShowYardage(int nPlayer) {
    if (gpGame->bShowYardage) {
        Player* p = &gPlayers[nPlayer];
        f32     dx;
        f32     dz;
        dx = p->ball.vPos[0] - p->vBall[0];
        dz = p->ball.vPos[2] - p->vBall[2];
        GUI_StartPostShotUI(1, nPlayer, Math_Sqrt(dx * dx + dz * dz));
    }
}

// With the mode's obstruction relief on (bBumpObstructions), a ball off the tee that rests against
// an object or hazard (within 1.5, Ter_CheckObjectAndHazardObstruction) is dropped at a legal point
// nearby, or else put back where it was before the shot (vPreShot) and, when vA44 matches vBall in
// x and z, set up there again (Physics_InitBall).
void GM_BumpBallForObstructions(int nPlayer) {
    int n;                      // fake match: a copy of nPlayer for the register order (permuter)
    f32 vDrop[4];
    if (gpGame->bBumpObstructions) {
        Player* p;
        Ball*   pBall;
        p = &gPlayers[nPlayer];
        n = nPlayer;
        if (p->ball.nLie != 0) {
            pBall = &p->ball;
            if (Ter_CheckObjectAndHazardObstruction(pBall->vPos, 1.5f, 0, 1, 2.0f, 1, 0.577f)) {
                if (Ter_SearchForDropLocation(n, 0, 0, vDrop)) {
                    Physics_DropBall(pBall, vDrop);
                    return;
                }
                Physics_DropBall(pBall, gPlayers[nPlayer].vPreShot);
                if (gPlayers[n].vA44[0] == gPlayers[n].vBall[0] &&
                    gPlayers[n].vA44[2] == gPlayers[n].vBall[2]) {
                    Physics_InitBall(pBall, gPlayers[nPlayer].vPreShot, n);
                }
            }
        }
    }
}

// Once the ball has stopped: the mulligan prompt (GUI_ToggleMulligan) and, for a human on one
// screen with a replay recorded and allowed, the replay prompt (GUI_ToggleReplay); the stroke, the
// penalty check (GM_CheckForBallOOB), the shot statistics and earnings when there was no penalty,
// and the hole contests (longest drive message 10, closest to the pin 11). Without a penalty: holed
// - the hole recorded, its payouts, the score message and the mode's pfnHoledOut; over the hole's
// stroke limit - the ball is picked up (lie holed, bShotLimitExceeded, 10 strokes in a PGA TOUR
// event else 11, putts 999), message 3, the mode's pfnHoledOut and pfnShotOverLimit; messages
// 0x11-0x13 when OnlineGolf_bIsOnlineGame (always 0) and bEE0 allow, 0x13 giving the other of
// players 0 and 1 the hole; otherwise the yardage. A penalty goes to the mode's pfnBallOOB. Last,
// the per-shot flags are carried over (GM_RecordBonusShotStats).
void GM_PlayerTookShot(int nPlayer) {
    u8   bOut;
    if (GM_CanPlayerTakeMulligan(nPlayer) && !(gPlayers[nPlayer].uFlags & 8)) {
        GUI_ToggleMulligan(1);
    }
    if (!Player_IsCPU(nPlayer) && gSession.nSplitScreen == 0 && gpGame->b287 && gReplayData.bF10) {
        GUI_ToggleReplay(1);
    }
    GM_PlayerAddStroke(nPlayer);
    bOut = GM_CheckForBallOOB(nPlayer);
    if (!bOut) {
        GM_RecordIndividualShotStats(nPlayer);
        GM_Earnings_PayShotGoals(nPlayer);
    }
    HoleContest_PlayerTookShot(nPlayer);
    if (HoleContest_IsReadyToDecide()) {
        HoleContest_PayWinner();
        if (HoleContest_IsLongestDriveHole()) {
            GUI_QueueMessage(1, 10, 0, 0);
        }
        if (HoleContest_IsClosestToPinHole()) {
            GUI_QueueMessage(1, 11, 0, 0);
        }
    }
    if (!bOut) {
        if (GM_CheckForBallInHole(nPlayer)) {
            GM_RecordIndividualHoleStats(nPlayer);
            GM_Earnings_PayHoledGoals(nPlayer);
            GM_CheckBallForUIHints(nPlayer);
            gpGame->pfnHoledOut(nPlayer);
        } else {
            if (GM_IsShotOverLimit(nPlayer, gPlayers[nPlayer].nStrokes[gpGame->nCurHole])) {
                gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
                gPlayers[nPlayer].bShotLimitExceeded = 1;
                if (GM_Currently_PgaTourMode()) {
                    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 10;
                } else {
                    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 11;
                }
                gPlayers[nPlayer].nPutts[gpGame->nCurHole] = 999;
                GM_RecordIndividualHoleStats(nPlayer);
                GUI_StartPostShotUI(3, nPlayer, 0.0f);
                gpGame->pfnHoledOut(nPlayer);
                gpGame->pfnShotOverLimit(nPlayer);
            } else if (OnlineGolf_bIsOnlineGame() && gPlayers[nPlayer].bEE0) {
                int n = gPlayers[nPlayer].nEE4;
                if (n == 3) {
                    GUI_StartPostShotUI(0x13, nPlayer, n);
                    if (nPlayer == 0) {
                        gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
                        gPlayers[1].nHolesWon++;
                    } else {
                        gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
                        gPlayers[0].nHolesWon++;
                    }
                } else if (n == 2) {
                    GUI_StartPostShotUI(0x12, nPlayer, n);
                } else {
                    GUI_StartPostShotUI(0x11, nPlayer, n);
                }
            } else {
                GM_ShowYardage(nPlayer);
            }
        }
    } else {
        gpGame->pfnBallOOB(nPlayer);
    }
    GM_RecordBonusShotStats(nPlayer);
}

// Takes a mulligan; 1 when taken. Refused when the mode has no mulligans, the golfer has conceded,
// or the UI flag GUI_IsAwardUIAnimating reads is set; under the one-per-nine rule (2) a player who has used
// theirs is refused, else it is marked used (bMulliganUsed). Then the crowd and commentary stop,
// the ball and player go back to before the shot (REPLAY_Restore), the mulligan flags
// (bUsedMulligan, bUsedMulliganThisHole) are set, the mode is told (pfnMulligan), a playing replay
// stops, the camera and the golfer's animation are reset, the golfer goes back to the Swing state
// with the HUD on, and the flag is taken out on the view unless another player on that view (not
// cut) still lies off the green.
u8 GM_PlayerTakeMulligan(int nPlayer) {
    int i;
    if (Game_GetMulliganRule() == 0) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == GS_CONCEDED) {
        return 0;
    }
    if (GUI_IsAwardUIAnimating()) {
        return 0;
    }
    if (Game_GetMulliganRule() == 2) {
        if (gPlayers[nPlayer].bMulliganUsed) {
            return 0;
        }
        gPlayers[nPlayer].bMulliganUsed = 1;
    }
    fn_800BB0A8();
    GUI_HideAllHelpTips();
    fn_800335F8(1);
    Gaud_StopComment();
    REPLAY_Restore(nPlayer);
    gPlayers[nPlayer].bUsedMulligan = 1;
    gPlayers[nPlayer].bUsedMulliganThisHole = 1;
    gpGame->pfnMulligan(nPlayer);
    if (gSession.bReplay) {
        REPLAY_Stop();
    }
    fn_800C70F8(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    CharacterState_ResetMorphState(gPlayers[nPlayer].pChar, 1);
    GOLFERSTATE_Switch(GS_SWING, nPlayer);
    GUI_ToggleUI(nPlayer, 1);
    ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (gPlayers[i].bPlayerCut == 0 && gPlayers[i].nView[0] == gPlayers[nPlayer].nView[0] &&
            gPlayers[i].ball.nLie != 10 && gPlayers[i].ball.nLie != LIE_GREEN_e &&
            gPlayers[i].ball.nLie != LIE_INCUP_e) {
            ViewController_GetIndexedViewController(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
        }
    }
    return 1;
}

// Whether the golfer plays his pre-shot animation. With session flags 0x4000 and 0x8000 both set,
// only off the tee. The mode's setting n290: 0 never; on course 18's 10th hole never within 40
// yards of the tee; 1 always; otherwise always off the tee, never from elsewhere with a driver or
// wood (clubs 0-8), never with an object or hazard nearby, else 85% of the time.
int GM_DoPreshotAnimation(int nPlayer) {
    f32 v[4];
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        return gPlayers[nPlayer].ball.nLie == 0;
    }
    if (gpGame->n290 == 0) {
        return 0;
    }
    if (Game_GetCourse() == 0x12 && Game_GetCurHoleNum() == 10) {
        GM_Vec4Sub(gPlayers[nPlayer].vBall, &Ter_GetTGD()->tee[gSession.nTeeSet[nPlayer]].x, v);
        v[1] = 0.0f;
        if ((f32)Math_Sqrt(Vec3_LengthSqClamped(v)) < 40.0f) {
            return 0;
        }
    }
    if (gpGame->n290 == 1) {
        return 1;
    }
    if (gPlayers[nPlayer].ball.nLie == 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ball.nLie != 0 && gPlayers[nPlayer].nClub < 9) {
        return 0;
    }
    if (Ter_CheckObjectAndHazardObstruction(gPlayers[nPlayer].ball.vPos, lbl_80281F78->f16C, 0, 1, 4.0f, 1,
                                            0.577f)) {
        return 0;
    }
    return (Misc_RandFunc(1) % 100) < 85;
}

// Whether the golfer plays a reaction after the shot. Never when GM_IsValidPostShotGameType says
// no, on course 18's 10th within 40 yards of the tee, or for character group 11 when the ball
// started on surface 45. Where the animation would leave the golfer: never with no ground there,
// out of bounds, on a surface without u34 bit 0, in water (class 7/16), or on a slope steeper than
// 0.1 with the ground more than 0.2 below the ball. With session flags 0x4000 and 0x8000 both set:
// only when holed, from surface 16, or for outcomes 1 and 2. Then: with bPlanReady only for outcome
// 2; always when fn_8004560C says so or animation 9 is playing; with a scripted reaction (uFlags
// bit 0) when the character has one (p1790). Otherwise by the outcome (fn_8006AA9C, 0..4): after a
// putt 80%, always, always, 70%, 90%, else 50%; after other shots 35%, always, always, 70%, 90%,
// else 50%.
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
    f32          fSlope;
    f32          fRise;

    nResult = fn_8006AA9C(nPlayer);
    if (!GM_IsValidPostShotGameType()) {
        return 0;
    }
    if (Game_GetCourse() == 0x12 && Game_GetCurHoleNum() == 10) {
        GM_Vec4Sub(gPlayers[nPlayer].vBall, &Ter_GetTGD()->tee[gSession.nTeeSet[nPlayer]].x, vTee);
        vTee[1] = 0.0f;
        if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vTee)) < 40.0f) {
            return 0;
        }
    }
    if (gPlayers[nPlayer].pChar->nGroup == 11 &&
        gPlayers[nPlayer].ball.nStartSurface == 0x2D) {
        return 0;
    }
    pCourse = Ter_GetTGD();
    if (pCourse) {
        Character_GetEndOfAnimationPosition(gPlayers[nPlayer].pChar, vPos);
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
            if (fabsf(fHighA - gPlayers[nPlayer].vBall[1]) < fabsf(fHighB - gPlayers[nPlayer].vBall[1])) {
                fHigh = fHighA;
                pSurf = pSurfA;
            } else {
                fHigh = fHighB;
                pSurf = pSurfB;
            }
        }
        GM_Vec3Sub(vPos, gPlayers[nPlayer].vBall, vFlat);
        vFlat[1] = 0.0f;
        fLen  = Math_Sqrt(Vec3_LengthSqClamped(vFlat));
        fRise = gPlayers[nPlayer].vBall[1] - fHigh;
        if (0.0f != fLen) {
            fSlope = fRise / fLen;
        } else {
            fSlope = 0.0f;
        }
        if (!Ter_PointInOOBNetwork(vPos) || (pSurf != NULL && !(pSurf->u34 & 1)) || pSurf->nClass == 7 ||
            pSurf->nClass == 16 || (fabsf(fSlope) > 0.1f && fRise > 0.2f)) {
            return 0;
        }
    }
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e || gPlayers[nPlayer].ball.nStartSurface == 16 ||
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
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9 || fn_80095798(gPlayers[nPlayer].pChar) == 9) {
        return 1;
    }
    if (gPlayers[nPlayer].uFlags & 1) {
        return gPlayers[nPlayer].pChar->pReactionClip != NULL;
    }
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
        switch (nResult) {
        case 0:  return Misc_RandFunc(1) % 100 < 80;
        case 1:  return Misc_RandFunc(1) % 100 < 100;
        case 2:  return 1;
        case 3:  return Misc_RandFunc(1) % 100 < 70;
        case 4:  return Misc_RandFunc(1) % 100 < 90;
        default: return Misc_RandFunc(1) % 100 < 50;
        }
    } else {
        switch (fn_8006AA9C(nPlayer)) {
        case 0:  return Misc_RandFunc(1) % 100 < 35;
        case 1:  return Misc_RandFunc(1) % 100 < 100;
        case 2:  return 1;
        case 3:  return Misc_RandFunc(1) % 100 < 70;
        case 4:  return Misc_RandFunc(1) % 100 < 90;
        default: return Misc_RandFunc(1) % 100 < 50;
        }
    }
}

// Whether the post-shot camera cuts to the crowd flyby: the crowd animation's countdown is at least
// 5 and its delayed-start percentage at least 0.5.
u8 GM_ShowPostShotCrowdFlyby(void) {
    if (fn_800336E4() >= 5.0f && fn_800336F4() >= 0.5f) {
        return 1;
    }
    return 0;
}

// Starts the hole's flyover: the mode's first player (GetHonors with 5, nobody before) is pushed
// into the flyover state (GS_INITIAL_FLY_BY) and becomes the active player of their view.
void GM_FlyByMode_Init(void) {
    int n = gpGame->pfnGetHonors(5);
    GOLFERSTATE_Push(GS_INITIAL_FLY_BY, n);
    ViewController_SetActivePlayerNumber(gPlayers[n].nView[0], n);
}

// Every frame of a round (game type 6): the mode's pfnUpdate; then queued UI messages are shown
// when one is waiting or the pause menu is open with flag GUI_IsEndGameUiShowing set, else, when
// the mode says so (b27E) and no scorecard is up, the next golfer is set up once everyone waits
// (GM_SetupGolfer_IfAllWaiting). The frame count is kept in n12C.
void GM_Update(void) {
    if (gSession.nGameType == 6) {
        gpGame->pfnUpdate();
        if ((GUI_IsEndGameUiShowing() && GUI_IsPauseMenuOpen()) || GUI_GetUIMessageQued()) {
            GUI_CheckMessageQue();
        } else if (gpGame->b27E && !GUI_ScoreCardUp()) {
            GM_SetupGolfer_IfAllWaiting();
        }
        gpGame->n12C = gSession.nFrameCount;
    }
}

// Restarts the current hole, when the mode allows it (b279): all five players' records of the hole
// cleared, the balls to the tee, the mode's pfnRestartHole, the flyover again when the mode has one
// (b27F), the frame count in n12C, the HUD re-initialised, effects and cameras reset, each golfer's
// animation and green morph reset, and message helper GameMsg_SetPending(2).
void GM_RestartHole(void) {
    int i;
    if (gpGame->b279) {
        for (i = 0; i < 5; i++) {
            GM_ClearPlayerHoleData(i, gpGame->nCurHole);
        }
        GM_InitBallsToTee();
        gpGame->pfnRestartHole();
        if (gpGame->b27F) {
            GM_FlyByMode_Init();
        }
        gpGame->n12C = gSession.nFrameCount;
        GUI_HideAllToggleUI();
        GUI_Init();
        GameEffects_ResetGameEffectSettings();
        fn_800C6C8C();
        for (i = 0; i < gSession.nNumPlayers; i++) {
            fn_800957D8(PLAYER(i)->pChar);
            CharacterState_ResetMorphState(PLAYER(i)->pChar, 1);
        }
        GameMsg_SetPending(2);
    }
}

// The player's ball's distance from the pin (fn_800D0478's).
f32 GM_GetGolferDistanceToPin(int nPlayer) {
    return fn_800D0478(nPlayer);
}

// A ball that must be dropped (b30E), or one in bounds (Ter_PointInOOBNetwork) after a shot with no
// penalty, is dropped at a legal point nearby when there is one. Otherwise (out of bounds, or after
// a penalty) it goes back where it was before the shot (vPreShot) and, when vA44 matches vBall in x
// and z, is set up there again (Physics_InitBall).
void GM_ReplaceOOBBall(int nPlayer) {
    f32   v[4];
    f32*  pPre;
    Ball* pBall;
    if ((gPlayers[nPlayer].b30E ||
         (Ter_PointInOOBNetwork(gPlayers[nPlayer].ball.vPos) && !gPlayers[nPlayer].bPenaltyShot)) &&
        Ter_SearchForDropLocation(nPlayer, 1, 1, v)) {
        Physics_DropBall(&gPlayers[nPlayer].ball, v);
        return;
    }
    pBall = &gPlayers[nPlayer].ball;
    pPre  = gPlayers[nPlayer].vPreShot;
    Physics_DropBall(pBall, pPre);
    if (gPlayers[nPlayer].vA44[0] == gPlayers[nPlayer].vBall[0] &&
        gPlayers[nPlayer].vA44[2] == gPlayers[nPlayer].vBall[2]) {
        Physics_InitBall(pBall, pPre, nPlayer);
    }
}

// A ball in a lateral water hazard (GameMode8 calls it when the penalty was water, surface class
// 7): dropped at a legal point nearby when there is one, otherwise put back where it was before the
// shot and, when vA44 matches vBall in x and z, set up there again. GM_ReplaceOOBBall without its
// in-bounds test.
void GM_ReplaceLateralHazardBall(int nPlayer) {
    f32   v[4];
    f32*  pPre;
    Ball* pBall;
    if (Ter_SearchForDropLocation(nPlayer, 1, 1, v)) {
        Physics_DropBall(&gPlayers[nPlayer].ball, v);
        return;
    }
    pBall = &gPlayers[nPlayer].ball;
    pPre  = gPlayers[nPlayer].vPreShot;
    Physics_DropBall(pBall, pPre);
    if (gPlayers[nPlayer].vA44[0] == gPlayers[nPlayer].vBall[0] &&
        gPlayers[nPlayer].vA44[2] == gPlayers[nPlayer].vBall[2]) {
        Physics_InitBall(pBall, pPre, nPlayer);
    }
}

// The player concedes the hole: HUD off, lie holed, 999 strokes and putts, the ball stopped.
// Outside mode 18, when that leaves exactly one golfer on the hole, every other player is marked
// holed too (the last one finishes without putting out). Then the message helpers, the post-shot
// display request and the Conceded state.
void GM_GolferConcede_Hole(int nPlayer) {
    Player* p;
    int     i;
    int     nPlayers;
    int     n;
    GUI_ToggleUI(nPlayer, 0);
    p = &gPlayers[nPlayer];
    gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole] = 999;
    p->nPutts[gpGame->nCurHole] = 999;
    gPlayers[nPlayer].ball.nState = 0;
    if (Game_GetMode() != 0x12) {
        nPlayers = gNumPlayersSetUp;
        n = 0;
        for (i = 0; i < nPlayers; i++) {
            if (PLAYER(i)->ball.nLie != LIE_INCUP_e) {
                n++;
            }
        }
        if (n == 1) {
            for (i = 0; i < nPlayers; i++) {
                if (i != nPlayer) {
                    PLAYER(i)->ball.nLie = LIE_INCUP_e;
                    PLAYER(i)->ball.nState = 0;
                }
            }
        }
    }
    GameMsg_SetPending(1);
    GameMsg_SetPendingValue(nPlayer);
    GUI_FlagPostShotRequest();
    GOLFERSTATE_Switch(GS_CONCEDED, nPlayer);
}

// The player walks to the ball: its position (vBall) becomes the ball's, the pre-shot position
// (vPreShot) is saved, and the height comes from the ground there: the upper surface unless there
// is none or it is more than 0.25 above the ball, then the lower one, and when neither exists the
// height is kept.
void GM_MovePlayerToBall(int nPlayer) {
    f32         fLow;
    f32         fHigh;
    CourseInfo* pCourse;
    f32         f;
    Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
    LLMath_CopyVec(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vPreShot);
    pCourse = Ter_GetTGD();
    if (pCourse) {
        Ter_GetEnclosingGroundHeight(pCourse, gPlayers[nPlayer].vBall, &fLow, &fHigh);
        f = fHigh;
        if (-65536.125f == f || f > 0.25f + gPlayers[nPlayer].vBall[1]) {
            f = fLow;
            if (-65536.125f == fLow) {
                f = gPlayers[nPlayer].vBall[1];
            }
        }
        gPlayers[nPlayer].vBall[1] = f;
    }
}

// A human's buttons while setting up a shot. Outside the green morph: buttons 9, 10 and 30 fire
// events 0xD-0xF and show the HUD (in modes 22 and 26 they end the check); then the zoom camera,
// the elevator camera (modes without b28D) or, with b28D, the re-plan button 47 when the mode
// allows it (pfnPickTarget: a fresh default target and shot), the next alternate swing camera, or
// the mid-hole flyover (unless cameras are skipped). Held buttons 11-14 fire events 0x12-0x15; any
// of that, or the target's momentum moving, updates the golfer's emotion.
void GM_CheckForShotChanges(int nPlayer) {
    u8 bChanged = 0;
    if (Player_IsCPU(nPlayer)) {
        return;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_GREEN_MORPH) {
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(9, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
            GUI_ToggleUI(nPlayer, 1);
            bChanged = 1;
        } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(10, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xE, 0, -1);
            GUI_ToggleUI(nPlayer, 1);
            bChanged = 1;
        } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x1E, 0)) {
            if (Game_GetMode() == 0x1A || Game_GetMode() == 0x16) return;
            EVENT_Trigger(nPlayer, 0xF, 0, -1);
            GUI_ToggleUI(nPlayer, 1);
            bChanged = 1;
        }
        if (GM_bIsZoomButtonPressed(nPlayer)) {
            GOLFERSTATE_Push(GS_ZOOM, nPlayer);
        } else if (!gpGame->b28D && GM_bIsElevatorCamButtonPressed(nPlayer)) {
            GOLFERSTATE_Push(GS_ELEVATOR, nPlayer);
        } else if (gpGame->b28D
                   && (Input_ReadControlPad(gPlayers[nPlayer].nController)
                       & Controller_GetButtonMask(0x2F, 0))) {
            if (gpGame->pfnPickTarget(nPlayer)) {
                AI_DefaultTarget(nPlayer);
                Shot_Prepare(nPlayer, 1);
                BreakLine_Reset(gPlayers[nPlayer].nView[0]);
                fn_8009B970(gPlayers[nPlayer].nView[0]);
                Character_AlignShotWithTarget(nPlayer, 1, 1);
                fn_800957D8(gPlayers[nPlayer].pChar);
                fn_80095744(gPlayers[nPlayer].pChar, 5);
                TARGET_SetupTarget(nPlayer);
                fn_80062C38();
                GUI_ToggleUI(nPlayer, 1);
            }
        } else if (!gpGame->b28D && GM_bIsAltSwingButtonPressed(nPlayer)) {
            GolfCamera_vSwitchToNextAlternateSwingCamera(
                    ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), nPlayer);
        } else if (GM_bIsMidholeFlybyButtonPressed(nPlayer)) {
            if (gSession.options.bSkipCameras) return;
            if (OnlineGolf_bIsOnlineGame()) return;
            GOLFERSTATE_Push(GS_MID_HOLE_FLY_BY, nPlayer);
        }
    }
    if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0xB, 1)) {
        EVENT_Trigger(nPlayer, 0x12, 0, -1);
        bChanged = 1;
    } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0xC, 1)) {
        EVENT_Trigger(nPlayer, 0x13, 0, -1);
        bChanged = 1;
    }
    if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0xD, 1)) {
        EVENT_Trigger(nPlayer, 0x14, 0, -1);
        bChanged = 1;
    } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0xE, 1)) {
        EVENT_Trigger(nPlayer, 0x15, 0, -1);
        bChanged = 1;
    }
    if (TARGET_UpdateMomentums(nPlayer)) {
        bChanged = 1;
    }
    if (bChanged) {
        Emotion_UpdatePlayerEmotion(nPlayer);
    }
}

// Every frame after a holed ball (and a conceded one) until the turn ends. With uFlags bit 3 the
// turn ends once the view has faded out (colour fade held, state 4). While a UI message is queued
// nothing else happens. With no post-shot display holding the player: faded out ends the turn; not
// fading starts a fade to half black. While one holds, a human (fn_8002E8B4) on one screen may take
// a mulligan (button 25; not in a saved replay or with uFlags bit 3) or watch the replay (button
// 24, when one was recorded, the mode allows it, the hole was not conceded and the UI flag
// GUI_IsAwardUIAnimating reads is clear), and button 0 moves the display on (in split screen too); for a CPU,
// button 0 on any pad does.
void GM_DoPostShotInHoleUI(int nPlayer) {
    View* pView = ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]);
    f32   vOffset[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    if ((gPlayers[nPlayer].uFlags & 8) && fn_80063C7C(pView)) {
        GM_EndOfGolferTurn(nPlayer);
        fn_80062D0C(nPlayer);
        return;
    }
    if (GUI_CheckMessageQue()) {
        return;
    }
    if (!GUI_IsPostShotUIAnimating(nPlayer)) {
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
        CameraController_FadeOut(pView, lbl_80281F78->f170, vOffset);
        return;
    }
    if (fn_8002E8B4(nPlayer) && gSession.nSplitScreen == 0) {
        if (gSession.bReplay == 0
            && (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x19, 0)) &&
            !(gPlayers[nPlayer].uFlags & 8)) {
            if (GM_PlayerTakeMulligan(nPlayer)) {
                fn_80062D0C(nPlayer);
            }
            return;
        }
        if (gReplayData.bF10
            && (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x18, 0)) &&
            gpGame->b287 && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED
                    && !GUI_IsAwardUIAnimating() &&
            !(gPlayers[nPlayer].pChar->uCharFlags & 0x40)) {
            fn_80062D0C(nPlayer);
            REPLAY_Play(nPlayer);
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
            return;
        }
        if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0, 0))
            && !OnlineGolf_bIsOnlineGame()) {
            GUI_AdvancePostShotUI(nPlayer);
        }
    } else if (fn_8002E8B4(nPlayer)) {
        if (!OnlineGolf_bIsOnlineGame()
            && (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0, 0))) {
            GUI_AdvancePostShotUI(nPlayer);
        }
    } else if (Player_IsCPU(nPlayer) && !OnlineGolf_bIsOnlineGame()
               && Controller_AnyPadHasButtons(Controller_GetButtonMask(0, 0))) {
        GUI_AdvancePostShotUI(nPlayer);
    }
}

// Whether the golfer takes the ball out of the cup with the special animation. Never with session
// flags 0x4000 and 0x8000 both set, when GM_IsValidPostShotGameType says no, or for a ball picked
// up at the stroke limit (bShotLimitExceeded). For outcome 2 the mode is asked first
// (pfnIsPuttForWin, with the stroke taken back); when it says no, only a solved tap-in (bPlanReady)
// goes on (yes, after fn_8006AAB4 records the shot again). Then yes with bPlanReady; no while
// animation 9 plays, beyond 5 (fA64) or when the ball did not start on the green; a scripted answer
// in uFlags bit 1 (with bit 0); otherwise one time in 10 two or more under par, else one in 4.
int GM_ChooseRemoveBallState(int nPlayer) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        return 0;
    }
    if (!GM_IsValidPostShotGameType()) {
        return 0;
    }
    if (gPlayers[nPlayer].bShotLimitExceeded) {
        return 0;
    }
    if (fn_8006AA9C(nPlayer) == 2) {
        gPlayers[nPlayer].nStrokes[gpGame->nCurHole]--;
        if (!gpGame->pfnIsPuttForWin(nPlayer)) {
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
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9) {
        return 0;
    }
    if (gPlayers[nPlayer].fA64 > 5.0f) {
        return 0;
    }
    if (gPlayers[nPlayer].ball.nStartSurface < 0 || gPlayers[nPlayer].ball.nStartSurface >= 156 ||
        gSurfaceTypes[gPlayers[nPlayer].ball.nStartSurface].nClass != 3) {
        return 0;
    }
    if (gPlayers[nPlayer].uFlags & 1) {
        return (gPlayers[nPlayer].uFlags >> 1) & 1;
    }
    if (Course_GetCurHolePar() - gPlayers[nPlayer].nStrokes[gpGame->nCurHole] > 1) {
        if (Misc_RandFunc(1) % 10 == 0) {
            return 1;
        }
    } else if (!(Misc_RandFunc(1) & 3)) {
        return 1;
    }
    return 0;
}

// Each frame of a shot: the ball's physics steps for this frame (GameEffects_BallUpdatesThisFrame;
// none when the mode has n294 and the ball is behind the camera), then on one screen the look-ahead
// copy (ballBefore) runs on within a budget of 0.83 ms minus what the real ball took, until it
// comes to rest (event 0x3C and its outcome recorded). Then, when fn_800BB1F8 allows and the mode
// has post-shot reactions: a scripted reaction (uFlags bit 0) starts animation 9 once the ball
// passes the saved distance (fEEC, with bit 2); otherwise, for outcomes 8 and 9, once per shot when
// the ball is 2 to 5.5 from the pin and near the closest it got, a look-ahead holed at par or
// better starts it half the time and a look-ahead miss that came within 0.2 always does.
void GM_SimulateBallMovement(int nPlayer) {
    int nSteps = 0;
    u64 t0;
    int nUpdates;
    int i;
    f32 fBudget;
    f32 fMs;
    u8  bReact;
    u8  bOn;
    f32 fDist;
    int nResult;

    t0 = TI_sReadCounter(0);
    nUpdates = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (gpGame->n294 != 0
        && fn_800C71A4(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), nPlayer)) {
        nUpdates = 0;
    }
    for (i = 0; i < nUpdates; i++) {
        Physics_Simulate(&gPlayers[nPlayer].ball, 20);
    }
    fMs = 1000.0f * fn_8006E118(TI_sReadCounter(0), t0);
    fBudget = 0.83f - fMs;
    if (OnlineGolf_bIsOnlineGame()) {
        fBudget = 0.83f;
    }
    if (gSession.nSplitScreen == 0 && gSession.fFrameTime > 0.0f) {
        fn_80050D24_SetSimulating(1);
        fn_80050D2C(1);
        while (gPlayers[nPlayer].ballBefore.nState != 1 && gPlayers[nPlayer].ballBefore.nState != 5 &&
               gPlayers[nPlayer].ballBefore.nState != 0 && fBudget > 0.1f) {
            t0 = TI_sReadCounter(0);
            Physics_Simulate(&gPlayers[nPlayer].ballBefore, 20);
            fMs = 1000.0f * fn_8006E118(TI_sReadCounter(0), t0);
            nSteps++;
            fBudget -= fMs;
            if (OnlineGolf_bIsOnlineGame()) {
                if (nSteps < 2) {
                    fBudget = 0.83f;
                } else {
                    fBudget = 0.0f;
                }
            }
            if (gPlayers[nPlayer].ballBefore.nState == 1 || gPlayers[nPlayer].ballBefore.nState == 5 ||
                gPlayers[nPlayer].ballBefore.nState == 0) {
                if (!(gPlayers[nPlayer].uFlags & 8)) {
                    EVENT_Trigger(nPlayer, 0x3C, 0, -1);
                    fn_8006B2C4(nPlayer, 1);
                }
                break;
            }
        }
        fn_80050D2C(0);
        fn_80050D24_SetSimulating(0);
        if (fn_800BB1F8(nPlayer)) {
            nResult = fn_8006AA9C(nPlayer);
            bReact  = nResult == 8 || nResult == 9;
            bOn     = GM_IsValidPostShotGameType();
            fDist   = fn_800D0478(nPlayer);
            if (bOn) {
                if (gPlayers[nPlayer].uFlags & 1) {
                    if ((gPlayers[nPlayer].uFlags & 4) && fDist < gPlayers[nPlayer].fEEC) {
                        fn_80095744(gPlayers[nPlayer].pChar, 9);
                    }
                } else if (bReact) {
                    if (!gPlayers[nPlayer].bRehearsalDone && fDist < 5.5f && fDist > 2.0f &&
                        fDist - gPlayers[nPlayer].ball.fClosest < 0.3f) {
                        if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
                            Hole_ScoreAfterTapIn(nPlayer) <= 0) {
                            if (Misc_RandFunc(1) % 100 < 50) {
                                gPlayers[nPlayer].uFlags |= 4;
                                gPlayers[nPlayer].fEEC = fDist;
                                fn_80095744(gPlayers[nPlayer].pChar, 9);
                            }
                        } else if (gPlayers[nPlayer].ballBefore.nLie != LIE_INCUP_e &&
                                   gPlayers[nPlayer].ballBefore.fClosest < 0.2f) {
                            gPlayers[nPlayer].uFlags |= 4;
                            gPlayers[nPlayer].fEEC = fDist;
                            fn_80095744(gPlayers[nPlayer].pChar, 9);
                        }
                        gPlayers[nPlayer].bRehearsalDone = 1;
                    }
                }
            }
        }
    }
}

// Every frame of a round (gomainloop): when the mode's pfnCheckControllerPulled allows it (by
// default always; speed golf only late in the countdown or while player 0's view is not fading) and
// player 0's view is not fading, calls GUI_DetectControllerPull, an empty function beside the pause
// requests of GameMessages.c: the controller-pulled check is stubbed out in this build.
void GM_CheckControllerPulled(void) {
    if (gpGame->pfnCheckControllerPulled()) {
        if (!fn_80063C90(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
            GUI_DetectControllerPull();
        }
    }
}

// Loads a saved custom round into the round's hole list: the hole and course of each of the 18
// holes from save profile nSaveSlot, custom round nSaveCourse.
void GM_SetupCustomHoleSelection(void) {
    int       i;
    SaveProfile* pSave;
    for (i = 0; i < 18; i++) {
        pSave = gpSaveData;
        gpGame->nHoleNum[i] = pSave[gpGame->nSaveSlot].aSavedRound[gpGame->nSaveCourse].nHoleNum[i];
        gpGame->nHoleCourse[i] = gpSaveData[gpGame->nSaveSlot].aSavedRound[gpGame->nSaveCourse].nCourse[i];
    }
}

// Whether button 8 was tapped: released after 2 to 5 frames of holding (the count is kept by
// GM_bIsZoomButtonPressed), which asks for the mid-hole flyover; only when the mode has one (b280).
// A tap resets the count.
u8 GM_bIsMidholeFlybyButtonPressed(int nPlayer) {
    if (!gpGame->b280) {
        return 0;
    }
    if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(8, 0)) ||
        (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(8, 1))) {
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

// Whether button 8 has been held for the zoom camera: a press starts the player's count (n144),
// each frame it stays held adds the frames that passed (FRAME_RATE a second, rounded), and at 6 the
// count resets and the answer is yes.
u8 GM_bIsZoomButtonPressed(int nPlayer) {
    if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(8, 0))
        && gpGame->n144[nPlayer] == 0) {
        gpGame->n144[nPlayer]++;
    } else if (gpGame->n144[nPlayer] > 0) {
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(8, 1)) {
            gpGame->n144[nPlayer] += (int)(FRAME_RATE * gSession.fFrameTime + 0.5f);
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

// Whether button 47 was tapped: released after 2 to 19 frames of holding (the count n158 is kept by
// GM_bIsElevatorCamButtonPressed), which switches to the next alternate swing camera. A tap resets
// the count.
u8 GM_bIsAltSwingButtonPressed(int nPlayer) {
    if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2F, 0)) ||
        (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2F, 1))) {
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

// Whether button 47 has been held for the elevator camera: counted as in GM_bIsZoomButtonPressed
// (in n158), yes at 20 frames.
u8 GM_bIsElevatorCamButtonPressed(int nPlayer) {
    if ((Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2F, 0))
        && gpGame->n158[nPlayer] == 0) {
        gpGame->n158[nPlayer]++;
    } else if (gpGame->n158[nPlayer] > 0) {
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x2F, 1)) {
            gpGame->n158[nPlayer] += (int)(FRAME_RATE * gSession.fFrameTime + 0.5f);
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

// GM_vGetAllTimeRecordsHeld's test that a place's value reaches a record. The original compares
// three times over (the three branches are in the binary): most likely a macro written for a
// record of several fields, all of which are the one value here.
#define RECORD_AT_LEAST(a, b) ((a) >= (b) && (a) >= (b) && (a) >= (b))

// How many all-time records the profile's golfer holds, matched by name: in the first table
// (gSession.recA, 8 records of 5 places) a place with the record's own value; in the other two
// (recB, 3 blocks of 3 records; recC, 5 blocks of 2 records; 5 places each) a place whose value is
// at least the record of any of the first three blocks. Each record counts once.
// GM_GetBonusProgress gives half a point for each.
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
                            // fake match: leaves all three loops (ending them by setting their counters: 83%)
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
                            // fake match: as above
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

// TW06: GM_GetGameProgress (by position). The profile's completion score: a point for each ladder
// event and each PGA TOUR tournament won; half a point for a TOUR card, for each challenge group
// with a medal, each of the first 23 awards, each of the 75 par-5 holes eagled (fn_800588F4), and
// 14 golfers and 6 courses unlocked (the lists lbl_80189528, lbl_801894D0); plus the bonus progress
// below.
f32 GM_GetGameProgress(SaveProfile* pProfile) {
    f32 f = 0.0f;
    int i;
    for (i = 0; i < 25; i++) {
        if (pProfile->aLadderAward[i].bWon) {
            f += 1.0f;
        }
    }
    if (pProfile->nTourCardLevel >= 1) {
        f += 0.5f;
    }
    for (i = 0; i < 29; i++) {
        if (pProfile->aMedal[i] != 3) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 31; i++) {
        if (pProfile->aC8[i].award.bWon) {
            f += 1.0f;
        }
    }
    for (i = 0; i < 23; i++) {
        if (pProfile->aAward[i].bWon == 1) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 75; i++) {
        if (fn_800588F4(pProfile, 0, i)) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 14; i++) {
        if (pProfile->aGolferUnlocked[lbl_80189528[i]]) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 6; i++) {
        if (pProfile->aCourseUnlocked[lbl_801894D0[i]]) {
            f += 0.5f;
        }
    }
    return f + GM_GetBonusProgress(pProfile);
}

// The bonus part of the profile's completion score (GM_GetGameProgress adds it): a point for each
// real-time event won; half a point for each of awards 23..38 won, each of the 16 a1C0 awards won
// and each all-time record held.
f32 GM_GetBonusProgress(SaveProfile* pProfile) {
    f32 f = 0.0f;
    int i;
    for (i = 0; i < 75; i++) {
        if (pProfile->aRTEAward[i].bWon) {
            f += 1.0f;
        }
    }
    for (i = 23; i < 39; i++) {
        if (pProfile->aAward[i].bWon) {
            f += 0.5f;
        }
    }
    for (i = 0; i < 12; i++) {
        if (pProfile->a1C0[i].bWon) {
            f += 0.5f;
        }
    }
    for (i = 12; i < 16; i++) {
        if (pProfile->a1C0[i].bWon) {
            f += 0.5f;
        }
    }
    f += 0.5f * GM_vGetAllTimeRecordsHeld(pProfile);
    return f;
}

// Sets whether GM_Pick_PlayOffHole has to build the playoff hole list (gpGame->b135): 1 at the
// start of a game, 0 once the list is built.
void GM_SetNeedToBuildPlayoffHoleList(u8 v) {
    gpGame->b135 = v;
}

// Always 0 in this build. When set, GM_EndOfGolferTurn treats the hole as finished and
// GM_EndOfGolferTurn_HoleFinished skips the last-hole payouts and ends the game, whatever the mode
// says.
u8 GM_IsRoundForcedOver(int nPlayer) {
    return 0;
}

// Shows (1) or hides (0) the HUD's replay prompt: UI message 46 with the flag.
void GUI_ToggleReplay(int a) {
    GameMsg_SendInt(46, (u8)a);
}

// Shows (1) or hides (0) the HUD's mulligan prompt: UI message 29 with the flag.
void GUI_ToggleMulligan(int a) {
    GameMsg_SendInt(29, (u8)a);
}
