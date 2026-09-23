// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801E3180[];
extern s32 lbl_80281FC8;
void CARDRead();

s32 fn_8009F208(s32 p0, s32 p1, s32 p2);
s32 fn_8009F208(s32 p0, s32 p1, s32 p2) {
    CARDRead((lbl_801E3180 + (p0 * 20)), p1, p2, lbl_80281FC8, (p0 * 20));
    lbl_80281FC8 = (lbl_80281FC8 + p2);
    return 0;
}
