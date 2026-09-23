// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
extern s32 lbl_802821F8;
extern s32 lbl_802821FC;

u8* fn_800BB1A8(s32 p0);
u8* fn_800BB1A8(s32 p0) {
    lbl_802821FC = p0;
    lbl_802821F8 = *(s32*)(gSession + 0x24);
    return gSession;
}
