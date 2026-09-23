// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "charstate.h"
#include "frontend/fe.h"

// Save the created golfer's body skin entries in the profile.
void fn_80103D6C(void) {
    SaveProfile* pProfile = fn_80077ACC();
    Skin* pSkin = lbl_80281EE0->pB4->pChar->pSkin;

    Mem_cpy(pProfile->a5614, pSkin->p10B4, fn_800CCA40(pSkin) * 8);
    Mem_cpy(pProfile->a5754, pSkin->p10C4, fn_800CCEA0(pSkin) * 8);
}
