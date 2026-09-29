// SitDevCommentaryZones.c (EA's name: TW06's and TW07's SitDevCommentaryZones.c; these two are
// TW07's first and last functions there): a hole's commentary zones, outlines that course chunk
// 5 brings, and the test of which of them a point is in (state value 79). Own unit; its .sbss
// starts 8-aligned at 0x80282210.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

SitDevZone* gSitDevCommentaryZones[10];    // the hole's zones, as loaded
s32 gSitDevNumCommentaryZones;             // how many; zeroed before each hole loads

// The course loader for chunk 5 of a hole (registered by SitDev_vInitModule): adds the chunk, one
// commentary zone (its outline network, then its bits), to the hole's zones. The ten slots of
// gSitDevCommentaryZones are not checked.
void SitDev_NetworkLoadCallback(u8* pChunk) {
    gSitDevCommentaryZones[gSitDevNumCommentaryZones] = (SitDevZone*)pChunk;
    gSitDevNumCommentaryZones++;
}

// State value 79 (for the look-ahead ball's position): the bits of every commentary zone the point
// is inside (wn_PnPoly, the outline test) OR'd together; a zone's bits are the u32 right after
// its outline's last node. 0 when the hole has no zones.
u32 SitDev_GetCommentaryZones(f32* pPos) {
    int i;
    u32 uBits = 0;
    if (gSitDevNumCommentaryZones == 0) return 0;
    for (i = 0; i < gSitDevNumCommentaryZones; i++) {
        if (wn_PnPoly(pPos, &gSitDevCommentaryZones[i]->net, gSitDevCommentaryZones[i]->net.nNumNodes)) {
            uBits |= *(u32*)&gSitDevCommentaryZones[i]->net.aNodes[gSitDevCommentaryZones[i]->net.nNumNodes];
        }
    }
    return uBits;
}
