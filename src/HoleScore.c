// HoleScore.c (our name; TW06's golf/gamemode/analysisutilities.c, medium evidence: TW06 names
// Hole_ScoreAfterTapIn GameAnalysis_IsPuttFor): per-player round analysis for the situation
// scripts, the earnings and the game modes: distances to the pin, the ground the shot started from,
// and counts and streaks of holes by score against par.

#include "golfer.h"
#include "game.h"

u8  lbl_80282240;

f32  fn_800D04AC(int nPlayer);
f32  fn_800D05A4(f32* pPos);
u32  fn_800D0BAC(int nPlayer);
u8   fn_800D0D54(int nPlayer);
int  fn_800D0DC8(int nPlayer, int nToPar);
int  fn_800D0E74(int nPlayer);
int  fn_800D0F04(int nPlayer, int nToPar);
int  fn_800D1330(int nPlayer);
void fn_800D1674(f32* pA, f32* pB, f32* pOut);
void fn_800C8C3C(int nView, f32* pOut);   // GoBreakLine: a point kept per view

// gpGame->pfn1F8: whether holing this ball would put the player in the lead (the others' balls
// not yet holed counting one more stroke). Strokes (scoring kind 0): only when not leading
// already. Holes won (1): level on holes and beating the best other score on this hole by more
// than a stroke. Skins (2): the same, from level or behind, when this hole's skin would lift the
// player past the best.
u8 fn_800CF158(int nPlayer) {
    int anTotal[4];   // one per player set up, as in fn_800CFE74
    int i;
    int nKind;
    int nMineStrokes;
    int nMine;
    int nBest;
    int nOther;
    int nBestStrokes;

    nKind = fn_8008AB40();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = fn_800E1904(i, 0);
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
                                       fn_800D2B08();
                    } else {
                        nOther = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()] - fn_800D2B08();
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
                nMine = gPlayers[i].n274;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].n274 > nBest) {
                    nBest = gPlayers[i].n274;
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

// gpGame->pfn1FC: whether holing this ball would win (the others' balls not yet holed counting
// one more stroke). Strokes (scoring kind 0): only on the round's last hole or with gpGame->bD4,
// beating the best other total. Holes won (1): winning this hole puts the player more holes up
// than are left, or halving it (beating the best by less than two) already does. Skins (2):
// winning this hole's skin lifts the player past the best.
u8 fn_800CF450(int nPlayer) {
    int anTotal[4];   // one per player set up, as in fn_800CFE74
    int nMineStrokes;
    int i;
    int nKind;
    int nLeft;
    int nMine;
    int nBest;
    int nOther;
    int nBestStrokes;

    nKind = fn_8008AB40();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    nLeft = fn_8008AC00();
    if (nKind == 0) {
        if (nLeft != 1 && !fn_800BCD50()) {
            return 0;
        }
        nBest = 1000;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = fn_800E1904(i, 0);
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
                                   fn_800D2B08();
                } else {
                    nOther = anTotal[i] + gPlayers[i].nStrokes[Game_CurHoleIndex()] - fn_800D2B08();
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
                nMine = gPlayers[i].n274;
                nMineStrokes = gPlayers[i].nStrokes[Game_CurHoleIndex()];
            } else {
                if (gPlayers[i].n274 > nBest) {
                    nBest = gPlayers[i].n274;
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

// For a human player: whether Player.ballBefore passes the fn_800D782C check or, when that ball
// is in the cup, the fn_800D7B1C putt check.
u8 fn_800CF77C(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800D782C(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        fn_800D7B1C(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    return 0;
}

// As fn_800CF77C, with the checks fn_800D7660 and fn_800D7684.
u8 fn_800CF848(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800D7660(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        fn_800D7684(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    return 0;
}

// For a human player, the course records (gSession.aCourseRecord, kind k's best) this shot
// keeps in reach, as bits, each only while goals of its kind count (fn_800D8DB4) and, but for
// kinds 1 and 2, outside a playoff: 0x1 on the last hole, the round would beat kind 0's with the
// tap-in; 0x2 the ball is on the tee; 0x4 on the green or fringe, a putt of 3 x fA64 beats kind
// 2's; 0x8 off the green two under par or better, one more of fn_800D1170 beats kind 3's; 0x10
// on the last hole, the round's putts (one more on the green or fringe) beat kind 4's; 0x20 on
// the tee of a par 4 or 5, one more of fn_800D0FBC beats kind 5's; 0x40 and 0x80 two under / one
// under par or better, one more eagle (fn_800D06FC) / birdie (fn_800D0620) beats kinds 6 / 7.
u32 fn_800CF904(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];
    u32 uFlags = 0;
    int nPutts;
    int i;
    Ball* pBall = &pPlayer->ball;

    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800D8DB4(0) && !gpGame->bD4 && Game_CurHoleIndex() == 17) {
        if (fn_800E17AC(nPlayer) + 1 < gSession.aCourseRecord[Game_GetCourse()].aRecord[0][0].nValue) {
            uFlags |= 0x1;
        }
    }
    if (fn_800D8DB4(1) && pBall->nLie == LIE_TEE_e) {
        uFlags |= 0x2;
    }
    if (fn_800D8DB4(2) && (pBall->nLie == LIE_GREEN_e || pBall->nLie == LIE_FRINGE_e) &&
        3.0f * gPlayers[nPlayer].fA64 > gSession.aCourseRecord[Game_GetCourse()].aRecord[2][0].nValue) {
        uFlags |= 0x4;
    }
    if (fn_800D8DB4(3) && !gpGame->bD4 && pBall->nLie != LIE_GREEN_e && pBall->nLie != LIE_FRINGE_e &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= fn_800D2B08() - 2) {
        if (fn_800D1170(nPlayer, 0) + 1 > gSession.aCourseRecord[Game_GetCourse()].aRecord[3][0].nValue) {
            uFlags |= 0x8;
        }
    }
    if (fn_800D8DB4(4) && !gpGame->bD4 && Game_CurHoleIndex() == 17) {
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
    if (fn_800D8DB4(5) && !gpGame->bD4 && pBall->nLie == LIE_TEE_e && fn_800D2B08() >= 4) {
        if (fn_800D0FBC(nPlayer) + 1 > gSession.aCourseRecord[Game_GetCourse()].aRecord[5][0].nValue) {
            uFlags |= 0x20;
        }
    }
    if (fn_800D8DB4(6) && !gpGame->bD4 &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= fn_800D2B08() - 2) {
        if (fn_800D06FC(nPlayer, 0, 0) + 1 > gSession.aCourseRecord[Game_GetCourse()].aRecord[6][0].nValue) {
            uFlags |= 0x40;
        }
    }
    if (fn_800D8DB4(7) && !gpGame->bD4 &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <= fn_800D2B08() - 1) {
        if (fn_800D0620(nPlayer, 0, 0) + 1 > gSession.aCourseRecord[Game_GetCourse()].aRecord[7][0].nValue) {
            uFlags |= 0x80;
        }
    }
    return uFlags;
}

// Runs the shot, putt and (outside a playoff) hole checks of Earnings.c for the player as a
// preview (bPreview 1) and returns the ids 0..22 they list, as a bit set.
u32 fn_800CFD58(int nPlayer) {
    u32 uIds = 0;
    u32 nId;
    int i;

    fn_800D477C(nPlayer, NULL, 1);
    for (i = 0; i < fn_800D9954(); i++) {
        nId = fn_800D995C(i);
        if (nId <= 22) {
            uIds |= 1 << nId;
        }
    }
    fn_800D4F14(nPlayer, 1);
    for (i = 0; i < fn_800D9954(); i++) {
        nId = fn_800D9970(i);
        if (nId <= 22) {
            uIds |= 1 << nId;
        }
    }
    if (!gpGame->bD4) {
        fn_800D588C(nPlayer, 1, 0);
        for (i = 0; i < fn_800D9954(); i++) {
            nId = fn_800D9984(i);
            if (nId <= 22) {
                uIds |= 1 << nId;
            }
        }
    }
    return uIds;
}

// gpGame->pfn200 (TW06: GetCurrentLead).
// The player's lead in the round so far (strokes, holes won or skins by the scoring kind
// fn_8008AB40): kind 0, the best other total (fn_800E1904; cut players left out) less the
// player's; kinds 1 and 2, the player's holes won (skins) less the best of the others'.
s32 fn_800CFE74(int nPlayer) {
    int anTotal[4];   // one per player set up; the frame has room for four
    int i;
    int nKind;
    int nMine;
    int nBest;

    nKind = fn_8008AB40();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = fn_800E1904(i, 0);
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
                nMine = gPlayers[i].n274;
            } else if (gPlayers[i].n274 > nBest) {
                nBest = gPlayers[i].n274;
            }
        }
        return nMine - nBest;
    }
    return 0;
}

// The player's score against par for the round once the tap-in on this hole drops; 0 when
// fn_8008AB40 is set.
int fn_800CFFE4(int nPlayer) {
    int nPar = 0;
    int nStrokes = 0;
    int i;
    if (fn_8008AB40()) {
        return 0;
    }
    for (i = 0; i < Game_CurHoleIndex(); i++) {
        nPar += fn_800D2AD8(i);
        nStrokes += gPlayers[nPlayer].nStrokes[i];
    }
    return (nStrokes - nPar) + gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}

// gpGame->pfn204 (TW06: GetPotentialLead).
// The player's lead (strokes, or holes or skins by the scoring kind fn_8008AB40) once this hole's
// ball drops, 0 when playing alone. Kind 0 (strokes): the best other round total, with a holed
// ball's score on this hole, less the player's total with the tap-in; players who missed the cut
// are left out. Kinds 1 and 2: the lead from fn_800BCCF8, moved by fn_800BCCCC's value: 3 no
// change, 2 up one (kind 2: up this hole's skin), 0 down the same.
s32 fn_800D0098(int nPlayer) {
    int anTotal[4];   // one per player set up, as in fn_800CFE74
    int i;
    int nKind;
    int nMine;
    int nBest;
    int nOther;
    int nLead;
    int nHole;

    nKind = fn_8008AB40();
    if (gNumPlayersSetUp == 1) {
        return 0;
    }
    if (nKind == 0) {
        nBest = 1000;
        // EA bug: nMine is never set when nPlayer is cut or not one of the players set up
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!gPlayers[i].bPlayerCut) {
                anTotal[i] = fn_800E1904(i, 0);
                if (i == nPlayer) {
                    nMine = anTotal[i];
                    nMine += gPlayers[i].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
                } else {
                    nOther = anTotal[i];
                    if (gPlayers[i].ball.nLie == LIE_INCUP_e) {
                        nOther += gPlayers[i].nStrokes[Game_CurHoleIndex()] - fn_800D2B08();
                    }
                    if (nOther < nBest) {
                        nBest = nOther;
                    }
                }
            }
        }
        return nBest - nMine;
    } else if (nKind == 1) {
        nLead = fn_800BCCF8(nPlayer);
        nHole = fn_800BCCCC(nPlayer);
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
        nLead = fn_800BCCF8(nPlayer);
        nHole = fn_800BCCCC(nPlayer);
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

// Whether holing the ball now would finish the hole: puts the ball in the cup with one more
// stroke, asks the mode (pfnHoleFinished, only asking), then puts both back.
u8 fn_800D024C(int nPlayer) {
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

// gpGame->pfn208.
// How the hole ends for the player if the ball drops now, against the best of the others (a
// ball not yet holed counts one more stroke; in mode 21 the other side is player 2 or 0): 2 the
// player wins it, 1 ties, 0 loses; 3 when playing alone or when holing would not end the hole.
s32 fn_800D030C(int nPlayer) {
    int i;
    int nMine;
    int nBest;
    int nOther;

    fn_8008AB40();
    if (gNumPlayersSetUp == 1) {
        return 3;
    }
    if (!fn_800D024C(nPlayer)) {
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

// The ball's distance from the pin: where it lies, where the shot started, and where it lay before
// the shot.
f32 fn_800D0478(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vPos);
}

f32 fn_800D04AC(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vStart);
}

f32 fn_800D04E0(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ballBefore.vPos);
}

// The class (SurfaceType.nClass) of the surface under the lying ball before the shot, or -1.
int fn_800D0514(int nPlayer) {
    int nSurface = gPlayers[nPlayer].ballBefore.nSurface;
    if (nSurface >= 0) {
        return gSurfaceTypes[nSurface].nClass;
    }
    return -1;
}

// The shot's length along the ground (the height left out).
f32 fn_800D0550(int nPlayer) {
    f32 vDiff[3];
    fn_800D1674(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
    vDiff[1] = 0.0f;
    return fn_80009680(fn_80009744(vDiff));
}

// A point's distance from the current pin along the ground; 0 with no hole loaded.
f32 fn_800D05A4(f32* pPos) {
    f32 vDiff[3];
    CourseInfo* pCourse = fn_8000C594();
    int nPin;
    if (pCourse == NULL) return 0.0f;
    nPin = Game_CurrentPinSet();
    fn_800D1674(pPos, &pCourse->pin[nPin].x, vDiff);
    vDiff[1] = 0.0f;
    return fn_80009680(fn_80009744(vDiff));
}

// The round's holes the player finished under par, counting back from the current hole (with
// bCurrent) or the one before; with bOnlyFlagged, only those whose gpGame->b16C entry is 1.
int fn_800D0620(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= fn_800D2AD8(i) - 1 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// As fn_800D0620, two under par or better.
int fn_800D06FC(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= fn_800D2AD8(i) - 2 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The player's current run of holes under par: counting back from the current hole (with
// bCurrent) or the one before, until a hole that is not.
int fn_800D07D8(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > fn_800D2AD8(i) - 1) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// As fn_800D07D8, two under par or better.
int fn_800D089C(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > fn_800D2AD8(i) - 2) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// The angle (radians) at the start of the shot, along the ground, from the direction of the pin
// to that of the player's view's point (fn_800C8C3C); negative on one side.
f32 fn_800D0960(int nPlayer) {
    f32 vView[4];
    f32 vToPin[4];
    f32 vToView[4];
    f32 fCos;
    f32 fAngle;

    fn_800C8C3C(gPlayers[nPlayer].nView[0], vView);
    fn_800D1674(&gPlayers[nPlayer].ball.pCourse->pin[Game_CurrentPinSet()].x, gPlayers[nPlayer].ball.vStart,
                vToPin);
    vToPin[1] = 0.0f;
    fn_800D1674(vView, gPlayers[nPlayer].ball.vStart, vToView);
    vToView[1] = 0.0f;
    if ((f32)fn_80009680(fn_80009744(vToPin)) > 0.0f) {
        fn_800BAF04(vToPin, vToPin);
    }
    if ((f32)fn_80009680(fn_80009744(vToView)) > 0.0f) {
        fn_800BAF04(vToView, vToView);
    }
    fCos = fn_8000C5FC(vToView, vToPin);
    if (fCos < -1.0f) {
        fCos = -1.0f;
    } else if (fCos > 1.0f) {
        fCos = 1.0f;
    }
    fAngle = fn_80009614(fCos);
    if (vToView[2] * vToPin[0] - vToView[0] * vToPin[2] < 0.0f) {
        fAngle *= -1.0f;
    }
    return fAngle;
}

// The score the hole will finish on once the tap-in drops: strokes so far plus one, minus par.
int Hole_ScoreAfterTapIn(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}

// Whether nobody took anything (mode points or n22C) on the last hole played before this one; 0
// on the round's first hole.
u8 fn_800D0AF4(void) {
    int i;
    int j;
    if (fn_800E1734()) {
        return 0;
    }
    for (i = Game_CurHoleIndex() - 1; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            for (j = 0; j < gNumPlayersSetUp; j++) {
                if (gPlayers[j].nModePoints[i] != 0 || gPlayers[j].n22C[i] != 0) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

// The class of the surface the shot started from, 0 for none.
u32 fn_800D0BAC(int nPlayer) {
    if (gPlayers[nPlayer].ball.nStartSurface < 0 ||
        gPlayers[nPlayer].ball.nStartSurface >= NUM_SURFACE_TYPES) {
        return 0;
    }
    return gSurfaceTypes[gPlayers[nPlayer].ball.nStartSurface].nClass;
}

// The ground under Player.vBall is not green (class 3) and the hole's strokes so far are three
// under par or better (bUnder: more than three); without bAnyLie, the ball must also be on the
// green or in the cup and two under par (bUnder: more).
u8 fn_800D0BF8(int nPlayer, u8 bUnder, u8 bAnyLie) {
    SurfaceType* pSurface;
    int nPar;
    int nStrokes;
    int nLie;

    pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, gPlayers[nPlayer].vBall);
    if (pSurface != NULL) {
        nPar = fn_800D2B08();
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

// The shot started from class 1 ground (fairway) and the ball now lies 1, 9 or 12 (12: in the cup).
u8 fn_800D0D54(int nPlayer) {
    if (fn_800D0BAC(nPlayer) != 1) return 0;
    if (gPlayers[nPlayer].ball.nLie == 1 || gPlayers[nPlayer].ball.nLie == 9 ||
        gPlayers[nPlayer].ball.nLie == 12) {
        return 1;
    }
    return 0;
}

// The round's holes the player finished nToPar or better; below -3 a hole in one always counts.
int fn_800D0DC8(int nPlayer, int nToPar) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) <= nToPar ||
                (gPlayers[nPlayer].nStrokes[i] == 1 && nToPar < -3)) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The round's holes the player finished over par.
int fn_800D0E74(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) >= 1) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The longest run of the round's holes finished nToPar or better (as fn_800D0DC8 counts them).
int fn_800D0F04(int nPlayer, int nToPar) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) <= nToPar ||
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

// The round's holes with b2E4 set (by position in the score block, TW06's fairways[]).
int fn_800D0FBC(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2E4[i]) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with b2E4 set; a par 3 does not break it.
int fn_800D10B0(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2E4[i]) {
            nRun++;
        } else if (fn_800D2AD8(i) != 3) {
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

// The round's holes with b2F6 set (TW06's gir[]); with bOnlyFlagged, only those whose
// gpGame->b16C entry is 1.
int fn_800D1170(int nPlayer, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2F6[i] &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with b2F6 set.
int fn_800D1250(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2F6[i]) {
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
int fn_800D1330(int nPlayer) {
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
int fn_800D13F4(int nPlayer) {
    f32 vWind[4];
    f32 fAim = gPlayers[nPlayer].fAim;
    f32 fAngle;

    if (Wind_Get(vWind) > 6.0f) {
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
int fn_800D1530(int nPlayer) {
    f32 vNormal[4];
    f32 vTurned[4];
    f32 fLength;
    f32 fAim;
    f32 fSin;
    f32 fCos;
    f32 fDegrees;
    f32 fEpsilon;

    if (!Ter_GetSupportingGroundNormal(fn_8000C594(), gPlayers[nPlayer].ball.vPos, vNormal)) {
        return 0;
    }
    fLength = fn_80009744(vNormal);
    if (fLength > 1.01f || fLength < 0.99f) {
        return 0;
    }
    fAim = gPlayers[nPlayer].fAim;
    fSin = fn_800095F0(fAim);
    fCos = fn_80009638(fAim);
    Vec3Copy(vNormal, vTurned);
    fn_80055D70(&vTurned[2], &vTurned[0], fSin, fCos);
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
asm void fn_800D1674(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800D1674(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
