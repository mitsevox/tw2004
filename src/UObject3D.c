// UObject3D.c (EA's name, from its asserts; also in EA's 2002 source tree): a model made from a
// stream object's data.

#include "dynobj.h"

// Builds a model from pData: its mesh tree, and up to three levels of detail from the tree's
// first mesh group.
UObjModel* fn_80045D80(u8* pData) {
    UObjMesh* pGroup;
    UObjModel* pModel;
    int nLods;
    int i;

    pModel = fn_80009B34(sizeof(UObjModel), 2, 0x80, "UObject3D.c", 506);
    pModel->p10 = fn_800073B4(pData, 0);
    pGroup = fn_80045F5C(fn_80045F6C(pModel->p10), 0);
    nLods = fn_80045F50(pGroup);
    for (i = 0; i < 3; i++) {
        if (nLods > i) {
            pModel->apLod[i] = fn_80045F5C(pGroup, i);
        } else {
            pModel->apLod[i] = NULL;
        }
        pModel->apLod[i + 3] = NULL;
    }
    return pModel;
}
