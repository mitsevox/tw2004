// HoleScore.c (our name; TW06's golf/gamemode/analysisutilities.c, medium evidence: TW06 names
// GameAnalysis_IsPuttFor GameAnalysis_IsPuttFor): per-player round analysis for the situation
// scripts, the earnings and the game modes: distances to the pin, the ground the shot started from,
// and counts and streaks of holes by score against par.

#include "golfer.h"
#include "game.h"

u8  lbl_80282240;

f32  GameAnalysis_GetInitialDistanceToPin(int nPlayer);
f32  GameAnalysis_GetPositionDistanceToPin(f32* pPos);
u32  GameAnalysis_GetInitialStartingLie(int nPlayer);
u8   GameAnalysis_GetFairwayDrive(int nPlayer);
int  GameAnalysis_CountHoleScoresOrBetter(int nPlayer, int nToPar);
int  GameAnalysis_CountHolesOverPar(int nPlayer);
int  GameAnalysis_CountStreakHoleScores(int nPlayer, int nToPar);
int  GameAnalysis_CountTotalPutts(int nPlayer);
void GameAnalysis_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void fn_800C8C3C(int nView, f32* pOut);   // GoBreakLine: a point kept per view

// gpGame->pfnIsPuttForLead: whether holing this ball would put the player in the lead (the others'
// balls not yet holed counting one more stroke). Strokes (scoring kind 0): only when not leading
// already. Holes won (1): level on holes and beating the best other score on this hole by more
// than a stroke. Skins (2): the same, from level or behind, when this hole's skin would lift the
// player past the best.
u8 GameAnalysis_IsPuttForLead(int nPlayer) {
    int anTotal[4];   // one per player set up, as in GameAnalysis_GetCurrentEventLead
    int i;
    int nKind;
    int nMineStrokes;
    int nMine;
    int nBest;
    int nOther;
    int nBestStrokes;

    nKind = GM_GetScoringType();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = GM_GetGolferRelativeCumulativeScore(i, 0);
                if (i == nPlayer) {
                    nMine = anTotal[i];
                } else if (anTotal[i] < nBest) {
                    nBest = anTotal[i];
                }
            }
        }
        if (nMine >= nBest) {
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (!gPlayers[i].bPlayerCut) {
                    if (i == nPlayer) {
                        nMineStrokes = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()] + 1 -
                                       GM_GetCurrentHolePar();
                    } else {
                        nOther = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()]
                                - GM_GetCurrentHolePar();
                        if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                            nOther++;
                        }
                        if (nOther < nBest) {
                            nBest = nOther;
                        }
                    }
                }
            }
            return nMineStrokes < nBest;
        }
        return 0;
    } else if (nKind == 1) {
        nBest = -1;
        nBestStrokes = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nHolesWon;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].nHolesWon > nBest) {
                    nBest = gPlayers[i].nHolesWon;
                }
                nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                    nOther++;
                }
                if (nOther < nBestStrokes) {
                    nBestStrokes = nOther;
                }
            }
        }
        if (nMine == nBest && nMineStrokes < nBestStrokes - 1) {
            return 1;
        }
        return 0;
    } else if (nKind == 2) {
        nBest = -1;
        nBestStrokes = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nSkinsTotal;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].nSkinsTotal > nBest) {
                    nBest = gPlayers[i].nSkinsTotal;
                }
                nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                    nOther++;
                }
                if (nOther < nBestStrokes) {
                    nBestStrokes = nOther;
                }
            }
        }
        if (nMine <= nBest && nMineStrokes < nBestStrokes - 1 && nMine + GameModeSkins_CurrentHoleValue() > nBest) {
            return 1;
        }
        return 0;
    }
    return 0;
}

// gpGame->pfnIsPuttForWin: whether holing this ball would win (the others' balls not yet holed
// counting one more stroke). Strokes (scoring kind 0): only on the round's last hole or with
// gpGame->bInPlayoff, beating the best other total. Holes won (1): winning this hole puts the
// player more holes up than are left, or halving it (beating the best by less than two) already
// does. Skins (2): winning this hole's skin lifts the player past the best.
u8 GameAnalysis_IsPuttForWin(int nPlayer) {
    int anTotal[4];   // one per player set up, as in GameAnalysis_GetCurrentEventLead
    int nMineStrokes;
    int i;
    int nKind;
    int nLeft;
    int nMine;
    int nBest;
    int nOther;
    int nBestStrokes;

    nKind = GM_GetScoringType();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    nLeft = GM_GetNumHolesRemainingInRound();
    if (nKind == 0) {
        if (nLeft != 1 && !GM_IsPlayoff()) {
            return 0;
        }
        nBest = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = GM_GetGolferRelativeCumulativeScore(i, 0);
                if (i != nPlayer && anTotal[i] < nBest) {
                    nBest = anTotal[i];
                }
            }
        }
        // EA bug: nMineStrokes is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                if (i == nPlayer) {
                    nMineStrokes = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()] + 1 -
                                   GM_GetCurrentHolePar();
                } else {
                    nOther = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()] - GM_GetCurrentHolePar();
                    if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                        nOther++;
                    }
                    if (nOther < nBest) {
                        nBest = nOther;
                    }
                }
            }
        }
        return nMineStrokes < nBest;
    } else if (nKind == 1) {
        nBest = -1;
        nBestStrokes = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nHolesWon;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].nHolesWon > nBest) {
                    nBest = gPlayers[i].nHolesWon;
                }
                nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                    nOther++;
                }
                if (nOther < nBestStrokes) {
                    nBestStrokes = nOther;
                }
            }
        }
        if (nMineStrokes < nBestStrokes - 1 && nMine + 1 - nBest > nLeft - 1) {
            return 1;
        }
        if (nMineStrokes < nBestStrokes && nMine - nBest > nLeft - 1) {
            return 1;
        }
        return 0;
    } else if (nKind == 2) {
        nBest = -1;
        nBestStrokes = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nSkinsTotal;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].nSkinsTotal > nBest) {
                    nBest = gPlayers[i].nSkinsTotal;
                }
                nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                    nOther++;
                }
                if (nOther < nBestStrokes) {
                    nBestStrokes = nOther;
                }
            }
        }
        if (nMineStrokes < nBestStrokes - 1 && nMine + GameModeSkins_CurrentHoleValue() > nBest) {
            return 1;
        }
        return 0;
    }
    return 0;
}

// For a human player: whether the look-ahead ball (Player.ballBefore) would set a record, by
// HighScoreRecords_GetEndOfShotRecord or, when that ball ends in the cup, by
// HighScoreRecords_GetEndOfHoleRecord. 0 for a CPU player.
u8 GameAnalysis_IsPredictedBallRecord(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (HighScoreRecords_GetEndOfShotRecord(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        HighScoreRecords_GetEndOfHoleRecord(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    return 0;
}

// As GameAnalysis_IsPredictedBallRecord, for the earnings awards: Earnings_CheckShotAwards or, when
// Player.ballBefore ends in the cup, Earnings_CheckPuttAwards. 0 for a CPU player.
u8 GameAnalysis_IsPredictedBallTrophy(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (Earnings_CheckShotAwards(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        Earnings_CheckPuttAwards(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    return 0;
}

// For a human player, the course records (gSession.aCourseRecord, kind k's best) this shot
// keeps in reach, as bits, each only while goals of its kind count
// (HighScoreRecords_CheckRecordGameSetting) and, but for kinds 1 and 2, outside a playoff: 0x1 on
// the last hole, the round would beat kind 0's with the tap-in; 0x2 the ball is on the tee; 0x4 on
// the green or fringe, a putt of 3 x fA64 beats kind 2's; 0x8 off the green two under par or
// better, one more of GameAnalysis_CountTotalGIRs beats kind 3's; 0x10 on the last hole, the
// round's putts (one more on the green or fringe) beat kind 4's; 0x20 on the tee of a par 4 or 5, one more of
// GameAnalysis_CountTotalFairways beats kind 5's; 0x40 and 0x80 two under / one under par or
// better, one more eagle (GameAnalysis_NumEaglesSoFarThisRound) / birdie
// (GameAnalysis_NumBirdiesSoFarThisRound) beats kinds 6 / 7.
u32 GameAnalysis_IsShotForRecord(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    u32 uFlags = 0;
    int nPutts;
    int i;
    Ball* pBall = &pPlayer->ball;

    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (HighScoreRecords_CheckRecordGameSetting(0) && !gpGame->bInPlayoff && Game_CurHoleIndex() == 17) {
        if (GM_GetPlayerRoundStrokes(nPlayer) + 1
            < gSession.aCourseRecord[Game_GetCourse()].aRecord[0][0].nValue) {
            uFlags |= 0x1;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(1) && pBall->nLie == LIE_TEE_e) {
        uFlags |= 0x2;
    }
    if (HighScoreRecords_CheckRecordGameSetting(2)
        && (pBall->nLie == LIE_GREEN_e || pBall->nLie == LIE_FRINGE_e) &&
        3.0f * gPlayers[nPlayer].fA64 > gSession.aCourseRecord[Game_GetCourse()].aRecord[2][0].nValue) {
        uFlags |= 0x4;
    }
    if (HighScoreRecords_CheckRecordGameSetting(3) && !gpGame->bInPlayoff && pBall->nLie != LIE_GREEN_e
        && pBall->nLie != LIE_FRINGE_e &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= GM_GetCurrentHolePar() - 2) {
        if (GameAnalysis_CountTotalGIRs(nPlayer, 0) + 1
            > gSession.aCourseRecord[Game_GetCourse()].aRecord[3][0].nValue) {
            uFlags |= 0x8;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(4) && !gpGame->bInPlayoff && Game_CurHoleIndex() == 17) {
        nPutts = 0;
        for (i = 0; i < 18; i++) {
            nPutts += pPlayer->nPutts[i];
        }
        if (pBall->nLie == LIE_GREEN_e || pBall->nLie == LIE_FRINGE_e) {
            nPutts++;
        }
        if (nPutts < gSession.aCourseRecord[Game_GetCourse()].aRecord[4][0].nValue) {
            uFlags |= 0x10;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(5) && !gpGame->bInPlayoff && pBall->nLie == LIE_TEE_e
        && GM_GetCurrentHolePar() >= 4) {
        if (GameAnalysis_CountTotalFairways(nPlayer) + 1
            > gSession.aCourseRecord[Game_GetCourse()].aRecord[5][0].nValue) {
            uFlags |= 0x20;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(6) && !gpGame->bInPlayoff &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= GM_GetCurrentHolePar() - 2) {
        if (GameAnalysis_NumEaglesSoFarThisRound(nPlayer, 0, 0) + 1
            > gSession.aCourseRecord[Game_GetCourse()].aRecord[6][0].nValue) {
            uFlags |= 0x40;
        }
    }
    if (HighScoreRecords_CheckRecordGameSetting(7) && !gpGame->bInPlayoff &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= GM_GetCurrentHolePar() - 1) {
        if (GameAnalysis_NumBirdiesSoFarThisRound(nPlayer, 0, 0) + 1
            > gSession.aCourseRecord[Game_GetCourse()].aRecord[7][0].nValue) {
            uFlags |= 0x80;
        }
    }
    return uFlags;
}

// Runs the shot, putt and (outside a playoff) hole checks of Earnings.c for the player as a
// preview (bPreview 1) and returns the ids 0..22 they list, as a bit set.
u32 GameAnalysis_IsShotForTrophyBall(int nPlayer) {
    u32 uIds = 0;
    u32 nId;
    int i;

    GM_Earnings_CheckShotGoals(nPlayer, NULL, 1);
    for (i = 0; i < Earnings_GetNumAwards(); i++) {
        nId = Earnings_GetShotAwardId(i);
        if (nId <= 22) {
            uIds |= 1 << nId;
        }
    }
    GM_Earnings_CheckPuttGoals(nPlayer, 1);
    for (i = 0; i < Earnings_GetNumAwards(); i++) {
        nId = Earnings_GetPuttAwardId(i);
        if (nId <= 22) {
            uIds |= 1 << nId;
        }
    }
    if (!gpGame->bInPlayoff) {
        GM_Earnings_CheckHoleGoals(nPlayer, 1, 0);
        for (i = 0; i < Earnings_GetNumAwards(); i++) {
            nId = Earnings_GetHoleAwardId(i);
            if (nId <= 22) {
                uIds |= 1 << nId;
            }
        }
    }
    return uIds;
}

// gpGame->pfnGetCurrentLead (TW06: GetCurrentLead). The player's lead in the round so far, by the
// scoring kind (GM_GetScoringType): kind 0 (strokes), the best other total against par
// (GM_GetGolferRelativeCumulativeScore; players who missed the cut left out) less the player's;
// kinds 1 and 2, the player's holes won (skins) less the best of the others'. 0 when playing alone.
s32 GameAnalysis_GetCurrentEventLead(int nPlayer) {
    int anTotal[4];   // one per player set up; the frame has room for four
    int i;
    int nKind;
    int nMine;
    int nBest;

    nKind = GM_GetScoringType();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = GM_GetGolferRelativeCumulativeScore(i, 0);
                if (i == nPlayer) {
                    nMine = anTotal[i];
                } else if (anTotal[i] < nBest) {
                    nBest = anTotal[i];
                }
            }
        }
        return nBest - nMine;
    } else if (nKind == 1) {
        nBest = 0;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nHolesWon;
            } else if (gPlayers[i].nHolesWon > nBest) {
                nBest = gPlayers[i].nHolesWon;
            }
        }
        return nMine - nBest;
    } else if (nKind == 2) {
        nBest = 0;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nSkinsTotal;
            } else if (gPlayers[i].nSkinsTotal > nBest) {
                nBest = gPlayers[i].nSkinsTotal;
            }
        }
        return nMine - nBest;
    }
    return 0;
}

// The player's score against par for the round once the tap-in on this hole drops (EA's spelling,
// Potentail); 0 when GM_GetScoringType is not strokes (0).
int GameAnalysis_GetPotentailRoundParScore(int nPlayer) {
    int nPar = 0;
    int nStrokes = 0;
    int i;
    if (GM_GetScoringType()) {
        return 0;
    }
    for (i = 0; i < Game_CurHoleIndex(); i++) {
        nPar += GM_GetHoleIndexPar(i);
        nStrokes += gPlayers[nPlayer].nStrokes[i];
    }
    return (nStrokes - nPar) + gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - GM_GetCurrentHolePar();
}

// gpGame->pfnGetPotentialLead (TW06: GetPotentialLead).
// The player's lead (strokes, or holes or skins by the scoring kind GM_GetScoringType) once this hole's
// ball drops, 0 when playing alone. Kind 0 (strokes): the best other round total, with a holed
// ball's score on this hole, less the player's total with the tap-in; players who missed the cut
// are left out. Kinds 1 and 2: the lead from GM_GetCurrentEventLead, moved by
// GM_GetPotentialHoleResult's value: 3 no change, 2 up one (kind 2: up this hole's skin), 0 down the same.
s32 GameAnalysis_GetPotentialEventLead(int nPlayer) {
    int anTotal[4];   // one per player set up, as in GameAnalysis_GetCurrentEventLead
    int i;
    int nKind;
    int nMine;
    int nBest;
    int nOther;
    int nLead;
    int nHole;

    nKind = GM_GetScoringType();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = GM_GetGolferRelativeCumulativeScore(i, 0);
                if (i == nPlayer) {
                    nMine = anTotal[i];
                    nMine += gPlayers[i].nStrokes[Game_CurHoleIndex()] + 1 - GM_GetCurrentHolePar();
                } else {
                    nOther = anTotal[i];
                    if (gPlayers[i].ball.nLie == LIE_INCUP_e) {
                        nOther += gPlayers[i].nStrokes[Game_CurHoleIndex()] - GM_GetCurrentHolePar();
                    }
                    if (nOther < nBest) {
                        nBest = nOther;
                    }
                }
            }
        }
        return nBest - nMine;
    } else if (nKind == 1) {
        nLead = GM_GetCurrentEventLead(nPlayer);
        nHole = GM_GetPotentialHoleResult(nPlayer);
        if (nHole == 3) {
            return nLead;
        }
        if (nHole == 2) {
            return nLead + 1;
        }
        if (nHole == 0) {
            return nLead - 1;
        }
        return nLead;
    } else if (nKind == 2) {
        nLead = GM_GetCurrentEventLead(nPlayer);
        nHole = GM_GetPotentialHoleResult(nPlayer);
        if (nHole == 3) {
            return nLead;
        }
        if (nHole == 2) {
            return nLead + GameModeSkins_CurrentHoleValue();
        }
        if (nHole == 0) {
            nLead -= GameModeSkins_CurrentHoleValue();
        }
        return nLead;
    }
    return 0;
}

// Whether holing the ball now would finish the hole: puts the ball in the cup with one more stroke,
// asks the mode (gpGame->pfnHoleFinished) with lbl_80282240 set, so the match modes do not count
// the asking player's own side as done, then puts both back.
u8 GameAnalysis_ShotWouldEndHole(int nPlayer) {
    int nLie;
    int nStrokes;
    int bFinished;

    nLie = gPlayers[nPlayer].ball.nLie;
    gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    lbl_80282240 = 1;
    bFinished = gpGame->pfnHoleFinished(nPlayer, 1) != 0;
    lbl_80282240 = 0;
    gPlayers[nPlayer].ball.nLie = nLie;
    gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] = nStrokes;
    return bFinished;
}

// gpGame->pfnGetPotentialHoleResult.
// How the hole ends for the player if the ball drops now, against the best of the others (a
// ball not yet holed counts one more stroke; in mode 21 the other side is player 2 or 0): 2 the
// player wins it, 1 ties, 0 loses; 3 when playing alone or when holing would not end the hole.
s32 GameAnalysis_GetPotentialHoleResult(int nPlayer) {
    int i;
    int nMine;
    int nBest;
    int nOther;

    GM_GetScoringType();
    if (gNumPlayersSetUp == 1) {
        return 3;
    }
    if (!GameAnalysis_ShotWouldEndHole(nPlayer)) {
        return 3;
    }
    nBest = 1000;
    if (Game_GetMode() == 21) {
        nMine = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1;
        if (nPlayer == 0 || nPlayer == 1) {
            i = 2;
        } else {
            i = 0;
        }
        nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
        if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
            nOther++;
        }
        nBest = nOther;
    } else {
        // EA bug: nMine is never set when nPlayer is not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == nPlayer) {
                nMine = gPlayers[i].nStrokes[Game_CurHoleIndex()] + 1;
            } else {
                nOther = gPlayers[i].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[i].ball.nLie != LIE_INCUP_e) {
                    nOther++;
                }
                if (nOther < nBest) {
                    nBest = nOther;
                }
            }
        }
    }
    if (nMine < nBest) {
        return 2;
    }
    return nMine == nBest;
}

// The distance from the pin, along the ground, of where the player's ball lies now.
f32 GameAnalysis_GetCurrentDistanceToPin(int nPlayer) {
    return GameAnalysis_GetPositionDistanceToPin(gPlayers[nPlayer].ball.vPos);
}

// The distance from the pin, along the ground, of where the shot started (Ball.vStart).
f32 GameAnalysis_GetInitialDistanceToPin(int nPlayer) {
    return GameAnalysis_GetPositionDistanceToPin(gPlayers[nPlayer].ball.vStart);
}

// The distance from the pin, along the ground, of Player.ballBefore (the ball before the shot, then
// the look-ahead ball).
f32 GameAnalysis_GetEstimatedDistanceToPin(int nPlayer) {
    return GameAnalysis_GetPositionDistanceToPin(gPlayers[nPlayer].ballBefore.vPos);
}

// The class (SurfaceType.nClass) of the surface under Player.ballBefore (the look-ahead ball), -1
// when it has none.
int GameAnalysis_GetEstimatedLie(int nPlayer) {
    int nSurface = gPlayers[nPlayer].ballBefore.nSurface;
    if (nSurface >= 0) {
        return gSurfaceTypes[nSurface].nClass;
    }
    return -1;
}

// The distance along the ground (height left out) from where the shot started to where the ball is
// now.
f32 GameAnalysis_GetCurrentBallFlightDistance(int nPlayer) {
    f32 vDiff[3];
    GameAnalysis_Vec3Sub(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
    vDiff[1] = 0.0f;
    return Math_Sqrt(Vec3_LengthSqClamped(vDiff));
}

// A point's distance from the current pin along the ground; 0 with no hole loaded.
f32 GameAnalysis_GetPositionDistanceToPin(f32* pPos) {
    f32 vDiff[3];
    CourseInfo* pCourse = Ter_GetTGD();
    int nPin;
    if (pCourse == NULL) return 0.0f;
    nPin = Game_CurrentPinSet();
    GameAnalysis_Vec3Sub(pPos, &pCourse->pin[nPin].x, vDiff);
    vDiff[1] = 0.0f;
    return Math_Sqrt(Vec3_LengthSqClamped(vDiff));
}

// The round's holes the player finished one under par or better, counting back from the current
// hole (with bCurrent: TW07's includeCurrentHole) or the one before; with bOnlyFlagged (TW07's
// onlyCompletedHoles, which there tests GM_PlayerHoledOut), only holes whose gpGame->b16C entry is
// 1.
int GameAnalysis_NumBirdiesSoFarThisRound(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= GM_GetHoleIndexPar(i) - 1 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// As GameAnalysis_NumBirdiesSoFarThisRound, for holes two under par or better.
int GameAnalysis_NumEaglesSoFarThisRound(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= GM_GetHoleIndexPar(i) - 2 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The player's current run of holes one under par or better: counting back from the current hole
// (with bCurrent) or the one before, until a hole that is not (holes outside the round are
// skipped).
int GameAnalysis_CurrentBirdieStreak(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > GM_GetHoleIndexPar(i)
                - 1) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// As GameAnalysis_CurrentBirdieStreak, for holes two under par or better.
int GameAnalysis_CurrentEagleStreak(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > GM_GetHoleIndexPar(i)
                - 2) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// The putt's break angle (radians): from the shot's start, along the ground, the angle from the
// direction of the pin to that of the break line's point closest to the cup in the player's view
// (fn_800C8C3C); negative when the cross product's y is below 0.
f32 GameAnalysis_GetPuttBreakAngle(int nPlayer) {
    f32 vView[4];
    f32 vToPin[4];
    f32 vToView[4];
    f32 fCos;
    f32 fAngle;

    fn_800C8C3C(gPlayers[nPlayer].nView[0], vView);
    GameAnalysis_Vec3Sub(&gPlayers[nPlayer].ball.pCourse->pin[Game_CurrentPinSet()].x,
                         gPlayers[nPlayer].ball.vStart,
                vToPin);
    vToPin[1] = 0.0f;
    GameAnalysis_Vec3Sub(vView, gPlayers[nPlayer].ball.vStart, vToView);
    vToView[1] = 0.0f;
    if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vToPin)) > 0.0f) {
        LLMath_Normalize3(vToPin, vToPin);
    }
    if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vToView)) > 0.0f) {
        LLMath_Normalize3(vToView, vToView);
    }
    fCos = Vec3_Dot(vToView, vToPin);
    if (fCos < -1.0f) {
        fCos = -1.0f;
    } else if (fCos > 1.0f) {
        fCos = 1.0f;
    }
    fAngle = Math_Acos(fCos);
    if (vToView[2] * vToPin[0] - vToView[0] * vToPin[2] < 0.0f) {
        fAngle *= -1.0f;
    }
    return fAngle;
}

// What the putt is for: the hole's score against par once this stroke drops (strokes so far plus
// one, minus par); -1 a birdie putt, 0 par, 1 bogey.
int GameAnalysis_IsPuttFor(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - GM_GetCurrentHolePar();
}

// Whether the last hole played before this one was tied: nobody took mode points or skins
// (nSkinsWon) on it; 0 on the round's first hole.
u8 GameAnalysis_LastHoleWasTied(void) {
    int i;
    int j;
    if (GM_OnFirstSelectedHole()) {
        return 0;
    }
    for (i = Game_CurHoleIndex() - 1; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            for (j = 0; j < gNumPlayersSetUp; j++) {
                if (gPlayers[j].nModePoints[i] != 0 || gPlayers[j].nSkinsWon[i] != 0) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

// The class (SurfaceType.nClass) of the surface the shot started from (Ball.nStartSurface), 0 when
// it has none or is out of range.
u32 GameAnalysis_GetInitialStartingLie(int nPlayer) {
    if (gPlayers[nPlayer].ball.nStartSurface < 0 ||
        gPlayers[nPlayer].ball.nStartSurface >= NUM_SURFACE_TYPES) {
        return 0;
    }
    return gSurfaceTypes[gPlayers[nPlayer].ball.nStartSurface].nClass;
}

// The ground under Player.vBall is not green (class 3) and the hole's strokes so far are three
// under par or better (bUnder: more than three); without bAnyLie, the ball must also be on the
// green or in the cup and two under par (bUnder: more).
u8 GameAnalysis_CurrentShotGIR(int nPlayer, u8 bUnder, u8 bAnyLie) {
    SurfaceType* pSurface;
    int nPar;
    int nStrokes;
    int nLie;

    pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, gPlayers[nPlayer].vBall);
    if (pSurface != NULL) {
        nPar = GM_GetCurrentHolePar();
        if (bAnyLie) {
            nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
            if (bUnder) {
                if (pSurface->nClass != 3 && nStrokes < nPar - 3) {
                    return 1;
                }
            } else if (pSurface->nClass != 3 && nStrokes <= nPar - 3) {
                return 1;
            }
        } else {
            nLie = gPlayers[nPlayer].ball.nLie;
            nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
            if (bUnder) {
                if (pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
                    nStrokes < nPar - 2) {
                    return 1;
                }
            } else if (pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
                       nStrokes <= nPar - 2) {
                return 1;
            }
        }
    }
    return 0;
}

// Whether the shot started from fairway ground (surface class 1) and the ball now lies on the
// fairway (LIE_FAIRWAY_e), the green (LIE_GREEN_e) or in the cup (LIE_INCUP_e). Earnings.c's awards
// test it.
u8 GameAnalysis_GetFairwayDrive(int nPlayer) {
    if (GameAnalysis_GetInitialStartingLie(nPlayer) != 1) return 0;
    if (gPlayers[nPlayer].ball.nLie == 1 || gPlayers[nPlayer].ball.nLie == 9 ||
        gPlayers[nPlayer].ball.nLie == 12) {
        return 1;
    }
    return 0;
}

// The round's holes the player finished nToPar or better; below -3 a hole in one always counts.
int GameAnalysis_CountHoleScoresOrBetter(int nPlayer, int nToPar) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - GM_GetHoleIndexPar(i) <= nToPar ||
                (gPlayers[nPlayer].nStrokes[i] == 1 && nToPar < -3)) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The round's holes the player finished over par.
int GameAnalysis_CountHolesOverPar(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - GM_GetHoleIndexPar(i) >= 1) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The longest run of the round's holes finished nToPar or better (as
// GameAnalysis_CountHoleScoresOrBetter counts them).
int GameAnalysis_CountStreakHoleScores(int nPlayer, int nToPar) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - GM_GetHoleIndexPar(i) <= nToPar ||
                (gPlayers[nPlayer].nStrokes[i] == 1 && nToPar < -3)) {
                nRun++;
            } else {
                if (nRun > nBest) {
                    nBest = nRun;
                }
                nRun = 0;
            }
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The round's holes with bFairwayHit set (by position in the score block, TW06's fairways[]).
int GameAnalysis_CountTotalFairways(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].bFairwayHit[i]) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with bFairwayHit set; a par 3 does not break it.
int GameAnalysis_CountStreakFairways(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].bFairwayHit[i]) {
            nRun++;
        } else if (GM_GetHoleIndexPar(i) != 3) {
            if (nRun > nBest) {
                nBest = nRun;
            }
            nRun = 0;
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The round's holes with bGreenInReg set (TW06's gir[]); with bOnlyFlagged (TW07's
// onlyCompletedHoles, which there tests GM_PlayerHoledOut), only holes whose gpGame->b16C entry is
// 1.
int GameAnalysis_CountTotalGIRs(int nPlayer, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].bGreenInReg[i] &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with bGreenInReg set.
int GameAnalysis_CountStreakGIRs(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].bGreenInReg[i]) {
            nRun++;
        } else {
            if (nRun > nBest) {
                nBest = nRun;
            }
            nRun = 0;
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The player's putts over the round's holes.
int GameAnalysis_CountTotalPutts(int nPlayer) {
    int nPutts = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            nPutts += gPlayers[nPlayer].nPutts[i];
        }
    }
    return nPutts;
}

// The wind's heading against the player's aim, by quarter: 2 within 45 degrees of the aim, 4 the
// next quarter round, 1 the opposite quarter, 3 the last; 0 when the wind is 6 or less.
int GameAnalysis_vGetPlayerWindDirection(int nPlayer) {
    f32 vWind[4];
    f32 fAim = gPlayers[nPlayer].fAim;
    f32 fAngle;

    if (Wind_GetPhysicsWindVelocity(vWind) > 6.0f) {
        fAngle = atan2f(-vWind[0], vWind[2]) - fAim;
        while (fAngle < 0.0f) {
            fAngle += 2.0f * PI;
        }
        while (fAngle > 2.0f * PI) {
            fAngle -= 2.0f * PI;
        }
        if (fAngle >= 7.0f * PI / 4.0f || fAngle <= PI / 4.0f) {
            return 2;
        }
        if (fAngle >= PI / 4.0f && fAngle <= 3.0f * PI / 4.0f) {
            return 4;
        }
        if (fAngle >= 3.0f * PI / 4.0f && fAngle <= 5.0f * PI / 4.0f) {
            return 1;
        }
        if (fAngle >= 5.0f * PI / 4.0f && fAngle <= 7.0f * PI / 4.0f) {
            return 3;
        }
        return 0;
    }
    return 0;
}

// The slope of the ground under the ball across the player's aim, in whole degrees (+-90 when the
// ground's normal has no upward part); 0 with no ground or a normal not of length 1.
int GameAnalysis_GetSidehillLie(int nPlayer) {
    f32 vNormal[4];
    f32 vTurned[4];
    f32 fLength;
    f32 fAim;
    f32 fSin;
    f32 fCos;
    f32 fDegrees;
    f32 fEpsilon;

    if (!Ter_GetSupportingGroundNormal(Ter_GetTGD(), gPlayers[nPlayer].ball.vPos, vNormal)) {
        return 0;
    }
    fLength = Vec3_LengthSqClamped(vNormal);
    if (fLength > 1.01f || fLength < 0.99f) {
        return 0;
    }
    fAim = gPlayers[nPlayer].fAim;
    fSin = Math_Sin(fAim);
    fCos = Math_Cos(fAim);
    Vec3Copy(vNormal, vTurned);
    Ball_RotatePair(&vTurned[2], &vTurned[0], fSin, fCos);
    fEpsilon = 0.000001f;
    if (vTurned[1] < fEpsilon && vTurned[1] > -fEpsilon) {
        if (vTurned[0] < 0.0f) {
            fDegrees = -90.0f;
        } else {
            fDegrees = 90.0f;
        }
    } else {
        fDegrees = atan2f(vTurned[0], vTurned[1]) * (180.0f / PI);
    }
    return (int)fDegrees;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void GameAnalysis_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void GameAnalysis_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
