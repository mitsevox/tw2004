// Golfer.c: golfer records, attribute access, and the CPU golfer's shot choice. No assert
// names this file; "Golfer.c" is our name. CodeWarrior GC/2.5, -O4,p. What the code does is
// written up in docs/gameplay.md; the record layout in docs/formats/game-data.md.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState (see game.h).

// ---- small accessors ------------------------------------------------------------------------

u8 Player_IsCPU(int nPlayer) {
    return gPlayers[nPlayer].nController == CONTROLLER_CPU;
}

u8 Controller_IsCPU(int nController) {
    return nController == CONTROLLER_CPU;
}

// The hole of the round being played, 0..17.
int Game_CurHoleIndex(void) {
    return gpGame->nCurHole;
}

// Empty in release: a hook after the CPU's target choice.
void AI_OnTargetChosen(int nPlayer) {
}

// Empty in release.
void fn_8002C8B4(void) {
}

void AI_TargetsHook(void) {
    fn_8002C8B4();
}

// The current hole's pin position, 0..3: which of CourseInfo.pin it uses.
int Game_CurrentPinSet(void) {
    return gpGame->nPinSet[gpGame->nCurHole];
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
    if (nClub == CLUB_PUTTER_e) {
        return ATTR_PUTTING;
    }
    if (nLie == 8 || nLie == 13 || nLie == 11 || nLie == 3 || nLie == 4 || nLie == 6 || nLie == 7) {
        return ATTR_RECOVERY;
    }
    if (nKind == SHOT_TYPE_DRIVE_e && nClub < 13 && fn_800D2B08() == 3) {
        return ATTR_BALL_STRIKING;
    }
    return ATTR_APPROACH;
}

// The one place attributes are read. Base = the record value (block B for a CPU pro in game
// mode 4) plus the equipment tier bonus; modifiers are the per-player adjustments.
int Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode) {
    int nValue = 0;
    if (nMode == ATTR_BASE || nMode == ATTR_TOTAL) {
        // fake match: the (s8) casts on the s8 record values keep the original's extsb before the add
        if (Controller_IsCPU(pPlayer->nController) && pPlayer->golfer.nIndex < FIRST_CREATED_GOLFER &&
            (Game_GetMode() == 4 || (gSession.uFlags & 2))) {
            nValue =
                (s8)((s8)pPlayer->golfer.attrAlt[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        } else {
            nValue = (s8)((s8)pPlayer->golfer.attr[nAttr] + Golfer_TierBonus(pPlayer->golfer.tier[nAttr]));
        }
    }
    if (nMode == ATTR_MODIFIERS || nMode == ATTR_TOTAL) {
        nValue += pPlayer->attrMod[nAttr];
    }
    return nValue;
}

// Keep base + modifiers inside [10, cap]: 110 for power, IQ and aggression, 100 for the rest.
#define CLAMP_ATTR(pPlayer, nAttr, nCap)                                                       \
    if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) > (nCap)) {                        \
        (pPlayer)->attrMod[nAttr] = (nCap) - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);   \
    } else if ((s8)Golfer_GetAttribute(pPlayer, nAttr, ATTR_TOTAL) < 10) {                     \
        (pPlayer)->attrMod[nAttr] = 10 - Golfer_GetAttribute(pPlayer, nAttr, ATTR_BASE);       \
    }

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

// The binary's only powf: the SDK's reverb effect (reverb_hi.c) calls this same function.
f32 powf(f32 x, f32 y) {
    return fn_8015F824(x, y);
}

// Pick where the CPU aims: the most demanding authored aim point it qualifies for. A human
// only ever gets a priority point from here (the default aim on walking up to the ball).
void AI_ChooseTarget(int nPlayer) {
    Player*     p;
    AITarget*   t;
    int         nKind;
    int         nPower;
    int         nAggr;
    int         nPinSet;
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

    nPinSet = Game_CurrentPinSet();
    pCourse = fn_8000C594();
    p       = &gPlayers[nPlayer];
    nAggr   = Golfer_GetAttribute(p, ATTR_AGGRESSION, ATTR_TOTAL);
    nIQ     = Golfer_GetAttribute(p, ATTR_IQ, ATTR_TOTAL);
    nPower  = Golfer_GetAttribute(p, ATTR_POWER, ATTR_TOTAL);
    nZone   = AI_NearestTarget(p->vBall, NULL);
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
            fDZ    = pCourse->pin[nPinSet].z - t->pDef->z;
            fDX    = pCourse->pin[nPinSet].x - t->pDef->x;
            fDist2 = fDX * fDX + fDZ * fDZ;
            if (t->nTeeSet != -1 && t->nTeeSet != gSession.nTeeSet[nPlayer]) continue;
            if (t->nPinSet != -1 && t->nPinSet != nPinSet) continue;
            if (nBest == -1 && t->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (!Player_IsCPU(nPlayer)) continue;

            fDZ    = p->vBall[2] - t->pDef->z;
            fDX    = p->vBall[0] - t->pDef->x;
            fDist  = fn_80009680(fDX * fDX + fDZ * fDZ);
            nKind  = AI_ShotKindForDistance(nPlayer, fDist);
            nClub  = AI_ClubForShot(nPlayer, nKind, 0, fDist);
            nSkill = Golfer_GetAttribute(p, Shot_GoverningAttribute(nPlayer, nClub, p->ball.nLie, nKind),
                                         ATTR_TOTAL);
            if (Player_IsCPU(nPlayer)) {
                // Low IQ makes the golfer think it is better than it is.
                if (p->bLowIQPenalty) {
                    nSkill += (int)(10.0f * (powf(fDumb, 2.0f) / 100.0f) / 100.0f);
                } else {
                    nSkill += (int)(40.0f * (powf(fDumb, 2.0f) / 100.0f) / 100.0f);
                }
                if ((s8)nSkill > 100) {
                    nSkill = 100;
                }
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
    fDZ = p->vBall[2] - pCourse->pin[nPinSet].z;
    fDX = p->vBall[0] - pCourse->pin[nPinSet].x;
    if (fDX * fDX + fDZ * fDZ < fBestDist2 && !(gpGame->nCurCourse == 3 && fn_80015464() == 17)) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    pBest          = &gAITargets[nBest];
    p->vTarget[0]    = pBest->pDef->x;
    p->vTarget[2]    = pBest->pDef->z;
    p->nShotShape = pBest->nType;
    AI_PlanShot(nPlayer, p->vTarget);
    Vec_Copy(p->vTarget, p->vTarget2);
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

    p->swing.fForwardSpin = 0.0f;
    p->swing.fSideSpin = 0.0f;
    if (p->fDistance < 1.0f) return;
    if (p->bPerfect) return;

    nAttr = Shot_GoverningAttribute(nPlayer, p->nClub, p->ball.nLie, p->nShotKind);
    if (nAttr == ATTR_PUTTING) {
        fSkill    = (f32)(s8)Golfer_GetAttribute(p, ATTR_PUTTING, ATTR_TOTAL);
        nSpin     = 0;
        fMaxAngle = DEG(8.0f);
        fDist1    = 20.0f;
        fDist2    = 10.0f;
        if (p->fDistance < 1.5f) return;
        if (p->fDistance < 5.0f) {
            fMaxAngle *= 0.5f;
        }
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

    if (fSkill > 98.0f) {
        fSkill = 98.0f;
    }
    if (fSkill < 100.0f) {
        f32 fDX, fDZ;
        if (fSkill < 15.0f) {
            fSkill = 15.0f;
        }

        // Aim: up to the angle limit, at least a quarter of a degree, either side.
        fRand   = Rand_Float(0);
        fMiss   = 100.0f - fSkill;
        fAimErr = fMaxAngle * (fMiss * fRand) / 100.0f;
        if (fAimErr < DEG(0.25f)) {
            fAimErr += DEG(0.25f);
        }
        if (Rand_Next(0) & 1) {
            fAimErr *= -1.0f;
        }
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
        // (A full swing outside lesson 3 always comes up short; otherwise a coin flip.)
        if ((p->nShotKind == SHOT_TYPE_DRIVE_e && fn_800D2B08() != 3) || (Rand_Next(0) & 1)) {
            fDistErr *= -1.0f;
        }
        p->fDistance = p->fDistance * ((100.0f + fDistErr) / 100.0f);
        fDX = p->fDistance * -fSin;
        fDZ = p->fDistance * fCos;
        p->vTarget[0] = p->vBall[0] + fDX;
        p->vTarget[2] = p->vBall[2] + fDZ;
        AI_PlanShot(nPlayer, p->vTarget);
        p->fPower = AI_PowerForTarget(nPlayer);

        // Spin in proportion to the error, scaled by the SPIN attribute. (Both clamps store +1.)
        if ((s8)nSpin != 0 && fn_80101DF4()) {
            f32 fScale = Swing_SpinScale(nSpin);
            p->swing.fForwardSpin  = fDistErr / fDist1;
            p->swing.fForwardSpin *= fScale;
            p->swing.fSideSpin  = fAimErr / fMaxAngle;
            p->swing.fSideSpin *= fScale;
            p->swing.fSideSpin *= -1.0f;
            if (p->swing.fForwardSpin > 1.0f) {
                p->swing.fForwardSpin = 1.0f;
            } else if (p->swing.fForwardSpin < -1.0f) {
                p->swing.fForwardSpin = 1.0f;
            }
            if (p->swing.fSideSpin > 1.0f) {
                p->swing.fSideSpin = 1.0f;
            } else if (p->swing.fSideSpin < -1.0f) {
                p->swing.fSideSpin = 1.0f;
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
        if ((p->ball.nLie == 6 || p->ball.nLie == 7 || p->ball.nLie == 8) && nClub < 9) {
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
    } while (nClub < CLUB_MAX_e);
    if (nClub == CLUB_MAX_e) {
        int  i;
        for (i = 0; i < CLUB_MAX_e; i++) {
            if ((1 << i) & gPlayers[nPlayer].golfer.uBagMask) return i;
        }
        nClub = CLUB_PUTTER_e;
    }
    return nClub;
}

// How far this golfer can hit this club for this kind of shot. Chips are 30, putts 60; a full
// swing interpolates the club's table distance by POWER (below 100 towards the power-0 table,
// above 100 a fixed step per point).
f32 AI_MaxDistance(int nPlayer, int nKind, int nClub) {
    f32 fMax;
    if (nKind == SHOT_TYPE_CHIP_e) {
        fMax = 30.0f;
    } else if (nKind == SHOT_TYPE_PUTT_e) {
        fMax = 60.0f;
    } else {
        Player* p = &gPlayers[nPlayer];
        fMax = fn_80050F44(nKind, nClub);
        if (nKind == SHOT_TYPE_DRIVE_e || nKind == 7 || nKind == 4 || nKind == 6) {
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
        int     nKind = SHOT_TYPE_DRIVE_e;
        Player* p     = &gPlayers[nPlayer];
        fDist /= fn_800510EC(&p->ball);
        if (p->ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            nKind = SHOT_TYPE_PUTT_e;
        } else if ((p->golfer.uBagMask & (1 << 21)) && fDist < 15.0f && AI_GreenTowardPin(nPlayer, 5.0f) &&
                   Lie_AllowsFullSwing(nPlayer)) {
            nKind = SHOT_TYPE_CHIP_e;
        } else if ((p->golfer.uBagMask & (1 << 23)) && fDist < 20.0f) {
            nKind = SHOT_TYPE_PITCH_e;
        } else if ((p->golfer.uBagMask & (1 << 21)) && fDist < 35.0f) {
            nKind = SHOT_TYPE_PITCH_e;
        } else if ((p->golfer.uBagMask & (1 << 19)) && fDist < 50.0f) {
            nKind = SHOT_TYPE_PITCH_e;
        } else if ((p->golfer.uBagMask & (1 << 18)) && fDist < 65.0f) {
            nKind = SHOT_TYPE_PITCH_e;
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
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000) && gPlayers[nPlayer].ball.nLie == 0) {
        return 2;
    }
    nClub = fn_801006F0(nPlayer);
    if (nClub != CLUB_MAX_e) {
        return nClub;
    }
    fBest = bUnderOnly ? 0.0f : 10000.0f;
    p     = &gPlayers[nPlayer];
    if (nKind == SHOT_TYPE_PUTT_e) {
        nClub = CLUB_PUTTER_e;
    } else if (nKind == SHOT_TYPE_CHIP_e) {
        nClub = CLUB_SANDWEDGE_e;
        if (!Club_UsableForKind(nPlayer, CLUB_SANDWEDGE_e, nKind)) {
            nClub = AI_FirstUsableClub(nPlayer, nKind);
        }
    } else {
        nClub = AI_FirstUsableClub(nPlayer, nKind);
        if (Game_GetMode() == 6 || Game_GetMode() == 7 || Game_GetMode() == 8 ||
            Controller_IsCPU(p->nController)) {
            fDist /= fn_800510EC(&p->ball);
        }
        for (c = 0; c < CLUB_MAX_e; c++) {
            if (c == CLUB_PUTTER_e) continue;
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
    if (p->nShotKind == SHOT_TYPE_PUTT_e) {
        return fn_80050D34(p->fDistance);
    }
    if (p->nShotKind == SHOT_TYPE_CHIP_e) {
        return fn_80050F88(p->fDistance, &p->ball, SHOT_TYPE_CHIP_e, p->nClub);
    }
    return p->fDistance / AI_MaxDistance(nPlayer, p->nShotKind, p->nClub);
}

// Reach for this golfer over the club's table reach; 1 for the shot kinds that do not scale.
f32 AI_PowerScale(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    f32 fTable;
    if (p->nShotKind == SHOT_TYPE_DRIVE_e || p->nShotKind == 7 || p->nShotKind == 4 || p->nShotKind == 6) {
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
            f32 fDX = pPos[0] - gAITargets[i].pDef->x;
            f32 fDZ = pPos[2] - gAITargets[i].pDef->z;
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
    SurfaceType* pSurface = NULL;
    f32          fHeight;
    f32          fDX, fDZ;
    int          nType;

    Vec_Copy(pTarget, gPlayers[nPlayer].vTarget);
    fHeight = Terrain_HeightAt(gPlayers[nPlayer].vTarget, &pSurface);
    gPlayers[nPlayer].uFlagsEF0 &= ~2;
    if (pSurface != NULL) {
        nType = pSurface - gSurfaceTypes;
        if (nType == 98 || nType == 105) {
            // The cup (surfaces 98 and 105 hole the ball, see Ball.c): aim at the pin's height
            // instead and flag it.
            fHeight = fn_8000C594()->pin[Game_CurrentPinSet()].y;
            gPlayers[nPlayer].nSurface = 16;
            gPlayers[nPlayer].uFlagsEF0 |= 2;
        } else {
            gPlayers[nPlayer].nSurface = nType;
        }
    } else {
        gPlayers[nPlayer].nSurface = -1;
    }
    if (fHeight != -65536.1f) {
        gPlayers[nPlayer].vTarget[1] = fHeight + 0.001f;
    }
    fDX = gPlayers[nPlayer].vTarget[0] - gPlayers[nPlayer].vBall[0];
    fDZ = gPlayers[nPlayer].vTarget[2] - gPlayers[nPlayer].vBall[2];
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
    Vec_Copy(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vTargetCopy);
}

// Aim at the pin.
void AI_DefaultTarget(int nPlayer) {
    int         nPinSet = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
    Player*     p       = &gPlayers[nPlayer];
    f32*        pTarget;
    p->vTarget[0]    = pCourse->pin[nPinSet].x;
    pTarget        = p->vTarget;
    p->vTarget[2]    = pCourse->pin[nPinSet].z;
    p->nShotShape = 0;
    AI_PlanShot(nPlayer, pTarget);
    Vec_Copy(pTarget, p->vTarget2);
}

extern s8 gLuckOdds[8];                 // 0x802810B0  "1 in n" per player: 12 12 12 12

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
    if (Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
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
        if (uOdds < 1) {
            uOdds = 1;
        }
    }
    uRoll = Rand_Next(0) % uOdds;
    if (gPlayers[nPlayer].ball.nLie != LIE_GREEN_e && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e &&
        !gSession.nSplitScreen) {
        Game_CurrentPinSet();
        fn_8000C594();
        if (fn_800D2B08() == 3) {
            bLucky = 1;
        } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PITCH_e) {
            bLucky = 1;
        } else if ((gPlayers[nPlayer].ball.nLie == 1 || gPlayers[nPlayer].ball.nLie == 2) &&
                   fn_800D0478(nPlayer) < 250.0f) {
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
    nHole    = Game_CurHoleIndex();
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

void Caddie_Start(int nPlayer) {
    if ((!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e) ||
        Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        return;
    }
    Mem_cpy(&gPlayers[CADDIE_SLOT], &gPlayers[nPlayer], sizeof(Player));
    gPlayers[CADDIE_SLOT].nController = CONTROLLER_CPU;
    AI_DefaultTarget(CADDIE_SLOT);
    gPlayers[CADDIE_SLOT].nRehearseState = 2;
    gCaddieDone   = 0;
    gCaddieActive = 1;
    gCaddieFrames = 0;
}

void Caddie_Update(int nPlayer) {
    f32 fDist2;
    if ((!gPlayers[nPlayer].bPerfect && gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e) ||
        Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        return;
    }
    if (!gCaddieActive) return;
    if (gPlayers[nPlayer].bPerfect) {
        AI_RehearseShot(CADDIE_SLOT, &fDist2, 0, CADDIE_TOLERANCE);
        if (fDist2 < 0.0625f) {
            gCaddieDone = 1;
        }
    } else {
        if (gCaddieDone) return;
        if (AI_RehearseShot(CADDIE_SLOT, NULL, 0, CADDIE_TOLERANCE)) {
            gCaddieDone = 1;
        }
    }
    gCaddieFrames++;
}

// Returns 0 when there is no tip for this shot, 1 when a tip is ready (the aim point in pOut), 2 when
// it gave up.
int Caddie_GetTip(int nPlayer, f32* pOut) {
    if (gPlayers[nPlayer].nShotKind != SHOT_TYPE_PUTT_e || Player_IsCPU(nPlayer) || gSession.nSplitScreen) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        pOut[3] = 0.0f;
        return 0;
    }
    if (gCaddieFrames > CADDIE_MAX_FRAMES) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        pOut[3] = 0.0f;
        gCaddieDone = 1;
        return 2;
    }
    Vec_Copy(gPlayers[CADDIE_SLOT].vTarget, pOut);
    return (s8)gCaddieDone;
}

// ---- the CPU's shot rehearsal ---------------------------------------------------------------------
// Before a CPU golfer swings (and for the caddie, on a copy of the human in slot 4) the planned shot
// is rehearsed on a private ball with the real physics, randomness off, one coarse step per frame.
// When the ball stops, the aim is moved by 45% of the miss and it goes again, until the miss is
// under the tolerance. Trouble - the hazard hook fires, or the ball ends in a hazard - costs the
// golfer +5 on its modifiers and a nudge (the authored aim point says which way), or a club swap:
// longer by one, shorter by one, longer by two... from the club it started with. The caller can
// force state 3 to make it stop: the best aim found so far, or +25 and a fresh target.

extern u8  gSimAborted;             // 0x80281D30  raised by AI_SimAbort from the hazard code
extern u8  gSimHaveResult;          // 0x80281D31  at least one rehearsal landed
extern u8  gSimClubTries[8];        // 0x80281D34  per player: club swaps tried
extern f32 gSimBestDist;            // 0x802810A8  best miss squared
extern f32 gSimBestAim[3];          // 0x801C64D8  the aim that produced it
extern Ball gSimBall;               // 0x801C64E4  the rehearsal's own ball
extern s32 gSimClub[6];             // 0x801C65A0  per player: club the rehearsal started with

// +n on every modifier the rehearsal cares about (not LUCK), aggression the other way.
#define BUMP_MODIFIERS(p, n)                                                                       \
    (p)->attrMod[ATTR_POWER]         += (n);                                                       \
    (p)->attrMod[ATTR_IQ]            += (n);                                                       \
    (p)->attrMod[ATTR_AGGRESSION]    -= (n);                                                       \
    (p)->attrMod[ATTR_BALL_STRIKING] += (n);                                                       \
    (p)->attrMod[ATTR_APPROACH]      += (n);                                                       \
    (p)->attrMod[ATTR_PUTTING]       += (n);                                                       \
    (p)->attrMod[ATTR_RECOVERY]      += (n);

// The hazard code calls this when the ball it is handling is the rehearsal's.
void AI_SimAbort(void) {
    gSimAborted = 1;
}

// Everything that follows from the target and the club choice: aim angle, the club for each shot
// kind, the shot kind for the distance, the club (a CPU's by reach, one longer when the target is
// within 30 of its own height), power, and the two launch parameter blocks.
void Shot_Prepare(int nPlayer, u8 bNotify) {
    Player* p = &gPlayers[nPlayer];
    int     i;
    f32     fRise, fDist;

    p->fAim = Shot_AimAngle(nPlayer);
    for (i = 0; i < 8; i++) {
        p->nShotKind      = i;
        p->nClubPerKind[(u32)i] = AI_ClubForShot(nPlayer, p->nShotKind, 0, p->fDistance);
    }
    p->nShotKind  = AI_ShotKindForDistance(nPlayer, p->fDistance);
    p->nShotKind2 = p->nShotKind;
    if (Player_IsCPU(nPlayer)) {
        fRise = gPlayers[nPlayer].vTarget[1] - gPlayers[nPlayer].vBall[1];
    } else {
        fRise = 0.0f;
    }
    fDist = p->fDistance;
    if (Player_IsCPU(nPlayer)) {
        p->nClub = AI_ClubForShot(nPlayer, p->nShotKind, 1, fDist);
        if (fRise < -30.0f) {
        } else if (fRise < 30.0f) {
            AI_ClubLonger(nPlayer, &p->nClub, 1);
        }
    } else {
        p->nClub = AI_ClubForShot(nPlayer, p->nShotKind, 0, fDist);
    }
    Shot_FitTargetToClub(nPlayer);
    if (!Player_IsCPU(nPlayer)) {
        Vec_Copy(p->vTarget, p->vTarget2);
        AI_PlanShot(nPlayer, p->vTarget);
    }
    p->nTrajectory = Shot_Trajectory(nPlayer);
    p->fPower    = AI_PowerForTarget(nPlayer);
    Shot_DefaultSpin(nPlayer, p->vLaunchA);
    Shot_FaceVector(nPlayer, p->vLaunchB);
    if (bNotify) {
        fn_8001C774(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
        fn_8001C724(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    }
}

// Turn the aim by fDelta radians (wrapped to -pi..pi) and re-plan the target at the same distance.
void AI_NudgeAim(int nPlayer, f32 fDelta) {
    f32     vTarget[4];
    f32     fSin, fCos;

    gPlayers[nPlayer].fAim += fDelta;
    if (gPlayers[nPlayer].fAim < -PI) {
        gPlayers[nPlayer].fAim += 2 * PI;
    } else if (gPlayers[nPlayer].fAim > PI) {
        gPlayers[nPlayer].fAim -= 2 * PI;
    }
    fSin = fn_800095F0(gPlayers[nPlayer].fAim);
    fCos = fn_80009638(gPlayers[nPlayer].fAim);
    vTarget[0] = gPlayers[nPlayer].vBall[0] + -fSin * gPlayers[nPlayer].fDistance;
    vTarget[2] = gPlayers[nPlayer].vBall[2] + fCos * gPlayers[nPlayer].fDistance;
    AI_PlanShot(nPlayer, vTarget);
}

// Lengthen the shot by fDelta and re-plan the target on the same line.
void AI_NudgeDistance(int nPlayer, f32 fDelta) {
    f32     vTarget[4];
    f32     fSin, fCos;

    gPlayers[nPlayer].fDistance += fDelta;
    fSin = fn_800095F0(gPlayers[nPlayer].fAim);
    fCos = fn_80009638(gPlayers[nPlayer].fAim);
    vTarget[0] = gPlayers[nPlayer].vBall[0] + -fSin * gPlayers[nPlayer].fDistance;
    vTarget[2] = gPlayers[nPlayer].vBall[2] + fCos * gPlayers[nPlayer].fDistance;
    AI_PlanShot(nPlayer, vTarget);
}

// *pClub goes nStep clubs longer (lower index), then as many shorter as it takes to find one
// usable for the shot kind. Never the putter, never past the driver.
void AI_ClubLonger(int nPlayer, s32* pClub, int nStep) {
    int nClub;
    u8  bOk;
    if (nStep == 0 || *pClub == CLUB_PUTTER_e || *pClub < nStep) return;
    nClub = *pClub - nStep;
    bOk   = Club_UsableForKind(nPlayer, nClub, gPlayers[nPlayer].nShotKind);
    while (nClub > 0 && !bOk) {
        nClub--;
        bOk = Club_UsableForKind(nPlayer, nClub, gPlayers[nPlayer].nShotKind);
    }
    if (bOk) {
        *pClub = nClub;
    }
}

// *pClub goes nStep clubs shorter (higher index), then further until one is usable.
void AI_ClubShorter(int nPlayer, s32* pClub, int nStep) {
    int nClub;
    u8  bOk;
    if (nStep == 0 || *pClub == CLUB_PUTTER_e || *pClub > CLUB_PUTTER_e - 1 - nStep) return;
    nClub = *pClub + nStep;
    bOk   = Club_UsableForKind(nPlayer, nClub, gPlayers[nPlayer].nShotKind);
    while (nClub < CLUB_PUTTER_e - 1 && !bOk) {
        nClub++;
        bOk = Club_UsableForKind(nPlayer, nClub, gPlayers[nPlayer].nShotKind);
    }
    if (bOk) {
        *pClub = nClub;
    }
}

// One frame of the rehearsal. Returns 1 once the aim is settled. pOutDist2, when given, receives
// the miss squared of the last landed rehearsal (1e10 until one lands).
u8 AI_RehearseShot(int nPlayer, f32* pOutDist2, u8 bFast, f32 fTolerance) {
    Player* p     = &gPlayers[nPlayer];
    u8      bDone = 0;
    f32     fPower;
    f32     fDX, fDZ, fDist2;
    int     nState;
    s8      nTries;

    if (pOutDist2 != NULL) {
        *pOutDist2 = 1e10f;
    }

    switch (p->nRehearseState) {
    case 2:     // reset
        gSimHaveResult          = 0;
        gSimBestDist            = 1e9f;
        gSimAborted             = 0;
        gSimClubTries[nPlayer]  = 0;
        gSimClub[nPlayer]       = gPlayers[nPlayer].nClub;
        p->nRehearseState       = 0;
        break;

    case 0:     // launch
        Mem_cpy(&gSimBall, &p->ball, sizeof(gSimBall));
        fPower = p->fPower * AI_PowerScale(nPlayer);
        if (fPower > 1.5f) {
            fPower = 1.5f;
        }
        Ball_SetSimulating(1);
        // Always the normal trajectory.
        Ball_Launch(&gSimBall, p->nClub, p->nShotKind, fPower, p->fAim, 1, p->vLaunchA, p->vLaunchB);
        Ball_SetSimulating(0);
        p->nRehearseState = 1;
        break;

    case 1:     // step
        gSimAborted = 0;
        Ball_SetSimulating(1);
        if (bFast) {
            Ball_SimStep(&gSimBall, 0.1f, 1.0f);
        } else {
            Ball_SimStep(&gSimBall, 0.2f, 1.0f);
        }
        Ball_SetSimulating(0);
        if (gSimAborted) {
            if (gSimHaveResult) {
                p->vTarget[0]       = gSimBestAim[0];
                p->vTarget[2]       = gSimBestAim[2];
                p->nRehearseState = 0;
                break;
            }
            BUMP_MODIFIERS(p, 5);
            Golfer_ClampModifiers(p);
            switch (p->nShotShape) {
            case SHAPE_NORMAL:   break;
            case SHAPE_FADE:     AI_NudgeAim(nPlayer, DEG(-1.0f)); break;
            case SHAPE_DRAW:     AI_NudgeAim(nPlayer, DEG(1.0f)); break;
            case SHAPE_HIGH:     AI_NudgeDistance(nPlayer, -5.0f); break;
            case SHAPE_LOW:      AI_NudgeDistance(nPlayer, 5.0f); break;
            case SHAPE_SLICE:    AI_NudgeAim(nPlayer, DEG(-2.0f)); break;
            case SHAPE_HOOK:     AI_NudgeAim(nPlayer, DEG(2.0f)); break;
            }
            Shot_Prepare(nPlayer, 0);
            p->nRehearseState = 0;
            break;
        }
        nState = gSimBall.nState;        // 2..4 in motion, 5 in a hazard, 1 stopped
        if (nState == 2) break;
        if (nState == 3 || nState == 4) break;
        if (nState != 5) {
            // Landed: measure the miss from where the CPU wanted the ball.
            fDX    = gSimBall.vPos[0] - p->vTarget2[0];
            fDZ    = gSimBall.vPos[2] - p->vTarget2[2];
            fDist2 = fDX * fDX + fDZ * fDZ;
            if (pOutDist2 != NULL) {
                *pOutDist2 = fDist2;
            }
            gSimHaveResult = 1;
            if (fDist2 < gSimBestDist) {
                gSimBestAim[0] = p->vTarget[0];
                gSimBestDist   = fDist2;
                gSimBestAim[2] = p->vTarget[2];
            }
            if (fDist2 > fTolerance) {
                p->vTarget[0] = p->vTarget[0] - 0.45f * fDX;
                p->vTarget[2] = p->vTarget[2] - 0.45f * fDZ;
            } else {
                p->nRehearseState = 4;
                bDone = 1;
            }
            AI_PlanShot(nPlayer, p->vTarget);
            Shot_Prepare(nPlayer, 0);
        } else {
            // Stopped in a hazard: try another club, from the original, alternating longer and
            // shorter by a growing step.
            if (!gSimHaveResult) {
                gSimClubTries[nPlayer]++;
                nTries = gSimClubTries[nPlayer];
                gPlayers[nPlayer].nClub = gSimClub[nPlayer];
                if (nTries % 2 != 0) {
                    AI_ClubLonger(nPlayer, &p->nClub, (nTries + 1) / 2);
                } else {
                    AI_ClubShorter(nPlayer, &p->nClub, (nTries + 1) / 2);
                }
            }
            BUMP_MODIFIERS(p, 5);
            AI_PlanShot(nPlayer, p->vTarget);
            Shot_Prepare(nPlayer, 0);
        }
        if (!bDone) {
            p->nRehearseState = 0;
        }
        break;

    case 3:     // told to stop: settle for the best, or start over with a fresh target
        if (gSimHaveResult) {
            p->vTarget[0] = gSimBestAim[0];
            p->vTarget[2] = gSimBestAim[2];
            AI_PlanShot(nPlayer, p->vTarget);
            p->nRehearseState = 4;
            bDone = 1;
        } else {
            BUMP_MODIFIERS(p, 25);
            Golfer_ClampModifiers(p);
            AI_ChooseTarget(nPlayer);
        }
        Shot_Prepare(nPlayer, 0);
        break;

    case 4:
        return 1;
    }
    return bDone;
}

// ---- shot setup helpers -------------------------------------------------------------------------

double fn_8015F7C4(double y, double x);   // atan2

// The aim angle from the ball to the target, wrapped to -pi..pi. 0 is +z; positive turns left.
f32 Shot_AimAngle(int nPlayer) {
    f32 fDX = gPlayers[nPlayer].vTarget[0] - gPlayers[nPlayer].vBall[0];
    f32 fDZ = gPlayers[nPlayer].vTarget[2] - gPlayers[nPlayer].vBall[2];
    f32 fAngle = fn_8015F7C4(-fDX, fDZ);
    if (fAngle > PI) {
        fAngle -= 2 * PI;
    } else if (fAngle < -PI) {
        fAngle += 2 * PI;
    }
    return fAngle;
}

// The trajectory the shot shape asks for: 3 and 4 are the two alternatives, anything else normal.
int Shot_Trajectory(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    int     nTrajectory;
    if (p->nShotShape == SHAPE_HIGH) {
        nTrajectory = 2;
    } else if (p->nShotShape == SHAPE_LOW) {
        nTrajectory = 0;
    } else {
        nTrajectory = 1;
    }
    return nTrajectory;
}

// The first launch block: no spin offset.
void Shot_DefaultSpin(int nPlayer, f32* pOut) {
    pOut[0] = 0.0f;
    pOut[1] = 0.0f;
    pOut[2] = 1.0f;
    pOut[3] = 0.0f;
}

// A CPU's clubface vector from its shot shape (a lesson in mode 11 can dictate the shape):
// The x part is 0.02 either way for a slight curve and 0.04 for a big one, then the vector is normalised.
void AI_FaceVector(int nPlayer, f32* pOut) {
    Player* p = &gPlayers[nPlayer];
    int     nShape = fn_8010069C(nPlayer);
    if (nShape != 7) {
        p->nShotShape = nShape;
    }
    pOut[0] = 0.0f;
    pOut[1] = 0.0f;
    pOut[2] = 1.0f;
    pOut[3] = 0.0f;
    if (p->nShotShape == SHAPE_NORMAL) {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    } else if (p->nShotShape == SHAPE_FADE) {
        pOut[0] = 0.02f;
        pOut[1] = 0.0f;
        pOut[2] = 0.98f;
        pOut[3] = 0.0f;
    } else if (p->nShotShape == SHAPE_DRAW) {
        pOut[0] = -0.02f;
        pOut[1] = 0.0f;
        pOut[2] = 0.98f;
        pOut[3] = 0.0f;
    } else if (p->nShotShape == SHAPE_SLICE) {
        pOut[0] = 0.04f;
        pOut[1] = 0.0f;
        pOut[2] = 0.96f;
        pOut[3] = 0.0f;
    } else if (p->nShotShape == SHAPE_HOOK) {
        pOut[0] = -0.04f;
        pOut[1] = 0.0f;
        pOut[2] = 0.96f;
        pOut[3] = 0.0f;
    }
    Vec_Normalize(pOut, pOut);
}

// The second launch block: a CPU's shaped clubface, a human's square one.
void Shot_FaceVector(int nPlayer, f32* pOut) {
    Player* p = &gPlayers[nPlayer];
    if (Controller_IsCPU(p->nController)) {
        AI_FaceVector(nPlayer, pOut);
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
}

// Fit the target to the club: past its reach, pull the target in to the reach; short of it, a
// human's target (not with the putter, not on a chip) is pushed out to the full reach - the aim
// marker always sits at the club's distance and power does the rest. A CPU keeps a short target.
void Shot_FitTargetToClub(int nPlayer) {
    Player* p    = &gPlayers[nPlayer];
    f32     fMax = AI_MaxDistance(nPlayer, p->nShotKind, p->nClub);
    f32     fSin, fCos, fDX, fDZ;
    if (p->fDistance > fMax) {
        fSin = fn_800095F0(p->fAim);
        fCos = fn_80009638(p->fAim);
        fDZ  = fMax * fCos;
        fDX  = fMax * -fSin;
        p->vTarget[0] = p->vBall[0] + fDX;
        p->vTarget[2] = p->vBall[2] + fDZ;
        AI_PlanShot(nPlayer, p->vTarget);
    } else if (p->fDistance < fMax) {
        if (Controller_IsCPU(p->nController)) return;
        if (p->nClub == CLUB_PUTTER_e) return;
        if (p->nShotKind == SHOT_TYPE_CHIP_e) return;
        fSin = fn_800095F0(p->fAim);
        fCos = fn_80009638(p->fAim);
        fDZ  = fMax * fCos;
        fDX  = fMax * -fSin;
        p->vTarget[0] = p->vBall[0] + fDX;
        p->vTarget[2] = p->vBall[2] + fDZ;
        AI_PlanShot(nPlayer, p->vTarget);
    }
}

// ---- planning a shot ------------------------------------------------------------------------------

// Plan the next shot: a CPU sets its modifiers, everyone picks a target, Shot_Prepare fills in
// the rest, the rehearsal is reset, and the luck roll decides whether this shot is perfect.
void Shot_Plan(int nPlayer, u8 bNotify) {
    if (Player_IsCPU(nPlayer)) {
        AI_SetShotModifiers(nPlayer);
    }
    AI_ChooseTarget(nPlayer);
    if (Player_IsCPU(nPlayer)) {
        AI_OnTargetChosen(nPlayer);
    }
    Shot_Prepare(nPlayer, bNotify);
    gPlayers[nPlayer].nRehearseState = 2;
    gPlayers[nPlayer].bPerfect       = Golfer_IsLucky(nPlayer);
}

// ---- the aim point table --------------------------------------------------------------------------
// Loaded from a course chunk: s16, s16 count, count x AITargetDef (0x30 each), then count x 8 bytes
// of requirements (tee set, hole, skill, aggression, priority, type, power, pad).

extern u8 gAITargetsLoaded;             // 0x80281D40

u8   Course_RegisterLoader(int nChunk, void (*pfn)(u8*));   // 0x8000C0B4

void AI_TargetsClear(void) {
    int i;
    for (i = 0; i < NUM_AI_TARGETS; i++) {
        fn_80005AE8(&gAITargets[i], 0, sizeof(AITarget));
        gAITargets[i].nPinSet  = -1;
        gAITargets[i].nTeeSet  = -1;
        gAITargets[i].bEnabled = 1;
    }
    gAITargetsLoaded = 0;
}

void AI_TargetsLoad(u8* pChunk) {
    int          i, k;
    AITargetDef* pDef;
    u8*          pReq;

    gNumAITargets = 0;
    // port: the course's AI targets are big-endian and read in place: AITargetDef is laid over the
    // chunk (and written to), and gAITargets points into it; a little-endian port converts the
    // chunk's AITargetDefs before this loop (the count at +2 is read with BES16).
    pDef = (AITargetDef*)(pChunk + 4);
    for (i = 0; i < BES16(pChunk + 2); pDef++, i++) {
        gAITargets[i].pDef = pDef;
        // A point that links to itself links to nothing.
        for (k = 0; k < NUM_AI_LINKS; k++) {
            if (pDef->nLinks[k] == i) {
                pDef->nLinks[k] = -1;
            }
        }
        gNumAITargets++;
    }
    pReq = (u8*)pDef;
    for (i = 0; i < BES16(pChunk + 2); i++, pReq += 8) {
        gAITargets[i].nTeeSet   = pReq[0];
        gAITargets[i].nPinSet   = pReq[1];
        gAITargets[i].nSkillReq = pReq[2];
        gAITargets[i].nAggrReq  = pReq[3];
        gAITargets[i].bPriority = pReq[4];
        gAITargets[i].nType     = pReq[5];
        gAITargets[i].nPowerReq = pReq[6];
        gAITargets[i].bEnabled  = 1;
    }
    gAITargetsLoaded = 1;
}

void AI_TargetsInit(void) {
    gAITargetsLoaded = 0;
    Course_RegisterLoader(0, AI_TargetsLoad);
    AI_TargetsClear();
}

// ---- odds and ends ----------------------------------------------------------------------------

// Aim at the pin without changing the shot shape.
void AI_AimAtPin(int nPlayer) {
    Player*     p       = &gPlayers[nPlayer];
    CourseInfo* pCourse = fn_8000C594();
    int         nPinSet = Game_CurrentPinSet();
    p->vTarget[0] = pCourse->pin[nPinSet].x;
    p->vTarget[2] = pCourse->pin[nPinSet].z;
    AI_PlanShot(nPlayer, p->vTarget);
}

// The luck odds: "1 in gLuckOdds[n]". 12 at the start of a round, and every hole transition
// takes one off any player still above 9 - so 1 in 12, then 1 in 11, then 1 in 10 for the rest.
void Luck_ResetOdds(int nPlayer) {
    gLuckOdds[nPlayer] = 12;
}

void Luck_ResetAllOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        Luck_ResetOdds((u8)i);
    }
}

void Luck_ResetAllOdds2(void) {
    Luck_ResetAllOdds();
}

void Luck_TightenOdds(void) {
    int i;
    for (i = 0; i < 5; i++) {
        if (gLuckOdds[i] > 9) {
            gLuckOdds[i]--;
        }
    }
}

// Lies 6, 7 and 8 do not allow a full swing.
u8 Lie_AllowsFullSwing(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (p->ball.nLie == 6 || p->ball.nLie == 7 || p->ball.nLie == 8) return 0;
    return 1;
}

// ---- ground probes ------------------------------------------------------------------------------

// Is the ground fDist yards from the ball toward the pin of class 3 (the green)? True when the
// ball is on the pin. The CPU putts from the fringe when the green starts within 1.5 yards and
// chips when it starts within 5.
u8 AI_GreenTowardPin(int nPlayer, f32 fDist) {
    u8          bGreen  = 0;
    int         nPinSet = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
    f32         vDir[4];
    f32         fHeight;
    SurfaceType* pSurface;

    vDir[0] = pCourse->pin[nPinSet].x - gPlayers[nPlayer].ball.vPos[0];
    vDir[1] = 0.0f;
    vDir[2] = pCourse->pin[nPinSet].z - gPlayers[nPlayer].ball.vPos[2];
    vDir[3] = 0.0f;
    if (0.0f == vDir[0] && 0.0f == vDir[2]) {
        return 1;
    }
    Vec_Normalize(vDir, vDir);
    vDir[0] = gPlayers[nPlayer].ball.vPos[0] + vDir[0] * fDist;
    vDir[2] = gPlayers[nPlayer].ball.vPos[2] + vDir[2] * fDist;
    fHeight = fn_8004D5C0(pCourse, vDir);
    if (fHeight != -65536.1f) {
        vDir[1]  = 10.0f + fHeight;
        pSurface = fn_800CC190(pCourse, vDir);
        if (pSurface->nClass == 3) {
            bGreen = 1;
        }
    }
    return bGreen;
}

// Stop the caddie.
void Caddie_Stop(void) {
    gCaddieActive = 0;
}

// ---- the golfer table's arrival ---------------------------------------------------------------

void Golfer_TableSetup(void);
void fn_80076158(u8** ppSrc, u8* pDst, int nBytes, int nWidth);   // byte-swap copy, nWidth 2/4/8

// The 0xA8 bytes at +0x98 of every record are 21 eight-byte values stored little-endian:
// swap them in place.
void Golfer_TableByteSwap(void) {
    u8* pSrc;
    int i;
    for (i = 0; i < NUM_GOLFERS; i++) {
        pSrc = (u8*)&gGolferTable[i] + 0x98;
        // port: 'stat' is little-endian on disc; a little-endian port does not swap here. Only
        // 0x98..0x140 of each record is swapped, in 8-byte units (the u32 at 0x90 is not); the
        // records are then read in place as GolferRecord.
        fn_80076158(&pSrc, (u8*)&gGolferTable[i] + 0x98, 0xA8, 8);
    }
}

// The 'stat' handler: copy the file over the table, fix its endianness, set it up.
void Golfer_OnStatsLoaded(UStreamObject* pObject) {
    Mem_cpy(gGolferTable, pObject->pData, pObject->uSize);
    Golfer_TableByteSwap();
    fn_80009E70(pObject);
    Golfer_TableSetup();
}

void Golfer_RegisterStatsHandler(void) {
    UStream_RegisterHandler('stat', Golfer_OnStatsLoaded);
}

void Golfer_UnregisterStatsHandler(void) {
    UStream_UnregisterHandler('stat');
}

// Take the caddie's solved shot (slot 4) as the player's own.
void Caddie_ApplyTip(int nPlayer) {
    gPlayers[nPlayer].nClub       = gPlayers[CADDIE_SLOT].nClub;
    gPlayers[nPlayer].nTrajectory = gPlayers[CADDIE_SLOT].nTrajectory;
    gPlayers[nPlayer].nShotKind   = gPlayers[CADDIE_SLOT].nShotKind;
    gPlayers[nPlayer].fPower      = gPlayers[CADDIE_SLOT].fPower;
    gPlayers[nPlayer].fAim        = gPlayers[CADDIE_SLOT].fAim;
}

// ---- small queries ------------------------------------------------------------------------------

void fn_80013200(int nPad, u8 nValue);

u8 Player_OnTee(int nPlayer) {
    return gPlayers[nPlayer].ball.nLie == 0;
}

u8 Player_IsHoled(int nPlayer) {
    return gPlayers[nPlayer].ball.nLie == LIE_INCUP_e;
}

u8 Player_IsHoledNotState23(int nPlayer) {
    int bResult = 0;
    if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e &&
        (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED) {
        bResult = 1;
    }
    return bResult;
}

// Team 0 is players 0 and 1, team 1 players 2 and 3.
u8 Team_IsAllCPU(int nTeam) {
    s32 i;
    int nFirst = 2;
    if (nTeam == 0) {
        nFirst = 0;
    }
    for (i = 0; i < 2; i++) {
        if (gPlayers[nFirst + i].nController != CONTROLLER_CPU) return 0;
    }
    return 1;
}

u8 Team_IsAllHuman(int nTeam) {
    s32 i;
    int nFirst = 2;
    if (nTeam == 0) {
        nFirst = 0;
    }
    for (i = 0; i < 2; i++) {
        if (gPlayers[nFirst + i].nController == CONTROLLER_CPU) return 0;
    }
    return 1;
}

u8 Player_IsController8(int nPlayer) {
    return gPlayers[nPlayer].nController == 8;
}

// Controllers 0..7 are pads; 8 is something else; 9 is the CPU.
u8 Player_HasPad(int nPlayer) {
    if (gPlayers[nPlayer].nController <= 7) return 1;
    return 0;
}

u8 Controller_IsPad(int nController) {
    return nController <= 7;
}

u8 Player_IsNotCPU(int nPlayer) {
    if (gPlayers[nPlayer].nController <= 8) return 1;
    return 0;
}

u8 Controller_IsNotCPU(int nController) {
    return nController <= 8;
}

// The bag: one bit per club in the player's copy of the golfer record.
u8 Bag_AddClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bAdded = 0;
    if (!(p->golfer.uBagMask & uBit)) {
        p->golfer.uBagMask |= uBit;
        bAdded = 1;
    }
    return bAdded;
}

u8 Bag_RemoveClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bRemoved = 0;
    if (p->golfer.uBagMask & uBit) {
        p->golfer.uBagMask &= ~uBit;
        bRemoved = 1;
    }
    return bRemoved;
}

u8 Bag_HasClub(int nPlayer, int nBit) {
    Player* p = &gPlayers[nPlayer];
    u32     uBit = 1 << nBit;
    u8      bHas = 0;
    if (p->golfer.uBagMask & uBit) {
        bHas = 1;
    }
    return bHas;
}

int Bag_CountClubs(int nPlayer) {
    Player* p      = &gPlayers[nPlayer];
    int     nCount = 0;
    int     i;
    for (i = 0; i < CLUB_MAX_e; i++) {
        if (p->golfer.uBagMask & (1 << i)) {
            nCount++;
        }
    }
    return nCount;
}

// The table slot whose record carries this id byte, or -1.
int Golfer_FindById(int nId) {
    int i;
    for (i = 0; i < NUM_GOLFERS; i++) {
        if (nId == gGolferTable[i].nModelID) return i;
    }
    return -1;
}

// The 'rcrd' handler: the courses' records into the session.
void Session_OnRecordsLoaded(UStreamObject* pObject) {
    // port: the records are big-endian on disc and copied straight over the course-record structs;
    // a little-endian port converts them field by field here.
    Mem_cpy(gSession.aCourseRecord, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}

void Session_RegisterRecordsHandler(void) {
    UStream_RegisterHandler('rcrd', Session_OnRecordsLoaded);
}

void Session_UnregisterRecordsHandler(void) {
    UStream_UnregisterHandler('rcrd');
}

void fn_8002EBA4(u8* pObj, u8 nValue) {
    int i;
    pObj[7] = nValue;
    for (i = 0; i < 4; i++) {
        fn_80013200(i, pObj[7]);
    }
}

// ---- the lucky shot ---------------------------------------------------------------------------
// When a human wins the luck roll (Player.bPerfect), Caddie_Start has been running the CPU's
// rehearsal on a copy of them in slot 4, aimed at the pin, since the shot was planned. At the
// moment the ball is struck this swaps the rehearsed shot in for theirs - club, trajectory, kind,
// power and aim - provided they were playing roughly the same shot: a club within two of the
// rehearsed one, the same shot kind, an aim within 5 degrees. Otherwise the shot is not perfect
// after all. A taken lucky shot puts the player's odds back to 1 in 12.
void Luck_TakePerfectShot(int nPlayer) {
    f32  fDiff;

    if (gPlayers[nPlayer].bPerfect == 0 || Player_IsCPU(nPlayer) || gSession.nSplitScreen != 0) return;
    {
        if (gPlayers[nPlayer].nClub > gPlayers[CADDIE_SLOT].nClub + 2) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gPlayers[CADDIE_SLOT].nClub > gPlayers[nPlayer].nClub + 2) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gPlayers[CADDIE_SLOT].nShotKind != gPlayers[nPlayer].nShotKind) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        fDiff = gPlayers[nPlayer].fAim - gPlayers[CADDIE_SLOT].fAim;
        while (fDiff < -PI) {
            fDiff += 2 * PI;
        }
        while (fDiff > PI) {
            fDiff -= 2 * PI;
        }
        if (fabs(fDiff) > 0.0872665) {
            gPlayers[nPlayer].bPerfect = 0;
            return;
        }
        if (gCaddieDone) {
            Luck_ResetOdds(nPlayer);
            gPlayers[nPlayer].nClub = gPlayers[CADDIE_SLOT].nClub;
            gPlayers[nPlayer].nTrajectory = gPlayers[CADDIE_SLOT].nTrajectory;
            gPlayers[nPlayer].nShotKind = gPlayers[CADDIE_SLOT].nShotKind;
            gPlayers[nPlayer].fPower = gPlayers[CADDIE_SLOT].fPower;
            gPlayers[nPlayer].fAim = gPlayers[CADDIE_SLOT].fAim;
        }
        Caddie_Stop();
    }
}

// ---- setting up the players ---------------------------------------------------------------------

#define BAG_ALL      0x03FFFFFF     // every club
#define BAG_DEFAULT  0x01FFFC7F     // a bag with no clubs 7, 8, 9 (the 3-, 4-, 5-woods?) or 25


u8    fn_800170A0(int nView);                                   // the view exists
void  fn_80016D18(int nView, f32 x, f32 y, f32 w, f32 h);       // open it (screen fractions)
void  fn_80009710(f32* pQuat);                                  // identity (0, 0, 0, 1)
void  fn_8005CE70(int nPlayer);
void  fn_80095504(int n);

// Fill a player slot from the golfer table and set up its view(s). uBag overrides the record's
// bag when non-zero (or everything, with session flag 0x200); an empty bag gets the default.
// In split screen, bRightSide picks the half; otherwise view 0, plus view 2 in game type 4.
void Player_SetGolfer(int nPlayer, int nGolfer, int nController, u32 uBag, int bRightSide) {
    Player* p = &gPlayers[nPlayer];
    s32     i;

    p->nIndex = nPlayer;
    Mem_cpy(&p->golfer, &gGolferTable[nGolfer], sizeof(GolferRecord));
    p->golfer.nIndex = nGolfer;
    if (gSession.uFlags & 0x200) {
        uBag |= BAG_ALL;
    }
    if (uBag != 0) {
        p->golfer.uBagMask = uBag;
    }
    if (p->golfer.uBagMask == 0) {
        p->golfer.uBagMask = BAG_DEFAULT;
    }
    for (i = 0; i < NUM_ATTRS; i++) {
        p->attrMod[i] = 0;
    }
    p->nClub       = 0;
    p->fAim        = 0.0f;
    p->fPower      = 100.0f;
    p->nShotKind   = SHOT_TYPE_DRIVE_e;
    p->nTrajectory = 1;
    p->vBall[0]      = 0.0f;
    p->vBall[1]      = 0.0f;
    p->vBall[2]      = 0.0f;
    p->vBall[3]      = 0.0f;
    p->ball.nLie        = 0;
    p->vTarget[0]    = 0.0f;
    p->vTarget[1]    = 0.0f;
    p->vTarget[2]    = 0.0f;
    p->vTarget[3]    = 0.0f;
    fn_80009710(p->vOrient);
    p->nController = nController;
    p->bMulliganUsed = 0;
    if (gSession.nSplitScreen) {
        if (bRightSide == 0) {
            if (!fn_800170A0(0)) {
                fn_80016D18(0, 0.0f, 0.0f, 0.5f, 1.0f);
            }
            p->nView[0] = 0;
            p->nView[1] = 0;
            fn_8001704C(p->nView[0], nPlayer);
            fn_8001704C(p->nView[1], nPlayer);
        } else if (bRightSide == 1) {
            if (!fn_800170A0(1)) {
                fn_80016D18(1, 0.5f, 0.0f, 0.5f, 1.0f);
            }
            p->nView[0] = 1;
            p->nView[1] = 1;
            fn_8001704C(p->nView[0], nPlayer);
            fn_8001704C(p->nView[1], nPlayer);
        }
    } else {
        if (!fn_800170A0(0)) {
            fn_80016D18(0, 0.0f, 0.0f, 1.0f, 1.0f);
        }
        p->nView[0] = 0;
        fn_8001704C(p->nView[0], nPlayer);
        if (gSession.nGameType == 4) {
            int nView;
            if (!fn_800170A0(2)) {
                fn_80016D18(2, 0.0f, 0.0f, 1.0f, 1.0f);
            }
            p->nView[1] = 2;
            fn_8001704C(p->nView[1], nPlayer);
            nView = p->nView[1];
            View_SetCamera(fn_80017028(nView), 0x19, nPlayer, nView);
        } else {
            p->nView[1] = 0;
            fn_8001704C(p->nView[1], nPlayer);
        }
    }
    p->pChar = gViewSlots[nPlayer].pChar;
    if (p->pChar != NULL) {
        p->pChar->nPlayer = nPlayer;
        if (gSession.nSplitScreen == 2) {
            p->pChar->n16DC = gNumPlayersSetUp * 2;
        }
    }
    p->swing.unk630 = 0;
    p->bLowIQPenalty = 0;
    p->nLevel       = 0;
    p->fC20         = 0.0f;
    p->bC2D         = 0;
    p->bPlanReady       = 0;
    p->bRehearsalDone       = 0;
    p->uFlags       = 0;
    gNumPlayersSetUp++;
}

// Every player in the session, from the session's golfer, controller and bag arrays; odd
// players take the right half of a split screen.
void Players_SetupAll(void) {
    u8 i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (i % 2 != 0) {
            Player_SetGolfer(i, gSession.nGolfer[i], gSession.nController[i], gSession.uBag[i], 1);
        } else {
            Player_SetGolfer(i, gSession.nGolfer[i], gSession.nController[i], gSession.uBag[i], 0);
        }
    }
}

void Players_Reset(void) {
    int i;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        GOLFERSTATE_Kill(i);
        PLAYER(i)->pChar = NULL;
    }
    gNumPlayersSetUp = 0;
}

void Session_SetNumPlayers(int n) {
    gSession.nNumPlayers = n;
}

void Session_SetGolfer(int nGolfer, int nPlayer) {
    gSession.nGolfer[nPlayer] = nGolfer;
}

void fn_8002F180(void) {
    fn_80095504(0);
    fn_800953C8(0);
}

void fn_8002E258(void) {
}

void fn_8002E25C(void) {
}

// ---- the session and its options ---------------------------------------------------------------

extern char gszEmpty[8];            // 0x802810B8  "" (small data)
extern char lbl_80187650[];         // "cl_bbsd" ... the default name at +0x1A

void fn_800CB700(char* pDst, char* pSrc);       // string copy

// Options_SetDefaults(): the defaults, then the debug "all 105" variant when session flag
// 0x4000 is set.
void Options_SetDefaults(GameOptions* pOpt) {
    int i, j;
    pOpt->a0[0]  = 4;
    pOpt->a0[1]  = 4;
    pOpt->a0[2]  = 5;
    pOpt->a0[3]  = 1;
    pOpt->a0[4]  = 4;
    pOpt->a7[1]    = 1;
    pOpt->a7[2]    = 1;
    pOpt->bGimmes  = 1;
    pOpt->bSkipCameras = 0;
    pOpt->nC       = 2;
    pOpt->nWind    = 0;
    pOpt->n14      = 0;
    pOpt->n18      = 1;
    pOpt->n1C      = 1;
    for (i = 0; i < 8; i++) {
        pOpt->a24[i] = 1;
    }
    pOpt->bBoostEnabled = 1;
    pOpt->bSpinEnabled  = 1;
    pOpt->b7E = 0;
    pOpt->n80 = 1;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 19; j++) {
            pOpt->rows[i][j] = 1;
        }
    }
    pOpt->b7A = 1;
    pOpt->b7B = 0;
    pOpt->b7C = 1;
    pOpt->b7D = 0;
    pOpt->b84 = 0;
    fn_8002EBA4((u8*)pOpt, 1);
    if (gSession.uFlags & 0x4000) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 19; j++) {
                pOpt->rows[i][j] = 0;
            }
        }
        pOpt->b7A      = 1;
        pOpt->rows[0][13] = 1;
        pOpt->rows[0][15] = 1;
        pOpt->rows[0][17] = 1;
        pOpt->rows[1][0]  = 1;
        fn_8002EBA4((u8*)pOpt, 0);
    }
    fn_80055C40(pOpt->n18);
    fn_80055CD0(pOpt->n1C);
}

// A fresh session: one player, every slot a CPU on tee set 2 with an empty profile.
void Session_Init(void) {
    Session* pSession = &gSession;
    int      i, j;

    pSession->uFlags      = 0;
    pSession->nGameType   = 0;
    pSession->a8[0]     = 0;
    pSession->nC          = 0;
    pSession->nSplitScreen = 0;
    pSession->b11         = 0;
    pSession->b12         = 0;
    pSession->bReplay     = 0;
    pSession->nPaused   = 0;
    pSession->uFlags     &= ~0x40;
    pSession->fFrameTime  = 0.0f;
    pSession->f1C         = 0.0f;
    pSession->n20         = 0;
    pSession->nFrameCount = 0;
    pSession->n28         = 0;
    Options_SetDefaults(&gSession.options);
    gSession.nSeed = Rand_Next(0);
    fn_8000B1D4(0, gSession.nSeed);
    gSession.nNumPlayers = 1;
    gSession.nPinSet     = -1;
    gSession.bStrokeLimit = 1;
    for (i = 0; i < 5; i++) {
        gSession.nController[i] = CONTROLLER_CPU;
        gSession.nGolfer[i]     = 0;
        gSession.nTeeSet[i]     = 2;
        gSession.uBag[i]        = 0;
        gSession.aProfile[i].n0 = 0;
        gSession.aProfile[i].n1 = 0;
        gSession.aProfile[i].n2 = 0;
        for (j = 0; j < 6; j++) {
            fn_800CB700(gSession.aProfile[i].szNames[j], gszEmpty);
        }
    }
    gSession.f5B3C = 0.0f;
    gSession.f5B40 = 150.0f;
    gSession.f5B44 = -400.0f;
    gSession.f5B48 = 1.0f;
}

// Fill each player's profile from their golfer: a created golfer's from the save, the two
// default golfers' from a fixed name, a pro's ball type from their SPIN rating.
void Session_SetupProfiles(void) {
    Session* pSession = &gSession;
    int      i, j;
    s8       nSpin;

    for (i = 0; i < pSession->nNumPlayers; i++) {
        // fake match: the cast keeps this near-miss at 70.9% (&gSession.aProfile[i]: 70.1%)
        PlayerProfile* pProf = (PlayerProfile*)gSession.aProfile + i;
        int            nGolfer;
        pProf->n1 = 0;
        nSpin = gGolferTable[pSession->nGolfer[i]].attr[ATTR_SPIN];
        for (j = 0; j < 6; j++) {
            fn_800CB700(pProf->szNames[j], gszEmpty);
        }
        pProf->nOutfit = gGolferTable[pSession->nGolfer[i]].nOutfit;
        nGolfer = pSession->nGolfer[i];
        if (nGolfer >= FIRST_CREATED_GOLFER) {
            SaveProfile* pSave = &gpSaveData[nGolfer - FIRST_CREATED_GOLFER];
            for (j = 0; j < 6; j++) {
                ((u32*)pProf->szNames[j])[0] = ((u32*)pSave->szGolferNames[j])[0];
                ((u32*)pProf->szNames[j])[1] = ((u32*)pSave->szGolferNames[j])[1];
            }
            pProf->n2        = pSave->n54C2;
            pProf->nBallType = pSave->nGolferBallType;
            pProf->nOutfit   = pSave->nGolferOutfit;
        } else if (nGolfer == 0 || nGolfer == 1) {
            fn_800CB700(pProf->szNames[0], lbl_80187650 + 0x1A);
            pProf->n2        = 0;
            pProf->nBallType = 0;
        } else if (fn_80077B18(nGolfer)) {
            pProf->n2        = 0;
            pProf->nBallType = 0;
        } else {
            pProf->n2 = 0;
            if (nSpin >= 100) {
                pProf->nBallType = 3;
            } else if (nSpin >= 75) {
                pProf->nBallType = 2;
            } else {
                pProf->nBallType = nSpin >= 50;
            }
        }
    }
}

// The table after the 'stat' file arrives: the created golfer being edited goes into slot 30
// when it has one, and the debug "105%" flag (session bit 0x4000) maxes everyone out.
void Golfer_TableSetup(void) {
    int i, k;
    gNumPlayersSetUp = 0;
    if ((s8)gCurGolferRecord.bAvailable != 0) {
        Mem_cpy(&gGolferTable[FIRST_CREATED_GOLFER], &gCurGolferRecord, sizeof(GolferRecord));
    }
    if (gSession.uFlags & 0x4000) {
        for (i = 0; i < NUM_GOLFERS; i++) {
            for (k = 0; k < NUM_ATTRS; k++) {
                gGolferTable[i].attr[k] = 105;
            }
            gGolferTable[i].uBagMask = 0x02A7FC44;
        }
    }
}
