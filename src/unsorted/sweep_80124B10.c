// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "engine.h"
#include "core/easb.h"

void fn_80124B10(UStreamObject* pObject) {
    Mem_cpy(lbl_80281988->pIcon, pObject->pData, pObject->uSize);
    fn_80009E70(pObject);
}
