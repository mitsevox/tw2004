#ifndef BALL_H
#define BALL_H

#include "golfer.h"

// A golf ball in flight or at rest (0xBC bytes): Player.ball and Player.ballBefore hold one each.
typedef struct Ball {
    // Field names in the comments after "TW06:" are from Tiger Woods PGA Tour 06's PhysicsBall_t
    // (0xCC bytes; see include/physics.h). TW06 added terrainHeight at 0x58, so from there on its
    // offsets are 4 higher, and it folded the u8 flags at 0x98 into one bit field.
    f32  vPos[3];               // 0x00  TW06: location
    u8   unkC[4];
    f32  vPrev[3];              // 0x10  position last step. TW06: lastLocation
    u8   unk1C[4];
    f32  vVel[3];               // 0x20  TW06: linearVelocity
    f32  f2C;                   // 0x2C
    f32  vSpin[3];              // 0x30  TW06: angularVelocity
    f32  f3C;                   // 0x3C
    f32  vStart[3];             // 0x40  where the shot started. TW06: initialShotPosition
    u8   unk4C[4];
    f32  fSpeed;                // 0x50  TW06: speed
    f32  fHeight;               // 0x54  height above the ground when dropped. TW06: altitude
    f32  fSpinX;                // 0x58  spin input x 15. TW06: sideSpinOverride
    f32  fSpinY;                // 0x5C  TW06: forwardSpinOverride
    f32  fClosest;              // 0x60  closest approach to the pin so far. TW06: closestToCupThisShot
    s32  nState;                // 0x64  physicsBallState_t: 0 dead, 1 waiting, 2 flying, 3/4 rolling, 5 out (OB or water)
    s32  nLie;                  // 0x68  Lie_t: 12 in the cup, 16 out of bounds (also water here)
    s32  n6C;                   // 0x6C  TW06 has five lie fields here (initialLie, lie, lieAngle,
    f32  f70;                   // 0x70  lieModifier, lieReadOffset); this game has three. f70 is added to a surface's value
    s32  nSurface;              // 0x74  surface type under the ball (90 = the cup). TW06: surfaceID
    s32  nStartSurface;         // 0x78  surface at the start of the shot. TW06: initialSurfaceID
    CourseInfo* pCourse;        // 0x7C  TW06: pTerrainData (TGD_TerrainInfo*)
    s32  nCollideCount;         // 0x80  TW06: collideCount
    s32  nSolidCollideCount;    // 0x84  TW06: solidCollideCount
    struct SurfaceType* pHitSurface;   // 0x88  what it last hit. TW06: pLastCollisionSurface
    s32  n8C;                   // 0x8C  TW06: pLastCollisionObject
    s32  n90;                   // 0x90  what the ball last hit (fn_80054040). TW06: pLastCollisionActor
    s32  nPlayer;               // 0x94  -1 when nobody's. TW06: playerID
    u8   bHoled;                // 0x98  TW06: PBF_InHole
    u8   b99;                   // 0x99
    u8   bHitTopArc;            // 0x9A  set when the ball starts coming down. TW06: PBF_HitTopArc
    u8   b9B;                   // 0x9B
    u8   bGotFirstSandPos;      // 0x9C  landed in sand (class 6) this shot. TW06: PBF_GotFirstSandPos
    u8   unk9D[0xA0 - 0x9D];
    f32  vFirstSandPos[3];      // 0xA0  where it first landed in sand. TW06: firstSandPosition
    f32  fAC;                   // 0xAC  ticks spent with no ground under a rolling ball
    f32  fFirstSandVMag;        // 0xB0  how hard it landed there (sqrt of Physics_HandleCollision's result). TW06: firstSandVMag
    f32  fLastDistFromInitShotPos; // 0xB4  distance from the start at the last stall check. TW06: same name
    f32  fTimeSinceLastCheck;   // 0xB8  time since the last stall check. TW06: same name
} Ball;

// Terrain
u8   Ter_PointInOOBNetwork(u8* pBall);
f32  fn_8004D5C0(CourseInfo* pCourse, f32* pPos);   // ground height, -65536.1 if none
f32  fn_8004D620(CourseInfo* pCourse, f32* pPos);   // ground height, -60000 and below if none
void Ter_GetEnclosingGroundHeight(CourseInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh);
void Ter_GetEnclosingGroundData(CourseInfo* pCourse, f32* pPos, f32* pHeight, SurfaceType** ppSurface, f32* pNormal,
                                f32* pHeight2, SurfaceType** ppSurface2, f32* pNormal2);

// Ball.c
void Ball_SetSimulating(u8 bOn);        // rehearsals and look-aheads: no sounds, effects or tree roll
void fn_80050D2C(u8 b);
void fn_80051A18(Ball* pBall, f32* pDir, f32 fSpeed, f32* pFrom);
void Ball_Launch(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB);
void fn_80054A6C(Ball* pBall);
int  Physics_Simulate(Ball* pBall, int nMs);
u8   Physics_DropBall(Ball* pBall, f32* pPos);          // put the ball on the ground at a point
void Ball_SimStep(Ball* pBall, f32 fSeconds, f32 fTick);
u8   fn_800559BC(Ball* pBall, f32* pPos);
u8   fn_80055AA8(Ball* pBall, f32* pPos, int nPlayer);  // a fresh ball for a player at a point
void fn_80055C1C(u8 b);
void fn_80055C40(int n);
void fn_80055CAC(int n);
void fn_80055CD0(int n);
void Wind_Set(int nDir, f32 fSpeed);
void Wind_Generate(void);

#endif
