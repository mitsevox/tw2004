// GameMode15.c (our name; TW07's GameMode_SkillZoneHorse.cpp): game mode 15, HORSE on the targets,
// one hole, set up for two players. A shot landing in a target's ring (0 the bullseye .. 4) with
// no leader makes its player the leader. The next player must land on the leader's target in the
// same ring (a match: the lead stands) or a closer one (they take the lead); anything else takes a
// letter (nE88) and ends the lead, so the next shot sets a new one. The lead also ends when play
// comes back round to the leader. Five letters and you are out; the last player in wins. Each shot
// on a target also pays its surface's points, which the players still in are paid at the end. Each
// turn has a shot clock: running out forfeits the shot and, against a leader, takes a letter.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// Mode 15's state; only this file uses it. The .sbss ones are defined last address first (the
// compiler lays a file's .sbss out last definition first).
s32 gHorseSavedOptionsC = 4;    // options.nC from before the game (StartGamePreData; Shutdown puts it back)
s32 gHorseLeader;               // the player whose shot is to be matched (5 = no leader)
s32 gHorseLeaderRing;           // the leader's ring (0 the bullseye .. 4), read only while there is one
s8  gHorseLeaderTarget;         // the target the leader landed on
s32 gHorseShotPoints;           // the last shot's surface points (GetIDScore)
u8  gHorseShotClockOut;         // the shot clock ran out (ShotClockOut): the shot scores nothing
u8  gHorseLastShotExceeded;     // the last shot took the lead with a closer ring (GetLastShotExceeded;
                                //   not set for a first leader, cleared at HitBall)
s32 gHorseSavedWind;            // options.nWind from before the game (StartGamePreData; Shutdown puts
                                //   it back)

void  Gaud_LetterForfeit(void);
void  Gaud_LetterGained(void);

void  GameModeSkillZoneHorse_Shutdown(void);
void  GameModeSkillZoneHorse_StartGamePreData(void);
u8    GameModeSkillZoneHorse_GoToPlayoff(u8 bCheck);
s32   GameModeSkillZoneHorse_GetHonors(int nPlayer);
void  GameModeSkillZoneHorse_EndGolferTurn(int nPlayer);
void  GameModeSkillZoneHorse_CheckShotAwards(int nPlayer);
void  GameModeSkillZoneHorse_SetupNextGolfer(void);
void  GameModeSkillZoneHorse_LoadHole(void);
void  GameModeSkillZoneHorse_RestartHole(void);
void  GameModeSkillZoneHorse_ClearPerHoleData(void);
void  GameModeSkillZoneHorse_UpdateSwingUI(int nPlayer);
u8    GameModeSkillZoneHorse_GameFinished(u8 bCheck);
void  GameModeSkillZoneHorse_BallOOB(int nPlayer);
s32   GameModeSkillZoneHorse_HoleFinished(int nPlayer, u8 bCheck);
u8    GameModeSkillZoneHorse_PickPrevTarget(int nPlayer);
u8    GameModeSkillZoneHorse_PickTarget(int nPlayer);
s8    GameModeSkillZoneHorse_GetCurrentLeaderRing(void);
void  GameModeSkillZoneHorse_HitBall(int nPlayer);
void  GameModeSkillZoneHorse_EndGame(void);
void  GameModeSkillZoneHorse_GetIDScore(s32 nSurface, s32* pPoints);
s32   GameModeSkillZoneHorse_GetLastShotExceeded(void);
s32   GameModeSkillZoneHorse_GreenType(int nPlayer, int nTarget);

// Game mode 15's setup (pfnInit, from GM_SetModeType): its hooks, nC and n10 2 (two players, as
// GameModeBattle sets them), no wind, no gimmes, no mulligans, b28D set (the re-plan button picks
// the next target), the current hole 0, pin set 0 and the target list emptied (the hole's targets
// fill it as they load). The same settings as mode 14.
void GameModeSkillZoneHorse_Init(void) {
    gpGame->pfnInit = GameModeSkillZoneHorse_Init;
    gpGame->pfnShutdown = GameModeSkillZoneHorse_Shutdown;
    gpGame->pfnSetupNextGolfer = GameModeSkillZoneHorse_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkillZoneHorse_GetHonors;
    // The original's GameModeSkillZoneHorse_HoleFinished returns an s32 (0 or 1; a u8 return adds a
    // clrlwi: 95%), so it goes in the u8 slot through a cast.
    gpGame->pfnHoleFinished = (u8 (*)(int, u8))GameModeSkillZoneHorse_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkillZoneHorse_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkillZoneHorse_GoToPlayoff;
    gpGame->pfnEndGolferTurn = GameModeSkillZoneHorse_EndGolferTurn;
    gpGame->pfn244 = GameModeSkillZoneHorse_CheckShotAwards;
    gpGame->pfn1E4 = GameModeSkillZoneHorse_LoadHole;
    gpGame->pfn228 = GameModeSkillZoneHorse_UpdateSwingUI;
    gpGame->pfn224 = GameModeSkillZoneHorse_RestartHole;
    gpGame->pfn1EC = GameModeSkillZoneHorse_StartGamePreData;
    gpGame->pfn250 = GameModeSkillZoneHorse_BallOOB;
    gpGame->pfn264 = GameModeSkillZoneHorse_PickPrevTarget;
    gpGame->pfn258 = GameModeSkillZoneHorse_PickTarget;
    gpGame->pfn260 = GameModeSkillZoneHorse_HitBall;
    gpGame->pfnEndGame = GameModeSkillZoneHorse_EndGame;
    gpGame->pfn26C = GameModeSkillZoneHorse_GreenType;
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
    gpGame->n10 = 2;
    gpGame->nC = 2;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    lbl_80282360 = 0;
    gSession.nSplitScreen = 0;
    gSession.nPinSet = 0;
}

// Puts back the two options StartGamePreData changed for the game: options.nC and the wind.
void GameModeSkillZoneHorse_Shutdown(void) {
    gSession.options.nC = gHorseSavedOptionsC;
    gSession.options.nWind = gHorseSavedWind;
}

// As a round starts (pfn1EC, GM_InitModule_PreDataStream): saves options.nC and the wind setting
// (Shutdown puts them back) and sets them to 4 and 0, no wind.
void GameModeSkillZoneHorse_StartGamePreData(void) {
    gHorseSavedOptionsC = gSession.options.nC;
    gHorseSavedWind = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
}

u8 GameModeSkillZoneHorse_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Who plays next (pfnGetHonors): player 0 while nobody has a stroke on the hole; after that the
// next player in turn after the current golfer (lbl_80282278) who is not nPlayer and is still in
// (fewer than 5 letters, nE88); 5 when there is none.
s32 GameModeSkillZoneHorse_GetHonors(int nPlayer) {
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
        if (n != nPlayer && gPlayers[n].nE88 < 5) {
            return n;
        }
    }
    return 5;
}

// End of a golfer's turn (pfnEndGolferTurn): the ball goes back on the player's tee (with in-flight
// replays on, gReplayData.bF10, the replay's saved ball is copied back instead) and the shots taken
// (nDC0) are counted.
void GameModeSkillZoneHorse_EndGolferTurn(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer].ball, &gReplayData.player.ball, sizeof(Ball));
    } else {
        Physics_InitBall(&gPlayers[nPlayer].ball,
                    &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
    }
    gPlayers[nPlayer].nDC0++;
}

// Scores a shot once the ball stops (pfn244 from GM_Earnings_PayShotGoals; BallOOB too). After a
// shot-clock timeout (gHorseShotClockOut) it only shows text 0xD1 and comment 0x14. A landing on a
// target short of the drive line (GameModeSkillZoneBase_IsLongDrive) is in one of its rings (0 the
// bullseye .. 4) and counts as a hit (nDE4 per target, aDC4[3]); its surface's points (GetIDScore,
// gHorseShotPoints; positive ones with the earnings modifiers) go to the winnings (nDD8, never
// below 0) and, outside replays, float up at the ball's screen position (message 0x33). With a
// leader (gHorseLeader): the leader's target in the leader's ring matches (comment 0x32 for a
// bullseye, else 0x11); the leader's target in a closer ring takes the lead (text 0xCF,
// gHorseLastShotExceeded set, comment 0x31 or 0x33); anything else takes a letter (nE88, message
// 0x38 with the count, Gaud_LetterGained; text 0xCE on the leader's target, else 0xD0) and ends the
// lead. Without a leader the shot sets the lead: its target and ring (a comment per ring). A shot
// off the targets takes a letter and ends the lead when there is one (text 0xCE, a comment per
// letter count 1..5); without one it does nothing. Rings, bullseyes and the shot multiplier (nDBC)
// play their sounds and ball effects.
void GameModeSkillZoneHorse_CheckShotAwards(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nRing;
    s32 nMsg;
    f32 fLength;
    s32 nMult;
    Ball* pBall;
    f32 x;
    f32 y;
    nMsg = -1;
    if (gHorseShotClockOut) {
        GameMsg_Send5Ints(0x33, 0, 0, 0, 0xD1, 1);
        nMsg = 0x14;
    } else {
        nSurface = gPlayers[nPlayer].ball.nSurface;
        fLength = fn_800D0550(nPlayer);
        GameModeSkillZoneHorse_GetIDScore(nSurface, &gHorseShotPoints);
        if (nSurface >= 0x85 && nSurface <= 0x90 && !GameModeSkillZoneBase_IsLongDrive(nPlayer, fLength)) {
            nTarget = GameModeSkillZoneBase_GetGreenIndexHit(nPlayer);
            nRing = GameModeSkillZoneBase_GetBullsEyeColor(nSurface);
            gPlayers[nPlayer].nDE4[nTarget]++;
            gPlayers[nPlayer].aDC4[3]++;
            if (gHorseShotPoints != 0) {
                if (gHorseShotPoints > 0) {
                    gHorseShotPoints = GM_Earnings_ComputeBonusModifiers(gHorseShotPoints,
                                                                         nPlayer, 1, 1, 1, 0);
                    gHorseShotPoints = GM_Earnings_ComputeTOURCardModifiers(gHorseShotPoints, nPlayer, 0);
                }
                gPlayers[nPlayer].nDD8 += gHorseShotPoints;
                if (gPlayers[nPlayer].nDD8 < 0) {
                    gPlayers[nPlayer].nDD8 = 0;
                }
                if (!gSession.bReplay) {
                    fn_8006434C(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]),
                                gPlayers[nPlayer].ball.vPrev, &x, &y,
                                0);
                    fn_8006A8D4(ViewController_GetRenderContext(gPlayers[nPlayer].nView[0]), &x, &y);
                    GameMsg_Send5Ints(0x33, gHorseShotPoints, 512.0f * x, 448.0f * y, nSurface, 1);
                }
            }
            if (gHorseLeader != 5) {
                if (nTarget == gHorseLeaderTarget && nRing == gHorseLeaderRing) {
                    GameMsg_Send5Ints(0x33, 0, 0, 0, nSurface, 1);
                    if (nRing == 0) {
                        Gaud_BullsEye();
                        nMsg = 0x32;
                        pBall = &gPlayers[nPlayer].ball;
                        fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                        }
                    } else {
                        Gaud_ScoreInRing();
                        nMsg = 0x11;
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                        }
                    }
                } else if (nTarget == gHorseLeaderTarget && nRing < gHorseLeaderRing) {
                    gHorseLeader = nPlayer;
                    gHorseLeaderRing = nRing;
                    gHorseLeaderTarget = nTarget;
                    GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCF, 1);
                    gHorseLastShotExceeded = 1;
                    if (nRing == 0) {
                        Gaud_BullsEye();
                        nMsg = 0x31;
                        pBall = &gPlayers[nPlayer].ball;
                        fn_800A30E4(8, pBall, nPlayer, 0, 0.0f);
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, pBall, nPlayer, 0, 0.0f);
                        }
                    } else {
                        Gaud_ScoreInRing();
                        nMsg = 0x33;
                        nMult = gPlayers[nPlayer].nDBC;
                        if (nMult > 1) {
                            fn_800A30E4(nMult + 7, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
                        }
                    }
                } else {
                    gHorseLeader = 5;
                    gPlayers[nPlayer].nE88++;
                    GameMsg_SendInt(0x38, gPlayers[nPlayer].nE88);
                    Gaud_LetterGained();
                    if (nTarget == gHorseLeaderTarget) {
                        GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCE, 1);
                        if (!(Misc_RandFunc(0) & 1)) {
                            nMsg = 0xE;
                        } else {
                            nMsg = 0x10;
                        }
                    } else {
                        GameMsg_Send5Ints(0x33, 0, 0, 0, 0xD0, 1);
                        if (!(Misc_RandFunc(0) & 1)) {
                            nMsg = 0xA;
                        } else {
                            nMsg = 0xC;
                        }
                    }
                }
            } else {
                gHorseLeader = nPlayer;
                gHorseLeaderRing = nRing;
                gHorseLeaderTarget = nTarget;
                GameMsg_Send5Ints(0x33, 0, 0, 0, nSurface, 1);
                if (nRing == 0) {
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
        } else if (gHorseLeader != 5) {
            gHorseLeader = 5;
            gPlayers[nPlayer].nE88++;
            GameMsg_Send5Ints(0x33, 0, 0, 0, 0xCE, 1);
            GameMsg_SendInt(0x38, gPlayers[nPlayer].nE88);
            Gaud_LetterGained();
            switch (gPlayers[nPlayer].nE88) {
            case 1:
                if (!(Misc_RandFunc(0) & 1)) {
                    nMsg = 3;
                } else {
                    nMsg = 4;
                }
                break;
            case 2:
                nMsg = 5;
                break;
            case 3:
                nMsg = 6;
                break;
            case 4:
                if (!(Misc_RandFunc(0) & 1)) {
                    nMsg = 7;
                } else {
                    nMsg = 8;
                }
                break;
            case 5:
                nMsg = 9;
                break;
            }
        } else {
            gHorseLeader = 5;
        }
    }
    if (nMsg != -1) {
        GameModeSkillZoneBase_StartComment(nMsg);
    }
}

// Before each shot (pfnSetupNextGolfer): the shot-clock timeout flag (gHorseShotClockOut) and the
// per-shot data clear (ClearPerShotData) and stroke play's golfer order runs
// (GameModeStroke_SetupNextGolfer). The golfer about to play (GS_PRE_SHOT) gets a 900 shot clock
// (GameModeSkillZoneCapture_SetShotClock); if they are the leader, play has come round to them
// unmatched and the lead ends (comment 0x23 or 0x24). While there is a leader the golfer is aimed
// at the leader's target (Player.nTarget, SetCup_AlignGolfer); with none, only before their first
// shot, at their own target.
void GameModeSkillZoneHorse_SetupNextGolfer(void) {
    int i;
    gHorseShotClockOut = 0;
    GameModeSkillZoneBase_ClearPerShotData();
    GameModeStroke_SetupNextGolfer();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if ((s8)GOLFERSTATE_GetCurrentState(i) == 1) {
            GameModeSkillZoneCapture_SetShotClock(900);
            if (gHorseLeader == i) {
                gHorseLeader = 5;
                if (!(Misc_RandFunc(0) & 1)) {
                    GameModeSkillZoneBase_PlayComment(0x23, 0);
                } else {
                    GameModeSkillZoneBase_PlayComment(0x24, 0);
                }
            }
            if (gHorseLeader != 5 || PLAYER(i)->nDC0 == 0) {
                if (gHorseLeader != 5) {
                    PLAYER(i)->nTarget = gHorseLeaderTarget;
                }
                GameModeSkillZoneBase_SetCup_AlignGolfer(i, (s8)PLAYER(i)->nTarget);
            }
        }
    }
}

// Hole start (pfn1E4): the targets sorted nearest the tee first (SortCupsByDistanceFromTee), then
// ClearPerHoleData.
void GameModeSkillZoneHorse_LoadHole(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    GameModeSkillZoneHorse_ClearPerHoleData();
}

// The hole restarts (pfn224, GM_RestartHole): the lead cleared (ClearPerHoleData) and player 0
// given the default aim.
void GameModeSkillZoneHorse_RestartHole(void) {
    GameModeSkillZoneHorse_ClearPerHoleData();
    AI_DefaultTarget(0);
}

// The shared per-hole clear (GameModeSkillZoneBase_ClearPerHoleData), then no leader: gHorseLeader
// and gHorseLeaderRing 5, gHorseLeaderTarget 0, gHorseLastShotExceeded clear.
void GameModeSkillZoneHorse_ClearPerHoleData(void) {
    GameModeSkillZoneBase_ClearPerHoleData();
    gHorseLeader = 5;
    gHorseLeaderRing = 5;
    gHorseLeaderTarget = 0;
    gHorseLastShotExceeded = 0;
}

// Every frame of the swing state (pfn228): once the swing has begun (SwingData.nState not
// SW_IDLE_SWING), UI message 0x36 (no value) is sent.
void GameModeSkillZoneHorse_UpdateSwingUI(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

// The game is over once its one hole is: always 1.
u8 GameModeSkillZoneHorse_GameFinished(u8 bCheck) {
    return 1;
}

// The ball went out of bounds (pfn250): the shot is scored as usual (CheckShotAwards).
void GameModeSkillZoneHorse_BallOOB(int nPlayer) {
    GameModeSkillZoneHorse_CheckShotAwards(nPlayer);
}

// The hole (and so the game) is over once at most one player is still in (fewer than 5 letters,
// nE88): 1, else 0. nPlayer and bCheck are not used.
s32 GameModeSkillZoneHorse_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    s32 n = 0;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nE88 < 5) {
            n++;
        }
    }
    return n <= 1;
}

// Aim at the previous target (pfn264, a re-plan), only without a leader
// (GameModeSkillZoneBase_PickPrevTarget); with one, the player's target (the leader's) is set
// again. Always 1.
u8 GameModeSkillZoneHorse_PickPrevTarget(int nPlayer) {
    if (gHorseLeader == 5) {
        GameModeSkillZoneBase_PickPrevTarget(nPlayer);
    } else {
        GameModeSkillZoneBase_SetCup(nPlayer, (s8)gPlayers[nPlayer].nTarget);
    }
    return 1;
}

// Aim at the next target (pfn258, a re-plan), only without a leader
// (GameModeSkillZoneBase_PickTarget); with one, the player's target (the leader's) is set again.
// Always 1.
u8 GameModeSkillZoneHorse_PickTarget(int nPlayer) {
    if (gHorseLeader == 5) {
        GameModeSkillZoneBase_PickTarget(nPlayer);
    } else {
        GameModeSkillZoneBase_SetCup(nPlayer, (s8)gPlayers[nPlayer].nTarget);
    }
    return 1;
}

// The ring the leader landed in, for the HUD (UI command 21): 0 the bullseye .. 4; -1 when there is
// no leader.
s8 GameModeSkillZoneHorse_GetCurrentLeaderRing(void) {
    if ((s32) gHorseLeader != 5) {
        return (s8) gHorseLeaderRing;
    }
    return -1;
}

// The ball was hit (pfn260): the shot clock is switched off (-1), its ticking stopped and
// gHorseLastShotExceeded cleared.
void GameModeSkillZoneHorse_HitBall(int nPlayer) {
    GameModeSkillZoneCapture_SetShotClock(-1);
    Gaud_StopShotClock();
    gHorseLastShotExceeded = 0;
}

// The shot clock ran out (GameModeSkillZoneBase_ShotClockOut, from a UI command): the current
// golfer goes to GS_SIMULATE, the toggle UI hides, the ticking stops and gHorseShotClockOut is set,
// so CheckShotAwards scores nothing and shows text 0xD1. With a leader, the player who is not the
// leader takes a letter (player 1 when the leader is player 0, else player 0: two players only;
// message 0x38, Gaud_LetterForfeit).
void GameModeSkillZoneHorse_ShotClockOut(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    GUI_HideAllToggleUI();
    Gaud_StopShotClock();
    gHorseShotClockOut = 1;
    if (gHorseLeader != 5) {
        if (gHorseLeader == 0) {
            gPlayers[1].nE88++;
            GameMsg_SendInt(0x38, gPlayers[1].nE88);
        } else {
            gPlayers[0].nE88++;
            GameMsg_SendInt(0x38, gPlayers[0].nE88);
        }
        Gaud_LetterForfeit();
    }
}

// End of the game (pfnEndGame): the game counts as won in the bio (EASBio_SetCurrentGameWon); each
// player still in (fewer than 5 letters) is paid their winnings (nDD8, GM_Earnings_AwardMoney), the
// others lose theirs (nDD8 0).
void GameModeSkillZoneHorse_EndGame(void) {
    int i;
    EASBio_SetCurrentGameWon(1);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nE88 < 5) {
            GM_Earnings_AwardMoney(i, PLAYER(i)->nDD8, 0);
        } else {
            PLAYER(i)->nDD8 = 0;
        }
    }
}

// The points for landing on surface nSurface: the n10 of its gEarningsTable.aMini row (the last of
// the 20 rows with that id), 0 when no row has it.
void GameModeSkillZoneHorse_GetIDScore(s32 nSurface, s32* pPoints) {
    int i;
    *pPoints = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].n10;
        }
    }
}

// Whether the last shot took the lead from a leader (gHorseLastShotExceeded), for the HUD (UI
// command 26): 1 or 0.
s32 GameModeSkillZoneHorse_GetLastShotExceeded(void) {
    return ((u32)((-gHorseLastShotExceeded) | gHorseLastShotExceeded) >> 31);
}

// Which marker model target nTarget shows (pfn26C, GoDynObj.c): with a leader, 1 for every target
// but the leader's; 0 for the leader's and for all when there is none. nPlayer is not used.
s32 GameModeSkillZoneHorse_GreenType(int nPlayer, int nTarget) {
    if (gHorseLeader == 5 || nTarget == gHorseLeaderTarget) {
        return 0;
    }
    return 1;
}
