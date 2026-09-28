// GameMode16.c (our name; TW07's GameMode_SkillZoneTarget.cpp, whose methods it has in the same
// order): game mode 16, the target game, one hole. Each player has 20 balls (nDC0) to hit the
// targets in any order; a target pays its points up to 4 times and is then closed out. Once every
// target has been hit, each further target hit pays the all-targets prize (the id 999 row) instead.
// A shot can get a random x2, x3 or x5 multiplier, and bonus objects hit on the way (pfn268,
// CollisionActor) raise a second points multiplier. A target surface past the tee set's drive line
// counts as a drive, paying only for a new longest one. Bullseyes, streaks and the longest drive
// are counted. The game ends when nobody has a ball left. The shared target-game code is
// GameTargets.c.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// Mode 16's state; only this file uses it. The .sbss ones are defined last address first (the
// compiler lays a file's .sbss out last definition first).
s32 gTargetSavedOptionsC = 4;   // options.nC from before the game (StartGamePreData; Shutdown puts it back)
s32 gTargetShotPoints;          // the points of the last shot (GetShotEarned)
s32 gTargetBonusMultiplier;     // the points multiplier from bonus objects: 1 each shot, raised by
                                //   CollisionActor (GetDriveMultiplier)
s32 gTargetSavedWind;           // options.nWind from before the game (StartGamePreData; Shutdown puts
                                //   it back)

void  GameModeSkillZoneTarget_Shutdown(void);
void  GameModeSkillZoneTarget_StartGamePreData(void);
u8    GameModeSkillZoneTarget_GoToPlayoff(u8 bCheck);
s32   GameModeSkillZoneTarget_GetHonors(int nPlayer);
void  GameModeSkillZoneTarget_EndGolferTurn(int nPlayer);
void  GameModeSkillZoneTarget_CheckShotAwards(int nPlayer);
void  GameModeSkillZoneTarget_SetupNextGolfer(void);
void  GameModeSkillZoneTarget_LoadHole(void);
void  GameModeSkillZoneTarget_RestartHole(void);
void  GameModeSkillZoneTarget_ClearPerHoleData(void);
void  GameModeSkillZoneTarget_UpdateSwingUI(int nPlayer);
u8    GameModeSkillZoneTarget_GameFinished(u8 bCheck);
void  GameModeSkillZoneTarget_BallOOB(int nPlayer);
u8    GameModeSkillZoneTarget_HoleFinished(int nPlayer, u8 bCheck);
void  GameModeSkillZoneTarget_GetIDScore(s32 nSurface, s32* pPoints);
s32   GameModeSkillZoneTarget_GreenType(int nPlayer, int nTarget);
void  GameModeSkillZoneTarget_CollisionActor(int nPlayer, int nId);
void  GameModeSkillZoneTarget_EndGame(void);

// Game mode 16's setup (pfnInit, from GM_SetModeType): its hooks (the target list ones from
// GameTargets.c), no wind, no gimmes, no mulligans, no GameBreakers (b285), b28D set (the re-plan
// button picks the next target), pin set 0, the target list emptied (the hole's targets fill it as
// they load) and the current hole 0.
void GameModeSkillZoneTarget_Init(void) {
    gpGame->pfnInit = GameModeSkillZoneTarget_Init;
    gpGame->pfnShutdown = GameModeSkillZoneTarget_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeSkillZoneTarget_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkillZoneTarget_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkillZoneTarget_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkillZoneTarget_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkillZoneTarget_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameModeSkillZoneTarget_EndGolferTurn;
    gpGame->pfn244 = GameModeSkillZoneTarget_CheckShotAwards;
    gpGame->pfn1E4 = GameModeSkillZoneTarget_LoadHole;
    gpGame->pfn228 = GameModeSkillZoneTarget_UpdateSwingUI;
    gpGame->pfn224 = GameModeSkillZoneTarget_RestartHole;
    gpGame->pfn1EC = GameModeSkillZoneTarget_StartGamePreData;
    gpGame->pfn250 = GameModeSkillZoneTarget_BallOOB;
    gpGame->pfn264 = GameModeSkillZoneBase_PickPrevTarget;
    gpGame->pfn258 = GameModeSkillZoneBase_PickTarget;
    gpGame->pfn26C = GameModeSkillZoneTarget_GreenType;
    gpGame->pfn268 = GameModeSkillZoneTarget_CollisionActor;
    gpGame->pfnEndGame = GameModeSkillZoneTarget_EndGame;
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
void GameModeSkillZoneTarget_Shutdown(void) {
    gSession.options.nC = gTargetSavedOptionsC;
    gSession.options.nWind = gTargetSavedWind;
}

// As a round starts (pfn1EC, GM_InitModule_PreDataStream): saves options.nC and the wind setting
// (Shutdown puts them back) and sets them to 4 and 0, no wind.
void GameModeSkillZoneTarget_StartGamePreData(void) {
    gTargetSavedOptionsC = gSession.options.nC;
    gTargetSavedWind = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
}

// Never a playoff: returns 0.
u8 GameModeSkillZoneTarget_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): player 0 while nobody has a stroke on the hole; after that the
// next player in turn after the current golfer (lbl_80282278) who is not nPlayer and still has
// balls (nDC0); 5 when there is none.
s32 GameModeSkillZoneTarget_GetHonors(int nPlayer) {
    int i;
    int nNext;
    u8 bFirst = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] != 0) {
            bFirst = 0;
        }
    }
    if (bFirst) {
        return 0;
    }
    nNext = lbl_80282278;
    for (i = 0; i < 5; i++) {
        nNext++;
        if (nNext >= gNumPlayersSetUp) {
            nNext = 0;
        }
        if (nNext != nPlayer && gPlayers[nNext].nDC0 != 0) {
            return nNext;
        }
    }
    return 5;
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (with in-flight
// replays on, gReplayData.bF10, the replay's saved ball is copied back instead); one ball fewer
// left (nDC0), and aDC4[0] counts the shot when it had a multiplier (nDBC).
void GameModeSkillZoneTarget_EndGolferTurn(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        Physics_InitBall(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nDC0--;
    if (gPlayers[nPlayer].nDBC > 1) {
        gPlayers[nPlayer].aDC4[0]++;
    }
}

// Scores a shot once the ball stops (pfn244; BallOOB too). The landing surface's
// gEarningsTable.aMini row gives the points (GetIDScore, mode 16's column). A target hit short of
// the drive line (GameModeSkillZoneBase_IsLongDrive) adds to the streak (nE90, best nE8C); after 4
// hits a target is closed out and pays nothing (Gaud_TargetClosedOut, comment 2); otherwise its hit
// count (nDE4) and aDC4[3] go up and, while every target has been hit (so on the last new one and
// on every hit after it), the all-targets prize is paid instead of the points, with comment 0x2B or
// 0x2C the first time (bE9E). A bullseye is counted (nDE0) and plays the bullseye sound and ball
// effect; a multiplier plays its own effect; the comment is the multiplier's or the ring's. A
// target surface past the drive line is a drive: a new longest one (nDDC) pays as a surface, any
// other nothing. Points are multiplied by the shot multiplier (nDBC) and the bonus multiplier
// (gTargetBonusMultiplier, raised by CollisionActor), then the earnings modifiers; a loss (comment
// 0 or 0x4E) never takes the winnings (nDD8) below 0. The result goes to the HUD (message 0x33)
// unless in a replay.
void GameModeSkillZoneTarget_CheckShotAwards(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    f32 fLength;
    s32 nMult;
    Ball* pBall;
    nMsg = -1;
    nSurface = gPlayers[nPlayer].ball.nSurface;
    fLength = fn_800D0550(nPlayer);
    GameModeSkillZoneTarget_GetIDScore(nSurface, &gTargetShotPoints);
    if (nSurface >= 0x85 && nSurface <= 0x90 && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        nTarget = GameModeSkillZoneBase_GetGreenIndexHit(nPlayer);
        GameModeSkillZoneBase_GetBullsEyeColor(nSurface);
        gPlayers[nPlayer].nE90++;
        if (gPlayers[nPlayer].nE90 > gPlayers[nPlayer].nE8C) {
            gPlayers[nPlayer].nE8C = gPlayers[nPlayer].nE90;
        }
        if (gPlayers[nPlayer].nDE4[nTarget] > 3) {
            gTargetShotPoints = 0;
            GameMsg_Send5Ints(0x33, 0, 0, 0, 0xC8, 1);
            Gaud_TargetClosedOut();
            nMsg = 2;
        } else {
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (lbl_80282360 == GameModeSkillZoneBase_CountGreensHit(nPlayer)) {
                gTargetShotPoints = GameModeSkillZoneBase_GetHitAllTargetsBonus();
                gTargetShotPoints = GM_Earnings_ComputeBonusModifiers(gTargetShotPoints, nPlayer, 1, 1, 1, 0);
                gTargetShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTargetShotPoints, nPlayer, 0);
                GM_Earnings_AwardMoney(nPlayer, gTargetShotPoints, 0);
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                gPlayers[nPlayer].nDD8 += gTargetShotPoints;
                GameMsg_Send5Ints(0x33, gTargetShotPoints, 0, 0, 0xCA, 1);
                gTargetShotPoints = 0;
                if ((s8)gPlayers[nPlayer].bE9E == 0) {
                    gPlayers[nPlayer].bE9E = 1;
                    if (!(Misc_RandFunc(0) & 1)) {
                        nMsg = 0x2B;
                    } else {
                        nMsg = 0x2C;
                    }
                }
            }
            if (nSurface == 0x85 || nSurface == 0x88 || nSurface == 0x8C) {
                gPlayers[nPlayer].nDE0++;
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
            if (nMsg == -1) {
                switch (gPlayers[nPlayer].nDBC) {
                case 2:
                    nMsg = 0x32;
                    break;
                case 3:
                    nMsg = 0x33;
                    break;
                case 5:
                    nMsg = 0x31;
                    break;
                default:
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
                    break;
                }
                GameMsg_Send5Ints(0x33, 0, 0, 0, nSurface, 1);
            }
        }
    } else {
        gPlayers[nPlayer].nE90 = 0;
    }
    if (nSurface >= 0x85 && nSurface <= 0x90 && GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
        if (fLength > gPlayers[nPlayer].nDDC) {
            gPlayers[nPlayer].nDDC = fLength;
            if (nMsg == -1) {
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
            }
        } else {
            gTargetShotPoints = 0;
            if (nMsg == -1) {
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
    }
    if (gTargetShotPoints != 0) {
        if (gTargetShotPoints > 0) {
            gTargetShotPoints = (f32)gTargetShotPoints * gPlayers[nPlayer].nDBC;
            gTargetShotPoints = (f32)(gTargetShotPoints * gTargetBonusMultiplier);
            gTargetShotPoints = GM_Earnings_ComputeBonusModifiers(gTargetShotPoints, nPlayer, 1, 1, 1, 0);
            gTargetShotPoints = GM_Earnings_ComputeTOURCardModifiers(gTargetShotPoints, nPlayer, 0);
        } else if (!(Misc_RandFunc(0) & 1)) {
            GameModeSkillZoneBase_StartComment(0);
        } else {
            GameModeSkillZoneBase_StartComment(0x4E);
        }
        if (gTargetShotPoints + gPlayers[nPlayer].nDD8 < 0) {
            GM_Earnings_AwardMoney(nPlayer, -gPlayers[nPlayer].nDD8, 0);
        } else {
            GM_Earnings_AwardMoney(nPlayer, gTargetShotPoints, 0);
        }
        gPlayers[nPlayer].nDD8 += gTargetShotPoints;
        if (gPlayers[nPlayer].nDD8 < 0) {
            gPlayers[nPlayer].nDD8 = 0;
        } else if (!gSession.bReplay) {
            if (GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
                GameMsg_Send5Ints(0x33, gTargetShotPoints, 0, 0, 0xD6, 1);
            } else {
                GameMsg_Send5Ints(0x33, gTargetShotPoints, 0, 0, nSurface, 1);
            }
            if (gTargetShotPoints > 0 && nSurface < 0x85) {
                Gaud_MoneyAward();
            }
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_StartComment(nMsg);
    }
    GameModeSkillZoneBase_PostShotAwards1(nPlayer);
    GameModeSkillZoneBase_PostShotAwards2(nPlayer);
}

// Before each shot (pfnSetupNextGolfer): per-shot data cleared (ClearPerShotData) and stroke play's
// golfer order (GameModeStroke_SetupNextGolfer); then for the golfer about to play (GS_PRE_SHOT) a
// chance of a shot multiplier (SetupBonusBall) and, when they have no balls left (nDC0 is 0), their
// current target set up again (SetCup_AlignGolfer). The bonus multiplier goes back to 1. The nDC0
// test is GameModeSkillZoneTimed_SetupNextGolfer's, where nDC0 counts shots up from 0 and so means
// before the first shot; here nDC0 counts balls down from 20.
void GameModeSkillZoneTarget_SetupNextGolfer(void) {
    int i;
    GameModeSkillZoneBase_ClearPerShotData();
    GameModeStroke_SetupNextGolfer();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            GameModeSkillZoneBase_SetupBonusBall(i);
            if (PLAYER(i)->nDC0 == 0) {
                GameModeSkillZoneBase_SetCup_AlignGolfer(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
    gTargetBonusMultiplier = 1;
}

// Hole start (pfn1E4): the targets sorted nearest the tee first (SortCupsByDistanceFromTee), then
// ClearPerHoleData.
void GameModeSkillZoneTarget_LoadHole(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    GameModeSkillZoneTarget_ClearPerHoleData();
}

// The hole restarts (pfn224, GM_RestartHole): the per-hole data cleared with 20 balls each again
// (ClearPerHoleData) and player 0's default aim.
void GameModeSkillZoneTarget_RestartHole(void) {
    GameModeSkillZoneTarget_ClearPerHoleData();
    AI_DefaultTarget(0);
}

// The shared per-hole clear (GameModeSkillZoneBase_ClearPerHoleData), then all five players get 20
// balls (nDC0).
void GameModeSkillZoneTarget_ClearPerHoleData(void) {
    int i;
    GameModeSkillZoneBase_ClearPerHoleData();
    i = 0;
    while (i < 5) {
        gPlayers[i++].nDC0 = 20;
    }
}

// Every frame of the swing state (pfn228): once the swing has begun (SwingData.nState not
// SW_IDLE_SWING), UI message 0x36 (no value) is sent.
void GameModeSkillZoneTarget_UpdateSwingUI(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

// The game is over once its one hole is: always 1.
u8 GameModeSkillZoneTarget_GameFinished(u8 bCheck) {
    return 1;
}

// The ball went out of bounds (pfn250): the shot is scored as usual (CheckShotAwards).
void GameModeSkillZoneTarget_BallOOB(int nPlayer) {
    GameModeSkillZoneTarget_CheckShotAwards(nPlayer);
}

// The hole (and so the game) is over once no player has a ball left (nDC0); nPlayer and bCheck are
// not used.
u8 GameModeSkillZoneTarget_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nDC0 != 0) {
            return 0;
        }
    }
    return 1;
}

// The points of the gEarningsTable.aMini row for surface nSurface, from mode 16's column (n8); 0
// when there is none (the last matching of the 20 rows wins).
void GameModeSkillZoneTarget_GetIDScore(s32 nSurface, s32* pPoints) {
    int i;
    *pPoints = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].n8;
        }
    }
}

// The points the last shot earned (gTargetShotPoints), whoever nPlayer is.
s32 GameModeSkillZoneTarget_GetShotEarned(s32 nPlayer) {
    return gTargetShotPoints;
}

// The bonus-object multiplier (gTargetBonusMultiplier), whoever nPlayer is.
s32 GameModeSkillZoneTarget_GetDriveMultiplier(s32 nPlayer) {
    return gTargetBonusMultiplier;
}

// Which marker model target nTarget shows for the player (pfn26C, GoDynObj.c): 1 once they have hit
// it 4 times (closed out, it pays no more), else 0.
s32 GameModeSkillZoneTarget_GreenType(int nPlayer, int nTarget) {
    if (gPlayers[nPlayer].nDE4[nTarget] > 3) {
        return 1;
    }
    return 0;
}

// The ball hit a bonus object (pfn268, with its id, Ball.n140): the bullseye ball effect plays and
// the bonus multiplier (gTargetBonusMultiplier) goes up by 2 plus the object's index
// (GameModeSkillZoneBase_GetBonusIndex), so by 2 to 6.
void GameModeSkillZoneTarget_CollisionActor(int nPlayer, int nId) {
    s32 nIndex = GameModeSkillZoneBase_GetBonusIndex(nId);
    fn_800A30E4(8, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
    gTargetBonusMultiplier += nIndex + 2;
}

// End of the game (pfnEndGame): the game counts as won in the bio (EASBio_SetCurrentGameWon).
void GameModeSkillZoneTarget_EndGame(void) {
    EASBio_SetCurrentGameWon(1);
}
