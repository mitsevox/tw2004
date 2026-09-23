// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "camera.h"

void fn_80045470(CamLens* pLens, f32 fFov) {
    pLens->fFov = fFov;
    fn_800763BC(pLens);
}
