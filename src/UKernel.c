// UKernel.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the kernel's list
// of the course's dynamic objects (dynobj.h), chained through DynObj.pNext from lbl_80281DBC to
// lbl_80281DB8. Only part is decompiled so far.

#include "dynobj.h"

DynObj* fn_80049018(void* pType);  // a new object; pType's first word is its handler (asked for message 1)
int  fn_8000EA1C(const char* pName, int a, int b, void* pObj);

DynObj* fn_80048E44(void) {
    return lbl_80281DBC;
}

DynObj* fn_80048E4C(int nId) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 == nId) {
            return pObj;
        }
    }
    return NULL;
}

void fn_80048FEC(DynObj* pObj) {
    if (lbl_80281DB8 != NULL) {
        lbl_80281DB8->pNext = pObj;
        lbl_80281DB8 = pObj;
    } else {
        lbl_80281DBC = pObj;
        lbl_80281DB8 = pObj;
    }
    pObj->pNext = NULL;
}

// The id of a new object of the type (fn_80049018), or -2 when none could be made.
s32 fn_800490B8(void* pType) {
    DynObj* pObj = fn_80049018(pType);

    if (pObj != NULL) {
        return pObj->n134;
    }
    return -2;
}

// Takes the object's id away (n134 = 0), first passing its p160 name to fn_8000EA1C unless flag
// 0x20000000 is set.
void fn_800491C4(DynObj* pObj) {
    if (pObj->n134 != 0) {
        if (!(pObj->uFlags & 0x20000000) && pObj->p160 != NULL) {
            fn_8000EA1C(pObj->p160, !(pObj->uFlags & 0x10000000), -1, pObj);
        }
        pObj->n134 = 0;
    }
}

void fn_8004950C(void) {
}

void fn_80049510(void) {
}
