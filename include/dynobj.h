// dynobj.h (our name): the course's dynamic objects (GoDynObj.c) and their types. Each type has
// a message handler, found by the type number (DynObj_GetTypeHandler), that answers the messages
// sent to the type's objects (DynObjMessage).

#ifndef DYNOBJ_H
#define DYNOBJ_H

#include "engine.h"

// A type's message handler: nMsg, the object (NULL for DYNOBJ_MSG_SIZE) and two message arguments
// (UKernel.c passes 0 for the second when a message has one).
typedef int (*DynObjHandler)(int nMsg, struct DynObj* pObj, void* pArg, void* pArg2);

// The messages a type's handler takes (nMsg).
typedef enum DynObjMessage {
    DYNOBJ_MSG_SIZE = 1,        // answers the size of the type's object (Kernel_CreateObject)
    DYNOBJ_MSG_INIT = 2,        // sets the object up; pArg is its DynObjSetup (Kernel_CreateObject)
    DYNOBJ_MSG_DRAW = 3,        // draws it (DynObj_RenderDynamicObjects, through
                                // Kernel_BroadcastMessage)
    DYNOBJ_MSG_UPDATE = 6       // the per-frame update; pArg holds the frame time's bits
                                // (DynObj_UpdateDynamicObjects, through Kernel_BroadcastMessage)
} DynObjMessage;

// Object types (DynObjDef.nType).
typedef enum DynObjType {
    DYNOBJ_TYPE_PIN = 6,        // a pin position: Kernel_DownloadActors hands the actor to
                                // Ter_PinLoadCallback, which always answers 0, so no object of
                                // this type is made
    DYNOBJ_TYPE_SOUND = 9       // a sound: Kernel_DownloadActors hands the actor to
                                // Gaud_ActorDownloadCallback, so no object of this type is made
} DynObjType;

// An object's definition in the course data: its 'tACT' chunk from the chunk's id on
// (Kernel_DownloadActors; TChunkR).
typedef struct DynObjDef {
    s32  nActorId;              // 0x00  the chunk's id -> DynObj.nActorId
    u8   nType;                 // 0x04  its type (DynObj_GetTypeHandler) -> DynObj.nType
    u8   unk5[3];
    f32  aPos[3];               // 0x08  where it stands
    u32  uFlags;                // 0x14  -> DynObj.uFlags (Kernel_InitObjectFromDef)
    u8   nGroup;                // 0x18  -> DynObj.nGroup (Kernel_InitObjectFromDef)
    u8   unk19;
    s16  nUnread;               // 0x1A  -> DynObj.nUnread2
} DynObjDef;
LAYOUT_ASSERT(DynObjDef, 0x1C);     // GoDynObj.c builds one on the stack: 0x1C bytes there

// Type 2's definition: a DynObjDef and its turning speed.
typedef struct DynObjTurningDef {
    DynObjDef base;             // 0x00
    f32  fSpeed;                // 0x1C  degrees a second
} DynObjTurningDef;

// Type 11's definition (GoAnimalActors.c ActAnimal_Init): a DynObjDef and the animal's settings.
typedef struct DynObjAnimalDef {
    DynObjDef base;             // 0x00
    f32  f1C;                   // 0x1C  -> DynObjAnimal.f16C (and twice it f174)
    f32  a20[5];                // 0x20  -> DynObjAnimal.a178
    f32  f34;                   // 0x34  -> DynObjAnimal.f194
    f32  f38;                   // 0x38  -> DynObjAnimal.f198
    f32  aAngles[3];            // 0x3C  its rotation, in degrees (given to mat44flt_EulerAngles as 0x40,
                                //       0x3C, 0x44)
} DynObjAnimalDef;

// The stream object an object's model comes from (a view of TStreamData: its +4 is the model).
typedef struct DynObjModelRef {
    u8*  pData;                 // 0x00  TStreamData.pData; an animal's route follows a 12-byte
                                //       header in it (ActAnimal_Init)
    struct UObjModel* pModel;   // 0x04  TStreamData.uHandlerData: goes to Object_Init
} DynObjModelRef;

// One stream object an object's 'aRSL' chunk names, found by UKernel.c's Kernel_DownloadActors.
typedef struct DynObjModelEntry {
    u32  uType;                 // 0x0  the stream object's type
    union {
        s32  nId;               // 0x4  its id in the file (0: none) ...
        DynObjModelRef* pRef;   //      ... replaced by the object once it is found
    } u;
} DynObjModelEntry;

// An object's 'aRSL' chunk, laid over the chunk from its id on (TChunkR): the stream objects
// it uses, the first its model.
typedef struct DynObjModel {
    s32  nEntries;              // 0x0  set from the chunk's size
    DynObjModelEntry aEntries[1]; // 0x4  nEntries of them (as many as the chunk holds)
} DynObjModel;

// The byte-code scripts an object runs with RPN_RunScript (DynObjSetup.pScripts): a view of the
// actor's TStreamData, whose aScriptRefs Stream_Progress made pointers.
typedef struct DynObjScripts {
    u8   unk0[0x28];
    u8*  pHitScript;            // 0x28  -> DynObj.pHitScript
    u8*  pReleaseScript;        // 0x2C  -> DynObj.pReleaseScript
    u8*  pUnreadScript;         // 0x30  -> DynObj.pUnreadScript
} DynObjScripts;

// What a type's DYNOBJ_MSG_INIT gets (Kernel_CreateObject asks its handler for the object's size
// first).
typedef struct DynObjSetup {
    DynObjHandler pfnHandler;   // 0x00
    DynObjModel* pModel;        // 0x04
    DynObjDef* pDef;            // 0x08
    DynObjScripts* pScripts;    // 0x0C  NULL: no scripts
} DynObjSetup;

// The two values (a, b) of UKernel.c's Kernel_PostPairToObject, recorded on an object
// (DynObj.aPairs) or in gKernelOverflowSlots when the object's four are taken. Nothing reads them
// back but the test for a free pair.
typedef struct DynObjPair {
    u8   nFirst;                // 0x0  a; 0: the pair is free
    u8   nSecond;               // 0x1  b
} DynObjPair;

// The header of a UObjMesh's data (UObjMesh.pInfo).
typedef struct UObjMeshInfo {
    s16  n0;                    // 0x00  how many meshes UObjMesh.ppChildren holds (GoTerrain.c
                                //       Ter_GetMeshChildCount)
    u8   unk2[0x4 - 0x2];
    s32  n4;                    // 0x04  not 0: a chunk follows; fn_8000799C passes n4 * 2 to fn_80007658
    u8   unk8[0x24 - 0x8];
    s8   aFlags[0x54 - 0x24];   // 0x24  the mesh's flag bytes, read one at a time (Object_GetMeshFlags,
                                //       GoTerrain.c Ter_GetMeshFlags); length unknown, at most this.
                                //       Bytes 0 and 2 of an object's mesh: UOBJMESH_F0_*, UOBJMESH_F2_*
    f32  f54;                   // 0x54  scales a terrain object's mipmap bias (GoTerrain.c
                                //       Ter_GetMeshMipmapBiasScale)
    f32  v58[3];                // 0x58  copied to UObjModel.v2C by type 0's setup; with f64 the
                                //       bounding sphere GoTerrain.c's Ter_GetMeshBoundingSphere returns
    f32  f64;                   // 0x64  copied to UObjModel.fRadius by type 0's setup
    f32  a68[8];                // 0x68  a terrain object's bounds (GoTerrain.c Ter_GetMeshBounds,
                                //       Ter_AddPatchObjects): its centre [0..2], a radius [3] and a height
                                //       [7]
    s16  n88;                   // 0x88  the type of the mesh's parts (LLObj_Gc.c fn_80007658)
    u8   unk8A[0x8B - 0x8A];
    u8   b8B;                   // 0x8B  bit 1: a terrain object drawn without z writes (GoTerrain.c
                                //       Ter_GetMeshDrawFlags, Ter_SetObjectRenderState); passed on
                                //       in UObjPartDesc.u0A
    s16  n8C;                   // 0x8C  how many parts UObjMesh.pParts holds (fn_80007658)
    u16  u8E;                   // 0x8E  (fn_80007658)
} UObjMeshInfo;

// UObjMeshInfo.aFlags[0] of an object's mesh.
#define UOBJMESH_F0_ANIMATED   0x01 // it animates: a value goes to its shader each draw (a tree's
                                    // sway, a crowd's or an animal's pose blend)
#define UOBJMESH_F0_POSES      0x02 // with UOBJMESH_F0_ANIMATED, by poses: a child mesh is drawn in
                                    // its place (the crowd)
#define UOBJMESH_F0_NO_FADE    0x40 // a course object that stays solid near the camera instead of
                                    // fading (GoTerrain.c Ter_BuildObjectDrawLists)
// UObjMeshInfo.aFlags[2] of an object's mesh (a terrain patch's ground uses the byte otherwise).
#define UOBJMESH_F2_POSE_KIND0 0x01 // } with UOBJMESH_F0_ANIMATED, the other two kinds of poses
#define UOBJMESH_F2_POSE_KIND1 0x02 // }   (an animal's kinds 0 and 1): a child mesh is drawn too
#define UOBJMESH_F2_LIT        0x04 // lit by the light set (Object_Render)

// An entry of UObjMesh.pParts (0x2C bytes; what ObjPart_Draw takes).
typedef struct UObjMeshPart {
    s32  n0;                    // 0x00  its type (fn_8000827C); copied to
                                //       Ter_ObjectDrawData.eShaderObjectType (Ter_AddObjectDraw)
    s32  n4;                    // 0x04  copied to UObjMesh.n24 (LLObj_Gc.c fn_80007658)
    u32  u8;                    // 0x08  not 0: the part is used; copied to UObjMesh.n20 (fn_80007658)
    u8   unkC[0x24 - 0xC];
    struct UObjArraySet* pSet;  // 0x24  its model's array set (part i: set i; fn_80007658)
    UObjPartFuncs* pFuncs;      // 0x28  its type's functions (fn_8000827C)
} UObjMeshPart;

// What fn_80007658 hands a mesh's first part's type function (our name; 0x10 bytes on the stack).
typedef struct UObjPartDesc {
    s16  n0;                    // 0x00  how many parts (UObjMeshInfo.n8C)
    u8   unk2[0x4 - 0x2];
    s32  n4;                    // 0x04  fn_80007658's nCount
    u16  u8;                    // 0x08  UObjMeshInfo.u8E
    u16  u0A;                   // 0x0A  bit 0: UObjMeshInfo.b8B's bit 0
    u8*  pC;                    // 0x0C  the model data after the mesh's chunk header
} UObjPartDesc;

// One level of detail of a UObjModel. The terrain's hole data is a tree of them (GoTerrain.c,
// Ter_GetMeshNext..Ter_GetHoleModelRoot): the root holds a mesh per patch in its ppChildren[1], a
// patch's ppChildren[0] is its ground and its ppChildren[1] holds its object lists, whose meshes are
// the objects' (Ter_GetObjectListModel).
typedef struct UObjMesh {
    UObjMeshInfo* pInfo;        // 0x00
    u8   unk4[4];
    struct UObjMesh** ppChildren; // 0x08  its child meshes (pInfo->n0 of them): a terrain mesh's
                                //       children (Ter_GetMeshChild); an object's mesh draws child
                                //       TObject.nMeshChild instead of itself (Object_GetMeshChild)
    struct UObjMesh* pC;        // 0x0C  in a terrain patch's ground: the mesh drawn for it
                                //       (Ter_GetGroundDrawMesh)
    struct UObjMesh* p10;       // 0x10  its first child, the rest by p14 (pInfo->n0 of them; LLObj_Gc.c
                                //       fn_80007524)
    struct UObjMesh* p14;       // 0x14  the next terrain mesh of a list (Ter_GetMeshNext); a patch's
                                //       ground's is its objects (Ter_PatchReference.pObjects)
    struct UObjMeshPart* pParts; // 0x18  its parts (pInfo->n8C of them; LLObj_Gc.c fn_80007658)
    u8   abPartUsed[4];         // 0x1C  nonzero: part i is in use and can be drawn
    u32  n20;                   // 0x20  a word: GoTerrain.c Ter_DrawPatchGround draws a ground's extra meshes
                                //       only when it is not 0
    s32  n24;                   // 0x24  0 when built (LLObj_Gc.c fn_8000799C)
    s32  nDrawPart;             // 0x28  the part drawn (Object_DrawMesh); 0 when built (LLObj_Gc.c
                                //       fn_8000799C), and nothing changes it
} UObjMesh;

// What UObjModel.p10 points to (our view): a mesh tree at 0xEC (GoAnimalActors.c ActAnimal_GetRootMesh).
// One of its four array sets (0x38 bytes; LLObj_Gc.c): set 0 is the model's own, sets 1-3 copies of
// set 0's arrays that fn_80007824 makes.
typedef struct UObjArraySet {
    void* ap0[5];               // 0x00  arrays of an20 rows of 12 bytes (set 0's counts)
    u8   unk14[0x20 - 0x14];
    u16  an20[5];               // 0x20  how many rows each array holds
    u8   unk2A[0x30 - 0x2A];
    s32  n30;                   // 0x30  its mode (fn_800081C8); -1: the set is not made
    UObjSetFuncs* pFuncs;       // 0x34  its mode's functions (fn_800081C8)
} UObjArraySet;

// The 'ARRA' chunks fn_800073B4 finds before a model's 'HEAD' chunk (our name). Its size is from
// fn_800073B4's stack frame: at most 15 chunks fit.
typedef struct UObjArrayList {
    s32  n;                     // 0x00  how many
    u8*  ap[15];                // 0x04  each chunk's data
} UObjArrayList;

// A model's 'HEAD' chunk (our name; 0x70 bytes, the mesh chunks follow it).
typedef struct UObjModelHead {
    s16  nMeshes;               // 0x00  how many meshes the tree has (fn_800073B4 makes room for them)
    u8   unk2[0x70 - 0x2];
} UObjModelHead;

typedef struct UObjModelRoot {
    UObjArraySet aSets[4];      // 0x000
    void* pE0;                  // 0x0E0  the data it was made from (fn_800073B4)
    void* pE4;                  // 0x0E4  freed with it when not NULL (fn_800075CC)
    UObjModelHead* pE8;         // 0x0E8  its 'HEAD' chunk (fn_800073B4)
    UObjMesh* pMesh;            // 0x0EC  the mesh tree, right after the root (fn_800073B4)
} UObjModelRoot;
LAYOUT_ASSERT(UObjModelRoot, 0xF0); // fn_800073B4 puts the meshes at 0xF0

// A TObject's model (TObject.pModel).
typedef struct UObjModel {
    u8   unk0[0x10];
    UObjModelRoot* p10;         // 0x10  (GoAnimalActors.c ActAnimal_Init)
    UObjMesh* apLod[6];         // 0x14  its levels of detail (the first four all the same: it has
                                //       none); UObject3D.c fn_80045D80 fills the first three from
                                //       its mesh group and clears the last three
    f32  v2C[3];                // 0x2C
    u8   unk38[0x5C - 0x38];
    f32  fRadius;               // 0x5C  its bounding radius: type 0's setup copies its first level of
                                //       detail's (UObjMeshInfo.f64) here
    u8   unk60[0x7C - 0x60];
} UObjModel;
LAYOUT_ASSERT(UObjModel, 0x7C);     // UObject3D.c fn_80045D80 allocates 0x7C bytes

// UObject3D.c: a model made from its stream data, and the mesh-tree readers it uses.
UObjModel* fn_80045D80(u8* pData);
int        fn_80045F50(UObjMesh* pMesh);            // how many child meshes pMesh has
UObjMesh*  fn_80045F5C(UObjMesh* pMesh, int i);     // pMesh's child mesh i
UObjMesh*  fn_80045F6C(UObjModelRoot* pRoot);       // pRoot->pMesh
UObjModelRoot* fn_800073B4(u8* pData, int n);       // builds a model's mesh tree from stream data

// code_800080D0.c: the parts' type functions and the array sets' mode functions.
void fn_800081C8(UObjArraySet* pSet, int nMode, void* pArg);
void fn_80008214(UObjArraySet* pSet);
void fn_80008248(UObjMeshPart* pPart);
void fn_8000827C(UObjMeshPart* pPart, UObjArraySet* pSet, int nType, void* pArg);
void ObjPart_Draw(UObjMeshPart* pPart);

// UObject.c's object (0x118 bytes, Object_Create allocates one; a DynObj holds one at +0x10): a
// model and the matrices that place it. Object_UpdateO2WComposed33 makes the 3x3 part of its
// mObjToWorld mRotation x mScale; row 3 is the position.
typedef struct TObjectS {
    f32  mRotation[4][4];       // 0x000  identity at setup
    f32  mScale[4][4];          // 0x040  identity at setup; a scale on its diagonal
    f32  mObjToWorld[4][4];     // 0x080  identity at setup; what it is drawn with, [3] the position
                                //        (x, y, z, 1)
    u8   unkC0[0xE4 - 0xC0];
    f32  fRadius;               // 0x0E4  the model's fRadius at setup; nothing reads it
    u8   unkE8[0xF0 - 0xE8];
    UObjModel* pModel;          // 0x0F0  NULL: nothing to draw
    u8   unkF4[4];
    u32  uFlags;                // 0x0F8  TOBJECT_HAS_LODS; the other bits its setup is given are
                                //        never read
    f32  fLodScale;             // 0x0FC  0.5 / the model's fRadius, kept to 0.1..2.5 (0.1 for radius
                                //        0); nothing reads it
    f32  fUnread;               // 0x100  0 at setup; nothing reads it
    s8   nLod;                  // 0x104  the level of detail drawn: an index into pModel->apLod
                                //        (Object_SetLod sets it)
    u8   unk105;
    s16  nUnread;               // 0x106  0 at setup; nothing reads it
    s32  nMeshChild;            // 0x108  0 at setup; the child of its mesh drawn in the mesh's place
                                //        when the mesh's flags ask for one (Object_Render); an
                                //        animal puts its pose here before it is drawn
    f32  fShaderParam;          // 0x10C  0.5 at setup; handed to shader type 3 for a mesh with 0x1
                                //        in flag byte 0 (Object_Render); an animal sets it before it
                                //        is drawn (DynObjAnimal.f1B4)
    u8   unk110[0x118 - 0x110];
} TObject;
LAYOUT_ASSERT(TObject, 0x118);

// TObject.uFlags.
#define TOBJECT_UNREAD_1  0x01  // } set by a dynamic object's setup (DYNOBJ_TOBJECT_FLAGS) and on
#define TOBJECT_UNREAD_10 0x10  // }   GoDynObj.c's 'TEO ' 10000; nothing reads them
#define TOBJECT_HAS_LODS  0x04  // the model's four levels of detail are not all the same
#define TOBJECT_UNREAD_40 0x40  // set by a dynamic object's setup (DYNOBJ_HIDE_WITH_FLAG_OUT); nothing
                                // reads it

// A dynamic object (0x16C bytes for type 0; a type may add fields after it).
typedef struct DynObj {
    f32  aRot[4];               // 0x000  a rotation, set to (0, 0, 0, 1)
    TObject obj;                // 0x010  what is drawn (Object_Init sets it up)
    struct DynObj* pNext;       // 0x128  the next object in UKernel.c's list
    u8   unk12C[0x130 - 0x12C];
    DynObjHandler pfnHandler;   // 0x130  its type's message handler
    s32  n134;                  // 0x134  its id (Kernel_FindObjectById finds it by this); 0 once
                                //        Kernel_ReleaseObject ran, then Kernel_SweepDeadObjects's sweeps set
                                //        -1 and free it
    DynObjPair aPairs[4];       // 0x138  pairs Kernel_PostPairToObject records (nFirst == 0: free)
    u16  nActorId;              // 0x140  its actor chunk's id (DynObjDef.nActorId): the byte-code
                                //        addresses objects by it (Kernel_PostPairByActorId,
                                //        Kernel_QueryActorById), and a ball that hits it passes it
                                //        to the game mode (HitObject.nActorId)
    s16  nHitAmount;            // 0x142  0 at setup, and nothing changes it; read only by message
                                //        8, which nothing sends
    s16  nPoints;               // 0x144  0 at setup, and nothing changes it (messages 8 and 10,
                                //        which would, are never sent); message 9 answers it for
                                //        value 0
    u8   nType;                 // 0x146  its type (DynObjDef.nType); message 9 answers it for value 6
    u8   nGroup;                // 0x147  from its definition (Kernel_InitObjectFromDef; 0 for types
                                //        6 and 9): opcode 57 posts a pair to every object of a
                                //        group (Kernel_PostPairByGroup); message 9 answers it for
                                //        value 7
    u8   nKey;                  // 0x148  0 at setup, and nothing changes it: opcode 58 posts a pair
                                //        to every object with a key (Kernel_PostPairByKey); message
                                //        9 answers it for value 8
    u8   unk149[3];
    s16  nUnread1;              // 0x14C  0 at setup; nothing reads it
    s16  nUnread2;              // 0x14E  DynObjDef.nUnread (-1 for type 6); nothing reads it
    s32  nUnread3;              // 0x150  0 at setup; nothing reads it
    u32  uFlags;                // 0x154  DYNOBJ_*; from its definition (Kernel_InitObjectFromDef; 0
                                //        for types 6 and 9)
    f32  fValue;                // 0x158  0 at setup, and nothing changes it; message 9 answers it
                                //        for value 4
    u8*  pHitScript;            // 0x15C  } its byte-code scripts (RPN_RunScript), from its actor's
    u8*  pReleaseScript;        // 0x160  }   stream object: pReleaseScript runs when
    u8*  pUnreadScript;         // 0x164  }   Kernel_ReleaseObject gives it up. pHitScript is run
                                //        }   only by messages 8 and 10, which nothing sends;
                                //        }   nothing reads pUnreadScript
    s32  nUnread4;              // 0x168  0 at setup; nothing reads it
} DynObj;
LAYOUT_ASSERT(DynObj, 0x16C);

// DynObj.uFlags. The low bits come from the object's definition, the high bits are its state.
#define DYNOBJ_INACTIVE           0x00000004 // set up with DYNOBJ_REMOVED
#define DYNOBJ_TOBJECT_FLAGS      0x00000080 // its TObject gets TOBJECT_UNREAD_1 | TOBJECT_UNREAD_10
// When DynObjBase_MessageHandler draws the object, the next three change how it is drawn.
#define DYNOBJ_HIDE_WITH_FLAG_OUT 0x00000200 // not drawn while the flagstick is out; its TObject
                                             // also gets TOBJECT_UNREAD_40 at setup
#define DYNOBJ_NO_ZWRITE          0x00000400 // drawn without z-buffer writes; also sets
                                             // DYNOBJ_UNREAD_2000000 at setup
#define DYNOBJ_NO_ALPHA_TEST      0x00000800 // drawn without the alpha test
#define DYNOBJ_UNREAD_1000000     0x01000000 // cleared at setup; nothing else touches it
#define DYNOBJ_UNREAD_2000000     0x02000000 // set at setup with DYNOBJ_NO_ZWRITE; nothing reads it
#define DYNOBJ_REMOVED            0x04000000 // set at setup with DYNOBJ_INACTIVE (message 4, which
                                             // nothing sends, would set it too)

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
    f32  f16C;                  // 0x16C  } divided by the route's length (ActAnimal_ScaleSpeedsByRouteLength)
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
    u32  n1AC;                  // 0x1AC  } its pose, copied to its TObject's nMeshChild and fShaderParam
    u32  n1B0;                  // 0x1B0  }   before it is drawn; n1B0 the pose it is heading for
                                //            (ActAnimal_Update)
    f32  f1B4;                  // 0x1B4  }
    u32  n1B8;                  // 0x1B8  frames counted (ActAnimal_Update)
    u8   b1BC;                  // 0x1BC
    u8   b1BD;                  // 0x1BD  set when its route's first point with ground under it is
                                //        less than 0.5 above the ground (and f198 was given)
    u8   unk1BE[2];
} DynObjAnimal;
LAYOUT_ASSERT(DynObjAnimal, 0x1C0);

// A change of an animal's pose (our name; GoAnimalActors.c's gAnimalPoseSteps, 6 of them): in pose
// nFrom heading for nTo, f1B4 moves to fTarget, then the animal takes pose nNext at fStart.
typedef struct AnimalStep {
    u32  nFrom;                 // 0x00
    u32  nTo;                   // 0x04
    u32  nNext;                 // 0x08
    f32  fTarget;               // 0x0C
    f32  fStart;                // 0x10
} AnimalStep;
extern AnimalStep gAnimalPoseSteps[6];

// One entry of UKernel.c's gKernelOverflowSlots (12 bytes), taken by Kernel_PostPairToOverflowSlot.
typedef struct DynObjSlot {
    DynObjPair pair;            // 0x00
    s16  n2;                    // 0x02  600 when taken
    DynObj* pObj;               // 0x04
    s32  nId;                   // 0x08  the object's id (DynObj.n134)
} DynObjSlot;
LAYOUT_ASSERT(DynObjSlot, 0xC);

extern DynObjSlot gKernelOverflowSlots[32];

// GoDynObj.c's two records per player (our names; 0x104 and 0x100 bytes, GoDynObjMgr.aTee and
// aDivot): the tee the ball sits on, knocked flying by the shot, and the divot a shot throws up.
typedef struct GoDynObjTee {
    u8   b0;                    // 0x00  set by DynObj_TeeAdd; DynObj_UpdateTee moves pF4 while it is set
    u8   unk1[0xC - 0x1];
    f32  fC;                    // 0x0C  its heading once launched
    f32  f10;                   // 0x10  time in flight
    u8   unk14[0x20 - 0x14];
    f32  v20[4];                // 0x20  where DynObj_TeeAdd put pF4 (x, y, z, 1)
    f32  v30[4];                // 0x30  where it is now (Swing.c's STATEFUNC_PreShotUpdate fills it)
    f32  v40[4];                // 0x40  its launch velocity
    f32  v50[4];                // 0x50  its turn angles so far (STATEFUNC_PreShotUpdate fills them) ...
    f32  v60[4];                // 0x60  ... and how fast they grow
    u8   b70;                   // 0x70  set by DynObj_TeeStruck: launch it
    u8   unk71[0xB4 - 0x71];
    f32  mB4[4][4];             // 0xB4  its rotation
    DynObj* pF4;                // 0xF4  the tee, a 'TEO ' 10004 object (DynObj_TeeAdd)
    u8   bF8;                   // 0xF8  pF4 is in use
    u8   bF9;                   // 0xF9  set by DynObj_TeeAdd
    u8   unkFA[0x104 - 0xFA];
} GoDynObjTee;
LAYOUT_ASSERT(GoDynObjTee, 0x104);

typedef struct GoDynObjDivot {
    u8   b0;                    // 0x00  set by DynObj_DivotAdd; DynObj_UpdateDivot moves pF0 while it is set
    u8   unk1[0xC - 0x1];
    f32  fC;                    // 0x0C  minus the player's aim when placed
    f32  f10;                   // 0x10  time in flight
    u8   unk14[0x20 - 0x14];
    f32  v20[4];                // 0x20  where DynObj_DivotAdd put pF0 (x, y, z, 1)
    f32  v30[4];                // 0x30  where it is now
    f32  v40[4];                // 0x40  its launch velocity
    f32  v50[4];                // 0x50  its turn angles so far ...
    f32  v60[4];                // 0x60  ... and how fast they grow
    u8   unk70[0xB0 - 0x70];
    f32  mB0[4][4];             // 0xB0  its rotation
    DynObj* pF0;                // 0xF0  the divot, a 'TEO ' 10002 object (DynObj_DivotAdd), given up by
                                //       DynObj_DivotHide
    u8   bF4;                   // 0xF4  pF0 is in use
    u8   bF5;                   // 0xF5  set by DynObj_DivotAdd
    u8   unkF6[0x100 - 0xF6];
} GoDynObjDivot;
LAYOUT_ASSERT(GoDynObjDivot, 0x100);

// GoDynObj.c's state (0xAB0 bytes, gpDynObjState, allocated by DynObj_InitModule): per-player records
// and objects, and the models of the 'TEO ' stream objects (DynObj_InitForHole makes them).
typedef struct GoDynObjMgr {
    GoDynObjTee aTee[5];        // 0x000
    GoDynObjDivot aDivot[5];    // 0x514
    DynObj* apDivotHole[5];     // 0xA14  each player's divot hole, 'TEO ' 10001 (DynObj_ShotDivotHoleAdd;
                                //       DynObj_ShotDivotHoleHide gives it up)
    DynObj* apPitchMark[10];    // 0xA28  the pitch marks, 'TEO ' 10005 (DynObj_PitchMarkAdd), used
                                //       in turn ...
    s32  nNextPitchMark;        // 0xA50  ... the next one
    TObject* pTeo10000;         // 0xA54  'TEO ' 10000
    TObject* apTeo10030[4];     // 0xA58  'TEO ' 10030..10033
    TObject* apTeo10040[4];     // 0xA68  'TEO ' 10040..10043
    TObject* apTeo10020[3];     // 0xA78  'TEO ' 10020..10022, only with GM_Currently_SkillZoneMode
    TObject* apTeo10006[4];     // 0xA84  'TEO ' 10006..10009, only with GM_Currently_SkillZoneMode
    // the tee's and the divot's flight when struck (DynObj_InitModule sets them)
    f32  fTeeSpeed;             // 0xA94  7: the tee's top launch speed (times a random 0..1)
    f32  fDivotSpeed;           // 0xA98  10.5: the divot's (times a random 0.5..1)
    f32  fTeeTilt;              // 0xA9C  0.3: the tee's launch tilt (DynObj_LaunchDirection's fTilt)
    f32  fDivotTilt;            // 0xAA0  0.7: the divot's
    f32  fTeeSpin;              // 0xAA4  1.5: the tee's spin range (each turn rate random in +-half)
    f32  fDivotSpin;            // 0xAA8  0.6: the divot's
    f32  fTeeRaise;             // 0xAAC  -0.05169: how far above the spot DynObj_TeeAdd puts the tee
} GoDynObjMgr;
LAYOUT_ASSERT(GoDynObjMgr, 0xAB0);

extern GoDynObjMgr* gpDynObjState;
extern s32  gAltShotPartner[4];            // GoDynObj.c: each player's partner in game mode 21
extern char gGolfBallLogoTextureNames[27][13];       // GoDynObj.c: 27 names (DynObj_GetGolfBallLogoIndex
                                                     // finds one)
extern char gGolfBallLogoSlotNames[4][13];        // GoDynObj.c: each player's ball logo texture ("logoea",
                                                  // ...)
extern f32  gGolfBallScale;               // 1.0: scales the ball's radius (DynObj_GetBallPixels); owner
                                          // unknown

// UKernel.c's list of the objects, first and last, the last id given out (DynObj.n134), a bit
// mask of the used entries of gKernelOverflowSlots (Kernel_PostPairToOverflowSlot; 16 at most), and two
// node pools (400- and 528-byte nodes).
extern DynObj* gKernelFirstObject;
extern DynObj* gKernelLastObject;
extern s32 gKernelLastObjectId;
extern u32 gKernelOverflowSlotMask;
extern MemPoolPtr gKernelSmallObjectPool;
extern MemPoolPtr gKernelLargeObjectPool;

// LLObj_Gc.c
void fn_800075CC(UObjModelRoot* pRoot);    // frees a model fn_800073B4 made

// GoDynObj.c
void DynObj_PitchMarkAdd(f32* pPos, int nPlayer);  // a fast ball landing on SURFCLASS_GREEN_e
                                                   // leaves a pitch mark ('TEO ' 10005)
void DynObj_ShotDivotHoleAdd(f32* pPos, int nPlayer);  // put the player's divot hole ('TEO ' 10001) at pPos
void DynObj_DivotAdd(f32* pPos, int nPlayer);  // put the player's divot ('TEO ' 10002) at pPos
void DynObj_TeeStruck(int nPlayer);             // knock the player's tee ('TEO ' 10004) flying
int  DynObj_GetGolfBallLogoIndex(const char* szName);   // the index of the name among
                                                        // gGolfBallLogoTextureNames's 27 (-1: none)

// UKernel.c
DynObj* Kernel_GetFirstObject(void);                                 // the first object
DynObj* Kernel_FindObjectById(int nId);                               // the object with this id, or NULL
void Kernel_AppendObject(DynObj* pObj);                             // adds it at the end of the list
s32  Kernel_CreateObjectId(DynObjSetup* pSetup);                      // a new object's id, -2 when none
void Kernel_SweepDeadObjects(void);                                     // sweeps out the objects given up
void Kernel_ReleaseObject(DynObj* pObj);
void Kernel_InitObjectFromDef(DynObj* pObj, DynObjSetup* pSetup);    // type 0's DYNOBJ_MSG_INIT
void Kernel_PostPairByActorId(int nActorId, int a, int b);   // records (a, b) on every object with
                                                             // this nActorId
void Kernel_PostPairByGroup(int nGroup, int a, int b);  // ... with this nGroup
void Kernel_PostPairByKey(int nKey, int a, int b);      // ... with this nKey
int  Kernel_QueryActorById(int nKey, uptr nWhat);     // asks the first object whose nActorId is nKey
                                                      // (message 9)
void Kernel_BroadcastMessage(int nMsg, void* pArg, void* pArg2);     // sends nMsg to every object with id > 0

// UObject.c. A dynamic object passes its object part (&DynObj.obj).
void Object_Init(TObject* pObject, UObjModel* pModel, u32 Flags);
TObject* Object_Create(UObjModel* pModel);
void Object_Free(TObject* pObj);
void Object_Destroy(TObject* xpObject_p);
void Object_Draw(TObject* pObj);

// UNetwork.c
// The 3x3 parts of rows 0..2 of pObject's mObjToWorld become mRotation x mScale; its position row
// mObjToWorld[3] is left alone.
void Object_UpdateO2WComposed33(TObject* pObject);

// GoDynObjBase.c
// type 0's handler, types 2/11's default
int  DynObjBase_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);
DynObjHandler DynObj_GetTypeHandler(int nType);

// GoDynObjTypes.c: the handlers of DYNOBJ_TYPE_PIN and DYNOBJ_TYPE_SOUND.
int  DynObjPin_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);
int  DynObjSound_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// GoAnimalActors.c: type 11's handler (the animals).
int  ActAnimal_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// How a mesh, a terrain patch or an object is drawn against the view (LLObj_GetMeshClipMethod
// answers 0, 2 or 3). The drawing code turns GX clipping on only for DC_CLIP_PARTLY_IN_VIEW.
typedef enum DC_EClippingMethod {
    DC_CLIP_PARTLY_IN_VIEW = 0, // crosses the view's edge: drawn with GX clipping
    DC_CLIP_UNTESTED = 1,       // never culled, drawn without GX clipping: GoTerrain.c's patch in
                                // object-test mode
    DC_CLIP_IN_VIEW = 2,        // wholly in view: drawn without GX clipping
    DC_CLIP_OUT_OF_VIEW = 3     // not drawn
} DC_EClippingMethod;

// LLObj_Gc.c: how a mesh is clipped against the camera (DC_EClippingMethod).
int  LLObj_GetMeshClipMethod(UObjMesh* pMesh, void* pCamera, f32 fDist, f32 fHalfFovTan, f32 fScale);

void DynObj_InitForHole(void);
void DynObj_DeInitForHole(void);
void DynObj_RegisterStreamClients(void);
void DynObj_UnRegisterStreamClients(void);

#endif
