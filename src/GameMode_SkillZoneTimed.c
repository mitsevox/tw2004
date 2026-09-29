// GameMode_SkillZoneTimed.c (TW07's name, GameMode_SkillZoneTimed.cpp there, the same methods in
// the same order): game mode 13, the timed target game, one hole. Each player starts with 90
// seconds (5400 frames, in n290 for the hole) and plays in turn while they have time. The first hit
// of a target pays 100 plus its points and adds its time plus 5 seconds; each earlier hit on the
// same target scales both by 0.75, and a target pays at most 4 times; the last target not yet hit
// pays the all-targets prize instead. A shot can get a random x2, x3 or x5 multiplier, and bonus
// objects hit on the way raise a second points multiplier. A target surface past the tee set's
// drive line counts as a drive, paying only for a new longest one. The game ends when everyone's
// time is up. The shared target-game code is GameMode_SkillZoneBase.c.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// Mode 13's state; only this file uses it. The .sbss ones are defined last address first (the
// compiler lays a file's .sbss out last definition first).
s32 gTimedSavedWeather = 4;    // options.nWeather before the game (StartGamePreData; Shutdown restores it)
s32 gTimedShotPoints;           // the points of the last shot (GetShotEarned)
s32 gTimedShotSeconds;          // the seconds added by the last shot (GetTimeEarned)
s32 gTimedBonusMultiplier;      // the points multiplier from bonus objects: 1 each shot, raised by
                                //   CollisionActor (GetDriveMultiplier)
s32 gTimedSavedWind;            // options.nWind from before the game (StartGamePreData; Shutdown puts
                                //   it back)

void  Gaud_StartShotClock(void);

void  GameModeSkillZoneTimed_Shutdown(void);
void  GameModeSkillZoneTimed_StartGamePreData(void);
u8    GameModeSkillZoneTimed_GoToPlayoff(u8 bCheck);
s32   GameModeSkillZoneTimed_GetHonors(int nPlayer);
void  GameModeSkillZoneTimed_EndGolferTurn(int nPlayer);
void  GameModeSkillZoneTimed_CheckShotAwards(int nPlayer);
void  GameModeSkillZoneTimed_GetIDScore(s32 nSurface, s32* pPoints, s32* pTime, s32* pBalls);
void  GameModeSkillZoneTimed_SetupNextGolfer(void);
void  GameModeSkillZoneTimed_LoadHole(void);
void  GameModeSkillZoneTimed_InitialFlyByDone(int nPlayer);
void  GameModeSkillZoneTimed_RestartHole(void);
void  GameModeSkillZoneTimed_ClearPerHoleData(void);
void  GameModeSkillZoneTimed_UpdateSwingUI(int nPlayer);
u8    GameModeSkillZoneTimed_GameFinished(u8 bCheck);
void  GameModeSkillZoneTimed_TenSecWarning(void);
void  GameModeSkillZoneTimed_BallOOB(int nPlayer);
void  GameModeSkillZoneTimed_Mulligan(int nPlayer);
void  GameModeSkillZoneTimed_SetTimer(int nPlayer, int nTime);
u8    GameModeSkillZoneTimed_HoleFinished(int nPlayer, u8 bCheck);
void  GameModeSkillZoneTimed_CollisionActor(int nPlayer, int nId);
s32   GameModeSkillZoneTimed_GreenType(int nPlayer, int nTarget);
void  GameModeSkillZoneTimed_EndGame(void);

// Game mode 13's setup (pfnInit, from GM_SetModeType): its hooks (the target list ones from
// GameMode_SkillZoneBase.c), one view, no wind, no gimmes, any number of mulligans (nMulligans 1), b28D set
// (the re-plan button picks the next target), the current hole 0, pin set 0 and the target list
// emptied (the hole's targets fill it as they load).
void GameModeSkillZoneTimed_Init(void) {
    gpGame->pfnInit = GameModeSkillZoneTimed_Init;
    gpGame->pfnShutdown = GameModeSkillZoneTimed_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeSkillZoneTimed_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkillZoneTimed_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkillZoneTimed_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkillZoneTimed_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkillZoneTimed_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameModeSkillZoneTimed_EndGolferTurn;
    gpGame->pfnCheckShotAwards = GameModeSkillZoneTimed_CheckShotAwards;
    gpGame->pfnLoadHole = GameModeSkillZoneTimed_LoadHole;
    gpGame->pfnSwingUpdate = GameModeSkillZoneTimed_UpdateSwingUI;
    gpGame->pfnInitialFlyByDone = GameModeSkillZoneTimed_InitialFlyByDone;
    gpGame->pfnRestartHole = GameModeSkillZoneTimed_RestartHole;
    gpGame->pfnStartGamePreData = GameModeSkillZoneTimed_StartGamePreData;
    gpGame->pfnBallOOB = GameModeSkillZoneTimed_BallOOB;
    gpGame->pfnMulligan = GameModeSkillZoneTimed_Mulligan;
    gpGame->pfnPickPrevTarget = GameModeSkillZoneBase_PickPrevTarget;
    gpGame->pfnPickTarget = GameModeSkillZoneBase_PickTarget;
    gpGame->pfnSetTimer = GameModeSkillZoneTimed_SetTimer;
    gpGame->pfnCollisionActor = GameModeSkillZoneTimed_CollisionActor;
    gpGame->pfnGreenType = GameModeSkillZoneTimed_GreenType;
    gpGame->pfnEndGame = GameModeSkillZoneTimed_EndGame;
    gpGame->b276 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b280 = 0;
    gpGame->b271 = 0;
    gpGame->b281 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->bAllowGameBreakers = 0;
    gpGame->b274 = 0;
    gpGame->b286 = 1;
    gpGame->b287 = 0;
    gpGame->b288 = 0;
    gpGame->b289 = 0;
    gpGame->b28A = 0;
    gpGame->bNoWind = 1;
    gpGame->bBumpObstructions = 0;
    gpGame->b28D = 1;
    gpGame->nScoringType = 0;
    gpGame->nMulligans = 1;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    gSkillZoneNumCups = 0;
    gSession.nSplitScreen = 0;
    gSession.nPinSet = 0;
}

// Puts back the two options StartGamePreData changed for the game: options.nWeather and the wind.
void GameModeSkillZoneTimed_Shutdown(void) {
    gSession.options.nWeather = gTimedSavedWeather;
    gSession.options.nWind = gTimedSavedWind;
}

// As a round starts (pfnStartGamePreData, GM_InitModule_PreDataStream): saves options.nWeather and
// the wind setting (Shutdown puts them back) and sets them to 4 and 0, no wind.
void GameModeSkillZoneTimed_StartGamePreData(void) {
    gTimedSavedWeather = gSession.options.nWeather;
    gTimedSavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
    gSession.options.nWind = 0;
}

// Never a playoff: returns 0.
u8 GameModeSkillZoneTimed_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): player 0 while nobody has a stroke on the hole; after that the
// next player in turn after the current golfer (lbl_80282278) who is not nPlayer and still has time
// (n290); 5 when there is none.
s32 GameModeSkillZoneTimed_GetHonors(int nPlayer) {
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
        if (n != nPlayer && gPlayers[n].n290[Game_CurHoleIndex()] != 0) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn (pfnEndGolferTurn): message 18 (PlayNow_SendMessage18); the ball goes back
// on the player's tee (with in-flight replays on, gReplayData.bF10, the replay's saved ball is
// copied back instead); the shots taken (nBalls) are counted, and aSkillZoneStats[0] when the shot
// had a multiplier.
void GameModeSkillZoneTimed_EndGolferTurn(int nPlayer) {
    PlayNow_SendMessage18(nPlayer);
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        Physics_InitBall(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nBalls++;
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].aSkillZoneStats[0]++;
    }
}

// Scores a shot once the ball stops (pfnCheckShotAwards from GM_Earnings_PayShotGoals; BallOOB
// too). The landing surface's gEarningsTable.aMini row gives points and seconds (GetIDScore). On a
// target short of the drive line (GameModeSkillZoneBase_IsLongDrive): a first hit pays 100 plus its
// points, times the shot multiplier (nDBC) and the hole's target factor (ScaleTargetPoints), and
// adds its seconds plus 5; the last target not yet hit pays the all-targets prize instead, with no
// time. A target hit before pays its points and seconds scaled by 0.75 per earlier hit; after 4
// hits it pays nothing (Gaud_TargetClosedOut). A target surface past the drive line is a drive: a
// new longest one (nSkillZoneLongestDrive) pays as a surface, any other nothing. Those points are
// multiplied by nDBC, the scale, the target factor (targets only) and the bonus multiplier
// (gTimedBonusMultiplier), then the earnings modifiers; a loss never takes the winnings
// (nSkillZonePoints) below 0. Added seconds go to the HUD clock as the player's new total (n290
// plus seconds * 60 frames; n290 itself is set through SetTimer, from the UI) and are summed in
// aSkillZoneStats[4]; the ticking stops when the clock climbs past 10 seconds and starts when time
// added to an empty one is 10 seconds or less. If time ran out in flight (bE9D), time earned saves
// the player, else comment 0x14. Target streaks (nHitStreak, best nBestHitStreak), bullseyes
// (nBullseyes) and target hits (aSkillZoneStats[3]) are counted.
void GameModeSkillZoneTimed_CheckShotAwards(int nPlayer) {
    s32 nBalls;
    s32 nSurface;
    int nTarget;
    s32 nMsg;
    s32 bTime;
    s32 nAdded;
    s32 nAddedFrames;
    f32 fLength;
    f32 fScale;
    s32 nMult;
    Ball* pBall;
    long j;
    s32 nHits;
    nMsg = -1;
    nSurface = gPlayers[nPlayer].ball.nSurface;
    GameModeSkillZoneTimed_GetIDScore(nSurface, &gTimedShotPoints, &gTimedShotSeconds, &nBalls);
    fScale = 1.0f;
    nAdded = 0;
    bTime = gTimedShotSeconds != 0;
    fLength = GameAnalysis_GetCurrentBallFlightDistance(nPlayer);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        nTarget = GameModeSkillZoneBase_GetGreenIndexHit(nPlayer);
        gPlayers[nPlayer].nHitStreak++;
        if (gPlayers[nPlayer].nHitStreak > gPlayers[nPlayer].nBestHitStreak) {
            gPlayers[nPlayer].nBestHitStreak = gPlayers[nPlayer].nHitStreak;
        }
        nHits = gPlayers[nPlayer].nTargetHits[nTarget];
        for (j = 0; j < gPlayers[nPlayer].nTargetHits[nTarget]; j++) {
            fScale *= 0.75f;
        }
        if (nHits == 0) {
            if (gSkillZoneNumCups - 1 == GameModeSkillZoneBase_CountGreensHit(nPlayer)) {
                fScale = 1.0f;
                gTimedShotPoints = GameModeSkillZoneBase_GetHitAllTargetsBonus();
                gTimedShotPoints = GM_Earnings_ComputeBonusModifiers(gTimedShotPoints, nPlayer, 1, 1, 1, 0);
                gTimedShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTimedShotPoints, nPlayer, 0);
                GM_Earnings_AwardMoney(nPlayer, gTimedShotPoints, 0);
                gPlayers[nPlayer].nHoleHits[Game_CurHoleIndex()]++;
                gTimedShotSeconds = 0;
                gPlayers[nPlayer].nSkillZonePoints += gTimedShotPoints;
                GameMsg_Send5Ints(0x33, gTimedShotPoints, 0, 0, 0xCA, 1);
                gTimedShotPoints = 0;
                gTimedShotSeconds = 0;
                if ((s8)gPlayers[nPlayer].bAllTargetsHit == 0) {
                    gPlayers[nPlayer].bAllTargetsHit = 1;
                    if (!(Misc_RandFunc(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            } else {
                fScale = 1.0f;
                gTimedShotPoints += 100;
                gTimedShotPoints *= gPlayers[nPlayer].nDBC;
                gTimedShotPoints = GameModeSkillZoneBase_ScaleTargetPoints(gTimedShotPoints, nTarget);
                gTimedShotPoints = GM_Earnings_ComputeBonusModifiers(gTimedShotPoints, nPlayer, 1, 1, 1, 0);
                gTimedShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTimedShotPoints, nPlayer, 0);
                GM_Earnings_AwardMoney(nPlayer, gTimedShotPoints, 0);
                gPlayers[nPlayer].aShotSurfaces[gPlayers[nPlayer].nShotSurfaceCount] = nSurface;
                gPlayers[nPlayer].nShotSurfaceCount++;
                gPlayers[nPlayer].nHoleHits[Game_CurHoleIndex()]++;
                gPlayers[nPlayer].nSkillZonePoints += gTimedShotPoints;
                gTimedShotSeconds += 5;
                gPlayers[nPlayer].aSkillZoneStats[4] += gTimedShotSeconds * 60;
                PlayNow_SendMessage18(nPlayer);
                GameModeSkillZoneTimed_SetHudClock(gPlayers[nPlayer].n290[Game_CurHoleIndex()]
                                                   + gTimedShotSeconds
                                                   * 60);
                nAdded = gTimedShotSeconds;
                GameMsg_Send5Ints(0x33, gTimedShotPoints, 0, 0, 0xC9, 1);
                GameMsg_Send3Ints(0x34, gTimedShotSeconds * 60, 0, 0);
                gTimedShotPoints = 0;
                gTimedShotSeconds = 0;
                if (gPlayers[nPlayer].nDBC > 1) {
                    nMsg = 0x31;
                } else {
                    nMsg = 0x22;
                }
            }
        } else {
            switch (GameModeSkillZoneBase_GetBullsEyeColor(nSurface)) {
            case 0:
                nMsg = 0x1D;
                break;
            case 1:
                nMsg = 0x1E;
                break;
            case 2:
                nMsg = 0x1F;
                break;
            case 3:
                nMsg = 0x21;
                break;
            case 4:
                nMsg = 0x20;
                break;
            }
        }
        if (gPlayers[nPlayer].nTargetHits[nTarget] > 3) {
            fScale = 0.0f;
            gTimedShotPoints = 0;
            gTimedShotSeconds = 0;
            GameMsg_Send5Ints(0x33, 0, 0, 0, 0xC8, 1);
            Gaud_TargetClosedOut();
            nMsg = 2;
        } else {
            gPlayers[nPlayer].nTargetHits[nTarget]++;
            gPlayers[nPlayer].aSkillZoneStats[3]++;
            if (nSurface == 0x85 || nSurface == 0x88 || nSurface == 0x8C) {
                gPlayers[nPlayer].nBullseyes++;
                Gaud_BullsEye();
                pBall = &gPlayers[nPlayer].ball;
                fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
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
        }
    } else {
        gPlayers[nPlayer].nHitStreak = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        if (fLength > gPlayers[nPlayer].nSkillZoneLongestDrive) {
            gPlayers[nPlayer].nSkillZoneLongestDrive = fLength;
            switch (Misc_RandFunc(0) & 3) {
            case 0:
                nMsg = 0x31;
                break;
            case 1:
                nMsg = 0x32;
                break;
            default:
                nMsg = 0x33;
                break;
            }
        } else {
            fScale = 0.0f;
            gTimedShotPoints = 0;
            gTimedShotSeconds = 0;
            GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCB, 1);
            switch (Misc_RandFunc(0) & 3) {
            case 0:
                nMsg = 0x2F;
                break;
            case 1:
                nMsg = 1;
                break;
            default:
                nMsg = 0x30;
                break;
            }
        }
    }
    if (gTimedShotSeconds != 0) {
        gTimedShotSeconds = gTimedShotSeconds * fScale;
        gPlayers[nPlayer].aSkillZoneStats[4] += gTimedShotSeconds * 60;
        PlayNow_SendMessage18(nPlayer);
        GameModeSkillZoneTimed_SetHudClock(gPlayers[nPlayer].n290[Game_CurHoleIndex()] + gTimedShotSeconds
                                           * 60);
        nAdded = gTimedShotSeconds;
        if (!gSession.bReplay) {
            GameMsg_Send3Ints(0x34, gTimedShotSeconds * 60, 0, 0);
        }
    }
    if (gTimedShotPoints != 0) {
        if (gTimedShotPoints > 0) {
            gTimedShotPoints = (f32)gTimedShotPoints * gPlayers[nPlayer].nDBC;
            gTimedShotPoints = gTimedShotPoints * fScale;
            if (nSurface >= 0x85 && nSurface <= 0x90
                && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
                gTimedShotPoints = GameModeSkillZoneBase_ScaleTargetPoints(gTimedShotPoints, nTarget);
                switch (gPlayers[nPlayer].nDBC) {
                case 2:
                    nMsg = 0x32;
                    break;
                case 3:
                    nMsg = 0x33;
                    break;
                case 4:
                    break;
                case 5:
                    nMsg = 0x31;
                    break;
                }
            }
            gTimedShotPoints = (f32)(gTimedShotPoints * gTimedBonusMultiplier);
            gTimedShotPoints = GM_Earnings_ComputeBonusModifiers(gTimedShotPoints, nPlayer, 1, 1, 1, 0);
            gTimedShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTimedShotPoints, nPlayer, 0);
        } else if (!(Misc_RandFunc(0) & 1)) {
            GameModeSkillZoneBase_StartComment(0);
        } else {
            GameModeSkillZoneBase_StartComment(0x4E);
        }
        gPlayers[nPlayer].aShotSurfaces[gPlayers[nPlayer].nShotSurfaceCount] = nSurface;
        gPlayers[nPlayer].nShotSurfaceCount++;
        gPlayers[nPlayer].nHoleHits[Game_CurHoleIndex()]++;
        if (gTimedShotPoints + gPlayers[nPlayer].nSkillZonePoints < 0) {
            GM_Earnings_AwardMoney(nPlayer, -gPlayers[nPlayer].nSkillZonePoints, 0);
        } else {
            GM_Earnings_AwardMoney(nPlayer, gTimedShotPoints, 0);
        }
        gPlayers[nPlayer].nSkillZonePoints += gTimedShotPoints;
        if (gPlayers[nPlayer].nSkillZonePoints < 0) {
            gPlayers[nPlayer].nSkillZonePoints = 0;
        } else if (!gSession.bReplay) {
            if (GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
                GameMsg_Send5Ints(0x33, gTimedShotPoints, 0, 0, 0xD6, 1);
            } else {
                GameMsg_Send5Ints(0x33, gTimedShotPoints, 0, 0, nSurface, 1);
            }
            if (gTimedShotPoints > 0 && nSurface < 0x85) {
                Gaud_MoneyAward();
            }
        }
    }
    if ((s8)gPlayers[nPlayer].bE9D) {
        if (bTime) {
            gPlayers[nPlayer].bE9D = 0;
            switch (Misc_RandFunc(0) & 3) {
            case 0:
                nMsg = 0x38;
                break;
            case 1:
                nMsg = 0x39;
                break;
            default:
                nMsg = 0x34;
                break;
            }
        } else {
            nMsg = 0x14;
        }
    }
    if (fLength > gPlayers[nPlayer].nSkillZoneLongestDrive
        && !GM_IsBallOOB(nPlayer, &gPlayers[nPlayer].ball)) {
        gPlayers[nPlayer].nSkillZoneLongestDrive = fLength;
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_StartComment(nMsg);
    }
    if (gPlayers[nPlayer].n290[Game_CurHoleIndex()] + (nAddedFrames = nAdded * 60) > 600 &&
        gPlayers[nPlayer].n290[Game_CurHoleIndex()] <= 600) {
        Gaud_StopShotClock();
    }
    if (gPlayers[nPlayer].n290[Game_CurHoleIndex()] <= 0 && nAddedFrames <= 600 && nAdded > 0) {
        Gaud_StartShotClock();
    }
    GameModeSkillZoneBase_PostShotAwards1(nPlayer);
    GameModeSkillZoneBase_PostShotAwards2(nPlayer);
}

// The points (n4), seconds (n14) and fourth value (n18; mode 13 does not use it) of the
// gEarningsTable.aMini row for surface nSurface; all 0 when there is none (the last matching of the
// 20 rows wins).
void GameModeSkillZoneTimed_GetIDScore(s32 nSurface, s32* pPoints, s32* pTime, s32* pBalls) {
    int i;
    *pPoints = 0;
    *pTime = 0;
    *pBalls = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].n4;
            *pTime = gEarningsTable.aMini[i].n14;
            *pBalls = gEarningsTable.aMini[i].n18;
        }
    }
}

// Before each shot (pfnSetupNextGolfer): per-shot data cleared (ClearPerShotData) and stroke play's
// golfer order (GameModeStroke_SetupNextGolfer); then for the golfer about to play (GS_PRE_SHOT) a
// chance of a shot multiplier (SetupBonusBall), their time on the HUD clock and, before their first
// shot, their target set up (SetCup_AlignGolfer). The bonus multiplier (gTimedBonusMultiplier) goes back to
// 1.
void GameModeSkillZoneTimed_SetupNextGolfer(void) {
    int i;
    GameModeSkillZoneBase_ClearPerShotData();
    GameModeStroke_SetupNextGolfer();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            GameModeSkillZoneBase_SetupBonusBall(i);
            GameModeSkillZoneTimed_SetHudClock(PLAYER(i)->n290[Game_CurHoleIndex()]);
            if (PLAYER(i)->nBalls == 0) {
                GameModeSkillZoneBase_SetCup_AlignGolfer(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
    gTimedBonusMultiplier = 1;
}

// Hole start (pfnLoadHole): the targets sorted nearest the tee first (SortCupsByDistanceFromTee),
// then ClearPerHoleData.
void GameModeSkillZoneTimed_LoadHole(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    GameModeSkillZoneTimed_ClearPerHoleData();
}

// After the hole flyover (pfnInitialFlyByDone): the HUD clock and all five players' time for the
// hole are set to 90 seconds (5400 frames).
void GameModeSkillZoneTimed_InitialFlyByDone(int nPlayer) {
    int i;
    GameModeSkillZoneTimed_SetHudClock(5400);
    for (i = 0; i < 5; i++) {
        PLAYER(i)->n290[Game_CurHoleIndex()] = 5400;
    }
}

// The hole restarts (pfnRestartHole, GM_RestartHole): the per-hole data cleared with everyone's
// time back to 90 seconds (ClearPerHoleData), player 0's default aim, the HUD clock hidden (-1) and
// the ticking stopped.
void GameModeSkillZoneTimed_RestartHole(void) {
    GameModeSkillZoneTimed_ClearPerHoleData();
    AI_DefaultTarget(0);
    GameModeSkillZoneTimed_SetHudClock(-1);
    Gaud_StopShotClock();
}

// The shared per-hole clear (GameModeSkillZoneBase_ClearPerHoleData), then all five players' time
// for the hole set to 90 seconds (5400 frames).
void GameModeSkillZoneTimed_ClearPerHoleData(void) {
    int i;
    GameModeSkillZoneBase_ClearPerHoleData();
    for (i = 0; i < 5; i++) {
        PLAYER(i)->n290[Game_CurHoleIndex()] = 5400;
    }
}

// Every frame of the swing state (pfnSwingUpdate): once the swing has begun (SwingData.nState not
// SW_IDLE_SWING), UI message 0x36 (no value) is sent.
void GameModeSkillZoneTimed_UpdateSwingUI(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

// The game is over once its one hole is: always 1.
u8 GameModeSkillZoneTimed_GameFinished(u8 bCheck) {
    return 1;
}

// The points the last shot earned (gTimedShotPoints), whoever nPlayer is.
s32 GameModeSkillZoneTimed_GetShotEarned(s32 nPlayer) {
    return gTimedShotPoints;
}

// The seconds the last shot added (gTimedShotSeconds), whoever nPlayer is.
s32 GameModeSkillZoneTimed_GetTimeEarned(s32 nPlayer) {
    return gTimedShotSeconds;
}

// Ten seconds left (a UI command, GameUICommands.c case 13): the clock starts ticking
// (Gaud_StartShotClock) and comment 0x15 or 0x25 plays, at random.
void GameModeSkillZoneTimed_TenSecWarning(void) {
    Gaud_StartShotClock();
    if (!(Misc_RandFunc(0) & 1)) {
        GameModeSkillZoneBase_StartComment(0x15);
        return;
    }
    GameModeSkillZoneBase_StartComment(0x25);
}

// The current golfer's time ran out (through GameModeSkillZoneBase_TimerOut): the ticking stops. If
// the ball is not yet in play (GS_PRE_SHOT to GS_ELEVATOR, or GS_SWING) the golfer's game ends: the
// lie set to in the hole (12), nSGFlags bit 26, the HUD clock 0, message 18, GS_IN_THE_HOLE and
// comment 0x14. With the shot under way bE9D is set instead, and CheckShotAwards decides once the
// ball stops.
void GameModeSkillZoneTimed_TimerOut(void) {
    Gaud_StopShotClock();
    if ((s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 1 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 2 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 3 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 4 ||
        (s8)GOLFERSTATE_GetCurrentState(lbl_80282278) == 10) {
        gPlayers[lbl_80282278].ball.nLie = 12;
        gPlayers[lbl_80282278].nSGFlags |= 0x04000000;
        GameModeSkillZoneTimed_SetHudClock(0);
        PlayNow_SendMessage18(lbl_80282278);
        GOLFERSTATE_Switch(13, lbl_80282278);
        GameModeSkillZoneBase_StartComment(0x14);
        return;
    }
    gPlayers[lbl_80282278].bE9D = 1;
}

// The ball went out of bounds (pfnBallOOB): the shot is scored as usual (CheckShotAwards).
void GameModeSkillZoneTimed_BallOOB(int nPlayer) {
    GameModeSkillZoneTimed_CheckShotAwards(nPlayer);
}

// A mulligan was taken (pfnMulligan): the shot's multiplier (nDBC) goes back to 1 and the target
// streak (nHitStreak) to 0.
void GameModeSkillZoneTimed_Mulligan(int nPlayer) {
    gPlayers[nPlayer].nDBC = 1;
    gPlayers[nPlayer].nHitStreak = 0;
}

// Sets the player's time left on the current hole (n290), in frames (pfnSetTimer, from a UI
// command).
void GameModeSkillZoneTimed_SetTimer(int nPlayer, int nTime) {
    gPlayers[nPlayer].n290[Game_CurHoleIndex()] = nTime;
}

// The hole (and so the game) is over once every player's time (n290) is 0; nPlayer and bCheck are
// not used.
u8 GameModeSkillZoneTimed_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->n290[Game_CurHoleIndex()] != 0) {
            return 0;
        }
    }
    return 1;
}

// The ball hit a bonus object (pfnCollisionActor, with its id, Ball.n140): the bullseye ball effect
// plays and the bonus multiplier (gTimedBonusMultiplier) goes up by 2 plus the object's index
// (GameModeSkillZoneBase_GetBonusIndex), so by 2 to 6.
void GameModeSkillZoneTimed_CollisionActor(int nPlayer, int nId) {
    s32 n = GameModeSkillZoneBase_GetBonusIndex(nId);
    fn_800A30E4(8, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
    gTimedBonusMultiplier += n + 2;
}

// Which marker model target nTarget shows for the player (pfnGreenType, GoDynObj.c): 1 once they
// have hit it 4 times (closed out, it pays no more), else 0.
s32 GameModeSkillZoneTimed_GreenType(int nPlayer, int nTarget) {
    if (gPlayers[nPlayer].nTargetHits[nTarget] > 3) {
        return 1;
    }
    return 0;
}

// The bonus-object multiplier (gTimedBonusMultiplier), whoever nPlayer is.
s32 GameModeSkillZoneTimed_GetDriveMultiplier(s32 nPlayer) {
    return gTimedBonusMultiplier;
}

// End of the game (pfnEndGame): the game counts as won in the bio (EASBio_SetCurrentGameWon) and
// the full-screen UI is switched off.
void GameModeSkillZoneTimed_EndGame(void) {
    EASBio_SetCurrentGameWon(1);
    GUI_ShowToggleFullScreenUI(0);
}

// Sends UI message 17, the HUD clock, with a value: mode 13 passes the player's time left in frames
// (0 when it ran out, -1 when the hole restarts); speed golf (GameMode8.c) sends 3 as its countdown
// starts, 2 at the go and 0 at the end.
void GameModeSkillZoneTimed_SetHudClock(s32 nTime) {
    GameMsg_SendInt(17, nTime);
}
