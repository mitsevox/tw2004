// Golfer.c: golfer records, attribute access, and the CPU golfer's shot choice. No assert
// names this file; "Golfer.c" is our name. CodeWarrior GC/2.5, -O4,p. What the code does is
// written up in docs/gameplay.md; the record layout in docs/formats/game-data.md.

#include "golfer.h"

// ---- small accessors ------------------------------------------------------------------------

u8 Player_IsCPU(int nPlayer) {
    return gPlayers[nPlayer].nController == CONTROLLER_CPU;
}

u8 Controller_IsCPU(int nController) {
    return nController == CONTROLLER_CPU;
}

int Game_CurrentHole(void) {
    return gpGame->holeOrder[gpGame->nCurHole];
}

// Equipment tier -> bonus points. Tiers are 0..4; anything else counts as nothing.
int Golfer_TierBonus(int nTier) {
    int nBonus = 0;
    switch (nTier) {
    case 1: nBonus = 1; break;
    case 2: nBonus = 2; break;
    case 3: nBonus = 3; break;
    case 4: nBonus = 4; break;
    }
    return nBonus;
}

// Which attribute governs a shot: the putter, then bad lies, then a full swing with a long
// club from a good lie, then everything else is an approach.
int Shot_GoverningAttribute(int nPlayer, int nClub, int nLie, int nKind) {
    if (nClub == CLUB_PUTTER) {
        return ATTR_PUTTING;
    }
    if (nLie == 8 || nLie == 13 || nLie == 11 || nLie == 3 || nLie == 4 || nLie == 6 || nLie == 7) {
        return ATTR_RECOVERY;
    }
    if (nKind == SHOT_FULL && nClub < 13 && fn_800D2B08() == 3) {
        return ATTR_BALL_STRIKING;
    }
    return ATTR_APPROACH;
}

// The one place attributes are read. Base = the record value (block B for a CPU pro in game
// mode 4) plus the equipment tier bonus; modifiers are the per-player adjustments.
int Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode) {
    int nValue = 0;
    if (nMode == ATTR_BASE || nMode == ATTR_TOTAL) {
        if (Controller_IsCPU(pPlayer->nController) && pPlayer->golfer.nIndex < FIRST_CREATED_GOLFER &&
            (Game_GetMode() == 4 || (gSession.uFlags & 2))) {
            nValue = (s8)((s32)pPlayer->golfer.attrAlt[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        } else {
            nValue = (s8)((s32)pPlayer->golfer.attr[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        }
    }
    if (nMode == ATTR_MODIFIERS || nMode == ATTR_TOTAL) {
        nValue += pPlayer->attrMod[nAttr];
    }
    return nValue;
}

// Keep base + modifiers inside [10, cap]: 110 for power, IQ and aggression, 100 for the rest.
#define CLAMP_ATTR(pPlayer, nAttr, nCap)                                                         if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) > (nCap)) {                              (pPlayer)->attrMod[nAttr] = (nCap) - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);     } else if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) < 10) {                           (pPlayer)->attrMod[nAttr] = 10 - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);         }

void Golfer_ClampModifiers(Player* pPlayer) {
    CLAMP_ATTR(pPlayer, ATTR_POWER, 110);
    CLAMP_ATTR(pPlayer, ATTR_IQ, 110);
    CLAMP_ATTR(pPlayer, ATTR_AGGRESSION, 110);
    CLAMP_ATTR(pPlayer, ATTR_BALL_STRIKING, 100);
    CLAMP_ATTR(pPlayer, ATTR_APPROACH, 100);
    CLAMP_ATTR(pPlayer, ATTR_PUTTING, 100);
    CLAMP_ATTR(pPlayer, ATTR_RECOVERY, 100);
}

// ---- the CPU golfer ---------------------------------------------------------------------------

f32 AI_Pow(f32 fBase, f32 fExp) {
    return fn_8015F824(fBase, fExp);
}

// Pick where the CPU aims: the most demanding authored aim point it qualifies for. A human
// only ever gets a priority point from here (the default aim on walking up to the ball).
void AI_ChooseTarget(int nPlayer) {
    Player*     p;
    AITarget*   t;
    int         nKind;
    int         nPower;
    int         nAggr;
    int         nHole;
    s8          k;
    s8          nBest;
    s8          nCand;
    int         nSkill;
    int         nIQ;
    f32         fDumb;         // (100 - IQ): the overconfidence term
    f32         fDist2;
    f32         fBestDist2;
    f32         fDist;
    CourseInfo* pCourse;
    s8          nZone;
    int         nClub;
    f32         fDX, fDZ;
    AITarget*   pBest;

    nHole   = Game_CurrentHole();
    pCourse = fn_8000C594();
    p       = &gPlayers[nPlayer];
    nAggr   = Golfer_GetAttribute(p, ATTR_AGGRESSION, ATTR_TOTAL);
    nIQ     = Golfer_GetAttribute(p, ATTR_IQ, ATTR_TOTAL);
    nPower  = Golfer_GetAttribute(p, ATTR_POWER, ATTR_TOTAL);
    nZone   = AI_NearestTarget(&p->fBallX, NULL);
    if (nZone == -1) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    nBest      = -1;
    fBestDist2 = 100000000.0f;
    if (gAITargets[nZone].pDef != NULL) {
        fDumb = 100.0f - (f32)(s8)nIQ;
        for (k = 0; k < NUM_AI_LINKS; k++) {
            nCand = (s8)gAITargets[nZone].pDef->nLinks[k];
            if (nCand == -1) continue;
            t = &gAITargets[nCand];
            if (!t->bEnabled) continue;
            fDZ    = pCourse->pin[nHole].z - t->pDef->z;
            fDX    = pCourse->pin[nHole].x - t->pDef->x;
            fDist2 = fDX * fDX + fDZ * fDZ;
            if (t->nTeeSet != -1 && t->nTeeSet != gSession.nTeeSet[nPlayer]) continue;
            if (t->nHole != -1 && t->nHole != nHole) continue;
            if (nBest == -1 && t->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (!Player_IsCPU(nPlayer)) continue;

            fDZ    = p->fBallZ - t->pDef->z;
            fDX    = p->fBallX - t->pDef->x;
            fDist  = fn_80009680(fDX * fDX + fDZ * fDZ);
            nKind  = AI_ShotKindForDistance(nPlayer, fDist);
            nClub  = AI_ClubForShot(nPlayer, nKind, 0, fDist);
            nSkill = Golfer_GetAttribute(p, Shot_GoverningAttribute(nPlayer, nClub, p->nLie, nKind), ATTR_TOTAL);
            if (Player_IsCPU(nPlayer)) {
                // Low IQ makes the golfer think it is better than it is.
                if (p->bLowIQPenalty) {
                    nSkill += (int)(10.0f * (AI_Pow(fDumb, 2.0f) / 100.0f) / 100.0f);
                } else {
                    nSkill += (int)(40.0f * (AI_Pow(fDumb, 2.0f) / 100.0f) / 100.0f);
                }
                if ((s8)nSkill > 100) nSkill = 100;
            }
            if (t->nSkillReq < 0 && (s8)nSkill > IABS((int)t->nSkillReq)) continue;
            if ((s8)nSkill < t->nSkillReq) continue;
            if (t->nAggrReq < 0 && (s8)nAggr > IABS((int)t->nAggrReq)) continue;
            if ((s8)nAggr < t->nAggrReq) continue;
            if (t->nPowerReq < 0 && (s8)nPower > IABS((int)t->nPowerReq)) continue;
            if ((s8)nPower < t->nPowerReq) continue;
            if (fDist > AI_MaxDistance(nPlayer, nKind, AI_FirstUsableClub(nPlayer, nKind))) continue;

            if (nBest == -1) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            pBest = &gAITargets[nBest];
            if (pBest->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            // Prefer the more demanding point: power, then aggression, then skill, then closer.
            if (t->nPowerReq > pBest->nPowerReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nPowerReq != IABS((int)t->nPowerReq)) continue;
            if (t->nAggrReq > pBest->nAggrReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nAggrReq != IABS((int)t->nAggrReq)) continue;
            if (t->nSkillReq > pBest->nSkillReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (pBest->nSkillReq != IABS((int)t->nSkillReq)) continue;
            if (fDist2 < fBestDist2) {
                nBest      = nCand;
                fBestDist2 = fDist2;
            }
        }
    }
    if (nBest == -1) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    // Already closer to the pin than the chosen point: aim normally instead.
    fDZ = p->fBallZ - pCourse->pin[nHole].z;
    fDX = p->fBallX - pCourse->pin[nHole].x;
    if (fDX * fDX + fDZ * fDZ < fBestDist2 && !(gpGame->unk14 == 3 && fn_80015464() == 17)) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    pBest          = &gAITargets[nBest];
    p->fTargetX    = pBest->pDef->x;
    p->fTargetZ    = pBest->pDef->z;
    p->nTargetType = pBest->nType;
    AI_PlanShot(nPlayer, &p->fTargetX);
    Vec_Copy(&p->fTargetX, p->vTarget2);
}

// Make the CPU miss: move its aim and distance by up to a shot-type limit scaled by
// (100 - skill), then re-plan for the moved target. Putts under 1.5 are never missed.
void AI_ApplyError(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    f32 fCos;
    f32 fMiss;
    f32 fAimErr;
    f32 fDistErr;
    f32 fSkill;
    f32 fMaxAngle;
    f32 fDist1;
    f32 fDist2;
    f32 fSin;
    f32 fRand;
    int nSpin;
    int nAttr;

    p->swing.fSpinX = 0.0f;
    p->swing.fSpinY = 0.0f;
    if (p->fDistance < 1.0f) return;
    if (p->bPerfect) return;

    nAttr = Shot_GoverningAttribute(nPlayer, p->nClub, p->nLie, p->nShotKind);
    if (nAttr == ATTR_PUTTING) {
        fSkill    = (f32)(s8)Golfer_GetAttribute(p, ATTR_PUTTING, ATTR_TOTAL);
        nSpin     = 0;
        fMaxAngle = DEG(8.0f);
        fDist1    = 20.0f;
        fDist2    = 10.0f;
        if (p->fDistance < 1.5f) return;
        if (p->fDistance < 5.0f) fMaxAngle *= 0.5f;
    } else if (nAttr == ATTR_RECOVERY) {
        fSkill = (f32)(s8)Golfer_GetAttribute(p, ATTR_RECOVERY, ATTR_TOTAL);
        nSpin  = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        if (p->fDistance > 200.0f) {
            fMaxAngle = DEG(2.5f); fDist1 = 6.25f; fDist2 = 2.5f;
        } else if (p->fDistance > 100.0f) {
            fMaxAngle = DEG(5.0f);  fDist1 = 12.5f; fDist2 = 5.0f;
        } else {
            fMaxAngle = DEG(10.0f); fDist1 = 25.0f; fDist2 = 10.0f;
        }
    } else if (nAttr == ATTR_BALL_STRIKING) {
        fSkill    = (f32)(s8)Golfer_GetAttribute(p, ATTR_BALL_STRIKING, ATTR_TOTAL);
        nSpin     = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        fMaxAngle = DEG(2.75f);
        fDist1    = 4.5f;
        fDist2    = 1.0f;
        if (p->fDistance < 150.0f) {
            fMaxAngle *= 2.0f;
            fDist2    *= 2.0f;
        }
    } else {
        fSkill = (f32)(s8)Golfer_GetAttribute(p, ATTR_APPROACH, ATTR_TOTAL);
        nSpin  = Golfer_GetAttribute(p, ATTR_SPIN, ATTR_TOTAL);
        if (p->fDistance > 100.0f) {
            fMaxAngle = DEG(3.75f); fDist1 = 7.5f; fDist2 = 5.25f;
        } else if (p->fDistance > 50.0f) {
            fMaxAngle = DEG(4.25f); fDist1 = 8.5f; fDist2 = 5.95f;
        } else {
            fMaxAngle = DEG(5.0f);  fDist1 = 10.0f; fDist2 = 7.0f;
        }
        // Even a good golfer occasionally blows an approach.
        if (fSkill > 80.0f) {
            if (Rand_Next(0) % 20 == 0) {
                fMaxAngle *= 4.0f; fDist1 *= 4.0f; fDist2 *= 4.0f; fSkill *= 0.25f;
            } else if (Rand_Next(0) % 10 == 0) {
                fMaxAngle *= 2.0f; fDist1 *= 2.0f; fDist2 *= 2.0f; fSkill *= 0.5f;
            }
        } else if (fSkill > 60.0f) {
            if (Rand_Next(0) % 20 == 0) {
                fMaxAngle *= 2.0f; fDist1 *= 2.0f; fDist2 *= 2.0f; fSkill *= 0.5f;
            }
        }
    }

    if (fSkill > 98.0f) fSkill = 98.0f;
    if (fSkill < 100.0f) {
        f32 fDX, fDZ;
        if (fSkill < 15.0f) fSkill = 15.0f;

        // Aim: up to the angle limit, at least a quarter of a degree, either side.
        fRand   = Rand_Float(0);
        fMiss   = 100.0f - fSkill;
        fAimErr = fMaxAngle * (fMiss * fRand) / 100.0f;
        if (fAimErr < DEG(0.25f)) fAimErr += DEG(0.25f);
        if (Rand_Next(0) & 1) fAimErr *= -1.0f;
        p->fAim += fAimErr;
        if (p->fAim < -PI) {
            p->fAim += TWOPI;
        } else if (p->fAim > PI) {
            p->fAim -= TWOPI;
        }
        fSin = fn_800095F0(p->fAim);
        fCos = fn_80009638(p->fAim);

        // Distance error (in fSkill from here): two percentage terms, either side; one shot
        // kind always comes up short.
        fRand   = Rand_Float(0);
        fDistErr  = fDist1 * (fMiss * fRand) / 100.0f;
        fRand     = Rand_Float(0);
        fDistErr += fDist2 * (fMiss * fRand) / 100.0f;
        if (p->nShotKind != SHOT_FULL || fn_800D2B08() == 3) {
            if (Rand_Next(0) & 1) fDistErr *= -1.0f;
        } else {
            fDistErr *= -1.0f;
        }
        p->fDistance = p->fDistance * ((100.0f + fDistErr) / 100.0f);
        fDX = p->fDistance * -fSin;
        fDZ = p->fDistance * fCos;
        p->fTargetX = p->fBallX + fDX;
        p->fTargetZ = p->fBallZ + fDZ;
        AI_PlanShot(nPlayer, &p->fTargetX);
        p->fPower = AI_PowerForTarget(nPlayer);

        // Spin in proportion to the error, scaled by the SPIN attribute. (Both clamps store +1.)
        if ((s8)nSpin != 0 && fn_80101DF4()) {
            f32 fScale = fn_8005C418(nSpin);
            p->swing.fSpinX  = fDistErr / fDist1;
            p->swing.fSpinX *= fScale;
            p->swing.fSpinY  = fAimErr / fMaxAngle;
            p->swing.fSpinY *= fScale;
            p->swing.fSpinY *= -1.0f;
            if (p->swing.fSpinX > 1.0f) {
                p->swing.fSpinX = 1.0f;
            } else if (p->swing.fSpinX < -1.0f) {
                p->swing.fSpinX = 1.0f;
            }
            if (p->swing.fSpinY > 1.0f) {
                p->swing.fSpinY = 1.0f;
            } else if (p->swing.fSpinY < -1.0f) {
                p->swing.fSpinY = 1.0f;
            }
        }
    }
}

// ---- clubs and distances ----------------------------------------------------------------------

// Can this club be used for this kind of shot? A per-kind table of allowed clubs, then the
// bag: a CPU golfer in sand never takes a wood, and any golfer only clubs it carries.
u8 Club_UsableForKind(int nPlayer, int nClub, int nKind) {
    Player* p = &gPlayers[nPlayer];
    if (Game_GetMode() == 13 || Game_GetMode() == 14 || Game_GetMode() == 16 || Game_GetMode() == 17) {
        return gClubKindTable[nKind][nClub];
    }
    if (Controller_IsCPU(p->nController)) {
        if ((p->nLie == 6 || p->nLie == 7 || p->nLie == 8) && nClub < 9) {
            return 0;
        }
    }
    if (gPlayers[nPlayer].golfer.uBagMask != 0) {
        u32 uBag = gPlayers[nPlayer].golfer.uBagMask;
        return gClubKindTable[nKind][nClub] && ((1 << nClub) & uBag);
    }
    return gClubKindTable[nKind][nClub];
}

// The lowest-numbered club usable for this kind of shot; failing that the first club in the
// bag; failing that the putter.
int AI_FirstUsableClub(int nPlayer, int nKind) {
    int nClub = 0;
    do {
        if (Club_UsableForKind(nPlayer, nClub, nKind)) break;
        nClub++;
    } while (nClub < NUM_CLUBS);
    if (nClub == NUM_CLUBS) {
        u32* pBag = &gPlayers[nPlayer].golfer.uBagMask;
        int  i;
        for (i = 0; i < NUM_CLUBS; i++) {
            if ((1 << i) & *pBag) return i;
        }
        nClub = CLUB_PUTTER;
    }
    return nClub;
}

// How far this golfer can hit this club for this kind of shot. Chips are 30, putts 60; a full
// swing interpolates the club's table distance by POWER (below 100 towards the power-0 table,
// above 100 a fixed step per point).
f32 AI_MaxDistance(int nPlayer, int nKind, int nClub) {
    f32 fMax;
    if (nKind == SHOT_CHIP) {
        fMax = 30.0f;
    } else if (nKind == SHOT_PUTT) {
        fMax = 60.0f;
    } else {
        Player* p = &gPlayers[nPlayer];
        fMax = fn_80050F44(nKind, nClub);
        if (nKind == SHOT_FULL || nKind == 7 || nKind == 4 || nKind == 6) {
            f32 fPower = (f32)(s8)Golfer_GetAttribute(p, ATTR_POWER, ATTR_TOTAL);
            if (fPower < 100.0f) {
                f32 fRange = fMax - gClubPowerStep[nClub] - gClubDistAtPower0[nClub];
                fMax = gClubDistAtPower0[nClub] + fRange * fPower / 100.0f;
            } else if (fPower > 100.0f) {
                fMax = (fPower - 100.0f) * gClubPowerStep[nClub] + fMax;
            }
        }
    }
    return fMax;
}

// The kind of shot the CPU plays from here for a given distance: putt on the green or
// very close, chip or pitch when a wedge in the bag reaches, otherwise a full swing.
int AI_ShotKindForDistance(int nPlayer, f32 fDist) {
    int nOverride = fn_80100744();
    switch (nOverride) {
    case 8: {
        int     nKind = SHOT_FULL;
        Player* p     = &gPlayers[nPlayer];
        fDist /= fn_800510EC(p->unkA90);
        if (p->nLie == LIE_GREEN || AI_WithinOfPin(nPlayer, 1.5f)) {
            nKind = SHOT_PUTT;
        } else if ((p->golfer.uBagMask & (1 << 21)) && fDist < 15.0f && AI_WithinOfPin(nPlayer, 5.0f) && fn_8002CEDC(nPlayer)) {
            nKind = SHOT_CHIP;
        } else if ((p->golfer.uBagMask & (1 << 23)) && fDist < 20.0f) {
            nKind = SHOT_PITCH;
        } else if ((p->golfer.uBagMask & (1 << 21)) && fDist < 35.0f) {
            nKind = SHOT_PITCH;
        } else if ((p->golfer.uBagMask & (1 << 19)) && fDist < 50.0f) {
            nKind = SHOT_PITCH;
        } else if ((p->golfer.uBagMask & (1 << 18)) && fDist < 65.0f) {
            nKind = SHOT_PITCH;
        }
        return nKind;
    }
    }
    return nOverride;
}

// The club for a shot of this kind and distance: the one whose reach is nearest the distance
// (only clubs that reach when bUnderOnly), the putter for putts, the sand wedge for chips.
int AI_ClubForShot(int nPlayer, int nKind, u8 bUnderOnly, f32 fDist) {
    Player* p;
    int     nClub;
    int     c;
    f32     fBest;
    f32     fRatio;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000) && gPlayers[nPlayer].nLie == 0) {
        return 2;
    }
    nClub = fn_801006F0(nPlayer);
    if (nClub != NUM_CLUBS) {
        return nClub;
    }
    fBest = bUnderOnly ? 0.0f : 10000.0f;
    p     = &gPlayers[nPlayer];
    if (nKind == SHOT_PUTT) {
        nClub = CLUB_PUTTER;
    } else if (nKind == SHOT_CHIP) {
        nClub = CLUB_SAND_WEDGE;
        if (!Club_UsableForKind(nPlayer, CLUB_SAND_WEDGE, nKind)) {
            nClub = AI_FirstUsableClub(nPlayer, nKind);
        }
    } else {
        nClub = AI_FirstUsableClub(nPlayer, nKind);
        if (Game_GetMode() == 6 || Game_GetMode() == 7 || Game_GetMode() == 8 || Controller_IsCPU(p->nController)) {
            fDist /= fn_800510EC(p->unkA90);
        }
        for (c = 0; c < NUM_CLUBS; c++) {
            if (c == CLUB_PUTTER) continue;
            if (!Club_UsableForKind(nPlayer, c, nKind)) continue;
            if (bUnderOnly) {
                fRatio = fDist / AI_MaxDistance(nPlayer, nKind, c);
                if (fRatio > 1.0f) continue;
                if (fRatio == 1.0f) {
                    nClub = c;
                    break;
                }
                if (fRatio > fBest) {
                    fBest = fRatio;
                    nClub = c;
                }
            } else {
                fRatio = AI_MaxDistance(nPlayer, nKind, c) / fDist;
                if (fRatio > 1.0f) {
                    if (fRatio - 1.0f < fBest) {
                        fBest = fRatio - 1.0f;
                        nClub = c;
                    }
                } else if (fRatio == 1.0f) {
                    nClub = c;
                    break;
                } else if (1.0f - fRatio < fBest) {
                    fBest = 1.0f - fRatio;
                    nClub = c;
                }
            }
        }
    }
    return nClub;
}

// Power (0..1) to reach the current target: putts and chips have their own curves, everything
// else is distance over the club's reach.
f32 AI_PowerForTarget(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (p->nShotKind == SHOT_PUTT) {
        return fn_80050D34(p->fDistance);
    }
    if (p->nShotKind == SHOT_CHIP) {
        return fn_80050F88(p->fDistance, p->unkA90, SHOT_CHIP, p->nClub);
    }
    return p->fDistance / AI_MaxDistance(nPlayer, p->nShotKind, p->nClub);
}

// Reach for this golfer over the club's table reach; 1 for the shot kinds that do not scale.
f32 AI_PowerScale(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    f32 fTable;
    if (p->nShotKind == SHOT_FULL || p->nShotKind == 7 || p->nShotKind == 4 || p->nShotKind == 6) {
        fTable = fn_80050F44(p->nShotKind, p->nClub);
        return AI_MaxDistance(nPlayer, p->nShotKind, p->nClub) / fTable;
    }
    return 1.0f;
}

// ---- targets ----------------------------------------------------------------------------------

// The authored aim point nearest a position (its index, -1 if none); optionally its x/z.
s8 AI_NearestTarget(f32* pPos, f32* pOut) {
    f32 fBest = 100000000.0f;
    s8  i;
    s8  nBest = -1;
    for (i = 0; i < gNumAITargets; i++) {
        if (gAITargets[i].pDef != NULL) {
            f32 fDZ = pPos[2] - gAITargets[i].pDef->z;
            f32 fDX = pPos[0] - gAITargets[i].pDef->x;
            f32 fD2 = fDX * fDX + fDZ * fDZ;
            if (fD2 < fBest) {
                fBest = fD2;
                nBest = i;
            }
        }
    }
    if (pOut != NULL && nBest != -1) {
        pOut[0] = gAITargets[nBest].pDef->x;
        pOut[2] = gAITargets[nBest].pDef->z;
    }
    return nBest;
}

// Fill in everything that follows from a target: the landing surface, the target height, the
// distance, and (for a human) a snap to the club's reach when it is just under.
void AI_PlanShot(int nPlayer, f32* pTarget) {
    f32*         pDst;
    SurfaceType* pSurface = NULL;
    f32          fHeight;
    f32          fDX, fDZ;
    int          nType;

    pDst = &gPlayers[nPlayer].fTargetX;
    Vec_Copy(pTarget, pDst);
    fHeight = Terrain_HeightAt(pDst, &pSurface);
    gPlayers[nPlayer].uFlagsEF0 &= ~2;
    if (pSurface != NULL) {
        nType = pSurface - gSurfaceTypes;
        if (nType == 98 || nType == 105) {
            // Water: aim at the pin's height instead and flag it.
            fHeight = fn_8000C594()->pin[Game_CurrentHole()].y;
            gPlayers[nPlayer].nSurface = 16;
            gPlayers[nPlayer].uFlagsEF0 |= 2;
        } else {
            gPlayers[nPlayer].nSurface = nType;
        }
    } else {
        gPlayers[nPlayer].nSurface = -1;
    }
    if (fHeight != -65536.1f) {
        gPlayers[nPlayer].fTargetY = fHeight + 0.001f;
    }
    fDX = gPlayers[nPlayer].fTargetX - gPlayers[nPlayer].fBallX;
    fDZ = gPlayers[nPlayer].fTargetZ - gPlayers[nPlayer].fBallZ;
    if (0.0f == fDX && 0.0f == fDZ) {
        gPlayers[nPlayer].fDistance = 10.0f;
    } else {
        gPlayers[nPlayer].fDistance = fn_80009680(fDX * fDX + fDZ * fDZ);
    }
    if (!Player_IsCPU(nPlayer)) {
        // A human whose distance is a hair under the club's reach gets the full reach.
        f32 fMax = AI_MaxDistance(nPlayer, gPlayers[nPlayer].nShotKind, gPlayers[nPlayer].nClub);
        if (gPlayers[nPlayer].fDistance / fMax > 0.999f && gPlayers[nPlayer].fDistance / fMax < 1.0f) {
            gPlayers[nPlayer].fDistance = fMax;
        }
    }
    gPlayers[nPlayer].fDistance2 = gPlayers[nPlayer].fDistance;
    Vec_Copy(pDst, gPlayers[nPlayer].vTargetCopy);
}

// Aim at the pin.
void AI_DefaultTarget(int nPlayer) {
    int         nHole   = Game_CurrentHole();
    CourseInfo* pCourse = fn_8000C594();
    Player*     p       = &gPlayers[nPlayer];
    f32*        pTarget;
    p->fTargetX    = pCourse->pin[nHole].x;
    pTarget        = &p->fTargetX;
    p->fTargetZ    = pCourse->pin[nHole].z;
    p->nTargetType = 0;
    AI_PlanShot(nPlayer, pTarget);
    Vec_Copy(pTarget, p->vTarget2);
}

int  fn_8002BBA4(void);                 // gpGame->nCurHole
void fn_80005628(void* pDst, void* pSrc, int nBytes);   // memcpy
extern s8 gLuckOdds[8];                 // 0x802810B0  "1 in n" per player: 12 12 12 12
u8   fn_80101D4C(int nPlayer);          // a CPU in game mode 11 is always lucky
u8   fn_800DA234(void);                 // the current hole is the flagged one
f32  fn_800D0478(int nPlayer);          // a distance, compared with 250

// ---- luck -------------------------------------------------------------------------------------

// Does this shot get a lucky bounce? Humans only. One chance in the player's odds (12), the odds
// cut by LUCK/2 percent, and halved again when 5+ holes down in game mode 4 or on the flagged
// hole. Only off the green, never a putt, and only for pitches, lies 1/2 under 250, or a
// special shot mode.
u8 Golfer_IsLucky(int nPlayer) {
    u8      bLucky = 0;
    u32     uOdds;
    u32     uRoll;
    if (fn_80101D4C(nPlayer)) {
        return 1;
    }
    if (Player_IsCPU(nPlayer) || gSession.bNoLuck) {
        return 0;
    }
    uOdds = gLuckOdds[nPlayer];
    if (Game_GetMode() == 4 && gPlayers[1].nHolesWon - gPlayers[0].nHolesWon > 4) {
        uOdds >>= 1;
    } else if (fn_800DA234()) {
        uOdds >>= 1;
    }
    if (nPlayer >= 0 && nPlayer <= 3) {
        int nLuck = (s8)Golfer_GetAttribute(&gPlayers[nPlayer], ATTR_LUCK, ATTR_TOTAL);
        nLuck = nLuck < 0 ? 0 : (nLuck > 110 ? 110 : nLuck);
        uOdds -= uOdds * (nLuck >> 1) / 100;
        if (uOdds < 1) uOdds = 1;
    }
    uRoll = Rand_Next(0) % uOdds;
    if (gPlayers[nPlayer].nLie != LIE_GREEN && gPlayers[nPlayer].nShotKind != SHOT_PUTT && !gSession.bNoLuck) {
        Game_CurrentHole();
        fn_8000C594();
        if (fn_800D2B08() == 3) {
            bLucky = 1;
        } else if (gPlayers[nPlayer].nShotKind == SHOT_PITCH) {
            bLucky = 1;
        } else if ((gPlayers[nPlayer].nLie == 1 || gPlayers[nPlayer].nLie == 2) && fn_800D0478(nPlayer) < 250.0f) {
            bLucky = 1;
        }
    }
    if (bLucky && uRoll != 0) {
        bLucky = 0;
    }
    return bLucky;
}

// ---- per-shot modifiers (CPU only) ------------------------------------------------------------

// Set a CPU golfer's attribute modifiers for the shot it is about to play. A per-player level
// overrides everything; otherwise the worse its hole is going the better it gets (+10 at one
// over, +20 per stroke beyond); in game mode 4 it loses 5 per hole it leads the match by;
// otherwise every modifier is a random -5..+4. Aggression always moves the other way.
void AI_SetShotModifiers(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    int     nPar, nHole, nStrokes;
    s8      nLevel;
    if (Game_GetMode() == 11) {
        p->attrMod[ATTR_POWER]         = 0;
        p->attrMod[ATTR_IQ]            = 0;
        p->attrMod[ATTR_AGGRESSION]    = 0;
        p->attrMod[ATTR_BALL_STRIKING] = 0;
        p->attrMod[ATTR_APPROACH]      = 0;
        p->attrMod[ATTR_PUTTING]       = 0;
        p->attrMod[ATTR_RECOVERY]      = 0;
        p->attrMod[ATTR_LUCK]          = 0;
        return;
    }
    nPar     = fn_800D2B08();
    nHole    = fn_8002BBA4();
    nLevel   = p->nLevel;
    nStrokes = p->nStrokes[nHole];
    if (nLevel != 0) {
        p->attrMod[ATTR_POWER]         = nLevel * 25;
        p->attrMod[ATTR_IQ]            = p->nLevel * 25;
        p->attrMod[ATTR_AGGRESSION]    = p->nLevel * -25;
        p->attrMod[ATTR_BALL_STRIKING] = p->nLevel * 25;
        p->attrMod[ATTR_APPROACH]      = p->nLevel * 25;
        p->attrMod[ATTR_PUTTING]       = p->nLevel * 25;
        p->attrMod[ATTR_RECOVERY]      = p->nLevel * 25;
        p->attrMod[ATTR_LUCK]          = p->nLevel * 25;
    } else if (nStrokes >= nPar + 2) {
        int n = nStrokes - (nPar + 1);
        p->attrMod[ATTR_POWER]         = n * 20;
        p->attrMod[ATTR_IQ]            = n * 20;
        p->attrMod[ATTR_AGGRESSION]    = n * -20;
        p->attrMod[ATTR_BALL_STRIKING] = n * 20;
        p->attrMod[ATTR_APPROACH]      = n * 20;
        p->attrMod[ATTR_PUTTING]       = n * 20;
        p->attrMod[ATTR_RECOVERY]      = n * 20;
        p->attrMod[ATTR_LUCK]          = n * 20;
    } else if (nStrokes >= nPar + 1) {
        p->attrMod[ATTR_POWER]         = 10;
        p->attrMod[ATTR_IQ]            = 10;
        p->attrMod[ATTR_AGGRESSION]    = -10;
        p->attrMod[ATTR_BALL_STRIKING] = 10;
        p->attrMod[ATTR_APPROACH]      = 10;
        p->attrMod[ATTR_PUTTING]       = 10;
        p->attrMod[ATTR_RECOVERY]      = 10;
        p->attrMod[ATTR_LUCK]          = 10;
    } else if (Game_GetMode() == 4) {
        int nLead = gPlayers[1].nHolesWon - gPlayers[0].nHolesWon;
        p->attrMod[ATTR_POWER] = Rand_Next(0) % 10 - 5;
        if (nLead > 0) {
            p->attrMod[ATTR_IQ]            = nLead * -5;
            p->attrMod[ATTR_AGGRESSION]    = nLead * -5;
            p->attrMod[ATTR_BALL_STRIKING] = nLead * -5;
            p->attrMod[ATTR_APPROACH]      = nLead * -5;
            p->attrMod[ATTR_PUTTING]       = nLead * -5;
            p->attrMod[ATTR_RECOVERY]      = nLead * -5;
            p->attrMod[ATTR_LUCK]          = nLead * -5;
        } else {
            p->attrMod[ATTR_IQ]            = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_AGGRESSION]    = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_BALL_STRIKING] = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_APPROACH]      = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_PUTTING]       = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_RECOVERY]      = Rand_Next(0) % 10 - 5;
            p->attrMod[ATTR_LUCK]          = Rand_Next(0) % 10 - 5;
        }
    } else {
        p->attrMod[ATTR_POWER]         = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_IQ]            = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_AGGRESSION]    = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_BALL_STRIKING] = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_APPROACH]      = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_PUTTING]       = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_RECOVERY]      = Rand_Next(0) % 10 - 5;
        p->attrMod[ATTR_LUCK]          = Rand_Next(0) % 10 - 5;
    }
    Golfer_ClampModifiers(p);
}

// ---- the caddie ---------------------------------------------------------------------------------
// The putt tip is the CPU's shot rehearsal run on a copy of the human in player slot 4: the copy
// is made a CPU, aimed at the pin, and AI_RehearseShot is stepped once per frame until the
// simulated ball stops within 0.05 of the target. 600 frames and it gives up.

#define CADDIE_SLOT       4
#define CADDIE_TOLERANCE  0.0025f   // 0.05 squared
#define CADDIE_MAX_FRAMES 599

extern u8  gCaddieDone;             // 0x80281D49
extern u8  gCaddieActive;           // 0x80281D4A
extern s32 gCaddieFrames;           // 0x80281D4C

u8 AI_RehearseShot(int nPlayer, f32* pOutDist2, u8 bFast, f32 fTolerance);   // 0x8002B030

void Caddie_Start(int nPlayer) {
    if (!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_PUTT) return;
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bNoLuck == 0) {
        fn_80005628(&gPlayers[CADDIE_SLOT], &gPlayers[nPlayer], sizeof(Player));
        gPlayers[CADDIE_SLOT].nController = CONTROLLER_CPU;
        AI_DefaultTarget(CADDIE_SLOT);
        gCaddieDone   = 0;
        gPlayers[CADDIE_SLOT].nRehearseState = 2;
        gCaddieActive = 1;
        gCaddieFrames = 0;
    }
}

void Caddie_Update(int nPlayer) {
    if (!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_PUTT) return;
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bNoLuck == 0) {
        if (!gCaddieActive) return;
        if (gPlayers[nPlayer].bPerfect) {
            f32 fDist2;
            AI_RehearseShot(CADDIE_SLOT, &fDist2, 0, CADDIE_TOLERANCE);
            if (fDist2 < 0.0625f) gCaddieDone = 1;
        } else {
            if (gCaddieDone) return;
            if (AI_RehearseShot(CADDIE_SLOT, NULL, 0, CADDIE_TOLERANCE)) gCaddieDone = 1;
        }
        gCaddieFrames++;
    }
}

// 0 = no tip for this shot, 1 = tip ready (the aim point in pOut), 2 = gave up.
int Caddie_GetTip(int nPlayer, f32* pOut) {
    if (gPlayers[nPlayer].nShotKind != SHOT_PUTT || Player_IsCPU(nPlayer) || gSession.bNoLuck) {
        pOut[0] = pOut[1] = pOut[2] = pOut[3] = 0.0f;
        return 0;
    }
    if (gCaddieFrames > CADDIE_MAX_FRAMES) {
        pOut[0] = pOut[1] = pOut[2] = pOut[3] = 0.0f;
        gCaddieDone = 1;
        return 2;
    }
    Vec_Copy(&gPlayers[CADDIE_SLOT].fTargetX, pOut);
    return (s8)gCaddieDone;
}
