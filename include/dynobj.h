// dynobj.h (our name): the course's dynamic objects (GoDynObj.c) and their types. Each type has
// a message handler, found by the type number (fn_800499B0): message 1 asks the size of the
// type's object, 2 sets one up from its definition, 3 runs fn_80048894 on its object part (when
// it has a model), 6 is the per-frame update. Only what the code reads so far.

#ifndef DYNOBJ_H
#define DYNOBJ_H

#include "engine.h"

// A type's message handler: nMsg, the object (NULL for message 1) and two message arguments
// (UKernel.c passes 0 for the second when a message has one).
typedef int (*DynObjHandler)(int nMsg, struct DynObj* pObj, void* pArg, void* pArg2);

// An object's definition in the course data: its 'tACT' chunk from the chunk's id on
// (fn_80048BDC; TagRecord).
typedef struct DynObjDef {
    s32  n0;                    // 0x00  the chunk's id -> DynObj.n140
    u8   n4;                    // 0x04  its type (fn_800499B0) -> DynObj.n146
    u8   unk5[3];
    f32  aPos[3];               // 0x08  where it stands
    u32  u14;                   // 0x14  -> DynObj.uFlags (type 0)
    u8   n18;                   // 0x18  -> DynObj.n147 (type 0)
    u8   unk19;
    s16  n1A;                   // 0x1A  -> DynObj.n14E
} DynObjDef;
LAYOUT_ASSERT(DynObjDef, 0x1C);     // GoDynObj.c builds one on the stack: 0x1C bytes there

// Type 2's definition: a DynObjDef and its turning speed.
typedef struct DynObjTurningDef {
    DynObjDef base;             // 0x00
    f32  fSpeed;                // 0x1C  degrees a second
} DynObjTurningDef;

// Type 11's definition (GoAnimalActors.c fn_8004A24C): a DynObjDef and the animal's settings.
typedef struct DynObjAnimalDef {
    DynObjDef base;             // 0x00
    f32  f1C;                   // 0x1C  -> DynObjAnimal.f16C (and twice it f174)
    f32  a20[5];                // 0x20  -> DynObjAnimal.a178
    f32  f34;                   // 0x34  -> DynObjAnimal.f194
    f32  f38;                   // 0x38  -> DynObjAnimal.f198
    f32  aAngles[3];            // 0x3C  its rotation, in degrees (given to fn_8000A194 as 0x40, 0x3C, 0x44)
} DynObjAnimalDef;

// The stream object an object's model comes from (a view of UStreamObject: its +4 is the model).
typedef struct DynObjModelRef {
    u8*  pData;                 // 0x00  UStreamObject.pData; an animal's route follows a 12-byte
                                //       header in it (fn_8004A24C)
    struct UObjModel* p4;       // 0x04  goes to fn_800486F4
} DynObjModelRef;

// One stream object an object's 'aRSL' chunk names, found by UKernel.c's fn_80048BDC.
typedef struct DynObjModelEntry {
    u32  uType;                 // 0x0  the stream object's type
    union {
        s32  nId;               // 0x4  its id in the file (0: none) ...
        DynObjModelRef* pRef;   //      ... replaced by the object once it is found
    } u;
} DynObjModelEntry;

// An object's 'aRSL' chunk, laid over the chunk from its id on (TagRecord): the stream objects
// it uses, the first its model.
typedef struct DynObjModel {
    s32  nEntries;              // 0x0  set from the chunk's size
    DynObjModelEntry aEntries[1]; // 0x4  nEntries of them (as many as the chunk holds)
} DynObjModel;

// The names an object passes to fn_8000EA1C (DynObjSetup.pC).
typedef struct DynObjNames {
    u8   unk0[0x28];
    const char* p28;            // 0x28  -> DynObj.p15C
    const char* p2C;            // 0x2C  -> DynObj.p160
    const char* p30;            // 0x30  -> DynObj.p164
} DynObjNames;

// What a type's message 2 gets (fn_80049018 asks its handler for the object's size first).
typedef struct DynObjSetup {
    DynObjHandler pfnHandler;   // 0x00
    DynObjModel* pModel;        // 0x04
    DynObjDef* pDef;            // 0x08
    DynObjNames* pC;            // 0x0C
} DynObjSetup;

// Two bytes UKernel.c's fn_80049298 records on an object (DynObj.a138), or in lbl_801D5228 when
// the object's four are taken.
typedef struct DynObjPair {
    u8   b0;                    // 0x0
    u8   b1;                    // 0x1
} DynObjPair;

// The header of a UObjMesh's data (UObjMesh.pInfo).
typedef struct UObjMeshInfo {
    s16  n0;                    // 0x00  how many meshes UObjMesh.p8 holds (GoTerrain.c fn_800354F4)
    u8   unk2[0x24 - 0x2];
    s8   a24[0x54 - 0x24];      // 0x24  fn_80048AD4, GoTerrain.c fn_800354D0 (length unknown, at most this)
    f32  f54;                   // 0x54  scales a terrain object's mipmap bias (GoTerrain.c fn_80035560)
    f32  v58[3];                // 0x58  copied to UObjModel.v2C by type 0's setup; with f64 the
                                //       bounding sphere GoTerrain.c's fn_800354C4 returns
    f32  f64;                   // 0x64  copied to UObjModel.f5C by type 0's setup
    f32  a68[8];                // 0x68  a terrain object's bounds (GoTerrain.c fn_80035508, fn_80031154):
                                //       its centre [0..2], a radius [3] and a height [7]
    u8   unk88[0x8B - 0x88];
    u8   b8B;                   // 0x8B  bit 1: a terrain object drawn without z writes (GoTerrain.c
                                //       fn_80035554, fn_80033308)
} UObjMeshInfo;

// An entry of UObjMesh.p18 (0x2C bytes; what fn_800082CC takes).
typedef struct UObjMeshPart {
    s32  n0;                    // 0x00  copied to Ter_ObjectDrawData.eShaderObjectType (fn_8003241C)
    u8   unk4[0x2C - 0x4];
} UObjMeshPart;

// One level of detail of a UObjModel. The terrain's hole data is a tree of them (GoTerrain.c,
// fn_800354BC..fn_80035500): the root holds a mesh per patch in its p8[1], a patch's p8[0] is its
// ground and its p8[1] holds its object lists, whose meshes are the objects' (fn_80034A20).
typedef struct UObjMesh {
    UObjMeshInfo* pInfo;        // 0x00
    u8   unk4[4];
    struct UObjMesh** p8;       // 0x08  alternatives, by UObject.n108 (fn_80048AC4); a terrain
                                //       mesh's children (fn_800354E4)
    struct UObjMesh* pC;        // 0x0C  in a terrain patch's ground: the mesh drawn for it (fn_8003556C)
    struct UObjMesh* p10;       // 0x10  its first child, the rest by p14 (pInfo->n0 of them; LLObj_Gc.c
                                //       fn_80007524)
    struct UObjMesh* p14;       // 0x14  the next terrain mesh of a list (fn_800354BC); a patch's
                                //       ground's is its objects (Ter_PatchReference.pObjects)
    struct UObjMeshPart* p18;   // 0x18  fn_80048A84 passes entry n28 to fn_800082CC
    u8   a1C[0x20 - 0x1C];      // 0x1C  nonzero: entry i of p18 is used
    u32  n20;                   // 0x20  a word: GoTerrain.c fn_80032B7C draws a ground's extra meshes
                                //       only when it is not 0
    u8   unk24[0x28 - 0x24];
    s32  n28;                   // 0x28
} UObjMesh;

// What UObjModel.p10 points to (our view): a mesh tree at 0xEC (GoAnimalActors.c fn_8004ABB4).
// One of its four array sets (0x38 bytes; LLObj_Gc.c): set 0 is the model's own, sets 1-3 copies of
// set 0's arrays that fn_80007824 makes.
typedef struct UObjArraySet {
    void* ap0[5];               // 0x00  arrays of an20 rows of 12 bytes (set 0's counts)
    u8   unk14[0x20 - 0x14];
    u16  an20[5];               // 0x20  how many rows each array holds
    u8   unk2A[0x30 - 0x2A];
    s32  n30;                   // 0x30  -1: the set is not made
    u8   unk34[0x38 - 0x34];
} UObjArraySet;

typedef struct UObjModelRoot {
    UObjArraySet aSets[4];      // 0x000
    void* pE0;                  // 0x0E0  the data it was made from (fn_800073B4)
    void* pE4;                  // 0x0E4  freed with it when not NULL (fn_800075CC)
    void* pE8;                  // 0x0E8
    UObjMesh* pMesh;            // 0x0EC
} UObjModelRoot;

// A UObject's model (UObject.pModel).
typedef struct UObjModel {
    u8   unk0[0x10];
    UObjModelRoot* p10;         // 0x10  (GoAnimalActors.c fn_8004A24C)
    UObjMesh* apLod[6];         // 0x14  its levels of detail (the first four all the same: it has
                                //       none); UObject3D.c fn_80045D80 fills the first three from
                                //       its mesh group and clears the last three
    f32  v2C[3];                // 0x2C
    u8   unk38[0x5C - 0x38];
    f32  f5C;                   // 0x5C  a size: the level-of-detail scale is 0.5 / f5C
    u8   unk60[0x7C - 0x60];
} UObjModel;
LAYOUT_ASSERT(UObjModel, 0x7C);     // UObject3D.c fn_80045D80 allocates 0x7C bytes

// UObject3D.c: a model made from its stream data, and the mesh-tree readers it uses.
UObjModel* fn_80045D80(u8* pData);
int        fn_80045F50(UObjMesh* pMesh);            // how many meshes pMesh->p8 holds
UObjMesh*  fn_80045F5C(UObjMesh* pMesh, int i);     // pMesh->p8[i]
UObjMesh*  fn_80045F6C(UObjModelRoot* pRoot);       // pRoot->pMesh
void*      fn_800073B4(u8* pData, int n);           // builds a mesh tree from stream data

// UObject.c's object (0x118 bytes, fn_80048808 allocates one; a DynObj holds one at +0x10): three
// matrices and a model drawn with them.
typedef struct UObject {
    f32  m0[4][4];              // 0x000  identity at setup
    f32  m40[4][4];             // 0x040  identity at setup
    f32  m80[4][4];             // 0x080  identity at setup; m80[3] is the position (x, y, z, 1)
    u8   unkC0[0xDC - 0xC0];
    s32  nDC;                   // 0x0DC  (GoAnimalActors.c fn_8004ABB4)
    u8   unkE0[4];
    f32  fE4;                   // 0x0E4  the model's f5C
    u8   unkE8[0xF0 - 0xE8];
    UObjModel* pModel;          // 0x0F0  NULL: nothing to draw
    u8   unkF4[4];
    u32  uFlags;                // 0x0F8  bit 2: the model has levels of detail
    f32  fFC;                   // 0x0FC  0.5 / the model's f5C, kept to 0.1..2.5
    f32  f100;                  // 0x100
    s8   n104;                  // 0x104  the level of detail drawn (fn_80048AE8)
    u8   unk105;
    s16  n106;                  // 0x106
    s32  n108;                  // 0x108  goes to fn_80048AC4 when drawn; the animals set it each frame
    f32  f10C;                  // 0x10C  0.5 at setup; the animals set it each frame
    u8   unk110[0x118 - 0x110];
} UObject;
LAYOUT_ASSERT(UObject, 0x118);

// A dynamic object (0x16C bytes for type 0; a type may add fields after it).
typedef struct DynObj {
    f32  aRot[4];               // 0x000  a rotation, set to (0, 0, 0, 1)
    UObject obj;                // 0x010  what is drawn (fn_800486F4 sets it up)
    struct DynObj* pNext;       // 0x128  the next object in UKernel.c's list
    u8   unk12C[0x130 - 0x12C];
    DynObjHandler pfnHandler;   // 0x130  its type's message handler
    s32  n134;                  // 0x134  its id (fn_80048E4C finds it by this); 0 once fn_800491C4 ran,
                                //        then fn_800490EC's sweeps set -1 and free it
    DynObjPair a138[4];         // 0x138  pairs fn_80049298 records (b0 == 0: free)
    u16  n140;                  // 0x140
    s16  n142;                  // 0x142
    s16  n144;                  // 0x144
    u8   n146;                  // 0x146
    u8   n147;                  // 0x147
    u8   n148;                  // 0x148
    u8   unk149[3];
    s16  n14C;                  // 0x14C
    s16  n14E;                  // 0x14E
    s32  n150;                  // 0x150
    u32  uFlags;                // 0x154
    f32  f158;                  // 0x158
    const char* p15C;           // 0x15C  } names given to fn_8000EA1C (p15C when n144 drops,
    const char* p160;           // 0x160  }  p160 by fn_800491C4)
    const char* p164;           // 0x164  }
    s32  n168;                  // 0x168
} DynObj;
LAYOUT_ASSERT(DynObj, 0x16C);

// Type 2 (GoDynObjBase.c): an object that turns at a steady speed.
typedef struct DynObjTurning {
    DynObj base;                // 0x000
    f32  fSpeed;                // 0x16C  degrees a second
    u8   unk170[0x194 - 0x170];
} DynObjTurning;
LAYOUT_ASSERT(DynObjTurning, 0x194);

// A point of an animal's route (our name; 0x30 bytes).
typedef struct AnimalNode {
    f32  vPos[4];               // 0x00
    s16  nNext;                 // 0x10  the point after it
    u8   unk12[0x30 - 0x12];
} AnimalNode;
LAYOUT_ASSERT(AnimalNode, 0x30);

// An animal's route (our name): points chained through nNext.
typedef struct AnimalRoute {
    u8   unk0[2];
    s16  nNodes;                // 0x02
    AnimalNode aNodes[1];       // 0x04  nNodes of them (as many as the data holds)
} AnimalRoute;

// Type 11 (GoAnimalActors.c): an animal.
typedef struct DynObjAnimal {
    DynObj base;                // 0x000
    f32  f16C;                  // 0x16C  } divided by the route's length (fn_8004A14C)
    f32  f170;                  // 0x170
    f32  f174;                  // 0x174  }
    f32  a178[5];               // 0x178  from its definition's a20
    f32  f18C;                  // 0x18C
    f32  f190;                  // 0x190
    f32  f194;                  // 0x194
    f32  f198;                  // 0x198  a height it stands above its position or route (-999: 0)
    f32  f19C;                  // 0x19C
    f32  f1A0;                  // 0x1A0
    s32  n1A4;                  // 0x1A4  0..4, from its model's mesh bits (4: no model)
    AnimalRoute* pRoute;        // 0x1A8  NULL: it stands at its definition's position
    u32  n1AC;                  // 0x1AC  } its pose, copied to its UObject's n108 and f10C before it
    u32  n1B0;                  // 0x1B0  }   is drawn; n1B0 the pose it is heading for (fn_8004A578)
    f32  f1B4;                  // 0x1B4  }
    u32  n1B8;                  // 0x1B8  frames counted (fn_8004A578)
    u8   b1BC;                  // 0x1BC
    u8   b1BD;                  // 0x1BD  set when its route's first point with ground under it is
                                //        less than 0.5 above the ground (and f198 was given)
    u8   unk1BE[2];
} DynObjAnimal;
LAYOUT_ASSERT(DynObjAnimal, 0x1C0);

// A change of an animal's pose (our name; GoAnimalActors.c's lbl_80187DF0, 6 of them): in pose
// nFrom heading for nTo, f1B4 moves to fTarget, then the animal takes pose nNext at fStart.
typedef struct AnimalStep {
    u32  nFrom;                 // 0x00
    u32  nTo;                   // 0x04
    u32  nNext;                 // 0x08
    f32  fTarget;               // 0x0C
    f32  fStart;                // 0x10
} AnimalStep;
extern AnimalStep lbl_80187DF0[6];

// One entry of UKernel.c's lbl_801D5228 (12 bytes), taken by fn_80049230.
typedef struct DynObjSlot {
    DynObjPair pair;            // 0x00
    s16  n2;                    // 0x02  600 when taken
    DynObj* pObj;               // 0x04
    s32  nId;                   // 0x08  the object's id (DynObj.n134)
} DynObjSlot;
LAYOUT_ASSERT(DynObjSlot, 0xC);

extern DynObjSlot lbl_801D5228[32];

// GoDynObj.c's two records per player (our names; 0x104 and 0x100 bytes, GoDynObjMgr.aA and aB).
typedef struct GoDynObjPlayerA {
    u8   b0;                    // 0x00  set by fn_80047EF0; fn_80048184 moves pF4 while it is set
    u8   unk1[0xC - 0x1];
    f32  fC;                    // 0x0C  its heading once launched
    f32  f10;                   // 0x10  time in flight
    u8   unk14[0x20 - 0x14];
    f32  v20[4];                // 0x20  where fn_80047EF0 put pF4 (x, y, z, 1)
    f32  v30[4];                // 0x30  where it is now (Swing.c's STATEFUNC_PreShotUpdate fills it)
    f32  v40[4];                // 0x40  its launch velocity
    f32  v50[4];                // 0x50  its turn angles so far (STATEFUNC_PreShotUpdate fills them) ...
    f32  v60[4];                // 0x60  ... and how fast they grow
    u8   b70;                   // 0x70  set by fn_8004816C: launch it
    u8   unk71[0xB4 - 0x71];
    f32  mB4[4][4];             // 0xB4  its rotation
    DynObj* pF4;                // 0xF4  a 'TEO ' 10004 object (fn_80047EF0)
    u8   bF8;                   // 0xF8  pF4 is in use
    u8   bF9;                   // 0xF9  set by fn_80047EF0
    u8   unkFA[0x104 - 0xFA];
} GoDynObjPlayerA;
LAYOUT_ASSERT(GoDynObjPlayerA, 0x104);

typedef struct GoDynObjPlayerB {
    u8   b0;                    // 0x00  set by fn_80047A24; fn_80047C24 moves pF0 while it is set
    u8   unk1[0xC - 0x1];
    f32  fC;                    // 0x0C  minus the player's aim when placed
    f32  f10;                   // 0x10  time in flight
    u8   unk14[0x20 - 0x14];
    f32  v20[4];                // 0x20  where fn_80047A24 put pF0 (x, y, z, 1)
    f32  v30[4];                // 0x30  where it is now
    f32  v40[4];                // 0x40  its launch velocity
    f32  v50[4];                // 0x50  its turn angles so far ...
    f32  v60[4];                // 0x60  ... and how fast they grow
    u8   unk70[0xB0 - 0x70];
    f32  mB0[4][4];             // 0xB0  its rotation
    DynObj* pF0;                // 0xF0  a 'TEO ' 10002 object (fn_80047A24), given up by fn_80047BC0
    u8   bF4;                   // 0xF4  pF0 is in use
    u8   bF5;                   // 0xF5  set by fn_80047A24
    u8   unkF6[0x100 - 0xF6];
} GoDynObjPlayerB;
LAYOUT_ASSERT(GoDynObjPlayerB, 0x100);

// GoDynObj.c's state (0xAB0 bytes, lbl_80281DA0, allocated by fn_800461A8): per-player records
// and objects, and the models of the 'TEO ' stream objects (fn_80046288 makes them).
typedef struct GoDynObjMgr {
    GoDynObjPlayerA aA[5];      // 0x000
    GoDynObjPlayerB aB[5];      // 0x514
    DynObj* apPlayer[5];        // 0xA14  one object per player (fn_80047B6C gives it up)
    DynObj* apRing[10];         // 0xA28  ten objects used in turn ...
    s32  nRing;                 // 0xA50  ... the next one
    UObject* pTeo10000;         // 0xA54  'TEO ' 10000
    UObject* apTeo10030[4];     // 0xA58  'TEO ' 10030..10033
    UObject* apTeo10040[4];     // 0xA68  'TEO ' 10040..10043
    UObject* apTeo10020[3];     // 0xA78  'TEO ' 10020..10022, only with fn_800E39F0
    UObject* apTeo10006[4];     // 0xA84  'TEO ' 10006..10009, only with fn_800E39F0
    f32  fA94;                  // 0xA94
    f32  fA98;                  // 0xA98
    f32  fA9C;                  // 0xA9C
    f32  fAA0;                  // 0xAA0
    f32  fAA4;                  // 0xAA4
    f32  fAA8;                  // 0xAA8
    f32  fAAC;                  // 0xAAC
} GoDynObjMgr;
LAYOUT_ASSERT(GoDynObjMgr, 0xAB0);

extern GoDynObjMgr* lbl_80281DA0;
extern s32  lbl_80187D38[4];            // GoDynObj.c: each player's partner in game mode 21
extern char lbl_80187B98[27][13];       // GoDynObj.c: 27 names (fn_800484F4 finds one)
extern char lbl_80187CF8[4][13];        // GoDynObj.c: each player's ball logo texture ("logoea", ...)
extern f32  lbl_80281128;               // 1.0: scales the ball's radius (fn_8004787C); owner unknown

// UKernel.c's list of the objects, first and last, the last id given out (DynObj.n134), a bit
// mask of the used entries of lbl_801D5228 (fn_80049230; 16 at most), and two node pools (400-
// and 528-byte nodes).
extern DynObj* lbl_80281DBC;
extern DynObj* lbl_80281DB8;
extern s32 lbl_80281DB4;
extern u32 lbl_80281DB0;
extern UMemPool* lbl_80281DAC;
extern UMemPool* lbl_80281DA8;

// GoDynObj.c
void fn_80046C34(f32* pPos, int nPlayer);  // a fast ball low over class 3 ground leaves a 'TEO ' 10005
void fn_80046E1C(f32* pPos, int nPlayer);  // put the player's 'TEO ' 10001 object at pPos
void fn_80047A24(f32* pPos, int nPlayer);  // put the player's 'TEO ' 10002 object at pPos
void fn_8004816C(int nPlayer);             // launch the player's 'TEO ' 10004 object
int  fn_800484F4(const char* szName);   // the index of the name among lbl_80187B98's 27 (-1: none)

// UKernel.c, UObject.c. The UObject functions take the object part (&DynObj.obj).
DynObj* fn_80048E44(void);                                  // the first object
DynObj* fn_80048E4C(int nId);                               // the object with this id, or NULL
void fn_80048FEC(DynObj* pObj);                             // adds it at the end of the list
s32  fn_800490B8(DynObjSetup* pSetup);                      // a new object's id, -2 when none
void fn_800490EC(void);                                     // sweeps out the objects given up
void fn_800491C4(DynObj* pObj);
void fn_80049514(DynObj* pObj, DynObjSetup* pSetup);    // type 0's message 2
void fn_800486F4(UObject* pObj, UObjModel* pModel, u32 uFlags);
UObject* fn_80048808(UObjModel* pModel);
void fn_80048860(UObject* pObj);
void fn_80048804(UObject* pObj);
void fn_80048894(UObject* pObj);

void fn_80048F68(int nMsg, void* pArg, void* pArg2);     // sends a message to every object with an id

// GoDynObjBase.c
int  fn_80049820(int nMsg, DynObj* pObj, void* pArg, void* pArg2);  // type 0's handler, the others' default
DynObjHandler fn_800499B0(int nType);

// GoDynObjTypes.c: the handlers of types 6 and 9.
int  fn_8004AD54(int nMsg, DynObj* pObj, void* pArg, void* pArg2);
int  fn_8004AF2C(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// GoAnimalActors.c: type 11's handler (the animals).
int  fn_8004AAEC(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// LLObj_Gc.c: how a mesh is clipped against the camera (3: off screen, not drawn).
int  fn_80007B2C(UObjMesh* pMesh, void* pCamera, f32 a, f32 fSize, f32 c);

#endif
