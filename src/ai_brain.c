// ai_brain.c (our name, after TW06's golf/ai/ai_brain.cpp: our AI_RehearseShot is TW06's
// AIBrain_Think; the 2003 game is C): split off Golfer.c at 0x8002A630-0x8002BBB0. Compiled
// alone it reproduces its own .sdata2 pool (0x80282D68-0x80282E40) byte for byte; as one file
// with the parts that follow, the compiler merged their duplicated constants. Data: .data
// 0x80187490-0x801874B0 (a jump table), .bss 0x801C64D8-0x801C65B8, .sdata
// 0x802810A8-0x802810B0, .sbss 0x80281D30-0x80281D40.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

void Golfer_ClampModifiers(Player* pPlayer);
void fn_8002B020_OnTargetChosen(int nPlayer);

// ---- per-shot modifiers (CPU only) ------------------------------------------------------------

// Set a CPU golfer's attribute modifiers for the shot it is about to play; game mode 11 zeroes
// them. A per-player level gives +25 a level; otherwise the worse its hole is going the better it
// gets (+10 at one over, +20 per stroke beyond); in game mode 4, while player 1 leads player 0 in
// holes won, every modifier but power (random) drops 5 per hole of the lead; otherwise every
// modifier is a random -5..+4. Aggression moves the other way in the level and over-par cases.
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
    nPar     = Course_GetCurHolePar();
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
        p->attrMod[ATTR_POWER] = Misc_RandFunc(0) % 10 - 5;
        if (nLead > 0) {
            p->attrMod[ATTR_IQ]            = nLead * -5;
            p->attrMod[ATTR_AGGRESSION]    = nLead * -5;
            p->attrMod[ATTR_BALL_STRIKING] = nLead * -5;
            p->attrMod[ATTR_APPROACH]      = nLead * -5;
            p->attrMod[ATTR_PUTTING]       = nLead * -5;
            p->attrMod[ATTR_RECOVERY]      = nLead * -5;
            p->attrMod[ATTR_LUCK]          = nLead * -5;
        } else {
            p->attrMod[ATTR_IQ]            = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_AGGRESSION]    = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_BALL_STRIKING] = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_APPROACH]      = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_PUTTING]       = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_RECOVERY]      = Misc_RandFunc(0) % 10 - 5;
            p->attrMod[ATTR_LUCK]          = Misc_RandFunc(0) % 10 - 5;
        }
    } else {
        p->attrMod[ATTR_POWER]         = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_IQ]            = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_AGGRESSION]    = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_BALL_STRIKING] = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_APPROACH]      = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_PUTTING]       = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_RECOVERY]      = Misc_RandFunc(0) % 10 - 5;
        p->attrMod[ATTR_LUCK]          = Misc_RandFunc(0) % 10 - 5;
    }
    Golfer_ClampModifiers(p);
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

// ---- planning a shot ------------------------------------------------------------------------------

// Plan the next shot: a CPU sets its modifiers, everyone picks a target, Shot_Prepare fills in
// the rest, the rehearsal is reset, and the luck roll decides whether this shot is perfect.
void Shot_Plan(int nPlayer, u8 bNotify) {
    if (Player_IsCPU(nPlayer)) {
        AI_SetShotModifiers(nPlayer);
    }
    AI_ChooseTarget(nPlayer);
    if (Player_IsCPU(nPlayer)) {
        fn_8002B020_OnTargetChosen(nPlayer);
    }
    Shot_Prepare(nPlayer, bNotify);
    gPlayers[nPlayer].nRehearseState = 2;
    gPlayers[nPlayer].bPerfect       = Golfer_IsLucky(nPlayer);
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
        LLMath_CopyVec(p->vTarget, p->vTarget2);
        fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
    }
    p->nTrajectory = Shot_Trajectory(nPlayer);
    p->fPower    = AI_PowerForTarget(nPlayer);
    fn_8002D544_StraightDir(nPlayer, p->vLaunchA);
    fn_8002D680_CpuShapeDir(nPlayer, p->vLaunchB);
    if (bNotify) {
        Character_SelectGameClub(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
        Character_SelectGameShotType(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    }
}

// Empty; Shot_Plan calls it for a CPU after its target choice.
void fn_8002B020_OnTargetChosen(int nPlayer) {
}

// ---- the CPU's shot rehearsal ---------------------------------------------------------------------
// Before a CPU golfer swings (and for the caddie, on a copy of the human in slot 4) the planned shot
// is rehearsed on a private ball with the real physics, randomness off, one coarse step per frame.
// When the ball stops, the aim is moved by 45% of the miss and it goes again, until the miss is
// under the tolerance. When the ball hits an object (AI_SimAbort), the best aim so far is taken
// if a rehearsal has landed, else the golfer gets +5 on its modifiers and a nudge (the shot shape
// says which way); when it ends in a hazard, +5 and, until one lands, a club swap: longer by one,
// shorter by one, longer by two... from the club it started with. The caller can force state 3
// to make it stop: the best aim found so far, or +25 and a fresh target.

// Defined here, last address first in each section (CodeWarrior lays out .bss and .sbss in
// reverse).
s32  gSimClub[6];                   // 0x801C65A0  per player: club the rehearsal started with
Ball gSimBall;                      // 0x801C64E4  the rehearsal's own ball
f32  gSimBestAim[3];                // 0x801C64D8  the aim that produced it
f32  gSimBestDist = 1e9f;           // 0x802810A8  best miss squared
u8   gSimClubTries[8];              // 0x80281D34  per player: club swaps tried
u8   gSimHaveResult;                // 0x80281D31  at least one rehearsal landed
u8   gSimAborted;                   // 0x80281D30  raised by AI_SimAbort (the ball hit an object)

// +n on every modifier the rehearsal cares about (not LUCK), aggression the other way.
#define BUMP_MODIFIERS(p, n)                                                                       \
    (p)->attrMod[ATTR_POWER]         += (n);                                                       \
    (p)->attrMod[ATTR_IQ]            += (n);                                                       \
    (p)->attrMod[ATTR_AGGRESSION]    -= (n);                                                       \
    (p)->attrMod[ATTR_BALL_STRIKING] += (n);                                                       \
    (p)->attrMod[ATTR_APPROACH]      += (n);                                                       \
    (p)->attrMod[ATTR_PUTTING]       += (n);                                                       \
    (p)->attrMod[ATTR_RECOVERY]      += (n);

// event.c calls this when a simulated ball hits an object (event 36).
void AI_SimAbort(void) {
    gSimAborted = 1;
}

// One frame of the rehearsal. Returns 1 once the aim is settled. pOutDist2, when given, receives
// the miss squared on a frame where a rehearsal lands, else 1e10.
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
        fn_80050D24_SetSimulating(1);
        // Always the normal trajectory.
        Physics_ShotImpact(&gSimBall, p->nClub, p->nShotKind, fPower, p->fAim, 1, p->vLaunchA, p->vLaunchB);
        fn_80050D24_SetSimulating(0);
        p->nRehearseState = 1;
        break;

    case 1:     // step
        gSimAborted = 0;
        fn_80050D24_SetSimulating(1);
        if (bFast) {
            fn_8005585C_SimForTime(&gSimBall, 0.1f, 1.0f);
        } else {
            fn_8005585C_SimForTime(&gSimBall, 0.2f, 1.0f);
        }
        fn_80050D24_SetSimulating(0);
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
            fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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
            fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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
            fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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
            fMaxAngle = DEG(4.25f); fDist1 = 8.5f; fDist2 = 7.0f * 0.85f;
        } else {
            fMaxAngle = DEG(5.0f);  fDist1 = 10.0f; fDist2 = 7.0f;
        }
        // Even a good golfer occasionally blows an approach.
        if (fSkill > 80.0f) {
            if (Misc_RandFunc(0) % 20 == 0) {
                fMaxAngle *= 4.0f; fDist1 *= 4.0f; fDist2 *= 4.0f; fSkill *= 0.25f;
            } else if (Misc_RandFunc(0) % 10 == 0) {
                fMaxAngle *= 2.0f; fDist1 *= 2.0f; fDist2 *= 2.0f; fSkill *= 0.5f;
            }
        } else if (fSkill > 60.0f) {
            if (Misc_RandFunc(0) % 20 == 0) {
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
        fRand   = Misc_RandFuncf(0);
        fMiss   = 100.0f - fSkill;
        fAimErr = fMaxAngle * (fMiss * fRand) / 100.0f;
        if (fAimErr < DEG(0.25f)) {
            fAimErr += DEG(0.25f);
        }
        if (Misc_RandFunc(0) & 1) {
            fAimErr *= -1.0f;
        }
        p->fAim += fAimErr;
        if (p->fAim < -PI) {
            p->fAim += TWOPI;
        } else if (p->fAim > PI) {
            p->fAim -= TWOPI;
        }
        fSin = Math_Sin(p->fAim);
        fCos = Math_Cos(p->fAim);

        // Distance error: two percentage terms, either side; one shot kind always comes up
        // short.
        fRand   = Misc_RandFuncf(0);
        fDistErr  = fDist1 * (fMiss * fRand) / 100.0f;
        fRand     = Misc_RandFuncf(0);
        fDistErr += fDist2 * (fMiss * fRand) / 100.0f;
        // (A full swing on anything but a par 3 always comes up short; otherwise a coin flip.)
        if ((p->nShotKind == SHOT_TYPE_DRIVE_e && Course_GetCurHolePar() != 3) || (Misc_RandFunc(0) & 1)) {
            fDistErr *= -1.0f;
        }
        p->fDistance = p->fDistance * ((100.0f + fDistErr) / 100.0f);
        fDX = p->fDistance * -fSin;
        fDZ = p->fDistance * fCos;
        p->vTarget[0] = p->vBall[0] + fDX;
        p->vTarget[2] = p->vBall[2] + fDZ;
        fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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

// The hole of the round being played, 0..17.
int Game_CurHoleIndex(void) {
    return gpGame->nCurHole;
}
