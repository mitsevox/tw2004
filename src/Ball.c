// Ball.c: the ball's flight and roll. No assert names this file; "Ball.c" is our name (the
// leaked list's PsBallFx.c is elsewhere). CodeWarrior GC/2.5, -O4,p. Units are yards and
// seconds; every constant below is a whole number of inches (1/36 yd). The cup is real geometry (surface kinds 12/18, type 90): the ball is holed when it
// has dropped below the pin height. What is written up in docs/gameplay.md is the near-cup
// pull below.

#include "golfer.h"

#define BALL_RADIUS 0.0256667f      // 0.92 in (a real one is 0.84)
#define CUP_DIAMETER 0.10717f       // 3.86 in (a real cup is 4.25)

typedef struct Ball {
    f32  vPos[3];               // 0x00
    u8   unkC[4];
    f32  vPrev[3];              // 0x10  position last step
    u8   unk1C[4];
    f32  vVel[3];               // 0x20
    f32  f2C;                   // 0x2C
    f32  vSpin[3];              // 0x30
    f32  f3C;                   // 0x3C
    f32  vStart[3];             // 0x40  where the shot started
    u8   unk4C[4];
    f32  fSpeed;                // 0x50
    f32  fHeight;               // 0x54  height above the ground when dropped
    f32  fSpinX;                // 0x58  spin input x 15
    f32  fSpinY;                // 0x5C
    f32  fClosest;              // 0x60  closest approach to the pin so far
    s32  nState;                // 0x64  0 placed, 1 stopped, 2 in the air, 3/4 on the ground, 5 in a hazard
    s32  nLie;                  // 0x68  12 holed, 16 hazard
    s32  n6C;                   // 0x6C
    f32  f70;                   // 0x70
    s32  nSurface;              // 0x74  surface type under the ball (90 = the cup)
    s32  nStartSurface;         // 0x78  surface at the start of the shot
    CourseInfo* pCourse;        // 0x7C
    s32  n80;                   // 0x80
    s32  n84;                   // 0x84
    u8   unk88[0x94 - 0x88];
    s32  nPlayer;               // 0x94  -1 when nobody's
    u8   bHoled;                // 0x98
    u8   b99;                   // 0x99
    u8   b9A;                   // 0x9A
    u8   b9B;                   // 0x9B
    u8   unk9C;                 // 0x9C
    u8   unk9D[0xAC - 0x9D];
    f32  fAC;                   // 0xAC
    u8   unkB0[4];
    f32  fB4;                   // 0xB4
    f32  fB8;                   // 0xB8
} Ball;

void   Vec3Copy(f32* pSrc, f32* pDst);           // 0x80008304
f32    Vec_Distance(f32* pA, f32* pB);           // 0x800BB050
f32    fn_8000AD78(f32 y, f32 x);                // atan2f
f32    fn_8000AD9C(f32 x);                       // fabsf
void   Ball_Stop(Ball* pBall);                   // 0x80054340
void   Vec_Copy(f32* pSrc, f32* pDst);           // 0x8000AD10
f32    fn_800095F0(f32 x);                       // sinf
f32    fn_80009638(f32 x);                       // cosf
void   fn_8000AE28(f32* pIn, f32 f, f32* pOut);  // scale a vector
void   fn_80067074(int nPlayer, int nEvent, int a, int b);   // the game-event table
void   Ball_SetLie(Ball* pBall, SurfaceType* pSurface);
void   Ball_Tick(Ball* pBall, f32 fTicks);
void   fn_8005418C(Ball* pBall, int a, f32 f);
SurfaceType* fn_8004D838(CourseInfo* pCourse, Ball* pBall);   // the surface under a point
f32    fn_8004D620(CourseInfo* pCourse, f32* pPos);   // ground height, -60000 and below if none
f32    fn_8004D5C0(CourseInfo* pCourse, f32* pPos);   // the same from another source
u8     fn_8004C798(s32 nSurface);
void   fn_8004C590(CourseInfo* pCourse, Ball* pBall, int a, u8* pA, u8* pB, int b);
// One club's distances for a shot kind: power 0.0, 0.1 .. 1.0 (fDist[9], "the reach", is what
// AI_PowerScale divides by).
typedef struct ClubRow {
    f32  fDist[11];
} ClubRow;

u8     fn_80050DE4(int nKind, int nClub, int a, ClubRow** ppRow, s32* pSurface);
f32    fn_8004D890(CourseInfo* pCourse, Ball* pBall, SurfaceType** ppSurface, f32* pNormal);   // ground height, surface and normal
void   fn_8001EF34(f32* pIn, f32 f, f32* pOut);   // scale a vector
u8     fn_800512BC(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB, f32* pVel, f32* pSpin);
extern f32     gPuttDist[23];                    // 0x80181604  putt distance at power 0, 0.05 .. 1.1
extern f32     gPuttSpeedScale[5];               // 0x80181660  x by the green-speed setting
extern ClubRow gClubRows1[25];                   // 0x80181674  full swing
extern ClubRow gClubRows2[25];                   // 0x80181AC0  chip (wedges and putter only)
extern ClubRow gClubRows3[25];                   // 0x80181F0C
extern ClubRow gClubRows4[25];                   // 0x80182358
extern ClubRow gClubRows5[25];                   // 0x801827A4
extern ClubRow gClubRows6[25];                   // 0x80182BF0
extern ClubRow gClubRows7[25];                   // 0x8018303C
extern f32 gKindSpeed[8];                        // 0x80181328  launch speed x, per shot kind
extern f32 gKindLoft[8];                         // 0x80181348  launch angle +, per shot kind (radians)
extern f32 gKindSpin[8];                         // 0x80181368  spin x, per shot kind
extern f32 gTrajLoft[3];                         // 0x80181388  -5, 0, +5 degrees
extern f32 gClubSpeed[26];                       // 0x80181394  launch speed, per club
extern f32 gClubLoft[26];                        // 0x801813FC  launch angle, per club (6 .. 60 degrees)
extern s32 gClubStep[26];                        // 0x80181464  0 for the woods, then 1 .. 15
extern f32 gChipLoft[26];                        // 0x801814CC  launch angle for a chip
extern f32 gChipSpeed[26];                       // 0x80181534  launch speed for a chip
extern f32 gClubSpin[26];                        // 0x8018159C  spin, per club
void   fn_800BAF04(f32* pSrc, f32* pDst);        // normalise
f32    fn_8000C5FC(f32* pA, f32* pB);            // dot product
void   fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // a + f x b
void   fn_8001EF78(f32* pA, f32* pB, f32* pOut); // cross product
void   fn_80055E7C(f32* pA, f32* pB, f32* pOut);
void   fn_80055EA0(f32* pA, f32* pB, f32* pOut);
void   fn_80055EC4(f32* pA, f32* pB, f32* pOut);
f32    fn_80051124(Ball* pBall, f32 fAim, f32* pNormal);
f32    fn_800511F0(Ball* pBall, f32 fAim, f32* pNormal);
int    Game_GetCourse(void);                     // 0x80008830
int    Hole_WindDir(void);
f32    Hole_WindSpeed(void);
int    fn_801021FC(void);
void   fn_800348DC(void);

f32    fn_80055E10(f32 a, f32 b, f32 fSin, f32 fCos);
f32    fn_80055E1C(f32 a, f32 b, f32 fSin, f32 fCos);
void   fn_80055E28(f32 fAngle, f32* pSin, f32* pCos);
void   fn_80055D70(f32* pA, f32* pB, f32 fSin, f32 fCos);
void   fn_80050CAC(Ball* pBall, u8 bSound);
f32    Ball_DistanceToPin(f32* pPos);
void   Ball_Holed(Ball* pBall);
void   Ball_SimSeconds(Ball* pBall, f32 fSeconds, f32 fTick);

extern u8  gSimulating;                          // 0x80281DD0  a rehearsal: no sounds or effects
extern u8  lbl_80281DD1;
extern u8  lbl_80281DD2;
extern s32 lbl_80281DD4;                         // course setting (options +0x18), 0..2
extern s32 lbl_80281DD8;                         // course setting, 0..2
extern s32 lbl_80281130;                         // green speed, 0..4, default 2 (gPuttSpeedScale)
extern s32 lbl_80281134;                         // course setting (options +0x1C), 0..2, default 1
extern u8  lbl_80281DE4;
extern f32 gWindSpeed;                           // 0x80281DE8
extern s32 gWindDir;                             // 0x80281DEC  0..7
extern f32 gWindDirs[8][4];                      // 0x80187EF8  unit vectors, 45 degrees apart
extern f32 lbl_801D5888[4][4];                   // per player: where the ball was last on ...
extern f32 lbl_801D58C8[4][4];                   // ... two kinds of surface (fn_8004C590)

#define PIN(pBall) ((f32*)&(pBall)->pCourse->pin[Game_CurrentHole()])

// The ball dropped in: park it in the cup and stop it.
void Ball_Holed(Ball* pBall) {
    Vec3Copy(PIN(pBall), pBall->vPos);
    pBall->vPos[0] += 0.0138889f;
    pBall->vPos[1] -= 0.0833333f;
    pBall->vPos[2] += 0.0138889f;
    pBall->bHoled = 1;
    Ball_Stop(pBall);
}

// The ball ends in a hazard: state 5, lie 16, and (for a real ball) the splash event 0x22.
// Surfaces 98 and 105 send it to Ball_Holed instead.
void fn_80050CAC(Ball* pBall, u8 bSound) {
    if (pBall->nSurface == 105 || pBall->nSurface == 98) {
        Ball_Holed(pBall);
        return;
    }
    pBall->nState = 5;
    pBall->nLie   = 16;
    if (bSound && pBall->nPlayer >= 0) {
        fn_80067074(pBall->nPlayer, 0x22, (int)pBall, !gSimulating);
    }
}

// On for rehearsals and look-aheads: no sounds, no effects, no tree roll.
void Ball_SetSimulating(u8 bOn) {
    gSimulating = bOn;
}

void fn_80050D2C(u8 b) {
    lbl_80281DD1 = b;
}

// Putt power for a distance: the putt table (22 steps of 0.05 power, distance ~ 43.4 x power
// squared on a medium green) scaled by the green-speed setting, interpolated; 1.1 beyond it.
f32 fn_80050D34(f32 fDist) {
    f32 fScale = gPuttSpeedScale[lbl_80281130];
    int i;
    for (i = 1; i < 23; i++) {
        f32 fHi = fScale * gPuttDist[i];
        if (fDist <= fHi) {
            f32 fLo = fScale * gPuttDist[i - 1];
            return 0.05f * ((fDist - fLo) / (fHi - fLo)) + 0.05f * (i - 1);
        }
    }
    return 1.1f;
}


// A club's distance row for a shot kind (1..7, clubs 0..24): the row, and the surface the
// table assumes (45; 14 for the chip table). 0 for a putt or a bad club.
u8 fn_80050DE4(int nKind, int nClub, int a, ClubRow** ppRow, s32* pSurface) {
    if (nClub < 0 || nClub >= 25) return 0;
    switch (nKind) {
    case 1:
        *ppRow = &gClubRows1[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    case 2:
        *ppRow = &gClubRows2[nClub];
        if (pSurface != NULL) *pSurface = 14;
        break;
    case 3:
        *ppRow = &gClubRows3[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    case 4:
        *ppRow = &gClubRows4[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    case 5:
        *ppRow = &gClubRows5[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    case 6:
        *ppRow = &gClubRows6[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    case 7:
        *ppRow = &gClubRows7[nClub];
        if (pSurface != NULL) *pSurface = 45;
        break;
    default:
        return 0;
    }
    return 1;
}

// A club's reach for a shot kind (the table entry's +0x24), 1 if there is none.
f32 fn_80050F44(int nKind, int nClub) {
    ClubRow* pRow;
    if (fn_80050DE4(nKind, nClub, 0, &pRow, NULL)) {
        return pRow->fDist[9];
    }
    return 1.0f;
}

// Power for a distance with a club: the row's 11 distances are power 0.0 to 1.0, interpolated,
// plus the difference between the table's surface and the one under the ball (a surface that
// is not a stopping surface counts as 14); 1.1 beyond the row.
f32 fn_80050F88(f32 fDist, u8* p, int nKind, int nClub) {
    Ball*        pBall = (Ball*)p;
    s32          nSurface;
    SurfaceType* pSurface;
    ClubRow*     pRow;
    f32          vNormal[4];
    f32          fBase, fAdj, fFrac;
    int          i;
    if (pBall == NULL) return 0.0f;
    if (!fn_80050DE4(nKind, nClub, 0, &pRow, &nSurface)) return 1.0f;
    fBase = gSurfaceTypes[nSurface].f00;
    if (fn_8004D890(pBall->pCourse, pBall, &pSurface, vNormal) < -60000.0f || 0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    fAdj = fBase - pSurface->f00;
    for (i = 1; i < 12; i++) {
        if (fDist <= pRow->fDist[i]) {
            fFrac = (fDist - pRow->fDist[i - 1]) / (pRow->fDist[i] - pRow->fDist[i - 1]);
            return 0.1f * fFrac + 0.1f * (i - 1) + fAdj;
        }
    }
    return 1.1f;
}

// The ball's f70 plus its surface's first value; 1 without a ball or a surface.
f32 fn_800510EC(u8* p) {
    Ball* pBall = (Ball*)p;
    if (pBall != NULL && pBall->nSurface >= 0) {
        return pBall->f70 + gSurfaceTypes[pBall->nSurface].f00;
    }
    return 1.0f;
}

// The slope along the aim: the ground normal turned into the aim's frame, then the angle of
// its x against its y, clamped to +-44 degrees.
f32 fn_80051124(Ball* pBall, f32 fAim, f32* pNormal) {
    f32 vN[4];
    f32 fSin, fCos;
    f32 fAngle;
    f32 fEps;
    Vec3Copy(pNormal, vN);
    fn_80055E28(fAim, &fSin, &fCos);
    fn_80055D70(&vN[2], &vN[0], fSin, fCos);
    fEps = 1e-6f;
    if (vN[1] < fEps && vN[1] > -fEps) {
        if (vN[0] < 0.0f) {
            fAngle = -1.5707964f;
        } else {
            fAngle = 1.5707964f;
        }
    } else {
        fAngle = -fn_8000AD78(vN[0], vN[1]);
    }
    if (fAngle < -0.76794487f) return -0.76794487f;
    if (fAngle > 0.76794487f) return 0.76794487f;
    return fAngle;
}

// The slope across the aim: as fn_80051124 with the normal's z.
f32 fn_800511F0(Ball* pBall, f32 fAim, f32* pNormal) {
    f32 vN[4];
    f32 fSin, fCos;
    f32 fAngle;
    f32 fEps;
    Vec3Copy(pNormal, vN);
    fn_80055E28(fAim, &fSin, &fCos);
    fn_80055D70(&vN[2], &vN[0], fSin, fCos);
    fEps = 1e-6f;
    if (vN[1] < fEps && vN[1] > -fEps) {
        if (vN[0] < 0.0f) {
            fAngle = -1.5707964f;
        } else {
            fAngle = 1.5707964f;
        }
    } else {
        fAngle = -fn_8000AD78(vN[2], vN[1]);
    }
    if (fAngle < -0.76794487f) return -0.76794487f;
    if (fAngle > 0.76794487f) return 0.76794487f;
    return fAngle;
}

static inline f32 Ball_Clamp(f32 x, f32 fLo, f32 fHi) {
    if (x < fLo) return fLo;
    if (x > fHi) return fHi;
    return x;
}

// The strike: club, shot kind, power, aim, trajectory and the two launch blocks become the
// ball's velocity and spin, on the ground under the ball (no ground: 0).
//
// Speed: a per-club (chips: per-club chip; pitches with clubs 18..24 and kinds 5..7: fixed)
// speed, less 0.01 per club step for kind 4, x the kind's factor x 8.33 x power.
// A putt (kind 0 or the putter) is x 7.2 along pB, turned to the aim, laid onto the ground
// plane, x 1.8, no spin. Anything else is x 12.83 along pB, then:
// - uphill along the aim (fn_800511F0 > 0) costs speed, (max - slope) / max with max = 125
//   degrees (75 for a pitch) - 1.8 per club step + 40 x the slope, and tilts the ball up;
// - kind 5 is tilted up another 42 degrees;
// - the launch angle is the club's loft (chips: their own), + the kind's, - 0.8 degrees per
//   club step for kind 4, + the trajectory's (-5, 0, +5 degrees), clamped to 0..80 degrees,
//   and turns pA into the spin axis;
// - a sidehill lie (fn_80051124, +-45 degrees) turns that axis by 0.2 of the slope - not for
//   slot 4 or a perfect shot;
// - the lie (rough 6/7/8, sand 3/4) or the surface sets how much of the speed survives and
//   how much spin; the club step adds 1.25% of the loss back per step; a chip from lie 3..5
//   loses another 0.1;
// - spin = cross(the part of the direction off the axis, the axis turned by 0.9 of the side
//   slope) x club spin x kind spin x 0.85 x the lie's spin / 0.84.
// Velocity and spin are then turned to the aim.
u8 fn_800512BC(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA,
               f32* pB, f32* pVel, f32* pSpin) {
    f32          vNormal[4];
    f32          vDir[4];
    f32          vAxis[4];
    f32          vAlong[4];
    f32          vOff[4];
    f32          vOffPart[4];
    f32          fSinAim, fCosAim;
    f32          fSin, fCos;
    f32          fSinF, fCosF;
    f32          fSinS, fCosS;
    SurfaceType* pSurface;
    f32          fSpeed, fSlope, fSide, fMax, fLaunch, fKeep, fSpin;
    int          nLie;

    if (fn_8004D890(pBall->pCourse, pBall, &pSurface, vNormal) < -60000.0f) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    switch (nKind) {
    case 2:
        fSpeed = gChipSpeed[nClub];
        break;
    case 5:
        fSpeed = 0.38f;
        break;
    case 6:
        fSpeed = 0.3f;
        break;
    case 7:
        fSpeed = 0.47f;
        break;
    case 3:
        switch (nClub) {
        case 18:
            fSpeed = 0.386f;
            break;
        case 19:
            fSpeed = 0.373f;
            break;
        case 20:
            fSpeed = 0.372f;
            break;
        case 21:
            fSpeed = 0.37f;
            break;
        case 22:
            fSpeed = 0.299f;
            break;
        case 23:
            fSpeed = 0.299f;
            break;
        case 24:
            fSpeed = 0.299f;
            break;
        default:
            goto normal;
        }
        break;
    default:
    normal:
        fSpeed = gClubSpeed[nClub];
        if (nKind == 4) {
            fSpeed -= 0.01f * gClubStep[nClub];
        }
        break;
    }
    fSpeed *= gKindSpeed[nKind];
    fSpeed = fPower * (8.333333f * fSpeed);
    fn_80055E28(fAim, &fSinAim, &fCosAim);
    if (nKind == SHOT_PUTT || nClub == CLUB_PUTTER) {
        fSpeed *= 7.2f;
        fn_8001EF34(pB, fSpeed, vDir);
        fn_80055D70(&vDir[0], &vDir[2], fSinAim, fCosAim);
        fn_800BAF04(vNormal, vNormal);
        fn_8000C5D4(vDir, vNormal, -fn_8000C5FC(vDir, vNormal), pVel);
        fn_8001EF34(pVel, 1.8f, pVel);
        pSpin[0] = 0.0f;
        pSpin[1] = 0.0f;
        pSpin[2] = 0.0f;
        goto done;
    }
    fSpeed *= 12.833333f;
    Vec3Copy(pB, vDir);
    fSlope = fn_800511F0(pBall, fAim, vNormal);
    if (fSlope > 0.0f) {
        if (nKind == 3) {
            fMax = 40.0f * fn_8000AD9C(fSlope) + (75.0f - 1.8f * gClubStep[nClub]);
        } else {
            fMax = 40.0f * fn_8000AD9C(fSlope) + (125.0f - 1.8f * gClubStep[nClub]);
        }
        fMax = 0.017453292f * fMax;
        fSpeed *= (1.0f / fMax) * (fMax - fSlope);
        fn_80055E28(fSlope, &fSinF, &fCosF);
        fn_80055D70(&vDir[2], &vDir[1], fSinF, fCosF);
    }
    fn_8001EF34(vDir, fSpeed, vDir);
    if (nKind == 5) {
        fn_80055E28(0.7330383f, &fSin, &fCos);
        fn_80055D70(&vDir[2], &vDir[1], fSin, fCos);
    }
    if (nKind == 2) {
        fLaunch = gChipLoft[nClub];
    } else {
        fLaunch = gClubLoft[nClub];
    }
    fLaunch += gKindLoft[nKind];
    if (nKind == 4) {
        fLaunch -= 0.013962634f * gClubStep[nClub];
    }
    fLaunch += gTrajLoft[nTrajectory];
    fn_80055E28(Ball_Clamp(fLaunch, 0.0f, 1.3962634f), &fSin, &fCos);
    Vec3Copy(pA, vAxis);
    vAxis[3] = 0.0f;
    fn_80055D70(&vAxis[2], &vAxis[1], fSin, fCos);
    if (pBall->nPlayer == 4 ||
        (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect)) {
        fSide = 0.0f;
    } else {
        fSide = fn_80051124(pBall, fAim, vNormal);
        if (fSide) {
            if (fSide < -0.7853982f) {
                fSide = -0.7853982f;
            } else if (fSide > 0.7853982f) {
                fSide = 0.7853982f;
            }
            fn_80055E28(0.2f * fSide, &fSinS, &fCosS);
            fn_80055D70(&vAxis[0], &vAxis[1], fSinS, fCosS);
        }
    }
    fn_80055EC4(vDir, vAxis, vAlong);
    fn_80055EA0(vDir, vAlong, vOff);
    fn_8001EF34(vOff, 0.10000002f, vOffPart);
    nLie = pBall->nLie;
    switch (nLie) {
    case 6:
        fKeep = 0.8f;
        fSpin = 0.9f;
        break;
    case 7:
        fKeep = 0.7f;
        fSpin = 0.8f;
        break;
    case 8:
        fKeep = 0.6f;
        fSpin = 0.7f;
        break;
    case 3:
        fKeep = 0.9f;
        fSpin = 0.7f;
        break;
    case 4:
        fKeep = 0.8f;
        fSpin = 0.7f;
        break;
    default:
        fKeep = pSurface->f00;
        fSpin = pSurface->f08;
        break;
    }
    fKeep += pBall->f70;
    fKeep = 0.0125f * (1.0f - fKeep) * gClubStep[nClub] + fKeep;
    if (nKind == 2 && (nLie == 3 || nLie == 4 || nLie == 5)) {
        fKeep -= 0.1f;
    }
    fn_80055E7C(vAlong, vOffPart, vAlong);
    fn_8001EF34(vAlong, fKeep, pVel);
    fKeep = gClubSpin[nClub] * gKindSpin[nKind];
    fKeep *= 0.85f;
    fSpin = 1.1904762f * (fKeep * fSpin);
    fn_80055EA0(vOff, vOffPart, vOff);
    if (fSide) {
        fn_80055E28(0.9f * fSide, &fSinS, &fCosS);
        fn_80055D70(&vAxis[0], &vAxis[1], fSinS, fCosS);
    }
    fn_8001EF78(vOff, vAxis, pSpin);
    fn_8001EF34(pSpin, fSpin, pSpin);
    fn_80055D70(&pVel[0], &pVel[2], fSinAim, fCosAim);
    fn_80055D70(&pSpin[0], &pSpin[2], fSinAim, fCosAim);
done:
    return 1;
}

// Launch the ball from a point along a direction at a speed (x 0.489): in the air, no spin.
void fn_80051A18(Ball* pBall, f32* pDir, f32 fSpeed, f32* pFrom) {
    pBall->nState = 2;
    Vec_Copy(pFrom, pBall->vStart);
    Vec_Copy(pFrom, pBall->vPos);
    Vec_Copy(pFrom, pBall->vPrev);
    fn_8001EF34(pDir, 0.48888889f * fSpeed, pBall->vVel);
    pBall->vSpin[0] = 0.0f;
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 0.0f;
    pBall->f3C      = 0.0f;
    pBall->fAC      = 0.0f;
    pBall->fB4      = 0.0f;
    pBall->fB8      = 0.0f;
    pBall->fSpinY   = 0.0f;
    pBall->fSpinX   = 0.0f;
    pBall->n84      = 0;
    pBall->n80      = 0;
    pBall->unk9C    = 0;
    pBall->b99      = 1;
    pBall->b9B      = 1;
    pBall->b9A      = 1;
}

// Strike the ball: fn_800512BC turns club, kind, power, aim, trajectory and the two launch
// blocks into its velocity and spin (failing that it is a hazard). A putt (kind 0, or the
// putter) starts rolling (state 3), anything else is in the air (state 2). Event 10.
void Ball_Launch(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB) {
    Vec_Copy(pBall->vPos, pBall->vStart);
    pBall->fAC    = 0.0f;
    pBall->fB4    = 0.0f;
    pBall->fB8    = 0.0f;
    pBall->fSpinY = 0.0f;
    pBall->fSpinX = 0.0f;
    pBall->n84    = 0;
    pBall->n80    = 0;
    pBall->unk9C  = 0;
    if (!fn_800512BC(pBall, nClub, nKind, fPower, fAim, nTrajectory, pA, pB, pBall->vVel, pBall->vSpin)) {
        fn_80050CAC(pBall, 1);
        return;
    }
    if (nKind == SHOT_PUTT || nClub == CLUB_PUTTER) {
        pBall->b99    = 1;
        pBall->b9B    = 1;
        pBall->b9A    = 1;
        pBall->nState = 3;
    } else {
        pBall->b99    = 0;
        pBall->b9B    = 0;
        pBall->b9A    = 0;
        pBall->nState = 2;
    }
    pBall->f70 = 0.0f;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
    }
    if (pBall->nPlayer >= 0) {
        fn_80067074(pBall->nPlayer, 10, (int)pBall, !gSimulating);
    }
}

// The spin stick's input: each axis must be within -1..1; stored x 15.
void fn_80051C84(Ball* pBall, f32 fX, f32 fY) {
    if (fX < -1.0f || fX > 1.0f || fY < -1.0f || fY > 1.0f) return;
    if (pBall == NULL) return;
    pBall->fSpinX = 15.0f * fX;
    pBall->fSpinY = 15.0f * fY;
}

// The ball has stopped. Holed: state 1, lie 12, events 0x21 and 0x20. Otherwise it is settled
// (fn_8005418C) and given the lie of the surface under it - a surface whose +0x1C is not 0.375
// counts as surface 14 - or, with no surface at all, it is a hazard.
void Ball_Stop(Ball* pBall) {
    SurfaceType* pSurface;
    if (pBall->bHoled) {
        pBall->nState = 1;
        pBall->nLie   = 12;
        pBall->n6C    = 0;
        if (pBall->nPlayer >= 0) {
            fn_80067074(pBall->nPlayer, 0x21, (int)pBall, !gSimulating);
            fn_80067074(pBall->nPlayer, 0x20, (int)pBall, !gSimulating);
        }
        return;
    }
    fn_8005418C(pBall, 1, 0.0f);
    pSurface = fn_8004D838(pBall->pCourse, pBall);
    if (pSurface == NULL) {
        fn_80050CAC(pBall, 1);
        return;
    }
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    pBall->nState = 1;
    Ball_SetLie(pBall, pSurface);
    if (pBall->nPlayer >= 0) {
        fn_80067074(pBall->nPlayer, 0x20, (int)pBall, !gSimulating);
    }
}

// Distance from a point to the pin, 1000 when there is no course.
f32 Ball_DistanceToPin(f32* pPos) {
    CourseInfo* pCourse = fn_8000C594();
    if (pCourse != NULL) {
        return Vec_Distance(pPos, (f32*)&pCourse->pin[Game_CurrentHole()]);
    }
    return 1000.0f;
}

// The shot starts here: remember the surface and the position, and the distance to the pin
// as the closest so far.
void fn_80054A6C(Ball* pBall) {
    pBall->nStartSurface = pBall->nSurface;
    Vec_Copy(pBall->vPos, pBall->vStart);
    pBall->fClosest = Ball_DistanceToPin(pBall->vPos);
}

// The pull toward the cup. Inside 5.5 in of the pin, while the ball is still short of it, a
// ball heading within 30 degrees of the cup (or within 3.5 in whatever its heading) gets
// 0.455 x dt x (pin - ball) added to its velocity - but never on an axis where that would speed
// it up while it is more than 16.8 degrees off line. A ball crossing over the cup fast and
// off line loses up to 67% of its speed instead: the lip.
void Ball_CupPull(Ball* pBall, f32 fDt) {
    f32 vPin[3];
    f32 fDist;

    fDist = Vec_Distance(PIN(pBall), (f32*)pBall);
    if (fDist >= 0.152778f) return;
    Vec_Copy(PIN(pBall), vPin);
    vPin[1] += BALL_RADIUS;
    {
        f32 fStartDist = Vec_Distance(pBall->vStart, vPin);
        f32 fAngle, fK, fPull;
        if (fStartDist < 0.166667f) return;
        if (Vec_Distance(pBall->vStart, (f32*)pBall) > fStartDist - 0.0416667f) return;

        fAngle = fn_8000AD78(pBall->vPos[0] - pBall->vPrev[0], pBall->vPos[2] - pBall->vPrev[2]);
        fAngle = fn_8000AD9C(fAngle - fn_8000AD78(vPin[0] - pBall->vPos[0], vPin[2] - pBall->vPos[2]));
        while (fAngle > 3.14159265f) {
            fAngle -= 3.14159265f;
        }
        if (fDist < 0.0625f) {
            if (fAngle > 0.523599f && pBall->fSpeed > 0.366667f) {
                f32 fSlow = 1.0f - 16.0f * (0.67f * fDist);
                pBall->vVel[0] *= fSlow;
                pBall->vVel[2] *= fSlow;
            }
            return;
        }
        if (fAngle >= 0.523599f && fDist >= 0.0972222f) return;

        fK    = 0.455472f * fDt;
        fPull = fK * (vPin[0] - pBall->vPos[0]);
        if ((pBall->vVel[0] < 0.0f && fPull < 0.0f) || (pBall->vVel[0] > 0.0f && fPull > 0.0f)) {
            if (fn_8000AD9C(fAngle) > 0.293333f) fPull = 0.0f;
        }
        pBall->vVel[0] += fPull;
        fPull = fK * (vPin[2] - pBall->vPos[2]);
        if ((pBall->vVel[2] < 0.0f && fPull < 0.0f) || (pBall->vVel[2] > 0.0f && fPull > 0.0f)) {
            if (fn_8000AD9C(fAngle) > 0.293333f) fPull = 0.0f;
        }
        pBall->vVel[2] += fPull;
    }
}

// Step a ball by nMs milliseconds (at least 20; one tick is 20 ms) unless it is placed, stopped
// or in a hazard. For players 0..3, fn_8004C590 may record the position in one of two tables.
int fn_8005567C(Ball* pBall, int nMs) {
    u8 bA, bB;
    if (nMs < 20) return 0;
    if (pBall->nState == 0 || pBall->nState == 1 || pBall->nState == 5) return 0;
    Ball_Tick(pBall, (f32)nMs / 20.0f);
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        if (fn_8004C798(pBall->nSurface)) {
            fn_8004C590(pBall->pCourse, pBall, 1, &bA, &bB, 0);
        } else {
            fn_8004C590(pBall->pCourse, pBall, 0, &bA, &bB, 0);
        }
        if (bA) Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        if (bB) Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
    }
    return 0;
}

// Run a ball ahead silently: ticks of fTick for fSeconds x 60 ticks' worth, or until it is no
// longer in the air or rolling.
void Ball_SimSeconds(Ball* pBall, f32 fSeconds, f32 fTick) {
    gSimulating = 1;
    fSeconds *= 60.0f;
    do {
        Ball_Tick(pBall, fTick);
        fSeconds -= fTick;
    } while (fSeconds > 0.0f && (pBall->nState == 2 || pBall->nState == 3 || pBall->nState == 4));
    gSimulating = 0;
}

void Ball_SimStep(Ball* pBall, f32 fSeconds, f32 fTick) {
    Ball_SimSeconds(pBall, fSeconds, fTick);
}

// Put the ball on the ground at a point: find the ground under it (from 2 in up), sit the ball
// on it (radius + 0.05 in), placed, with the lie of the surface there and f70 halved.
u8 fn_8005587C(Ball* pBall, f32* pPos) {
    f32          v[4];
    SurfaceType* pSurface;
    f32          fGround;
    Vec3Copy(pPos, v);
    v[1] += 0.0555556f;
    fGround = fn_8004D620(pBall->pCourse, v);
    if (fGround < -60000.0f) {
        fGround = fn_8004D5C0(pBall->pCourse, v);
        if (fGround < -60000.0f) return 0;
    }
    v[1] = 0.0013888889f + (BALL_RADIUS + fGround);
    pSurface = fn_8004D838(pBall->pCourse, (Ball*)v);
    if (pSurface == NULL) return 0;
    Vec3Copy(v, pBall->vPos);
    Vec3Copy(v, pBall->vPrev);
    pBall->nState = 0;
    pBall->bHoled = 0;
    Ball_SetLie(pBall, pSurface);
    pBall->f70 = 0.5f * pBall->f70;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
    }
    return 1;
}

// Drop the ball from a point: at least 0.25 in above the ground, its height remembered, and it
// is in the air (state 2) with no velocity or spin.
u8 fn_800559BC(Ball* pBall, f32* pPos) {
    f32 v[4];
    f32 fGround;
    Vec3Copy(pPos, v);
    fGround = fn_8004D620(pBall->pCourse, v);
    if (fGround < -60000.0f) return 0;
    if (v[1] <= fGround) {
        v[1] = 0.0069444445f + fGround;
    }
    pBall->fHeight = v[1] - fGround;
    v[1] += 0.027055556f;
    Vec3Copy(v, pBall->vPos);
    Vec3Copy(v, pBall->vPrev);
    pBall->nState  = 2;
    pBall->bHoled  = 0;
    pBall->vVel[0] = 0.0f;
    pBall->vVel[1] = 0.0f;
    pBall->vVel[2] = 0.0f;
    pBall->f2C     = 0.0f;
    pBall->vSpin[0] = 0.0f;
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 0.0f;
    pBall->f3C     = 0.0f;
    pBall->fSpeed  = 0.0f;
    pBall->n84     = 0;
    pBall->n80     = 0;
    pBall->unk9C   = 0;
    return 1;
}

// A fresh ball for a player at a point: everything cleared (surface 45), the course looked up,
// and the ball set on the ground there (radius + 1 in; the point itself if there is no
// ground). Returns 1 when there is no course.
u8 fn_80055AA8(Ball* pBall, f32* pPos, int nPlayer) {
    f32 fGround;
    Vec_Copy(pPos, pBall->vPos);
    pBall->nState        = 0;
    pBall->nLie          = 0;
    pBall->n6C           = 0;
    pBall->fHeight       = 0.0f;
    pBall->f70           = 0.0f;
    pBall->nStartSurface = 45;
    pBall->nSurface      = 45;
    pBall->nPlayer       = nPlayer;
    pBall->n84           = 0;
    pBall->n80           = 0;
    pBall->bHoled        = 0;
    pBall->b99           = 0;
    pBall->b9B           = 0;
    pBall->b9A           = 0;
    pBall->fSpeed        = 0.0f;
    pBall->vSpin[0]      = 0.0f;
    pBall->vSpin[1]      = 0.0f;
    pBall->vSpin[2]      = 0.0f;
    pBall->f3C           = 0.0f;
    pBall->vVel[0]       = 0.0f;
    pBall->vVel[1]       = 0.0f;
    pBall->vVel[2]       = 0.0f;
    pBall->f2C           = 0.0f;
    pBall->pCourse = fn_8000C594();
    if (pBall->pCourse == NULL) return 1;
    pBall->vPos[1] += 0.053444445f;
    fGround = fn_8004D620(pBall->pCourse, pBall->vPos);
    if (fGround < -60000.0f) {
        fGround = fn_8004D5C0(pBall->pCourse, pBall->vPos);
        if (fGround < -60000.0f) {
            fGround = pBall->vPos[1];
        }
    }
    pBall->vPos[1] = 0.027777778f + (BALL_RADIUS + fGround);
    Vec_Copy(pPos, pBall->vPrev);
    if (nPlayer >= 0 && nPlayer <= 3) {
        Vec_Copy(pPos, lbl_801D58C8[nPlayer]);
        Vec_Copy(pPos, lbl_801D5888[nPlayer]);
    }
    return 0;
}

void fn_80055C1C(u8 b) {
    lbl_80281DD2 = b;
}

// Course setting (0..4, else 2).
void fn_80055C24(int n) {
    if (n < 0 || n >= 5) n = 2;
    lbl_80281130 = n;
}

// Course setting from options +0x18 (0..2, else 0); courses 6 and 15 take it one lower, not
// below 1.
void fn_80055C40(int n) {
    if (n < 0 || n >= 3) n = 0;
    switch (Game_GetCourse()) {
    case 6:
    case 15:
        if (n > 1) n--;
        break;
    }
    lbl_80281DD4 = n;
}

int fn_80055CA4(void) {
    return lbl_80281DD4;
}

// Course setting (0..2, else 0).
void fn_80055CAC(int n) {
    if (n < 0 || n >= 3) n = 0;
    lbl_80281DD8 = n;
}

int fn_80055CC8(void) {
    return lbl_80281DD8;
}

// Course setting from options +0x1C (0..2, else 1); courses 6 and 15 take it one higher, not
// above 2.
void fn_80055CD0(int n) {
    if (n < 0 || n >= 3) n = 1;
    switch (Game_GetCourse()) {
    case 6:
    case 15:
        if (n < 2) n++;
        break;
    }
    lbl_80281134 = n;
}

int fn_80055D34(void) {
    return lbl_80281134;
}

void fn_80055D3C(void) {
    lbl_80281DD4 = 0;
    lbl_80281DD8 = 0;
    lbl_80281134 = 1;
}

void fn_80055D54(void) {
    lbl_80281130 = 2;
    lbl_80281DE4 = 0;
    lbl_80281DD2 = 0;
}

void fn_80055D6C(void) {
}

// Rotate the pair (*pA, *pB) by the angle whose sine and cosine are given.
void fn_80055D70(f32* pA, f32* pB, f32 fSin, f32 fCos) {
    f32 fA = fn_80055E1C(*pA, *pB, fSin, fCos);
    *pB = fn_80055E10(*pB, *pA, fSin, fCos);
    *pA = fA;
}

f32 fn_80055E10(f32 a, f32 b, f32 fSin, f32 fCos) {
    return a * fCos + b * fSin;
}

f32 fn_80055E1C(f32 a, f32 b, f32 fSin, f32 fCos) {
    return a * fCos - b * fSin;
}

void fn_80055E28(f32 fAngle, f32* pSin, f32* pCos) {
    *pSin = fn_800095F0(fAngle);
    *pCos = fn_80009638(fAngle);
}

// out = b + a (three floats)
asm void fn_80055E7C(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}

// out = a - b (three floats)
asm void fn_80055EA0(register f32* pA, register f32* pB, register f32* pOut) {
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

// out = b scaled by a . b (b's fourth float included)
asm void fn_80055EC4(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 0, 0
    ps_mul   f0, f0, f2
    ps_mul   f1, f1, f3
    ps_sum1  f1, f0, f1, f0
    ps_sum0  f1, f1, f1, f1
    ps_muls0 f2, f2, f1
    ps_muls0 f3, f3, f1
    psq_st   f2, 0(pOut), 0, 0
    psq_st   f3, 8(pOut), 0, 0
    blr
}

// out = -a (three floats)
asm void fn_80055EF8(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}

void fn_80055F14(void) {
}

void fn_80055F18(void) {
}

// The wind: its speed, and (when asked) its vector - the direction's unit vector x speed.
f32 Wind_Get(f32* pOut) {
    if (pOut != NULL) {
        f32 v[4];
        fn_8000AE28(gWindDirs[gWindDir], gWindSpeed, v);
        Vec_Copy(v, pOut);
    }
    return gWindSpeed;
}

int fn_80055F78(void) {
    return gWindDir;
}

f32 fn_80055F80(void) {
    return gWindSpeed;
}

// Set the wind (a speed of 0 or less is stored as 0.1) and tell fn_800348DC.
void Wind_Set(int nDir, f32 fSpeed) {
    gWindDir   = nDir;
    gWindSpeed = fSpeed;
    if (fSpeed <= 0.0f) {
        gWindSpeed = 0.1f;
    }
    fn_800348DC();
}

// The hole's wind: the authored one, none with the wind off, or - when the hole has none - one
// rolled from the wind setting (courses 6 and 15 at least 2, except in mode 4 without a
// challenge): speed 0..6, 2..12, 5..20 or 12..31, direction one of eight.
void Wind_Generate(void) {
    int n      = Hole_WindDir();
    f32 fSpeed = Hole_WindSpeed();
    if (gpGame->bNoWind) {
        fSpeed = 0.0f;
        n      = 0;
    } else if (n == 0 && 0.0f == fSpeed) {
        n = SESSION_OPTIONS->nWind;
        switch (Game_GetCourse()) {
        case 6:
        case 15:
            if (Game_GetMode() != 4 || fn_801021FC() > 0) {
                if (n < 2) {
                    n = 2;
                } else {
                    n = 3;
                }
            }
            break;
        }
        switch (n) {
        case 0:
            fSpeed = Rand_Next(0) % 7;
            break;
        case 1:
            fSpeed = 2.0f + Rand_Next(0) % 11;
            break;
        case 2:
            fSpeed = 5.0f + Rand_Next(0) % 16;
            break;
        case 3:
            fSpeed = 12.0f + Rand_Next(0) % 20;
            break;
        }
        n = Rand_Next(0) % 8;
    }
    Wind_Set(n, fSpeed);
}

// The wind's direction as a unit vector.
void fn_800561CC(f32* pOut) {
    Vec_Copy(gWindDirs[gWindDir], pOut);
}

void fn_80056204(void) {
}

void fn_80056208(void) {
}
