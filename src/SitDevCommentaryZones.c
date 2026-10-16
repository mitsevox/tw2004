// SitDevCommentaryZones.c: the hole's commentary zones, outlines that arrive as course networks of
// type NETWORK_TYPE_COMMENTARY_ZONE, and the test of which of them a point is in, the value the
// commentary scripts test as SITDEV_VALUE_COMMENTARY_ZONES.

#include "sitdev.h"

TNetwork* gSitDevCommentaryZones[SITDEV_NUM_COMMENTARY_ZONES];  // the hole's zones, as they arrive
s32 gSitDevNumCommentaryZones;                                   // how many; zeroed before each hole loads

// The loader SitDev_vInitModule registers for commentary-zone networks: adds the network, one
// zone's outline followed by its TCommentaryNetworkParam, to the hole's zones.
void SitDev_NetworkLoadCallback(u8* netPtr) {
    gSitDevCommentaryZones[gSitDevNumCommentaryZones] = (TNetwork*)netPtr;
    gSitDevNumCommentaryZones++;
}

// The zone bits of every commentary zone whose outline the point is inside, OR'd together; 0 when
// the hole has no zones.
u32 SitDev_GetCommentaryZones(f32* location) {
    int i;
    u32 uiReturnValue = 0;
    TCommentaryNetworkParam* pNetworkParams;

    if (gSitDevNumCommentaryZones == 0) return 0;
    for (i = 0; i < gSitDevNumCommentaryZones; i++) {
        if (wn_PnPoly(location, gSitDevCommentaryZones[i], gSitDevCommentaryZones[i]->nNumNodes)) {
            pNetworkParams = (TCommentaryNetworkParam*)&gSitDevCommentaryZones[i]->aNodes[
                gSitDevCommentaryZones[i]->nNumNodes];
            uiReturnValue |= pNetworkParams->uZoneBits;
        }
    }
    return uiReturnValue;
}
