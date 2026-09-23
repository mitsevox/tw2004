// dynobj.h (our name): the course's dynamic objects (GoDynObj.c) and their types. Each type has
// a message handler, found by the type number (fn_800499B0): message 1 asks the size of the
// type's object, 2 sets one up from its definition, 3 runs fn_80048894 on its object part (when
// it has a model), 6 is the per-frame update. Only what the code reads so far.

#ifndef DYNOBJ_H
#define DYNOBJ_H

#include "engine.h"

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
    void* p4;                   // 0x04  goes to fn_800486F4
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

// What a type's message 2 gets.
typedef struct DynObjSetup {
    u8   unk0[4];
    DynObjModel* pModel;        // 0x04
    DynObjDef* pDef;            // 0x08
    DynObjNames* pC;            // 0x0C
} DynObjSetup;

// A dynamic object (0x16C bytes for type 0; a type may add fields after it).
typedef struct DynObj {
    f32  aRot[4];               // 0x000  a rotation, set to (0, 0, 0, 1)
    f32  mObj[4][4];            // 0x010  the object part starts here (fn_800486F4 sets it up)
    f32  m50[4][4];             // 0x050
    u8   unk90[0xC0 - 0x90];
    f32  aPos[4];               // 0x0C0  (x, y, z, 1)
    u8   unkD0[0x100 - 0xD0];
    void* p100;                 // 0x100  the model; message 3 does nothing without one
    u8   unk104[0x118 - 0x104];
    s32  n118;                  // 0x118  type 11 (the animals)
    f32  f11C;                  // 0x11C  type 11
    u8   unk120[0x128 - 0x120];
    struct DynObj* pNext;       // 0x128  the next object in UKernel.c's list
    u8   unk12C[0x134 - 0x12C];
    s32  n134;                  // 0x134  its id (fn_80048E4C finds it by this; 0 once fn_800491C4 ran)
    u8   b138;                  // 0x138
    u8   unk139;
    u8   b13A;                  // 0x13A
    u8   unk13B;
    u8   b13C;                  // 0x13C
    u8   unk13D;
    u8   b13E;                  // 0x13E
    u8   unk13F;
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
    DynObj obj;                 // 0x000
    f32  fSpeed;                // 0x16C  degrees a second
    u8   unk170[0x194 - 0x170];
} DynObjTurning;
LAYOUT_ASSERT(DynObjTurning, 0x194);

typedef int (*DynObjHandler)(int nMsg, DynObj* pObj, void* pArg);

// UKernel.c's list of the objects, first and last, the last id given out (DynObj.n134), a bit
// mask of the used entries of lbl_801D5228 (fn_80049230), and two node pools (400- and 528-byte
// nodes).
extern DynObj* lbl_80281DBC;
extern DynObj* lbl_80281DB8;
extern s32 lbl_80281DB4;
extern u32 lbl_80281DB0;
extern UMemPool* lbl_80281DAC;
extern UMemPool* lbl_80281DA8;

// GoDynObj.c
int  fn_800484F4(char* szName);         // the index of the name among lbl_80187B98's 27 (-1: none)

// UKernel.c, UObject.c. The UObject functions take the object part (&DynObj.mObj).
DynObj* fn_80048E44(void);                                  // the first object
DynObj* fn_80048E4C(int nId);                               // the object with this id, or NULL
void fn_80048FEC(DynObj* pObj);                             // adds it at the end of the list
void fn_800491C4(DynObj* pObj);
void fn_80049514(DynObj* pObj, DynObjSetup* pSetup);    // type 0's message 2
void fn_800486F4(void* pObj, void* pModel, int nFlags);
void fn_80048804(void* pObj);
void fn_80048894(void* pObj);

// GoDynObjBase.c
int  fn_80049820(int nMsg, DynObj* pObj, void* pArg);  // type 0's handler, the others' default
DynObjHandler fn_800499B0(int nType);

// GoDynObjTypes.c: the handlers of types 6 and 9.
int  fn_8004AD54(int nMsg, DynObj* pObj, void* pArg);
int  fn_8004AF2C(int nMsg, DynObj* pObj, void* pArg);

#endif
