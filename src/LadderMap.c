// LadderMap.c (our name): the rules of the map screen of game mode 4's ladder (GameMode4.c), split
// from GameMode4Menu.c, which holds the screen's front-end messages. The map has a node per event
// (gLadderNodeEvents): six regions of three nodes (0 to 17) that give way, once won, to the
// region's final (18 to 23), and the World final (24) once every other event is won. It gives each
// node's region and state (shown, open, won, locked) and finds the node the cursor moves to.

#include "engine.h"
#include "frontend/fe.h"
#include "game/modes/ladder.h"

int LadderMap_IsNodeShown(int nNode);

char* gLadderRegionNames[7] = {
    "US Northwest", "US Southwest", "US East", "Europe", "Pacific", "Southern Hemisphere", "World",
};

s32 gLadderNodeEvents[NUM_LADDER_EVENTS] = {
    1, 0, 2, 5, 6, 4, 8, 10, 9, 13, 12, 14, 18, 16, 17, 20, 21, 22, 3, 7, 11, 15, 19, 23, 24,
};

char* gLadderHoleSetNames[4] = { "None", "All", "Front 9", "Back 9" };

char* gLadderStageNames[6] = { "1/4", "2/4", "3/4", "Dominated", "World", NULL };

// Nodes 18 to 23 hold the regions' fourth events (events 3, 7, 11, 15, 19 and 23, the "Dominated"
// stage), one a region; LadderMap_IsNodeShown shows one once its region's first three events are
// won.
u8 LadderMap_IsRegionFinalNode(int nNode) {
    int b = 0;

    if (nNode >= 18 && nNode < 24) {
        b = 1;
    }
    return b;
}

// Node 24 holds event 24, the ladder's final (the "World" stage), shown once every other event is
// won.
u8 LadderMap_IsWorldFinalNode(int nNode) {
    return nNode == 24;
}

// Region 6 ("World") holds only the final, node 24.
u8 LadderMap_IsWorldRegion(int nRegion) {
    return nRegion == 6;
}

// Whether the current profile has won the region: the events of its three nodes nRegion * 3 to
// nRegion * 3 + 2 (its fourth event does not count), or event 24 for the World region (6).
u8 LadderMap_HasWonRegion(int nRegion) {
    int nProfile = fn_80077B08();

    if (LadderMap_IsWorldRegion(nRegion)) {
        return GameMode4_HasWonEvent(nProfile, 24);
    }
    return GameMode4_HasWonEvent(nProfile, gLadderNodeEvents[nRegion * 3]) &&
           GameMode4_HasWonEvent(nProfile, gLadderNodeEvents[nRegion * 3 + 1]) &&
           GameMode4_HasWonEvent(nProfile, gLadderNodeEvents[nRegion * 3 + 2]);
}

// Whether the current profile has won events 0 to 23, every event but the final (24); the final's
// node is shown once it has.
u8 LadderMap_HasWonAllButFinal(void) {
    int nProfile;
    int i;
    u8 bWon = 1;

    nProfile = fn_80077B08();
    for (i = 0; i < 24; i++) {
        if (!GameMode4_HasWonEvent(nProfile, i)) {
            bWon = 0;
        }
    }
    return bWon;
}

// A node's state on the map: -1 not shown (LadderMap_IsNodeShown), 1 its event won, 0 open (every
// event it needs is won), 2 locked, for the current profile. The final, the region finals and the
// other nodes have a branch each, but the three do the same.
int LadderMap_GetNodeState(int nNode) {
    int nProfile;
    int nEvent = gLadderNodeEvents[nNode];

    nProfile = fn_80077B08();
    if (!LadderMap_IsNodeShown(nNode)) return -1;
    if (LadderMap_IsWorldFinalNode(nNode)) {
        if (GameMode4_HasWonEvent(nProfile, nEvent)) return 1;
        return GameMode4_IsEventOpen(nProfile, nEvent) ? 0 : 2;
    }
    if (LadderMap_IsRegionFinalNode(nNode)) {
        if (GameMode4_HasWonEvent(nProfile, nEvent)) return 1;
        return GameMode4_IsEventOpen(nProfile, nEvent) ? 0 : 2;
    }
    if (GameMode4_HasWonEvent(nProfile, nEvent)) return 1;
    return GameMode4_IsEventOpen(nProfile, nEvent) ? 0 : 2;
}

// Whether a node is on the map: the final (24) once events 0 to 23 are won; a region final (18 to
// 23) once its region is won (LadderMap_HasWonRegion); a region's first three nodes only until
// then, when its final takes their place.
int LadderMap_IsNodeShown(int nNode) {
    u8 bRegionWon = LadderMap_HasWonRegion(LadderMap_GetNodeRegion(nNode));

    if (LadderMap_IsWorldFinalNode(nNode)) {
        return LadderMap_HasWonAllButFinal();
    }
    if (LadderMap_IsRegionFinalNode(nNode)) {
        return bRegionWon;
    }
    return !bRegionWon;
}

// A node's region (gLadderRegionNames): nodes 0 to 17 three a region, 18 to 23 one each (the region
// finals), 24 the World (6); 0 for any node above 24.
int LadderMap_GetNodeRegion(int nNode) {
    if (nNode < 18) {
        return nNode / 3;
    }
    return nNode <= 24 ? nNode - 18 : 0;
}

// An event's stage within its region, the index of its gLadderStageNames text: nEvent % 4 below 24
// (0 "1/4", 1 "2/4", 2 "3/4", 3 "Dominated"), 4 ("World") for event 24, 0 above. It takes an event,
// not a node.
int LadderMap_GetEventStage(int nEvent) {
    if (nEvent < 24) {
        return nEvent % 4;
    }
    return nEvent == 24 ? 4 : 0;
}

// Sets abCandidate[i] for each node that lies in direction nDir from the cursor's node (0 up, 1
// down, 2 left, 3 right; y grows downwards), within 45 degrees either side, and clears it for the
// others. The cursor's own entry is left as it was, and so is every entry for any other nDir.
void LadderMap_MarkNodesInDirection(int nDir, u8* abCandidate) {
    f32 fDX;
    f32 fDY;
    f32 fX = gLadderMap.aNode[gLadderMap.nNode].fX;
    f32 fY = gLadderMap.aNode[gLadderMap.nNode].fY;
    int i;
    u8 b;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (i != gLadderMap.nNode) {
            fDX = gLadderMap.aNode[i].fX - fX;
            fDY = gLadderMap.aNode[i].fY - fY;
            switch (nDir) {
            case 0:
                b = 0;
                if (fDY < 0.0f && -fDY > fabsf(fDX)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 1:
                b = 0;
                if (fDY > 0.0f && fDY > fabsf(fDX)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 2:
                b = 0;
                if (fDX < 0.0f && -fDX > fabsf(fDY)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            case 3:
                b = 0;
                if (fDX > 0.0f && fDX > fabsf(fDY)) {
                    b = 1;
                }
                abCandidate[i] = b;
                break;
            }
        }
    }
}

// The node marked in abCandidate that lies nearest the cursor's node in a straight line, -1 when
// none is marked.
int LadderMap_FindNearestMarkedNode(u8* abCandidate) {
    f32 fX = gLadderMap.aNode[gLadderMap.nNode].fX;
    f32 fY = gLadderMap.aNode[gLadderMap.nNode].fY;
    f32 fBest = 3.4028235e38f;
    f32 fDX;
    f32 fDY;
    f32 fDist;
    int i;
    int nBest = -1;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (abCandidate[i]) {
            fDX = fX - gLadderMap.aNode[i].fX;
            fDY = fY - gLadderMap.aNode[i].fY;
            fDX *= fDX;
            fDY *= fDY;
            fDist = Math_Sqrt(fDX + fDY);
            if (fDist < fBest) {
                nBest = i;
                fBest = fDist;
            }
        }
    }
    return nBest;
}

// Whether the current profile may play the event: not won yet, and every event it needs is won.
u8 LadderMap_IsEventPlayable(int nEvent) {
    int b;
    int nProfile = fn_80077B08();

    b = 0;
    if (!GameMode4_HasWonEvent(nProfile, nEvent) && GameMode4_IsEventOpen(nProfile, nEvent)) {
        b = 1;
    }
    return b;
}

// The node that holds the event (its index in gLadderNodeEvents), -1 for none.
int LadderMap_GetEventNode(int nEvent) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (nEvent == gLadderNodeEvents[i]) return i;
    }
    return -1;
}

// The node of the lowest-numbered event the current profile may play (LadderMap_IsEventPlayable),
// else the final's node (event 24).
int LadderMap_GetFirstPlayableNode(void) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (LadderMap_IsEventPlayable(i)) {
            return LadderMap_GetEventNode(i);
        }
    }
    return LadderMap_GetEventNode(24);
}

// Clears abCandidate for every node that is not on the map (LadderMap_IsNodeShown), so the cursor
// never lands on one.
void LadderMap_UnmarkHiddenNodes(u8* abCandidate) {
    int i;

    for (i = 0; i < NUM_LADDER_EVENTS; i++) {
        if (!LadderMap_IsNodeShown(i)) {
            abCandidate[i] = 0;
        }
    }
}
