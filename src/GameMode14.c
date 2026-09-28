// GameMode14.c (our name; TW07's GameMode_SkillZoneCapture.cpp): game mode 14, the two-player
// capture target game, one hole. The targets are up to 40 spots on the hole (the list in
// GameModeReplay.c; the code the target modes share is GameTargets.c). A shot landing in a
// target's ring (0 the bullseye .. 4) claims it when that ring is closer than the one it is held
// with, whoever holds it; a target claimed with a bullseye is locked. Taking one from the other
// player is a steal. The first to hold 5 targets wins and is paid their points: each held target's
// ring points (500 for the bullseye down to 100), times the hole's target factor, with the
// earnings modifiers; the loser gets nothing. Each turn has a shot clock, and running out forfeits
// the shot.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// One target's claim (TW07's captured ring and ring owner): the ring of the claiming shot (0 the
// bullseye .. 4, 5 unclaimed) and who holds it (0 or 1, 5 nobody).
typedef struct Claim {
    s32 nRing;                  // 0x0
    s32 nOwner;                 // 0x4
} Claim;

// Mode 14's state; only this file uses it. The .sbss ones are defined last address first (the
// compiler lays a file's .sbss out last definition first).
Claim gCaptureClaims[40];       // per target of the target list (gSkillZoneCups)
s32 gCaptureSavedWeather = 4;  // options.nWeather before the game (StartGamePreData; Shutdown restores it)
s32 gCaptureShotPoints;         // the points of the last claim (GetShotEarned)
u8  gCaptureShotClockOut;       // the shot clock ran out (ShotClockOut): the shot claims nothing
s32 gCaptureFirstGolfer;        // who plays first: player 0 or 1, at random (StartGamePreData)
s32 gCaptureSavedWind;          // options.nWind from before the game (StartGamePreData; Shutdown puts
                                //   it back)
s32 gCaptureRingPoints[6] = {500, 400, 300, 200, 100, 0};  // a claim's points by its ring (5: none)

void  GameModeSkillZoneCapture_Shutdown(void);
void  GameModeSkillZoneCapture_StartGamePreData(void);
u8    GameModeSkillZoneCapture_GoToPlayoff(u8 bCheck);
s32   GameModeSkillZoneCapture_GetHonors(int nPlayer);
void  GameModeSkillZoneCapture_EndGolferTurn(int nPlayer);
void  GameModeSkillZoneCapture_CheckShotAwards(int nPlayer);
void  GameModeSkillZoneCapture_SetupNextGolfer(void);
void  GameModeSkillZoneCapture_LoadHole(void);
void  GameModeSkillZoneCapture_RestartHole(void);
void  GameModeSkillZoneCapture_ClearPerHoleData(void);
void  GameModeSkillZoneCapture_UpdateSwingUI(int nPlayer);
u8    GameModeSkillZoneCapture_GameFinished(u8 bCheck);
void  GameModeSkillZoneCapture_BallOOB(int nPlayer);
u8    GameModeSkillZoneCapture_HoleFinished(int nPlayer, u8 bCheck);
s32   GameModeSkillZoneCapture_GetMadeMoneyFromIndex(int nTarget);
void  GameModeSkillZoneCapture_ComputePlayerScore(void);
void  GameModeSkillZoneCapture_HitBall(int nPlayer);
void  GameModeSkillZoneCapture_EndGame(void);
s32   GameModeSkillZoneCapture_GreenType(int nPlayer, int nTarget);

// Game mode 14's setup (pfnInit, from GM_SetModeType): its hooks (the target-list ones from
// GameTargets.c), nC and n10 2 (two players, as GameModeBattle sets them), no wind, no gimmes, no
// mulligans, b28D set (the re-plan button picks the next target), the current hole 0, pin set 0 and
// the target list emptied (the hole's targets fill it as they load).
void GameModeSkillZoneCapture_Init(void) {
    gpGame->pfnInit = GameModeSkillZoneCapture_Init;
    gpGame->pfnShutdown = GameModeSkillZoneCapture_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeSkillZoneCapture_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkillZoneCapture_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkillZoneCapture_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkillZoneCapture_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkillZoneCapture_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameModeSkillZoneCapture_EndGolferTurn;
    gpGame->pfnCheckShotAwards = GameModeSkillZoneCapture_CheckShotAwards;
    gpGame->pfnLoadHole = GameModeSkillZoneCapture_LoadHole;
    gpGame->pfn228 = GameModeSkillZoneCapture_UpdateSwingUI;
    gpGame->pfnRestartHole = GameModeSkillZoneCapture_RestartHole;
    gpGame->pfnStartGamePreData = GameModeSkillZoneCapture_StartGamePreData;
    gpGame->pfnBallOOB = GameModeSkillZoneCapture_BallOOB;
    gpGame->pfnPickPrevTarget = GameModeSkillZoneBase_PickPrevTarget;
    gpGame->pfnPickTarget = GameModeSkillZoneBase_PickTarget;
    gpGame->pfn260 = GameModeSkillZoneCapture_HitBall;
    gpGame->pfnEndGame = GameModeSkillZoneCapture_EndGame;
    gpGame->pfnGreenType = GameModeSkillZoneCapture_GreenType;
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
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    gSkillZoneNumCups = 0;
    gSession.nSplitScreen = 0;
    gSession.nPinSet = 0;
}

// Puts back the two options StartGamePreData changed for the game: options.nWeather and the wind.
void GameModeSkillZoneCapture_Shutdown(void) {
    gSession.options.nWeather = gCaptureSavedWeather;
    gSession.options.nWind = gCaptureSavedWind;
}

// As a round starts (pfn1EC, GM_InitModule_PreDataStream): saves options.nWeather and the wind
// setting (Shutdown puts them back), sets them to 4 and 0 (no wind), and picks the first golfer,
// player 0 or 1 at random (gCaptureFirstGolfer).
void GameModeSkillZoneCapture_StartGamePreData(void) {
    gCaptureSavedWeather = gSession.options.nWeather;
    gCaptureSavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
    gSession.options.nWind = 0;
    gCaptureFirstGolfer = Misc_RandFunc(0) & 1;
}

u8 GameModeSkillZoneCapture_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): the random first golfer (gCaptureFirstGolfer) while nobody has a
// stroke on the hole; after that the next player in turn after the current golfer (lbl_80282278)
// who is not nPlayer; 5 when there is none.
s32 GameModeSkillZoneCapture_GetHonors(int nPlayer) {
    int i;
    int n;
    u8 bFirst = 1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nStrokes[Game_CurHoleIndex()] != 0) {
            bFirst = 0;
        }
    }
    if (bFirst) {
        return gCaptureFirstGolfer;
    }
    n = lbl_80282278;
    for (i = 0; i < 5; i++) {
        n++;
        if (n >= gNumPlayersSetUp) {
            n = 0;
        }
        if (n != nPlayer) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (with in-flight
// replays on, gReplayData.bF10, the replay's saved ball is copied back instead) and the shots taken
// (nDC0) are counted.
void GameModeSkillZoneCapture_EndGolferTurn(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        Physics_InitBall(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nDC0++;
}

// Scores a shot once the ball stops (pfn244 from GM_Earnings_PayShotGoals; BallOOB too). After a
// shot-clock timeout (gCaptureShotClockOut) it only shows text 0xD1 and comment 0x14. A landing on
// a target short of the drive line (GameModeSkillZoneBase_IsLongDrive) is in one of its rings (0
// the bullseye .. 4). A target claimed with a bullseye is locked (text 0xCD, Gaud_TargetClosedOut);
// a ring no closer than the target's claim, whoever holds it, claims nothing (text 0xCC). A closer
// ring claims the target for nPlayer: taking it from the other player counts a steal (nE94, text
// 0xD4), a bullseye counts in nDE0 (text 0xD2, or 0xD5 when stolen). The claim's surface is logged
// (aCD4[nCD0++], nD70 per hole); its points (GetMadeMoneyFromIndex, the hole's target factor, the
// earnings modifiers) go to gCaptureShotPoints and everyone's totals are recomputed
// (ComputePlayerScore). Outside replays the points float up at the surface (message 0x33; text 0xD3
// for a plain claim) and the ring or bullseye sound plays, with the bullseye and shot-multiplier
// (nDBC) ball effects. A comment fits each case.
void GameModeSkillZoneCapture_CheckShotAwards(int nPlayer) {
    s32 nSurface;
    int nTarget;
    s32 nRing;
    s32 nMsg = -1;
    s32 nText;
    s32 nStolenLocked = 0;
    f32 fLength;
    s32 nMult;
    if (gCaptureShotClockOut) {
        GameMsg_Send5Ints(0x33, 0, 0, 0, 0xD1, 1);
        nMsg = 0x14;
    } else {
        nSurface = gPlayers[nPlayer].ball.nSurface;
        fLength = fn_800D0550(nPlayer);
        if (nSurface >= 0x85 && nSurface <= 0x90 && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
            nTarget = GameModeSkillZoneBase_GetGreenIndexHit(nPlayer);
            nRing = GameModeSkillZoneBase_GetBullsEyeColor(nSurface);
            if (gCaptureClaims[nTarget].nRing == 0) {
                GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCD, 1);
                Gaud_TargetClosedOut();
                nMsg = 2;
            } else if (nRing >= gCaptureClaims[nTarget].nRing) {
                GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCC, 1);
                nMsg = 0x10;
            } else {
                nText = 0;
                if (gCaptureClaims[nTarget].nRing == 5) {
                    if (!(Misc_RandFunc(0) & 1)) {
                        nMsg = 0x1A;
                    } else {
                        nMsg = 0x52;
                    }
                } else if (nPlayer == gCaptureClaims[nTarget].nOwner) {
                    nMsg = 0x42;
                } else {
                    nText = 0xD4;
                    nStolenLocked = 1;
                    gPlayers[nPlayer].nE94++;
                    if (!(Misc_RandFunc(0) & 1)) {
                        nMsg = 0xF;
                    } else {
                        nMsg = 0x1B;
                    }
                }
                if (nRing == 0) {
                    gPlayers[nPlayer].nDE0++;
                    if (nStolenLocked == 0) {
                        nText = 0xD2;
                        if (!(Misc_RandFunc(0) & 1)) {
                            nMsg = 0x16;
                        } else {
                            nMsg = 0x53;
                        }
                    } else {
                        nText = 0xD5;
                        if (!(Misc_RandFunc(0) & 1)) {
                            nMsg = 0x17;
                        } else {
                            nMsg = 0x1C;
                        }
                    }
                    nStolenLocked = 2;
                }
                gCaptureClaims[nTarget].nRing = nRing;
                gCaptureClaims[nTarget].nOwner = nPlayer;
                gPlayers[nPlayer].aCD4[gPlayers[nPlayer].nCD0] = nSurface;
                gPlayers[nPlayer].nCD0++;
                gPlayers[nPlayer].nD70[Game_CurHoleIndex()]++;
                gCaptureShotPoints = GameModeSkillZoneCapture_GetMadeMoneyFromIndex(nTarget);
                gCaptureShotPoints = GameModeSkillZoneBase_ScaleTargetPoints(gCaptureShotPoints, nTarget);
                gCaptureShotPoints = GM_Earnings_ComputeBonusModifiers(gCaptureShotPoints,
                                                                       nPlayer, 1, 1, 1, 0);
                gCaptureShotPoints = GM_Earnings_ComputeTOURCardModifiers(gCaptureShotPoints, nPlayer, 0);
                GameModeSkillZoneCapture_ComputePlayerScore();
                if (nText != 0) {
                    GameMsg_Send5Ints(0x33, gCaptureShotPoints, 0, 0, nText, 1);
                }
                if (!gSession.bReplay) {
                    GameMsg_Send5Ints(0x33, gCaptureShotPoints, 0, 0, nSurface, 1);
                    if (nStolenLocked == 0) {
                        GameMsg_Send5Ints(0x33, gCaptureShotPoints, 0, 0, 0xD3, 1);
                    }
                    if (nRing == 0) {
                        Ball* pBall;
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
            }
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_StartComment(nMsg);
    }
}

// Before each shot (pfnSetupNextGolfer): the shot-clock timeout flag (gCaptureShotClockOut) and the
// per-shot data clear (ClearPerShotData) and stroke play's golfer order runs
// (GameModeStroke_SetupNextGolfer). The golfer about to play (GS_PRE_SHOT) gets a 900 shot clock
// (GameModeSkillZoneCapture_SetShotClock) and, before their first shot, their target set up
// (SetCup_AlignGolfer); if they hold 3 or more targets fewer than the other player, a comment
// (0x4A/0x4C for player 0, 0x4B/0x4D for player 1) plays.
void GameModeSkillZoneCapture_SetupNextGolfer(void) {
    int i;
    s32 n0;
    s32 n1;
    s32 nMsg = -1;
    gCaptureShotClockOut = 0;
    GameModeSkillZoneBase_ClearPerShotData();
    GameModeStroke_SetupNextGolfer();
    n0 = GameModeSkillZoneCapture_GetTotalTargetsHit(0);
    n1 = GameModeSkillZoneCapture_GetTotalTargetsHit(1);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            GameModeSkillZoneCapture_SetShotClock(900);
            if (PLAYER(i)->nDC0 == 0) {
                GameModeSkillZoneBase_SetCup_AlignGolfer(i, (s8)PLAYER(i)->nTarget);
            }
            if (i == 0 && n1 >= n0 + 3) {
                if (!(Misc_RandFunc(0) & 1)) {
                    nMsg = 0x4A;
                } else {
                    nMsg = 0x4C;
                }
            } else if (i == 1 && n0 >= n1 + 3) {
                if (!(Misc_RandFunc(0) & 1)) {
                    nMsg = 0x4B;
                } else {
                    nMsg = 0x4D;
                }
            }
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_PlayComment((u16)nMsg, 0);
    }
}

// Hole start (pfn1E4): the targets sorted nearest the tee first (SortCupsByDistanceFromTee), then
// ClearPerHoleData.
void GameModeSkillZoneCapture_LoadHole(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    GameModeSkillZoneCapture_ClearPerHoleData();
}

// The hole restarts (pfn224, GM_RestartHole): every claim cleared (ClearPerHoleData) and player 0
// given the default aim.
void GameModeSkillZoneCapture_RestartHole(void) {
    GameModeSkillZoneCapture_ClearPerHoleData();
    AI_DefaultTarget(0);
}

// The shared per-hole clear (GameModeSkillZoneBase_ClearPerHoleData), then all 40 targets unclaimed
// (gCaptureClaims: ring and holder 5).
void GameModeSkillZoneCapture_ClearPerHoleData(void) {
    int i;
    GameModeSkillZoneBase_ClearPerHoleData();
    for (i = 0; i < 40; i++) {
        gCaptureClaims[i].nRing = 5;
        gCaptureClaims[i].nOwner = 5;
    }
}

// Every frame of the swing state (pfn228): once the swing has begun (SwingData.nState not
// SW_IDLE_SWING), UI message 0x36 (no value) is sent.
void GameModeSkillZoneCapture_UpdateSwingUI(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

// The game is over once its one hole is: always 1.
u8 GameModeSkillZoneCapture_GameFinished(u8 bCheck) {
    return 1;
}

// The ball went out of bounds (pfn250): the shot is scored as usual (CheckShotAwards).
void GameModeSkillZoneCapture_BallOOB(int nPlayer) {
    GameModeSkillZoneCapture_CheckShotAwards(nPlayer);
}

// The hole (and so the game) is over once a player holds 5 targets (GetTotalTargetsHit); nPlayer
// and bCheck are not used.
u8 GameModeSkillZoneCapture_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (GameModeSkillZoneCapture_GetTotalTargetsHit(i) >= 5) {
            return 1;
        }
    }
    return 0;
}

// fake match: GetTargettedRingOwner gets the aimed-at target through this identity inline
// (GetTargettedCapturedRing calls GameModeSkillZoneBase_GetGreenTargetted directly); the direct call
// there scores 99.2% (rm2, 2026-09-28). EA's real form is not known.
static inline int CurrentTarget(int nPlayer) {
    return GameModeSkillZoneBase_GetGreenTargetted(nPlayer);
}

// Who holds the target the player is aiming at, for the HUD (UI command 14): 0 or 1, 5 nobody; -1
// when the aim point (Player.nSurface) is not on a target.
s32 GameModeSkillZoneCapture_GetTargettedRingOwner(int nPlayer) {
    if (gPlayers[nPlayer].nSurface < 0x85 || gPlayers[nPlayer].nSurface > 0x90) {
        return -1;
    }
    return gCaptureClaims[CurrentTarget(nPlayer)].nOwner;
}

// The ring the target the player is aiming at was claimed with, for the HUD (UI command 15): 0 the
// bullseye .. 4, 5 unclaimed; -1 when the aim point (Player.nSurface) is not on a target.
s32 GameModeSkillZoneCapture_GetTargettedCapturedRing(int nPlayer) {
    if (gPlayers[nPlayer].nSurface < 0x85 || gPlayers[nPlayer].nSurface > 0x90) {
        return -1;
    }
    return gCaptureClaims[GameModeSkillZoneBase_GetGreenTargetted(nPlayer)].nRing;
}

// How many of the 40 targets the player holds (gCaptureClaims). 5 wins; GameEffects starts the
// GameBreaker camera at 4.
int GameModeSkillZoneCapture_GetTotalTargetsHit(int nPlayer) {
    s32 n = 0;
    int i;
    for (i = 0; i < 40; i++) {
        if (nPlayer == gCaptureClaims[i].nOwner) {
            n++;
        }
    }
    return n;
}

// Who holds target nTarget, for the HUD (UI command 17): 0 or 1, 5 nobody.
s32 GameModeSkillZoneCapture_GetRingOwnerFromIndex(int nTarget) {
    return gCaptureClaims[nTarget].nOwner;
}

// The ring target nTarget was claimed with, for the HUD (UI command 18): 0 the bullseye .. 4, 5
// unclaimed.
s32 GameModeSkillZoneCapture_GetCapturedRingFromIndex(s32 nTarget) {
    return gCaptureClaims[nTarget].nRing;
}

// A target's points by the ring it was claimed with (gCaptureRingPoints: 500 for the bullseye down
// to 100 for ring 4); 0 when nobody holds it. Before the hole's target factor and the earnings
// modifiers.
s32 GameModeSkillZoneCapture_GetMadeMoneyFromIndex(int nTarget) {
    if (gCaptureClaims[nTarget].nOwner != 5) {
        return gCaptureRingPoints[gCaptureClaims[nTarget].nRing];
    }
    return 0;
}

// Every player's winnings (nDD8) recomputed: the sum over the targets they hold of the claim's
// points (GetMadeMoneyFromIndex), times the hole's target factor (ScaleTargetPoints), with the
// earnings bonus and TOUR card modifiers.
void GameModeSkillZoneCapture_ComputePlayerScore(void) {
    int i;
    s32 n;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nDD8 = 0;
    }
    for (i = 0; i < 40; i++) {
        if (gCaptureClaims[i].nOwner != 5) {
            n = GameModeSkillZoneCapture_GetMadeMoneyFromIndex(i);
            n = GameModeSkillZoneBase_ScaleTargetPoints(n, i);
            n = GM_Earnings_ComputeBonusModifiers(n, gCaptureClaims[i].nOwner, 1, 1, 1, 0);
            n = GM_Earnings_ComputeTOURCardModifiers(n, gCaptureClaims[i].nOwner, 0);
            gPlayers[gCaptureClaims[i].nOwner].nDD8 += n;
        }
    }
}

// The points of the last claim (gCaptureShotPoints), whoever nPlayer is.
s32 GameModeSkillZoneCapture_GetShotEarned(s32 nPlayer) {
    return gCaptureShotPoints;
}

// The ball was hit (pfn260): the shot clock is switched off (-1) and its ticking stopped.
void GameModeSkillZoneCapture_HitBall(int nPlayer) {
    GameModeSkillZoneCapture_SetShotClock(-1);
    Gaud_StopShotClock();
}

// The shot clock ran out (GameModeSkillZoneBase_ShotClockOut, from a UI command): the current
// golfer goes to GS_SIMULATE, the toggle UI hides, the ticking stops and gCaptureShotClockOut is
// set, so CheckShotAwards claims nothing and shows text 0xD1.
void GameModeSkillZoneCapture_ShotClockOut(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    GUI_HideAllToggleUI();
    Gaud_StopShotClock();
    gCaptureShotClockOut = 1;
}

// End of the game (pfnEndGame): the player holding 5 targets wins (EASBio_SetCurrentGameWon) and is
// paid their winnings (nDD8, GM_Earnings_AwardMoney); a player with fewer loses theirs (nDD8 0),
// and comment 0x48 plays if one ends with none.
void GameModeSkillZoneCapture_EndGame(void) {
    int i;
    s32 nMsg = -1;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (GameModeSkillZoneCapture_GetTotalTargetsHit(i) < 5) {
            PLAYER(i)->nDD8 = 0;
            if (GameModeSkillZoneCapture_GetTotalTargetsHit(i) == 0) {
                nMsg = 0x48;
            }
        } else {
            EASBio_SetCurrentGameWon(1);
            GM_Earnings_AwardMoney(i, PLAYER(i)->nDD8, 0);
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_PlayComment((u16)nMsg, 0);
    }
}

// Which marker model target nTarget shows (pfn26C, GoDynObj.c): 1 once claimed with a bullseye
// (locked), 2 player 0's, 3 player 1's, 0 free; nPlayer is not used.
s32 GameModeSkillZoneCapture_GreenType(int nPlayer, int nTarget) {
    if (gCaptureClaims[nTarget].nRing == 0) {
        return 1;
    }
    if (gCaptureClaims[nTarget].nOwner == 0) {
        return 2;
    }
    return gCaptureClaims[nTarget].nOwner == 1 ? 3 : 0;
}

// Sends front-end message nMsg with five int values.
void GameMsg_Send5Ints(int nMsg, s32 a, s32 b, s32 c, s32 d, s32 e) {
    GameMsg_Send5(nMsg, 0, &a, &b, &c, &d, &e);
}

// Sends UI message 55, the HUD shot clock, with a value: modes 14 and 15 pass 900 as a golfer's
// turn starts and -1 (off) when the ball is hit. Its running out comes back as a UI command
// (GameModeSkillZoneBase_ShotClockOut).
void GameModeSkillZoneCapture_SetShotClock(s32 nClock) {
    GameMsg_SendInt(55, nClock);
}
