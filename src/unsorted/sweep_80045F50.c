// Small functions found by the sweep (sweep.py). Original file unknown; UObject3D.c's fn_80045D80
// calls them.

#include "dynobj.h"

int fn_80045F50(UObjMesh* pMesh) {
    return pMesh->pInfo->n0;
}

UObjMesh* fn_80045F5C(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}
