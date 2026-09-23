// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "camera.h"

u8 fn_8003DC78(CamShot* pShot) {
    u8 nKind = pShot->bAC;

    if (nKind == 1 || (u8)(nKind - 2) <= 4U || nKind == 7) {
        return 1;
    }
    return 0;
}
