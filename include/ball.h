// ball.h (our name): the ball and the ground it lies on. The Ball struct and Ball.c's physics,
// the surface types, the current hole's terrain data (its pins and tees) and the terrain queries
// (GoTerrainCollision, 0x8004AFA0..) that other files call. Players are in golfer.h, which
// includes this file.

#ifndef BALL_H
#define BALL_H

#include "game_types.h"
#include "platform.h"
#include "terrain.h"   // the terrain manager (gTerRenderer)
#include "materialtypes.h"   // TGD_MaterialInfo

extern TGD_MaterialInfo gSurfaceTypes[NUM_SURFACE_TYPES];   // 0x8017E9B8
extern const f32 gRealBallRadiusIn;     // 0x80283300  0.84: a real golf ball, in inches
extern const f32 lbl_80283304;          // 0.7056 (0.84 squared); GoDynObj.c DynObj_DrawGolfBalls

// A point on the course: x, y (up), z, and w = 1.
typedef struct PinPos {
    f32  x, y, z, w;
} PinPos;

// One strip of triangles (8 bytes): triangle k of the strip is vertices k, k+1 and k+2 from its
// first vertex.
typedef struct TGD_PolygonReference {
    u16  nVertex;               // 0x0  first vertex, low 16 bits
    u16  nObject;               // 0x2  the course object (row of pObjects) the strip is part of; 0: ground
    u8   uFlags;                // 0x4  POLYREF_*; 0 for ground that is there whatever the pin position
    u8   nTris;                 // 0x5  its triangles
    u8   nSurface;              // 0x6  row of gSurfaceTypes
    u8   nVertexHi;             // 0x7  first vertex, high bits
} TGD_PolygonReference;
LAYOUT_ASSERT(TGD_PolygonReference, 0x8);

// TGD_PolygonReference.uFlags: a strip with any bit set is part of a cup's ground, there only for
// the pin positions it names.
#define POLYREF_PIN_POSITIONS       0x0F   // bit n: there when the hole's pin is at position n
#define POLYREF_NOT_IN_SPLIT_SCREEN 0x10   // and not while the screen is split

#define TER_FIRST_VERTEX(pRef) (((pRef)->nVertexHi << 16) + (pRef)->nVertex)

// One cell of the ground grid (0xC bytes).
typedef struct TGD_Cell {
    s16  nMaxHeight;            // 0x0  highest ground in the cell, whole yards
    s16  nMinHeight;            // 0x2  lowest ground in the cell, whole yards
    u32  uRefs;                 // 0x4  its strips: their first row of pPolyRefs << CELL_REFS_FIRST_SHIFT
                                //      | their count (CELL_REFS_COUNT_MASK)
    u16  nObjRefs;              // 0x8  its course objects: how many
    u16  nObjRefOffset;         // 0xA  and their first row of pObjRefs
} TGD_Cell;
LAYOUT_ASSERT(TGD_Cell, 0xC);

#define CELL_REFS_FIRST_SHIFT 12      // TGD_Cell.uRefs: the first strip's row, above the count
#define CELL_REFS_COUNT_MASK  0xFFF   // TGD_Cell.uRefs: how many strips

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

// One of a hole's lights as the course data stores it (TW07: TGD_LightDesc).
typedef struct TGD_LightDesc {
    u8   nType;                 // 0x00  1: directional, 2: a point light
    u8   unk1[0x10 - 0x1];
    f32  vColor[4];             // 0x10
    f32  vPos[4];               // 0x20
} TGD_LightDesc;

// A hole's lights as the course data stores them (TW07: TGD_LightingData). Only the first five
// descriptions are ever read.
typedef struct TGD_LightingData {
    s32  nLights;               // 0x00
    u8   unk4[0x10 - 0x4];
    TGD_LightDesc aLight[5];    // 0x10
} TGD_LightingData;

// The optional block at TGD_TerrainInfo.p3C: what Ter_CourseLoadCallback hands the glows (glows.h)
// when a hole is loaded; without it the glows get 0.8, 0.8, 0.4 / 0, 150, -400 / 1.
typedef struct CourseGlowBlock {
    f32  v0[3];                 // 0x00  to SF_vSetSunPosition (GlowState.v4), and gSession.f5B3C..f5B44
    u32  nC;                    // 0x0C  to SF_vSetFlareType (GlowState.n1930) when 0..3, else 1
    f32  v10[4];                // 0x10  to SF_vSetSunColor (GlowState.v14)
} CourseGlowBlock;

// One direction of a hole's fog as the course data stores it (TW06: TGD_FogDesc).
typedef struct TGD_FogDesc {
    f32  vColour[4];            // 0x00  red, green, blue (0..1); the fourth is replaced by fDistance
    f32  fDistance;             // 0x10
    u8   unk14[0x20 - 0x14];
} TGD_FogDesc;

// A hole's fog as the course data stores it (TW06: TGD_FoggingData): see fog.h.
typedef struct TGD_FoggingData {
    TGD_FogDesc aDirection[4];  // 0x00
    f32  fRotation;             // 0x80
} TGD_FoggingData;

// The current hole's terrain data (Ter_GetTGD): the ground as collision data. The ground is
// triangle strips (TGD_PolygonReference) found through a grid of cells (TGD_Cell).
typedef struct TGD_TerrainInfo {
    u32    nGridWidth;          // 0x00  cells across (x)
    u32    nGridLength;         // 0x04  cells along (z)
    f32    fGridOrigin[2];      // 0x08  x, z of the grid's corner
    f32    fGridCellSize[2];    // 0x10  a cell's size along x and z
    u8     unk18[0x20 - 0x18];
    u32    nPolyRefs;           // 0x20  rows of pPolyRefs
    u8     unk24[4];
    f32  (*pVerts)[3];          // 0x28  every strip's vertices
    u8*    pTriFlags;           // 0x2C  per vertex, for the triangle that ends there (TER_TRI_*)
    u8*    pLight;              // 0x30  per vertex: the light on the ground there, 0..255
                                //       (Ter_GetAmbientLight)
    TGD_Cell* pGrid;            // 0x34  nGridWidth x nGridLength cells, row by row
    TGD_FoggingData* p38;      // 0x38  four optional blocks (NULL when absent); TW06 has its fog, sun,
    struct CourseGlowBlock* p3C;    // 0x3C    sky and lighting data in the same place. fn_800A27FC uses
    u8*    p40;                 // 0x40    p38 without a test, p44 only when it is set
    TGD_FoggingData* p44;      // 0x44
    TerObject* pObjects;        // 0x48  the course objects. TW06: pObjectInstanceTable (0x54)
    TGD_PolygonReference* pPolyRefs;   // 0x4C  the strips, listed cell by cell (TGD_Cell.uRefs)
    u16*   pObjRefs;            // 0x50  per cell, the objects in it (indices). TW06: pObjectReferenceList
                                //       (0x5C)
    u8     unk54[0x60 - 0x54];
    f32    v60[3];              // 0x60  the hole's own gSession.f5B3C..f5B44 when not all 0
                                //       (Ter_CourseLoadCallback)
    f32    fFloor;              // 0x6C  a ball in the air above this with no ground under it is still in play
    PinPos pin[4];              // 0x70  the hole's four pin positions: gpGame->nPinSet[] picks one
    PinPos tee[4];              // 0xB0  the tee of each tee set (gSession.nTeeSet[])
    u8     unkF0[0x100 - 0xF0];
    TGD_LightingData lights;    // 0x100 the hole's lights
} TGD_TerrainInfo;

// TGD_TerrainInfo.pTriFlags, the byte of the triangle that ends at a vertex
#define TER_TRI_KICK_MASK    0x07   // bits 0-2; 0: the height and collision lookups skip the triangle
                                    // (Ter_GetGroundPolyList's strip list does not test them)
#define TER_TRI_DONE         0x08   // set while a walk over the strips has seen the triangle
#define TER_TRI_HIGHEST_SHIFT 4     // bits 4-5: which corner (0..2) is highest
#define TER_TRI_LOWEST_SHIFT  6     // bits 6-7: which corner is lowest
#define TER_TRI_HIGHEST_CORNER(uFlags) (((uFlags) >> TER_TRI_HIGHEST_SHIFT) & 3)
#define TER_TRI_LOWEST_CORNER(uFlags)  (((uFlags) >> TER_TRI_LOWEST_SHIFT) & 3)

// An object in the world the ball can hit (a tree, a building, the flagstick), as DynObj_FindBallHit
// reports it. Only the fields Ball.c reads are known; its size is not.
typedef struct HitObject {
    u8   unk0[0x140];
    u16  nActorId;              // 0x140  DynObj.nActorId: passed to gpGame->pfnCollisionActor when the
                                //        ball hits it (event.c)
    u8   unk142[0x146 - 0x142];
    u8   nType;                 // 0x146  DynObj.nType; 11 = the flagstick
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
    s32  nState;                // 0x64  physicsBallState_t: 0 dead, 1 waiting, 2 flying, 3/4 rolling, 5 out
                                //       (OB or water)
    s32  nLie;                  // 0x68  Lie_t: 12 in the cup, 16 out of bounds (also water here)
    s32  nLieAngle;             // 0x6C  the UI can ask it (message 19,
                                //       GM_vGetPlayerCurrentLieAngle); only ever set to 0. TW06
                                //       has five lie fields here (initialLie, lie, lieAngle,
                                //       lieModifier, lieReadOffset); this game has three
    f32  fLieModifier;          // 0x70  the random lie quality (Physics_SetLie: up to +- the surface's
                                //       f04, less with LUCK), added to its f00 at launch. TW06:
                                //       lieModifier (next after lieAngle)
    s32  nSurface;              // 0x74  surface type under the ball (90 = the cup). TW06: surfaceID
    s32  nStartSurface;         // 0x78  surface at the start of the shot. TW06: initialSurfaceID
    TGD_TerrainInfo* pCourse;        // 0x7C  TW06: pTerrainData (TGD_TerrainInfo*)
    s32  nCollideCount;         // 0x80  TW06: collideCount
    s32  nSolidCollideCount;    // 0x84  TW06: solidCollideCount
    TGD_MaterialInfo* pHitSurface;   // 0x88  what it last hit. TW06: pLastCollisionSurface
    TerObject* pHitObject;      // 0x8C  the object it last hit. TW06: pLastCollisionObject
    HitObject* pHitActor;       // 0x90  the world object it last hit (Physics_CheckDynObjCollisions). TW06:
                                //       pLastCollisionActor
    s32  nPlayer;               // 0x94  -1 when nobody's. TW06: playerID
    u8   bHoled;                // 0x98  TW06: PBF_InHole
    u8   bLanded;               // 0x99  first bounce on a solid surface done (event 0x1D); 1 from the
                                //       start for a putt or a placed ball
    u8   bHitTopArc;            // 0x9A  set when the ball starts coming down (a putt starts with it
                                //       set). TW06: PBF_HitTopArc
    u8   bSpinApplied;          // 0x9B  the spin-stick input went into the spin at that bounce
                                //       (event 0x1F); 1 from the start for a putt
    u8   bGotFirstSandPos;      // 0x9C  landed in sand (class 6) this shot. TW06: PBF_GotFirstSandPos
    u8   unk9D[0xA0 - 0x9D];
    f32  vFirstSandPos[3];      // 0xA0  where it first landed in sand. TW06: firstSandPosition
    f32  fAC;                   // 0xAC  ticks spent with no ground under a rolling ball
    f32  fFirstSandVMag;        // 0xB0  how hard it landed there (sqrt of Physics_HandleCollision's result).
                                //       TW06: firstSandVMag
    f32  fLastDistFromInitShotPos; // 0xB4  distance from the start at the last stall check. TW06: same name
    f32  fTimeSinceLastCheck;   // 0xB8  time since the last stall check. TW06: same name
} Ball;
LAYOUT_ASSERT(Ball, 0xBC);

// ---- the terrain ----------------------------------------------------------------------------

// A node of a TNetwork outline (0x30 bytes). Each names its two neighbours; the outline is
// walked from node 0 by taking the neighbour it did not come from.
typedef struct TNodeS {
    f32  vPos[3];               // 0x00
    u8   unkC[4];
    s16  aLinks[10];            // 0x10  linked node indexes (-1: none); an outline's first two
                                //       are its two neighbours. fn_8006A7A8 clears those naming
                                //       the node itself
    u8   unk24[0x30 - 0x24];
} TNode;
LAYOUT_ASSERT(TNode, 0x30);

// The placement outline's nodes (target.c fn_8006A7A8 fills pNode; lbl_80281E44 counts them).
// Only what the cleaned code reads; the table's full size (0x2AC) is not a whole number of these.
typedef struct TPlaceNode {
    TNode* pNode;               // 0x0
    u8   unk4[4];
} TPlaceNode;
extern TPlaceNode lbl_801D5CCC[85];
extern s32 lbl_80281E44;

// A network: a set of linked nodes on the course, from the hole's 'Cnet' stream objects
// (UNetwork.c). Most are closed outlines (out of bounds, free-drop areas, commentary zones, where
// the ball may be placed); the AI's aim points and the grass come the same way. The nodes follow
// the two halfwords, and some types put more data after the last node.
typedef struct TNetwork {
    s16  nExportType;           // 0x0  which system it is for (NetworkType)
    s16  nNumNodes;             // 0x2
    TNode aNodes[1];            // 0x4  nNumNodes of them
} TNetwork;

// TNetwork.nExportType: which system a network is for, the type its loader is registered under
// (Network_RegisterLoadNetworkCallback).
enum NetworkType {
    NETWORK_TYPE_AI_TARGETS = 0,        // the AI's aim points (AI_TargetsLoad)
    NETWORK_TYPE_OUT_OF_BOUNDS = 1,     // an in-bounds outline (Ter_OOBNetworkLoadCallback)
    NETWORK_TYPE_PLACE_BALL = 3,        // where the ball may be placed (target.c)
    NETWORK_TYPE_FREE_DROP = 4,         // a free-drop area (Ter_FreeDropNetworkLoadCallback)
    NETWORK_TYPE_COMMENTARY_ZONE = 5,   // a commentary zone (SitDevCommentaryZones.c)
    NETWORK_TYPE_GRASS = 6              // the grass (Grass_LoadNetworkData)
};

#define MAX_FREE_DROP_NETWORKS 25
#define MAX_OOB_NETWORKS       5

// The bounds of one set of 3D cup geometry (0x20 bytes).
typedef struct TerBox {
    f32  vMin[4];               // 0x00
    f32  vMax[4];               // 0x10
} TerBox;

#define NUM_CUP_POSITIONS 4     // one set of cup geometry per pin position (TGD_TerrainInfo.pin)

#define MAX_OBJECTS 1000        // course objects a line test can mark
#define TER_NO_GROUND -65536.125f   // the height the ground lookups return when nothing is under the point

extern TerBox   gTerCupGeometryBounds[NUM_CUP_POSITIONS];  // the 3D cup geometry of each pin position
extern TNetwork* gTerFreeDropNetworks[MAX_FREE_DROP_NETWORKS];
extern TNetwork* gTerOOBNetworks[MAX_OOB_NETWORKS];
extern u8        gTerObjectMarks[MAX_OBJECTS];        // objects near the current line
extern f32       gBallDropSpot[4][4];  // per player: the last spot where the ball could be dropped
extern f32       gBallPreferredLieSpot[4][4];  // per player: the last such spot with a preferred lie
extern u8        gTerUse3DCupGeometry;        // the cup is real geometry
extern s32       gTerNumFreeDropNetworks;        // free-drop networks loaded
extern s32       gTerNumOOBNetworks;        // out-of-bounds networks loaded

// UNetwork.c: the hole's networks and the outline tests.
typedef void (*LoadNetworkCallbackPtr)(TNetwork* pNetwork);   // a loader: gets each network of its type

// A registered loader: the network type it takes and the loader.
typedef struct TLoadNetworkCallbackListEntryS {
    int  nNetType;                      // 0x0  a NetworkType
    LoadNetworkCallbackPtr pfnLoad;     // 0x4
} TLoadNetworkCallbackListEntry;
LAYOUT_ASSERT(TLoadNetworkCallbackListEntry, 0x8);

#define NETWORK_NUM_NETWORKS 32         // the networks the hole's list holds
#define NETWORK_NUM_LOADERS  8          // the loaders that can be registered

// Registers callbackPtr as the loader of networks of type netType; 0 when all
// NETWORK_NUM_LOADERS are taken.
u8   Network_RegisterLoadNetworkCallback(int netType, LoadNetworkCallbackPtr callbackPtr);
void Network_CloseModule(void);
void Network_InitModule(void);
// The winding number of the outline around P (x, z): nonzero when P is inside.
s32  wn_PnPoly(f32* P, TNetwork* network, s32 n);
// Whether the segment start-end crosses the outline (x, z); intersect gets the crossing nearest start.
u8   Network_LineNetworkIntersection(f32* start, f32* end, TNetwork* network, s32 n, f32* intersect);
// Whether the ray from start through end crosses the outline (x, z).
u8   Network_RayNetworkDoesIntersect(f32* start, f32* end, TNetwork* network, s32 n);
TGD_TerrainInfo* Ter_GetTGD(void);          // the current hole's terrain data, NULL until loaded

// GoTerrainCollision.c
s32  Ter_iNumOOBNetworksLoaded(void);

// target.c: placing the ball (TW06's PlaceBall_* functions, names not proven here)
u8   PlaceBall_IsValidDropLocation(f32* pPos);            // a ball may be placed here. TW06:
                                                          // PlaceBall_IsValidDropLocation?
u8   PlaceBall_CheckInBounds(f32* pPos);            // the point is in bounds. TW06: PlaceBall_CheckInBounds?
TNetwork* PlaceBall_GetPlaceBallNetwork(void);            // the hole's placement outline (lbl_80281E30), if
                                                          // any
f32  CamScript_GuessBestPlayableHeight(f32* pPos, TGD_MaterialInfo** ppSurface);   // 0x800447DC

// TerrainGround.c: the ground under a quad or a point
// The ground strips under a quad, as runs of triangles, leaving out surfaces with uiIgnoreFlags
int  Ter_GetGroundPolyList(TGD_TerrainInfo* pTerrainData, f32* p1, f32* p2, f32* p3, f32* p4,
                           TGD_PolygonReference* pReturnResult, int iListSize, u32 uiIgnoreFlags);
// The highest ground triangle at or below a point: its height there, its cell, its strip, its first
// vertex and that vertex's number in the strip; TER_NO_GROUND when there is none
f32  Ter_GetSupportingGroundTriangle(TGD_TerrainInfo* pTerrainData, f32* location, TGD_Cell** pCellResult,
                                     TGD_PolygonReference** pPolyRefResult, f32 (**pTriangleVertices)[3],
                                     s32* iFirstVertexIndex);
// The surface of that triangle; NULL when there is none
TGD_MaterialInfo* Ter_GetSupportingGroundMaterial(TGD_TerrainInfo* pTerrainData, f32* location);

// GoTerrainCollision (TW06's goterraincollision.c; types from its definitions)
void Ter_InitTGD(TGD_TerrainInfo* pCourse);          // TW06: Ter_InitTGD, readies a loaded course's data
u8   Ter_Use3DCupGeometry(void);               // the cup is real geometry the ball drops into
u8   Ter_PointInFreeDropNetwork(f32* pPos);    // inside a free-drop area
u8   Ter_PointInOOBNetwork(f32* pPos);         // inside the in-bounds outlines (always, with none loaded)
u8   Ter_CheckObjectAndHazardObstruction(f32* pPos, f32 fRadius, u8 bModels, u8 bHazards, f32 fStep, u8 bSlope,
                                         f32 fMaxSlope);
u8   Ter_SearchForDropLocation(int nPlayer, u8 bPreferred, u8 bCheck, f32* pOut);   // where to drop the ball
f32  Ter_CheckForDropLocation(TGD_TerrainInfo* pCourse, f32* pPos, u8 bOnDropSurface, u8* pbDrop,
                              u8* pbPreferred,
                              TGD_MaterialInfo** ppSurface);   // whether a ball could be dropped at a point
u8   Ter_IsValidDropSurface(s32 nSurface);
u8   Ter_GetSupportingGroundNormal(TGD_TerrainInfo* pCourse, f32* pPos, f32* pNormal);   // 0 with no ground
u8   Ter_PointInTriangleXZpY(f32* pA, f32* pB, f32* pC, f32 fX, f32 fZ);   // (x, z) lies inside the triangle
// a point's weights in a triangle
void Ter_GetBarycentricCoords(f32 (*pTri)[3], f32* pPos, f32* pA, f32* pB, f32* pC);
f32  Ter_GetHighestGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos);   // ground height, -65536.1 if none
f32  Ter_GetLowestGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos);   // ground height (GoTerrainCollision.c)
f32  Ter_GetAmbientLight(TGD_TerrainInfo* pCourse, f32* pPos);   // the ground's light at a point, 0..1
// Every ground height under and over pPos (up to nMax), with its surface; returns how many.
u32  Ter_GetTerrainLayers(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurfaces, f32* pHeights,
                          u32 nMax);
// ground height, -60000 and below if none
f32  Ter_GetSupportingGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos);
// covering ground height and normal
f32  Ter_GetCoveringGroundHeightAndNormal(TGD_TerrainInfo* pCourse, f32* pPos, f32* pNormal);
// the surface under a point
TGD_MaterialInfo* Ter_GetSupportingWorldMaterial(TGD_TerrainInfo* pCourse, f32* pPos);
f32  Ter_GetSupportingGroundData(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurface,
                                 f32* pNormal);
// The same with objects included (TW06: Ter_GetSupportingWorldData); GoTerrainCollision.c.
f32  Ter_GetSupportingWorldData(TGD_TerrainInfo* pCourse, f32* pPos, TGD_MaterialInfo** ppSurface,
                                f32* pNormal);
void Ter_GetEnclosingGroundHeight(TGD_TerrainInfo* pCourse, f32* pPos, f32* pLow, f32* pHigh);
void Ter_GetEnclosingGroundData(TGD_TerrainInfo* pCourse, f32* pPos, f32* pLow,
                                TGD_MaterialInfo** ppSurfaceLow, f32* pNormalLow, f32* pHigh,
                                TGD_MaterialInfo** ppSurfaceHigh, f32* pNormalHigh);
u8   Ter_CheckForPinCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                              f32* pNormal,
                              TGD_MaterialInfo** ppSurface, TerObject** ppObj);
// the first thing a line hits
u8   Ter_CheckForWorldCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                                f32* pNormal, TGD_MaterialInfo** ppSurface, TerObject** ppObj, u8* pbFlags);
u8   Ter_CheckForSolidWorldCollision(TGD_TerrainInfo* pCourse, int nPlayer, f32* pFrom, f32* pTo, f32* pHit,
                                     f32* pNormal,
                 TGD_MaterialInfo** ppSurface, TerObject** ppObj);   // the same, through branches and leaves
u8   Ter_CheckForObjectCollision(TGD_TerrainInfo* pCourse, f32* pFrom, f32* pTo, f32* pHit, f32* pNormal,
                                 TGD_MaterialInfo** ppSurface,
                 TerObject** ppObj);   // the first object a line hits
s32  MaterialTypes_getMaterialID(TGD_MaterialInfo* pSurface);   // a surface's row in gSurfaceTypes, or -1

// ---- the ball (Ball.c) ----------------------------------------------------------------------

// One club's carry for a shot kind, in yards: fDist[0] at power 0.1 up to fDist[10] at 1.1
// (fDist[9], full power, is the club's reach). Ball.c's gClubRows1..7 hold one per club, 0..24.
typedef struct ClubRow {
    f32 fDist[11];
} ClubRow;

void Physics_SetSimulating(u8 bOn);        // rehearsals and look-aheads: no sounds, effects or tree roll
void Physics_SetSimFullCup(u8 b);
f32  Physics_EstimatePuttPower(f32 fDist);            // putt power for a distance
f32  Physics_EstimateShotDistance100(int nKind, int nClub); // a club's table reach for a shot kind
// chip power from the ball's lie
f32  Physics_EstimateShotPower(f32 fDist, Ball* pBall, int nKind, int nClub);
f32  Physics_GetLiePowerPercentage(Ball* pBall);          // the ball's fLieModifier + its surface's f00; 1
                                                          // without either
void Physics_ThrowBall(Ball* pBall, f32* pDir, f32 fSpeed, f32* pFrom);
void Physics_ShotImpact(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB);
void Physics_InitShotData(Ball* pBall);
int  Physics_Simulate(Ball* pBall, int nMs);
u8   Physics_DropBall(Ball* pBall, f32* pPos);          // put the ball on the ground at a point
void Physics_TimedSimulation(Ball* pBall, f32 fSeconds, f32 fTick);
u8   Physics_SetBallPosition(Ball* pBall, f32* pPos);
u8   Physics_InitBall(Ball* pBall, f32* pPos, int nPlayer);  // a fresh ball for a player at a point
void Physics_SetNoRandomRolls(u8 b);
void Physics_SetGreenSpeedByType(int n);
void Physics_SetFairwaySpeedByType(int n);
void Physics_SetRoughLengthByType(int n);
void Wind_SetPhysicsWind(int nDir, f32 fSpeed);
f32  Wind_GetPhysicsWindVelocity(f32* pOut);               // the wind's speed; its vector (direction x speed)
                                                           // into pOut
void Wind_InitForHole(void);
void Ball_RotatePair(f32* pA, f32* pB, f32 fSin, f32 fCos);   // turns the pair (*pA, *pB) by an angle

void DynObj_ShotDivotHoleHide(Ball* pBall, int nPlayer);
void DynObj_DivotHide(Ball* pBall, int nPlayer);
// An effect at the ball (the target games): PsBallFx.c starts effect nKind's emitters whose
// threshold fValue reaches, pointing along the ball's flight (bFlight) or the player's aim.
void fn_800A30E4(int nKind, Ball* pBall, int nPlayer, u8 bFlight, f32 fValue);

#endif
