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

// An object's definition in the course data.
typedef struct DynObjDef {
    s32  n0;                    // 0x00  -> DynObj.n140
    u8   n4;                    // 0x04  -> DynObj.n146
    u8   unk5[3];
    f32  aPos[3];               // 0x08  where it stands
    u32  u14;                   // 0x14  -> DynObj.uFlags (type 0)
    u8   n18;                   // 0x18  -> DynObj.n147 (type 0)
    u8   unk19;
    s16  n1A;                   // 0x1A  -> DynObj.n14E
    f32  f1C;                   // 0x1C  type 2: its turning speed, degrees a second
} DynObjDef;

typedef struct DynObjModelRef {
    u8   unk0[4];
    struct UObjModel* p4;       // 0x04  goes to fn_800486F4
} DynObjModelRef;

typedef struct DynObjModel {
    u8   unk0[8];
    DynObjModelRef* p8;         // 0x08  the model, if any
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
    u8   unk0[0x24];
    s8   a24[0x58 - 0x24];      // 0x24  fn_80048AD4 (length unknown, at most this)
    f32  v58[3];                // 0x58  copied to UObjModel.v2C by type 0's setup
    f32  f64;                   // 0x64  copied to UObjModel.f5C by type 0's setup
} UObjMeshInfo;

// An entry of UObjMesh.p18 (0x2C bytes; what fn_800082CC takes).
typedef struct UObjMeshPart {
    u8   unk0[0x2C];
} UObjMeshPart;

// One level of detail of a UObjModel.
typedef struct UObjMesh {
    UObjMeshInfo* pInfo;        // 0x00
    u8   unk4[4];
    s32* p8;                    // 0x08  fn_80048AC4 reads it by index
    u8   unkC[0x18 - 0xC];
    struct UObjMeshPart* p18;   // 0x18  fn_80048A84 passes entry n28 to fn_800082CC
    u8   a1C[0x28 - 0x1C];      // 0x1C  nonzero: entry i of p18 is used
    s32  n28;                   // 0x28
} UObjMesh;

// A UObject's model (UObject.pModel).
typedef struct UObjModel {
    u8   unk0[0x14];
    UObjMesh* apLod[4];         // 0x14  its levels of detail (all four the same: it has none)
    u8   unk24[0x2C - 0x24];
    f32  v2C[3];                // 0x2C
    u8   unk38[0x5C - 0x38];
    f32  f5C;                   // 0x5C  a size: the level-of-detail scale is 0.5 / f5C
} UObjModel;

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

// Type 11 (GoAnimalActors.c): an animal.
typedef struct DynObjAnimal {
    DynObj base;                // 0x000
    u8   unk16C[0x1AC - 0x16C];
    s32  n1AC;                  // 0x1AC  } copied to its UObject's n108 and f10C before it is drawn
    u8   unk1B0[4];             //        }
    f32  f1B4;                  // 0x1B4  }
    u8   unk1B8[0x1C0 - 0x1B8];
} DynObjAnimal;
LAYOUT_ASSERT(DynObjAnimal, 0x1C0);

// One entry of UKernel.c's lbl_801D5228 (12 bytes), taken by fn_80049230.
typedef struct DynObjSlot {
    DynObjPair pair;            // 0x00
    s16  n2;                    // 0x02  600 when taken
    DynObj* pObj;               // 0x04
    s32  nId;                   // 0x08  the object's id (DynObj.n134)
} DynObjSlot;
LAYOUT_ASSERT(DynObjSlot, 0xC);

extern DynObjSlot lbl_801D5228[32];

// UKernel.c's list of the objects, first and last, the last id given out (DynObj.n134), a bit
// mask of the used entries of lbl_801D5228 (fn_80049230; 16 at most), and two node pools (400-
// and 528-byte nodes).
extern DynObj* lbl_80281DBC;
extern DynObj* lbl_80281DB8;
extern s32 lbl_80281DB4;
extern u32 lbl_80281DB0;
extern UMemPool* lbl_80281DAC;
extern UMemPool* lbl_80281DA8;

// UKernel.c, UObject.c. The UObject functions take the object part (&DynObj.obj).
DynObj* fn_80048E44(void);                                  // the first object
DynObj* fn_80048E4C(int nId);                               // the object with this id, or NULL
void fn_80048FEC(DynObj* pObj);                             // adds it at the end of the list
void fn_800491C4(DynObj* pObj);
void fn_80049514(DynObj* pObj, DynObjSetup* pSetup);    // type 0's message 2
void fn_800486F4(UObject* pObj, UObjModel* pModel, u32 uFlags);
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

#endif
