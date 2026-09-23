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
    struct SurfaceType* pHitSurface;   // 0x88  what it last hit
    s32  n8C;                   // 0x8C
    s32  n90;                   // 0x90  what the ball last hit (fn_80054040)
    s32  nPlayer;               // 0x94  -1 when nobody's
    u8   bHoled;                // 0x98
    u8   b99;                   // 0x99
    u8   b9A;                   // 0x9A
    u8   b9B;                   // 0x9B
    u8   unk9C;                 // 0x9C  landed in sand (class 6) this shot
    u8   unk9D[0xA0 - 0x9D];
    f32  vLand[3];              // 0xA0  where it first landed in sand
    f32  fAC;                   // 0xAC  ticks spent with no ground under a rolling ball
    f32  fLandImpact;           // 0xB0  how hard it landed there (sqrt of fn_80052598's result)
    f32  fB4;                   // 0xB4  distance from the start at the last stall check
    f32  fB8;                   // 0xB8  time since the last stall check
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
void   fn_8005418C(Ball* pBall, u8 bSettle, f32 fTicks);
SurfaceType* fn_8004D838(CourseInfo* pCourse, Ball* pBall);   // the surface under a point
f32    fn_8004D620(CourseInfo* pCourse, f32* pPos);   // ground height, -60000 and below if none
f32    fn_8004D5C0(CourseInfo* pCourse, f32* pPos);   // the same from another source
u8     fn_8004C798(s32 nSurface);
void   fn_8004C590(CourseInfo* pCourse, Ball* pBall, int a, u8* pA, u8* pB, int b);
// One club's distances for a shot kind: power 0.1, 0.2 .. 1.1 (fDist[9], full power, is "the
// reach" AI_PowerScale divides by).
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
extern f32 gTurfSpeedMul[5];                     // 0x80183488  by gTurfSpeed: 0.6 .. 1.4 (classes 2, 3, 4)
extern f32 gGreenSpeedMul[3];                    // 0x8018349C  by options +0x18: 1.0 1.1 1.2 (class 3)
extern f32 gFairwaySpeedMul[3];                  // 0x801834A8  by gFairwaySetting: 1.0 1.1 1.2 (class 2)
extern f32 gRoughMul[3];                         // 0x801834B4  by options +0x1C: 1.3 1.0 0.7 (class 5)
void   fn_800A3980(Ball* pBall, int nPlayer);    // rolling sound / effect
void   Ball_CupPull(Ball* pBall, f32 fDt);
void   fn_800BAF04(f32* pSrc, f32* pDst);        // normalise
f32    fn_8000C5FC(f32* pA, f32* pB);            // dot product
void   fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // a + f x b
void   fn_8001EF78(f32* pA, f32* pB, f32* pOut); // cross product
void   fn_80055E7C(f32* pA, f32* pB, f32* pOut);
void   fn_80055EA0(f32* pA, f32* pB, f32* pOut);
void   fn_80055EC4(f32* pA, f32* pB, f32* pOut);
void   fn_80055EF8(f32* pA, f32* pOut);
f32    fn_80051124(Ball* pBall, f32 fAim, f32* pNormal);
f32    fn_800511F0(Ball* pBall, f32 fAim, f32* pNormal);
int    Game_GetCourse(void);                     // 0x80008830
int    Game_CurrentHole(void);                   // Golfer.c
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

f32    fn_80009744(f32* pVec);                   // dot with itself
double fn_80009680(double x);                    // sqrt
void   fn_8004D9E0(CourseInfo* pCourse, Ball* pBall, f32* pHeight, SurfaceType** ppSurface, f32* pNormal,
                   f32* pHeight2, SurfaceType** ppSurface2, f32* pNormal2);
void   fn_8004D9A8(CourseInfo* pCourse, Ball* pBall, f32* pHeight, f32* pHeight2);
int    fn_80050BEC(SurfaceType* pSurface);       // a surface's index
u8     fn_800E2B40(int nPlayer, Ball* pBall);
u8     fn_800B1B18(int nPlayer, f32* pTo, f32* pFrom, f32* pHit, f32* pNormal, s32* pWhat);
u8     fn_80053E98(Ball* pBall, void* pObj, f32* pHit, f32* pNormal);
u8     fn_800539F8(Ball* pBall, f32* pHit, f32* pNormal, SurfaceType* pSurface, s32 nWhat, f32* pFrac, f32 fTicks);
u8     fn_80053240(Ball* pBall, f32 fTicks);
f32    fn_80052598(Ball* pBall, f32* pNormal, SurfaceType* pSurface);   // the bounce; returns the impact
f32    fn_800BB028(f32* pA, f32* pB);            // squared distance
u8     fn_8004FF34(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface, s32* pWhat);
u8     fn_8004E1B0(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface, s32* pWhat);
u8     fn_8004E558(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface, s32* pWhat, u8* pOut);
u8     fn_8004EE20(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface, s32* pWhat);
void   fn_800B1AB0(u8* pObj, f32* pPos, f32* pRadius);   // the flagstick's position and radius
int    Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode);
u8     Ball_Collide(Ball* pBall, f32 fTicks);
void   Ball_GroundContact(Ball* pBall, f32 fTicks);
void   fn_80052268(Ball* pBall, f32 fTicks);
void   Ball_FlightStep(Ball* pBall, f32 fTicks);
f32    fn_80055324(Ball* pBall);
u8     fn_80054040(Ball* pBall, f32 fTicks);
f32    Wind_Get(f32* pOut);

extern u8  gSimulating;                          // 0x80281DD0  a rehearsal: no sounds or effects
extern u8  gSimFullCup;                          // a sim that still gets the cup pull and the near-cup gravity (state 15, look-ahead)
extern u8  lbl_80281DD2;
extern s32 gGreenSpeedSetting;                   // options +0x18 (GREEN SPEED?), 0..2: green friction x 1.0 / 0.9 / 0.8
extern s32 gFairwaySetting;                      // 0..2: class-2 friction x 1.0 / 0.9 / 0.8
extern s32 gTurfSpeed;                           // 0..4, default 2; rain sets 1 (light) or 0 (heavy): putt table and friction
extern s32 gRoughSetting;                        // options +0x1C (ROUGH LENGTH?), 0..2: class-5 friction x 0.7 / 1.0 / 1.3
extern u8  lbl_80281DE4;                          // the two ground heights below are current
extern f32 lbl_80281DE0;                         // ground height under the ball
extern f32 lbl_80281DDC;                         // the other ground height (fn_8004D9E0)
extern f32 gWindSpeed;                           // 0x80281DE8
extern s32 gWindDir;                             // 0x80281DEC  0..7
extern f32 gWindDirs[8][4];                      // 0x80187EF8  unit vectors, 45 degrees apart
extern f32 lbl_801D5888[4][4];                   // per player: where the ball was last on ...
extern f32 lbl_801D58C8[4][4];                   // ... two kinds of surface (fn_8004C590)

static inline u8 Ball_NoGround(f32 fHeight) {
    return fHeight < -60000.0f;
}

static inline f32 Ball_Clamp(f32 x, f32 fLo, f32 fHi) {
    if (x < fLo) return fLo;
    if (x > fHi) return fHi;
    return x;
}

#define PIN(pBall) ((f32*)&(pBall)->pCourse->pin[Game_CurrentHole()])

// A landing: a player's ball fires 0x25 on flagged surfaces, then 0x26 on the cup (surface 90),
// 0x24 on an object, 0x23 on the ground; a look-ahead ball's first landing (Player +0xB5C) tells
// its player (0x49).
#define BALL_LANDING_EVENTS(pBall, pSurface, nWhat)                                                \
    if ((pBall)->nPlayer >= 0) {                                                                   \
        if ((pSurface)->u34 & 0x10) {                                                              \
            fn_80067074((pBall)->nPlayer, 0x25, (int)(pBall), !gSimulating);                       \
        }                                                                                          \
        if (fn_80050BEC(pSurface) == 90) {                                                         \
            fn_80067074((pBall)->nPlayer, 0x26, (int)(pBall), !gSimulating);                       \
        } else if (nWhat) {                                                                        \
            fn_80067074((pBall)->nPlayer, 0x24, (int)(pBall), !gSimulating);                       \
        } else {                                                                                   \
            fn_80067074((pBall)->nPlayer, 0x23, (int)(pBall), !gSimulating);                       \
        }                                                                                          \
    } else if ((pBall)->n80 == 0) {                                                                \
        int     nOwner;                                                                            \
        Player* pP = gPlayers;                                                                     \
        nOwner = 0;                                                                                \
        if ((pBall) != (Ball*)pP[0].ballBefore) {                                                  \
            nOwner = 1;                                                                            \
            if ((pBall) != (Ball*)pP[1].ballBefore) {                                              \
                nOwner = 2;                                                                        \
                if ((pBall) != (Ball*)pP[2].ballBefore) {                                          \
                    nOwner = 3;                                                                    \
                    if ((pBall) != (Ball*)pP[3].ballBefore) {                                      \
                        nOwner = 4;                                                                \
                        if ((pBall) != (Ball*)pP[4].ballBefore) nOwner = 5;                        \
                    }                                                                              \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
        fn_80067074(nOwner, 0x49, (int)(pBall), 1);                                                \
    }


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
    gSimFullCup = b;
}

// Putt power for a distance: the putt table (22 steps of 0.05 power, distance ~ 43.4 x power
// squared on a medium green) scaled by the green-speed setting, interpolated; 1.1 beyond it.
f32 fn_80050D34(f32 fDist) {
    int i;
    f32 fScale = gPuttSpeedScale[gTurfSpeed];
    f32 fPower;
    for (i = 1; i < 23; i++) {
        f32 fHi = fScale * gPuttDist[i];
        if (fDist <= fHi) {
            f32 fLo = fScale * gPuttDist[i - 1];
            f32 fT = (fDist - fLo) / (fHi - fLo);
            fPower = 0.05f * (i - 1);
            fPower = 0.05f * fT + fPower;
            return fPower;
        }
    }
    fPower = 1.1f;
    return fPower;
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

// Power for a distance with a club: the row's 11 distances are power 0.1 to 1.1, interpolated,
// plus the difference between the table's surface and the one under the ball (a surface that
// is not a stopping surface counts as 14); 1.1 beyond the row.
// A row's distance at column i (an accessor in the original: reading the array directly gives
// different registers).
static inline f32 ClubRow_Dist(ClubRow* pRow, int i) {
    return pRow->fDist[i];
}

f32 fn_80050F88(f32 fDist, u8* p, int nKind, int nClub) {
    s32          nSurface;
    SurfaceType* pSurface;
    ClubRow*     pRow;
    f32          vNormal[4];
    f32          fBase, fAdj, fFrac, fPower;
    int          i;
    if (p == NULL) return 0.0f;
    if (!fn_80050DE4(nKind, nClub, 0, &pRow, &nSurface)) return 1.0f;
    fBase = gSurfaceTypes[nSurface].f00;
    if (fn_8004D890(((Ball*)p)->pCourse, (Ball*)p, &pSurface, vNormal) < -60000.0f || 0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    fAdj = fBase - pSurface->f00;
    for (i = 1; i < 12; i++) {
        if (fDist <= ClubRow_Dist(pRow, i)) {
            fFrac = (fDist - ClubRow_Dist(pRow, i - 1)) / (ClubRow_Dist(pRow, i) - ClubRow_Dist(pRow, i - 1));
            fPower = 0.1f * i;
            fPower = 0.1f * fFrac + fPower;
            fPower += fAdj;
            return fPower;
        }
    }
    fPower = 1.1f;
    return fPower;
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

// One tick in the air. The wind (a CPU's clamped to +-15 on each axis) is weaker near the
// ground: x (0.25 + 0.75 x height / 25 ft) below 25 ft. Air speed is the velocity less 0.19 x
// the wind. Drag and lift are quadratic in air speed with coefficients that depend on speed and
// spin; lift is along spin x air velocity. Gravity 0.10717 - and, for a real ball (or a sim
// with gSimFullCup set), three times that within 2.25 in of the top of the cup. Event 0x1C
// at the top of the flight. Spin decays 0.3% a tick, faster flying into the wind.
void Ball_FlightStep(Ball* pBall, f32 fTicks) {
    f32 vWind[4];
    f32 vRel[4];
    f32 vDrag[4];
    f32 vLift[4];
    f32 vAccel[4];
    f32 vPin[4];
    f32 fDrag, fSpeed2, fSpin, fSpeed, fLift, fLen, fInto, fExtra, fHeight, fK, fX, fZ;

    Wind_Get(vWind);
    if (Player_IsCPU(pBall->nPlayer)) {
        vWind[0] = Ball_Clamp(vWind[0], -15.0f, 15.0f);
        vWind[1] = Ball_Clamp(vWind[1], -15.0f, 15.0f);
        vWind[2] = Ball_Clamp(vWind[2], -15.0f, 15.0f);
    }
    if (pBall->fHeight < 8.333333f) {
        fHeight = pBall->fHeight;
        if (fHeight < 0.0f) fHeight = 0.0f;
        fn_8001EF34(vWind, 0.75f * (fHeight / 8.333333f) + 0.25f, vWind);
    }
    fn_8000C5D4(pBall->vVel, vWind, -0.190666676f, vRel);
    fSpeed2 = fn_80009744(vRel);
    fSpeed  = fn_80009680(fSpeed2);
    fSpin   = fn_80009680(fn_80009744(pBall->vSpin));
    if (fSpeed != 0.0f) {
        fDrag = 0.000780952396f * fSpeed;
        fDrag = -(fSpeed2 * (0.000474568689f * (0.225790471f + (fSpin * (-0.000348685688f * fSpeed + 0.0168940704f) + fDrag))) / fSpeed);
    } else {
        fDrag = 0.0f;
    }
    fn_8001EF34(vRel, fDrag, vDrag);
    fLift = -0.000201047602f * fSpeed;
    fLift = fSpeed2 * (0.000474568689f * (0.0847342834f + (fSpin * (-0.000628289126f * fSpeed + 0.0407094695f) + fLift)));
    fn_8001EF78(pBall->vSpin, vRel, vLift);
    fLen = fn_80009680(fn_80009744(vLift));
    fn_8001EF34(vLift, fLen != 0.0f ? fLift / fLen : 0.0f, vAccel);
    fn_80055E7C(vDrag, vAccel, vAccel);
    vAccel[1] -= 0.107170001f;
    if (!gSimulating || gSimFullCup) {
        Vec_Copy(PIN(pBall), vPin);
        vPin[1] += BALL_RADIUS;
        if (Vec_Distance(vPin, pBall->vPos) < 0.0625f) {
            vAccel[1] -= 0.214340001f;
        }
    }
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    if (!pBall->b9A && pBall->nPlayer >= 0 && pBall->vVel[1] < 0.0f && pBall->vVel[1] - fTicks * vAccel[1] >= 0.0f) {
        pBall->b9A = 1;
        fn_80067074(pBall->nPlayer, 0x1C, (int)pBall, !gSimulating);
    }
    fInto  = vWind[0] * pBall->vVel[0] + vWind[2] * pBall->vVel[2];
    if (fInto < 0.0f) {
        fK = fTicks * fInto / fn_80009744(pBall->vVel);
        fX = fK * pBall->vVel[0];
        fZ = fK * pBall->vVel[2];
        fExtra = 0.3f * (f32)fn_80009680(fX * fX + fZ * fZ);
    } else {
        fExtra = 0.0f;
    }
    fn_8001EF34(pBall->vSpin, 1.0f - fTicks * (0.003f * (1.0f + fExtra)), pBall->vSpin);
}

// The ground under a rolling ball: its surface and normal. No ground: if there is none under
// the other source either (or it is more than 1 in above the ball), the ball may coast on for
// up to two ticks on surface 109 (or its own) as if on flat ground - unless fn_800E2B40 says
// otherwise - and is a hazard after that. Returns 0 for a hazard.
u8 fn_80052088(Ball* pBall, SurfaceType** ppSurface, f32* pNormal) {
    SurfaceType* pSurface;
    SurfaceType* pSurface2;
    f32          vNormal[4];
    f32          vNormal2[4];
    f32          fDrop;
    int          nPlayer;
    fn_8004D9E0(pBall->pCourse, pBall, &lbl_80281DE0, &pSurface, vNormal, &lbl_80281DDC, &pSurface2, vNormal2);
    lbl_80281DE4 = 1;
    if (lbl_80281DE0 < -60000.0f) {
        if (!Ball_NoGround(lbl_80281DDC)) {
            fDrop = lbl_80281DDC - pBall->vPos[1];
        }
        if (lbl_80281DDC < -60000.0f || fDrop > 0.028f) {
            nPlayer = pBall->nPlayer;
            if (nPlayer < 0) nPlayer = 4;
            if (lbl_80281DDC < -60000.0f && pBall->fAC < 2.0f && !fn_800E2B40(nPlayer, pBall)) {
                if (pBall->nSurface < 0 || pBall->nSurface < 156) {
                    pBall->nSurface = 109;
                }
                pSurface = &gSurfaceTypes[pBall->nSurface];
                lbl_80281DE0 = pBall->vPrev[1] - BALL_RADIUS - 0.0013888889f;
                pBall->fAC += 1.0f;
                vNormal[0] = 0.0f;
                vNormal[1] = 1.0f;
                vNormal[2] = 0.0f;
                vNormal[3] = 0.0f;
                goto check;
            }
            fn_80050CAC(pBall, 1);
            return 0;
        }
        pSurface = pSurface2;
        Vec3Copy(vNormal2, vNormal);
    }
    pBall->fAC = 0.0f;
check:
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    *ppSurface = pSurface;
    Vec3Copy(vNormal, pNormal);
    return 1;
}

// State 3, skidding: a ball on the ground whose spin has not caught up with its speed. Gravity
// along the ground plane (x (1 - surface +0x14) and the course settings) accelerates it; it is
// kept on the plane at its speed; friction (1.5 x surface +0x18 x the normal force, same
// settings) builds roll spin about dir x normal. Once 0.84 x the spin reaches the speed it
// is rolling (state 4).
void fn_80052268(Ball* pBall, f32 fTicks) {
    f32          vNormal[4];
    f32          vAccel[4];
    f32          vTmp[4];
    f32          vDir[4];
    f32          vSpinAdd[4];
    SurfaceType* pSurface;
    f32          fPull, fDot, fFric;
    if (!fn_80052088(pBall, &pSurface, vNormal)) return;
    pBall->nSurface = fn_80050BEC(pSurface);
    fn_800BAF04(vNormal, vNormal);
    fPull = 1.0f - pSurface->f14;
    if (pSurface->nClass == 3) fPull *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    if (pSurface->nClass == 2) fPull *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    if (pSurface->nClass == 5) fPull *= 2.0f - gRoughMul[gRoughSetting];
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fPull *= 2.0f - gTurfSpeedMul[gTurfSpeed];
    }
    fDot = -0.107170001f * vNormal[1];
    vAccel[0] = -(fPull * (vNormal[0] * fDot));
    vAccel[1] = fPull * (-0.107170001f - vNormal[1] * fDot);
    vAccel[2] = -(fPull * (vNormal[2] * fDot));
    fn_8000C5D4(pBall->vVel, vNormal, -fn_8000C5FC(pBall->vVel, vNormal), vTmp);
    fn_800BAF04(vTmp, vDir);
    fn_8001EF34(vDir, fn_80009680(fn_80009744(pBall->vVel)), pBall->vVel);
    fFric = 1.5f * (pSurface->f18 * (-0.107170001f * vNormal[1]));
    if (pSurface->nClass == 3) fFric *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    if (pSurface->nClass == 2) fFric *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    if (pSurface->nClass == 5) fFric *= 2.0f - gRoughMul[gRoughSetting];
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fFric *= 2.0f - gTurfSpeedMul[gTurfSpeed];
    }
    fFric = 2.97619057f * fFric;
    fn_8001EF78(vDir, vNormal, vSpinAdd);
    fn_8001EF34(vSpinAdd, fFric, vSpinAdd);
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    fn_8000C5D4(pBall->vSpin, vSpinAdd, fTicks, pBall->vSpin);
    fTicks = fn_80009680(fn_80009744(pBall->vVel));
    if (0.84f * (f32)fn_80009680(fn_80009744(pBall->vSpin)) >= fTicks) {
        pBall->nState = 4;
    }
    if (pBall->nPlayer >= 0) {
        fn_800A3980(pBall, pBall->nPlayer);
    }
}

// The bounce. Returns the square of the speed into the surface (Ball_Collide keeps its root as
// the sand-landing impact).
// - Water (class 7, 16) quarters the spin; elsewhere sideways spin is capped (2.93, or the
//   ball's ground speed once that is 5.28 or more).
// - The normal is bent toward the incoming ball by how soft the ground is: speed into it /
//   surface +0x24 (x the course settings; the green-speed setting and rain make it firmer or
//   softer) + surface +0x28 (x 0.667 in rough, x 0.5 for class 11); past a full bend the ball
//   comes straight back.
// - In the frame of that normal: restitution is surface +0x0C (0.5..1 counts as 0.5); on
//   surfaces with a negative +0x0C (branches, leaves) a real ball's value is randomised by
//   +-0.75 x (1 + it), pushed down by 0.005 x LUCK - the "kinder" deflection - and course 9
//   halves it first. Fast balls on short grass bounce less (x 1 - 1.2 x (speed - 8.8) / +0x24),
//   short grass and rough lose a bit more when slow, and rain deadens the bounce.
// - Friction at the contact point (surface +0x10 x 0.3, x rain) takes sliding speed and turns
//   it into spin; a soft surface scales spin by (1 + restitution) instead. Rough x 0.8 and class
//   11 x 0.6 on everything.
// - A ball that has stopped bouncing (rising under 0.67, ground speed under 1.91) starts to
//   roll; on its first five bounces, on dry short grass more than 63 yd from where it was
//   hit, backspin makes it check or spin back (x 13.3 x friction; a tenth of that if the spin
//   stick was used or the spin is not backspin).
// - Short grass with a soft landing speed (+0x24 of 80 or less) loses up to 35% of its rise.
f32 fn_80052598(Ball* pBall, f32* pNormal, SurfaceType* pSurface) {
    f32 vBent[4];
    f32 vDown[4];
    f32 vCon[4];
    f32 vSlide[4];
    f32 vDir[4];
    f32 vSlip[4];
    f32 fA;
    f32 fSpeed;
    f32 fImpact;
    f32 fRest;
    f32 fCo;
    f32 fC;
    f32 fD;
    f32 fB;
    f32 fSi;
    f32 fT, fS, fLen, fBounce, fGrip, fBite, fScale;
    u8  bFlip;

    pBall->n80++;
    if (pSurface->nClass == 7 || pSurface->nClass == 16) {
        fn_8001EF34(pBall->vSpin, 0.25f, pBall->vSpin);
    } else {
        fT = 0.84f * pBall->vSpin[0];
        fS = 0.84f * pBall->vSpin[2];
        fT = fT * fT;
        fS = fS * fS;
        fSi = fn_80009680(fT + fS);
        fT = pBall->vVel[0] * pBall->vVel[0];
        fS = pBall->vVel[2] * pBall->vVel[2];
        fLen = fn_80009680(fT + fS);
        if (fLen < 5.28000021f) {
            if (fSi > 2.9333334f) {
                fn_8001EF34(pBall->vSpin, 2.9333334f / fSi, pBall->vSpin);
            }
        } else if (fSi > fLen) {
            fn_8001EF34(pBall->vSpin, fLen / fSi, pBall->vSpin);
        }
    }
    bFlip = 0;
    if (pNormal[1] < 0.0f) {
        fn_80055EF8(pNormal, pNormal);
        fn_80055EF8(pBall->vVel, pBall->vVel);
        fn_80055EF8(pBall->vSpin, pBall->vSpin);
        bFlip = 1;
    }
    fSpeed = fn_80009680(fn_80009744(pBall->vVel));
    fD     = fn_8000C5FC(pBall->vVel, pNormal);
    fT = pNormal[0] * fD;
    fS = pNormal[1] * fD;
    fLen = pNormal[2] * fD;
    fT = fT * fT;
    fS = fS * fS;
    fLen = fLen * fLen;
    fImpact = fLen + (fT + fS);
    fn_8000C5D4(pNormal, pBall->vVel, 1.0f / fSpeed, vBent);
    fn_80055EF8(vBent, vBent);
    fA = fn_8000AD9C(fD) / pSurface->f24;
    if (pSurface->nClass == 3) fA *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    if (pSurface->nClass == 2) fA *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    if (pSurface->nClass == 5) fA *= 2.0f - gRoughMul[gRoughSetting];
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fA *= 2.0f - (1.4f * (gTurfSpeedMul[gTurfSpeed] - 1.0f) + 1.0f);
    }
    fT = pSurface->f28;
    if (pSurface->nClass == 5) fT *= 0.667f;
    if (pSurface->nClass == 11) fT *= 0.5f;
    fA += fT;
    fLen = fn_80009680(fn_80009744(vBent));
    if (fLen < fA) {
        fn_80055EF8(pBall->vVel, pNormal);
    } else if (fLen != 0.0f) {
        fn_8000C5D4(pNormal, vBent, fA / fLen, pNormal);
    }
    fn_800BAF04(pNormal, pNormal);
    fn_8001EF34(pNormal, -0.839999974f, vDown);
    fn_8001EF78(pBall->vSpin, vDown, vCon);
    fn_80055E7C(vCon, pBall->vVel, vCon);
    fn_8000C5D4(vCon, pNormal, -fn_8000C5FC(vCon, pNormal), vSlide);
    if (fn_80009744(vSlide) != 0.0f) {
        fn_800BAF04(vSlide, vDir);
    } else {
        Vec3Copy(pNormal, vDir);
    }
    fA  = fn_8000AD78(pNormal[0], pNormal[1]);
    fB  = fn_800095F0(fA);
    fCo = fn_80009638(fA);
    vBent[0] = fn_80055E1C(pNormal[0], pNormal[1], fB, fCo);
    vBent[1] = fn_80055E10(pNormal[1], pNormal[0], fB, fCo);
    vBent[2] = pNormal[2];
    fD    = fn_80055E1C(vDir[0], vDir[1], fB, fCo);
    fC    = fn_80055E10(vDir[1], vDir[0], fB, fCo);
    fRest = vDir[2];
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fB, fCo);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[1], fB, fCo);
    fB  = -fn_8000AD78(vBent[2], vBent[1]);
    fCo = fn_800095F0(fB);
    fSi = fn_80009638(fB);
    fT  = fn_80055E1C(fC, fRest, fCo, fSi);
    fC  = fn_80055E10(fRest, fT, fCo, fSi);
    fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fCo, fSi);
    fn_80055D70(&pBall->vSpin[1], &pBall->vSpin[2], fCo, fSi);
    fC  = fn_8000AD78(fD, fC);
    fSi = fn_800095F0(fC);
    fCo = fn_80009638(fC);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fSi, fCo);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[2], fSi, fCo);
    vSlip[0] = 0.462857157f * (0.839999974f * pBall->vSpin[2] + pBall->vVel[0]);
    fRest = pSurface->f0C;
    if (fRest > 0.5f && fRest < 1.0f) {
        fRest = 0.5f;
    }
    if (fRest < 0.0f) {
        if (!(gSimulating || lbl_80281DD2 || pBall->nPlayer == 4 ||
              (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect))) {
            fD = 1.0f - 2.0f * Rand_Float(0);
            fD -= 0.005f * (s8)Golfer_GetAttribute(&gPlayers[pBall->nPlayer], ATTR_LUCK, ATTR_TOTAL);
            if (fD < -1.0f) fD = -1.0f;
            if (Game_GetCourse() == 9) {
                fRest -= 0.5f * (1.0f + fRest);
            }
            fRest = (0.75f * (1.0f + fRest)) * fD + fRest;
        }
    } else {
        pBall->n84++;
    }
    if (fSpeed > 8.80000019f && fRest >= 0.0f) {
        fT = 1.0f - 1.20000005f * ((fSpeed - 8.80000019f) / pSurface->f24);
    } else {
        fT = 1.0f;
    }
    fBounce = 0.0350000001f + (fRest * fT + 1.0f);
    if (fRest >= 0.0f) {
        if (pSurface->nClass == 4 || pSurface->nClass == 2 || pSurface->nClass == 3 || pSurface->nClass == 5) {
            if (pSurface->f24 <= 80.0f && fSpeed < pSurface->f24) {
                fBounce -= ((0.150000006f * fRest) * (pSurface->f24 - fSpeed)) / pSurface->f24;
            }
            if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
                fBounce *= 0.109999999f * (gTurfSpeedMul[gTurfSpeed] - 1.0f) + 1.0f;
            }
        }
    } else if (pBall->fSpeed < 2.44444442f) {
        fBounce = 0.0f;
    }
    vSlip[1] = 1.62f * (fBounce * pBall->vVel[1]);
    vSlip[2] = 0.462857157f * (pBall->vVel[2] - 0.839999974f * pBall->vSpin[0]);
    fT = vSlip[0] * vSlip[0];
    fS = vSlip[2] * vSlip[2];
    fD = fn_80009680(fT + fS);
    fGrip = pSurface->f10 * fn_8000AD9C(vSlip[1]) * 0.3f;
    if (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) {
        fGrip *= gTurfSpeedMul[gTurfSpeed];
    }
    if (fGrip < fD && fD != 0.0f) {
        fGrip = pSurface->f10 * 0.3f;
        if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
            fGrip *= gTurfSpeedMul[gTurfSpeed];
        }
        fScale = fn_8000AD9C(fGrip * vSlip[1] / fD);
        vSlip[0] *= fScale;
        vSlip[2] *= fScale;
    }
    fn_8000C5D4(pBall->vVel, vSlip, -0.61728394f, pBall->vVel);
    if (fRest >= 0.0f) {
        pBall->vSpin[0] = 1.83715463f * vSlip[2] + pBall->vSpin[0];
        pBall->vSpin[2] = 1.83715463f * vSlip[0] + pBall->vSpin[2];
    } else {
        fn_8001EF34(pBall->vSpin, 1.0f + fRest, pBall->vSpin);
    }
    if (pSurface->nClass == 5 || pSurface->nClass == 11) {
        if (pSurface->nClass == 5) {
            fSi = 0.8f;
        } else {
            fSi = 0.6f;
        }
        fn_8001EF34(pBall->vVel, fSi, pBall->vVel);
        fn_8001EF34(pBall->vSpin, fSi, pBall->vSpin);
    }
    if (pSurface != NULL && pSurface->f0C >= 0.0f && pBall->vVel[1] < 0.674666584f &&
        (f32)fn_80009680(pBall->vVel[0] * pBall->vVel[0] + pBall->vVel[2] * pBall->vVel[2]) < 1.90666652f) {
        if (pBall->n84 < 6 && (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) &&
            gTurfSpeedMul[gTurfSpeed] > 0.7f &&
            (f32)fn_80009680((pBall->vPos[0] - pBall->vStart[0]) * (pBall->vPos[0] - pBall->vStart[0]) +
                             (pBall->vPos[2] - pBall->vStart[2]) * (pBall->vPos[2] - pBall->vStart[2])) > 63.0f) {
            fBite = 13.333333f * (0.3f * pSurface->f10);
            if (pBall->fSpinX != 0.0f || pBall->fSpinY != 0.0f) {
                fBite *= 0.1f;
            } else if (-0.839999974f * (pBall->vVel[0] * pBall->vSpin[2]) + -0.839999974f * (pBall->vVel[2] * -pBall->vSpin[0]) >= 0.0f) {
                fBite *= 0.1f;
            }
            pBall->vVel[0] = pBall->vVel[0] - fBite * (-0.839999974f * -pBall->vSpin[2]);
            pBall->vVel[2] = pBall->vVel[2] - fBite * (-0.839999974f * pBall->vSpin[0]);
        }
        if (pBall->nState != 4) {
            pBall->nState   = 4;
            pBall->nSurface = fn_80050BEC(pSurface);
        }
    }
    fD  = -fC;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fC, fD);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[2], fC, fD);
    fD  = -fB;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fC, fD);
    fn_80055D70(&pBall->vSpin[1], &pBall->vSpin[2], fC, fD);
    fD  = -fA;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fC, fD);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[1], fC, fD);
    if (pSurface != NULL && pSurface->f0C >= 0.0f &&
        (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) &&
        pSurface->f24 <= 80.0f && fSpeed < pSurface->f24) {
        pBall->vVel[1] *= 1.0f - 0.349999994f * ((pSurface->f24 - fSpeed) / pSurface->f24);
    }
    if (bFlip) {
        fn_80055EF8(pNormal, pNormal);
        fn_80055EF8(pBall->vVel, pBall->vVel);
        fn_80055EF8(pBall->vSpin, pBall->vSpin);
    }
    return fImpact;
}

// A ball on the ground: did it run into anything between last tick and this one? Out of
// bounds (600 yd from the start) is a hazard. Within 10 ft of the pin the detailed test
// (fn_8004E1B0) runs, elsewhere fn_8004FF34. On a hit: the landing events, the bounce, and a
// nudge along the velocity. Returns 1 on a hit.
u8 fn_80053240(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    u32          nWhat;
    SurfaceType* pSurface;
    if (pBall->pCourse == NULL) return 0;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (fn_800BB028(vTo, pBall->vStart) > 360000.0f) {
        fn_80050CAC(pBall, 1);
        return 0;
    }
    if (pBall->nSurface < 0 || pBall->nSurface >= 156) return 0;
    pSurface = &gSurfaceTypes[pBall->nSurface];
    if (fn_800BB028(PIN(pBall), pBall->vPos) > 11.1111107f) {
        if (!fn_8004FF34(pBall->pCourse, vFrom, vTo, vHit, vNormal, &pSurface, (s32*)&nWhat)) return 0;
    } else {
        if (!fn_8004E1B0(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface, (s32*)&nWhat)) return 0;
    }
    if (pSurface == NULL) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    fn_800BAF04(vNormal, vNormal);
    if (pSurface->f0C >= 0.0f) {
        Vec3Copy(vHit, pBall->vPos);
        pBall->vPos[1] += 0.027055556f;
    }
    pBall->pHitSurface = pSurface;
    pBall->n8C = nWhat;
    BALL_LANDING_EVENTS(pBall, pSurface, nWhat);
    fn_80052598(pBall, vNormal, pSurface);
    if (pSurface->f0C >= 0.0f) {
        Vec3Copy(pBall->vPos, pBall->vPrev);
        fn_8000C5D4(pBall->vPos, pBall->vVel, 9.25925906e-06f, pBall->vPos);
    }
    return 1;
}

// The lie from the surface class, with LUCK (0..110, players 0..3) in the rolls; a sim or
// lbl_80281DD2 rolls 0 (the kind result):
//   1, 2: lie 1.  3, 18: lie 9 (green).  4: lie 10.  7, 16: lie 13.  8: lie 11.
//   5 (rough): surface 145 is lie 4; else a coin flip, forced good when (r & 127) < LUCK/2 -
//     good = lie 3 / surface 27 (never on course 6), bad = lie 4 / surface 28.
//   11: (r & 127) < LUCK/4 + 16 gives the rough treatment instead of lie 5 / surface 26.
//   6, 20 (sand): lie 6 (clean, surface 35), 7 (surface 36) or 8 (plugged, surface 34),
//     from where and how hard it landed and three LUCK rolls; class 20 is surface 146.
//   12: holed (event 0x21 the first time).  Anything else: lie 17.
// Then a random lie quality (+0x70), +-(Rand_Float - LUCK/200, not below 0) x surface +0x04.
void Ball_SetLie(Ball* pBall, SurfaceType* pSurface) {
    SurfaceType* pLie;
    u32          uLuck;
    u32          r;
    int          nLuck;
    f32          f;
    pBall->nSurface = fn_80050BEC(pSurface);
    uLuck = 0;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        nLuck = (s8)Golfer_GetAttribute(&gPlayers[pBall->nPlayer], ATTR_LUCK, ATTR_TOTAL);
        if (nLuck < 0) {
            uLuck = 0;
        } else {
            uLuck = nLuck > 110 ? 110 : nLuck;
        }
    }
    switch (pSurface->nClass) {
    case 1:
    case 2:
        pBall->nLie = 1;
        break;
    case 3:
    case 18:
        pBall->nLie = 9;
        break;
    case 4:
        pBall->nLie = 10;
        break;
    case 5:
        if (pBall->nSurface == 145) {
            pBall->nLie = 4;
            break;
        }
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
    rough:
        if ((r & 127) < uLuck / 2) r = 0;
        if (!(r & 1) && gpGame != NULL && Game_GetCourse() != 6) {
            pBall->nLie     = 3;
            pBall->nSurface = 27;
        } else {
            pBall->nLie     = 4;
            pBall->nSurface = 28;
        }
        break;
    case 11:
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        if ((r & 127) < uLuck / 4 + 16) {
            r >>= 8;
            goto rough;
        }
        pBall->nLie     = 5;
        pBall->nSurface = 26;
        break;
    case 6:
    case 20:
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        if (!pBall->unk9C || fn_8000AD9C(pBall->vPos[0] - pBall->vLand[0]) >= 0.16666667f ||
            fn_8000AD9C(pBall->vPos[1] - pBall->vLand[1]) >= 0.16666667f ||
            fn_8000AD9C(pBall->vPos[2] - pBall->vLand[2]) >= 0.16666667f || pBall->fLandImpact < 5.0f) {
            if ((r & 127) < 16 - uLuck / 16) {
                r >>= 8;
                goto sandC;
            }
        sandClean:
            pBall->nLie     = 6;
            pBall->nSurface = 35;
            goto sandEnd;
        }
        if (!(pBall->fLandImpact < 6.0f)) goto sandC;
    sandB:
        if ((r & 127) < uLuck / 4) goto sandClean;
        pBall->nLie     = 7;
        pBall->nSurface = 36;
        goto sandEnd;
    sandC:
        if ((r & 127) < uLuck / 4 + 16) {
            r >>= 8;
            goto sandB;
        }
        pBall->nLie     = 8;
        pBall->nSurface = 34;
    sandEnd:
        if (pSurface->nClass == 20) {
            pBall->nSurface = 146;
        }
        break;
    case 7:
    case 16:
        pBall->nLie = 13;
        break;
    case 8:
        pBall->nLie = 11;
        break;
    case 12:
        pBall->bHoled = 1;
        if (pBall->nLie != 12) {
            pBall->nLie = 12;
            if (pBall->nPlayer >= 0) {
                fn_80067074(pBall->nPlayer, 0x21, (int)pBall, !gSimulating);
            }
        }
        break;
    default:
        pBall->nLie = 17;
        break;
    }
    pBall->n6C = 0;
    pBall->f70 = 0.0f;
    if (pBall->nSurface >= 0 && pBall->nSurface < 156) {
        pLie = &gSurfaceTypes[pBall->nSurface];
        if (pLie->f04) {
            if (gSimulating || lbl_80281DD2) {
                f = 0.0f;
                r = 0;
            } else {
                r = Rand_Next(0);
                f = Rand_Float(0) - 0.5f * (0.01f * uLuck);
                if (f < 0.0f) f = 0.0f;
                f *= pLie->f04;
            }
            if (r & 1) f = -f;
            pBall->f70 = f;
        }
    }
}

// The ball has hit something (a surface, or an object as surface 13). A tree (class 17) tilts
// the hit normal by 12..19 degrees on two axes - negative unless the roll's low 5 bits are 0,
// so almost always negative; a sim, lbl_80281DD2, slot 4 or a perfect shot rolls 0 (+12, +12).
// The ball is put at the hit point (backed off along its velocity on a solid surface), a
// fraction of the tick used is returned in *pFrac, a surface-108 hit is a hazard, and gravity is
// given back for the part of the tick not flown. First bounce of a shot in flight: event 0x1D,
// and the spin stick's input becomes the ball's spin (backspin x (1.9 - green setting), side x
// (1.95 - green setting)), turned to the direction of travel; event 0x1F.
// The stick spin's weight, 1 - n84 (an inline in the original: written out twice, the registers
// come out differently).
static inline f32 Ball_SpinKeep(Ball* pBall) {
    return (f32)(1 - pBall->n84);
}

u8 fn_800539F8(Ball* pBall, f32* pHit, f32* pNormal, SurfaceType* pSurface, s32 nWhat, f32* pFrac, f32 fTicks) {
    f32 vSpin[4];
    f32 fSin, fCos;
    s32 r;
    int nA, nB;
    f32 fPrev, fNow, fBack, fAngle, fS, fC;
    if (pSurface->nClass == 17) {
        if (gSimulating || lbl_80281DD2 || pBall->nPlayer == 4 ||
            (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect)) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        nA = (r & 7) + 12;
        if (r & 31) nA = -nA;
        r >>= 8;
        nB = (r & 7) + 12;
        if (r & 31) nB = -nB;
        fn_800BAF04(pNormal, pNormal);
        fn_80055E28(0.017453292f * nA, &fSin, &fCos);
        fn_80055D70(&pNormal[0], &pNormal[1], fSin, fCos);
        fn_80055E28(0.017453292f * nB, &fSin, &fCos);
        fn_80055D70(&pNormal[2], &pNormal[1], fSin, fCos);
    }
    fn_800BAF04(pNormal, pNormal);
    pHit[1] += BALL_RADIUS;
    if (pNormal[1] > 0.0f) {
        pHit[1] += 0.0013888889f;
    }
    fPrev = Vec_Distance(pBall->vPrev, pBall->vPos);
    fNow  = Vec_Distance(pBall->vPrev, pHit);
    if (fPrev <= 2.77777799e-05f) fPrev = 2.77777799e-05f;
    if (fPrev < fNow) fPrev = 2.77777799e-05f + fNow;
    if (fPrev != 0.0f) {
        *pFrac = fNow / fPrev;
        if (*pFrac > 0.97f) *pFrac = 0.97f;
    } else {
        *pFrac = 0.97f;
    }
    Vec3Copy(pHit, pBall->vPos);
    if (pSurface->f0C >= 0.0f) {
        fn_8000C5D4(pBall->vPos, pBall->vVel, -9.25925906e-06f, pBall->vPos);
    }
    if (pSurface->nClass == 19 && pSurface == &gSurfaceTypes[108]) {
        fn_80050CAC(pBall, 1);
        return 0;
    }
    if (pNormal[1] >= 0.0f && pBall->vVel[1] < 0.0f) {
        pBall->vVel[1] += (1.05f - *pFrac) * (0.107170001f * fTicks);
        if (pBall->vVel[1] >= 0.0f) {
            pBall->vVel[1] = -0.000178616669f;
        }
    }
    pBall->pHitSurface = pSurface;
    pBall->n8C         = nWhat;
    if (pSurface->f0C >= 0.0f && !pBall->b99 && pBall->nPlayer >= 0 && pBall->nState == 2) {
        if (!pBall->b99) {
            pBall->b99 = 1;
            fn_80067074(pBall->nPlayer, 0x1D, (int)pBall, !gSimulating);
        }
        if (pBall->fSpinX != 0.0f || pBall->fSpinY != 0.0f) {
            fBack = pBall->fSpinY;
            if (fBack < 0.0f) {
                fBack *= 1.9f - gGreenSpeedMul[gGreenSpeedSetting];
            }
            vSpin[0] = fBack * Ball_SpinKeep(pBall);
            vSpin[1] = 0.0f;
            fPrev = pBall->fSpinX;
            fPrev *= 1.95f - gGreenSpeedMul[gGreenSpeedSetting];
            vSpin[2] = fPrev * Ball_SpinKeep(pBall);
            fAngle = -fn_8000AD78(pBall->vVel[0], pBall->vVel[2]);
            fS = fn_800095F0(fAngle);
            fC = fn_80009638(fAngle);
            fn_80055D70(&vSpin[0], &vSpin[2], fS, fC);
            Vec3Copy(vSpin, pBall->vSpin);
        }
        pBall->b9B = 1;
        fn_80067074(pBall->nPlayer, 0x1F, (int)pBall, !gSimulating);
    }
    return 1;
}

// The flagstick (object type 11 at +0x146; any other object is a plain hit). Not while it is
// still swaying (+0x19C). A hit is within the pole's radius (at least 1 in) plus 2 ft and within
// 8 in of it along z; the ball is stopped against it along z (normal +-z), and a real ball sets
// the flag swaying by how far off centre and how fast it hit.
u8 fn_80053E98(Ball* pBall, void* pv, f32* pHit, f32* pNormal) {
    u8* pObj = (u8*)pv;
    f32 vPole[4];
    f32 fRadius;
    f32 fDZ, fDist2;
    if (pObj[0x146] != 11) return 1;
    if (0.0f != *(f32*)(pObj + 0x19C)) return 0;
    fDist2 = (pHit[0] - pBall->vPos[0]) * (pHit[0] - pBall->vPos[0]) + (pHit[2] - pBall->vPos[2]) * (pHit[2] - pBall->vPos[2]);
    fn_800B1AB0(pObj, vPole, &fRadius);
    if (fRadius < 0.027777778f || fDist2 >= fRadius * fRadius + 0.444444478f) return 0;
    fDZ = vPole[2] - pBall->vPos[2];
    if (fn_8000AD9C(fDZ) > 0.222222224f) return 0;
    pNormal[1] = 0.0f;
    pNormal[0] = 0.0f;
    if (fDZ <= 0.0f) {
        pNormal[2] = 1.0f;
        pHit[2]    = vPole[2] - 0.222222224f;
    } else {
        pNormal[2] = -1.0f;
        pHit[2]    = 0.222222224f + vPole[2];
    }
    pHit[0] = pBall->vPos[0];
    pHit[1] = pBall->vPos[1];
    if (!gSimulating) {
        fDZ = 18.849556f;
        fDZ = fDZ * ((vPole[0] - pBall->vPos[0]) / fRadius);
        fDZ = fDZ * (pBall->fSpeed / 29.333334f);
        *(f32*)(pObj + 0x19C) = -fDZ;
    }
    return 1;
}

// Did the ball hit something (a tree, an object) between last tick and this one? fn_800B1B18
// sweeps the path, fn_80053E98 and fn_800539F8 decide; then event 0x27 and the bounce off it
// as surface 13. Returns 1 on a hit.
u8 fn_80054040(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    s32          nWhat;
    f32          fFrac;
    SurfaceType* pSurface;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (!fn_800B1B18(pBall->nPlayer, vTo, vFrom, vHit, vNormal, &nWhat)) return 0;
    if (!fn_80053E98(pBall, (u8*)nWhat, vHit, vNormal)) return 0;
    pSurface = &gSurfaceTypes[13];
    if (!fn_800539F8(pBall, vHit, vNormal, pSurface, 0, &fFrac, fTicks)) return 0;
    if (pBall->nPlayer >= 0) {
        pBall->n90 = nWhat;
        fn_80067074(pBall->nPlayer, 0x27, (int)pBall, !gSimulating);
    }
    fn_80052598(pBall, vNormal, pSurface);
    return 1;
}

// Keep a ball that is on the ground on the ground: find the ground under it (no ground at all:
// hazard). Unless settling, fn_80053240 may throw it back into the air. More than 1.68 in above
// the ground: take the other ground height if it is within 2.5 in, else look again once, else
// (unless settling) it is in the air. Then sit it on the ground.
void fn_8005418C(Ball* pBall, u8 bSettle, f32 fTicks) {
    u8  bRetried;
    f32 fGround;
    if (pBall->bHoled || pBall->nState == 2) return;
    bRetried = 0;
retry:
    if (!lbl_80281DE4) {
        fn_8004D9A8(pBall->pCourse, pBall, &lbl_80281DE0, &lbl_80281DDC);
    }
    fGround = lbl_80281DE0;
    if (fGround < -60000.0f) {
        fGround = lbl_80281DDC;
        if (fGround < -60000.0f) {
            fn_80050CAC(pBall, 1);
            return;
        }
    }
    if (!bSettle && fn_80053240(pBall, fTicks)) {
        pBall->nState = 2;
        return;
    }
    if (pBall->vPos[1] - fGround > 0.0466666669f) {
        if (pBall->nSurface != 98 && !(lbl_80281DDC < -60000.0f) && lbl_80281DDC - pBall->vPos[1] <= 0.07f) {
            fGround = lbl_80281DDC;
        } else if (!bRetried) {
            bRetried     = 1;
            lbl_80281DE4 = 0;
            goto retry;
        } else if (!bSettle) {
            pBall->nState = 2;
            return;
        }
    }
    pBall->vPos[1] = 0.0013888889f + (BALL_RADIUS + fGround);
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

// A ball in the air meets the ground. Out of bounds (600 yd) is a hazard. fn_8004E558 finds
// the hit; fn_800539F8 handles it; landing events; the cup (surface 90) kills spin; the bounce
// (fn_80052598) - on sand (class 6) the first landing point and impact are kept for the lie. On
// a cup surface more than 2.5 in below the pin: holed. Then the ball flies on for the rest of the
// tick (fn_8004EE20 for a second hit). Water (class 7 or 16): surfaces 47, 41 and 104 let a ball
// that bounces up faster than 2.93 and moves on more than 1.71 x that (and over 5.87) skip, its
// rise quartered; anything else is a hazard.
u8 Ball_Collide(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    f32          fFrac;
    SurfaceType* pSurface;
    u32          nWhat;
    u8           bOut;
    int          nIndex;
    f32          fImpact, fAcross;
    if (pBall->pCourse == NULL) return 0;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (fn_800BB028(vTo, pBall->vStart) > 360000.0f) {
        fn_80050CAC(pBall, 1);
        return 0;
    }
    if (!fn_8004E558(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface, (s32*)&nWhat, &bOut)) return 0;
    if (pSurface == NULL) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    if ((pSurface->u34 & 0x10) && pSurface->f0C < 0.0f && bOut <= 1) return 0;
    if (!fn_800539F8(pBall, vHit, vNormal, pSurface, nWhat, &fFrac, fTicks)) return 0;
    BALL_LANDING_EVENTS(pBall, pSurface, nWhat);
    nIndex = fn_80050BEC(pSurface);
    if (nIndex == 90) {
        pBall->vSpin[0] = 0.0f;
        pBall->vSpin[1] = 0.0f;
        pBall->vSpin[2] = 0.0f;
    }
    fImpact = fn_80052598(pBall, vNormal, pSurface);
    if (pSurface->nClass == 6 && !pBall->unk9C) {
        pBall->unk9C       = 1;
        pBall->fLandImpact = fn_80009680(fImpact);
        Vec3Copy(pBall->vPos, pBall->vLand);
    }
    if (pSurface->nClass == 12 || pSurface->nClass == 18 ||
        (nIndex == 90 && Vec_Distance(pBall->vPos, PIN(pBall)) < 2.0f)) {
        if (pBall->pCourse->pin[Game_CurrentHole()].y - pBall->vPos[1] > 0.0694444478f) {
            Ball_Holed(pBall);
            return 1;
        }
    }
    if (pBall->nState == 4) {
        if (pSurface->nClass == 12) {
            pBall->bHoled = 1;
            Ball_Stop(pBall);
            return 1;
        }
        pBall->vVel[1] = 0.0293333326f;
    }
    Vec3Copy(pBall->vPos, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(vFrom, vTo);
    if (vNormal[1] >= 0.0f && pBall->vVel[1] > 0.0f) {
        pBall->vVel[1] -= (1.05f - fFrac) * (0.107170001f * fTicks);
        if (pBall->vVel[1] <= 0.0f) {
            pBall->vVel[1] = 0.000178616669f;
        }
    }
    fn_8000C5D4(vTo, pBall->vVel, ((fTicks - 0.05f) * (1.0f - fFrac)) / 36.0f, vTo);
    if (fn_8004EE20(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface, (s32*)&nWhat)) {
        Vec3Copy(pBall->vPos, pBall->vPrev);
        fn_8000C5D4(pBall->vPos, pBall->vVel, 9.25925906e-06f, pBall->vPos);
    } else {
        vFrom[1] += BALL_RADIUS;
        Vec3Copy(vFrom, pBall->vPrev);
        vTo[1] += BALL_RADIUS;
        Vec3Copy(vTo, pBall->vPos);
    }
    if (pSurface->nClass == 7 || pSurface->nClass == 16) {
        nIndex = fn_80050BEC(pSurface);
        if ((nIndex == 47 || nIndex == 41 || nIndex == 104) && pBall->vVel[1] > 2.9333334f) {
            fAcross = fn_80009680(pBall->vVel[0] * pBall->vVel[0] + pBall->vVel[2] * pBall->vVel[2]);
            if (fAcross > 1.71f * pBall->vVel[1] && fAcross > 5.86666679f) {
                pBall->vVel[1] *= 0.25f;
                return 1;
            }
        }
        fn_80050CAC(pBall, 1);
        return 0;
    }
    return 1;
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

// State 4, rolling. Holed when on a cup surface (class 12 or 18, or surface 90 within 2 yd of
// the pin) more than 2 in below the pin. Otherwise the velocity is laid onto the ground plane
// at its speed, then:
// - break: the slope's sideways part (surface +0x1C, x 0.6 on a green) turns the velocity by
//   slope / (0.457 x spin) a tick about an axis built from the spin and the slope - so a slow
//   ball breaks more;
// - gravity along the direction of travel, x 5/7 (a rolling sphere);
// - the cup pull (real ball; sims only with gSimFullCup);
// - rolling friction: normal x 0.0766 x surface +0x20 (capped on slopes over 30 degrees),
//   x 0.575 on a green, x the course settings, x the tick; when the speed is below it the ball
//   stops;
// - spin set to pure roll.
void Ball_GroundContact(Ball* pBall, f32 fTicks) {
    f32          vNormal[4];
    f32          vAccel[4];
    f32          vTmp[4];
    f32          vDown[4];
    f32          vDir[4];
    f32          vAxis[4];
    SurfaceType* pSurface;
    f32          fSin, fCos;
    f32          fA, fB;
    f32          fTurn, fZ, fX, fLen, fAngle, fK, fV, fRough;
    u8           bFlip;
    if (!fn_80052088(pBall, &pSurface, vNormal)) return;
    pBall->nSurface = fn_80050BEC(pSurface);
    if (pSurface->nClass == 12 || pSurface->nClass == 18 ||
        (pBall->nSurface == 90 && Vec_Distance(pBall->vPos, PIN(pBall)) < 2.0f)) {
        if (pBall->pCourse->pin[Game_CurrentHole()].y - pBall->vPos[1] > 0.055555556f) {
            Ball_Holed(pBall);
            return;
        }
    }
    fn_800BAF04(vNormal, vNormal);
    fn_8000C5D4(pBall->vVel, vNormal, -fn_8000C5FC(pBall->vVel, vNormal), vTmp);
    fn_800BAF04(vTmp, vDir);
    fn_8001EF34(vDir, fn_80009680(fn_80009744(pBall->vVel)), pBall->vVel);
    fn_8001EF34(vNormal, -0.839999974f, vDown);
    fZ   = -0.173615396f * vDown[2];
    fX   = 0.173615396f * vDown[0];
    fLen = fn_80009680(fZ * fZ + fX * fX);
    if (fLen != 0.0f) {
        fK = fn_8000AD9C(fZ * vDir[0] + fX * vDir[2]) / fLen;
        fX *= fK;
        fZ *= fK;
        fLen = pSurface->f1C * (f32)fn_80009680(fZ * fZ + fX * fX);
        if (pSurface->nClass == 3) {
            fLen *= 0.6f;
        }
        fn_8001EF78(pBall->vVel, vDown, pBall->vSpin);
        fn_8001EF34(pBall->vSpin, 1.41723347f, pBall->vSpin);
        fAngle = 0.45722881f * (f32)fn_80009680(fn_80009744(pBall->vSpin));
        fTurn = 0.0f;
        if (fTurn != fAngle) {
            fTurn = fLen / fAngle;
        }
        vAxis[0] = pBall->vSpin[1] * fX;
        vAxis[1] = pBall->vSpin[2] * fZ - pBall->vSpin[0] * fX;
        vAxis[2] = -(pBall->vSpin[1] * fZ);
        vAxis[3] = 1.0f;
        if (vAxis[1] < 0.0f) {
            fn_80055EF8(vAxis, vAxis);
            bFlip = 1;
        } else {
            bFlip = 0;
        }
        fX = fn_8000AD78(vAxis[0], vAxis[1]);
        fn_80055E28(fX, &fSin, &fCos);
        fA   = vAxis[0];
        fAngle = vAxis[2];
        fB   = vAxis[1];
        fn_80055D70(&fA, &fB, fSin, fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fSin, fCos);
        fAngle = -fn_8000AD78(fAngle, fB);
        fn_80055E28(fAngle, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fSin, fCos);
        if (!bFlip) {
            fTurn = -fTurn;
        }
        fn_80055E28(fTurn, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fSin, fCos);
        fn_80055E28(-fAngle, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fSin, fCos);
        fn_80055E28(-fX, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fSin, fCos);
    }
    fn_8001EF34(vDir, -(0.714285731f * (0.107170001f * vDir[1])), vAccel);
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    if (!gSimulating || gSimFullCup) {
        Ball_CupPull(pBall, fTicks);
    }
    fRough = pSurface->f20;
    if (vNormal[1] < 0.866f) {
        fRough *= 0.5f * vNormal[1];
        if (fRough > 0.14f) fRough = 0.14f;
    }
    fTurn = vNormal[1] * (0.0765499994f * fRough);
    if (pSurface->nClass == 3) {
        fTurn *= 0.575f;
    }
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fV = gTurfSpeedMul[gTurfSpeed];
        if (fV > 1.0f) {
            fTurn *= 2.0f - (1.16f * (fV - 1.0f) + 1.0f);
        } else if (fV < 1.0f) {
            fTurn *= 0.55f * -fV + 2.0f;
        } else {
            fTurn *= 2.0f - fV;
        }
    }
    if (pSurface->nClass == 3) fTurn *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    if (pSurface->nClass == 2) fTurn *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    if (pSurface->nClass == 5) fTurn *= 2.0f - gRoughMul[gRoughSetting];
    fTurn *= fTicks;
    if ((f32)fn_80009680(fn_80009744(pBall->vVel)) < fTurn) {
        Ball_Stop(pBall);
        return;
    }
    fB = pBall->vVel[1];
    fn_8000C5D4(pBall->vVel, vDir, -fTurn, pBall->vVel);
    if (pBall->nSurface == 98) {
        pBall->vVel[1] = 0.9f * fB;
    }
    pBall->vSpin[0] = -1.41723347f * (pBall->vVel[2] * vDown[1]);
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 1.41723347f * (pBall->vVel[0] * vDown[1]);
    if (pBall->nPlayer >= 0) {
        fn_800A3980(pBall, pBall->nPlayer);
    }
}

// The height of a ball in the air above the ground (0 when not in the air). With no ground
// under it: within 8 ft of the pin it is set on the other ground height; if that is less than
// 1.68 in above it, that is the ground; else above the course floor it is still in play
// (surface -1), and below it a hazard.
f32 fn_80055324(Ball* pBall) {
    f32          vNormal[4];
    f32          vNormal2[4];
    f32          vPin[4];
    f32          fHeight;
    f32          fHeight2;
    SurfaceType* pSurface;
    SurfaceType* pSurface2;
    if (pBall->nState != 2) return 0.0f;
    fn_8004D9E0(pBall->pCourse, pBall, &fHeight, &pSurface, vNormal, &fHeight2, &pSurface2, vNormal2);
    if (fHeight < -60000.0f) {
        Vec_Copy(PIN(pBall), vPin);
        vPin[1] += BALL_RADIUS;
        if (Vec_Distance(vPin, pBall->vPos) < 2.66666675f) {
            pBall->vPos[1]  = 0.0013888889f + (BALL_RADIUS + fHeight2);
            pBall->nSurface = -1;
            return 0.0f;
        }
        if (!(fHeight2 < -60000.0f) && fHeight2 - pBall->vPos[1] < 0.0466666669f) {
            pBall->vPos[1] = 0.0013888889f + (BALL_RADIUS + fHeight2);
            fHeight  = fHeight2;
            pSurface = pSurface2;
        } else {
            if (pBall->vPos[1] > fn_8000C594()->fFloor) {
                pBall->nSurface = -1;
                return pBall->vPos[1] - fn_8000C594()->fFloor;
            }
            fn_80050CAC(pBall, 1);
            return 0.0f;
        }
    }
    pBall->nSurface = fn_80050BEC(pSurface);
    return pBall->vPos[1] - fHeight;
}

// One tick: the state's step (air, rolling, bouncing), then move by velocity / 36. A ball
// still in play is kept on the ground or collided, checked against objects, and every 4 s
// (0.0167 a tick) it must have moved 4 in, or it stops. Then its speed (x 60 / 36), its height
// and its closest approach to the pin.
void Ball_Tick(Ball* pBall, f32 fTicks) {
    f32 fDist;
    lbl_80281DE4 = 0;
    switch (pBall->nState) {
    case 2:
        Ball_FlightStep(pBall, fTicks);
        break;
    case 3:
        fn_80052268(pBall, fTicks);
        break;
    case 4:
        Ball_GroundContact(pBall, fTicks);
        break;
    default:
        goto done;
    }
    Vec3Copy(pBall->vPos, pBall->vPrev);
    fn_8000C5D4(pBall->vPos, pBall->vVel, fTicks / 36.0f, pBall->vPos);
    if (pBall->nState != 1 && pBall->nState != 5) {
        if (!pBall->bHoled) {
            if (pBall->nState != 2) {
                fn_8005418C(pBall, 0, fTicks);
            } else {
                Ball_Collide(pBall, fTicks);
            }
            fn_80054040(pBall, fTicks);
        }
        pBall->fB8 += 0.0166666675f * fTicks;
        if (pBall->fB8 > 4.0f) {
            fDist = Vec_Distance(pBall->vStart, pBall->vPos);
            if (fn_8000AD9C(fDist - pBall->fB4) < 0.111111112f) {
                Ball_Stop(pBall);
            } else {
                pBall->fB4 = fDist;
                pBall->fB8 = 0.0f;
            }
        }
    }
done:
    lbl_80281DE4 = 0;
    if (pBall->bHoled || pBall->nState == 1 || pBall->nState == 5) {
        pBall->fSpeed = 0.0f;
    } else {
        fDist = fn_80009680(fn_80009744(pBall->vVel));
        pBall->fSpeed  = 60.0f * (fDist / 36.0f);
        pBall->fHeight = fn_80055324(pBall);
        fDist = Ball_DistanceToPin(pBall->vPos);
        if (fDist < pBall->fClosest) {
            pBall->fClosest = fDist;
        }
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
    gTurfSpeed = n;
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
    gGreenSpeedSetting = n;
}

int fn_80055CA4(void) {
    return gGreenSpeedSetting;
}

// Course setting (0..2, else 0).
void fn_80055CAC(int n) {
    if (n < 0 || n >= 3) n = 0;
    gFairwaySetting = n;
}

int fn_80055CC8(void) {
    return gFairwaySetting;
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
    gRoughSetting = n;
}

int fn_80055D34(void) {
    return gRoughSetting;
}

void fn_80055D3C(void) {
    gGreenSpeedSetting = 0;
    gFairwaySetting = 0;
    gRoughSetting = 1;
}

void fn_80055D54(void) {
    gTurfSpeed = 2;
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
