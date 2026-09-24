// SitDevCommentaryZones.c (EA file, TW06/TW07): own unit, its .sbss starts 8-aligned at 0x80282210

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

SitDevZone* lbl_801FA1C0[10];
s32 lbl_80282210;

// Course loader for chunk 5 of a hole: one more situation zone.
void fn_800BB6DC(u8* pChunk) {
    lbl_801FA1C0[lbl_80282210] = (SitDevZone*)pChunk;
    lbl_80282210++;
}

// The bits of every zone the point is in (0 when the hole has none).
u32 SitDev_GetCommentaryZones(f32* pPos) {
    int i;
    u32 uBits = 0;
    if (lbl_80282210 == 0) return 0;
    for (i = 0; i < lbl_80282210; i++) {
        if (fn_8000C140(pPos, &lbl_801FA1C0[i]->net, lbl_801FA1C0[i]->net.nNumNodes)) {
            uBits |= *(u32*)&lbl_801FA1C0[i]->net.aNodes[lbl_801FA1C0[i]->net.nNumNodes];
        }
    }
    return uBits;
}
