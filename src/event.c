// event.c (TW06's golf/eventmanager/event.c): the game's event handlers. EVENT_Trigger calls the
// handler for an event number from the file's table (lbl_80188628): stepping through the clubs and
// shot kinds, the camera and commentary for each moment of a shot, and the lessons' checks
// (Lessons_OnEvent can block an event). Most handlers pass the moment on to SitDev_QueueEvent.

#include "game.h"
#include "terrain.h"
#include "sitdev.h"
#include "core/easb.h"

SitDevData lbl_801D5AB0;
SitDevData* lbl_802811B8 = &lbl_801D5AB0;
u32 lbl_80281E20;              // seconds counted by event 26

void Character_InitNewClubAndShotType(int nPlayer);
void fn_80033704(u16 nPatch, u16 nObject);
void fn_8003349C(f32 fPercentage, f32 fDuration, f32 fDelay);
void fn_80051C84(Ball* pBall, f32 fX, f32 fY);
void SW_vSetDisplayBoostUI(int nPlayer, int bDisplay);
void SW_vGetCurrentSpin(int nPlayer, f32* pfSide, f32* pfForward);
void SW_vCloseSpinWindow(int nPlayer);
void fn_800690C0(int nPlayer);
void fn_80069104(int nPlayer);
void fn_80069148(int nPlayer);
void fn_800691B0(int nPlayer);
void fn_80069A84(int nPlayer);
void fn_80069AFC(int nPlayer);
void fn_80069B74(int nPlayer);
void fn_80069BEC(int nPlayer);
int  fn_8006AA70(int nPlayer);
int  fn_8006AA84(int nPlayer);
void fn_8009A16C(void);
void fn_800A2FFC(int nPlayer, int nArg);
void fn_800A31E0(Ball* pBall, int nPlayer);
void fn_800A3348(Ball* pBall, int nPlayer);
void Gaud_EndHole(void);
void Gaud_BallBounce(u8 nPlayer);
void Gaud_BallStopped(u8 nPlayer);
void Gaud_BallInCup(u8 nPlayer);
void Gaud_BallHitPole(u8 nPlayer);
void Gaud_BallHitMetalTarget(u8 nPlayer);
void Gaud_Tappa(u8 nPlayer);
void Gaud_Spina(u8 nPlayer);
void Gaud_PlayTappaFeedback(u8 nPlayer);
void Gaud_InitGameBreaker(u8 nPlayer, int a);
void Gaud_ExitGameBreaker(u8 nPlayer);
void Gaud_InitCamZoom(u8 nPlayer);
void Gaud_ExitCamZoom(u8 nPlayer);
void Gaud_ExitSpecialShot(u8 nPlayer);
void AnimStream_AssignSlots(void);
void GameEffects_SpinWindowDone(int nPlayer);
void GUI_QueueTip(int n);
void GameMode26_NoteSplitScreenShot(int nPlayer);
void AI_SimAbort(void);
void AI_AimAtPin(int nPlayer);
void fn_80067220(int nPlayer);
void fn_8006752C(void);
void fn_80067550(int nPlayer);
void fn_80067554(int nPlayer);
void fn_80067558(int nPlayer);
void fn_8006755C(int nPlayer);
int  fn_80067560(void);

// At the start of a round (GO_vInitIG): zeroes the count of seconds EVENT_Idle (event 26) keeps.
// TW07 has both EVENT_InitForGame and EVENT_ResetIdleSeconds here, the same size; the caller makes
// this one InitForGame.
void EVENT_InitForGame(void) {
    lbl_80281E20 = 0;
}

// Event 41 (GoBreakLine.c: the putt's break line passed the cup): queues situation event 26 for the
// commentary scripts (SitDev_QueueEvent) unless a lesson blocks it.
void EVENT_BreaklinePassedCup(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 41)) {
        SitDev_QueueEvent(nPlayer, 2, 26);
    }
}

// Event 40 (GoBreakLine.c: the putt's break line is drawn to its end): does nothing in this build.
void EVENT_BreaklineDone(int nPlayer, int nEvent, void* pData, int nArg) {
}

// Event 0 (GM_InitForHole: a hole begins): queues situation event 1 for the commentary scripts
// unless a lesson blocks it.
void EVENT_BeginHole(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 0)) {
        SitDev_QueueEvent(nPlayer, 2, 1);
    }
}

// Event 1 (a hole is over): queues situation event 12 for the commentary scripts unless a lesson
// blocks it, then always Gaud_EndHole.
void EVENT_EndHole(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 1)) {
        SitDev_QueueEvent(nPlayer, 2, 12);
    }
    Gaud_EndHole();
}

// Event 2 (GameUICommands.c: the hole is restarted): clears the swing boosts of all five players
// (SW_vClearBoosts).
void EVENT_RestartHole(int nPlayer, int nEvent, void* pData, int nArg) {
    int i = 0;

    do {
        SW_vClearBoosts(i);
        i++;
    } while (i < 5);
}

// Event 3 (a player's turn begins): queues situation event 2 for the commentary scripts unless a
// lesson blocks it, then always hands out the animation stream slots (AnimStream_AssignSlots).
void EVENT_BeginTurn(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 3)) {
        SitDev_QueueEvent(nPlayer, 2, 2);
    }
    AnimStream_AssignSlots();
}

// Event 4 (a player's turn is over): only tells the lessons (their answer is not used).
void EVENT_EndTurn(int nPlayer, int nEvent, void* pData, int nArg) {
    Lessons_OnEvent(nPlayer, 4);
}

// Event 6 (the shot is set up): queues situation event 3 for the commentary scripts unless a lesson
// blocks it.
void EVENT_ShotSetup(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 6)) {
        SitDev_QueueEvent(nPlayer, 7, 3);
    }
}

// Event 7 (just before the swing): queues situation event 25 for the commentary scripts unless a
// lesson blocks it.
void EVENT_PreSwing(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 7)) {
        SitDev_QueueEvent(nPlayer, 2, 25);
    }
}

// Event 8: does nothing (nothing in this build fires it).
void EVENT_Delay(int nPlayer, int nEvent, void* pData, int nArg) {
}

// Event 9 (Swing.c: a practice swing): queues situation event 4 for the commentary scripts unless a
// lesson blocks it. It asks the lessons with 21 (EVENT_MoveTargetBack's number), not its own 9.
void EVENT_PracticeSwing(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 21)) {
        SitDev_QueueEvent(nPlayer, 2, 4);
    }
}

// Event 10 (Ball.c: the ball is struck), only for the real ball (nArg 1; 0 is the AI's simulated
// ball): in the demo (session flag 0x4000) of mode 26 the demo's timer restarts (fn_8009A16C); then
// the mode's ball-hit hook (gpGame->pfn260), the lessons (their crowd sound), the swing effect at
// the ball (fn_800A31E0), SitDev starts watching the ball (fn_800BB1A8), commentary situation event
// 5, and GameMode26's split-screen flag (fn_8010D3B8, which takes no argument).
void EVENT_HitBall(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        if (Game_GetMode() == 26 && (gSession.uFlags & 0x4000)) {
            fn_8009A16C();
        }
        gpGame->pfn260(nPlayer);
        Lessons_OnEvent(nPlayer, 10);
        fn_800A31E0(pData, nPlayer);
        fn_800BB1A8(&gPlayers[nPlayer].ball);
        SitDev_QueueEvent(nPlayer, 2, 5);
        GameMode26_NoteSplitScreenShot(nPlayer);
    }
}

// Event 11 (CharAnim.c: the swing animation is over): queues situation event 6 for the commentary
// scripts.
void EVENT_SwingDone(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 2, 6);
}

// Event 12: does nothing (nothing in this build fires it).
void EVENT_BallBounce(int nPlayer, int nEvent, void* pData, int nArg) {
}

// Event 13 (the club-change button): steps nClub down one at a time (toward the driver, then round
// to the putter) to the first club Club_UsableForKind allows for the shot kind, the putter after 26
// tries. The putter is kept only on the green or with the green within 1.5 yards toward the pin,
// otherwise the old club comes back. Then the target is fitted to the club (not for a chip), the
// power worked out again, the front end told (message 7 and fn_8006752C), the golfer re-set for the
// club, and the club remembered for the shot kind. Nothing when a lesson blocks it, in the
// long-drive modes 22 and 26, or with the putter in hand.
void EVENT_NextClub(int nPlayer, int nEvent, void* pData, int nArg) {
    int nOldClub;
    int nTries;
    u8 bOk;

    if (Lessons_OnEvent(nPlayer, 13) || Game_GetMode() == 22 || Game_GetMode() == 26) return;
    nOldClub = gPlayers[nPlayer].nClub;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) return;
    nTries = 0;
    do {
        if (gPlayers[nPlayer].nClub > 0) {
            gPlayers[nPlayer].nClub--;
        } else {
            gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
        }
        bOk = Club_UsableForKind(nPlayer, gPlayers[nPlayer].nClub, gPlayers[nPlayer].nShotKind);
        nTries++;
    } while (!bOk && nTries < CLUB_MAX_e);
    if (nTries == CLUB_MAX_e) {
        gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            gPlayers[nPlayer].nShotKind = 0;
        } else {
            gPlayers[nPlayer].nClub = nOldClub;
        }
    }
    if (gPlayers[nPlayer].nShotKind != 2) {
        Shot_FitTargetToClub(nPlayer);
        LLMath_CopyVec(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTarget2);
    }
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    fn_8006752C();
    Character_InitNewClubAndShotType(nPlayer);
    gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind] = gPlayers[nPlayer].nClub;
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Event 14: as EVENT_NextClub, but nClub steps up (toward the putter, then round to the driver). It
// asks the lessons with 13 (EVENT_NextClub's number), not its own 14; the lessons block both alike.
void EVENT_PrevClub(int nPlayer, int nEvent, void* pData, int nArg) {
    int nOldClub;
    int nTries;
    u8 bOk;

    if (Lessons_OnEvent(nPlayer, 13) || Game_GetMode() == 22 || Game_GetMode() == 26) return;
    nOldClub = gPlayers[nPlayer].nClub;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) return;
    nTries = 0;
    do {
        if (gPlayers[nPlayer].nClub < CLUB_PUTTER_e) {
            gPlayers[nPlayer].nClub++;
        } else {
            gPlayers[nPlayer].nClub = 0;
        }
        bOk = Club_UsableForKind(nPlayer, gPlayers[nPlayer].nClub, gPlayers[nPlayer].nShotKind);
        nTries++;
    } while (!bOk && nTries < CLUB_MAX_e);
    if (nTries == CLUB_MAX_e) {
        gPlayers[nPlayer].nClub = CLUB_PUTTER_e;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            gPlayers[nPlayer].nShotKind = 0;
        } else {
            gPlayers[nPlayer].nClub = nOldClub;
        }
    }
    if (gPlayers[nPlayer].nShotKind != 2) {
        Shot_FitTargetToClub(nPlayer);
        LLMath_CopyVec(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTarget2);
    }
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    fn_8006752C();
    Character_InitNewClubAndShotType(nPlayer);
    gPlayers[nPlayer].nClubPerKind[gPlayers[nPlayer].nShotKind] = gPlayers[nPlayer].nClub;
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Event 15 (the shot-kind button): the next shot kind with its club (fn_80067220), and in split
// screen the front end is sent message 0x14 with the player's nC58. Nothing when a lesson blocks it
// or in the long-drive modes 22 and 26.
void EVENT_NextShotType(int nPlayer, int nEvent, void* pData, int nArg) {
    if (Lessons_OnEvent(nPlayer, 15) || Game_GetMode() == 26 || Game_GetMode() == 22) return;
    fn_80067220(nPlayer);
    if (gSession.nSplitScreen) {
        fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    }
}

// Event 16: raises the trajectory (nTrajectory, up to 2, high) and works out the power for the
// target again; front-end message 7 is sent even at the top. Nothing when a lesson blocks it. TW06
// and TW07 call this setting the stance; nothing in this build fires the event.
void EVENT_PrevStance(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 16)) {
        if (gPlayers[nPlayer].nTrajectory != 2) {
            gPlayers[nPlayer].nTrajectory++;
            gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
        }
        fn_80062C38();
    }
}

// Event 17: as EVENT_PrevStance, the trajectory one lower (down to 0, low). Nothing in this build
// fires the event.
void EVENT_NextStance(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 17)) {
        if (gPlayers[nPlayer].nTrajectory != 0) {
            gPlayers[nPlayer].nTrajectory--;
            gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
        }
        fn_80062C38();
    }
}

// Event 19 (aiming): the aim point's turn input ramps toward +1 (fn_80069104) and the green grid of
// the player's view is laid out again; nothing when a lesson blocks it.
void EVENT_RotateRight(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 19)) {
        fn_80069104(nPlayer);
        fn_8009B970(gPlayers[nPlayer].nView[0]);
    }
}

// Event 18 (aiming): the aim point's turn input ramps toward -1 (fn_800690C0) and the green grid of
// the player's view is laid out again; nothing when a lesson blocks it.
void EVENT_RotateLeft(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 18)) {
        fn_800690C0(nPlayer);
        fn_8009B970(gPlayers[nPlayer].nView[0]);
    }
}

// Event 20 (aiming): the aim point's move input ramps toward +1 (fn_80069148); nothing when a
// lesson blocks it.
void EVENT_MoveTargetForward(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 20)) {
        fn_80069148(nPlayer);
    }
}

// Event 21 (aiming): the aim point's move input ramps toward -1 (fn_800691B0); nothing when a
// lesson blocks it.
void EVENT_MoveTargetBack(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 21)) {
        fn_800691B0(nPlayer);
    }
}

// Event 23 (placing the ball): the placement cursor's turn input (fA80) ramps toward -1
// (fn_80069BEC).
void EVENT_PlaceBallRotateRight(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069BEC(nPlayer);
}

// Event 22 (placing the ball): the placement cursor's turn input (fA80) ramps toward +1
// (fn_80069B74).
void EVENT_PlaceBallRotateLeft(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069B74(nPlayer);
}

// Event 24 (placing the ball): the placement cursor's move input (fA84) ramps toward +1
// (fn_80069A84).
void EVENT_PlaceBallMoveTargetForward(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069A84(nPlayer);
}

// Event 25 (placing the ball): the placement cursor's move input (fA84) ramps toward -1
// (fn_80069AFC).
void EVENT_PlaceBallMoveTargetBack(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80069AFC(nPlayer);
}

// Event 26 (the main loop, once a second of game time, for player 0xFF): counts the second, then
// queues commentary situation event 19 on every 60th second and 18 on every other 10th.
void EVENT_Idle(int nPlayer, int nEvent, void* pData, int nArg) {
    lbl_80281E20++;
    if (lbl_80281E20 % 60 == 0) {
        SitDev_QueueEvent(nPlayer, 5, 19);
    } else if (lbl_80281E20 % 10 == 0) {
        SitDev_QueueEvent(nPlayer, 5, 18);
    }
}

// Event 28 (Ball.c: the ball tops its arc), only for the real ball (nArg 1) and unless a lesson
// blocks it: camera event 1 for the player's view (fn_80063CF0), emotion event 3 (fn_8006ACF8), and
// fn_80095744 with 13 on the golfer's character, which that function ignores.
void EVENT_TopOfArc(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1 && !Lessons_OnEvent(nPlayer, 28)) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 1, nPlayer);
        fn_8006ACF8(nPlayer, 3);
        fn_80095744(gPlayers[nPlayer].pChar, 13);
    }
}

void fn_80066664(int nPlayer, int nEvent, void* pData, int nArg) {
    f32 fSpinY;
    f32 fSpinX;

    if (nArg == 1) {
        fSpinY = 0.0f;
        fSpinX = 0.0f;
        if (!Lessons_OnEvent(nPlayer, 29)) {
            SitDev_QueueEvent(nPlayer, 2, 28);
        }
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 3, nPlayer);
        fn_8006ACF8(nPlayer, 2);
        if (Player_IsController8(nPlayer)) {
            fn_80067554(nPlayer);
        } else {
            fn_80067550(nPlayer);
        }
        SW_vGetCurrentSpin(nPlayer, &fSpinY, &fSpinX);
        if (!gSession.bReplay) {
            REPLAY_SaveSpin(nPlayer, fSpinX, fSpinY);
        }
        fn_80051C84(&gPlayers[nPlayer].ball, fSpinY, fSpinX);
    }
}

void fn_8006676C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 30, nPlayer);
    }
}

void fn_800667C0(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 31, nPlayer);
        SW_vCloseSpinWindow(nPlayer);
        SW_vSetDisplayBoostUI(nPlayer, 0);
    }
}

void fn_80066828(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        Gaud_BallStopped(nPlayer);
        if (!Lessons_OnEvent(nPlayer, 32)) {
            SitDev_QueueEvent(nPlayer, 2, 8);
        }
        if (Player_IsController8(nPlayer)) {
            fn_8006755C(nPlayer);
        } else {
            fn_80067558(nPlayer);
        }
    }
}

void fn_800668A8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 8, nPlayer);
        fn_8006ACF8(nPlayer, 4);
        Gaud_BallInCup(nPlayer);
        SitDev_QueueEvent(nPlayer, 2, 9);
    }
}

void fn_80066920(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1 && !Lessons_OnEvent(nPlayer, 34)) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 6, nPlayer);
        SitDev_QueueEvent(nPlayer, 2, 8);
    }
}

void fn_800670A8(int nPlayer, int nEvent, void* pData, int nArg);

void fn_80066994(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669B4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669D4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_800669F4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800670A8(nPlayer, nEvent, pData, nArg);
}

void fn_80066A14(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 2, 29);
}

// Event 39: the ball hit a world object; tell the game mode which.
void fn_80066A3C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (gPlayers[nPlayer].ball.pHitActor != NULL) {
        gpGame->pfn268(nPlayer, gPlayers[nPlayer].ball.pHitActor->n140);
        Gaud_BallHitMetalTarget(nPlayer);
    }
}

void fn_80066A9C(int nPlayer, int nEvent, void* pData, int nArg) {
    int nA;
    int nB;
    int nAnim;

    if (Game_GetMode() != 11 || nArg == 1) {
        nA = fn_8006AA70(nPlayer);
        nB = fn_8006AA84(nPlayer);
        switch (nA) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            nAnim = 1;
            break;
        default:
            nAnim = -1;
            break;
        }
        if (nAnim >= 0) {
            fn_800957B0(gPlayers[nPlayer].pChar, nAnim);
        }
        if (nA == 0) {
            switch (nB) {
            case 0:
                fn_8003349C(0.3f, 3.0f, 0.0f);
                break;
            case 1:
                fn_8003349C(0.5f, 6.0f, 0.0f);
                break;
            case 2:
                fn_8003349C(0.8f, 8.0f, 0.0f);
                break;
            case 3:     // EA lists case 3 with the default (it sets CW's compare tree)
            default:
                fn_8003349C(1.0f, 10.0f, 0.0f);
                break;
            }
        }
    }
}

void fn_80066BB8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        SitDev_QueueEvent(nPlayer, 2, 7);
    }
}

void fn_80066BE8(int nPlayer, int nEvent, void* pData, int nArg) {
    GameEffects_SpinWindowDone(nPlayer);
}

void fn_80066C08(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800335F8(0);
}

void fn_80066C2C(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_Spina(nPlayer);
    if (Lessons_OnEvent(nPlayer, 46)) return;   // the result is tested (clrlwi.) with nothing after it
}

void fn_80066C6C(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_Tappa(nPlayer);
    if (Lessons_OnEvent(nPlayer, 45)) return;   // as in fn_80066C2C
}

void fn_80066CAC(int nPlayer, int nEvent, void* pData, int nArg) {
    if (SW_fGetBoostMagnitude(nPlayer) > 0 && gPlayers[nPlayer].nClub >= 0 && gPlayers[nPlayer].nClub <= 5) {
        Gaud_PlayTappaFeedback(nPlayer);
    }
}

void fn_80066D0C(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_InitCamZoom(nPlayer);
}

void fn_80066D30(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_ExitCamZoom(nPlayer);
}

void fn_80066D54(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D58(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D5C(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D60(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D64(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D68(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D6C(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D70(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D74(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066D78(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_800A2FFC(nPlayer, nArg);
    if (nArg == 1) {
        Gaud_ExitSpecialShot(nPlayer);
    }
}

void fn_80066DC4(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 2, 20);
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 11, nPlayer);
    }
}

void fn_80066E28(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_InitGameBreaker(nPlayer, 0);
    if (fn_80067560()) {
        GUI_QueueTip((u8)(fn_80067560() + 15));
    }
}

void fn_80066E6C(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_ExitGameBreaker(nPlayer);
}

void fn_80066E90(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_InitGameBreaker(nPlayer, 1);
}

void fn_80066EB8(int nPlayer, int nEvent, void* pData, int nArg) {
    Gaud_ExitGameBreaker(nPlayer);
}

void fn_80066EDC(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!gSession.a8[0]) {
        EASBio_IncrementGamesPlayed(1);
    }
    SitDev_QueueEvent(nPlayer, 2, 13);
}

void fn_80066F30(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 4, 22);
}

void fn_80066F58(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 8, 23);
}

void fn_80066F80(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 8, 24);
}

void fn_80066FA8(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FAC(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FB0(int nPlayer, int nEvent, void* pData, int nArg) {
}

void fn_80066FB4(int nPlayer, int nEvent, void* pData, int nArg) {
    fn_80035574();
}

void fn_80066FD4(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 5) {
        SitDev_QueueEvent(nPlayer, 2, 32);
    }
}

void fn_80067004(int nPlayer, int nEvent, void* pData, int nArg) {
    SitDev_QueueEvent(nPlayer, 9, 30);
}

void fn_8006702C(int nPlayer, int nEvent, void* pData, int nArg) {
    if (!Lessons_OnEvent(nPlayer, 75)) {
        SitDev_QueueEvent(nPlayer, 2, 31);
    }
}

// The handlers, by event number.
EventHandler lbl_80188628[76] = {
    EVENT_BeginHole, EVENT_EndHole, EVENT_RestartHole, EVENT_BeginTurn, EVENT_EndTurn, fn_80066EDC, EVENT_ShotSetup,
    EVENT_PreSwing, EVENT_Delay, EVENT_PracticeSwing, EVENT_HitBall, EVENT_SwingDone, EVENT_BallBounce, EVENT_NextClub,
    EVENT_PrevClub, EVENT_NextShotType, EVENT_PrevStance, EVENT_NextStance, EVENT_RotateLeft, EVENT_RotateRight, EVENT_MoveTargetForward,
    EVENT_MoveTargetBack, EVENT_PlaceBallRotateLeft, EVENT_PlaceBallRotateRight, EVENT_PlaceBallMoveTargetForward, EVENT_PlaceBallMoveTargetBack, EVENT_Idle, fn_80066BB8,
    EVENT_TopOfArc, fn_80066664, fn_8006676C, fn_800667C0, fn_80066828, fn_800668A8, fn_80066920,
    fn_80066994, fn_800669B4, fn_800669D4, fn_800669F4, fn_80066A3C, EVENT_BreaklineDone, EVENT_BreaklinePassedCup,
    fn_80066A9C, fn_80066BE8, fn_80066C08, fn_80066C6C, fn_80066C2C, fn_80066CAC, fn_80066D0C,
    fn_80066D30, fn_80066D54, fn_80066D58, fn_80066D5C, fn_80066D60, fn_80066D64, fn_80066D68,
    fn_80066D6C, fn_80066D70, fn_80066D74, fn_80066D78, fn_80066DC4, fn_80066E28, fn_80066E6C,
    fn_80066E90, fn_80066EB8, fn_80066F30, fn_80066F58, fn_80066F80, fn_80066FA8, fn_80066FAC,
    fn_80066FB0, fn_80066FB4, fn_80066FD4, fn_80066A14, fn_80067004, fn_8006702C,
};

void EVENT_Trigger(int nPlayer, int nEvent, void* pData, int b) {
    lbl_80188628[nEvent](nPlayer, nEvent, pData, b);
}

// Events 35..38, the ball lands (35 ground, 36 an object, 37 a flagged surface, 38 surface 90):
// nArg 1: the camera, sounds, effects and commentary; otherwise event 36 aborts the simulation.
void fn_800670A8(int nPlayer, int nEvent, void* pData, int nArg) {
    if (nArg == 1) {
        fn_80063CF0(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]), 7, nPlayer);
        Gaud_BallBounce(nPlayer);
        fn_800A3348(pData, nPlayer);
        fn_8006ACF8(nPlayer, 1);
        if (nEvent == 36 && gPlayers[nPlayer].ball.pHitObject != NULL) {
            fn_80033704(gPlayers[nPlayer].ball.pHitObject->nPatch,
                        gPlayers[nPlayer].ball.pHitObject->nObjList);
        }
        if (nEvent == 38) {
            gPlayers[nPlayer].b30D = 1;
            Gaud_BallHitPole(nPlayer);
        }
        if (nEvent == 36) {
            gPlayers[nPlayer].b30C = 1;
        }
        if (nEvent == 37) {
            SitDev_QueueEvent(nPlayer, 2, 27);
        }
        if (nEvent == 36 || nEvent == 37) {
            SitDev_QueueEvent(nPlayer, 2, 16);
        } else if (nEvent == 38) {
            SitDev_QueueEvent(nPlayer, 2, 21);
        }
        gpGame->pfn23C(nPlayer);
    } else if (nEvent == 36) {
        AI_SimAbort();
    }
}

// The next shot kind, with its club and aim.
// fake match: EA reads a few fields as gPlayers[nPlayer] beside pPlayer (the asm recomputes the
// address there), so those stay in that form.
void fn_80067220(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];

    switch (pPlayer->nShotKind) {
    case 0:
        pPlayer->nShotKind = 3;
        pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
        break;
    case 1:
        pPlayer->nShotKind = 4;
        pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
        pPlayer->nShotKind2 = pPlayer->nShotKind;
        break;
    case 4:
        pPlayer->nShotKind = 3;
        pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
        pPlayer->nShotKind2 = pPlayer->nShotKind;
        break;
    case 3:
        if (pPlayer->ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            AI_AimAtPin(nPlayer);
            pPlayer->fAim = Shot_AimAngle(nPlayer);
            pPlayer->nShotKind = 2;
            pPlayer->nClub = pPlayer->nClub =
                pPlayer->nClubPerKind[pPlayer->nShotKind];
            Shot_FitTargetToClub(nPlayer);
        } else {
            pPlayer->nShotKind = 5;
            pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
            pPlayer->nShotKind2 = pPlayer->nShotKind;
        }
        break;
    case 5:
        if (pPlayer->ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            AI_AimAtPin(nPlayer);
            pPlayer->fAim = Shot_AimAngle(nPlayer);
            pPlayer->nShotKind = 0;
            pPlayer->nClub = CLUB_PUTTER_e;
            Shot_FitTargetToClub(nPlayer);
        } else if (Player_NotInSand(nPlayer)) {
            AI_AimAtPin(nPlayer);
            pPlayer->fAim = Shot_AimAngle(nPlayer);
            pPlayer->nShotKind = 2;
            pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
            pPlayer->nShotKind2 = gPlayers[nPlayer].nShotKind;
            Shot_FitTargetToClub(nPlayer);
        } else {
            pPlayer->nShotKind = 1;
            pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
            pPlayer->nShotKind2 = gPlayers[nPlayer].nShotKind;
        }
        break;
    case 2:
        if (pPlayer->ball.nLie == LIE_GREEN_e) {
            AI_AimAtPin(nPlayer);
            pPlayer->fAim = Shot_AimAngle(nPlayer);
            pPlayer->nShotKind = 0;
            pPlayer->nClub = CLUB_PUTTER_e;
            Shot_FitTargetToClub(nPlayer);
        } else {
            pPlayer->nShotKind = 1;
            pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
            pPlayer->nShotKind2 = pPlayer->nShotKind;
        }
        break;
    case 6:
    case 7:
    default:
        pPlayer->nShotKind = 1;
        pPlayer->nClub = pPlayer->nClubPerKind[pPlayer->nShotKind];
        pPlayer->nShotKind2 = pPlayer->nShotKind;
        break;
    }
    TARGET_SetupTarget(nPlayer);
    pPlayer->fA60 = 0.0f;
    Shot_FitTargetToClub(nPlayer);
    LLMath_CopyVec(pPlayer->vTarget, pPlayer->vTarget2);
    gPlayers[nPlayer].fPower = AI_PowerForTarget(nPlayer);
    fn_80062C38();
    Character_InitNewClubAndShotType(nPlayer);
}

void fn_8006752C(void) {
    GameMsg_Send(58);
}

void fn_80067550(int nPlayer) {
}

void fn_80067554(int nPlayer) {
}

void fn_80067558(int nPlayer) {
}

void fn_8006755C(int nPlayer) {
}

// The lowest of bits 0..23 set in the effects' flags, 0 if none.
int fn_80067560(void) {
    int i;

    for (i = 0; i < 24; i++) {
        if (gGameEffects.uFlags & (1 << i)) {
            return i;
        }
    }
    return 0;
}

void fn_80067608(void) {
    Mem_set(lbl_802811B8, 0, sizeof(SitDevData));
    lbl_802811B8->pE8 = NULL;
    lbl_802811B8->n13C = 0;
    Course_RegisterLoader(5, fn_800BB6DC);
    fn_800BB0C8();
}

void fn_8006765C(void) {
    if (lbl_802811B8->pD0 != NULL) {
        StaticMem_Free(lbl_802811B8->pD0);
    }
    StaticMem_Free(lbl_802811B8->pCC);
    StaticMem_Free(lbl_802811B8->pD4);
    lbl_80282208 = NULL;
}

void fn_800676AC(void) {
    lbl_80282210 = 0;
}

void fn_800676B8(void) {
    // port: SitDevFile.c defines the handler with the object's first word (the scripts) as its
    //       parameter; UStream calls it with the object. Same address on the GameCube.
    Stream_RegisterLoadChunkCallback('sscr', (void (*)(UStreamObject*))SitDev_LoadScripts);
}

void fn_800676E8(void) {
    Stream_UnregisterLoadChunkCallback('sscr');
}
