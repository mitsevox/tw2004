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

// The course loader for chunk 5 of a hole (registered by SitDev_vInitModule): adds the chunk, one
// commentary zone (its outline network, then its bits), to the hole's zones. The ten slots of
// lbl_801FA1C0 are not checked.
void SitDev_NetworkLoadCallback(u8* pChunk) {
    lbl_801FA1C0[lbl_80282210] = (SitDevZone*)pChunk;
    lbl_80282210++;
}

// State value 79 (for the look-ahead ball's position): the bits of every commentary zone the point
// is inside (fn_8000C140, the outline test) OR'd together; a zone's bits are the u32 right after
// its outline's last node. 0 when the hole has no zones.
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
