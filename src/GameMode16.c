// GameMode16.c (our name): game mode 16. Each player has 20 balls (nDC0) to hit the targets in any
// order; a target pays up to 4 times, hitting every target pays the prize row's bonus, and the
// bullseyes, streaks and the longest shot are counted. Hitting world objects on the way (pfn268)
// raises a points multiplier.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/earnings.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

s32 gTargetSavedOptionsC = 4;                    // the options saved while the game runs
s32 gTargetShotPoints;                    // the points of the last shot
s32 gTargetBonusMultiplier;                    // the points multiplier from bonuses
s32 gTargetSavedWind;

void  fn_800F4D6C(void);
void  fn_800F4D88(void);
u8    fn_800F4DB4(u8 bCheck);
s32   fn_800F4DBC(int nPlayer);
void  fn_800F4F40(int nPlayer);
void  fn_800F5014(int nPlayer);
void  fn_800F56D4(void);
void  fn_800F577C(void);
void  fn_800F57A0(void);
void  fn_800F57C8(void);
void  fn_800F5808(int nPlayer);
u8    fn_800F5848(u8 bCheck);
void  fn_800F5850(int nPlayer);
u8    fn_800F5870(int nPlayer, u8 bCheck);
void  fn_800F58B4(s32 nSurface, s32* pPoints);
s32   fn_800F59DC(int nPlayer, int i);
void  fn_800F5A14(int nPlayer, int nId);
void  fn_800F5A88(void);

// Mode 16 starts: no wind, no gimmes, no mulligans.
void fn_800F4B40(void) {
    gpGame->pfnInit = fn_800F4B40;
    gpGame->pfnShutdown = fn_800F4D6C;
    gpGame->pfnSetupNextGolfer = fn_800F56D4;
    gpGame->pfnGetHonors = fn_800F4DBC;
    gpGame->pfnHoleFinished = fn_800F5870;
    gpGame->pfnGameFinished = fn_800F5848;
    gpGame->pfnGoToPlayoff = fn_800F4DB4;
    gpGame->pfnEndGolferTurn = fn_800F4F40;
    gpGame->pfn244 = fn_800F5014;
    gpGame->pfn1E4 = fn_800F577C;
    gpGame->pfn228 = fn_800F5808;
    gpGame->pfn224 = fn_800F57A0;
    gpGame->pfn1EC = fn_800F4D88;
    gpGame->pfn250 = fn_800F5850;
    gpGame->pfn264 = GameModeSkillZoneBase_PickPrevTarget;
    gpGame->pfn258 = GameModeSkillZoneBase_PickTarget;
    gpGame->pfn26C = fn_800F59DC;
    gpGame->pfn268 = fn_800F5A14;
    gpGame->pfnEndGame = fn_800F5A88;
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

void fn_800F4D6C(void) {
    gSession.options.nC = gTargetSavedOptionsC;
    gSession.options.nWind = gTargetSavedWind;
}

void fn_800F4D88(void) {
    gTargetSavedOptionsC = gSession.options.nC;
    gTargetSavedWind = gSession.options.nWind;
    gSession.options.nC = 4;
    gSession.options.nWind = 0;
}

u8 fn_800F4DB4(u8 bCheck) {
    return 0;
}

// Who plays next: player 0 first, then the players with balls left, in turn.
s32 fn_800F4DBC(int nPlayer) {
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

// End of a golfer's turn: the ball goes back to the tee, one ball fewer; count multiplied shots.
void fn_800F4F40(int nPlayer) {
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

// The ball stopped: score the target, the all-targets bonus, and the longest shot.
void fn_800F5014(int nPlayer) {
    s32 nSurface;
    s8 nTarget;
    s32 nMsg;
    f32 fLength;
    s32 nMult;
    Ball* pBall;
    nMsg = -1;
    nSurface = gPlayers[nPlayer].ball.nSurface;
    fLength = fn_800D0550(nPlayer);
    fn_800F58B4(nSurface, &gTargetShotPoints);
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

// Next turn: each player in pre-shot may get a shot multiplier, and is re-aimed at their target
// only when out of balls; the bonus multiplier goes back to 1.
void fn_800F56D4(void) {
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

void fn_800F577C(void) {
    GameModeSkillZoneBase_SortCupsByDistanceFromTee();
    fn_800F57C8();
}

void fn_800F57A0(void) {
    fn_800F57C8();
    AI_DefaultTarget(0);
}

// 20 balls each.
void fn_800F57C8(void) {
    int i;
    GameModeSkillZoneBase_ClearPerHoleData();
    i = 0;
    while (i < 5) {
        gPlayers[i++].nDC0 = 20;
    }
}

void fn_800F5808(int nPlayer) {
    if (gPlayers[nPlayer].swing.nState != 0) {
        GameMsg_Send(0x36);
    }
}

u8 fn_800F5848(u8 bCheck) {
    return 1;
}

void fn_800F5850(int nPlayer) {
    fn_800F5014(nPlayer);
}

// The game is over when nobody has a ball left.
u8 fn_800F5870(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nDC0 != 0) {
            return 0;
        }
    }
    return 1;
}

// The points for landing on a surface.
void fn_800F58B4(s32 nSurface, s32* pPoints) {
    int i;
    *pPoints = 0;
    for (i = 0; i < 20; i++) {
        if (nSurface == gEarningsTable.aMini[i].nId) {
            *pPoints = gEarningsTable.aMini[i].n8;
        }
    }
}

s32 fn_800F59CC(s32 a) {
    return gTargetShotPoints;
}

s32 fn_800F59D4(s32 a) {
    return gTargetBonusMultiplier;
}

// A target's state for the HUD: 1 when it has paid out 4 times (closed).
s32 fn_800F59DC(int nPlayer, int i) {
    if (gPlayers[nPlayer].nDE4[i] > 3) {
        return 1;
    }
    return 0;
}

// The ball hit a world object (nId): the points multiplier goes up by 2 to 6.
void fn_800F5A14(int nPlayer, int nId) {
    s32 n = GameModeSkillZoneBase_GetBonusIndex(nId);
    fn_800A30E4(8, &gPlayers[nPlayer].ball, nPlayer, 0, 0.0f);
    gTargetBonusMultiplier += n + 2;
}

void fn_800F5A88(void) {
    EASBio_SetCurrentGameWon(1);
}
