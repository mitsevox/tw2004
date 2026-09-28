// GameMode17.c (our name): game mode 17. Each player hits the targets in order (nNextTarget) with
// 5 balls; the surface a ball lands on can pay extra balls. Hitting every target wins the prize
// row's bonus plus 100 points per ball left.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

s32 gTargetToTargetSavedOptionsC = 4;                    // the options saved while the game runs
s32 gTargetToTargetShotPoints;                    // the points of the last shot
s32 gTargetToTargetShotBalls;                    // the extra balls of the last shot
s32 gTargetToTargetSavedWind;

void  GameModeSkillZoneTargetToTarget_Shutdown(void);
void  GameModeSkillZoneTargetToTarget_StartGamePreData(void);
u8    GameModeSkillZoneTargetToTarget_GoToPlayoff(u8 bCheck);
s32   GameModeSkillZoneTargetToTarget_GetHonors(int nPlayer);
void  GameModeSkillZoneTargetToTarget_EndGolferTurn(int nPlayer);
void  GameModeSkillZoneTargetToTarget_CheckShotAwards(int nPlayer);
void  GameModeSkillZoneTargetToTarget_SetupNextGolfer(void);
void  GameModeSkillZoneTargetToTarget_LoadHole(void);
void  GameModeSkillZoneTargetToTarget_RestartHole(void);
void  GameModeSkillZoneTargetToTarget_ClearPerHoleData(void);
void  GameModeSkillZoneTargetToTarget_UpdateSwingUI(int nPlayer);
u8    GameModeSkillZoneTargetToTarget_GameFinished(u8 bCheck);
void  GameModeSkillZoneTargetToTarget_BallOOB(int nPlayer);
u8    GameModeSkillZoneTargetToTarget_HoleFinished(int nPlayer, u8 bCheck);
void  GameModeSkillZoneTargetToTarget_GetIDScore(s32 nSurface, s32* pPoints, s32* pBalls);
u8    GameModeSkillZoneTargetToTarget_PickPrevTarget(int nPlayer);
u8    GameModeSkillZoneTargetToTarget_PickTarget(int nPlayer);
s32   GameModeSkillZoneTargetToTarget_GreenType(int nPlayer, int i);
void  GameModeSkillZoneTargetToTarget_EndGame(void);

// Game mode 17's setup (pfnInit, from GM_SetModeType): its hooks (its own target pickers, which
// keep the aim on the next target), no wind, no gimmes, no mulligans, no GameBreakers (b285), b28D
// set (the re-plan button calls PickTarget), pin set 0, the target list emptied (the hole's targets
// fill it as they load) and the current hole 0.
void GameModeSkillZoneTargetToTarget_Init(void) {
    gpGame->pfnInit = GameModeSkillZoneTargetToTarget_Init;
    gpGame->pfnShutdown = GameModeSkillZoneTargetToTarget_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeSkillZoneTargetToTarget_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkillZoneTargetToTarget_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkillZoneTargetToTarget_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkillZoneTargetToTarget_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkillZoneTargetToTarget_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameModeSkillZoneTargetToTarget_EndGolferTurn;
    gpGame->pfn244 = GameModeSkillZoneTargetToTarget_CheckShotAwards;
    gpGame->pfn1E4 = GameModeSkillZoneTargetToTarget_LoadHole;
    gpGame->pfn228 = GameModeSkillZoneTargetToTarget_UpdateSwingUI;
    gpGame->pfn224 = GameModeSkillZoneTargetToTarget_RestartHole;
    gpGame->pfn1EC = GameModeSkillZoneTargetToTarget_StartGamePreData;
    gpGame->pfn250 = GameModeSkillZoneTargetToTarget_BallOOB;
    gpGame->pfn264 = GameModeSkillZoneTargetToTarget_PickPrevTarget;
    gpGame->pfn258 = GameModeSkillZoneTargetToTarget_PickTarget;
    gpGame->pfn26C = GameModeSkillZoneTargetToTarget_GreenType;
    gpGame->pfnEndGame = GameModeSkillZoneTargetToTarget_EndGame;
    gpGame->b276 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b285 = 0;
    gpGame->b274 = 0;
    gpGame->b286 = 1;
    gpGame->b287 = 0;
    gpGame->b288 = 0;
    gpGame->b289 = 0;
    gpGame->b28A = 0;
    gpGame->bNoWind = 1;
    gpGame->bBumpObstructions = 0;
    gpGame->b28D = 1;
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    lbl_80282360 = 0;
    gSession.nPinSet = 0;
    GM_SetCurrentHole(0);
}

// Puts back the two options StartGamePreData changed for the game: options.nC and the wind.
void GameModeSkillZoneTargetToTarget_Shutdown(void) {
    gSession.options.nC = gTargetToTargetSavedOptionsC;
    gSession.options.nWind = gTargetToTargetSavedWind;
}

// As a round starts (pfn1EC, GM_InitModule_PreDataStream): saves options.nC and the wind setting
// (Shutdown puts them back) and sets them to 4 and 0, no wind.
void GameModeSkillZoneTargetToTarget_StartGamePreData(void) {
    gTargetToTargetSavedOptionsC = gSession.options.nC;
    gTargetToTargetSavedWind = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
}

// Never a playoff: returns 0.
u8 GameModeSkillZoneTargetToTarget_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): player 0 while nobody has a stroke on the hole; after that the
// next player in turn after the current golfer (lbl_80282278) who is not nPlayer and still has
// balls (nDC0); 5 when there is none.
s32 GameModeSkillZoneTargetToTarget_GetHonors(int nPlayer) {
    int i;
    int n;
    u8 bFirst = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] != 0) {
            bFirst = 0;
        }
    }
    if (bFirst) {
        return 0;
    }
    n = lbl_80282278;
    for (i = 0; i < 5; i++) {
        n++;
        if (n >= gNumPlayersSetUp) {
            n = 0;
        }
        if (n != nPlayer && gPlayers[n].nDC0 != 0) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (with in-flight
// replays on, gReplayData.bF10, the replay's saved ball is copied back instead) and one ball fewer
// is left (nDC0).
void GameModeSkillZoneTargetToTarget_EndGolferTurn(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        Physics_InitBall(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nDC0--;
}

// Scores a shot once the ball stops (pfn244; BallOOB too). The landing surface's
// gEarningsTable.aMini row gives points and extra balls (GetIDScore). A target hit short of the
// drive line (GameModeSkillZoneBase_IsLongDrive) counts only when it is the player's next target
// (nNextTarget): the next target moves on (back to 0 after the last), the streak (nE90, best nE8C),
// its hit count (nDE4) and aDC4[3] go up; while every target has been hit, the all-targets prize
// plus 100 per ball left is paid instead of the points, with comment 0x2B or 0x2C the first time
// (bE9E). The comment says how many targets are left (1 to 5), else how far along the player is; a
// bullseye is counted (nDE0), plays the bullseye sound and ball effect and gets comment 0x1D. Any
// other target pays nothing, no balls, ends the streak (message 0xD0, comment 0xB or 0xD). A target
// surface past the drive line pays nothing. Points get the earnings modifiers (no multipliers in
// this mode); a loss (comment 0 or 0x4E) never takes the winnings (nDD8) below 0, and the result
// goes to the HUD (message 0x33) unless in a replay. Extra balls are added to nDC0 and counted in
// aDC4[1].
void GameModeSkillZoneTargetToTarget_CheckShotAwards(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    s32 bDone;
    f32 fLength;
    s32 nMult;
    Ball* pBall;
    nMsg = -1;
    bDone = 0;
    nSurface = gPlayers[nPlayer].ball.nSurface;
    fLength = fn_800D0550(nPlayer);
    GameModeSkillZoneTargetToTarget_GetIDScore(nSurface, &gTargetToTargetShotPoints,
                                               &gTargetToTargetShotBalls);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        nTarget = GameModeSkillZoneBase_GetGreenIndexHit(nPlayer);
        GameModeSkillZoneBase_GetBullsEyeColor(nSurface);
        if (nTarget == gPlayers[nPlayer].nNextTarget) {
            gPlayers[nPlayer].nNextTarget++;
            if (gPlayers[nPlayer].nNextTarget >= lbl_80282360) {
                gPlayers[nPlayer].nNextTarget = 0;
            }
            gPlayers[nPlayer].nE90++;
            if (gPlayers[nPlayer].nE90 > gPlayers[nPlayer].nE8C) {
                gPlayers[nPlayer].nE8C = gPlayers[nPlayer].nE90;
            }
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (lbl_80282360 == GameModeSkillZoneBase_CountGreensHit(nPlayer)) {
                gTargetToTargetShotPoints = GameModeSkillZoneBase_GetHitAllTargetsBonus();
                gTargetToTargetShotPoints += gPlayers[nPlayer].nDC0 * 100;
                gTargetToTargetShotPoints = GM_Earnings_ComputeBonusModifiers(gTargetToTargetShotPoints, nPlayer, 1, 1, 1, 0);
                gTargetToTargetShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTargetToTargetShotPoints, nPlayer, 0);
                GM_Earnings_AwardMoney(nPlayer, gTargetToTargetShotPoints, 0);
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                bDone = 1;
                gPlayers[nPlayer].nDD8 += gTargetToTargetShotPoints;
                GameMsg_Send5Ints(0x33, gTargetToTargetShotPoints, 0, 0, 0xCA, 1);
                gTargetToTargetShotPoints = 0;
                if ((s8)gPlayers[nPlayer].bE9E == 0) {
                    gPlayers[nPlayer].bE9E = 1;
                    if (!(Misc_RandFunc(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            }
            if (nMsg == -1) {
                switch (lbl_80282360 - GameModeSkillZoneBase_CountGreensHit(nPlayer)) {
                case 1:
                    if (Misc_RandFunc(0) & 1) {
                        nMsg = 0x26;
                    } else {
                        nMsg = 0x43;
                    }
                    break;
                case 2:
                    if (Misc_RandFunc(0) & 1) {
                        nMsg = 0x27;
                    } else {
                        nMsg = 0x44;
                    }
                    break;
                case 3:
                    if (Misc_RandFunc(0) & 1) {
                        nMsg = 0x28;
                    } else {
                        nMsg = 0x45;
                    }
                    break;
                case 4:
                    nMsg = 0x29;
                    break;
                case 5:
                    nMsg = 0x2A;
                    break;
                default:
                    if (GameModeSkillZoneBase_CountGreensHit(nPlayer) <= lbl_80282360 / 4) {
                        if (gTargetToTargetShotBalls == 1) {
                            if (Misc_RandFunc(0) & 1) {
                                nMsg = 0x36;
                            } else {
                                nMsg = 0x37;
                            }
                        } else {
                            switch (Misc_RandFunc(0) & 3) {
                            case 0:
                                nMsg = 0x4F;
                                break;
                            case 1:
                                nMsg = 0x50;
                                break;
                            case 2:
                                nMsg = 0x51;
                                break;
                            case 3:
                                nMsg = 0x4F;
                                break;
                            }
                        }
                    } else {
                        switch (Misc_RandFunc(0) & 7) {
                        case 0:
                        case 1:
                            nMsg = 0x46;
                            break;
                        case 2:
                        case 3:
                            nMsg = 0x47;
                            break;
                        case 4:
                            nMsg = 0x32;
                            break;
                        case 5:
                            nMsg = 0x31;
                            break;
                        case 6:
                            nMsg = 0x12;
                            break;
                        case 7:
                            nMsg = 0x11;
                            break;
                        }
                    }
                    break;
                }
            }
            if (nSurface == 0x85 || nSurface == 0x88 || nSurface == 0x8C) {
                gPlayers[nPlayer].nDE0++;
                Gaud_BullsEye();
                pBall = &gPlayers[nPlayer].ball;
                fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                nMsg = 0x1D;
                nMult = gPlayers[nPlayer].nDBC;
                if (nMult > 1) {
                    fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                }
            } else {
                Gaud_ScoreInRing();
                nMult = gPlayers[nPlayer].nDBC;
                if (nMult > 1) {
                    fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                }
            }
            if (bDone == 0) {
                GameMsg_Send5Ints(0x33, 0, 0, 0, nSurface, 1);
            }
        } else {
            GameMsg_Send5Ints(0x33, 0, 0, 0, 0xD0, 1);
            gTargetToTargetShotPoints = 0;
            gPlayers[nPlayer].nE90 = 0;
            gTargetToTargetShotBalls = 0;
            if (Misc_RandFunc(0) & 1) {
                nMsg = 0xB;
            } else {
                nMsg = 0xD;
            }
        }
    } else {
        gPlayers[nPlayer].nE90 = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        gTargetToTargetShotPoints = 0;
        gTargetToTargetShotBalls = 0;
    }
    if (gTargetToTargetShotPoints != 0) {
        if (gTargetToTargetShotPoints > 0) {
            gTargetToTargetShotPoints = GM_Earnings_ComputeBonusModifiers(gTargetToTargetShotPoints, nPlayer, 1, 1, 1, 0);
            gTargetToTargetShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTargetToTargetShotPoints, nPlayer, 0);
        } else if (nMsg == -1) {
            if (!(Misc_RandFunc(0) & 1)) {
                nMsg = 0;
            } else {
                nMsg = 0x4E;
            }
        }
        if (gTargetToTargetShotPoints + gPlayers[nPlayer].nDD8 < 0) {
            GM_Earnings_AwardMoney(nPlayer, -gPlayers[nPlayer].nDD8, 0);
        } else {
            GM_Earnings_AwardMoney(nPlayer, gTargetToTargetShotPoints, 0);
        }
        gPlayers[nPlayer].nDD8 += gTargetToTargetShotPoints;
        if (gPlayers[nPlayer].nDD8 < 0) {
            gPlayers[nPlayer].nDD8 = 0;
        } else if (!gSession.bReplay) {
            if (GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
                GameMsg_Send5Ints(0x33, gTargetToTargetShotPoints, 0, 0, 0xD6, 1);
            } else {
                GameMsg_Send5Ints(0x33, gTargetToTargetShotPoints, 0, 0, nSurface, 1);
            }
            if (gTargetToTargetShotPoints > 0 && nSurface < 0x85) {
                Gaud_MoneyAward();
            }
        }
    }
    if (gTargetToTargetShotBalls != 0) {
        gPlayers[nPlayer].nDC0 += gTargetToTargetShotBalls;
        gPlayers[nPlayer].aDC4[1] += gTargetToTargetShotBalls;
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_StartComment(nMsg);
    }
    GameModeSkillZoneBase_PostShotAwards1(nPlayer);
    GameModeSkillZoneBase_PostShotAwards2(nPlayer);
}

// Before each shot (pfnSetupNextGolfer): per-shot data cleared (ClearPerShotData) and stroke play's
// golfer order (GameModeStroke_SetupNextGolfer); then the golfer about to play (GS_PRE_SHOT), when
// not aimed at their next target (nNextTarget), is set up for it (SetCup_AlignGolfer).
void GameModeSkillZoneTargetToTarget_SetupNextGolfer(void) {
    int i;
    GameModeSkillZoneBase_ClearPerShotData();
    GameModeStroke_SetupNextGolfer();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            if (PLAYER(i)->nTarget != PLAYER(i)->nNextTarget) {
                GameModeSkillZoneBase_SetCup_AlignGolfer(i, PLAYER(i)->nNextTarget);
            }
        }
    }
}

// Hole start (pfn1E4): the targets sorted nearest the tee first (SortCupsByDistanceFromTee, so they
// are played nearest first), then ClearPerHoleData.
void GameModeSkillZoneTargetToTarget_LoadHole(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    GameModeSkillZoneTargetToTarget_ClearPerHoleData();
}

// The hole restarts (pfn224, GM_RestartHole): the per-hole data cleared with 5 balls each and
// target 0 next again (ClearPerHoleData), and player 0's default aim.
void GameModeSkillZoneTargetToTarget_RestartHole(void) {
    GameModeSkillZoneTargetToTarget_ClearPerHoleData();
    AI_DefaultTarget(0);
}

// The shared per-hole clear (GameModeSkillZoneBase_ClearPerHoleData), then all five players get 5
// balls (nDC0) and target 0 as their next (nNextTarget).
void GameModeSkillZoneTargetToTarget_ClearPerHoleData(void) {
    int i;
    GameModeSkillZoneBase_ClearPerHoleData();
    for (i = 0; i < 5; i++) {
        gPlayers[i].nNextTarget = 0;
        gPlayers[i].nDC0 = 5;
    }
}

// Every frame of the swing state (pfn228): once the swing has begun (SwingData.nState not
// SW_IDLE_SWING), UI message 0x36 (no value) is sent.
void GameModeSkillZoneTargetToTarget_UpdateSwingUI(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

// The game is over once its one hole is: always 1.
u8 GameModeSkillZoneTargetToTarget_GameFinished(u8 bCheck) {
    return 1;
}

// The ball went out of bounds (pfn250): the shot is scored as usual (CheckShotAwards).
void GameModeSkillZoneTargetToTarget_BallOOB(int nPlayer) {
    GameModeSkillZoneTargetToTarget_CheckShotAwards(nPlayer);
}

// The hole (and so the game) is over once player 0 has hit every target, or once no player has a
// ball left (nDC0); nPlayer and bCheck are not used.
u8 GameModeSkillZoneTargetToTarget_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    if (lbl_80282360 == GameModeSkillZoneBase_CountGreensHit(0)) {
        return 1;
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nDC0 != 0) {
            return 0;
        }
    }
    return 1;
}

// The points (mode 17's column, nC) and extra balls (n18) of the gEarningsTable.aMini row for
// surface nSurface; both 0 when there is none (the last matching of the 20 rows wins).
void GameModeSkillZoneTargetToTarget_GetIDScore(s32 nSurface, s32* pPoints, s32* pBalls) {
    int i;
    *pPoints = 0;
    *pBalls = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].nC;
            *pBalls = gEarningsTable.aMini[i].n18;
        }
    }
}

// The previous-target button (pfn264): the player stays aimed at their next target in order
// (nNextTarget, SetCup); always returns 1.
u8 GameModeSkillZoneTargetToTarget_PickPrevTarget(int nPlayer) {
    GameModeSkillZoneBase_SetCup(nPlayer, gPlayers[nPlayer].nNextTarget);
    return 1;
}

// The next-target (re-plan) button (pfn258): the player stays aimed at their next target in order
// (nNextTarget, SetCup); always returns 1.
u8 GameModeSkillZoneTargetToTarget_PickTarget(int nPlayer) {
    GameModeSkillZoneBase_SetCup(nPlayer, gPlayers[nPlayer].nNextTarget);
    return 1;
}

// The points the last shot earned (gTargetToTargetShotPoints), whoever nPlayer is.
s32 GameModeSkillZoneTargetToTarget_GetShotEarned(s32 a) {
    return gTargetToTargetShotPoints;
}

// Which marker model target nTarget shows for the player (pfn26C, GoDynObj.c): 0 for their next
// target in order (nNextTarget), 1 for every other.
s32 GameModeSkillZoneTargetToTarget_GreenType(int nPlayer, int i) {
    return i != gPlayers[nPlayer].nNextTarget;
}

// The extra balls the last shot earned (gTargetToTargetShotBalls), whoever nPlayer is.
s32 GameModeSkillZoneTargetToTarget_GetExtraBallsEarned(s32 a) {
    return gTargetToTargetShotBalls;
}

// End of the game (pfnEndGame): the game counts as won in the bio (EASBio_SetCurrentGameWon).
void GameModeSkillZoneTargetToTarget_EndGame(void) {
    EASBio_SetCurrentGameWon(1);
}
