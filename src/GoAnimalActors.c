// GoAnimalActors.c (TW06's golf/hi-rendering/goanimalactors.c; our spelling): the animals on the
// course (ActAnimal_*). Not yet decompiled; the unit covers the file's certain core, whose
// functions share its constant block (0x80283218-0x80283280).

#include "dynobj.h"

void fn_8004A24C(DynObjAnimal* pAnimal, DynObjSetup* pSetup);  // message 2: sets it up
void fn_8004A578(DynObjAnimal* pAnimal, void* pArg);            // message 6: pArg holds the frame
                                                                // time's bits

// Type 11's message handler; other messages go to type 0's.
int fn_8004AAEC(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObjAnimal);
    case 2:
        fn_8004A24C((DynObjAnimal*)pObj, pArg);
        return 0;
    case 6:
        fn_8004A578((DynObjAnimal*)pObj, pArg);
        return 0;
    case 3:
        if (pObj->obj.pModel != NULL) {
            pObj->obj.n108 = ((DynObjAnimal*)pObj)->n1AC;
            pObj->obj.f10C = ((DynObjAnimal*)pObj)->f1B4;
            fn_80048894(&pObj->obj);
        }
        return 0;
    default:
        return fn_80049820(nMsg, pObj, pArg, pArg2);
    }
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_8004ABA4(u8* p0, s32 p1);
s32 fn_8004ABB4(u8* p);

s32 fn_8004ABA4(u8* p0, s32 p1) {
    return *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + (p1 << 2));
}

s32 fn_8004ABB4(u8* p) {
    return *(s32*)(p + 0xEC);
}

// ---- end of sweep code ----
