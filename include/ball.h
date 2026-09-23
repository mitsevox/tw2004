// ball.h (our name): the ball and the ground it lies on. The Ball struct and Ball.c's physics,
// the surface types, the current hole's terrain data (its pins and tees) and the terrain queries
// (GoTerrainCollision, 0x8004AFA0..) that other files call. Players are in golfer.h, which
// includes this file.

#ifndef BALL_H
#define BALL_H

#include "game_types.h"
#include "platform.h"
#include "terrain.h"   // the terrain manager (lbl_801D3CB0)

// A row of gSurfaceTypes (0x44 bytes): how a ball behaves on one kind of ground.
typedef struct SurfaceType {
    f32  f00;                   // 0x00  launch: share of the speed kept; + the ball's f70 (fn_800510EC)
    f32  f04;                   // 0x04  lie: size of the random lie quality (Ball_SetLie)
    f32  f08;                   // 0x08  launch: spin factor
    f32  f0C;                   // 0x0C  bounce restitution; below 0: branches/leaves (randomised, LUCK)
    f32  f10;                   // 0x10  bounce: friction at the contact
    f32  f14;                   // 0x14  skid: 1 - this scales the slope pull
    f32  f18;                   // 0x18  skid: friction building roll spin
    f32  f1C;                   // 0x1C  0.375 on surfaces a ball may stop on; roll: break strength
    f32  f20;                   // 0x20  roll: rolling friction
    f32  f24;                   // 0x24  bounce: how hard a landing it takes to bend the normal (softness)
    f32  f28;                   // 0x28  bounce: base softness
    u32  nClass;                // 0x2C  surface class (TW06: lieID), not a Lie_t. Ball_SetLie makes the lie
                                //       from it: 1, 2 fairway; 3 green; 4 fringe; 5, 11 rough; 6, 20 sand;
                                //       7, 16 water; 8 cart path; 12 the cup; 18 green (holes a ball, as 12).
                                //       17 = tree; 19 = not playable (Ter_CalcLowestPlayableWorldHeight)
    u8   unk30[4];
    u32  u34;                   // 0x34  bit 0x10: event 0x25 on landing
    u8   unk38[0x44 - 0x38];
} SurfaceType;
LAYOUT_ASSERT(SurfaceType, 0x44);

#define NUM_SURFACE_TYPES 156   // rows in gSurfaceTypes

extern SurfaceType gSurfaceTypes[NUM_SURFACE_TYPES];   // 0x8017E9B8

// A point on the course: x, y (up), z, and w = 1.
typedef struct PinPos {
    f32  x, y, z, w;
} PinPos;

// One strip of ground triangles (8 bytes). TW06: TGD_PolygonReference (0xC bytes, the same up
// to 0x8).
typedef struct TerPolyRef {
    u16  nVertex;               // 0x0  first vertex, low 16 bits. TW06: uiVertexIndex
    u16  n2;                    // 0x2  TW06: uiSourceObject
    u8   u4;                    // 0x4  TW06: flags
    u8   nTris;                 // 0x5  TW06: uiPolygonCount
    u8   nSurface;              // 0x6  row of gSurfaceTypes. TW06: uiMaterialType
    u8   nVertexHi;             // 0x7  first vertex, high bits. TW06: uiVertexIndexHi
} TerPolyRef;

#define TER_FIRST_VERTEX(pRef) (((pRef)->nVertexHi << 16) + (pRef)->nVertex)

// One cell of the ground grid (0xC bytes). TW06: TGD_Cell.
typedef struct TerCell {
    s16  nMaxHeight;            // 0x0  highest ground in the cell, whole yards. TW06: maxHeightYards
    s16  nMinHeight;            // 0x2  TW06: minHeightYards
    u32  uRefs;                 // 0x4  its strips: the first TerPolyRef << 12 | the count.
                                //      TW06: uiPolygonReferenceListOffsetAndCount
    u16  nObjRefs;              // 0x8  TW06: uiObjectReferenceCount
    u16  nObjRefOffset;         // 0xA  TW06: uiObjectReferenceListOffset
} TerCell;

// A course object (0x24 bytes): a tree, a building, the pin. TW06: TGD_ObjectInstanceInfo (0x28
// bytes, the same up to 0x24).
typedef struct TerObject {
    f32  vCentre[3];            // 0x00  TW06: fBoundingSphereOrigin
    f32  fRadius;               // 0x0C  TW06: fBoundingSphereRadius
    f32  vBase[3];              // 0x10  TW06: fBoundingCylinderBase
    f32  fBaseRadius;           // 0x1C  TW06: fBoundingCylinderRadius
    u16  nPatch;                // 0x20  TW06: uiPatchNum
    u16  nObjList;              // 0x22  TW06: uiObjectListNum
} TerObject;

// The current hole's terrain data (fn_8000C594): the ground as collision data. TW06:
// TGD_TerrainInfo, the same offsets up to 0x2C; TW06 has three more pointers before the polygon
// list. The ground is triangle strips: each TerPolyRef names a first vertex and a triangle count,
// and triangle k of a strip is vertices k, k+1, k+2.
typedef struct CourseInfo {
    u32    nGridWidth;          // 0x00  cells across (x). TW06: uiReferenceGridWidth
    u32    nGridLength;         // 0x04  cells along (z). TW06: uiReferenceGridLength
    f32    fGridOrigin[2];      // 0x08  x, z of the grid's corner. TW06: fReferenceGridOrigin
    f32    fGridCellSize[2];    // 0x10  TW06: fReferenceGridCellSize
    u8     unk18[0x20 - 0x18];
    u32    nPolyRefs;           // 0x20  TW06: uiPolygonReferencesListSize
    u8     unk24[4];
    f32  (*pVerts)[3];          // 0x28  TW06: pVertexList
    u8*    pTriFlags;           // 0x2C  per vertex, for the triangle that ends there: bits 0-2 = 0 skip it;
                                //       bit 3 done (fn_80050794); bits 4-5 / 6-7 its highest / lowest corner
    u8*    pLight;              // 0x30  per vertex: the light on the ground there, 0..255 (fn_8004B78C)
    TerCell* pGrid;             // 0x34  nGridWidth x nGridLength cells, row by row. TW06: pTerrainGrid (0x3C)
    u8*    p38;                 // 0x38  four optional blocks (NULL when absent); TW06 has its fog, sun,
    u8*    p3C;                 // 0x3C    sky and lighting data in the same place
    u8*    p40;                 // 0x40
    u8*    p44;                 // 0x44
    TerObject* pObjects;        // 0x48  the course objects. TW06: pObjectInstanceTable (0x54)
    TerPolyRef* pPolyRefs;      // 0x4C  TW06: pPolygonReferenceList (at 0x58 there)
    u16*   pObjRefs;            // 0x50  per cell, the objects in it (indices). TW06: pObjectReferenceList (0x5C)
    u8     unk54[0x6C - 0x54];
    f32    fFloor;              // 0x6C  a ball in the air above this with no ground under it is still in play
    PinPos pin[4];              // 0x70  the hole's four pin positions: gpGame->nPinSet[] picks one
    PinPos tee[4];              // 0xB0  the tee of each tee set (gSession.nTeeSet[])
} CourseInfo;

// An object in the world the ball can hit (a tree, a building, the flagstick), as fn_800B1B18
// reports it. Only the fields Ball.c reads are known; its size is not.
typedef struct HitObject {
    u8   unk0[0x146];
    u8   nKind;                 // 0x146  11 = the flagstick
    u8   unk147[0x19C - 0x147];
    f32  fSway;                 // 0x19C  the flag's sway; not 0 while it still moves
} HitObject;

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
    SurfaceType* pHitSurface;   // 0x88  what it last hit. TW06: pLastCollisionSurface
    TerObject* pHitObject;      // 0x8C  the object it last hit. TW06: pLastCollisionObject
    HitObject* pHitActor;       // 0x90  the world object it last hit (fn_80054040). TW06: pLastCollisionActor
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
LAYOUT_ASSERT(Ball, 0xBC);

// ---- the terrain ----------------------------------------------------------------------------

// A closed outline on the course (TW06: TNetwork, 0x14 bytes): the free-drop areas and the
// in-bounds outlines. Only the header is read so far.
typedef struct TNetwork {
    s16  nExportType;           // 0x0  TW06: ExportType
    s16  nNumNodes;             // 0x2  TW06: NumNodes
} TNetwork;

#define MAX_FREE_DROP_NETWORKS 25
#define MAX_OOB_NETWORKS       5

// The bounds of one set of 3D cup geometry (0x20 bytes).
typedef struct TerBox {
    f32  vMin[4];               // 0x00
    f32  vMax[4];               // 0x10
} TerBox;

#define NUM_CUP_POSITIONS 4     // one set of cup geometry per pin position (CourseInfo.pin)

#define MAX_OBJECTS 1000        // course objects a line test can mark
#define TER_NO_GROUND -65536.125f   // the height the ground lookups return when nothing is under the point

extern TerBox   lbl_801D53A8[NUM_CUP_POSITIONS];  // the 3D cup geometry of each pin position
extern TNetwork* lbl_801D5428[MAX_FREE_DROP_NETWORKS];
extern TNetwork* lbl_801D548C[MAX_OOB_NETWORKS];
extern u8        lbl_801D54A0[MAX_OBJECTS];        // objects near the current line
extern f32       lbl_801D5888[4][4];  // per player: the last spot where the ball could be dropped
extern f32       lbl_801D58C8[4][4];  // per player: the last such spot with a preferred lie
extern u8        lbl_80281DC0;        // the cup is real geometry
extern s32       lbl_80281DC4;        // free-drop networks loaded
extern s32       lbl_80281DC8;        // out-of-bounds networks loaded

CourseInfo* fn_8000C594(void);          // the current hole's terrain data
SurfaceType* fn_800CC190(CourseInfo* pCourse, f32* pPos);   // surface type under a point
f32  Terrain_HeightAt(f32* pPos, SurfaceType** ppSurface);   // 0x800447DC

// GoTerrainCollision (TW06's goterraincollision.c; types from its definitions)
u8   Ter_Use3DCupGeometry(void);               // the cup is real geometry the ball drops into
u8   Ter_PointInFreeDropNetwork(f32* pPos);    // inside a free-drop area
u8   Ter_PointInOOBNetwork(f32* pPos);         // inside the in-bounds outlines (always, with none loaded)
u8   Ter_CheckObjectAndHazardObstruction(f32* pPos, f32 fRadius, u8 bModels, u8 bHazards, f32 fStep, u8 bSlope,
                                         f32 fMaxSlope);
u8   Ter_SearchForDropLocation(int nPlayer, u8 bPreferred, u8 bCheck, f32* pOut);   // where to drop the ball
f32  Ter_CheckForDropLocation(CourseInfo* pCourse, f32* pPos, u8 bOnDropSurface, u8* pbDrop, u8* pbPreferred,
                              SurfaceType** ppSurface);   // whether a ball could be dropped at a point
u8   Ter_IsValidDropSurface(s32 nSurface);
f32  fn_8004D5C0(CourseInfo* pCourse, f32* pPos);   // ground height, -65536.1 if none
f32  fn_8004D620(CourseInfo* pCourse, f32* pPos);   // ground height, -60000 and below if none
SurfaceType* Ter_GetSupportingWorldMaterial(CourseInfo* pCourse, f32* pPos);   // the surface under a point
f32  Ter_GetSupportingGroundData(CourseInfo* pCourse, f32* pPos, SurfaceType** ppSurface, f32* pNormal);
void Ter_GetEnclosingGroundHeight(CourseInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh);
void Ter_GetEnclosingGroundData(CourseInfo* pCourse, f32* pPos, f32* pLow, SurfaceType** ppSurfaceLow,
                                f32* pNormalLow, f32* pHigh, SurfaceType** ppSurfaceHigh, f32* pNormalHigh);
u8   Ter_CheckForPinCollision(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                              SurfaceType** ppSurface, TerObject** ppObj);
u8   fn_8004E558(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                 SurfaceType** ppSurface, TerObject** ppObj, u8* pbFlags);   // the first thing a line hits
u8   fn_8004EE20(CourseInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                 SurfaceType** ppSurface, TerObject** ppObj);   // the same, through branches and leaves
u8   fn_8004FF34(CourseInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal, SurfaceType** ppSurface,
                 TerObject** ppObj);   // the first object a line hits
s32  fn_80050BEC(SurfaceType* pSurface);   // a surface's row in gSurfaceTypes, or -1

// ---- the ball (Ball.c) ----------------------------------------------------------------------

// One club's carry for a shot kind, in yards: fDist[0] at power 0.1 up to fDist[10] at 1.1
// (fDist[9], full power, is the club's reach). Ball.c's gClubRows1..7 hold one per club, 0..24.
typedef struct ClubRow {
    f32 fDist[11];
} ClubRow;

void Ball_SetSimulating(u8 bOn);        // rehearsals and look-aheads: no sounds, effects or tree roll
void fn_80050D2C(u8 b);
f32  fn_80050D34(f32 fDist);            // putt power for a distance
f32  fn_80050F44(int nKind, int nClub); // a club's table reach for a shot kind
f32  fn_80050F88(f32 fDist, Ball* pBall, int nKind, int nClub);   // chip power from the ball's lie
f32  fn_800510EC(Ball* pBall);          // the ball's f70 + its surface's f00; 1 without either
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
f32  Wind_Get(f32* pOut);               // the wind's speed; its vector (direction x speed) into pOut
void Wind_Generate(void);

void fn_80047B6C(Ball* pBall, int nPlayer);
void fn_80047BC0(Ball* pBall, int nPlayer);
void fn_800A30E4(int nKind, Ball* pBall, int nPlayer, int a, f32 f);   // an effect at the ball (the target games)

#endif
