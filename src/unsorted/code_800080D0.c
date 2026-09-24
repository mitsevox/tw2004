// Vertex format setup and per-type function-table dispatch. Original file name unknown.
// Sits between the visibility code (code_80007BC4.c) and the accessors (code_800082F8.c).

#include "dynobj.h"

typedef struct {
    /* 0x00 */ char pad0[0x10];
    /* 0x10 */ int unk10;
    /* 0x14 */ int unk14;
} UnkGlobal;

UnkGlobal lbl_801A24A0;
UnkGlobal* lbl_80280DA0 = &lbl_801A24A0;

void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, unsigned char frac);

void fn_800080D0(void) {
}

void fn_800080D4(void) {
    lbl_80280DA0->unk14 = 0;
    lbl_80280DA0->unk10 = 0;
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 3, 9);
    GXSetVtxAttrFmt(0, 14, 1, 3, 9);
    GXSetVtxAttrFmt(0, 10, 0, 4, 0);
    GXSetVtxAttrFmt(4, 9, 1, 4, 0);
    GXSetVtxAttrFmt(4, 11, 1, 5, 0);
    GXSetVtxAttrFmt(4, 13, 1, 4, 0);
}

void fn_800081C4(void) {
}

// Gives an array set its mode and runs the mode's setup function on it.
void fn_800081C8(UObjArraySet* pSet, int nMode, void* pArg) {
    pSet->n30 = nMode;
    pSet->pFuncs = &lbl_80188E78[nMode].set;
    pSet->pFuncs->p0(pSet, pArg);
}

void fn_80008214(UObjArraySet* pSet) {
    if (pSet->pFuncs->p4 != NULL) {
        pSet->pFuncs->p4(pSet);
    }
}

void fn_80008248(UObjMeshPart* pPart) {
    if (pPart->pFuncs->p4 != NULL) {
        pPart->pFuncs->p4(pPart);
    }
}

// Gives a mesh part its type and array set and runs the type's setup function on it.
void fn_8000827C(UObjMeshPart* pPart, UObjArraySet* pSet, int nType, void* pArg) {
    pPart->n0 = nType;
    pPart->pSet = pSet;
    pPart->pFuncs = &lbl_80188E88[nType].part;
    pPart->pFuncs->p0(pPart, pArg);
}

void fn_800082CC(UObjMeshPart* pPart) {
    pPart->pFuncs->p8(pPart);
}
