// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "camera.h"

u8 fn_8004562C(CamShot* pShot) {
    u8 nKind = pShot->bAC;

    if (nKind == 0 || (u8)(nKind - 13) <= 2U || nKind == 23) {
        return 1;
    }
    return 0;
}
