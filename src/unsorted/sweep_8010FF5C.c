// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "frontend/fe.h"

u8 lbl_80212B60[64 * 64];

u8* fn_8010FF5C(u8* pLogo, int nWidth, int nHeight) {
    fn_8010FC3C(lbl_80212B60, pLogo, 1, nWidth, nHeight);
    return lbl_80212B60;
}
