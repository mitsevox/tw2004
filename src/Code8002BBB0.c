// Code8002BBB0.c (our name): split off Golfer.c at 0x8002BBB0-0x8002C984. Compiled alone it
// reproduces its own .sdata2 pool (0x80282E40-0x80282E80) byte for byte; as one file with its
// neighbours, the compiler merged their duplicated constants. Data: .bss
// 0x801C65B8-0x801C66E8 (gAITargets), .sbss 0x80281D40-0x80281D48.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

void AI_TargetsClear(void);
void AI_TargetsLoad(u8* pChunk);
void fn_8002C8B4(void);

// ---- the aim point table --------------------------------------------------------------------------
// Loaded from a course chunk: s16, s16 count, count x AITargetDef (0x30 each), then count x 8 bytes
// of requirements (tee set, pin position, skill, aggression, priority, type, power, pad).

// Defined here, last address first (CodeWarrior lays out .sbss in reverse); gAITargets
// (0x801C65B8) and gNumAITargets (0x80281D44) are declared in golfer.h.
AITarget gAITargets[25];
s32      gNumAITargets;
u8       gAITargetsLoaded;              // 0x80281D40


void AI_TargetsInit(void) {
    gAITargetsLoaded = 0;
    Course_RegisterLoader(0, AI_TargetsLoad);
    AI_TargetsClear();
}

void AI_TargetsClear(void) {
    int i;
    for (i = 0; i < NUM_AI_TARGETS; i++) {
        Mem_set(&gAITargets[i], 0, sizeof(AITarget));
        gAITargets[i].nPinSet  = -1;
        gAITargets[i].nTeeSet  = -1;
        gAITargets[i].bEnabled = 1;
    }
    gAITargetsLoaded = 0;
}

void fn_8002BC6C(void) {
    fn_8002C8B4();
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

// Fill in everything that follows from a target: the landing surface, the target height, the
// distance, and (for a human) a snap to the club's reach when it is just under.
void fn_8002BDEC_SetTarget(int nPlayer, f32* pTarget) {
    SurfaceType* pSurface = NULL;
    f32          fHeight;
    f32          fDX, fDZ;
    int          nType;

    Vec_Copy(pTarget, gPlayers[nPlayer].vTarget);
    fHeight = CamScript_GuessBestPlayableHeight(gPlayers[nPlayer].vTarget, &pSurface);
    gPlayers[nPlayer].uFlagsEF0 &= ~2;
    if (pSurface != NULL) {
        nType = pSurface - gSurfaceTypes;
        if (nType == 98 || nType == 105) {
            // The cup (surfaces 98 and 105 hole the ball, see Ball.c): aim at the pin's height
            // instead and flag it.
            fHeight = Ter_GetTGD()->pin[Game_CurrentPinSet()].y;
            gPlayers[nPlayer].nSurface = 16;
            gPlayers[nPlayer].uFlagsEF0 |= 2;
        } else {
            gPlayers[nPlayer].nSurface = nType;
        }
    } else {
        gPlayers[nPlayer].nSurface = -1;
    }
    if (fHeight != TER_NO_GROUND) {
        gPlayers[nPlayer].vTarget[1] = fHeight + 0.001f;
    }
    fDX = gPlayers[nPlayer].vTarget[0] - gPlayers[nPlayer].vBall[0];
    fDZ = gPlayers[nPlayer].vTarget[2] - gPlayers[nPlayer].vBall[2];
    if (0.0f == fDX && 0.0f == fDZ) {
        gPlayers[nPlayer].fDistance = 10.0f;
    } else {
        gPlayers[nPlayer].fDistance = Math_Sqrt(fDX * fDX + fDZ * fDZ);
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
    fSin = Math_Sin(gPlayers[nPlayer].fAim);
    fCos = Math_Cos(gPlayers[nPlayer].fAim);
    vTarget[0] = gPlayers[nPlayer].vBall[0] + -fSin * gPlayers[nPlayer].fDistance;
    vTarget[2] = gPlayers[nPlayer].vBall[2] + fCos * gPlayers[nPlayer].fDistance;
    fn_8002BDEC_SetTarget(nPlayer, vTarget);
}

// Lengthen the shot by fDelta and re-plan the target on the same line.
void AI_NudgeDistance(int nPlayer, f32 fDelta) {
    f32     vTarget[4];
    f32     fSin, fCos;

    gPlayers[nPlayer].fDistance += fDelta;
    fSin = Math_Sin(gPlayers[nPlayer].fAim);
    fCos = Math_Cos(gPlayers[nPlayer].fAim);
    vTarget[0] = gPlayers[nPlayer].vBall[0] + -fSin * gPlayers[nPlayer].fDistance;
    vTarget[2] = gPlayers[nPlayer].vBall[2] + fCos * gPlayers[nPlayer].fDistance;
    fn_8002BDEC_SetTarget(nPlayer, vTarget);
}

// Aim at the pin.
void AI_DefaultTarget(int nPlayer) {
    int         nPinSet = Game_CurrentPinSet();
    CourseInfo* pCourse = Ter_GetTGD();
    Player*     p       = &gPlayers[nPlayer];
    f32*        pTarget;
    p->vTarget[0]    = pCourse->pin[nPinSet].x;
    pTarget        = p->vTarget;
    p->vTarget[2]    = pCourse->pin[nPinSet].z;
    p->nShotShape = 0;
    fn_8002BDEC_SetTarget(nPlayer, pTarget);
    Vec_Copy(pTarget, p->vTarget2);
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

// Pick where the CPU aims: the most demanding authored aim point it qualifies for. A human
// only ever gets a priority point from here (the default aim on walking up to the ball).
void AI_ChooseTarget(int nPlayer) {
    s8          k;
    s8          nBest;
    s8          nCand;
    int         nClub;
    f32         fDumb;         // (100 - IQ): the overconfidence term
    int         nIQ;
    f32         fDist2;
    int         nSkill;
    int         nAttr;
    f32         fDist;
    f32         fBestDist2;
    Player*     p;
    AITarget*   t;
    // fake match: the untyped alias preserves the original target-table register order.
    void*       pTargets;
    int         nKind;
    CourseInfo* pCourse;
    s8          nZone;
    // fake match: widened attribute locals reproduce the original hoisted cast order.
    long long   nPower;
    // fake match: keep the narrow power value separate for register allocation.
    int         powerByte;
    int         nPinSet;
    // fake match: like nPower, widen storage only to match the original cast scheduling.
    long long   nAggr;
    f32         fDX, fDZ;

    nPinSet = Game_CurrentPinSet();
    pCourse = Ter_GetTGD();
    p       = &gPlayers[nPlayer];
    nAggr   = Golfer_GetAttribute(p, ATTR_AGGRESSION, ATTR_TOTAL);
    nIQ     = Golfer_GetAttribute(p, ATTR_IQ, ATTR_TOTAL);
    nPower  = Golfer_GetAttribute(p, ATTR_POWER, ATTR_TOTAL);
    nZone   = AI_NearestTarget(p->vBall, NULL);
    if (nZone == -1) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    // fake match: keep the zone's target-table base separate to match register allocation.
    pTargets   = gAITargets;
    nBest      = -1;
    fBestDist2 = 100000000.0f;
    if (((AITarget*)pTargets)[nZone].pDef != NULL) {
        for (k = 0; k < NUM_AI_LINKS; k++) {
            nCand = (s8)((AITarget*)pTargets)[nZone].pDef->nLinks[k];
            t = &gAITargets[nCand];
            if (nCand == -1) continue;
            if (!t->bEnabled) continue;
            fDX    = pCourse->pin[nPinSet].x - t->pDef->x;
            fDZ    = pCourse->pin[nPinSet].z - t->pDef->z;
            fDist2 = fDX * fDX + fDZ * fDZ;
            if (t->nTeeSet != -1 && t->nTeeSet != gSession.nTeeSet[nPlayer]) continue;
            if (t->nPinSet != -1 && t->nPinSet != nPinSet) continue;
            fDumb = 100.0f - (f32)(s8)nIQ;
            if (nBest == -1 && t->bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (!Player_IsCPU(nPlayer)) continue;

            fDX    = p->vBall[0] - t->pDef->x;
            fDZ    = p->vBall[2] - t->pDef->z;
            fDist  = Math_Sqrt(fDX * fDX + fDZ * fDZ);
            nKind  = AI_ShotKindForDistance(nPlayer, fDist);
            nClub  = AI_ClubForShot(nPlayer, nKind, 0, fDist);
            nAttr  = Shot_GoverningAttribute(nPlayer, nClub, p->ball.nLie, nKind);
            nSkill = Golfer_GetAttribute(p, nAttr, ATTR_TOTAL);
            if (Player_IsCPU(nPlayer)) {
                // Low IQ makes the golfer think it is better than it is.
                if (p->bLowIQPenalty) {
                    nSkill += (int)(10.0f * (powf(fDumb, 2.0f) / 100.0f) / 100.0f);
                    if ((s8)nSkill > 100) {
                        nSkill = 100;
                    }
                } else {
                    nSkill += (int)(40.0f * (powf(fDumb, 2.0f) / 100.0f) / 100.0f);
                    if ((s8)nSkill > 100) {
                        nSkill = 100;
                    }
                }
            }
            if (t->nSkillReq < 0 && (s8)nSkill > __abs(t->nSkillReq)) continue;
            if ((s8)nSkill < t->nSkillReq) continue;
            if (t->nAggrReq < 0 && (s8)nAggr > __abs(t->nAggrReq)) continue;
            if ((s8)nAggr < t->nAggrReq) continue;
            powerByte = (s8)nPower;
            if (t->nPowerReq < 0 && powerByte > __abs(t->nPowerReq)) continue;
            if ((s8)nPower < t->nPowerReq) continue;
            if (fDist > AI_MaxDistance(nPlayer, nKind, AI_FirstUsableClub(nPlayer, nKind))) continue;

            if (nBest == -1) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (gAITargets[nBest].bPriority) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            // Prefer the more demanding point: power, then aggression, then skill, then closer.
            if (t->nPowerReq > gAITargets[nBest].nPowerReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (gAITargets[nBest].nPowerReq != __abs(t->nPowerReq)) continue;
            if (t->nAggrReq > gAITargets[nBest].nAggrReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (gAITargets[nBest].nAggrReq != __abs(t->nAggrReq)) continue;
            if (t->nSkillReq > gAITargets[nBest].nSkillReq) {
                fBestDist2 = fDist2;
                nBest      = nCand;
                continue;
            }
            if (gAITargets[nBest].nSkillReq != __abs(t->nSkillReq)) continue;
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
    fDX = p->vBall[0] - pCourse->pin[nPinSet].x;
    fDZ = p->vBall[2] - pCourse->pin[nPinSet].z;
    if (fDX * fDX + fDZ * fDZ < fBestDist2 && !(gpGame->nCurCourse == 3 && Game_GetCurHoleNum() == 17)) {
        AI_DefaultTarget(nPlayer);
        return;
    }
    p->vTarget[0]    = gAITargets[nBest].pDef->x;
    p->vTarget[2]    = gAITargets[nBest].pDef->z;
    p->nShotShape = gAITargets[nBest].nType;
    fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
    Vec_Copy(p->vTarget, p->vTarget2);
}

// Empty.
void fn_8002C8B4(void) {
}

// The current hole's pin position, 0..3: which of CourseInfo.pin it uses.
int Game_CurrentPinSet(void) {
    return gpGame->nPinSet[gpGame->nCurHole];
}

// ---- the CPU golfer ---------------------------------------------------------------------------

// The binary's only powf: the SDK's reverb effect (reverb_hi.c) calls this same function.
f32 powf(f32 x, f32 y) {
    return pow(x, y);
}

// Which attribute governs a shot: the putter, then bad lies, then a full swing with a long
// club (below 13) on a par 3, then everything else is an approach.
int Shot_GoverningAttribute(int nPlayer, int nClub, int nLie, int nKind) {
    if (nClub == CLUB_PUTTER_e) {
        return ATTR_PUTTING;
    }
    if (nLie == 8 || nLie == 13 || nLie == 11 || nLie == 3 || nLie == 4 || nLie == 6 || nLie == 7) {
        return ATTR_RECOVERY;
    }
    if (nKind == SHOT_TYPE_DRIVE_e && nClub < 13 && Course_GetCurHolePar() == 3) {
        return ATTR_BALL_STRIKING;
    }
    return ATTR_APPROACH;
}
