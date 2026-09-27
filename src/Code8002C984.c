// Code8002C984.c (our name): split off Golfer.c at 0x8002C984-0x8002D8A8. Compiled alone it
// reproduces its own .sdata2 pool (0x80282E80-0x80282EE8) byte for byte; as one file with its
// neighbours, the compiler merged their duplicated constants. Data: .data
// 0x801874B0-0x80187650 (the club tables).

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

// Declared in golfer.h; the values are the DOL's.
u8 gClubKindTable[8][CLUB_MAX_e] = {        // 0x801874B0
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
};
f32 gClubDistAtPower0[CLUB_MAX_e] = {       // 0x80187580
    245.0f, 245.0f, 245.0f, 245.0f, 245.0f, 245.0f, 225.0f, 205.0f, 190.0f, 185.0f, 180.0f, 175.0f, 170.0f,
    165.0f, 160.0f, 150.0f, 140.0f, 130.0f, 120.0f, 100.0f, 95.0f,  90.0f,  85.0f,  80.0f,  75.0f,  10.0f,
};
f32 gClubPowerStep[CLUB_MAX_e] = {          // 0x801875E8
    6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 5.0f, 4.0f, 4.0f, 3.0f, 3.0f, 3.0f, 2.0f,
    2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

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

// ---- shot setup helpers -------------------------------------------------------------------------


// The aim angle from the ball to the target, wrapped to -pi..pi. 0 is +z; positive turns left.
f32 Shot_AimAngle(int nPlayer) {
    f32 fDX = gPlayers[nPlayer].vTarget[0] - gPlayers[nPlayer].vBall[0];
    f32 fDZ = gPlayers[nPlayer].vTarget[2] - gPlayers[nPlayer].vBall[2];
    f32 fAngle = atan2(-fDX, fDZ);
    if (fAngle > PI) {
        fAngle -= 2 * PI;
    } else if (fAngle < -PI) {
        fAngle += 2 * PI;
    }
    return fAngle;
}

// ---- ground probes ------------------------------------------------------------------------------

// Is the ground fDist from the ball toward the pin of class 3 (the green)? True when the ball is
// on the pin. The CPU putts when the green starts within 1.5 and may chip when within 5.
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
    fHeight = Ter_GetHighestGroundHeight(pCourse, vDir);
    if (fHeight != TER_NO_GROUND) {
        vDir[1]  = 10.0f + fHeight;
        pSurface = Ter_GetSupportingGroundMaterial(pCourse, vDir);
        if (pSurface->nClass == 3) {
            bGreen = 1;
        }
    }
    return bGreen;
}

// 0 in lies 6, 7 and 8 (TW06's three sand lies), else 1; no chip is planned from them.
u8 Player_NotInSand(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (p->ball.nLie == 6 || p->ball.nLie == 7 || p->ball.nLie == 8) return 0;
    return 1;
}

// The kind of shot the CPU plays from here for a given distance: putt on the green or
// very close, chip or pitch when a wedge in the bag reaches, otherwise a full swing.
int AI_ShotKindForDistance(int nPlayer, f32 fDist) {
    int nOverride = fn_80100744();
    switch (nOverride) {
    case 8: {
        int     nKind = SHOT_TYPE_DRIVE_e;
        Player* p     = &gPlayers[nPlayer];
        fDist /= Physics_GetLiePowerPercentage(&p->ball);
        if (p->ball.nLie == LIE_GREEN_e || AI_GreenTowardPin(nPlayer, 1.5f)) {
            nKind = SHOT_TYPE_PUTT_e;
        } else if ((p->golfer.uBagMask & (1 << 21)) && fDist < 15.0f && AI_GreenTowardPin(nPlayer, 5.0f) &&
                   Player_NotInSand(nPlayer)) {
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

// How far this golfer can hit this club for this kind of shot. Chips are 30, putts 60; other
// kinds use the club's table distance, which for kinds 1 (full swing), 4, 6 and 7 is scaled by
// POWER (below 100 towards the power-0 table, above 100 a fixed step per point).
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
            fDist /= Physics_GetLiePowerPercentage(&p->ball);
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

// Power (0..1) to reach the current target: putts and chips have their own curves, everything
// else is distance over the club's reach.
f32 AI_PowerForTarget(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    if (p->nShotKind == SHOT_TYPE_PUTT_e) {
        return fn_80050D34(p->fDistance);
    }
    if (p->nShotKind == SHOT_TYPE_CHIP_e) {
        return Physics_EstimateShotPower(p->fDistance, &p->ball, SHOT_TYPE_CHIP_e, p->nClub);
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

// The first launch block (vLaunchA): straight ahead, (0, 0, 1).
void fn_8002D544_StraightDir(int nPlayer, f32* pOut) {
    pOut[0] = 0.0f;
    pOut[1] = 0.0f;
    pOut[2] = 1.0f;
    pOut[3] = 0.0f;
}

// A direction vector from the shot shape (a lesson in mode 11 can dictate the shape): the x part
// is 0.02 either way for a slight curve and 0.04 for a big one, then the vector is normalised.
void fn_8002D560_ShapeDir(int nPlayer, f32* pOut) {
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

// The second launch block (vLaunchB): a CPU's shot-shape direction, a human's straight one.
void fn_8002D680_CpuShapeDir(int nPlayer, f32* pOut) {
    Player* p = &gPlayers[nPlayer];
    if (Controller_IsCPU(p->nController)) {
        fn_8002D560_ShapeDir(nPlayer, pOut);
    } else {
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = 1.0f;
        pOut[3] = 0.0f;
    }
}

// ---- odds and ends ----------------------------------------------------------------------------

// Aim at the pin without changing the shot shape.
void AI_AimAtPin(int nPlayer) {
    Player*     p       = &gPlayers[nPlayer];
    CourseInfo* pCourse = fn_8000C594();
    int         nPinSet = Game_CurrentPinSet();
    p->vTarget[0] = pCourse->pin[nPinSet].x;
    p->vTarget[2] = pCourse->pin[nPinSet].z;
    fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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
        fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
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
        fn_8002BDEC_SetTarget(nPlayer, p->vTarget);
    }
}
