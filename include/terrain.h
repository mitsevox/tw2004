// terrain.h (our name): the terrain renderer of GoTerrain.c (TW06: Ter_TerrainRendererMgr): the
// current hole's ground and objects as they are loaded from the 'ter ' chunk, the lists they are
// sorted into and drawn from, and the renderer-state setters that share the file. ball.h includes
// it. Type and field names are TW06's (its PDB) where the offsets and the use agree; this game's
// versions of the list entries are larger than TW06's, so only their first fields carry its names.

#ifndef TERRAIN_H
#define TERRAIN_H

#include "game_types.h"
#include "platform.h"

struct CourseInfo;
struct UStreamObject;
struct UObjMesh;                    // dynobj.h: the hole data is a tree of them (fn_800354BC..fn_80035500)

// A patch of ground to draw (0x34 bytes; TW06: Ter_PatchReference, 0x2C, the same up to 0x10).
// fn_80030CC8 fills Ter_TerrainRendererMgr.pPatchList with them and chains each into
// pSortedPatchList by the bits of n1C.
typedef struct Ter_PatchReference {
    void*  pGround;             // 0x00  TW06: pGround
    void*  pObjects;            // 0x04  TW06: pObjects
    f32    fDistance;           // 0x08  from the camera, less the radius. TW06: fDistance
    f32    fBoundingRadius;     // 0x0C  TW06: fBoundingRadius
    s32    eClipMethod;         // 0x10  how it is clipped (0..2; 3, off screen, is not listed): the last
                                //       index of its pSortedPatchList rows. TW06: eClipMethod (at 0x24)
    s32    iRenderPass;         // 0x14  0..2: the first index of its pSortedPatchList rows. TW06:
                                //       iRenderPass (at 0x20)
    s32    n18;                 // 0x18  } bytes of its model (fn_800354D0 3, 2, 1)
    s32    n1C;                 // 0x1C  }   bits 0x1, 0x2, 0x4: the passes it is drawn in; with 0x80,
    s32    n20;                 // 0x20  }   the lists it goes in
    struct Ter_PatchReference* pNext[4];   // 0x24  the next in each of its lists. TW06: pNext
} Ter_PatchReference;
LAYOUT_ASSERT(Ter_PatchReference, 0x34);

// An object to sort by distance (0x30 bytes; TW06: Ter_ObjectReference, 0x24, the same up to 0x14).
// From 0x1C on the fields carry TW06's names by their use (fn_80031AB4); TW06 keeps them elsewhere.
typedef struct Ter_ObjectReference {
    struct UObjMesh* apObject[3];   // 0x00  its model at each level of detail (fn_80031E58). TW06:
                                //       pObject
    struct Ter_PatchReference* pContainerPatch; // 0x0C  the patch it is in (fn_80031154). TW06: the same
    f32  fDistanceSquared;      // 0x10  the sort key (fn_8003185C, smallest first). TW06: the same
    f32  f14;                   // 0x14  } fn_80031E58 draws it opaque when f14 is beyond
    f32  f18;                   // 0x18  }   fXZDistanceToClosestBallSquared and f18 is above 0
    s8   nLODs;                 // 0x1C  how many levels of detail its model has. TW06: nLODs
    s8   iOpaqueLOD;            // 0x1D  the level drawn opaque. TW06: iOpaqueLOD
    s8   iTranslucentLOD;       // 0x1E  the level faded in over it. TW06: iTranslucentLOD
    u8   unk1F;
    f32  fAlpha;                // 0x20  how far the fade has gone, 0..1. TW06: fAlpha
    s32  iGlobalObjectIndex;    // 0x24  its row of pObjectStateList. TW06: iGlobalObjectIndex
    s32  eClipMethod;           // 0x28  TW06: eClipMethod
    u8   unk2C[0x30 - 0x2C];
} Ter_ObjectReference;
LAYOUT_ASSERT(Ter_ObjectReference, 0x30);

// An object in a draw list (0x20 bytes; TW06: Ter_ObjectDrawData, the same size); fn_80032F88 draws a
// list of them.
typedef struct Ter_ObjectDrawData {
    struct UObjMesh* pObject;   // 0x00  its model (fn_8003241C). TW06: pObject
    f32    fAlpha;              // 0x04  TW06: fAlpha
    f32    fMipmapBias;         // 0x08  TW06: fMipmapBias
    f32    fDistanceSquared;    // 0x0C  TW06: fDistanceSquared
    s32    iGlobalObjectIndex;  // 0x10  its row of pObjectStateList. TW06: iGlobalObjectIndex
    s32    eClipMethod;         // 0x14  TW06: eClipMethod (DC_EClippingMethod)
    s32    eShaderObjectType;   // 0x18  TW06: eShaderObjectType (SD_EShaderObjectType)
    u8     bUseFog;             // 0x1C  TW06: bUseFog
    u8     bSetsPrimField;      // 0x1D  TW06: bSetsPrimField
    u8     unk1E[2];
} Ter_ObjectDrawData;
LAYOUT_ASSERT(Ter_ObjectDrawData, 0x20);

#define TER_NUM_OBJECTS 650     // Ter_TerrainRendererMgr.pObjectStateList's length

// The state of one course object (0x40 bytes; TW06: Ter_ObjectState, 0x2C, laid out differently).
// Found by patch: iPatchFirstObjectInstanceIndex[patch] + the object's number in it.
typedef struct Ter_ObjectState {
    f32  f0;                    // 0x00  fn_80030254: fTreeMinPeriod plus a random share of fTreeDiffPeriod
    f32  f4;                    // 0x04  handed to row 2 or 3 of fn_8003519C (fn_80032F88), 0.5 the rest;
                                //       fTreeOverdrive at first
    f32  f8;                    // 0x08
    s32  nC;                    // 0x0C
    f32  f10;                   // 0x10  } fn_800335F8 resets f14 to f10, or to 0 with n18
    f32  f14;                   // 0x14  }
    s32  n18;                   // 0x18
    s32  n1C;                   // 0x1C
    s32  a20[4];                // 0x20  four flag words read from the object's model (fn_800354D0, 0..3)
    struct {
        f32  f0;                // 0x0   fn_80031E58 sets 1 and n4 3 when it draws the object opaque
        s32  n4;                // 0x4
    } aView[2];                 // 0x30  one per view (Ter_TerrainRendererMgr.iCurrentViewContext)
} Ter_ObjectState;
LAYOUT_ASSERT(Ter_ObjectState, 0x40);

// A level of detail's distance range (TW06: Ter_LODPlane).
typedef struct Ter_LODPlane {
    f32  fBegin;                // 0x0
    f32  fEnd;                  // 0x4
} Ter_LODPlane;
LAYOUT_ASSERT(Ter_LODPlane, 8);

// The data of the chunks fn_80034720 and fn_800347B4 are given (by UKernel.c): the position of one
// tee or one pin.
typedef struct TerPosData {
    u8   unk0[0x10];
    f32  vPos[3];               // 0x10
    u8   nIndex;                // 0x1C  its row of CourseInfo.tee or CourseInfo.pin
} TerPosData;

// What fn_80030894 hands to row 4 of the table fn_8003519C calls through: four values that swing
// between 0 and 1 over cycles of different lengths, and the frame count they were made for.
typedef struct TerWaveData {
    f32  aWave[4];              // 0x00
    u32  nFrame;                // 0x10  gSession.nFrameCount
} TerWaveData;

// The data of the 'tLOD' chunk (fn_800341A4); only two values are read.
typedef struct TerLODData {
    u8   unk0[0xC];
    f32  fC;                    // 0xC  } made whole numbers into lbl_802810E4 and lbl_802810E8
    f32  f10;                   // 0x10 }
} TerLODData;

// The terrain renderer (0x11C8 bytes): TW06's Ter_TerrainRendererMgr up to its 0x11C8 (TW06's goes
// on to 0x1238). Every field from 0x10 on has TW06's name and offset, and the code here uses each
// named one the way its name says; 0x8 and 0xC differ (TW06 keeps its sky there; here it is the
// course, which TW06 keeps in Ter_TerrainGameDataMgr: GetTGD, fn_8000C594, returns pCourse).
typedef struct Ter_TerrainRendererMgr {
    void*        pCurrentHoleData;              // 0x000  made from the 'ter ' chunk's data (fn_800073B4)
    struct UStreamObject* pCurrentHoleDataStreamData;   // 0x004  the 'ter ' chunk
    struct CourseInfo* pCourse;                 // 0x008  NULL until loaded
    struct UStreamObject* pCourseStreamData;    // 0x00C  the chunk pCourse came in
    Ter_PatchReference*  pPatchList;            // 0x010  iTotalPatches of them
    Ter_PatchReference*  pPostDrawTerrainList;  // 0x014  iTotalPostDrawTerrainPatches of them
    Ter_ObjectReference* pObjectSortList;       // 0x018  iTotalSortObjects of them
    Ter_ObjectDrawData*  pOpaqueObjectList;     // 0x01C  } iOpaqueObjects of them, and so on
    Ter_ObjectDrawData*  pTranslucentObjectList;    // 0x020  }
    Ter_ObjectDrawData*  pNearbyObjectList;     // 0x024  }
    Ter_ObjectDrawData*  pDeferredItemsList;    // 0x028  }
    Ter_ObjectDrawData*  pPanoramaItemsList;    // 0x02C  }
    Ter_ObjectDrawData*  pPostDrawItemsList;    // 0x030  }
    Ter_ObjectState*     pObjectStateList;      // 0x034  TER_NUM_OBJECTS of them
    Ter_PatchReference*  pSortedPatchList[3][4][3];     // 0x038  the heads of the patch lists
    s32          iPatchFirstObjectInstanceIndex[1024];  // 0x0C8  a patch's first object in pObjectStateList
    s32          iTotalPatches;                 // 0x10C8
    s32          iTotalPostDrawTerrainPatches;  // 0x10CC
    s32          iTotalSortObjects;             // 0x10D0
    s32          iOpaqueObjects;                // 0x10D4
    s32          iTranslucentObjects;           // 0x10D8
    s32          iNearbyObjects;                // 0x10DC
    s32          iDeferredItems;                // 0x10E0
    s32          iPostDrawItems;                // 0x10E4
    f32          xCameraReferencePos[4];        // 0x10E8
    f32          xCameraLookVector[4];          // 0x10F8
    f32          fXZDistanceToClosestBallSquared;   // 0x1108
    f32          fCameraMinHalfFieldOfViewTan;  // 0x110C
    Ter_LODPlane LODPlanes[3];                  // 0x1110  filled by fn_80031938
    s32          iLOD0End;                      // 0x1128
    s32          iLOD1Begin;                    // 0x112C
    s32          iLOD1End;                      // 0x1130
    s32          iLOD2Begin;                    // 0x1134
    f32          fDetailMipmapBias;             // 0x1138
    f32          fLakeSurfaceMipmapBias;        // 0x113C
    f32          fDefaultObjectMipmapBias[3];   // 0x1140  a row of lbl_801876D8 (fn_80034648)
    u8           bObjectTestMode;               // 0x114C
    u8           unk114D[3];
    s32          iCurrentViewContext;           // 0x1150  a view number (fn_80017028)
    s32          iLowLODListOffset;             // 0x1154  -1 once unloaded
    f32          fTreeOverdrive;                // 0x1158
    f32          fTreeMinPeriod;                // 0x115C
    f32          fTreeDiffPeriod;               // 0x1160
    f32          fTreeNoisePeriodScale;         // 0x1164
    f32          fTreeNoiseAmplitudeScale;      // 0x1168
    f32          fCrowdAnimationDelayedStartTimer;      // 0x116C  -1 after fn_800335F8
    f32          fCrowdAnimationDelayedStartPercentage; // 0x1170  GameManager's crowd flyby wants 0.5
    f32          fCrowdAnimationDelayedStartDuration;   // 0x1174
    f32          fCrowdAnimationCountdown;      // 0x1178  GameManager's crowd flyby wants 5
    f32          fCrowdFadeDistanceMin;         // 0x117C
    f32          fCrowdFadeDistanceMax;         // 0x1180
    s32          iCrowdPose;                    // 0x1184
    f32          fCrowdInterpValue;             // 0x1188
    f32          fGolferDistanceToCup;          // 0x118C
    f32          fCrowdFullMaxDistanceFromGolfer;   // 0x1190
    f32          fCrowdHalfMaxDistanceFromGolfer;   // 0x1194
    f32          fTreeDampingDistance;          // 0x1198
    f32          fTreeDampingMaxForce;          // 0x119C
    f32          fFOVScale;                     // 0x11A0
    f32          fDistanceCullYardsBase;        // 0x11A4
    f32          fDistanceCullFrameYardsSquared;    // 0x11A8  (fFOVScale x fDistanceCullYardsBase)^2
    u8           boManageZUpdate;               // 0x11AC  fn_8003272C passes its value on only when set
    u8           unk11AD[3];
    s32          eTerrainFilterMin;             // 0x11B0
    s32          eTerrainFilterMag;             // 0x11B4
    s32          eObjectFilterMin;              // 0x11B8  } passed on to fn_80032F88, which does not
    s32          eObjectFilterMag;              // 0x11BC  } read them
    Ter_PatchReference* xpGrassPatchList;       // 0x11C0
    s32          iNumGrassPatches;              // 0x11C4
} Ter_TerrainRendererMgr;
LAYOUT_ASSERT(Ter_TerrainRendererMgr, 0x11C8);

// The settings fn_80035440 copies in (0x54 bytes); fn_80035398 makes the renderer's colour
// (RenderState.a30) and its distances f28 and f2C from them.
typedef struct TerSettings {
    f32  aColours[4][4];        // 0x00  Code8006F154.c: four colours, blended round the compass by
                                //       the camera's heading
    f32  f40;                   // 0x40  Code8006F154.c: an angle added to the heading (radians)
    f32  f44;                   // 0x44  } the colour, 0..255 each
    f32  f48;                   // 0x48  }
    f32  f4C;                   // 0x4C  }
    f32  f50;                   // 0x50  RenderState.f28 is 0.375 of it, f2C 4.15 x (it + 10)
} TerSettings;
LAYOUT_ASSERT(TerSettings, 0x54);

extern Ter_TerrainRendererMgr lbl_801D3CB0;
extern TerSettings* lbl_802811E0;    // Code8006F154.c: points at lbl_801D70A8
extern TerSettings lbl_801D70A8;
// A step of a crowd member's move from one pose state to another (0x14 bytes; fn_80033744): when its
// states are n0 and n4, f4 is eased toward fC, and once there it goes to state n8 with f4 = f10.
typedef struct TerPoseStep {
    s32  n0;                    // 0x00
    s32  n4;                    // 0x04
    s32  n8;                    // 0x08
    f32  fC;                    // 0x0C
    f32  f10;                   // 0x10
} TerPoseStep;

extern TerPoseStep lbl_801877E0[6]; // fn_80033744: for objects without bit 0x40 of word 3
extern TerPoseStep lbl_80187858[2]; // fn_80033744: for objects with it
extern f32 lbl_80281D60;           // fn_80030254's random number, 0..1, stepped once per object
extern s32 lbl_801D3A30[5][32];     // [n][k]: how many of k's lowest n bits are set (fn_80030254);
                                    // fn_80032B7C picks a ground's mesh by it

void fn_8006F334(TerSettings* pSettings);   // Code8006F154.c: the default colours
extern f32 lbl_801876D8[21][3];     // rows fn_80034648 copies into fDefaultObjectMipmapBias
extern f32 lbl_802810C8;
extern s8  lbl_802810CC;
extern u8  lbl_802810EC;            // } 1: fn_80033F94 draws object list 0, list 2
extern u8  lbl_802810ED;            // }
extern s32 lbl_80281D64;            // how many of list 2's last objects fn_80033F94 leaves out
extern s32 lbl_802810D0;            // } fn_80031938's arguments (the LOD planes); 26 and 16 once
extern s32 lbl_802810D4;            // } unloaded, else set by fn_80031A08 from the 'tLOD' chunk
extern f32 lbl_802810D8;            // }
extern s32 lbl_802810DC;            // }
extern s32 lbl_802810E0;            // }
extern s32 lbl_802810E4;            // } from the 'tLOD' chunk (fn_800341A4); -1 until then
extern s32 lbl_802810E8;            // }

// GoTerrain.c
void fn_800306B8(void);             // frees the terrain
void fn_800335F8(u8 bReset);
f32  fn_800336E4(void);
f32  fn_800336F4(void);
f32  fn_80035074(f32 x);            // floor
void fn_80034720(struct UStreamObject* pObject);   // a tee's position (TerPosData)
u8   fn_800347B4(struct UStreamObject* pObject);   // a pin's position (TerPosData)
void fn_80035118(int a, int b);     // renderer state: n10 and n14
void fn_80035098(u8 b);             // renderer state
void fn_80035154(u8 b);             // renderer state
void fn_80035138(int a);            // renderer state: uFC
struct UObjMesh* fn_80034A20(u16 nPatch, u16 nObjList);    // a course object's model

#endif
